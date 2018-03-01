#include "isp_unit_stat_fv.h"
#include <stdio.h>

#define SETTING_FILE_PATHNAME	"./profiles/stat_fv_profile.txt"

isp_unit_result_t isp_unit_stat_fv(
		unsigned char *buf,
		isp_3a_statistic_t *isp_3a_buf)
{
	printf("stat fv using profile\n");
	return ISP_UNIT_RESULT_OK;
}
