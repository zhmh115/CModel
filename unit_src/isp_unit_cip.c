#include "isp_unit_cip.h"
#include <stdio.h>

#define SETTING_FILE_PATHNAME	"./profiles/cip_profile.txt"

#define GETBUF(i, j) \
	(buf[((i) * imageWidth + j) * 2] + buf[((i) * imageWidth + j) * 2 + 1] * 256)

isp_unit_result_t isp_unit_cip(
		unsigned char *buf,
		isp_3a_statistic_t *isp_3a_buf)
{
	int i = 0, j = 0;
	int bufIndex, bufIndexL, bufIndexLT, bufIndexT, bufIndexRT, bufIndexR, bufIndexRB, bufIndexB, bufIndexLB;
	unsigned short tempB = 0, tempG = 0, tempR = 0;
	unsigned short valL, valLT, valT, valRT, valR, valRB, valB, valLB;
	unsigned int imageWidth = isp_3a_buf->imageWidth;
	unsigned int imageHeight = isp_3a_buf->imageHeight;
	unsigned int pageSize = imageWidth * imageHeight * 2;
	unsigned char *middleBuf =
		(unsigned char *)malloc(6 * imageWidth * imageHeight);

	for (i = 0; i < imageHeight; i += 1) {
		for (j = 0; j < imageWidth; j += 1) {
			if ((i >= 1) && (i < imageHeight - 1) &&
					(j >= 1) && (j < imageWidth - 1)) {
				bufIndex = (i * imageWidth + j) * 2;
				bufIndexL = (i * imageWidth + j - 1) * 2;
				bufIndexLT = ((i - 1) * imageWidth + j - 1) * 2;
				bufIndexT = ((i - 1) * imageWidth + j) * 2;
				bufIndexRT = ((i - 1) * imageWidth + j + 1) * 2;
				bufIndexR = (i * imageWidth + j + 1) * 2;
				bufIndexRB = ((i + 1) * imageWidth + j + 1) * 2;
				bufIndexB = ((i + 1) * imageWidth + j) * 2;
				bufIndexLB = ((i + 1) * imageWidth + j - 1) * 2;
				if (!(i%2) && !(j%2)) {
					/* for B position */
					/* for B channel */
					middleBuf[bufIndex] = buf[bufIndex];
					middleBuf[bufIndex + 1] = buf[bufIndex + 1];
					/* for G channel */
					tempG = (GETBUF(i, j - 1) + GETBUF(i - 1, j) +
							GETBUF(i, j + 1) + GETBUF(i + 1, j)) / 4;
					middleBuf[pageSize + bufIndex] = tempG & 0xff;
					middleBuf[pageSize + bufIndex + 1] = tempG / 256;
					/* for R channel */
					tempR = (GETBUF(i - 1, j - 1) + GETBUF(i - 1, j + 1) +
							GETBUF(i + 1, j - 1) + GETBUF(i + 1, j + 1)) / 4;

					middleBuf[pageSize * 2 + bufIndex] = tempR & 0xff;
					middleBuf[pageSize * 2 + bufIndex + 1] = tempR / 256;
				} else if (!(i%2) && (j%2)) {
					/* for GB position */
					/* for B channel */
					tempB = (GETBUF(i, j - 1) + GETBUF(i, j + 1)) / 2;
					middleBuf[bufIndex] = tempB & 0xff;
					middleBuf[bufIndex + 1] = tempB / 256;

					/* for G channel */
					middleBuf[pageSize + bufIndex] = buf[bufIndex];
					middleBuf[pageSize + bufIndex + 1] = buf[bufIndex + 1];
					/* for R channel */
					tempR = (GETBUF(i - 1, j) + GETBUF(i + 1, j)) / 2;
					middleBuf[pageSize * 2 + bufIndex] = tempR & 0xff;
					middleBuf[pageSize * 2 + bufIndex + 1] = tempR / 256;
				} else if ((i%2) && !(j%2)) {
					/* for GR position */
					/* for B channel */
					tempB = (GETBUF(i - 1, j) + GETBUF(i + 1, j)) / 2;
					middleBuf[bufIndex] = tempB & 0xff;
					middleBuf[bufIndex + 1] = tempB / 256;

					/* for G channel */
					middleBuf[pageSize + bufIndex] = buf[bufIndex];
					middleBuf[pageSize + bufIndex + 1] = buf[bufIndex + 1];
					/* for R channel */
					tempR = (GETBUF(i, j - 1) + GETBUF(i, j + 1)) / 2;
					middleBuf[pageSize * 2 + bufIndex] = tempR & 0xff;
					middleBuf[pageSize * 2 + bufIndex + 1] = tempR / 256;
				} else if ((i%2) && (j%2)) {
					/* for R position */
					/* for B channel */
					tempB = (GETBUF(i - 1, j - 1) + GETBUF(i - 1, j + 1) +
							GETBUF(i + 1, j + 1) + GETBUF(i + 1, j - 1)) / 4;
					middleBuf[bufIndex] = tempB & 0xff;
					middleBuf[bufIndex + 1] = tempB / 256;

					/* for G channel */
					tempG = (GETBUF(i, j - 1) + GETBUF(i - 1, j) +
							GETBUF(i, j + 1) + GETBUF(i + 1, j)) / 4;
					middleBuf[pageSize + bufIndex] = tempG & 0xff;
					middleBuf[pageSize + bufIndex + 1] = tempG / 256;

					/* for R channel */
					middleBuf[pageSize * 2 + bufIndex] = buf[bufIndex];
					middleBuf[pageSize * 2 + bufIndex + 1] = buf[bufIndex + 1];
				}
			} else {
				/* bondary case */

			}
		}
	}
	memcpy(buf, middleBuf, 3 * pageSize);
	for (i = 1; i < imageHeight - 1; i++) {
		for (j = 0; j < 3; j++) {
			buf[pageSize * j + i * imageWidth * 2 + 0] =
				buf[pageSize * j + i * imageWidth * 2 + 2];
			buf[pageSize * j + i * imageWidth * 2 + 1] =
				buf[pageSize * j + i * imageWidth * 2 + 3];
			buf[pageSize * j + i * imageWidth * 2 + imageWidth * 2 - 1] =
				buf[pageSize * j + i * imageWidth * 2 + imageWidth * 2 - 3];
			buf[pageSize * j + i * imageWidth * 2 + imageWidth * 2 - 2] =
				buf[pageSize * j + i * imageWidth * 2 + imageWidth * 2 - 4];
		}
	}
	for (i = 0; i < 3; i++) {
		memcpy(buf + pageSize * i,
				buf + pageSize * i + 2 * imageWidth,
				2 * imageWidth);
		memcpy(buf + pageSize * i + 2 * (imageHeight - 1) * imageWidth,
				buf + pageSize * i + 2 * (imageHeight - 2) * imageWidth,
				2 * imageWidth);
	}

	return ISP_UNIT_RESULT_OK;
}
