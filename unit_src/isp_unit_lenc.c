#include "isp_unit_lenc.h"
#include <stdio.h>

#define SETTING_FILE_PATHNAME	"./profiles/lenc_profile.txt"

isp_unit_result_t isp_unit_lenc(
		unsigned char *buf,
		isp_3a_statistic_t *isp_3a_buf)
{
	printf("lenc using profile\n");
	return ISP_UNIT_RESULT_OK;
}
