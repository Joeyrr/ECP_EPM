#include "TLibCommon/TComStatistics.h"
#include <math.h>
#include <string.h>

double QP_config = -1;

bool isDepthPic = false;
UInt ViewIdVal = 0;
UInt numSubCU = 0;
UInt numCU = 0;

UInt numSkip = 0;
UInt numMerge = 0;
UInt numInter2Nx2N = 0;
UInt numOthers = 0;

double rateSkip = 0.0;
double rateMerge = 0.0;
double rateInter2Nx2N = 0.0;
double rateOthers = 0.0;

UInt numCtus = 0; // ?��?�訪3???y����D?�ꡧ12����???������??CTU��?
UInt StatDepth_Texture_DepthMap[numDepth][numDepth] = { {0, 0, 0, 0 }, {0, 0, 0, 0 } ,{0, 0, 0, 0 } ,{0, 0, 0, 0 } };

UInt numCUs[numDepth] = { 0, 0, 0, 0 };
UInt numSkips[numDepth] = { 0, 0, 0, 0 };
UInt numMerges[numDepth] = { 0, 0, 0, 0 };
UInt numInter2Nx2Ns[numDepth] = { 0, 0, 0, 0 };
UInt numOtherss[numDepth] = { 0, 0, 0, 0 };

UInt numCUs_Ctu[numDepth] = { 0, 0, 0, 0 };
UInt numSkips_Ctu[numDepth] = { 0, 0, 0, 0 };
UInt numMerges_Ctu[numDepth] = { 0, 0, 0, 0 };
UInt numInter2Nx2Ns_Ctu[numDepth] = { 0, 0, 0, 0 };
UInt numOtherss_Ctu[numDepth] = { 0, 0, 0, 0 };

double rateSkips[numDepth] = { 0.0, 0.0, 0.0, 0.0 };
double rateMerges[numDepth] = { 0.0, 0.0, 0.0, 0.0 };
double rateInter2Nx2Ns[numDepth] = { 0.0, 0.0, 0.0, 0.0 };
double rateOtherss[numDepth] = { 0.0, 0.0, 0.0, 0.0 };

/*
* 0:  ?��?��?��???�� grayscaleSimilar
* 1:  ����������????����??�� tColDepth
* 2:  ����?����??�� lDepth
* 3:  ��??����??�� aDepth
* 4:  ������??����??�� laDepth
* 5:  ??������????��?��?�� cColmean
* 6:  ??������????��??2? cColrange
* 7:  ??������????����?2? cColvar
* 8:  ??������????����Y?�� cColgrad
* 9:  ??��������?����?�䨮��?2? cColSubmvar
* 10: ??��������?����?�䨮��Y?�� cColSubmgrad
* 11: ??��������?��?��?��o��???����?�䨮2??�� cColSubdmean
* 12: ??��������?����?2?o��???����?�䨮2??�� cColSubdvar
* 13: ??��������?����Y?��o��???����?�䨮2??�� cColSubdgrad
* 14: ??������????����?��??��?�� cColDepth
* 15: �̡�?��?��?��?�� curMean
* 16: �̡�?��?��??2? curRange
* 17: �̡�?��?����?2? curVar
* 18: �̡�?��?����Y?�� curGrad
* 19: �̡�?��?������?����?�䨮��?2? curSubmvar
* 20: �̡�?��?������?����?�䨮��Y?�� curSubmgrad
* 21: �̡�?��?������?��?��?��o��???����?�䨮2??�� curSubdmean
* 22: �̡�?��?������?����?2?o��???����?�䨮2??�� curSubdvar
* 23: �̡�?��?������?����Y?��o��???����?�䨮2??�� curSubdgrad
* 24: �̡�?��LCU��???��??��=>??��??��??��?�ꡧ����??��?split
*/

// ??�����������̨�����???��??��������?��??��(��3???��???????��������?��??�¨���?o��???����)
/*
* 0: ������????��?��?��???�� vGS
* 1: ?��������???��?��??�� vColDepth
* 2: ?��������???��|?��?��?�� vColMean
* 3: ?��������???��|?��??2? vColRange
* 4: ?��������???��|?����?2? vColVar
* 5: ?��������???��|?����Y?�� _5
* 6: ?��������???��|?������?����?�䨮��?2? _6
* 7: ?��������???��|?������?����?�䨮��Y?�� _7
* 8: ?��������???��|?������?��?��?��o��???����?�䨮2??�� vColSubdmean
* 9: ?��������???��|?������?����?2?o��???����?�䨮2??�� vColSubdvar
* 10: ?��������???��|?������?����Y?��o��???����?�䨮2??�� _10
*/

/*
* 0: QP
* 1: RDcost
* 2. RD_MSM Merge/Skip ?�꨺?��?RDcost
* 3. RD_DIS DIS?�꨺?��?RDcost
* 4. RD_Intra ???��?������??�䨲??
* 5. Ratio ???����?RD_DIS?�꨺?��?RDcost??����
* 6. RD_Inter2Nx2N ??��?Inter2Nx2N ?�꨺??������??�䨲??
* 7. RatioInter Inter2Nx2N o�� MSM ?������??�䨲????����
* 8. RelRatio Inter2Nx2N o�� MSM ?������??�䨲??1����??��2??��
* 9. Skip_flag
* 10. DIS_flag
*/

