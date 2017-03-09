/*
 * BMP image format decoder
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

#include <stdio.h>
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
    const uint8_t *buf = avpkt->data; //buf is an int pointer to av packet data
    int buf_size       = avpkt->size; // buf_size points to size of packet data
    AVFrame *p         = data; //points to frame data
    unsigned int fsize, hsize; //file size, header size
    int width, height; //width and height of image
    unsigned int depth; 

    unsigned int ihsize; //info header size
    int i, j, n, linesize, ret; 
    uint8_t *ptr;
    const uint8_t *buf0 = buf;

    //packet data is too small
    if (buf_size < 14) {
        av_log(avctx, AV_LOG_ERROR, "buf size too small (%d)\n", buf_size);
        return AVERROR_INVALIDDATA;
    }

    //if beginning of buffer does not have these
    if (bytestream_get_byte(&buf) != 'S' ||
        bytestream_get_byte(&buf) != 'P' ||
	bytestream_get_byte(&buf) != 'F' ||
	bytestream_get_byte(&buf) != 'F')
      {
        av_log(avctx, AV_LOG_ERROR, "bad magic number\n");
        return AVERROR_INVALIDDATA;
      }

    //assigns file size to be info given from buffer, checks if all buffer info exists
    fsize = bytestream_get_le32(&buf);
    if (buf_size < fsize) {
        av_log(avctx, AV_LOG_ERROR, "not enough data (%d < %u), trying to decode anyway\n",
               buf_size, fsize);
        fsize = buf_size;;
    }

    //assigns header/infor  to be information of buffer and checks to see if theres a difference
    hsize  = bytestream_get_le32(&buf); /* header size */
    ihsize = bytestream_get_le32(&buf); /* more header size */
    if (ihsize + 14LL > hsize) {
        av_log(avctx, AV_LOG_ERROR, "invalid header size %u\n", hsize);
        return AVERROR_INVALIDDATA;
    }

    /* sometimes file size is set to some headers size, set a real size in that case */
    if (fsize == 14 || fsize == ihsize + 14)
        fsize = buf_size - 2;

    if (fsize <= hsize) {
        av_log(avctx, AV_LOG_ERROR,
               "Declared file size is less than header size (%u < %u)\n",
               fsize, hsize);
        return AVERROR_INVALIDDATA;
    }
    avctx->pix_fmt = AV_PIX_FMT_RGB8;

    //ihsize = 40
    width  = bytestream_get_le32(&buf);
    height = bytestream_get_le32(&buf);


    /* planes */
    if (bytestream_get_le16(&buf) != 1) {
        av_log(avctx, AV_LOG_ERROR, "invalid SPFF header\n");
        return AVERROR_INVALIDDATA;
    }

    depth = bytestream_get_le16(&buf); //8

    avctx->width  = width;
    avctx->height = height > 0 ? height : -height;

    if ((ret = ff_get_buffer(avctx, p, 0)) < 0)
        return ret;
    p->pict_type = AV_PICTURE_TYPE_I;
    p->key_frame = 1;

    buf   = buf0 + hsize;

    n = ((avctx->width * depth + 31) / 8) & ~3;

    if (height > 0) {
        ptr      = p->data[0] + (avctx->height - 1) * p->linesize[0];
        linesize = -p->linesize[0];
    } else {
        ptr      = p->data[0];
        linesize = p->linesize[0];
    }

            for (i = 0; i < avctx->height; i++)
	      {
                memcpy(ptr, buf, n);
                buf += n;
                ptr += linesize;
	      }

    static int beenHere = 0;
    
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
