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
    int i, j, n;
    int bit_count = avctx->bits_per_coded_sample;
    uint8_t *ptr, *buf;

    av_assert1(bit_count == 8);;
    n_bytes_per_row = ((int64_t)avctx->width * (int64_t)bit_count + 7LL) >> 3LL;
    pad_bytes_per_row = (4 - n_bytes_per_row) & 3;
    n_bytes_image = avctx->height * (n_bytes_per_row + pad_bytes_per_row);

    // STRUCTURE.field refer to the MSVC documentation for BITMAPFILEHEADER
    // and related pages.
#define SIZE_BITMAPFILEHEADER 14
#define SIZE_BITMAPINFOHEADER 40
    hsize = SIZE_BITMAPFILEHEADER + SIZE_BITMAPINFOHEADER;
    n_bytes = n_bytes_image + hsize;
    if ((ret = ff_alloc_packet2(avctx, pkt, n_bytes, 0)) < 0)
        return ret;
    buf = pkt->data;
    bytestream_put_byte(&buf, 'S');                   // BITMAPFILEHEADER.bfType
    bytestream_put_byte(&buf, 'P');                   // do.
    bytestream_put_byte(&buf, 'F');
    bytestream_put_byte(&buf, 'F');
    bytestream_put_le32(&buf, n_bytes);               // BITMAPFILEHEADER.bfSize
    bytestream_put_le32(&buf, hsize);                 // BITMAPFILEHEADER.bfOffBits
    bytestream_put_le32(&buf, SIZE_BITMAPINFOHEADER); // BITMAPINFOHEADER.biSize
    bytestream_put_le32(&buf, avctx->width);          // BITMAPINFOHEADER.biWidth
    bytestream_put_le32(&buf, avctx->height);         // BITMAPINFOHEADER.biHeight
    bytestream_put_le16(&buf, 1);                     // BITMAPINFOHEADER.biPlanes
    bytestream_put_le16(&buf, bit_count);             // BITMAPINFOHEADER.biBitCount
    bytestream_put_le32(&buf, n_bytes_image);         // BITMAPINFOHEADER.biSizeImage

    // BMP files are bottom-to-top so we start from the end...
    ptr = p->data[0] + (avctx->height - 1) * p->linesize[0];
    buf = pkt->data + hsize;
    for(i = 0; i < avctx->height; i++) {
        if (bit_count == 16) {
            const uint16_t *src = (const uint16_t *) ptr;
            uint16_t *dst = (uint16_t *) buf;
            for(n = 0; n < avctx->width; n++)
                AV_WL16(dst + n, src[n]);
        } else {
            memcpy(buf, ptr, n_bytes_per_row);
        }
        buf += n_bytes_per_row;
        memset(buf, 0, pad_bytes_per_row);
        buf += pad_bytes_per_row;
        ptr -= p->linesize[0]; // ... and go back
    }

    pkt->flags |= AV_PKT_FLAG_KEY;
    *got_packet = 1;
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
