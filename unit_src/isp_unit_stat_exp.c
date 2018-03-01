#include "isp_unit_stat_exp.h"
#include <stdio.h>

#define SETTING_FILE_PATHNAME	"./profiles/stat_exp_profile.txt"

static stat_ae_parameters_t para;

static isp_unit_result_t load_setting(stat_ae_parameters_t *para)
{
	char str[128];
	FILE *fp = fopen(SETTING_FILE_PATHNAME, "rb");
	if (fp != NULL) {
		while (!feof(fp)) {
			fgets(str, 128, fp);
			if (sscanf(str, "statWinLeft = %hd", &para->left)) continue;
			if (sscanf(str, "statWinTop = %hd", &para->top)) continue;
			if (sscanf(str, "statWinWidth = %hd", &para->width)) continue;
			if (sscanf(str, "statWinHeight = %hd", &para->height)) continue;
		}
		fclose(fp);
	} else {
		printf("open stat ae profile failed, create a new profile\n");
		fp = fopen(SETTING_FILE_PATHNAME, "wb");
		if (fp != NULL) {
			para->left = 0;
			sprintf(str, "statWinLeft = %hd\n", para->left);
			fputs(str, fp);
			para->top = 0;
			sprintf(str, "statWinTop = %hd\n", para->top);
			fputs(str, fp);
			para->width = 4000;
			sprintf(str, "statWinWidth = %hd\n", para->width);
			fputs(str, fp);
			para->height = 3000;
			sprintf(str, "statWinHeight = %hd\n", para->height);
			fputs(str, fp);
			fclose(fp);
		}
	}

	return ISP_UNIT_RESULT_OK;
}

isp_unit_result_t isp_unit_stat_exp(
		unsigned char *buf,
		isp_3a_statistic_t *isp_3a_buf)
{
	unsigned long sumB = 0, sumBG = 0, sumR = 0, sumRG = 0, accumulator = 0;
	int i = 0, j = 0, x, y, bufIndex = 0;
	unsigned short imageWidth = isp_3a_buf->imageWidth;
	unsigned short imageHeight = isp_3a_buf->imageHeight;

	load_setting(&para);

	para.left = (para.left >> 1) << 1;
	para.top = (para.top >> 1) << 1;
	para.width = (para.width >> 1) << 1;
	para.height = (para.height >> 1) << 1;

	if ((para.left < 0) && (para.top < 0) &&
			(para.left + para.width > isp_3a_buf->imageWidth) &&
			(para.top + para.height > isp_3a_buf->imageHeight)) {
		printf("improper ae statistic window!\n");
		return ISP_UNIT_RESULT_FAIL;
	}

	for (i = 0; i < para.height; i += 2) {
		for (j = 0; j < para.width; j += 2, accumulator++) {
			x = para.left + j;
			y = para.top + i;
			bufIndex = (y * imageWidth + x) * 2;
			sumB += buf[bufIndex] + buf[bufIndex + 1] * 256;

			x = para.left + j + 1;
			y = para.top + i;
			bufIndex = (y * imageWidth + x) * 2;
			sumBG += buf[bufIndex] + buf[bufIndex + 1] * 256;

			x = para.left + j;
			y = para.top + i + 1;
			bufIndex = (y * imageWidth + x) * 2;
			sumRG += buf[bufIndex] + buf[bufIndex + 1] * 256;

			x = para.left + j + 1;
			y = para.top + i + 1;
			bufIndex = (y * imageWidth + x) * 2;
			sumR += buf[bufIndex] + buf[bufIndex + 1] * 256;
		}
	}
	isp_3a_buf->meanB = (sumB / accumulator)>>2;
	isp_3a_buf->meanBG = (sumBG / accumulator)>>2;
	isp_3a_buf->meanRG = (sumRG / accumulator)>>2;
	isp_3a_buf->meanR = (sumR / accumulator)>>2;

	return ISP_UNIT_RESULT_OK;
}
