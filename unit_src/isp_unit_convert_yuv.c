#include "isp_unit_convert_yuv.h"
#include <stdio.h>

#define SETTING_FILE_PATHNAME	"./profiles/convert_yuv_profile.txt"

isp_unit_result_t isp_unit_convert_yuv(
		unsigned char *buf,
		isp_3a_statistic_t *isp_3a_buf)
{
	printf("convert yuv using profile\n");

	unsigned short imageWidth = isp_3a_buf->imageWidth;
	unsigned short imageHeight = isp_3a_buf->imageHeight;
	FILE *fp = fopen("middleOutput.rgb10", "wb");
	if (fp != NULL) {
		fwrite(buf, sizeof(unsigned char), imageWidth * imageHeight * 6, fp);
		fclose(fp);
		fp = NULL;
	}

	return ISP_UNIT_RESULT_OK;
}
