#ifndef ISP_UNIT_STAT_EXP_H
#define ISP_UNIT_STAT_EXP_H

#include "defination.h"

typedef struct stat_ae_parameters {
	unsigned short left;
	unsigned short top;
	unsigned short width;
	unsigned short height;
} stat_ae_parameters_t;

isp_unit_result_t isp_unit_stat_exp(
		unsigned char *buf,
		isp_3a_statistic_t *isp_3a_buf
		);

#endif
