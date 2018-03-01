#ifndef ISP_UNIT_STAT_WB_H
#define ISP_UNIT_STAT_WB_H

#include "defination.h"

typedef struct stat_wb_parameters {
	int xOffset;
	int yOffset;
	int xScale;
	int yScale;
	unsigned char wbMap[256];
} stat_wb_parameters_t;

isp_unit_result_t isp_unit_stat_wb(
		unsigned char *buf,
		isp_3a_statistic_t *isp_3a_buf
		);

#endif
