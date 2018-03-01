#include "isp_unit_ccm.h"
#include <stdio.h>

#define SETTING_FILE_PATHNAME	"./profiles/ccm_profile.txt"

static ccm_parameters_t para;

static isp_unit_result_t load_setting(ccm_parameters_t *para)
{
	char str[128];
	unsigned int i = 0, j = 0;
	unsigned int temp = 0;
	FILE *fp = fopen(SETTING_FILE_PATHNAME, "rb");
	if (fp != NULL) {
		while (!feof(fp)) {
			fgets(str, 128, fp);
			if (sscanf(str, "ccmCT_%d = 0x%x", &i, &temp)) {
				para->ctcp[i] = temp;
				continue;
			}
			if (sscanf(str, "ccmCoef_%d_%d = 0x%x", &i, &j, &temp)) {
				para->ccmCoef[i][j] = temp;
				continue;
			}
			if (sscanf(str, "ccmSat = 0x%x", &temp)) {
				para->sat = temp;
				continue;
			}
		}
		fclose(fp);

	} else {
		printf("open ccm profile failed, create a new profile\n");
		fp = fopen(SETTING_FILE_PATHNAME, "wb");
		if (fp != NULL) {
			for (i = 0; i < CCM_TABLE_NUM; i++) {
				para->ctcp[i] = 0x40 * (i + 1) / CCM_TABLE_NUM;
				sprintf(str, "ccmCT_%d = 0x%x\n", i, para->ctcp[i]);
				fputs(str, fp);
			}
			for (i = 0; i < CCM_TABLE_NUM; i++) {
				for (j = 0; j < 9; j++) {
					para->ccmCoef[i][j] = ((j==0) || (j==4) || (j==8)) ? 256 : 0;
					sprintf(str, "ccmCoef_%d_%d = 0x%x\n", i, j, para->ccmCoef[i][j]);
					fputs(str, fp);
				}
			}
			para->sat = 0x80;
			sprintf(str, "ccmSat = 0x%x\n", para->sat);
			fputs(str, fp);

			fclose(fp);
		}
	}
}

isp_unit_result_t isp_unit_ccm(
		unsigned char *buf,
		isp_3a_statistic_t *isp_3a_buf)
{
	unsigned int i = 0, j = 0, bufIndex = 0, pageSize = 0;
	int ccmLocal[9], ccmLocalBoost[9], m1, m2, m3, n1, n2, n3;
	int inB, inG, inR, outB, outG, outR;
	unsigned int ct = 0;
	unsigned short imageWidth = isp_3a_buf->imageWidth;
	unsigned short imageHeight = isp_3a_buf->imageHeight;

	load_setting(&para);
	ct = 0x80 * isp_3a_buf->curRGain / isp_3a_buf->curBGain;


	for (i = 0; i < CCM_TABLE_NUM - 1; i++) {
		if ((ct >= para.ctcp[i]) && (ct < para.ctcp[CCM_TABLE_NUM - 1])) {
			for (j = 0; j < 9; j++) {
				ccmLocal[j] = interp1(para.ctcp[i], ct, para.ctcp[i + 1],
						para.ccmCoef[i][j], para.ccmCoef[i][j + 1]);
			}
		}
	}
	if (ct < para.ctcp[0]) {
		for (j = 0; j < 9; j++) {
			ccmLocal[j] = para.ccmCoef[0][j];
		}
	}
	if (ct >= para.ctcp[CCM_TABLE_NUM - 1]) {
		for (j = 0; j < 9; j++) {
			ccmLocal[j] = para.ccmCoef[CCM_TABLE_NUM - 1][j];
		}
	}

	for (i = 0; i < 9; i++) {
		m1 = (i < 3) ? (384 + 640 * para.sat / 0x80) :
			((640 - 640 * para.sat / 0x80) / 2);
		m2 = ((i >= 3) && (i < 6)) ? (384 + 640 * para.sat / 0x80) :
			((640 - 640 * para.sat / 0x80) / 2);
		m3 = (i >= 6) ? (384 + 640 * para.sat / 0x80) :
			((640 - 640 * para.sat / 0x80) / 2);
		n1 = ccmLocal[i%3];
		n2 = ccmLocal[(i%3) + 3];
		n3 = ccmLocal[(i%3) + 6];
		ccmLocalBoost[i] = (m1*n1 + m2*n2 + m3*n3) / 1024;
	}

	pageSize = imageWidth * imageHeight * 2;
	for (i = 0; i < imageHeight; i++) {
		for (j = 0; j < imageWidth; j++) {
			bufIndex = (i * imageWidth + j) * 2;
			inB = buf[bufIndex] + buf[bufIndex + 1] * 256;
			inG = buf[pageSize + bufIndex] + buf[pageSize + bufIndex + 1] * 256;
			inR = buf[pageSize * 2 + bufIndex] + buf[pageSize * 2 + bufIndex + 1] * 256;

			outB = (ccmLocalBoost[0] * inB +
					ccmLocalBoost[1] * inG +
					ccmLocalBoost[2] * inR) / 256;
			outG = (ccmLocalBoost[3] * inB +
					ccmLocalBoost[4] * inG +
					ccmLocalBoost[5] * inR) / 256;
			outR = (ccmLocalBoost[6] * inB +
					ccmLocalBoost[7] * inG +
					ccmLocalBoost[8] * inR) / 256;
			outB = outB > 0 ? outB : 0;
			outG = outG > 0 ? outG : 0;
			outR = outR > 0 ? outR : 0;
			outB = outB <= 1023 ? outB : 1023;
			outG = outG <= 1023 ? outG : 1023;
			outR = outR <= 1023 ? outR : 1023;

			buf[bufIndex] = outB & 0xff;
			buf[bufIndex + 1] = (outB / 256) & 0xff;
			buf[pageSize + bufIndex] = outG & 0xff;
			buf[pageSize + bufIndex + 1] = (outG / 256) & 0xff;
			buf[pageSize * 2 + bufIndex] = outR & 0xff;
			buf[pageSize * 2 + bufIndex + 1] = (outR / 256) & 0xff;
		}
	}

	return ISP_UNIT_RESULT_OK;
}
