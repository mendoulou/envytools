/*
 * Copyright (C) 2011 Marcelina Kościelnicka <mwk@0x04.net>
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef H264_CABAC_H
#define H264_CABAC_H

#include "h264.h"
#include <inttypes.h>

struct h264_cabac_ctx_init {
	int8_t m;
	int8_t n;
};

enum h264_cabac_ctxidx {
	H264_CABAC_CTXIDX_MB_TYPE_SI_PRE = 0,
	H264_CABAC_CTXIDX_MB_TYPE_I = 3, /* also SI suffix */

	H264_CABAC_CTXIDX_MB_SKIP_FLAG_P = 11, /* and SP */
	H264_CABAC_CTXIDX_MB_TYPE_P_PRE = 14,
	H264_CABAC_CTXIDX_MB_TYPE_P_SUF = 17,
	H264_CABAC_CTXIDX_SUB_MB_TYPE_P = 21,

	H264_CABAC_CTXIDX_MB_SKIP_FLAG_B = 24,
	H264_CABAC_CTXIDX_MB_TYPE_B_PRE = 27,
	H264_CABAC_CTXIDX_MB_TYPE_B_SUF = 32,
	H264_CABAC_CTXIDX_SUB_MB_TYPE_B = 36,

	H264_CABAC_CTXIDX_MVD_X = 40,
	H264_CABAC_CTXIDX_MVD_Y = 47,

	H264_CABAC_CTXIDX_REF_IDX = 54,

	H264_CABAC_CTXIDX_MB_QP_DELTA = 60,
	H264_CABAC_CTXIDX_INTRA_CHROMA_PRED_MODE = 64,
	H264_CABAC_CTXIDX_PREV_INTRA_PRED_MODE_FLAG = 68,
	H264_CABAC_CTXIDX_REM_INTRA_PRED_MODE = 69,

	H264_CABAC_CTXIDX_MB_FIELD_DECODING_FLAG = 70,
	H264_CABAC_CTXIDX_CODED_BLOCK_PATTERN_LUMA = 73,
	H264_CABAC_CTXIDX_CODED_BLOCK_PATTERN_CHROMA = 77,

	H264_CABAC_CTXIDX_CODED_BLOCK_FLAG_CAT0 = 85,
	H264_CABAC_CTXIDX_CODED_BLOCK_FLAG_CAT1 = 89,
	H264_CABAC_CTXIDX_CODED_BLOCK_FLAG_CAT2 = 93,
	H264_CABAC_CTXIDX_CODED_BLOCK_FLAG_CAT3 = 97,
	H264_CABAC_CTXIDX_CODED_BLOCK_FLAG_CAT4 = 101,

	H264_CABAC_CTXIDX_SIGNIFICANT_COEFF_FLAG_FRAME_CAT0 = 105,
	H264_CABAC_CTXIDX_SIGNIFICANT_COEFF_FLAG_FRAME_CAT1 = 120,
	H264_CABAC_CTXIDX_SIGNIFICANT_COEFF_FLAG_FRAME_CAT2 = 134,
	H264_CABAC_CTXIDX_SIGNIFICANT_COEFF_FLAG_FRAME_CAT3 = 149,
	H264_CABAC_CTXIDX_SIGNIFICANT_COEFF_FLAG_FRAME_CAT4 = 152,

	H264_CABAC_CTXIDX_LAST_SIGNIFICANT_COEFF_FLAG_FRAME_CAT0 = 166,
	H264_CABAC_CTXIDX_LAST_SIGNIFICANT_COEFF_FLAG_FRAME_CAT1 = 181,
	H264_CABAC_CTXIDX_LAST_SIGNIFICANT_COEFF_FLAG_FRAME_CAT2 = 195,
	H264_CABAC_CTXIDX_LAST_SIGNIFICANT_COEFF_FLAG_FRAME_CAT3 = 210,
	H264_CABAC_CTXIDX_LAST_SIGNIFICANT_COEFF_FLAG_FRAME_CAT4 = 213,

	H264_CABAC_CTXIDX_COEFF_ABS_LEVEL_MINUS1_PRE_CAT0 = 227,
	H264_CABAC_CTXIDX_COEFF_ABS_LEVEL_MINUS1_PRE_CAT1 = 237,
	H264_CABAC_CTXIDX_COEFF_ABS_LEVEL_MINUS1_PRE_CAT2 = 247,
	H264_CABAC_CTXIDX_COEFF_ABS_LEVEL_MINUS1_PRE_CAT3 = 257,
	H264_CABAC_CTXIDX_COEFF_ABS_LEVEL_MINUS1_PRE_CAT4 = 266,

	H264_CABAC_CTXIDX_TERMINATE = 276, /* special */

	H264_CABAC_CTXIDX_SIGNIFICANT_COEFF_FLAG_FIELD_CAT0 = 277,
	H264_CABAC_CTXIDX_SIGNIFICANT_COEFF_FLAG_FIELD_CAT1 = 292,
	H264_CABAC_CTXIDX_SIGNIFICANT_COEFF_FLAG_FIELD_CAT2 = 306,
	H264_CABAC_CTXIDX_SIGNIFICANT_COEFF_FLAG_FIELD_CAT3 = 321,
	H264_CABAC_CTXIDX_SIGNIFICANT_COEFF_FLAG_FIELD_CAT4 = 324,

	H264_CABAC_CTXIDX_LAST_SIGNIFICANT_COEFF_FLAG_FIELD_CAT0 = 338,
	H264_CABAC_CTXIDX_LAST_SIGNIFICANT_COEFF_FLAG_FIELD_CAT1 = 353,
	H264_CABAC_CTXIDX_LAST_SIGNIFICANT_COEFF_FLAG_FIELD_CAT2 = 367,
	H264_CABAC_CTXIDX_LAST_SIGNIFICANT_COEFF_FLAG_FIELD_CAT3 = 382,
	H264_CABAC_CTXIDX_LAST_SIGNIFICANT_COEFF_FLAG_FIELD_CAT4 = 385,

	H264_CABAC_CTXIDX_TRANSFORM_SIZE_8X8_FLAG = 399,

	H264_CABAC_CTXIDX_SIGNIFICANT_COEFF_FLAG_FRAME_CAT5 = 402,
	H264_CABAC_CTXIDX_LAST_SIGNIFICANT_COEFF_FLAG_FRAME_CAT5 = 417,
	H264_CABAC_CTXIDX_COEFF_ABS_LEVEL_MINUS1_PRE_CAT5 = 426,
	H264_CABAC_CTXIDX_SIGNIFICANT_COEFF_FLAG_FIELD_CAT5 = 436,
	H264_CABAC_CTXIDX_LAST_SIGNIFICANT_COEFF_FLAG_FIELD_CAT5 = 451,

	H264_CABAC_CTXIDX_CODED_BLOCK_FLAG_CAT6 = 460,
	H264_CABAC_CTXIDX_CODED_BLOCK_FLAG_CAT7 = 464,
	H264_CABAC_CTXIDX_CODED_BLOCK_FLAG_CAT8 = 468,
	H264_CABAC_CTXIDX_CODED_BLOCK_FLAG_CAT10 = 472,
	H264_CABAC_CTXIDX_CODED_BLOCK_FLAG_CAT11 = 476,
	H264_CABAC_CTXIDX_CODED_BLOCK_FLAG_CAT12 = 480,

	H264_CABAC_CTXIDX_SIGNIFICANT_COEFF_FLAG_FRAME_CAT6 = 484,
	H264_CABAC_CTXIDX_SIGNIFICANT_COEFF_FLAG_FRAME_CAT7 = 499,
	H264_CABAC_CTXIDX_SIGNIFICANT_COEFF_FLAG_FRAME_CAT8 = 513,
	H264_CABAC_CTXIDX_SIGNIFICANT_COEFF_FLAG_FRAME_CAT10 = 528,
	H264_CABAC_CTXIDX_SIGNIFICANT_COEFF_FLAG_FRAME_CAT11 = 543,
	H264_CABAC_CTXIDX_SIGNIFICANT_COEFF_FLAG_FRAME_CAT12 = 557,

	H264_CABAC_CTXIDX_LAST_SIGNIFICANT_COEFF_FLAG_FRAME_CAT6 = 572,
	H264_CABAC_CTXIDX_LAST_SIGNIFICANT_COEFF_FLAG_FRAME_CAT7 = 587,
	H264_CABAC_CTXIDX_LAST_SIGNIFICANT_COEFF_FLAG_FRAME_CAT8 = 601,
	H264_CABAC_CTXIDX_LAST_SIGNIFICANT_COEFF_FLAG_FRAME_CAT10 = 616,
	H264_CABAC_CTXIDX_LAST_SIGNIFICANT_COEFF_FLAG_FRAME_CAT11 = 631,
	H264_CABAC_CTXIDX_LAST_SIGNIFICANT_COEFF_FLAG_FRAME_CAT12 = 645,

	H264_CABAC_CTXIDX_SIGNIFICANT_COEFF_FLAG_FRAME_CAT9 = 660,
	H264_CABAC_CTXIDX_SIGNIFICANT_COEFF_FLAG_FIELD_CAT9 = 675,
	H264_CABAC_CTXIDX_LAST_SIGNIFICANT_COEFF_FLAG_FRAME_CAT9 = 690,
	H264_CABAC_CTXIDX_LAST_SIGNIFICANT_COEFF_FLAG_FIELD_CAT9 = 699,
	H264_CABAC_CTXIDX_COEFF_ABS_LEVEL_MINUS1_PRE_CAT9 = 708,

	H264_CABAC_CTXIDX_SIGNIFICANT_COEFF_FLAG_FRAME_CAT13 = 718,
	H264_CABAC_CTXIDX_SIGNIFICANT_COEFF_FLAG_FIELD_CAT13 = 733,
	H264_CABAC_CTXIDX_LAST_SIGNIFICANT_COEFF_FLAG_FRAME_CAT13 = 748,
	H264_CABAC_CTXIDX_LAST_SIGNIFICANT_COEFF_FLAG_FIELD_CAT13 = 757,
	H264_CABAC_CTXIDX_COEFF_ABS_LEVEL_MINUS1_PRE_CAT13 = 766,

	H264_CABAC_CTXIDX_SIGNIFICANT_COEFF_FLAG_FIELD_CAT6 = 776,
	H264_CABAC_CTXIDX_SIGNIFICANT_COEFF_FLAG_FIELD_CAT7 = 791,
	H264_CABAC_CTXIDX_SIGNIFICANT_COEFF_FLAG_FIELD_CAT8 = 805,
	H264_CABAC_CTXIDX_SIGNIFICANT_COEFF_FLAG_FIELD_CAT10 = 820,
	H264_CABAC_CTXIDX_SIGNIFICANT_COEFF_FLAG_FIELD_CAT11 = 835,
	H264_CABAC_CTXIDX_SIGNIFICANT_COEFF_FLAG_FIELD_CAT12 = 849,

	H264_CABAC_CTXIDX_LAST_SIGNIFICANT_COEFF_FLAG_FIELD_CAT6 = 864,
	H264_CABAC_CTXIDX_LAST_SIGNIFICANT_COEFF_FLAG_FIELD_CAT7 = 879,
	H264_CABAC_CTXIDX_LAST_SIGNIFICANT_COEFF_FLAG_FIELD_CAT8 = 893,
	H264_CABAC_CTXIDX_LAST_SIGNIFICANT_COEFF_FLAG_FIELD_CAT10 = 908,
	H264_CABAC_CTXIDX_LAST_SIGNIFICANT_COEFF_FLAG_FIELD_CAT11 = 923,
	H264_CABAC_CTXIDX_LAST_SIGNIFICANT_COEFF_FLAG_FIELD_CAT12 = 937,

	H264_CABAC_CTXIDX_COEFF_ABS_LEVEL_MINUS1_PRE_CAT6 = 952,
	H264_CABAC_CTXIDX_COEFF_ABS_LEVEL_MINUS1_PRE_CAT7 = 962,
	H264_CABAC_CTXIDX_COEFF_ABS_LEVEL_MINUS1_PRE_CAT8 = 972,
	H264_CABAC_CTXIDX_COEFF_ABS_LEVEL_MINUS1_PRE_CAT10 = 982,
	H264_CABAC_CTXIDX_COEFF_ABS_LEVEL_MINUS1_PRE_CAT11 = 992,
	H264_CABAC_CTXIDX_COEFF_ABS_LEVEL_MINUS1_PRE_CAT12 = 1002,

	H264_CABAC_CTXIDX_CODED_BLOCK_FLAG_CAT5 = 1012,
	H264_CABAC_CTXIDX_CODED_BLOCK_FLAG_CAT9 = 1016,
	H264_CABAC_CTXIDX_CODED_BLOCK_FLAG_CAT13 = 1020,

	/* SVC */
	H264_CABAC_CTXIDX_BASE_MODE_FLAG = 1024,
	H264_CABAC_CTXIDX_MOTION_PREDICTION_FLAG_L0 = 1027,
	H264_CABAC_CTXIDX_MOTION_PREDICTION_FLAG_L1 = 1028,
	H264_CABAC_CTXIDX_RESIDUAL_PREDICTION_FLAG = 1029,

	H264_CABAC_CTXIDX_NUM = 1031,
};

