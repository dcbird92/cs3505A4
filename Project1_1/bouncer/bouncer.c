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

void DrawBall()
{
  
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
	  
      //Convert Image from its native format to RGB
      sws_scale(swsCtx,(uint8_t const * const *)vFrame->data,
		vFrame->linesize,0,imgCodex->height,
		vFrameRGB->data, vFrameRGB->linesize);	

	  
      
      
      //Copy to another frame after conversion, then draw, then encode, and then save


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
      spffContext->pix_fmt = vFrameRGB->format;
      spffContext->height = vFrameRGB->height;
      spffContext->width = vFrameRGB->width;
      spffContext->time_base = (AVRational){1,30};

      printf("here? \n");
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

      printf("here? \n");
      // get a packet from spff and place it in imgPack

      if(avcodec_receive_packet(spffContext, spffPkt) != 0)
	{
	  
	  printf("Could not find the packet \n");
	  return -1;
	}

      printf("here? \n");
      //av packet dump
      file = fopen("frame%d.spff", "wb");
      fwrite(spffPkt->data, 1, spffPkt->size, file);

  //av_pkt_dump2(FILE f, const AVPacket* pkt, int dump payload (dont matter = 0),Const AVStream* st)
  // file stream point where the dump should be sent to
  // st AVStream the packet belongs to

      //Free the packet that was allocated by av_read_frame
  
      //Free the RGB image
      av_free(buffer);
      av_free(vFrameRGB);
  
      //Free the YUV frame
      av_free(vFrame);

      //Close the codecs
      avcodec_close(imgCodex);
      avcodec_close(spffContext);
      

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
