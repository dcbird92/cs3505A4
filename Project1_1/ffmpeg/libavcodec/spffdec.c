/*
 * Michael Sorger and Sean Hammond
 * SPFF image format decoder
 * Copyright (c) 2005 Mans Rullgard
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <inttypes.h>
#include "avcodec.h"
#include "bytestream.h"
#include "spff.h"
#include "internal.h"
#include "msrledec.h"

static int spff_decode_frame(AVCodecContext *avctx,
                            void *data, int *got_frame,
                            AVPacket *avpkt)
{
    const uint8_t *avpktBufPtr = avpkt->data; //int buffer that points to avpktdata
    int buf_size  = avpkt->size; //size of packet data
    AVFrame *framePointer = data; //points to frame data
    unsigned int fileSize, headerSize;
    int width, height; //width and height of image
    unsigned int depth; //the '8' in RGB8 

    unsigned int infoHeaderSize;
    int i, n, linesize, retInt; 
    uint8_t *smallUIntPointer;
    const uint8_t *bufPtrCopy = avpktBufPtr;

    static int beenHere = 0;

    //size packet is too small
    if (buf_size < 14) 
      {
        av_log(avctx, AV_LOG_ERROR, "buf size too small (%d)\n", buf_size);
        return AVERROR_INVALIDDATA;
      }

    //if beginning of buffer does not have these
    if (bytestream_get_byte(&avpktBufPtr) != 'S' ||
        bytestream_get_byte(&avpktBufPtr) != 'P' ||
	bytestream_get_byte(&avpktBufPtr) != 'F' ||
	bytestream_get_byte(&avpktBufPtr) != 'F')
      {
        av_log(avctx, AV_LOG_ERROR, "bad magic number\n");
        return AVERROR_INVALIDDATA;
      }

    //assigns file size to be info given from buffer, checks if all buffer info exists
    fileSize = bytestream_get_le32(&avpktBufPtr);
    if (buf_size < fileSize)
      {
        av_log(avctx, AV_LOG_ERROR, "not enough data (%d < %u), trying to decode anyway\n",
               buf_size, fileSize);
        fileSize = buf_size;;
      }

    //assigns header/info size to be information of buffer and checks to see if theres a difference
    headerSize  = bytestream_get_le32(&avpktBufPtr); /* header size */
    infoHeaderSize = bytestream_get_le32(&avpktBufPtr); /* more header size */
    if (infoHeaderSize + 14LL > headerSize) 
      {
        av_log(avctx, AV_LOG_ERROR, "invalid header size %u\n", headerSize);
        return AVERROR_INVALIDDATA;
      }

    /* sometimes file size is set to some headers size, set a real size in that case */
    if (fileSize == 14 || fileSize == infoHeaderSize + 14)
        fileSize = buf_size - 2;

    if (fileSize <= headerSize)
      {
        av_log(avctx, AV_LOG_ERROR,
               "Declared file size is less than header size (%u < %u)\n",
               fileSize, headerSize);
        return AVERROR_INVALIDDATA;
      }

    //set AVCodecContext pixel format to be RGB8 
    avctx->pix_fmt = AV_PIX_FMT_RGB8;

    //infoHeaderSize = 40
    width  = bytestream_get_le32(&avpktBufPtr);
    height = bytestream_get_le32(&avpktBufPtr);


    /* planes */
    if (bytestream_get_le16(&avpktBufPtr) != 1) {
        av_log(avctx, AV_LOG_ERROR, "invalid SPFF header\n");
        return AVERROR_INVALIDDATA;
    }

    depth = bytestream_get_le16(&avpktBufPtr); //8

    //set height and width of picture
    avctx->width  = width;
    avctx->height = height > 0 ? height : -height;

    //check if the picture's buffer is less than 0
    if ((retInt = ff_get_buffer(avctx, framePointer, 0)) < 0)
        return retInt;

    //dereference frame's attributes to assign 
    framePointer->pict_type = AV_PICTURE_TYPE_I;
    framePointer->key_frame = 1;

    //assign bufferPointer to be bufferPointerCopy + headerSize
    avpktBufPtr = bufPtrCopy + headerSize;

    //assign n to be bytes per row
    n = ((avctx->width * depth + 31) / 8) & ~3;

    //assignt small Pointer to start at last row of image
    if (height > 0) 
      {
        smallUIntPointer = framePointer->data[0] + (avctx->height - 1) * framePointer->linesize[0];
        linesize = -framePointer->linesize[0];
      } 
    //otherwise, it is the last row, start there
    else 
      {
        smallUIntPointer = framePointer->data[0];
        linesize = framePointer->linesize[0];
      }

    //copy each line into memory
    for (i = 0; i < avctx->height; i++)
      {
	memcpy(smallUIntPointer, avpktBufPtr, n);
	avpktBufPtr += n;
	smallUIntPointer += linesize;
      }

    //was spffdec.c ran?
    if(!beenHere)
      {
	av_log(avctx,AV_LOG_ERROR, "*** CS 3505:  Executing in %s and %s\n",__FUNCTION__,__FILE__);
	av_log(avctx, AV_LOG_ERROR, "*** CS 3505:  Modified by Michael Sorger and Sean Hammond ***\n");
	beenHere = 1;
      }

    *got_frame = 1;

    return buf_size;
}

AVCodec ff_spff_decoder = {
    .name           = "spff",
    .long_name      = NULL_IF_CONFIG_SMALL("SPFF (Project for CS 3505)"),
    .type           = AVMEDIA_TYPE_VIDEO,
    .id             = AV_CODEC_ID_SPFF,
    .decode         = spff_decode_frame,
    .capabilities   = AV_CODEC_CAP_DR1,
};
