/*
 * Michael Sorger and Sean Hammond
 * SPFF image format encoder
 * Copyright (c) 2006, 2007 Michel Bardiaux
 * Copyright (c) 2009 Daniel Verkamp <daniel at drv.nu>
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

#include "libavutil/imgutils.h"
#include "libavutil/avassert.h"
#include "avcodec.h"
#include "bytestream.h"
#include "spff.h"
#include "internal.h"

static av_cold int spff_encode_init(AVCodecContext *avctx)
{
  avctx->bits_per_coded_sample = 8; //(msb) 2R 3G 3B (lsb)
  return 0;
}

static int spff_encode_frame(AVCodecContext *avctx, AVPacket *pkt,
                            const AVFrame *pict, int *got_packet)
{
    const AVFrame * const p = pict;
    int n_bytes_image, n_bytes_per_row, n_bytes, hsize, ret;
    int pad_bytes_per_row = 0;
    int i;
    int bit_count = avctx->bits_per_coded_sample;
    uint8_t *ptr, *buf;

    av_assert1(bit_count == 8); //bits_per_coded_sample = 8 check
    n_bytes_per_row = ((int64_t)avctx->width * (int64_t)bit_count + 7LL) >> 3LL; //width picture * 8 + 7 gives number of bits... divide by 8 for number of bytes
    pad_bytes_per_row = (4 - n_bytes_per_row) & 3; //4 - ^^ & 011
    n_bytes_image = avctx->height * (n_bytes_per_row + pad_bytes_per_row); //height picture * (bytes per row + pad bytes per row)

    // STRUCTURE for SPFFFILEHEADER
    // and related pages.
#define SIZE_SPFFHEADER 14
#define SIZE_SPFFINFOHEADER 40
    hsize = SIZE_SPFFHEADER + SIZE_SPFFINFOHEADER; //header size = 54
    n_bytes = n_bytes_image + hsize; //number of bytes is = n bytes of image + 54
    if ((ret = ff_alloc_packet2(avctx, pkt, n_bytes, 0)) < 0) //allocate int ret with given
        return ret;
    buf = pkt->data; //*int buf = data (8 bit int)
    bytestream_put_byte(&buf, 'S');                   // SPFFHEADER.bfType
    bytestream_put_byte(&buf, 'P');                   // do.
    bytestream_put_byte(&buf, 'F');
    bytestream_put_byte(&buf, 'F');
    bytestream_put_le32(&buf, n_bytes);               // SPFFHEADER.bfSize
    bytestream_put_le32(&buf, hsize);                 // SPFFHEADER.bfOffBits
    bytestream_put_le32(&buf, SIZE_SPFFINFOHEADER);   // SPFFINFOHEADER.biSize
    bytestream_put_le32(&buf, avctx->width);          // SPFFINFOHEADER.biWidth
    bytestream_put_le32(&buf, avctx->height);         // SPFFINFOHEADER.biHeight
    bytestream_put_le16(&buf, 1);                     // SPFFINFOHEADER.biPlanes
    bytestream_put_le16(&buf, bit_count);             // SPFFINFOHEADER.biBitCount
    bytestream_put_le32(&buf, n_bytes_image);         // SPFFINFOHEADER.biSizeImage

    // SPFF files are currently bottom-to-top so we start from the end...
    ptr = p->data[0] + (avctx->height - 1) * p->linesize[0]; //set pointer to byte line at the bottom of picture 
    buf = pkt->data + hsize; //buf (8 bit pointer) = packet data size + 54
    for(i = 0; i < avctx->height; i++)   //for the entire height of the image (749 pixels for us)
      { 
	memcpy(buf, ptr, n_bytes_per_row); //copies buf (8 bit pointer int), pointer, "" ...??
        buf += n_bytes_per_row; //buf adds number of bytes to itself
        memset(buf, 0, pad_bytes_per_row); //fill block in memory with the buffer (int), 0, 
        buf += pad_bytes_per_row; //add the buffer's number buffer
        ptr -= p->linesize[0]; //assign the pointer to location to be picture pointer linesize - 1
      }

    pkt->flags |= AV_PKT_FLAG_KEY; //pkt->flags = pkt->flags | AV_PKT_FLAG_KEY (pkt contains keyframe, bool operation)
    *got_packet = 1; //int pointer gets set to 1
    return 0;
}

AVCodec ff_spff_encoder = {
    .name           = "spff",
    .long_name      = NULL_IF_CONFIG_SMALL("SPFF (A Project for CS 3505)"),
    .type           = AVMEDIA_TYPE_VIDEO,
    .id             = AV_CODEC_ID_SPFF,
    .init           = spff_encode_init,
    .encode2        = spff_encode_frame,
    .pix_fmts       = (const enum AVPixelFormat[]){
    AV_PIX_FMT_RGB8,
    AV_PIX_FMT_NONE
    },
};
