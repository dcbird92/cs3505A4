/* Michael Sorger and Sean Hammond
 */

#ifndef AVCODEC_SPFF_H
#define AVCODEC_SPFF_H

#include "avcodec.h"

typedef enum {
    SPFF_RGB         =0,
    SPFF_RLE8        =1,
    SPFF_RLE4        =2,
    SPFF_BITFIELDS   =3,
} BiCompression;

#endif /* AVCODEC_SPFF_H */
