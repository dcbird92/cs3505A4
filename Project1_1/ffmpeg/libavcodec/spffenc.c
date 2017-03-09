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

static int spff_encode_frame(AVCodecContext *avctx, AVPacket *AVpkt,
                            const AVFrame *pict, int *got_packet)
{
    const AVFrame * const pointerToPicture = pict;
    int n_bytes_image, n_bytes_per_row, n_bytes, headerSize, retInt;
    int pad_bytes_per_row = 0;
    int i;
    int bit_count = avctx->bits_per_coded_sample;
    uint8_t *dataPtr, *dataBuffer;

    av_assert1(bit_count == 8); //bits_per_coded_sample = 8 check
    n_bytes_per_row = ((int64_t)avctx->width * (int64_t)bit_count + 7LL)/8; //find number of bytes per row
    pad_bytes_per_row = (4 - n_bytes_per_row) & 3; //find number of pad bytes per row (max is 3 bytes)
    n_bytes_image = avctx->height * (n_bytes_per_row + pad_bytes_per_row); //find number of bytes for the image

    // STRUCTURE for SPFFFILEHEADER
    // and related pages.
#define SIZE_SPFFHEADER 14
#define SIZE_SPFFINFOHEADER 40
    headerSize = SIZE_SPFFHEADER + SIZE_SPFFINFOHEADER; //calculate headerSize
    n_bytes = n_bytes_image + headerSize; //sum of bytes of image and headerSize
    if ((retInt = ff_alloc_packet2(avctx, AVpkt, n_bytes, 0)) < 0) //allocate data using AVpkt
        return retInt;
    dataBuffer = AVpkt->data; //*int buf = data (8 bit int)
    bytestream_put_byte(&dataBuffer, 'S');                   // SPFFHEADER.bfType
    bytestream_put_byte(&dataBuffer, 'P');                   // do.
    bytestream_put_byte(&dataBuffer, 'F');
    bytestream_put_byte(&dataBuffer, 'F');
    bytestream_put_le32(&dataBuffer, n_bytes);               // SPFFHEADER.bfSize
    bytestream_put_le32(&dataBuffer, headerSize);            // SPFFHEADER.bfOffBits
    bytestream_put_le32(&dataBuffer, SIZE_SPFFINFOHEADER);   // SPFFINFOHEADER.biSize
    bytestream_put_le32(&dataBuffer, avctx->width);          // SPFFINFOHEADER.biWidth
    bytestream_put_le32(&dataBuffer, avctx->height);         // SPFFINFOHEADER.biHeight
    bytestream_put_le16(&dataBuffer, 1);                     // SPFFINFOHEADER.biPlanes
    bytestream_put_le16(&dataBuffer, bit_count);             // SPFFINFOHEADER.biBitCount
    bytestream_put_le32(&dataBuffer, n_bytes_image);         // SPFFINFOHEADER.biSizeImage

    // SPFF files are currently bottom-to-top so we start from the end...
    dataPtr = pointerToPicture->data[0] + (avctx->height - 1) * pointerToPicture->linesize[0]; //set pointer to byte line at the bottom of picture 
    dataBuffer = AVpkt->data + headerSize; //set the dataBuffer to AVpacket's data + headersize
    for(i = 0; i < avctx->height; i++) 
      { 
	memcpy(dataBuffer, dataPtr, n_bytes_per_row); //copies dataBuffer, dataPtr, and bytes per row into memory
        dataBuffer += n_bytes_per_row; //dataBuffer increments for next pass
        memset(dataBuffer, 0, pad_bytes_per_row); //fill block in memory with the dataBuffer and pad 
        dataBuffer += pad_bytes_per_row; //dataBuffer adds the pad at the end for next pass
        dataPtr -= pointerToPicture->linesize[0]; //assign the pointer to location to be picture pointer linesize - 1
      }

    AVpkt->flags |= AV_PKT_FLAG_KEY; //pkt->flags = pkt->flags | AV_PKT_FLAG_KEY (pkt contains keyframe, bool operation)
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