struct h264_cabac_context {
	struct h264_slice *slice;
	uint8_t pStateIdx[H264_CABAC_CTXIDX_NUM];
	uint8_t valMPS[H264_CABAC_CTXIDX_NUM];
	uint32_t codIOffset; /* and codILow */
	uint32_t codIRange;
	int firstBitFlag;
	int bitsOutstanding;
	int BinCount;
};

struct h264_cabac_se_val {
	int val;
	const struct h264_cabac_se_val *subtab;
	int blen;
	struct h264_cabac_se_bit {
		int bidx;
		int val;
	} bits[8];
};

struct h264_cabac_context *h264_cabac_new(struct h264_slice *slice);
int h264_cabac_init_arith(struct bitstream *str, struct h264_cabac_context *cabac);
int h264_cabac_renorm(struct bitstream *str, struct h264_cabac_context *cabac);
int h264_cabac_decision(struct bitstream *str, struct h264_cabac_context *cabac, int ctxIdx, uint32_t *binVal);
int h264_cabac_bypass(struct bitstream *str, struct h264_cabac_context *cabac, uint32_t *binVal);
int h264_cabac_terminate(struct bitstream *str, struct h264_cabac_context *cabac, uint32_t *binVal);
int h264_cabac_se(struct bitstream *str, struct h264_cabac_context *cabac, const struct h264_cabac_se_val *tab, int *ctxIdx, uint32_t *val);
int h264_cabac_tu(struct bitstream *str, struct h264_cabac_context *cabac, int *ctxIdx, int numidx, uint32_t cMax, uint32_t *val);
int h264_cabac_ueg(struct bitstream *str, struct h264_cabac_context *cabac, int *ctxIdx, int numidx, int k, int sign, uint32_t uCoff, int32_t *val);
void h264_cabac_destroy(struct h264_cabac_context *cabac);

#endif
