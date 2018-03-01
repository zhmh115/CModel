cmake_minimum_required(VERSION 2.8)
PROJECT(ISP_PIPELINE)
SET( SRC_LIST ../isp_pipeline.c
	../unit_src/isp_unit_remove_blc.c
	../unit_src/isp_unit_apply_wb.c
	../unit_src/isp_unit_ccm.c
	../unit_src/isp_unit_cip.c
	../unit_src/isp_unit_convert_yuv.c
	../unit_src/isp_unit_lenc.c
	../unit_src/isp_unit_rawdns.c
	../unit_src/isp_unit_rawstretch.c
	../unit_src/isp_unit_rgbgamma.c
	../unit_src/isp_unit_sharpen.c
	../unit_src/isp_unit_stat_exp.c
	../unit_src/isp_unit_stat_fv.c
	../unit_src/isp_unit_stat_wb.c
	)
include_directories(../unit_include ..)
LINK_LIBRARIES( m )
ADD_EXECUTABLE(../out/bin/isp_pipeline ${SRC_LIST})
