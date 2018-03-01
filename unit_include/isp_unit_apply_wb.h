#ifndef ISP_UNIT_APPLY_WB_H
#define ISP_UNIT_APPLY_WB_H

#include "defination.h"

#define WB_SHIFT_CTCP_NUM	5
#define WB_SHIFT_BRCP_NUM	3

struct wbShiftNode {
	unsigned short bGainShift;
	unsigned short gGainShift;
	unsigned short rGainShift;
};

typedef struct apply_wb_parameters {
	unsigned long brCtrlPoint[WB_SHIFT_BRCP_NUM];
	unsigned short ctCtrlPoint[WB_SHIFT_CTCP_NUM];
	struct wbShiftNode wbShiftGain[WB_SHIFT_CTCP_NUM][WB_SHIFT_BRCP_NUM];
} apply_wb_parameters_t;

isp_unit_result_t isp_unit_apply_wb(
		unsigned char *buf,
		isp_3a_statistic_t *isp_3a_buf
		);

#endif
