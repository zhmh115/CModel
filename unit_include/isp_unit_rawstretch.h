#ifndef ISP_UNIT_RAWSTRETCH_H
#define ISP_UNIT_RAWSTRETCH_H

#include "defination.h"

typedef struct rawstretch_parameters {
    unsigned short darkPer;
    unsigned short minLowLevel;
    unsigned short maxLowLevel;
    unsigned short saturationPer;
    unsigned short minHighLevel;
    unsigned short maxHighLevel;
} rawstretch_parameters_t;

isp_unit_result_t isp_unit_rawstretch(
		unsigned char *buf,
		isp_3a_statistic_t *isp_3a_buf
		);

#endif
