//Michael Sorger and Sean Hammond
//Bouncer .c file
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>


//*************************************************
//this is copied from the tutorial
void saveFrame(AVFrame *vFrame, int width, int height, int iFrame)
{
  FILE *imgFILE;
  char sizeFile[32];
  int i;

  //open file
  sprintf(sizeFile, "frame%d.ppm", iFrame);
  imgFILE = fopen(sizeFile, "spff");
  if(imgFILE==NULL)
    return;

  //write header
  fprintf(imgFILE, "P6\n%d %d\n255\n", width, height);

  //write pixel data
  for(i = 0; i < height; i++)
    fwrite(vFrame->data[0]+i*vFrame->linesize[0],1,width*3,imgFILE);

  //close file
  fclose(imgFILE);
  //******************************************************

}

void DrawBall()
{
  
}

int main(int argc, char *argv[])
{
  AVPacket imgPack;
  AVFormatContext *inContext;
  AVCodecContext *imgCodex;
  AVCodec *vCodec;
  AVFrame *vFrame;
  AVFrame *vFrameRGB;
  int i, videoStream;
  //register all the codecs
  avcodec_register_all();

  if(argc !=2)
    {
      printf("Please input a jpg image");
    }
  else
    {
      
      if(avformat_open_input(&inContext, argv[1], NULL, NULL) != 0)
	{
	  printf("Couldn't open file");
	  return 0;
	}

      //dump information about file onto standard error
      av_dump_format(inContext, 0, argv[1], 0);
      
      // Find the first video stream
      videoStream=-1;
      for(i=0; i<inContext->nb_streams; i++)
	if(inContext->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO) {
	  videoStream=i;
	  break;
	}
      if(videoStream==-1)
	return -1; // Didn't find a video stream

      // The pointer to the codec context for the video stream for decoding
      // This is deperecated
      imgCodex = inContext->streams[videoStream]->codec;
     
      //find the deocder for the video stream
      vCodec = avcodec_find_decoder(imgCodex->codec_id);
      if(vCodec->id != AV_CODEC_ID_MJPEG) //warning here, checking if JPG
	{
	  printf("This is not a Jpg file, cannot decode");
	  return 0;
	}

      //attempt to open codec
      if(avcodec_open2(imgCodex, vCodec, 0) < 0)
	{
	  printf("Could not open codec\n");
	  return 0;
	}
      
      //make and allocate memory for frame
      vFrame = av_frame_alloc();
      
      
      vFrameRGB = av_frame_alloc();

      //make the buffer to store data from frame into
      uint8_t *buffer = (uint8_t *)av_malloc(32*sizeof(uint8_t)); //32 * 8
      
      //copy original context into another frame
      struct SwsContext *swsCtx = sws_getContext(imgCodex->width, 
						 imgCodex->height,
						 imgCodex->pix_fmt,
						 imgCodex->width,
						 imgCodex->height,
						 AV_PIX_FMT_RGB24,
						 0,
						 NULL,
						 NULL,
						 NULL
						 );
      
      
      //perform a pixelcopy for info of copy frame to new frame
      

      //fill frame with info from copy frame (had to change info to RGB24
      //this is the non-deprecated version of avpicture_fill().
      av_image_fill_arrays(vFrameRGB->data,vFrameRGB->linesize, buffer, AV_PIX_FMT_RGB24, imgCodex->width, imgCodex->height,1); 
    
      av_read_frame(inContext, &imgPack);

      //  avcodec_decode_video2(imgCodex, vFrame,1, &imgPack);
     
      avcodec_send_packet(imgCodex,&imgPack);
      avcodec_receive_frame(imgCodex,vFrame);
      sws_scale(swsCtx,(uint8_t const * const *)vFrame->data,vFrame->linesize,0,imgCodex->height,
		vFrameRGB->data, vFrameRGB->linesize);
			   

    }
  
  return 0;
}
