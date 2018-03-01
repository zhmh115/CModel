#include "isp_unit_remove_blc.h"
#include <stdio.h>

#define SETTING_FILE_PATHNAME	"./profiles/remove_blc_profile.txt"

isp_unit_result_t isp_unit_remove_blc(
		unsigned char *buf,
		isp_3a_statistic_t *isp_3a_buf)
{
	int i = 0, j = 0, bufIndex = 0;
	unsigned int temp;
	unsigned short imageWidth = isp_3a_buf->imageWidth;
	unsigned short imageHeight = isp_3a_buf->imageHeight;

	for (i = 0; i < imageHeight; i++) {
		for (j = 0; j < imageWidth; j++) {
			bufIndex = (i * imageWidth + j) * 2;
			temp = buf[bufIndex] + buf[bufIndex + 1] * 256;
			temp = temp > isp_3a_buf->nBLC ? (temp - isp_3a_buf->nBLC) : 0;
			buf[bufIndex] = temp & 0xff;
			buf[bufIndex + 1] = temp / 256;
		}
	}

	return ISP_UNIT_RESULT_OK;
}
