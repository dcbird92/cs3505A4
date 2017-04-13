//Michael Sorger and Sean Hammond
//Bouncer .c file
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>


//Error - maybe from 32 bits per line?

//*************************************************
//this is copied from the tutorial
/*
void saveFrame(AVFrame *vFrame, int width, int height, int iFrame)
{
  FILE *imgFILE;
  char sizeFile[32];
  int i;

  //open file
  sprintf(sizeFile, "frame%d.spff", iFrame);
  imgFILE = fopen(sizeFile, "wb");
  if(imgFILE==NULL)
    return;

  //write header
  fprintf(imgFILE, "P6\n%d %d\n255\n", width, height);

  //write pixel data
  for(i = 0; i < height; i++)
    fwrite(vFrame->data[0]+i*vFrame->linesize[0],1,width*3,imgFILE);

  //close file
  fclose(imgFILE);

}
*/


      /*
      int x;
      int y;
      int ls0 = vFrameRGB->linesize[0];
      
      printf("ls0 = %d \n", ls0);
      printf("vFrameRGB width = %d \n", vFrameRGB->width);
      printf("vFrameRGB height = %d \n", vFrameRGB->height);

      //Draw Rectangle
      for(y = 0; y < vFrameRGB->height ; y++)
	{
	  for(x = 0; x <vFrameRGB->width ; x++)
	    {
	      if(x > 150 && x < 200 && y > 500 && y < 550)
		{
		  vFrameRGB->data[0][ls0*y+x*3+0] = 0x22; //R
		  vFrameRGB->data[0][ls0*y+x*3+1] = 0x44; //G
		  vFrameRGB->data[0][ls0*y+x*3+2] = 0x88; //B
		}
	    }
	
} reference for drawBall
      */


void DrawBall(uint8_t* data, int lineSize, int width, int height, int centerX, int centerY, int radius, int color)
{
  int x, y;
  int r = (color >> 16) & 255; //gives the R value from colorRGB 0xRGB
  int g = (color >> 8) & 255;  //gives the G value from colorRGB 0xRGB
  int b = (color >> 0) & 255;  //gives the B value from colorRGB 0xRGB
  for(y = 0; y < height ; y++)
    {
      for(x = 0; x < width ; x++)
	{
	  int dx = centerX - x;
	  int dy = centerY - y;
	  int dist = dx*dx + dy*dy;
	  if(dist < radius*radius)
	    {
	      data[lineSize*y+x*3+0] = r; //R
	      data[lineSize*y+x*3+1] = g; //G
	      data[lineSize*y+x*3+2] = b; //B
	    }
	}
    }
}


