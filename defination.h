#ifndef DEFINATION_H
#define DEFINATION_H

#include <stdlib.h>
#include <string.h>
#include <math.h>

#define min(x, y) ((x < y ? x : y))
#define max(x, y) ((x > y ? x : y))

typedef enum isp_unit_result {
	ISP_UNIT_RESULT_OK = 0,
	ISP_UNIT_RESULT_FAIL,
} isp_unit_result_t;

typedef struct isp_rect {
	unsigned short left;
	unsigned short top;
	unsigned short width;
	unsigned short height;
} isp_rect_t;

typedef struct isp_3a_statistic {
	/* image size read from vrf */
	unsigned short imageWidth;
	unsigned short imageHeight;

	/* exp info read from vrf */
	unsigned int curExp;
	unsigned int curGain;
	unsigned int curVTS;

	/* awb gain read from vrf */
	unsigned int curBGain;
	unsigned int curGGain;
	unsigned int curRGain;

	/* blc read from vrf */
	unsigned char nBLC;

	/* exposure statistic */
	unsigned int meanY;
	unsigned int meanB;
	unsigned int meanBG;
	unsigned int meanRG;
	unsigned int meanR;

    /*raw stretch statistic*/
    unsigned int stretchGain;
    unsigned int stretchOffset;

	/* awb gain statistic */
	unsigned int BGain;
	unsigned int GGain;
	unsigned int RGain;
	/* fv statistic */
	unsigned int fvValue;
} isp_3a_statistic_t;

static inline int interp1(int x1, int x, int x2, int y1, int y2)
{
	return ((y2 - y1) * (x - x1) / (x2 - x1)) + y1;
}

static inline int clip(int input, int a, int b)
{
    return ((input < a) ? a : ((input <= b) ? input : b));
}

#endif
