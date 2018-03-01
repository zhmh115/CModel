#include "isp_unit_rawdns.h"
#include <stdio.h>

#define SETTNG_FILE_PATHNAME	"./profiles/rawdns_profile.txt"

isp_unit_result_t isp_unit_rawdns(
		unsigned char *buf,
		isp_3a_statistic_t *isp_3a_buf)
{
	printf("rawdns using profile\n");
	return ISP_UNIT_RESULT_OK;
}
