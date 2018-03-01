#include "isp_unit_apply_wb.h"
#include <stdio.h>

#define SETTING_FILE_PATHNAME	"./profiles/apply_wb_profile.txt"

static apply_wb_parameters_t para;

static isp_unit_result_t load_setting(apply_wb_parameters_t *para)
{
	char str[128], loop_format[128];
	unsigned int i = 0, j = 0;
	unsigned int temp = 0;
	FILE *fp = fopen(SETTING_FILE_PATHNAME, "rb");
	if (fp != NULL) {
		while (!feof(fp)) {
			fgets(str, 128, fp);
			if (sscanf(str, "wbShiftCT_%d = 0x%x", &i, &temp)) {
				para->ctCtrlPoint[i] = temp;
				continue;
			}
			if (sscanf(str, "wbShiftBR_%d = 0x%x", &i, &temp)) {
				para->brCtrlPoint[i] = temp;
				continue;
			}
			if (sscanf(str, "bShift_%d_%d = 0x%x", &i, &j, &temp)) {
				para->wbShiftGain[i][j].bGainShift = temp;
				continue;
			}
			if (sscanf(str, "gShift_%d_%d = 0x%x", &i, &j, &temp)) {
				para->wbShiftGain[i][j].gGainShift = temp;
				continue;
			}
			if (sscanf(str, "rShift_%d_%d = 0x%x", &i, &j, &temp)) {
				para->wbShiftGain[i][j].rGainShift = temp;
				continue;
			}
		}
		fclose(fp);
	} else {
		printf("open apply wb gain profile failed, create a new profile\n");
		fp = fopen(SETTING_FILE_PATHNAME, "wb");
		if (fp != NULL) {
			for (i = 0; i < WB_SHIFT_CTCP_NUM; i++) {
				para->ctCtrlPoint[i] = 0x80 * (i+1) / WB_SHIFT_CTCP_NUM;
				sprintf(str, "wbShiftCT_%d = 0x%x\n", i, para->ctCtrlPoint[i]);
				fputs(str, fp);
			}
			for (i = 0; i < WB_SHIFT_BRCP_NUM; i++) {
				para->brCtrlPoint[i] = 0x800 * (i+1) / WB_SHIFT_BRCP_NUM;
				sprintf(str, "wbShiftBR_%d = 0x%lx\n", i, para->brCtrlPoint[i]);
				fputs(str, fp);
			}
			for (i = 0; i < WB_SHIFT_CTCP_NUM; i++) {
				for (j = 0; j < WB_SHIFT_BRCP_NUM; j++) {
					para->wbShiftGain[0][0].bGainShift = 0x80;
					sprintf(str, "bShift_%d_%d = 0x%x\n", i, j, para->wbShiftGain[0][0].bGainShift);
					fputs(str, fp);
					para->wbShiftGain[0][0].gGainShift = 0x80;
					sprintf(str, "gShift_%d_%d = 0x%x\n", i, j, para->wbShiftGain[0][0].gGainShift);
					fputs(str, fp);
					para->wbShiftGain[0][0].rGainShift = 0x80;
					sprintf(str, "rShift_%d_%d = 0x%x\n", i, j, para->wbShiftGain[0][0].rGainShift);
					fputs(str, fp);
				}
			}
			fclose(fp);
		}
	}

	return ISP_UNIT_RESULT_OK;
}

