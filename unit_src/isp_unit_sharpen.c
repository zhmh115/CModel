#include "isp_unit_sharpen.h"
#include <stdio.h>

#define SETTING_FILE_PATHNAME	"./profiles/sharpen_profile.txt"

isp_unit_result_t isp_unit_sharpen(
		unsigned char *buf,
		isp_3a_statistic_t *isp_3a_buf)
{
	printf("sharpen using profile\n");
	return ISP_UNIT_RESULT_OK;
}