// ================================ 特征定义 =====================================
// 时域相关性
double deltaPOC_t0 = -1.0;
double SG_t0 = -1.0;
double Dt_t0 = -1.0;
double Dt_cc_t0 = -1.0;
//double RDcost_t0 = -1.0;
//double RDcost_cc_t0 = -1.0;
double Pt_t0 = -1.0;
double Pt_cc_t0 = -1.0;
double MergeFlag_cc_t0 = -1.0;
double SkipFlag_cc_t0 = -1.0;
double SDCFlag_cc_t0 = -1.0;
double DISFlag_cc_t0 = -1.0;
//double CbfFlag_cc_t0 = -1.0;
double MSMFlag_cc_t0 = -1.0;
double deltaPOC_t1 = -1.0;
double SG_t1 = -1.0;
double Dt_t1 = -1.0;
double Dt_cc_t1 = -1.0;
//double RDcost_t1 = -1.0;
//double RDcost_cc_t1 = -1.0;
double Pt_t1 = -1.0;
double Pt_cc_t1 = -1.0;
double MergeFlag_cc_t1 = -1.0;
double SkipFlag_cc_t1 = -1.0;
double SDCFlag_cc_t1 = -1.0;
double DISFlag_cc_t1 = -1.0;
//double CbfFlag_cc_t1 = -1.0;
double MSMFlag_cc_t1 = -1.0;

// 视点间相关性
double VSG = -1.0;
double Dv = -1.0;
double Dv_cc = -1.0;
//double RDcost_v = -1.0;
//double RDcost_cc_v = -1.0;
double Pv = -1.0;
double Pv_cc = -1.0;
double MergeFlag_cc_v = -1.0;
double SkipFlag_cc_v = -1.0;
double SDCFlag_cc_v = -1.0;
double DISFlag_cc_v = -1.0;
//double CbfFlag_cc_v = -1.0;
double MSMFlag_cc_v = -1.0;

// 组件间相关性(尝试同位块，或者crisscross)
double Dc = -1.0;
double Dc_cc = -1.0;
//double RDcost_c = -1.0;
//double RDcost_cc_c = -1.0;
double Pc = -1.0;
double Pc_cc = -1.0;
double MergeFlag_c = -1.0;
double SkipFlag_c = -1.0;
double SDCFlag_c = -1.0;
double DISFlag_c = -1.0;
//double CbfFlag_c = -1.0;
double MSMFlag_c = -1.0;
double MergeFlag_cc_c = -1.0;
double SkipFlag_cc_c = -1.0;
double SDCFlag_cc_c = -1.0;
double DISFlag_cc_c = -1.0;
//double CbfFlag_cc_c = -1.0;
double MSMFlag_cc_c = -1.0;
double Hc = -1.0;

// 空域相关性
double Ds_GAMMA = -1.0;
double Ds4mean = -1.0;
double Ps4mean = -1.0;
double MergeFlag_s = -1.0;
double SkipFlag_s = -1.0;
double SDCFlag_s = -1.0;
double DISFlag_s = -1.0;
//double CbfFlag_s = -1.0;
double MSMFlag_s = -1.0;

// 当前块与子块
double curMean = -1.0;
double curVar = -1.0;
double Hcur = -1.0;
double curSubmvar = -1.0;
double Ratio_curSubmvar = -1.0;
double curSubdmean = -1.0;
double Norm_curSubdmean = -1.0;
double curSubdvar = -1.0;
double Norm_curSubdvar = -1.0;

// 当前块已编码信息
double QP = -1.0;
//double QP_config = -1.0;
double RD_MSM = -1.0;
double SkipFlag = -1.0;
double SDCFlag = -1.0;
//double CbfFlag = -1.0;
double TotalBins = -1.0;
double TotalBits = -1.0;
double TotalDistortion = -1.0;
double POC = -1.0;
double isB_SLICE = -1.0;

// ================================ 特征定义 =====================================

double Data_CU_64[30] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
						 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
						 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };

double Data_CU_64_DependentView[20] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
										0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };

double Data_CU_64_ModeInfo[20] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
								   0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };

double time_tag_filename_suffix = 0.0;

Int labels[85] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
					-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
					-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
					-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
					-1, -1, -1, -1, -1 };
Int PU_labels[85] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
					-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
					-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
					-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
					-1, -1, -1, -1, -1 };

//double features[104] = {-1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
//						-1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
//						-1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
//						-1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
//						-1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
//						-1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
//						-1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
//						-1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
//						-1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
//						-1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
//						-1.0, -1.0, -1.0, -1.0 };
double Ds[40] = { -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
				  -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
				  -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
				  -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0 };


// ��������̡¨�?�䨲??��??����?D?����??
// �̨���???����?��̡¨�?�̣�?���̡¨�?��?��? compressSlice 
// 0: m_pcSliceEncoder->compressSlice   ( pcPic, false, false );
// 1: m_pcCuEncoder->compressCtu( pCtu ); 
// 2: xCheckRDCostDIS( rpcBestCU, rpcTempCU, SIZE_2Nx2N DEBUG_STRING_PASS_INTO(sDebug) );
// 3: xCheckRDCostInter( rpcBestCU, rpcTempCU, SIZE_NxN DEBUG_STRING_PASS_INTO(sDebug), bFMD  );
bool myFlag[5] = { false, false, false, false, false };

double timeCost = 0.0;
clock_t timeTag = 0.0;