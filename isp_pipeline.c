#include "defination.h"
#include "isp_pipeline.h"

#include <stdio.h>

static isp_3a_statistic_t isp_3a_buf;

int isp_unit_sequence(unsigned char *rawbuf)
{
	int i = 0;
	int num = 0;
	num = sizeof(isp_unit_process) / sizeof(isp_unit_process[0]);
	for (i = 0; i < num; i++) {
		if ((*isp_unit_process[i])(rawbuf, &isp_3a_buf) != ISP_UNIT_RESULT_OK) {
			printf("error in the %dth pipeline unit!\n", i);
			break;
		}
	}

	return 0;
}

int main(int argc, char *argv[])
{
	int raw_size = 0, i;
	unsigned char *rawbuf = NULL;
	FILE *fp = NULL;
	unsigned char cRawType[3];
	unsigned char cRawSize[4];
	unsigned char cExpInfo[6];
	unsigned char cWbInfo[6];

	if (argc < 2) {
		printf("usage:\n");
		printf("./isp_pipeline filename.vrf\n");
	} else {
		fp = fopen(argv[1], "rb");
		if (fp == NULL) {
			printf("open raw file %s error\n", argv[1]);
			return -1;
		}

		fseek(fp, -3, SEEK_END);
		fscanf(fp, "%s", cRawType);
		if (strcmp(cRawType, "VRF")) {
			printf("wrong raw type\n");
			return -1;
		}
		fseek(fp, -128, SEEK_END);
		long raw_buf_size = ftell(fp);
		fread(cRawSize, sizeof(unsigned char), 4, fp);
		int width = cRawSize[0] + cRawSize[1]*256;
		int height = cRawSize[2] + cRawSize[3]*256;
		if (raw_buf_size != 2 * (long)width * (long)height) {	
			printf("width = %d, height = %d, does not match with the vrf file\n", width, height);
		}
		fread(cExpInfo, sizeof(unsigned char), 6, fp);
		isp_3a_buf.curGain = cExpInfo[0] + cExpInfo[1] * 256;
		isp_3a_buf.curExp = cExpInfo[2] + cExpInfo[3] * 256;
		isp_3a_buf.curVTS = cExpInfo[4] + cExpInfo[5] * 256;

		fread(cWbInfo, sizeof(unsigned char), 6, fp);
		isp_3a_buf.curBGain = cWbInfo[0] + cWbInfo[1] * 256;
		isp_3a_buf.curGGain = cWbInfo[2] + cWbInfo[3] * 256;
		isp_3a_buf.curRGain = cWbInfo[4] + cWbInfo[5] * 256;

		fseek(fp, -128 + 24, SEEK_END);
		fread(&isp_3a_buf.nBLC, sizeof(unsigned char), 1, fp);

		isp_3a_buf.imageWidth = width;
		isp_3a_buf.imageHeight = height;
		rawbuf = (unsigned char *)malloc(3 * raw_buf_size * sizeof(unsigned char));
		fseek(fp, 0, SEEK_SET);
		fread(rawbuf, sizeof(unsigned char), raw_buf_size, fp);

		if (fp != NULL)
			fclose(fp);
		if (rawbuf != NULL) {
			isp_unit_sequence(rawbuf);
		}
	}
	if (rawbuf != NULL) {
		free(rawbuf);
		rawbuf = NULL;
	}

	return 0;
}
