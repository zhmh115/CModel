#include "isp_unit_rawstretch.h"
#include <stdio.h>

#define SETTING_FILE_PATHNAME	"./profiles/rawstretch_profile.txt"

static rawstretch_parameters_t para;

static isp_unit_result_t load_setting(rawstretch_parameters_t *para)
{
    char str[128];
    FILE *fp = fopen(SETTING_FILE_PATHNAME, "rb");
    if (fp != NULL) {
        while (!feof(fp)) {
            fgets(str, 128, fp);
            if (sscanf(str, "darkPer = %hd", &para->darkPer)) continue;
            if (sscanf(str, "minLowLevel = %hd", &para->minLowLevel)) continue;
            if (sscanf(str, "maxLowLevel = %hd", &para->maxLowLevel)) continue;
            if (sscanf(str, "saturationPer = %hd", &para->saturationPer)) continue;
            if (sscanf(str, "minHighLevel = %hd", &para->minHighLevel)) continue;
            if (sscanf(str, "maxHighLevel = %hd", &para->maxHighLevel)) continue;
        }
        fclose(fp);
        fp = NULL;
    } else {
        printf("open rawstretch profile failed, create a new profile\n");
        fp = fopen(SETTING_FILE_PATHNAME, "wb");
        if (fp != NULL) {
            para->darkPer = 1;
            sprintf(str, "darkPer = %hd\n", para->darkPer);
            fputs(str, fp);
            para->minLowLevel = 0;
            sprintf(str, "minLowLevel = %hd\n", para->minLowLevel);
            fputs(str, fp);
            para->maxLowLevel = 0x10;
            sprintf(str, "maxLowLevel = %hd\n", para->maxLowLevel);
            fputs(str, fp);
            para->saturationPer = 1;
            sprintf(str, "saturationPer = %hd\n", para->saturationPer);
            fputs(str, fp);
            para->minHighLevel = 0x3c0;
            sprintf(str, "minHighLevel = %hd\n", para->minHighLevel);
            fputs(str, fp);
            para->maxHighLevel = 0x3ff;
            sprintf(str, "maxHighLevel = %hd\n", para->maxHighLevel);
            fputs(str, fp);
            fclose(fp);
            fp = NULL;
        }
    }
}

isp_unit_result_t isp_unit_rawstretch(
		unsigned char *buf,
		isp_3a_statistic_t *isp_3a_buf)
{
    unsigned int i = 0, j = 0, temp = 0, bufIndex = 0;
    unsigned short stretchGain = 0, stretchOffset = 0;
    unsigned short imageWidth = isp_3a_buf->imageWidth;
    unsigned short imageHeight = isp_3a_buf->imageHeight;
    unsigned short *hist = (unsigned short *)malloc(
            1024 * sizeof(unsigned short));
    load_setting(&para);
    memset(hist, 0, sizeof(hist) * sizeof(unsigned short));
    for (i = 0; i < imageHeight; i++) {
        for (j = 0; j < imageWidth; j++) {
            bufIndex = (i * imageWidth + j) * 2;
            temp = buf[bufIndex] + buf[bufIndex + 1] * 256;
            hist[temp]++;
        }
    }
    unsigned short highBoundary, lowBoundary;
    unsigned int tempNum = 0;
    for (lowBoundary = 0; lowBoundary < 1023; lowBoundary++) {
        tempNum += hist[lowBoundary];
        if (tempNum >= para.darkPer * imageWidth * imageHeight / 1024)
            break;
    }
    tempNum = 0;
    for (highBoundary = 1023; highBoundary > lowBoundary; highBoundary--) {
        tempNum += hist[highBoundary];
        if (tempNum >= para.saturationPer * imageWidth * imageHeight / 1024)
            break;
    }
    unsigned short lowClip, highClip;
    if (lowBoundary < para.minLowLevel)
        lowClip = para.minLowLevel;
    else if (lowBoundary < para.maxLowLevel)
        lowClip = lowBoundary;
    else
        lowClip = para.maxLowLevel;

    if (highBoundary < para.minHighLevel)
        highClip = para.minHighLevel;
    else if (highBoundary < para.maxHighLevel)
        highClip = highBoundary;
    else
        highClip = para.maxHighLevel;
    isp_3a_buf->stretchOffset = lowClip;
    /*the first 1024 is brightness range, thd second 1024 is nomorlized factor*/
    isp_3a_buf->stretchGain = 1024 * 1024 / (highClip - lowClip);
    for (i = 0; i < imageHeight; i++) {
        for (j = 0; j < imageWidth; j++) {
            bufIndex = (i * imageWidth + j) * 2;
            temp = buf[bufIndex] + buf[bufIndex + 1] * 256;
            temp = (temp > isp_3a_buf->stretchOffset) ?
                (temp - isp_3a_buf->stretchOffset) : 0;
            temp = ((int)temp - (int)isp_3a_buf->stretchOffset) *
                (int)isp_3a_buf->stretchGain / 1024;
            temp = clip(temp, 0, 1023);
            buf[bufIndex] = temp % 256;
            buf[bufIndex + 1] = temp / 256;
        }
    }

	return ISP_UNIT_RESULT_OK;
}
