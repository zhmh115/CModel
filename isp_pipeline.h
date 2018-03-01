#include "defination.h"
#include "isp_unit_remove_blc.h"
#include "isp_unit_lenc.h"
#include "isp_unit_rawdns.h"
#include "isp_unit_stat_exp.h"
#include "isp_unit_stat_wb.h"
#include "isp_unit_stat_fv.h"
#include "isp_unit_rawstretch.h"
#include "isp_unit_apply_wb.h"
#include "isp_unit_ccm.h"
#include "isp_unit_sharpen.h"
#include "isp_unit_rgbgamma.h"
#include "isp_unit_cip.h"
#include "isp_unit_convert_yuv.h"

static isp_unit_result_t
(*isp_unit_process[])(
		unsigned char *buf,
		isp_3a_statistic_t *isp_3a_buf) = {

	isp_unit_remove_blc,
	isp_unit_lenc,
	isp_unit_rawdns,
	isp_unit_stat_exp,
	isp_unit_stat_wb,
	isp_unit_stat_fv,
	isp_unit_rawstretch,
	isp_unit_apply_wb,
	isp_unit_sharpen,
	isp_unit_cip,
	isp_unit_ccm,
	isp_unit_rgbgamma,
	isp_unit_convert_yuv,

};