int main(int argc, char *argv[])
{
  //Initialize these to Null prevents segfaults
  AVPacket imgPack;
  AVFormatContext *inContext = NULL;
  AVCodecContext *imgCodex = NULL;
  AVCodec *vCodec = NULL;
  AVFrame *vFrame = NULL;
  AVFrame *vFrameRGB = NULL;
  AVCodecParameters *codecParams; 
  int i, videoStream;
  FILE *file;

  //register all the codecs
  av_register_all();

  if(argc !=2)
    {
      printf("Please input a jpg image");
    }
  else
    {
      if(avformat_open_input(&inContext, argv[1], NULL, NULL) != 0)
  	{
	  printf("Couldn't open file - open_input \n");
	  return -1;
	}
      
      if(avformat_find_stream_info(inContext, NULL) < 0)
	{
	  printf("Could not find stream information");
	  return -1;
	}

      //dump information about file onto standard error
      av_dump_format(inContext, 0, argv[1], 0);
            
      //Find the first video stream
      videoStream=-1;
      for(i=0; i<inContext->nb_streams; i++)
	{
	  if(inContext->streams[i]->codecpar->codec_type==AVMEDIA_TYPE_VIDEO)
	    {
	      videoStream=i;
	      break;
	    }
	}
      
      if(videoStream == -1)
	{
	  printf("Didnt not find a video stream");
	  return -1;
	}

      //Allocate Memory for codex context
      imgCodex = avcodec_alloc_context3(vCodec);

      // The pointer to the codec context for the video stream for decoding
      codecParams = inContext->streams[videoStream]->codecpar;
      if(avcodec_parameters_to_context(imgCodex, codecParams) < 0)
	{
	  printf("Could not get codexContext from parameters");
	  return -1;
	}

      //find the decoder for the video stream
      vCodec = avcodec_find_decoder(imgCodex->codec_id);
      if(vCodec->id != AV_CODEC_ID_MJPEG)
	{
	  printf("This is not a Jpg file, cannot decode");
	  return -1;
	}
      
      //attempt to open codec
      if(avcodec_open2(imgCodex, vCodec, 0) < 0)
	{
	  printf("Could not open codec\n");
	  return -1;
	}

      //When the JPG image data comes in uncompressed, these are the pixelformats that it comes in as.
      //The pixelformats; however, are deprecated and thus need to be changed to a different one
      switch (imgCodex->pix_fmt)
	{
	case AV_PIX_FMT_YUVJ420P :
	  imgCodex->pix_fmt = AV_PIX_FMT_YUV420P;
	  break;
	case AV_PIX_FMT_YUVJ422P  :
	  imgCodex->pix_fmt = AV_PIX_FMT_YUV422P;
	  break;
	case AV_PIX_FMT_YUVJ444P   :
	  imgCodex->pix_fmt = AV_PIX_FMT_YUV444P;
	  break;
	case AV_PIX_FMT_YUVJ440P :
	  imgCodex->pix_fmt = AV_PIX_FMT_YUV440P;
	default:
	  break;
	}

      //STORING THE DATA

      //make and allocate memory for frames
      vFrame = av_frame_alloc();          
      vFrameRGB = av_frame_alloc();

      //av_image_alloc 
      av_image_alloc(vFrame->data, vFrame->linesize, imgCodex->width, imgCodex->height,
		     imgCodex->pix_fmt, 16);

      av_image_alloc(vFrameRGB->data, vFrameRGB->linesize, imgCodex->width, imgCodex->height,
		     imgCodex->pix_fmt, 16);
      

      //make the buffer to store data from frame into
      uint8_t *buffer = NULL;
      int numBytes;
      //Determine required buffer size and allocate buffer
      numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, imgCodex->width, imgCodex->height,1);
      buffer = (uint8_t *)av_malloc(numBytes*sizeof(uint8_t));

      //fill frame with info from copy frame (had to change info to RGB24
      //this is the non-deprecated version of avpicture_fill().

      vFrameRGB->height = imgCodex->height;
      vFrameRGB->width = imgCodex->width;
      vFrameRGB->format = AV_PIX_FMT_RGB24;
      
      av_image_fill_arrays(vFrameRGB->data,vFrameRGB->linesize, buffer, AV_PIX_FMT_RGB24, imgCodex->width, imgCodex->height,1);

      //READING THE DATA

      //copy original context into another frame
      struct SwsContext *swsCtx = sws_getContext(imgCodex->width, 
						 imgCodex->height,
						 imgCodex->pix_fmt,
						 imgCodex->width,
						 imgCodex->height,
						 AV_PIX_FMT_RGB24,
						 SWS_BILINEAR,
						 NULL,
						 NULL,
						 NULL
						 );

     
      if(av_read_frame(inContext, &imgPack) != 0)
	{
	  //error
	  return -1;
	}

      //Decode video frame
      avcodec_send_packet(imgCodex,&imgPack);
      avcodec_receive_frame(imgCodex,vFrame);


      float ballPx = 75;
      float ballPy = 325;
      int ballR = 25;
      int ballX = 75;
      int ballY = 325;
      float ballVx = 2.25f;
      float ballVy = -8.0f;
      float ballAy = .18f;    //pixels/(frame*frame) 10 meters = 750 pixels 30 frames = 1 second = 75/900 
      int motion = 1;    //0 for linear 1 for euler
      //RENDER
      int renFrame;
      for(renFrame = 1; renFrame <= 300; renFrame++)
	{
	  printf("%d %f %f \n", renFrame, ballPx, ballPy);

	  if(motion == 0)
	    {
	      //move the ball linearly
	      ballX+=2;
	      if(renFrame < 150)
		{
		  ballY+=-1;
		}
	      else
		{
		  ballY+=1;
		} 
	    }
	  
	  else if(motion == 1)
	    {
	      //euler
	      ballVy+=ballAy;
	      ballPx+=ballVx; ballPy+=ballVy;
	      ballX = (int)ballPx; ballY = (int)ballPy; //convert to screen coordinates

	      //detect
	      if(ballPy > vFrame->height*.8 && ballVy > 0)
		{
		  //respond - flip velocity
		  ballVy = -ballVy;
		  ballPy = vFrame->height*.8;
		}

	    }


	  //draw original image into buffer
	  //thereby erasing the ball
	  //Convert Image from its native format to RGB
	  sws_scale(swsCtx,(uint8_t const * const *)vFrame->data,
		    vFrame->linesize,0,imgCodex->height,
		    vFrameRGB->data, vFrameRGB->linesize);	
	   
	  //then draw,
	  //drawball at new position
	  DrawBall(vFrameRGB->data[0], vFrameRGB->linesize[0], vFrameRGB->width, vFrameRGB->height, ballX, ballY, ballR, 0x654321); //RGB
      
         
	  //then encode, 
	  //allocate data for packet
	  AVPacket *spffPkt = av_packet_alloc();
      
	  //initiallize packet
	  av_init_packet(spffPkt);
	  spffPkt->data = NULL;
	  spffPkt->size = 0;
	  
	  // Get the encoder for spff
	  AVCodec *spffCodec = avcodec_find_encoder(AV_CODEC_ID_SPFF);
	  if(!spffCodec)
	    return -1;
      
	  // create a codec context spff
	  AVCodecContext *spffContext = avcodec_alloc_context3(spffCodec);
	  if(!spffContext)
	    return -1;
      
	  // give the values from the AVFrame to the codec context
	  spffContext->pix_fmt = AV_PIX_FMT_RGB24; //vFrameRGB->format;
	  spffContext->height = vFrameRGB->height;
	  spffContext->width = vFrameRGB->width;
	  spffContext->time_base = (AVRational){1,30};
	  
	  //avcodec_open2 does not support AV_PIX_FMT_RGB24..?
	  if(avcodec_open2(spffContext, spffCodec, NULL) < 0)
	    {
	      printf("Could not open codec \n");
	      return -1;
	    }


	  // get the encoder from frame
	  if(avcodec_send_frame(spffContext, vFrameRGB)!=0)
	    {
	      printf("Could not send the frame \n");
	      return -1;
	    }

	  // get a packet from spff and place it in imgPack
	  if(avcodec_receive_packet(spffContext, spffPkt) != 0)
	    {
	      
	      printf("Could not find the packet \n");
	      return -1;
	    }
	  
	  //and then save
	  //av packet dump
	  char fileName[80];
	  sprintf(fileName,"frame%03d.spff",renFrame); //3d is always 3 digits long, starting with 001,002,003,004,...300
	  file = fopen(fileName, "wb");
	  fwrite(spffPkt->data, 1, spffPkt->size, file);
	  
	  //Since we made it in for loop, have to free it in loop, made every frame, free everry frame
	  av_packet_unref(spffPkt);

	  //Make/remake context, not sure
	  avcodec_close(spffContext);
	}
     

  //av_pkt_dump2(FILE f, const AVPacket* pkt, int dump payload (dont matter = 0),Const AVStream* st)
  // file stream point where the dump should be sent to
  // st AVStream the packet belongs to

      //Free the packet that was allocated by av_read_frame
      av_packet_unref((struct AVPacket*)&imgPack);
      
      //Free the RGB image
      av_free(buffer);
      av_free(vFrameRGB);
  
      //Free the YUV frame
      av_free(vFrame);

      //Close the codecs
      avcodec_close(imgCodex);
      

      //Close the video file
      avformat_close_input(&inContext);
    }
  return 0;
}

 /* 
  A visual reminder of what is declared

  AVPacket imgPack;
  AVFormatContext *inContext;
  AVCodecContext *imgCodex;
  AVCodec *vCodec;
  AVFrame *vFrame;
  AVFrame *vFrameRGB;
 */
