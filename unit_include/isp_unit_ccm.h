#ifndef ISP_UNIT_CCM_H
#define ISP_UNIT_CCM_H

#include "defination.h"

#define CCM_TABLE_NUM	3

typedef struct ccm_parameters {
	int ccmCoef[CCM_TABLE_NUM][9];
	int ctcp[CCM_TABLE_NUM];
	int sat;
} ccm_parameters_t;

isp_unit_result_t isp_unit_ccm(
		unsigned char *buf,
		isp_3a_statistic_t *isp_3a_buf
		);

#endif
