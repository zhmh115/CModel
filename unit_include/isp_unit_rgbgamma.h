#ifndef ISP_UNIT_RGBGAMMA_H
#define ISP_UNIT_RGBGAMMA_H

#include "defination.h"

#define CURVE_NUM	3
#define CURVE_CP_NUM	255

typedef struct gamma_parameters {
	unsigned int expIdxCtrlPt[2 * (CURVE_NUM - 1)];
	unsigned short node[CURVE_NUM][CURVE_CP_NUM];
} gamma_parameters_t;

isp_unit_result_t isp_unit_rgbgamma(
		unsigned char *buf,
		isp_3a_statistic_t *isp_3a_buf
		);

#endif
