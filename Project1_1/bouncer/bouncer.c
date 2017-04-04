//Michael Sorger and Sean Hammond
//Bouncer .c file
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>


int main(int argc, char *argv[])
{

  //register all the codecs
  avcodec_register_all();

  if(argc !=2)
    {
      //we need 2 arguements, null pointer, jpg, missing an argument?
      printf("Please input a jpg image");
    }
  else
    {
      AVFormatContext *inContext = NULL;
      if(avformat_open_input(&inContext, argv[1], NULL, NULL) != 0)
	{
	  printf("Couldn't open file");
	  return NULL;
	}

      //dump information about file onto standar error
      av_dump_format(inContext, 0, argv[1], 0);

      AVCodecContext *imgCodex = NULL;
      // AVCodecContext *imageCodex = NULL;
      
      imgCodex = inContext->streams[0]->codec;
      /*
      imgCodex->width = inContext->width;
      imgCodex->height = inContext->height;
      imgCodex->pix_fmt = inContext->pix_fmt;
      */
     
      //find the deocder for the video stream
      AVCodec *vCodec = avcodec_find_decoder(imgCodex->codec_id);
      if(vCodec != AV_CODEC_ID_MJPEG)
	{
	  printf("This is not a Jpg file, cannot decode");
	  return NULL;
	}

      //open codec
      if(avcodec_open(imgCodex, vCodec) < 0)
	{
	  printf("Could not open codec\n");
	  return NULL;
	}
      
      AVFrame *vFrame = NULL;
      vFrame = av_frame_alloc();
      
      uint8_t *buffer = (uint8_t *)av_malloc(32*sizeof(uint8_t)); //32 * 8
      
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

      
    }
  return 0;
}
