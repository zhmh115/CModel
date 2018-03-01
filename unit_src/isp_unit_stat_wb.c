#include "isp_unit_stat_wb.h"
#include <stdio.h>

#define SETTING_FILE_PATHNAME	"./profiles/stat_wb_profile.txt"

static stat_wb_parameters_t para;

static isp_unit_result_t load_setting(stat_wb_parameters_t *para)
{
	char str[128];
	unsigned int i = 0, j = 0;
	unsigned int temp = 0;
	FILE *fp = fopen(SETTING_FILE_PATHNAME, "rb");
	if (fp != NULL) {
		while (!feof(fp)) {
			fgets(str, 128, fp);
			if (sscanf(str, "xOffset = 0x%x", &para->xOffset))
				continue;
			if (sscanf(str, "yOffset = 0x%x", &para->yOffset))
				continue;
			if (sscanf(str, "xScale = 0x%x", &para->xScale))
				continue;
			if (sscanf(str, "yScale = 0x%x", &para->yScale))
				continue;
			if (sscanf(str, "wbMap_%d = 0x%x", &i, &temp)) {
				para->wbMap[i] = temp;
				continue;
			}
		}
		fclose(fp);
		fp = NULL;
	} else {
		printf("open stat wb profile failed, create a new profile\n");
		fp = fopen(SETTING_FILE_PATHNAME, "wb");
		if (fp != NULL) {
			para->xOffset = 0xde7;
			sprintf(str, "xOffset = 0x%04x\n", para->xOffset);
			fputs(str, fp);
			para->yOffset = 0xe76;
			sprintf(str, "yOffset = 0x%04x\n", para->yOffset);
			fputs(str, fp);
			para->xScale = 0x1c;
			sprintf(str, "xScale = 0x%02x\n", para->xScale);
			fputs(str, fp);
			para->yScale = 0x19;
			sprintf(str, "yScale = 0x%02x\n", para->yScale);
			fputs(str, fp);
			for (i = 0; i < 256; i++) {
				para->wbMap[i] = 0xf;
				sprintf(str, "wbMap_%03d = 0x%x\n", i, para->wbMap[i]);
				fputs(str, fp);
			}

			fclose(fp);
			fp = NULL;
		}
	}
}

isp_unit_result_t isp_unit_stat_wb(
		unsigned char *buf,
		isp_3a_statistic_t *isp_3a_buf)
{
	int i = 0, j = 0;
	int bufIndexB = 0, bufIndexGB = 0, bufIndexGR = 0, bufIndexR = 0;
	unsigned int tempB, tempGB, tempGR, tempR;
	int localBGain, localGGain = 0x80, localRGain;
	int outputBGain, outputGGain, outputRGain, minWbGain = 10000;
	float xPos, yPos;
	int xIndex, yIndex;
	int xOffset, yOffset, xScale, yScale;
	unsigned char weight;
	unsigned long long sumB = 0, sumGB = 0, sumGR = 0, sumR = 0;
	unsigned short imageWidth = isp_3a_buf->imageWidth;
	unsigned short imageHeight = isp_3a_buf->imageHeight;
	load_setting(&para);

	xOffset = (para.xOffset > 2048) ? (para.xOffset - 4096) : para.xOffset;
	yOffset = (para.yOffset > 2048) ? (para.yOffset - 4096) : para.yOffset;
	xScale = para.xScale;
	yScale = para.yScale;

	for (i = 0; i < imageHeight; i += 2) {
		for (j = 0; j < imageWidth; j += 2) {
			bufIndexB = (i * imageWidth + j) * 2;
			bufIndexGB = (i * imageWidth + j + 1) * 2;
			bufIndexGR = ((i + 1) * imageWidth + j) * 2;
			bufIndexR = ((i + 1) * imageWidth + j + 1) * 2;
			tempB = buf[bufIndexB] + buf[bufIndexB + 1] * 256 + 1;
			tempGB = buf[bufIndexGB] + buf[bufIndexGB + 1] * 256 + 1;
			tempGR = buf[bufIndexGR] + buf[bufIndexGR + 1] * 256 + 1;
			tempR = buf[bufIndexR] + buf[bufIndexR + 1] * 256 + 1;
			localBGain = 0x80 * tempGB / tempB;
			localRGain = 0x80 * tempGR / tempR;
			xPos = 256.0 * (2 * log2((float)localGGain) - 
					log2((float)localBGain) - log2((float)localRGain));
			yPos = 256.0 * (log2(localRGain) - log2(localBGain));
			xPos = (xPos - xOffset) * xScale / 64 / 16;
			yPos = (yPos - yOffset) * yScale / 64 / 16;

			xIndex = (int)(xPos + 0.5f);
			yIndex = (int)(yPos + 0.5f);
			xIndex = (xIndex >= 1) ? xIndex : 0;
			xIndex = (xIndex <= 16) ? xIndex : 0;
			yIndex = (yIndex >= 1) ? yIndex : 0;
			yIndex = (yIndex <= 16) ? yIndex : 0;

			if ((xIndex != 0) && (yIndex != 0))
				weight = para.wbMap[16 * (yIndex - 1) + (xIndex - 1)];
			else
				weight = 0;
			sumB += (int)((pow(2, (float)weight / 16) - 1) * (tempB));
			sumGB += (int)((pow(2, (float)weight / 16) - 1) * (tempGB));
			sumGR += (int)((pow(2, (float)weight / 16) - 1) * (tempGR));
			sumR += (int)((pow(2, (float)weight / 16) - 1) * (tempR));

		}
	}

	outputBGain = 0x80 * sumGB / sumB;
	if (minWbGain > outputBGain) minWbGain = outputBGain;
	outputGGain = 0x80;
	if (minWbGain > outputGGain) minWbGain = outputGGain;
	outputRGain = 0x80 * sumGR / sumR;
	if (minWbGain > outputRGain) minWbGain = outputRGain;

	isp_3a_buf->BGain = 0x80 * outputBGain / minWbGain;
	isp_3a_buf->GGain = 0x80 * outputGGain / minWbGain;
	isp_3a_buf->RGain = 0x80 * outputRGain / minWbGain;

	return ISP_UNIT_RESULT_OK;
}