isp_unit_result_t isp_unit_apply_wb(
		unsigned char *buf,
		isp_3a_statistic_t *isp_3a_buf)
{
	int i = 0, j = 0, x, y, bufIndex = 0;
	unsigned long expIdx = 0, gain = 0, exp = 0;
	unsigned int ct = 0;
	struct wbShiftNode arrShiftNode[WB_SHIFT_CTCP_NUM], shiftNode;
	unsigned int bGainApply = 0, gGainApply = 0, rGainApply = 0, temp;
	unsigned int blc = isp_3a_buf->nBLC;
	unsigned short imageWidth = isp_3a_buf->imageWidth;
	unsigned short imageHeight = isp_3a_buf->imageHeight;

	load_setting(&para);
	gain = isp_3a_buf->curGain;
	exp = isp_3a_buf->curExp;
	expIdx = gain * exp;
	ct = 0x80 * isp_3a_buf->curRGain / isp_3a_buf->curBGain;

	for (i = 0; i <WB_SHIFT_BRCP_NUM - 1; i++) {
		if ((expIdx >= para.brCtrlPoint[i]) &&
				(expIdx < para.brCtrlPoint[i + 1])) {
			for (j = 0; j < WB_SHIFT_CTCP_NUM; j++) {
				arrShiftNode[j].bGainShift =
					interp1(para.brCtrlPoint[i], expIdx, para.brCtrlPoint[i+1],
							para.wbShiftGain[j][i].bGainShift,
							para.wbShiftGain[j][i + 1].bGainShift);
				arrShiftNode[j].gGainShift =
					interp1(para.brCtrlPoint[i], expIdx, para.brCtrlPoint[i+1],
							para.wbShiftGain[j][i].gGainShift,
							para.wbShiftGain[j][i + 1].gGainShift);
				arrShiftNode[j].rGainShift =
					interp1(para.brCtrlPoint[i], expIdx, para.brCtrlPoint[i+1],
							para.wbShiftGain[j][i].rGainShift,
							para.wbShiftGain[j][i + 1].rGainShift);
			}
			continue;
		}
	}
	if (expIdx < para.brCtrlPoint[0]) {
		for (j = 0; j < WB_SHIFT_CTCP_NUM; j++) {
			arrShiftNode[j].bGainShift = para.wbShiftGain[j][0].bGainShift;
			arrShiftNode[j].gGainShift = para.wbShiftGain[j][0].gGainShift;
			arrShiftNode[j].rGainShift = para.wbShiftGain[j][0].rGainShift;
		}
	}
	if (expIdx >= para.brCtrlPoint[WB_SHIFT_BRCP_NUM - 1]) {
		for (j = 0; j < WB_SHIFT_CTCP_NUM; j++) {
			arrShiftNode[j].bGainShift =
				para.wbShiftGain[j][WB_SHIFT_BRCP_NUM - 1].bGainShift;
			arrShiftNode[j].gGainShift =
				para.wbShiftGain[j][WB_SHIFT_BRCP_NUM - 1].gGainShift;
			arrShiftNode[j].rGainShift =
				para.wbShiftGain[j][WB_SHIFT_BRCP_NUM - 1].rGainShift;
		}
	}

	for (i = 0; i < WB_SHIFT_CTCP_NUM - 1; i++) {
		if ((ct >= para.ctCtrlPoint[i]) &&
				(ct < para.ctCtrlPoint[i + 1])) {
			shiftNode.bGainShift =
				interp1(para.ctCtrlPoint[i], ct, para.ctCtrlPoint[i + 1],
						arrShiftNode[i].bGainShift,
						arrShiftNode[i + 1].bGainShift);
			shiftNode.gGainShift =
				interp1(para.ctCtrlPoint[i], ct, para.ctCtrlPoint[i + 1],
						arrShiftNode[i].gGainShift,
						arrShiftNode[i + 1].gGainShift);
			shiftNode.rGainShift =
				interp1(para.ctCtrlPoint[i], ct, para.ctCtrlPoint[i + 1],
						arrShiftNode[i].rGainShift,
						arrShiftNode[i + 1].rGainShift);
		}
	}
	if (ct < para.ctCtrlPoint[0]) {
		shiftNode.bGainShift = arrShiftNode[0].bGainShift;
		shiftNode.gGainShift = arrShiftNode[0].gGainShift;
		shiftNode.rGainShift = arrShiftNode[0].rGainShift;
	}
	if (ct >= para.ctCtrlPoint[WB_SHIFT_CTCP_NUM - 1]) {
		shiftNode.bGainShift = arrShiftNode[WB_SHIFT_CTCP_NUM - 1].bGainShift;
		shiftNode.gGainShift = arrShiftNode[WB_SHIFT_CTCP_NUM - 1].gGainShift;
		shiftNode.rGainShift = arrShiftNode[WB_SHIFT_CTCP_NUM - 1].rGainShift;
	}

	bGainApply = isp_3a_buf->curBGain * shiftNode.bGainShift / 0x80;
	gGainApply = isp_3a_buf->curGGain * shiftNode.gGainShift / 0x80;
	rGainApply = isp_3a_buf->curRGain * shiftNode.rGainShift / 0x80;
	temp = min(bGainApply, min(gGainApply, rGainApply));
	bGainApply = bGainApply * 0x80 / temp;
	gGainApply = gGainApply * 0x80 / temp;
	rGainApply = rGainApply * 0x80 / temp;

	for (i = 0; i < imageHeight; i += 2) {
		for (j = 0; j < imageWidth; j += 2) {
			bufIndex = (i * imageWidth + j) * 2;
			temp = buf[bufIndex] + buf[bufIndex + 1] * 256;
			temp = temp * bGainApply / 0x80;
            temp = clip(temp, 0, 1023);
			buf[bufIndex] = temp & 0xff;
			buf[bufIndex + 1] = temp / 256;

			bufIndex = (i * imageWidth + j + 1) * 2;
			temp = buf[bufIndex] + buf[bufIndex + 1] * 256;
			temp = temp * gGainApply / 0x80;
            temp = clip(temp, 0, 1023);
			buf[bufIndex] = temp & 0xff;
			buf[bufIndex + 1] = temp / 256;

			bufIndex = ((i + 1) * imageWidth + j) * 2;
			temp = buf[bufIndex] + buf[bufIndex + 1] * 256;
			temp = temp * gGainApply / 0x80;
            temp = clip(temp, 0, 1023);
			buf[bufIndex] = temp & 0xff;
			buf[bufIndex + 1] = temp / 256;

			bufIndex = ((i + 1) * imageWidth + j + 1) * 2;
			temp = buf[bufIndex] + buf[bufIndex + 1] * 256;
			temp = temp * rGainApply / 0x80;
            temp = clip(temp, 0, 1023);
			buf[bufIndex] = temp & 0xff;
			buf[bufIndex + 1] = temp / 256;
		}
	}
	return ISP_UNIT_RESULT_OK;
}
