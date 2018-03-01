#include "isp_unit_rgbgamma.h"
#include <stdio.h>

#define SETTING_FILE_PATHNAME	"./profiles/rgbgamma_profile.txt"

static gamma_parameters_t para;

static isp_unit_result_t load_setting(gamma_parameters_t *para)
{
	char str[128];
	unsigned int i = 0, j = 0, temp = 0;
	FILE *fp = fopen(SETTING_FILE_PATHNAME, "rb");
	if (fp != NULL) {
		while (!feof(fp)) {
			fgets(str, 128, fp);
			if (sscanf(str, "gammaExpIdx_%d_%d = 0x%x", &i, &j, &temp)) {
				para->expIdxCtrlPt[2 * i + j] = temp;
				continue;
			}
			if (sscanf(str, "gammaNode_%d_%d = 0x%x", &i, &j, &temp)) {
				para->node[i][j] = temp;
				continue;
			}
		}
		fclose(fp);
	} else {
		printf("open gamma profile failed, create a new profile\n");
		fp = fopen(SETTING_FILE_PATHNAME, "wb");
		if (fp != NULL) {
			for (i = 0; i < CURVE_NUM - 1; i++) {
				para->expIdxCtrlPt[i * 2] = (i * 2) * 0x800 / (2 * CURVE_NUM);
				sprintf(str, "gammaExpIdx_%d_0 = 0x%x\n", i, para->expIdxCtrlPt[i * 2]);
				fputs(str, fp);

				para->expIdxCtrlPt[i * 2 + 1] = (i * 2 + 1) * 0x800 / (2 * CURVE_NUM);
				sprintf(str, "gammaExpIdx_%d_1 = 0x%x\n", i, para->expIdxCtrlPt[i * 2 + 1]);
				fputs(str, fp);
			}
			for (i = 0; i < CURVE_NUM; i++) {
				for (j = 0; j < CURVE_CP_NUM; j++) {
					para->node[i][j] = (j + 1) * 4;
					sprintf(str, "gammaNode_%d_%03d = 0x%x\n", i, j, para->node[i][j]);
					fputs(str, fp);
				}
			}
			fclose(fp);
		}
	}
	return ISP_UNIT_RESULT_OK;
}

isp_unit_result_t isp_unit_rgbgamma(
		unsigned char *buf,
		isp_3a_statistic_t *isp_3a_buf)
{
	unsigned int i = 0, j = 0, bufIndex = 0, pageSize = 0;
	unsigned int expIdx = 0, index = 0;
	unsigned int inTemp = 0, outTemp = 0;
	unsigned short tempNode[CURVE_CP_NUM];
	unsigned short imageWidth = isp_3a_buf->imageWidth;
	unsigned short imageHeight = isp_3a_buf->imageHeight;

	load_setting(&para);
	expIdx = isp_3a_buf->curExp * isp_3a_buf->curGain;

	for (i = 0; i < CURVE_NUM - 1; i++) {
		if ((expIdx >= para.expIdxCtrlPt[2 * i]) &&
				(expIdx < para.expIdxCtrlPt[2 * i + 1])) {
			for (j = 0; j < CURVE_CP_NUM; j++) {
				tempNode[j] =
					interp1(para.expIdxCtrlPt[2 * i], expIdx, para.expIdxCtrlPt[2 * i + 1],
							para.node[i][j], para.node[i + 1][j]);
			}
		}
	}
	for (i = 0; i < CURVE_NUM - 2; i++) {
		if ((expIdx >= para.expIdxCtrlPt[2 * i + 1]) &&
				(expIdx < para.expIdxCtrlPt[2 * (i + 1)])) {
			for (j = 0; j < CURVE_CP_NUM; j++) {
				tempNode[j] = para.node[i + 1][j];
			}
		}
	}
	if (expIdx < para.expIdxCtrlPt[0]) {
		for (j = 0; j < CURVE_CP_NUM; j++) {
			tempNode[j] = para.node[0][j];
		}
	}
	if (expIdx >= para.expIdxCtrlPt[2 * (CURVE_NUM - 1) - 1]) {
		for (j = 0; j < CURVE_CP_NUM; j++) {
			tempNode[j] = para.node[CURVE_NUM - 1][j];
		}
	}

	for (i = 0; i < imageWidth * imageHeight * 6; i += 2) {
		inTemp = buf[i] + buf[i + 1] * 256;
		if ((inTemp <1020) && (inTemp >= 4)) {
			index = inTemp / 4;
			outTemp = interp1(4 * index, inTemp, 4 * (index + 1),
					tempNode[index], tempNode[index + 1]);
		} else if (inTemp < 4) {
			outTemp = interp1(0, inTemp, 4, 0, tempNode[1]);
		} else if (inTemp >= 1020) {
			outTemp = interp1(1020, inTemp, 1023, tempNode[255], 1023);
		}

		buf[i] = outTemp & 0xff;
		buf[i + 1] = outTemp / 256;
	}

	return ISP_UNIT_RESULT_OK;
}
