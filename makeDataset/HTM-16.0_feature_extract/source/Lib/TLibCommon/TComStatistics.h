#pragma once

#ifndef __TCOMSTATISTICS__
#define __TCOMSTATISTICS__

#include "TLibCommon/CommonDef.h"
#include "time.h"
// #include <fstream>

#define MYTRACE_ENCODECU	0
#define MYTRACE_XCOMPRESSCU 0
#define MYTRACE_GETALLPIC	0

#define MYTRACE_TIME		0
#define MYTRACE_TIME2		0
#define MYTRACE_VIEW		0
#define MYTRACE_SPATIAL		0
#define MYTRACE_COMPONENT	0

#define TREE_CU_64			0   // 获取特征（deprecated）
#define PRINT_ALL_FEATURES	0	// 获取除视点相关外全部信息，打印所有视点深度图特征做训练集，最终结果可以仅应用到基视点编码中
#define PRINT_DEP_FEATURES  0   // 获取全部信息（增加基视点信息：基视点像素信息，模式信息），打印所有依赖视点深度图特征做训练集，最终结果应用到依赖视点编码中
#define DT_CU_64			0	// 集成决策树（deprecated）

#define TREE_PU				0   // PU 模式选择树


#define DT_CU_32_16			0   // 验证从递归结构中获得当前CU是否划分的标签

#define PRINT_LABEL			1   // 从最终划分结果中读取划分标签
#define PRINT_PU_LABEL		0   // 从最终划分结果中读取PU标签
#define PRINT_FEATURE		1   // 从full RDO过程中提取编码特征
#define PRINT_DEBUG			0   // 调试上面两个PRINT过程正确性(正确)

#define PRINT_TIME          1
#define PRINT_FEATURE_VALUE 0

// extern std::ofstream feature0;
// extern std::ofstream feature1;
// extern std::ofstream feature2;
// extern std::ofstream feature_Ds0;
// extern std::ofstream feature_Ds1;
// extern std::ofstream feature_Ds2;
// extern std::ofstream fLabel0;
// extern std::ofstream fLabel1;
// extern std::ofstream fLabel2;
// extern std::ofstream fPUModeLabel0;
// extern std::ofstream fPUModeLabel1;
// extern std::ofstream fPUModeLabel2;
// extern std::ofstream fPUSizeLabel0;
// extern std::ofstream fPUSizeLabel1;
// extern std::ofstream fPUSizeLabel2;


#define DEPTHMAP_DEPTH_DISTRIBUTION		0 // rapPic 有修改（需要重新测试）

// fast_enable change begin

// fast_enable change end

#define QP_CONFIG			1  // TAppEncCfg.cpp G1813
extern double QP_config;

extern bool isDepthPic;
extern UInt ViewIdVal;
extern UInt numSubCU;
extern UInt numCU;
const UInt ViewId2cameraId[3] = { 5, 1, 9 };

const UInt numDepth = 4;
//const UInt CU_SIZE = 16; // 分别处理 64x64，32x32，16x16 CU 块 (deprecated)
const UInt TARGET_DEPTH = 2; // 树所在的深度 (deprecated) 重构代码，只区分 ALL 和 DEP，不区分CU块大小


extern UInt numSkip;
extern UInt numMerge;
extern UInt numInter2Nx2N;
extern UInt numOthers;

extern double rateSkip;
extern double rateMerge;
extern double rateInter2Nx2N;
extern double rateOthers;

extern UInt numCtus;
extern UInt StatDepth_Texture_DepthMap[numDepth][numDepth];


extern UInt numCUs[numDepth];
extern UInt numSkips[numDepth];
extern UInt numMerges[numDepth];
extern UInt numInter2Nx2Ns[numDepth];
extern UInt numOtherss[numDepth];

extern UInt numCUs_Ctu[numDepth];
extern UInt numSkips_Ctu[numDepth];
extern UInt numMerges_Ctu[numDepth];
extern UInt numInter2Nx2Ns_Ctu[numDepth];
extern UInt numOtherss_Ctu[numDepth];

extern double rateSkips[numDepth];
extern double rateMerges[numDepth];
extern double rateInter2Nx2Ns[numDepth];
extern double rateOtherss[numDepth];

extern double Data_CU_64[30];
extern double Data_CU_64_DependentView[20];
extern double Data_CU_64_ModeInfo[20];

// ================================ 特征定义 =====================================
// 时域相关性
extern double deltaPOC_t0;
extern double SG_t0;
extern double Dt_t0;
extern double Dt_cc_t0;
//extern double RDcost_t0;
//extern double RDcost_cc_t0;
extern double Pt_t0;
extern double Pt_cc_t0;
extern double MergeFlag_cc_t0;
extern double SkipFlag_cc_t0;
extern double SDCFlag_cc_t0;
extern double DISFlag_cc_t0;
//extern double CbfFlag_cc_t0;
extern double MSMFlag_cc_t0;
extern double deltaPOC_t1;
extern double SG_t1;
extern double Dt_t1;
extern double Dt_cc_t1;
//extern double RDcost_t1;
//extern double RDcost_cc_t1;
extern double Pt_t1;
extern double Pt_cc_t1;
extern double MergeFlag_cc_t1;
extern double SkipFlag_cc_t1;
extern double SDCFlag_cc_t1;
extern double DISFlag_cc_t1;
//extern double CbfFlag_cc_t1;
extern double MSMFlag_cc_t1;

// 视点间相关性
extern double VSG;
extern double Dv;
extern double Dv_cc;
//extern double RDcost_v;
//extern double RDcost_cc_v;
extern double Pv;
extern double Pv_cc;
extern double MergeFlag_cc_v;
extern double SkipFlag_cc_v;
extern double SDCFlag_cc_v;
extern double DISFlag_cc_v;
//extern double CbfFlag_cc_v;
extern double MSMFlag_cc_v;

// 组件间相关性(尝试同位块，或者crisscross)
extern double Dc;
extern double Dc_cc;
//extern double RDcost_c;
//extern double RDcost_cc_c;
extern double Pc;
extern double Pc_cc;
extern double MergeFlag_c;
extern double SkipFlag_c;
extern double SDCFlag_c;
extern double DISFlag_c;
//extern double CbfFlag_c;
extern double MSMFlag_c;
extern double MergeFlag_cc_c;
extern double SkipFlag_cc_c;
extern double SDCFlag_cc_c;
extern double DISFlag_cc_c;
//extern double CbfFlag_cc_c;
extern double MSMFlag_cc_c;
extern double Hc;

// 空域相关性
extern double Ds_GAMMA;
extern double Ds4mean;
extern double Ps4mean;
extern double MergeFlag_s;
extern double SkipFlag_s;
extern double SDCFlag_s;
extern double DISFlag_s;
//extern double CbfFlag_s;
extern double MSMFlag_s;

// 当前块与子块
extern double curMean;
extern double curVar;
extern double Hcur;
extern double curSubmvar;
extern double Ratio_curSubmvar;
extern double curSubdmean;
extern double Norm_curSubdmean;
extern double curSubdvar;
extern double Norm_curSubdvar;

// 当前块已编码信息
extern double QP;
//extern double QP_config;
extern double RD_MSM;
extern double SkipFlag;
extern double SDCFlag;
//extern double CbfFlag;
extern double TotalBins;
extern double TotalBits;
extern double TotalDistortion;
extern double POC;
extern double isB_SLICE;

// ================================ 特征定义 =====================================

extern Int labels[85];
extern Int PU_labels[85];
//extern double features[104];
extern double Ds[40];

extern double time_tag_filename_suffix;

// 确保调用代码的唯一性标志
/*
* 0: m_pcSliceEncoder->compressSlice   ( pcPic, false, false ); G1870 in TEncGOP.cpp
* 1: m_pcCuEncoder->compressCtu( pCtu ); G1010 in TEncSlice.cpp
* 2: xCheckRDCostDIS( rpcBestCU, rpcTempCU, SIZE_2Nx2N DEBUG_STRING_PASS_INTO(sDebug) );
* 3: xCheckRDCostInter( rpcBestCU, rpcTempCU, SIZE_2Nx2N DEBUG_STRING_PASS_INTO(sDebug), bFMD );  G1022 in TEncCU
*/
extern bool myFlag[5];

// 统计用时
extern double timeCost;
extern clock_t timeTag;

//extern bool mySplitFlag;
//extern bool myMSMFlag;
//
//void CUscore1(double * input, double * output);
//void CUscore2(double * input, double * output);
//void CUscore3(double * input, double * output);
//void CUscore4(double * input, double * output);
//void CUscore5(double * input, double * output);
//void CUscore6(double * input, double * output);
//
//void PUscore1(double * input, double * output);
//void PUscore2(double * input, double * output);
//void PUscore3(double * input, double * output);
//void PUscore4(double * input, double * output);
//void PUscore5(double * input, double * output);
//void PUscore6(double * input, double * output);
//void PUscore7(double * input, double * output);
//void PUscore8(double * input, double * output);

#endif
