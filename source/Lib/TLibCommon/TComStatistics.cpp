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
//                      -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
//                      -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
//                      -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
//                      -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
//                      -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
//                      -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
//                      -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
//                      -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
//                      -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
//                      -1.0, -1.0, -1.0, -1.0 };
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
//long timeCost_us = 0;
//long timeTag_us = 0;

bool mySplitFlag = true;
bool myMSMFlag = false;

void CUclf1(double * input, double * output) {
	double var0;
	if ((input[38]) >= (0.125)) {
		if ((input[56]) >= (3637.5)) {
			if ((input[14]) >= (0.45)) {
				var0 = 0.19305189;
			}
			else {
				var0 = 0.15075457;
			}
		}
		else {
			if ((input[37]) >= (0.28785)) {
				var0 = 0.15835963;
			}
			else {
				var0 = -0.0031700288;
			}
		}
	}
	else {
		if ((input[60]) >= (2.5)) {
			if ((input[51]) >= (0.00995)) {
				var0 = 0.14515491;
			}
			else {
				var0 = -0.16228956;
			}
		}
		else {
			if ((input[13]) >= (0.5)) {
				var0 = -0.044492234;
			}
			else {
				var0 = -0.19420035;
			}
		}
	}
	double var1;
	if ((input[38]) >= (0.125)) {
		if ((input[56]) >= (2423.5)) {
			if ((input[2]) >= (0.5)) {
				var1 = 0.17468342;
			}
			else {
				var1 = 0.13319764;
			}
		}
		else {
			if ((input[37]) >= (0.28785)) {
				var1 = 0.14097814;
			}
			else {
				var1 = -0.02851142;
			}
		}
	}
	else {
		if ((input[2]) >= (0.5)) {
			if ((input[13]) >= (0.5)) {
				var1 = 0.115606844;
			}
			else {
				var1 = -0.117193125;
			}
		}
		else {
			if ((input[60]) >= (3.5)) {
				var1 = 0.02231722;
			}
			else {
				var1 = -0.17735603;
			}
		}
	}
	double var2;
	if ((input[38]) >= (0.125)) {
		if ((input[38]) >= (0.58335)) {
			if ((input[50]) >= (1.75565)) {
				var2 = 0.16320042;
			}
			else {
				var2 = 0.12416112;
			}
		}
		else {
			if ((input[56]) >= (1733.5)) {
				var2 = 0.13944069;
			}
			else {
				var2 = 0.040380895;
			}
		}
	}
	else {
		if ((input[60]) >= (2.5)) {
			if ((input[51]) >= (0.01425)) {
				var2 = 0.12828802;
			}
			else {
				var2 = -0.12523568;
			}
		}
		else {
			if ((input[2]) >= (0.5)) {
				var2 = -0.03365242;
			}
			else {
				var2 = -0.16383992;
			}
		}
	}
	double var3;
	if ((input[38]) >= (0.125)) {
		if ((input[56]) >= (3785.5)) {
			if ((input[14]) >= (0.45)) {
				var3 = 0.15216461;
			}
			else {
				var3 = 0.11241517;
			}
		}
		else {
			if ((input[37]) >= (0.28785)) {
				var3 = 0.12172931;
			}
			else {
				var3 = -0.0113315135;
			}
		}
	}
	else {
		if ((input[13]) >= (0.5)) {
			if ((input[3]) >= (0.225)) {
				var3 = 0.09417844;
			}
			else {
				var3 = -0.11422842;
			}
		}
		else {
			if ((input[60]) >= (2.5)) {
				var3 = 0.006154688;
			}
			else {
				var3 = -0.15332113;
			}
		}
	}
	double var4;
	if ((input[38]) >= (0.125)) {
		if ((input[56]) >= (3785.5)) {
			if ((input[38]) >= (0.41665)) {
				var4 = 0.14518045;
			}
			else {
				var4 = 0.11692335;
			}
		}
		else {
			if ((input[37]) >= (0.95455)) {
				var4 = 0.13925861;
			}
			else {
				var4 = 0.053400423;
			}
		}
	}
	else {
		if ((input[22]) >= (0.5)) {
			if ((input[48]) >= (29.471199)) {
				var4 = 0.10024627;
			}
			else {
				var4 = -0.07307395;
			}
		}
		else {
			if ((input[60]) >= (2.5)) {
				var4 = 0.01889859;
			}
			else {
				var4 = -0.14347921;
			}
		}
	}
	double var5;
	if ((input[38]) >= (0.125)) {
		if ((input[56]) >= (1819.5)) {
			if ((input[2]) >= (0.5)) {
				var5 = 0.13507019;
			}
			else {
				var5 = 0.08822572;
			}
		}
		else {
			if ((input[37]) >= (0.07575)) {
				var5 = 0.101707116;
			}
			else {
				var5 = -0.06046191;
			}
		}
	}
	else {
		if ((input[13]) >= (0.5)) {
			if ((input[2]) >= (0.5)) {
				var5 = 0.092269115;
			}
			else {
				var5 = -0.08694797;
			}
		}
		else {
			if ((input[60]) >= (2.5)) {
				var5 = 0.002484665;
			}
			else {
				var5 = -0.13791145;
			}
		}
	}
	double var6;
	if ((input[38]) >= (0.125)) {
		if ((input[38]) >= (0.70835)) {
			if ((input[50]) >= (1.1064501)) {
				var6 = 0.13048849;
			}
			else {
				var6 = 0.082701184;
			}
		}
		else {
			if ((input[13]) >= (0.5)) {
				var6 = 0.10714749;
			}
			else {
				var6 = 0.017372191;
			}
		}
	}
	else {
		if ((input[23]) >= (0.45)) {
			if ((input[56]) >= (3150.5)) {
				var6 = 0.0968286;
			}
			else {
				var6 = -0.072935574;
			}
		}
		else {
			if ((input[60]) >= (3.5)) {
				var6 = 0.017424675;
			}
			else {
				var6 = -0.13063923;
			}
		}
	}
	double var7;
	if ((input[38]) >= (0.125)) {
		if ((input[56]) >= (3785.5)) {
			if ((input[3]) >= (0.775)) {
				var7 = 0.12696481;
			}
			else {
				var7 = 0.094741754;
			}
		}
		else {
			if ((input[37]) >= (0.95455)) {
				var7 = 0.119719125;
			}
			else {
				var7 = 0.03520942;
			}
		}
	}
	else {
		if ((input[2]) >= (0.5)) {
			if ((input[12]) >= (0.009649999)) {
				var7 = 0.07631113;
			}
			else {
				var7 = -0.11271224;
			}
		}
		else {
			if ((input[60]) >= (4.5)) {
				var7 = 0.017449629;
			}
			else {
				var7 = -0.12701195;
			}
		}
	}
	double var8;
	if ((input[38]) >= (0.125)) {
		if ((input[56]) >= (3388.5)) {
			if ((input[3]) >= (0.775)) {
				var8 = 0.1222901;
			}
			else {
				var8 = 0.08643072;
			}
		}
		else {
			if ((input[37]) >= (0.95455)) {
				var8 = 0.11370879;
			}
			else {
				var8 = 0.029806668;
			}
		}
	}
	else {
		if ((input[13]) >= (0.5)) {
			if ((input[12]) >= (0.01575)) {
				var8 = 0.07235395;
			}
			else {
				var8 = -0.10153089;
			}
		}
		else {
			if ((input[23]) >= (0.63335)) {
				var8 = -0.01659291;
			}
			else {
				var8 = -0.12422445;
			}
		}
	}
	double var9;
	if ((input[38]) >= (0.125)) {
		if ((input[56]) >= (1819.5)) {
			if ((input[2]) >= (0.5)) {
				var9 = 0.11515236;
			}
			else {
				var9 = 0.06444463;
			}
		}
		else {
			if ((input[37]) >= (0.07575)) {
				var9 = 0.078476;
			}
			else {
				var9 = -0.06732066;
			}
		}
	}
	else {
		if ((input[22]) >= (0.5)) {
			if ((input[56]) >= (1319.0)) {
				var9 = 0.06703221;
			}
			else {
				var9 = -0.09449783;
			}
		}
		else {
			if ((input[2]) >= (0.5)) {
				var9 = -0.034885522;
			}
			else {
				var9 = -0.121217586;
			}
		}
	}
	double var10;
	if ((input[38]) >= (0.125)) {
		if ((input[38]) >= (0.875)) {
			if ((input[51]) >= (0.00715)) {
				var10 = 0.11485056;
			}
			else {
				var10 = 0.033510942;
			}
		}
		else {
			if ((input[60]) >= (2.5)) {
				var10 = 0.10210457;
			}
			else {
				var10 = 0.023710577;
			}
		}
	}
	else {
		if ((input[14]) >= (0.225)) {
			if ((input[56]) >= (1630.0)) {
				var10 = 0.060684707;
			}
			else {
				var10 = -0.09548532;
			}
		}
		else {
			if ((input[56]) >= (14685.0)) {
				var10 = -0.018143361;
			}
			else {
				var10 = -0.11868314;
			}
		}
	}
	double var11;
	if ((input[38]) >= (0.125)) {
		if ((input[56]) >= (3976.5)) {
			if ((input[38]) >= (0.41665)) {
				var11 = 0.11303969;
			}
			else {
				var11 = 0.07553344;
			}
		}
		else {
			if ((input[37]) >= (0.95455)) {
				var11 = 0.10184574;
			}
			else {
				var11 = 0.019177599;
			}
		}
	}
	else {
		if ((input[60]) >= (2.5)) {
			if ((input[47]) >= (0.14725)) {
				var11 = 0.09177883;
			}
			else {
				var11 = -0.14719187;
			}
		}
		else {
			if ((input[61]) >= (13947.0)) {
				var11 = 0.038397145;
			}
			else {
				var11 = -0.10811539;
			}
		}
	}
	double var12;
	if ((input[38]) >= (0.125)) {
		if ((input[56]) >= (3976.5)) {
			if ((input[14]) >= (0.675)) {
				var12 = 0.109469965;
			}
			else {
				var12 = 0.06669103;
			}
		}
		else {
			if ((input[37]) >= (0.98485)) {
				var12 = 0.09971844;
			}
			else {
				var12 = 0.018688636;
			}
		}
	}
	else {
		if ((input[2]) >= (0.5)) {
			if ((input[14]) >= (0.1)) {
				var12 = 0.06460752;
			}
			else {
				var12 = -0.098650396;
			}
		}
		else {
			if ((input[60]) >= (2.5)) {
				var12 = 0.014473249;
			}
			else {
				var12 = -0.110521376;
			}
		}
	}
	double var13;
	if ((input[38]) >= (0.125)) {
		if ((input[13]) >= (0.5)) {
			if ((input[2]) >= (0.5)) {
				var13 = 0.10032757;
			}
			else {
				var13 = 0.031626828;
			}
		}
		else {
			if ((input[37]) >= (0.07575)) {
				var13 = 0.06218643;
			}
			else {
				var13 = -0.095814936;
			}
		}
	}
	else {
		if ((input[14]) >= (0.225)) {
			if ((input[12]) >= (0.01575)) {
				var13 = 0.05520272;
			}
			else {
				var13 = -0.08900488;
			}
		}
		else {
			if ((input[60]) >= (5.5)) {
				var13 = 0.003272753;
			}
			else {
				var13 = -0.10975802;
			}
		}
	}
	double var14;
	if ((input[38]) >= (0.125)) {
		if ((input[38]) >= (0.875)) {
			if ((input[50]) >= (1.9508)) {
				var14 = 0.107658006;
			}
			else {
				var14 = 0.06157654;
			}
		}
		else {
			if ((input[60]) >= (2.5)) {
				var14 = 0.08812895;
			}
			else {
				var14 = 0.0069839545;
			}
		}
	}
	else {
		if ((input[3]) >= (0.225)) {
			if ((input[56]) >= (1504.5)) {
				var14 = 0.056833953;
			}
			else {
				var14 = -0.08789521;
			}
		}
		else {
			if ((input[60]) >= (5.5)) {
				var14 = 0.0052672494;
			}
			else {
				var14 = -0.10781366;
			}
		}
	}
	double var15;
	if ((input[38]) >= (0.125)) {
		if ((input[56]) >= (4372.5)) {
			if ((input[54]) >= (43.5)) {
				var15 = 0.05620606;
			}
			else {
				var15 = 0.10319052;
			}
		}
		else {
			if ((input[37]) >= (0.95455)) {
				var15 = 0.088434905;
			}
			else {
				var15 = 0.008027016;
			}
		}
	}
	else {
		if ((input[23]) >= (0.45)) {
			if ((input[56]) >= (10941.5)) {
				var15 = 0.09984043;
			}
			else {
				var15 = -0.012766694;
			}
		}
		else {
			if ((input[13]) >= (0.5)) {
				var15 = -0.024486834;
			}
			else {
				var15 = -0.10624813;
			}
		}
	}
	double var16;
	if ((input[13]) >= (0.5)) {
		if ((input[3]) >= (0.45)) {
			if ((input[56]) >= (1049.5)) {
				var16 = 0.092802525;
			}
			else {
				var16 = -0.0054256152;
			}
		}
		else {
			if ((input[37]) >= (0.28785)) {
				var16 = 0.06153209;
			}
			else {
				var16 = -0.050032355;
			}
		}
	}
	else {
		if ((input[37]) >= (0.07575)) {
			if ((input[3]) >= (0.1)) {
				var16 = -0.00013934966;
			}
			else {
				var16 = 0.10538949;
			}
		}
		else {
			if ((input[60]) >= (2.5)) {
				var16 = 0.018070333;
			}
			else {
				var16 = -0.10330013;
			}
		}
	}
	double var17;
	if ((input[13]) >= (0.5)) {
		if ((input[2]) >= (0.5)) {
			if ((input[56]) >= (987.5)) {
				var17 = 0.087502114;
			}
			else {
				var17 = -0.0022621686;
			}
		}
		else {
			if ((input[37]) >= (0.28785)) {
				var17 = 0.040833842;
			}
			else {
				var17 = -0.050896503;
			}
		}
	}
	else {
		if ((input[37]) >= (0.07575)) {
			if ((input[3]) >= (0.1)) {
				var17 = -0.00012542892;
			}
			else {
				var17 = 0.101634495;
			}
		}
		else {
			if ((input[60]) >= (1.5)) {
				var17 = 0.013450217;
			}
			else {
				var17 = -0.10084921;
			}
		}
	}
	double var18;
	if ((input[2]) >= (0.5)) {
		if ((input[14]) >= (0.45)) {
			if ((input[56]) >= (1515.5)) {
				var18 = 0.08894547;
			}
			else {
				var18 = 0.010675109;
			}
		}
		else {
			if ((input[60]) >= (2.5)) {
				var18 = 0.04997837;
			}
			else {
				var18 = -0.049134795;
			}
		}
	}
	else {
		if ((input[37]) >= (0.28785)) {
			if ((input[3]) >= (0.1)) {
				var18 = -0.0013469442;
			}
			else {
				var18 = 0.09665143;
			}
		}
		else {
			if ((input[60]) >= (2.5)) {
				var18 = 0.016748553;
			}
			else {
				var18 = -0.09928233;
			}
		}
	}
	double var19;
	if ((input[22]) >= (0.5)) {
		if ((input[48]) >= (18.1907)) {
			if ((input[60]) >= (0.5)) {
				var19 = 0.093093194;
			}
			else {
				var19 = 0.030933036;
			}
		}
		else {
			if ((input[54]) >= (37.5)) {
				var19 = -0.0513433;
			}
			else {
				var19 = 0.037997276;
			}
		}
	}
	else {
		if ((input[37]) >= (0.28785)) {
			if ((input[3]) >= (0.1)) {
				var19 = 0.010561212;
			}
			else {
				var19 = 0.09758361;
			}
		}
		else {
			if ((input[1]) >= (0.0951)) {
				var19 = -0.019725371;
			}
			else {
				var19 = -0.10457313;
			}
		}
	}
	double var20;
	if ((input[13]) >= (0.5)) {
		if ((input[3]) >= (0.70835)) {
			if ((input[56]) >= (1515.5)) {
				var20 = 0.0884929;
			}
			else {
				var20 = 0.012636964;
			}
		}
		else {
			if ((input[60]) >= (2.5)) {
				var20 = 0.05562116;
			}
			else {
				var20 = -0.028993309;
			}
		}
	}
	else {
		if ((input[37]) >= (0.07575)) {
			if ((input[3]) >= (0.1)) {
				var20 = -0.0033862346;
			}
			else {
				var20 = 0.09160891;
			}
		}
		else {
			if ((input[23]) >= (0.63335)) {
				var20 = 0.006543419;
			}
			else {
				var20 = -0.095765196;
			}
		}
	}
	double var21;
	if ((input[60]) >= (1.5)) {
		if ((input[46]) >= (0.63135004)) {
			if ((input[60]) >= (19.5)) {
				var21 = 0.10665911;
			}
			else {
				var21 = 0.065024234;
			}
		}
		else {
			if ((input[61]) >= (883.5)) {
				var21 = -0.0005445439;
			}
			else {
				var21 = -0.1807097;
			}
		}
	}
	else {
		if ((input[37]) >= (0.28785)) {
			if ((input[37]) >= (0.98485)) {
				var21 = 0.08359324;
			}
			else {
				var21 = 0.0064384714;
			}
		}
		else {
			if ((input[61]) >= (14154.0)) {
				var21 = 0.046307094;
			}
			else {
				var21 = -0.08701818;
			}
		}
	}
	double var22;
	if ((input[2]) >= (0.5)) {
		if ((input[14]) >= (0.70835)) {
			if ((input[12]) >= (0.0238)) {
				var22 = 0.08144247;
			}
			else {
				var22 = -0.00056082377;
			}
		}
		else {
			if ((input[56]) >= (949.0)) {
				var22 = 0.01638316;
			}
			else {
				var22 = -0.07726675;
			}
		}
	}
	else {
		if ((input[37]) >= (0.28785)) {
			if ((input[3]) >= (0.1)) {
				var22 = -0.009477089;
			}
			else {
				var22 = 0.08578346;
			}
		}
		else {
			if ((input[22]) >= (0.5)) {
				var22 = -0.0039278;
			}
			else {
				var22 = -0.0949445;
			}
		}
	}
	double var23;
	if ((input[60]) >= (1.5)) {
		if ((input[46]) >= (0.63135004)) {
			if ((input[60]) >= (22.5)) {
				var23 = 0.10461128;
			}
			else {
				var23 = 0.059708655;
			}
		}
		else {
			if ((input[3]) >= (1.1)) {
				var23 = 0.01671794;
			}
			else {
				var23 = -0.16653942;
			}
		}
	}
	else {
		if ((input[37]) >= (0.28785)) {
			if ((input[37]) >= (0.98485)) {
				var23 = 0.07904508;
			}
			else {
				var23 = 0.0018707927;
			}
		}
		else {
			if ((input[61]) >= (8344.5)) {
				var23 = 0.02573509;
			}
			else {
				var23 = -0.08544158;
			}
		}
	}
	double var24;
	if ((input[13]) >= (0.5)) {
		if ((input[2]) >= (1.5)) {
			if ((input[1]) >= (0.0045)) {
				var24 = 0.089635864;
			}
			else {
				var24 = 0.007820759;
			}
		}
		else {
			if ((input[60]) >= (1.5)) {
				var24 = 0.05999551;
			}
			else {
				var24 = -0.013650839;
			}
		}
	}
	else {
		if ((input[37]) >= (0.95455)) {
			if ((input[1]) >= (0.0011)) {
				var24 = 0.042283718;
			}
			else {
				var24 = 0.09819936;
			}
		}
		else {
			if ((input[60]) >= (6.5)) {
				var24 = 0.03218947;
			}
			else {
				var24 = -0.07470364;
			}
		}
	}
	double var25;
	if ((input[22]) >= (0.5)) {
		if ((input[56]) >= (9095.5)) {
			if ((input[54]) >= (45.5)) {
				var25 = 0.016235724;
			}
			else {
				var25 = 0.095799066;
			}
		}
		else {
			if ((input[54]) >= (40.5)) {
				var25 = -0.072465196;
			}
			else {
				var25 = 0.044661533;
			}
		}
	}
	else {
		if ((input[37]) >= (0.28785)) {
			if ((input[3]) >= (0.1)) {
				var25 = -0.004729589;
			}
			else {
				var25 = 0.08686503;
			}
		}
		else {
			if ((input[12]) >= (0.0321)) {
				var25 = -0.012929383;
			}
			else {
				var25 = -0.09856527;
			}
		}
	}
	double var26;
	if ((input[48]) >= (21.6165)) {
		if ((input[54]) >= (42.5)) {
			if ((input[56]) >= (17099.5)) {
				var26 = 0.050401967;
			}
			else {
				var26 = -0.090517215;
			}
		}
		else {
			if ((input[56]) >= (3167.5)) {
				var26 = 0.09483002;
			}
			else {
				var26 = 0.025503293;
			}
		}
	}
	else {
		if ((input[37]) >= (0.28785)) {
			if ((input[47]) >= (0.66075003)) {
				var26 = 0.038799386;
			}
			else {
				var26 = -0.0781429;
			}
		}
		else {
			if ((input[23]) >= (1.225)) {
				var26 = 0.043051664;
			}
			else {
				var26 = -0.09063104;
			}
		}
	}
	double var27;
	if ((input[48]) >= (21.6165)) {
		if ((input[54]) >= (42.5)) {
			if ((input[56]) >= (14903.5)) {
				var27 = 0.042622868;
			}
			else {
				var27 = -0.08880876;
			}
		}
		else {
			if ((input[56]) >= (4372.5)) {
				var27 = 0.095757596;
			}
			else {
				var27 = 0.03002703;
			}
		}
	}
	else {
		if ((input[37]) >= (0.28785)) {
			if ((input[3]) >= (0.1)) {
				var27 = -0.0044891736;
			}
			else {
				var27 = 0.079426475;
			}
		}
		else {
			if ((input[23]) >= (1.225)) {
				var27 = 0.03897384;
			}
			else {
				var27 = -0.087256245;
			}
		}
	}
	double var28;
	if ((input[60]) >= (1.5)) {
		if ((input[46]) >= (0.42535)) {
			if ((input[60]) >= (19.5)) {
				var28 = 0.09700757;
			}
			else {
				var28 = 0.04575894;
			}
		}
		else {
			if ((input[38]) >= (0.125)) {
				var28 = -0.20586148;
			}
			else {
				var28 = -0.111839615;
			}
		}
	}
	else {
		if ((input[37]) >= (0.83335)) {
			if ((input[47]) >= (1.84725)) {
				var28 = 0.08145058;
			}
			else {
				var28 = 0.019071346;
			}
		}
		else {
			if ((input[13]) >= (1.5)) {
				var28 = 0.04240125;
			}
			else {
				var28 = -0.059125245;
			}
		}
	}
	double var29;
	if ((input[22]) >= (0.5)) {
		if ((input[61]) >= (9095.5)) {
			if ((input[54]) >= (45.5)) {
				var29 = 0.013438145;
			}
			else {
				var29 = 0.09102267;
			}
		}
		else {
			if ((input[54]) >= (40.5)) {
				var29 = -0.054930415;
			}
			else {
				var29 = 0.037989084;
			}
		}
	}
	else {
		if ((input[37]) >= (0.28785)) {
			if ((input[3]) >= (0.1)) {
				var29 = -0.009911292;
			}
			else {
				var29 = 0.0808525;
			}
		}
		else {
			if ((input[12]) >= (0.01135)) {
				var29 = -0.010125211;
			}
			else {
				var29 = -0.09582837;
			}
		}
	}
	double var30;
	if ((input[2]) >= (1.5)) {
		if ((input[56]) >= (1483.5)) {
			if ((input[54]) >= (42.5)) {
				var30 = 0.04421015;
			}
			else {
				var30 = 0.088402085;
			}
		}
		else {
			if ((input[54]) >= (35.5)) {
				var30 = -0.05143639;
			}
			else {
				var30 = 0.06438138;
			}
		}
	}
	else {
		if ((input[37]) >= (0.80305)) {
			if ((input[47]) >= (1.8295)) {
				var30 = 0.076823756;
			}
			else {
				var30 = 0.019941563;
			}
		}
		else {
			if ((input[48]) >= (18.80785)) {
				var30 = 0.0013862932;
			}
			else {
				var30 = -0.06622773;
			}
		}
	}
	double var31;
	if ((input[60]) >= (1.5)) {
		if ((input[46]) >= (2.67735)) {
			if ((input[60]) >= (19.5)) {
				var31 = 0.0946879;
			}
			else {
				var31 = 0.044824507;
			}
		}
		else {
			if ((input[61]) >= (794.5)) {
				var31 = 0.04015683;
			}
			else {
				var31 = -0.12490406;
			}
		}
	}
	else {
		if ((input[37]) >= (0.98485)) {
			if ((input[60]) >= (0.5)) {
				var31 = 0.018305538;
			}
			else {
				var31 = 0.07953974;
			}
		}
		else {
			if ((input[61]) >= (2352.5)) {
				var31 = 0.0019990967;
			}
			else {
				var31 = -0.06541707;
			}
		}
	}
	double var32;
	if ((input[2]) >= (1.5)) {
		if ((input[1]) >= (0.0082)) {
			if ((input[13]) >= (1.5)) {
				var32 = 0.087805584;
			}
			else {
				var32 = 0.041127484;
			}
		}
		else {
			if ((input[32]) >= (2.5)) {
				var32 = -0.05773538;
			}
			else {
				var32 = 0.036505584;
			}
		}
	}
	else {
		if ((input[37]) >= (0.28785)) {
			if ((input[3]) >= (0.1)) {
				var32 = 0.008914505;
			}
			else {
				var32 = 0.07329979;
			}
		}
		else {
			if ((input[32]) >= (3.5)) {
				var32 = -0.0724898;
			}
			else {
				var32 = 0.01705796;
			}
		}
	}
	double var33;
	if ((input[61]) >= (4357.5)) {
		if ((input[54]) >= (43.5)) {
			if ((input[56]) >= (32930.5)) {
				var33 = 0.043920677;
			}
			else {
				var33 = -0.08001949;
			}
		}
		else {
			if ((input[54]) >= (42.5)) {
				var33 = 0.043364257;
			}
			else {
				var33 = 0.090038866;
			}
		}
	}
	else {
		if ((input[54]) >= (40.5)) {
			if ((input[37]) >= (0.2727)) {
				var33 = 0.008690794;
			}
			else {
				var33 = -0.10061345;
			}
		}
		else {
			if ((input[56]) >= (854.5)) {
				var33 = 0.04834022;
			}
			else {
				var33 = -0.039467838;
			}
		}
	}
	double var34;
	if ((input[56]) >= (10914.5)) {
		if ((input[54]) >= (45.5)) {
			if ((input[56]) >= (39123.5)) {
				var34 = 0.024264297;
			}
			else {
				var34 = -0.08684274;
			}
		}
		else {
			if ((input[1]) >= (0.0221)) {
				var34 = 0.08621006;
			}
			else {
				var34 = 0.0076114684;
			}
		}
	}
	else {
		if ((input[54]) >= (42.5)) {
			if ((input[38]) >= (0.41665)) {
				var34 = 0.019515224;
			}
			else {
				var34 = -0.09901513;
			}
		}
		else {
			if ((input[56]) >= (1000.5)) {
				var34 = 0.0432938;
			}
			else {
				var34 = -0.03162935;
			}
		}
	}
	double var35;
	if ((input[38]) >= (0.875)) {
		if ((input[47]) >= (1.61905)) {
			if ((input[45]) >= (101.571045)) {
				var35 = 0.09857552;
			}
			else {
				var35 = 0.07041137;
			}
		}
		else {
			if ((input[60]) >= (23.5)) {
				var35 = 0.08922906;
			}
			else {
				var35 = -0.009596314;
			}
		}
	}
	else {
		if ((input[12]) >= (0.0343)) {
			if ((input[54]) >= (42.5)) {
				var35 = -0.031013815;
			}
			else {
				var35 = 0.039491244;
			}
		}
		else {
			if ((input[37]) >= (0.28785)) {
				var35 = 0.017498663;
			}
			else {
				var35 = -0.076454505;
			}
		}
	}
	double var36;
	if ((input[56]) >= (13220.5)) {
		if ((input[54]) >= (45.5)) {
			if ((input[31]) >= (4.5)) {
				var36 = -0.07832608;
			}
			else {
				var36 = 0.026067013;
			}
		}
		else {
			if ((input[61]) >= (31638.0)) {
				var36 = 0.10152161;
			}
			else {
				var36 = 0.062949635;
			}
		}
	}
	else {
		if ((input[54]) >= (42.5)) {
			if ((input[37]) >= (0.2727)) {
				var36 = 0.0027800582;
			}
			else {
				var36 = -0.09926347;
			}
		}
		else {
			if ((input[56]) >= (1630.5)) {
				var36 = 0.050343275;
			}
			else {
				var36 = -0.020291442;
			}
		}
	}
	double var37;
	if ((input[56]) >= (5150.5)) {
		if ((input[54]) >= (43.5)) {
			if ((input[56]) >= (37340.0)) {
				var37 = 0.04229888;
			}
			else {
				var37 = -0.06805672;
			}
		}
		else {
			if ((input[54]) >= (42.5)) {
				var37 = 0.038982708;
			}
			else {
				var37 = 0.08395412;
			}
		}
	}
	else {
		if ((input[54]) >= (37.5)) {
			if ((input[37]) >= (0.98485)) {
				var37 = 0.053139593;
			}
			else {
				var37 = -0.061674654;
			}
		}
		else {
			if ((input[56]) >= (686.5)) {
				var37 = 0.064698674;
			}
			else {
				var37 = -0.0323905;
			}
		}
	}
	double var38;
	if ((input[57]) >= (0.5)) {
		if ((input[38]) >= (0.875)) {
			if ((input[47]) >= (1.84725)) {
				var38 = 0.07169685;
			}
			else {
				var38 = -0.008442667;
			}
		}
		else {
			if ((input[61]) >= (1211.5)) {
				var38 = -0.0011949266;
			}
			else {
				var38 = -0.054184556;
			}
		}
	}
	else {
		if ((input[47]) >= (0.012399999)) {
			if ((input[58]) >= (0.5)) {
				var38 = 0.04296408;
			}
			else {
				var38 = 0.089851655;
			}
		}
		else {
			if ((input[23]) >= (1.55)) {
				var38 = -0.010928037;
			}
			else {
				var38 = -0.14777985;
			}
		}
	}
	double var39;
	if ((input[54]) >= (45.5)) {
		if ((input[56]) >= (39123.5)) {
			if ((input[54]) >= (48.5)) {
				var39 = -0.08687481;
			}
			else {
				var39 = 0.044491872;
			}
		}
		else {
			if ((input[0]) >= (3.0)) {
				var39 = -0.027728906;
			}
			else {
				var39 = -0.11047272;
			}
		}
	}
	else {
		if ((input[56]) >= (4504.5)) {
			if ((input[54]) >= (42.5)) {
				var39 = 0.023889977;
			}
			else {
				var39 = 0.078095846;
			}
		}
		else {
			if ((input[54]) >= (37.5)) {
				var39 = -0.034016162;
			}
			else {
				var39 = 0.023740886;
			}
		}
	}
	double var40;
	if ((input[56]) >= (13947.0)) {
		if ((input[54]) >= (45.5)) {
			if ((input[32]) >= (3.5)) {
				var40 = -0.053605296;
			}
			else {
				var40 = 0.03991812;
			}
		}
		else {
			if ((input[1]) >= (0.0001)) {
				var40 = 0.08028329;
			}
			else {
				var40 = -0.01327052;
			}
		}
	}
	else {
		if ((input[54]) >= (42.5)) {
			if ((input[37]) >= (0.2727)) {
				var40 = 0.006842138;
			}
			else {
				var40 = -0.09406097;
			}
		}
		else {
			if ((input[61]) >= (1661.5)) {
				var40 = 0.046855055;
			}
			else {
				var40 = -0.013088052;
			}
		}
	}
	double var41;
	if ((input[60]) >= (23.5)) {
		if ((input[47]) >= (0.0147)) {
			if ((input[1]) >= (8.29575)) {
				var41 = 0.058541816;
			}
			else {
				var41 = 0.09297433;
			}
		}
		else {
			var41 = -0.14531685;
		}
	}
	else {
		if ((input[61]) >= (14147.5)) {
			if ((input[54]) >= (45.5)) {
				var41 = -0.010284117;
			}
			else {
				var41 = 0.06877417;
			}
		}
		else {
			if ((input[54]) >= (42.5)) {
				var41 = -0.060644455;
			}
			else {
				var41 = 0.0010586221;
			}
		}
	}
	double var42;
	if ((input[38]) >= (0.875)) {
		if ((input[47]) >= (1.84725)) {
			if ((input[3]) >= (0.36665)) {
				var42 = 0.06068624;
			}
			else {
				var42 = 0.096973516;
			}
		}
		else {
			if ((input[12]) >= (0.0006)) {
				var42 = -0.010385006;
			}
			else {
				var42 = 0.06913907;
			}
		}
	}
	else {
		if ((input[32]) >= (3.5)) {
			if ((input[37]) >= (0.28785)) {
				var42 = 0.016357793;
			}
			else {
				var42 = -0.051990997;
			}
		}
		else {
			if ((input[61]) >= (833.0)) {
				var42 = 0.033061318;
			}
			else {
				var42 = -0.03496017;
			}
		}
	}
	double var43;
	if ((input[22]) >= (1.5)) {
		if ((input[56]) >= (12302.0)) {
			if ((input[54]) >= (48.5)) {
				var43 = -0.13671908;
			}
			else {
				var43 = 0.078800365;
			}
		}
		else {
			if ((input[54]) >= (37.5)) {
				var43 = -0.009932913;
			}
			else {
				var43 = 0.061617594;
			}
		}
	}
	else {
		if ((input[37]) >= (0.98485)) {
			if ((input[60]) >= (0.5)) {
				var43 = 0.014311268;
			}
			else {
				var43 = 0.07358482;
			}
		}
		else {
			if ((input[12]) >= (0.45155)) {
				var43 = 0.0077252463;
			}
			else {
				var43 = -0.038011692;
			}
		}
	}
	double var44;
	if ((input[60]) >= (23.5)) {
		if ((input[47]) >= (0.0147)) {
			if ((input[1]) >= (8.29575)) {
				var44 = 0.052726936;
			}
			else {
				var44 = 0.09024089;
			}
		}
		else {
			var44 = -0.1337393;
		}
	}
	else {
		if ((input[56]) >= (31636.0)) {
			if ((input[54]) >= (48.5)) {
				var44 = -0.07556986;
			}
			else {
				var44 = 0.065641716;
			}
		}
		else {
			if ((input[54]) >= (42.5)) {
				var44 = -0.05065589;
			}
			else {
				var44 = 0.004890201;
			}
		}
	}
	double var45;
	if ((input[57]) >= (0.5)) {
		if ((input[37]) >= (0.98485)) {
			if ((input[3]) >= (0.325)) {
				var45 = 0.019660562;
			}
			else {
				var45 = 0.08643707;
			}
		}
		else {
			if ((input[61]) >= (1286.5)) {
				var45 = 0.0023161885;
			}
			else {
				var45 = -0.04258161;
			}
		}
	}
	else {
		if ((input[47]) >= (0.45695)) {
			if ((input[12]) >= (1.2461)) {
				var45 = 0.049228106;
			}
			else {
				var45 = 0.09296797;
			}
		}
		else {
			if ((input[3]) >= (1.1)) {
				var45 = 0.062068474;
			}
			else {
				var45 = -0.09869329;
			}
		}
	}
	double var46;
	if ((input[54]) >= (45.5)) {
		if ((input[56]) >= (43829.0)) {
			if ((input[32]) >= (4.5)) {
				var46 = -0.06654378;
			}
			else {
				var46 = 0.046102144;
			}
		}
		else {
			if ((input[0]) >= (3.0)) {
				var46 = -0.012259758;
			}
			else {
				var46 = -0.10813805;
			}
		}
	}
	else {
		if ((input[61]) >= (2276.5)) {
			if ((input[54]) >= (40.5)) {
				var46 = 0.012646099;
			}
			else {
				var46 = 0.069683366;
			}
		}
		else {
			if ((input[54]) >= (35.5)) {
				var46 = -0.023260703;
			}
			else {
				var46 = 0.04902229;
			}
		}
	}
	double var47;
	if ((input[47]) >= (0.0187)) {
		if ((input[57]) >= (0.5)) {
			if ((input[54]) >= (47.5)) {
				var47 = -0.075316064;
			}
			else {
				var47 = -0.00014589388;
			}
		}
		else {
			if ((input[51]) >= (0.01565)) {
				var47 = 0.0616031;
			}
			else {
				var47 = -0.015580959;
			}
		}
	}
	else {
		if ((input[45]) >= (73.0)) {
			if ((input[38]) >= (0.125)) {
				var47 = -0.1758064;
			}
			else {
				var47 = -0.09890374;
			}
		}
		else {
			if ((input[36]) >= (3.2881498)) {
				var47 = -0.1205129;
			}
			else {
				var47 = 0.0308727;
			}
		}
	}
	double var48;
	if ((input[56]) >= (16215.5)) {
		if ((input[54]) >= (45.5)) {
			if ((input[31]) >= (4.5)) {
				var48 = -0.05791045;
			}
			else {
				var48 = 0.026185587;
			}
		}
		else {
			if ((input[12]) >= (0.0001)) {
				var48 = 0.07339842;
			}
			else {
				var48 = -0.026205057;
			}
		}
	}
	else {
		if ((input[54]) >= (40.5)) {
			if ((input[37]) >= (0.07575)) {
				var48 = 0.006916661;
			}
			else {
				var48 = -0.069825724;
			}
		}
		else {
			if ((input[56]) >= (854.5)) {
				var48 = 0.039107375;
			}
			else {
				var48 = -0.02196589;
			}
		}
	}
	double var49;
	if ((input[47]) >= (0.0187)) {
		if ((input[54]) >= (37.5)) {
			if ((input[56]) >= (10914.5)) {
				var49 = 0.026997928;
			}
			else {
				var49 = -0.023764579;
			}
		}
		else {
			if ((input[56]) >= (743.5)) {
				var49 = 0.062127937;
			}
			else {
				var49 = -0.013085979;
			}
		}
	}
	else {
		if ((input[45]) >= (73.0)) {
			if ((input[38]) >= (0.125)) {
				var49 = -0.15880777;
			}
			else {
				var49 = -0.097221516;
			}
		}
		else {
			if ((input[36]) >= (3.2881498)) {
				var49 = -0.11770596;
			}
			else {
				var49 = 0.031059047;
			}
		}
	}
	double var50;
	if ((input[60]) >= (23.5)) {
		if ((input[47]) >= (0.0147)) {
			if ((input[1]) >= (6.5259)) {
				var50 = 0.04263347;
			}
			else {
				var50 = 0.084820814;
			}
		}
		else {
			var50 = -0.10318118;
		}
	}
	else {
		if ((input[37]) >= (0.98485)) {
			if ((input[12]) >= (0.0006)) {
				var50 = 0.01766358;
			}
			else {
				var50 = 0.07984907;
			}
		}
		else {
			if ((input[61]) >= (823.5)) {
				var50 = 0.0019131274;
			}
			else {
				var50 = -0.037983287;
			}
		}
	}
	double var51;
	if ((input[54]) >= (45.5)) {
		if ((input[56]) >= (43829.0)) {
			if ((input[46]) >= (3280.462)) {
				var51 = -0.19319598;
			}
			else {
				var51 = 0.026223952;
			}
		}
		else {
			if ((input[0]) >= (3.0)) {
				var51 = -0.008503004;
			}
			else {
				var51 = -0.1059796;
			}
		}
	}
	else {
		if ((input[61]) >= (3618.5)) {
			if ((input[54]) >= (40.5)) {
				var51 = 0.017640723;
			}
			else {
				var51 = 0.0726757;
			}
		}
		else {
			if ((input[54]) >= (35.5)) {
				var51 = -0.015441944;
			}
			else {
				var51 = 0.0473562;
			}
		}
	}
	double var52;
	if ((input[47]) >= (0.0187)) {
		if ((input[57]) >= (0.5)) {
			if ((input[37]) >= (0.28785)) {
				var52 = 0.015388779;
			}
			else {
				var52 = -0.017696476;
			}
		}
		else {
			if ((input[12]) >= (1.2461)) {
				var52 = 0.030274725;
			}
			else {
				var52 = 0.08256214;
			}
		}
	}
	else {
		if ((input[45]) >= (73.0)) {
			if ((input[23]) >= (1.55)) {
				var52 = 0.01632109;
			}
			else {
				var52 = -0.1249369;
			}
		}
		else {
			if ((input[36]) >= (3.8401499)) {
				var52 = -0.12917459;
			}
			else {
				var52 = 0.020887585;
			}
		}
	}
	double var53;
	if ((input[61]) >= (33238.5)) {
		if ((input[54]) >= (45.5)) {
			if ((input[1]) >= (10.09095)) {
				var53 = 0.12748565;
			}
			else {
				var53 = -0.009472644;
			}
		}
		else {
			if ((input[52]) >= (35.1251)) {
				var53 = 0.09109056;
			}
			else {
				var53 = -0.061339956;
			}
		}
	}
	else {
		if ((input[54]) >= (42.5)) {
			if ((input[37]) >= (0.045450002)) {
				var53 = 0.011724535;
			}
			else {
				var53 = -0.063722335;
			}
		}
		else {
			if ((input[61]) >= (1661.5)) {
				var53 = 0.0380875;
			}
			else {
				var53 = -0.009209092;
			}
		}
	}
	double var54;
	if ((input[54]) >= (37.5)) {
		if ((input[61]) >= (10645.0)) {
			if ((input[54]) >= (46.5)) {
				var54 = -0.035540003;
			}
			else {
				var54 = 0.038082737;
			}
		}
		else {
			if ((input[37]) >= (0.07575)) {
				var54 = 0.005923885;
			}
			else {
				var54 = -0.0464216;
			}
		}
	}
	else {
		if ((input[56]) >= (686.5)) {
			if ((input[12]) >= (0.0875)) {
				var54 = 0.07722836;
			}
			else {
				var54 = 0.012633847;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var54 = 0.016242502;
			}
			else {
				var54 = -0.0424108;
			}
		}
	}
	double var55;
	if ((input[47]) >= (0.0187)) {
		if ((input[54]) >= (40.5)) {
			if ((input[61]) >= (27997.0)) {
				var55 = 0.035158254;
			}
			else {
				var55 = -0.026373712;
			}
		}
		else {
			if ((input[61]) >= (2160.5)) {
				var55 = 0.055452075;
			}
			else {
				var55 = 0.0010463065;
			}
		}
	}
	else {
		if ((input[45]) >= (73.0)) {
			if ((input[23]) >= (1.55)) {
				var55 = 0.015944608;
			}
			else {
				var55 = -0.12063165;
			}
		}
		else {
			if ((input[45]) >= (16.0284)) {
				var55 = 0.03844625;
			}
			else {
				var55 = -0.1108199;
			}
		}
	}
	double var56;
	if ((input[37]) >= (1.7727499)) {
		if ((input[45]) >= (48.18045)) {
			if ((input[38]) >= (1.125)) {
				var56 = 0.09547375;
			}
			else {
				var56 = 0.0024056023;
			}
		}
		else {
			if ((input[45]) >= (47.66735)) {
				var56 = -0.18591271;
			}
			else {
				var56 = 0.067854285;
			}
		}
	}
	else {
		if ((input[0]) >= (1.5)) {
			if ((input[22]) >= (1.5)) {
				var56 = 0.039084624;
			}
			else {
				var56 = -0.00061873836;
			}
		}
		else {
			if ((input[54]) >= (44.5)) {
				var56 = -0.06968453;
			}
			else {
				var56 = -0.011146249;
			}
		}
	}
	double var57;
	if ((input[47]) >= (0.0187)) {
		if ((input[60]) >= (19.5)) {
			if ((input[1]) >= (8.29575)) {
				var57 = 0.015050407;
			}
			else {
				var57 = 0.07840798;
			}
		}
		else {
			if ((input[54]) >= (47.5)) {
				var57 = -0.062438566;
			}
			else {
				var57 = 0.0018557374;
			}
		}
	}
	else {
		if ((input[45]) >= (73.0)) {
			if ((input[23]) >= (1.55)) {
				var57 = 0.01401783;
			}
			else {
				var57 = -0.11642195;
			}
		}
		else {
			if ((input[36]) >= (3.8401499)) {
				var57 = -0.12551904;
			}
			else {
				var57 = 0.026053393;
			}
		}
	}
	double var58;
	if ((input[61]) >= (33238.5)) {
		if ((input[54]) >= (45.5)) {
			if ((input[1]) >= (10.09095)) {
				var58 = 0.12486116;
			}
			else {
				var58 = -0.006180509;
			}
		}
		else {
			if ((input[52]) >= (35.1251)) {
				var58 = 0.08527972;
			}
			else {
				var58 = -0.06372298;
			}
		}
	}
	else {
		if ((input[54]) >= (42.5)) {
			if ((input[37]) >= (0.045450002)) {
				var58 = 0.011698178;
			}
			else {
				var58 = -0.055642415;
			}
		}
		else {
			if ((input[61]) >= (3618.5)) {
				var58 = 0.04743397;
			}
			else {
				var58 = -0.0035705843;
			}
		}
	}
	double var59;
	if ((input[37]) >= (1.7727499)) {
		if ((input[45]) >= (48.18045)) {
			if ((input[50]) >= (1.9408)) {
				var59 = 0.09833131;
			}
			else {
				var59 = 0.051315136;
			}
		}
		else {
			if ((input[45]) >= (47.66735)) {
				var59 = -0.15962593;
			}
			else {
				var59 = 0.064367995;
			}
		}
	}
	else {
		if ((input[54]) >= (37.5)) {
			if ((input[61]) >= (8310.0)) {
				var59 = 0.017494053;
			}
			else {
				var59 = -0.023499856;
			}
		}
		else {
			if ((input[56]) >= (734.5)) {
				var59 = 0.046021245;
			}
			else {
				var59 = -0.015479821;
			}
		}
	}
	double var60;
	if ((input[50]) >= (0.0038)) {
		if ((input[39]) >= (2.875)) {
			if ((input[50]) >= (1.7827001)) {
				var60 = 0.054602828;
			}
			else {
				var60 = -0.027446708;
			}
		}
		else {
			if ((input[48]) >= (1.45735)) {
				var60 = -0.009423847;
			}
			else {
				var60 = 0.037610937;
			}
		}
	}
	else {
		if ((input[45]) >= (73.0)) {
			if ((input[23]) >= (1.55)) {
				var60 = 0.028264517;
			}
			else {
				var60 = -0.11424563;
			}
		}
		else {
			if ((input[45]) >= (21.50025)) {
				var60 = 0.044129174;
			}
			else {
				var60 = -0.106872514;
			}
		}
	}
	double var61;
	if ((input[37]) >= (1.7727499)) {
		if ((input[45]) >= (48.18045)) {
			if ((input[50]) >= (1.9408)) {
				var61 = 0.09770256;
			}
			else {
				var61 = 0.049345776;
			}
		}
		else {
			if ((input[45]) >= (45.9164)) {
				var61 = -0.1030823;
			}
			else {
				var61 = 0.06860091;
			}
		}
	}
	else {
		if ((input[12]) >= (0.45155)) {
			if ((input[52]) >= (4.1858997)) {
				var61 = 0.022342917;
			}
			else {
				var61 = -0.07254795;
			}
		}
		else {
			if ((input[37]) >= (0.28785)) {
				var61 = 0.017257694;
			}
			else {
				var61 = -0.03750001;
			}
		}
	}
	double var62;
	if ((input[54]) >= (48.5)) {
		if ((input[61]) >= (533765.0)) {
			if ((input[47]) >= (4.8905497)) {
				var62 = -0.063238315;
			}
			else {
				var62 = 0.121839784;
			}
		}
		else {
			if ((input[25]) >= (2.1)) {
				var62 = 0.038564704;
			}
			else {
				var62 = -0.11271917;
			}
		}
	}
	else {
		if ((input[61]) >= (33238.5)) {
			if ((input[46]) >= (3287.359)) {
				var62 = -0.10902532;
			}
			else {
				var62 = 0.0570112;
			}
		}
		else {
			if ((input[54]) >= (40.5)) {
				var62 = -0.019246975;
			}
			else {
				var62 = 0.009614333;
			}
		}
	}
	double var63;
	if ((input[0]) >= (1.5)) {
		if ((input[61]) >= (885.5)) {
			if ((input[54]) >= (40.5)) {
				var63 = 0.003431109;
			}
			else {
				var63 = 0.045922812;
			}
		}
		else {
			if ((input[38]) >= (0.125)) {
				var63 = 0.008239297;
			}
			else {
				var63 = -0.049651865;
			}
		}
	}
	else {
		if ((input[54]) >= (44.5)) {
			if ((input[56]) >= (316785.0)) {
				var63 = 0.046722163;
			}
			else {
				var63 = -0.09137334;
			}
		}
		else {
			if ((input[61]) >= (17789.0)) {
				var63 = 0.05921728;
			}
			else {
				var63 = -0.015303413;
			}
		}
	}
	double var64;
	if ((input[37]) >= (1.80305)) {
		if ((input[45]) >= (48.19215)) {
			if ((input[49]) >= (3.7337)) {
				var64 = 0.005574602;
			}
			else {
				var64 = 0.093821965;
			}
		}
		else {
			if ((input[45]) >= (47.66735)) {
				var64 = -0.12748918;
			}
			else {
				var64 = 0.056326877;
			}
		}
	}
	else {
		if ((input[22]) >= (1.5)) {
			if ((input[31]) >= (4.5)) {
				var64 = -0.0145857185;
			}
			else {
				var64 = 0.03556584;
			}
		}
		else {
			if ((input[45]) >= (21.53405)) {
				var64 = -0.0038183245;
			}
			else {
				var64 = -0.054552346;
			}
		}
	}
	double var65;
	if ((input[60]) >= (23.5)) {
		if ((input[1]) >= (6.5259)) {
			if ((input[3]) >= (1.29165)) {
				var65 = 0.07294849;
			}
			else {
				var65 = 0.0018324973;
			}
		}
		else {
			if ((input[47]) >= (0.13275)) {
				var65 = 0.07448992;
			}
			else {
				var65 = -0.090263076;
			}
		}
	}
	else {
		if ((input[56]) >= (31636.0)) {
			if ((input[23]) >= (0.45)) {
				var65 = 0.059513237;
			}
			else {
				var65 = -0.007961994;
			}
		}
		else {
			if ((input[54]) >= (45.5)) {
				var65 = -0.051049422;
			}
			else {
				var65 = -0.0017867323;
			}
		}
	}
	double var66;
	if ((input[47]) >= (0.0187)) {
		if ((input[48]) >= (1.45735)) {
			if ((input[45]) >= (88.91945)) {
				var66 = 0.020613648;
			}
			else {
				var66 = -0.017683178;
			}
		}
		else {
			if ((input[38]) >= (0.125)) {
				var66 = 0.064786635;
			}
			else {
				var66 = -0.05461303;
			}
		}
	}
	else {
		if ((input[36]) >= (3.2814)) {
			if ((input[23]) >= (1.55)) {
				var66 = 0.026168425;
			}
			else {
				var66 = -0.10808124;
			}
		}
		else {
			if ((input[36]) >= (3.0359)) {
				var66 = 0.047727033;
			}
			else {
				var66 = -0.10147452;
			}
		}
	}
	double var67;
	if ((input[54]) >= (47.5)) {
		if ((input[61]) >= (533765.0)) {
			if ((input[47]) >= (4.94955)) {
				var67 = -0.03959625;
			}
			else {
				var67 = 0.100060225;
			}
		}
		else {
			if ((input[12]) >= (3.5353)) {
				var67 = 0.009570448;
			}
			else {
				var67 = -0.104512945;
			}
		}
	}
	else {
		if ((input[61]) >= (8309.5)) {
			if ((input[1]) >= (0.032849997)) {
				var67 = 0.037130013;
			}
			else {
				var67 = -0.041209318;
			}
		}
		else {
			if ((input[23]) >= (0.1)) {
				var67 = -0.015937418;
			}
			else {
				var67 = 0.018267054;
			}
		}
	}
	double var68;
	if ((input[54]) >= (35.5)) {
		if ((input[61]) >= (2375.5)) {
			if ((input[54]) >= (40.5)) {
				var68 = -0.0040187803;
			}
			else {
				var68 = 0.048883762;
			}
		}
		else {
			if ((input[37]) >= (0.28785)) {
				var68 = 0.007201136;
			}
			else {
				var68 = -0.045030054;
			}
		}
	}
	else {
		if ((input[1]) >= (0.00255)) {
			if ((input[37]) >= (0.045450002)) {
				var68 = 0.054328967;
			}
			else {
				var68 = 0.13232481;
			}
		}
		else {
			if ((input[26]) >= (0.5)) {
				var68 = -0.036017023;
			}
			else {
				var68 = 0.050279558;
			}
		}
	}
	double var69;
	if ((input[0]) >= (1.5)) {
		if ((input[61]) >= (815.5)) {
			if ((input[54]) >= (40.5)) {
				var69 = 0.0030861842;
			}
			else {
				var69 = 0.040841185;
			}
		}
		else {
			if ((input[38]) >= (0.125)) {
				var69 = 0.0057700165;
			}
			else {
				var69 = -0.046273712;
			}
		}
	}
	else {
		if ((input[54]) >= (44.5)) {
			if ((input[56]) >= (316785.0)) {
				var69 = 0.041211497;
			}
			else {
				var69 = -0.0864726;
			}
		}
		else {
			if ((input[61]) >= (27620.0)) {
				var69 = 0.07326733;
			}
			else {
				var69 = -0.012606482;
			}
		}
	}
	double var70;
	if ((input[51]) >= (0.00645)) {
		if ((input[48]) >= (1.45735)) {
			if ((input[12]) >= (0.45155)) {
				var70 = 0.017064396;
			}
			else {
				var70 = -0.020112565;
			}
		}
		else {
			if ((input[38]) >= (0.125)) {
				var70 = 0.070224345;
			}
			else {
				var70 = -0.044138398;
			}
		}
	}
	else {
		if ((input[41]) >= (3.5)) {
			if ((input[45]) >= (112.3927)) {
				var70 = -0.08404856;
			}
			else {
				var70 = 0.017110787;
			}
		}
		else {
			if ((input[51]) >= (0.00465)) {
				var70 = -0.17571759;
			}
			else {
				var70 = -0.05732553;
			}
		}
	}
	double var71;
	if ((input[37]) >= (1.80305)) {
		if ((input[45]) >= (48.19215)) {
			if ((input[52]) >= (0.517)) {
				var71 = 0.09239311;
			}
			else {
				var71 = 0.0054583373;
			}
		}
		else {
			if ((input[45]) >= (45.91445)) {
				var71 = -0.08915223;
			}
			else {
				var71 = 0.05964556;
			}
		}
	}
	else {
		if ((input[54]) >= (35.5)) {
			if ((input[61]) >= (1698.5)) {
				var71 = 0.007893401;
			}
			else {
				var71 = -0.019828305;
			}
		}
		else {
			if ((input[1]) >= (0.0033)) {
				var71 = 0.083898835;
			}
			else {
				var71 = -0.01733303;
			}
		}
	}
	double var72;
	if ((input[54]) >= (48.5)) {
		if ((input[61]) >= (533765.0)) {
			if ((input[47]) >= (4.8905497)) {
				var72 = -0.06680529;
			}
			else {
				var72 = 0.10665929;
			}
		}
		else {
			if ((input[12]) >= (12.9205)) {
				var72 = 0.03472912;
			}
			else {
				var72 = -0.11208236;
			}
		}
	}
	else {
		if ((input[61]) >= (33238.5)) {
			if ((input[46]) >= (2850.7495)) {
				var72 = -0.095490925;
			}
			else {
				var72 = 0.050217267;
			}
		}
		else {
			if ((input[54]) >= (40.5)) {
				var72 = -0.015912762;
			}
			else {
				var72 = 0.008165876;
			}
		}
	}
	double var73;
	if ((input[37]) >= (1.80305)) {
		if ((input[45]) >= (48.19215)) {
			if ((input[47]) >= (1.5908501)) {
				var73 = 0.0956549;
			}
			else {
				var73 = 0.042745072;
			}
		}
		else {
			if ((input[45]) >= (45.91445)) {
				var73 = -0.07732491;
			}
			else {
				var73 = 0.0567951;
			}
		}
	}
	else {
		if ((input[0]) >= (1.5)) {
			if ((input[61]) >= (885.5)) {
				var73 = 0.014894664;
			}
			else {
				var73 = -0.009188628;
			}
		}
		else {
			if ((input[54]) >= (40.5)) {
				var73 = -0.03909054;
			}
			else {
				var73 = -0.0002054496;
			}
		}
	}
	double var74;
	if ((input[45]) >= (88.91945)) {
		if ((input[61]) >= (1102.5)) {
			if ((input[41]) >= (3.5)) {
				var74 = 0.0093419915;
			}
			else {
				var74 = 0.054975796;
			}
		}
		else {
			if ((input[46]) >= (2.39605)) {
				var74 = -0.0034990571;
			}
			else {
				var74 = -0.08611912;
			}
		}
	}
	else {
		if ((input[48]) >= (1.4624)) {
			if ((input[12]) >= (0.4603)) {
				var74 = 0.004528647;
			}
			else {
				var74 = -0.030166162;
			}
		}
		else {
			if ((input[38]) >= (0.125)) {
				var74 = 0.061768573;
			}
			else {
				var74 = -0.052485492;
			}
		}
	}
	double var75;
	if ((input[54]) >= (35.5)) {
		if ((input[61]) >= (4357.5)) {
			if ((input[54]) >= (43.5)) {
				var75 = -0.01665504;
			}
			else {
				var75 = 0.037127525;
			}
		}
		else {
			if ((input[23]) >= (0.1)) {
				var75 = -0.030123753;
			}
			else {
				var75 = 0.015985308;
			}
		}
	}
	else {
		if ((input[1]) >= (0.0033)) {
			if ((input[38]) >= (0.125)) {
				var75 = 0.05866445;
			}
			else {
				var75 = 0.14900349;
			}
		}
		else {
			if ((input[43]) >= (0.5)) {
				var75 = -0.13060108;
			}
			else {
				var75 = -0.0063591166;
			}
		}
	}
	double var76;
	if ((input[56]) >= (324163.5)) {
		if ((input[54]) >= (48.5)) {
			if ((input[61]) >= (533765.0)) {
				var76 = 0.04715614;
			}
			else {
				var76 = -0.0996116;
			}
		}
		else {
			if ((input[48]) >= (888.08844)) {
				var76 = 0.0884927;
			}
			else {
				var76 = -0.04828882;
			}
		}
	}
	else {
		if ((input[54]) >= (47.5)) {
			if ((input[12]) >= (3.5353)) {
				var76 = 0.007818438;
			}
			else {
				var76 = -0.104293995;
			}
		}
		else {
			if ((input[3]) >= (0.36665)) {
				var76 = -0.008698313;
			}
			else {
				var76 = 0.01635318;
			}
		}
	}
	double var77;
	if ((input[0]) >= (1.5)) {
		if ((input[22]) >= (1.5)) {
			if ((input[37]) >= (0.07575)) {
				var77 = 0.0053170584;
			}
			else {
				var77 = 0.04505452;
			}
		}
		else {
			if ((input[23]) >= (0.1)) {
				var77 = -0.0068676085;
			}
			else {
				var77 = 0.020238131;
			}
		}
	}
	else {
		if ((input[54]) >= (40.5)) {
			if ((input[61]) >= (17807.0)) {
				var77 = 0.004278701;
			}
			else {
				var77 = -0.06005187;
			}
		}
		else {
			if ((input[61]) >= (2127.5)) {
				var77 = 0.03256702;
			}
			else {
				var77 = -0.016416393;
			}
		}
	}
	double var78;
	if ((input[37]) >= (0.98485)) {
		if ((input[60]) >= (0.5)) {
			if ((input[23]) >= (1.225)) {
				var78 = 0.037175592;
			}
			else {
				var78 = -0.04779038;
			}
		}
		else {
			if ((input[37]) >= (1.0151501)) {
				var78 = 0.01377361;
			}
			else {
				var78 = 0.08917929;
			}
		}
	}
	else {
		if ((input[47]) >= (2.1735501)) {
			if ((input[56]) >= (39594.0)) {
				var78 = 0.026157161;
			}
			else {
				var78 = -0.02514458;
			}
		}
		else {
			if ((input[37]) >= (0.5)) {
				var78 = -0.03924499;
			}
			else {
				var78 = 0.022384714;
			}
		}
	}
	double var79;
	if ((input[50]) >= (0.0038)) {
		if ((input[48]) >= (1.45735)) {
			if ((input[45]) >= (88.91945)) {
				var79 = 0.018009674;
			}
			else {
				var79 = -0.015007033;
			}
		}
		else {
			if ((input[45]) >= (41.667)) {
				var79 = -0.07700626;
			}
			else {
				var79 = 0.045260962;
			}
		}
	}
	else {
		if ((input[36]) >= (3.65865)) {
			if ((input[61]) >= (829.0)) {
				var79 = 0.003489957;
			}
			else {
				var79 = -0.11331763;
			}
		}
		else {
			if ((input[45]) >= (73.0)) {
				var79 = -0.08386341;
			}
			else {
				var79 = 0.042958554;
			}
		}
	}
	double var80;
	if ((input[37]) >= (1.80305)) {
		if ((input[45]) >= (48.19215)) {
			if ((input[47]) >= (1.5908501)) {
				var80 = 0.094731145;
			}
			else {
				var80 = 0.042047564;
			}
		}
		else {
			if ((input[45]) >= (47.66735)) {
				var80 = -0.090625666;
			}
			else {
				var80 = 0.0478207;
			}
		}
	}
	else {
		if ((input[54]) >= (37.5)) {
			if ((input[3]) >= (0.45)) {
				var80 = -0.022905773;
			}
			else {
				var80 = 0.0061710547;
			}
		}
		else {
			if ((input[56]) >= (686.5)) {
				var80 = 0.036670856;
			}
			else {
				var80 = -0.015840111;
			}
		}
	}
	double var81;
	if ((input[56]) >= (13721.0)) {
		if ((input[54]) >= (43.5)) {
			if ((input[25]) >= (1.1)) {
				var81 = 0.04088048;
			}
			else {
				var81 = -0.022324137;
			}
		}
		else {
			if ((input[12]) >= (0.0001)) {
				var81 = 0.060722616;
			}
			else {
				var81 = -0.045109;
			}
		}
	}
	else {
		if ((input[54]) >= (40.5)) {
			if ((input[1]) >= (0.0021000002)) {
				var81 = -0.042534065;
			}
			else {
				var81 = 0.018074019;
			}
		}
		else {
			if ((input[61]) >= (1634.5)) {
				var81 = 0.032794088;
			}
			else {
				var81 = -0.0065268935;
			}
		}
	}
	double var82;
	if ((input[56]) >= (324163.5)) {
		if ((input[54]) >= (48.5)) {
			if ((input[62]) >= (32.0)) {
				var82 = 0.034587976;
			}
			else {
				var82 = -0.1273907;
			}
		}
		else {
			if ((input[36]) >= (6.00465)) {
				var82 = 0.08632556;
			}
			else {
				var82 = -0.047724184;
			}
		}
	}
	else {
		if ((input[54]) >= (47.5)) {
			if ((input[1]) >= (8.7865)) {
				var82 = 0.065226935;
			}
			else {
				var82 = -0.08731112;
			}
		}
		else {
			if ((input[3]) >= (0.36665)) {
				var82 = -0.0073958905;
			}
			else {
				var82 = 0.014216283;
			}
		}
	}
	double var83;
	if ((input[54]) >= (35.5)) {
		if ((input[61]) >= (4357.5)) {
			if ((input[54]) >= (43.5)) {
				var83 = -0.013998632;
			}
			else {
				var83 = 0.034918856;
			}
		}
		else {
			if ((input[23]) >= (0.1)) {
				var83 = -0.025415776;
			}
			else {
				var83 = 0.011385786;
			}
		}
	}
	else {
		if ((input[1]) >= (0.00255)) {
			if ((input[56]) >= (348.5)) {
				var83 = 0.091947734;
			}
			else {
				var83 = -0.01744734;
			}
		}
		else {
			if ((input[56]) >= (2844.0)) {
				var83 = -0.1536604;
			}
			else {
				var83 = -0.008775137;
			}
		}
	}
	double var84;
	if ((input[0]) >= (1.5)) {
		if ((input[1]) >= (0.2694)) {
			if ((input[38]) >= (0.125)) {
				var84 = -0.0021580919;
			}
			else {
				var84 = 0.050282754;
			}
		}
		else {
			if ((input[48]) >= (2.8059)) {
				var84 = -0.018722875;
			}
			else {
				var84 = 0.029763479;
			}
		}
	}
	else {
		if ((input[54]) >= (44.5)) {
			if ((input[56]) >= (221725.0)) {
				var84 = 0.022000689;
			}
			else {
				var84 = -0.08168993;
			}
		}
		else {
			if ((input[61]) >= (27620.0)) {
				var84 = 0.06282155;
			}
			else {
				var84 = -0.009985537;
			}
		}
	}
	double var85;
	if ((input[50]) >= (0.0038)) {
		if ((input[45]) >= (89.13125)) {
			if ((input[61]) >= (2293.5)) {
				var85 = 0.040483933;
			}
			else {
				var85 = -0.0028228753;
			}
		}
		else {
			if ((input[48]) >= (1.4624)) {
				var85 = -0.013405164;
			}
			else {
				var85 = 0.031508062;
			}
		}
	}
	else {
		if ((input[36]) >= (3.8401499)) {
			if ((input[61]) >= (829.0)) {
				var85 = 0.02583844;
			}
			else {
				var85 = -0.11592404;
			}
		}
		else {
			if ((input[45]) >= (73.0)) {
				var85 = -0.08079468;
			}
			else {
				var85 = 0.03673728;
			}
		}
	}
	double var86;
	if ((input[37]) >= (1.80305)) {
		if ((input[45]) >= (48.19215)) {
			if ((input[47]) >= (1.5908501)) {
				var86 = 0.09377961;
			}
			else {
				var86 = 0.039083533;
			}
		}
		else {
			if ((input[45]) >= (45.91445)) {
				var86 = -0.05811285;
			}
			else {
				var86 = 0.052068293;
			}
		}
	}
	else {
		if ((input[54]) >= (37.5)) {
			if ((input[3]) >= (0.45)) {
				var86 = -0.020857828;
			}
			else {
				var86 = 0.005754818;
			}
		}
		else {
			if ((input[12]) >= (0.0917)) {
				var86 = 0.040566277;
			}
			else {
				var86 = -0.009418996;
			}
		}
	}
	double var87;
	if ((input[51]) >= (0.01375)) {
		if ((input[39]) >= (2.875)) {
			if ((input[12]) >= (19.6664)) {
				var87 = -0.06249663;
			}
			else {
				var87 = 0.042596277;
			}
		}
		else {
			if ((input[37]) >= (0.95455)) {
				var87 = 0.03207754;
			}
			else {
				var87 = -0.00529414;
			}
		}
	}
	else {
		if ((input[12]) >= (0.22525)) {
			if ((input[61]) >= (513.0)) {
				var87 = -0.033055205;
			}
			else {
				var87 = -0.09591731;
			}
		}
		else {
			if ((input[53]) >= (0.64814997)) {
				var87 = -0.009688125;
			}
			else {
				var87 = 0.06771498;
			}
		}
	}
	double var88;
	if ((input[61]) >= (1698.5)) {
		if ((input[54]) >= (40.5)) {
			if ((input[45]) >= (88.91945)) {
				var88 = 0.02082316;
			}
			else {
				var88 = -0.01809702;
			}
		}
		else {
			if ((input[12]) >= (0.0001)) {
				var88 = 0.047368154;
			}
			else {
				var88 = -0.0709648;
			}
		}
	}
	else {
		if ((input[38]) >= (0.125)) {
			if ((input[12]) >= (0.0011)) {
				var88 = -0.029543702;
			}
			else {
				var88 = 0.05054146;
			}
		}
		else {
			if ((input[1]) >= (0.0023)) {
				var88 = 0.001483432;
			}
			else {
				var88 = -0.07800613;
			}
		}
	}
	double var89;
	if ((input[54]) >= (35.5)) {
		if ((input[61]) >= (6123.0)) {
			if ((input[54]) >= (42.5)) {
				var89 = -0.0012470012;
			}
			else {
				var89 = 0.048088714;
			}
		}
		else {
			if ((input[23]) >= (0.1)) {
				var89 = -0.02156486;
			}
			else {
				var89 = 0.011207201;
			}
		}
	}
	else {
		if ((input[1]) >= (0.0033)) {
			if ((input[56]) >= (348.5)) {
				var89 = 0.08903425;
			}
			else {
				var89 = -0.010108916;
			}
		}
		else {
			if ((input[38]) >= (0.41665)) {
				var89 = -0.052008014;
			}
			else {
				var89 = 0.00992801;
			}
		}
	}
	double var90;
	if ((input[61]) >= (529385.0)) {
		if ((input[47]) >= (4.94955)) {
			if ((input[3]) >= (1.125)) {
				var90 = 0.061163194;
			}
			else {
				var90 = -0.108120166;
			}
		}
		else {
			if ((input[20]) >= (2.5)) {
				var90 = 0.014378421;
			}
			else {
				var90 = 0.105321765;
			}
		}
	}
	else {
		if ((input[54]) >= (48.5)) {
			if ((input[25]) >= (2.1)) {
				var90 = 0.05512966;
			}
			else {
				var90 = -0.10688688;
			}
		}
		else {
			if ((input[3]) >= (0.36665)) {
				var90 = -0.006220218;
			}
			else {
				var90 = 0.011403366;
			}
		}
	}
	double var91;
	if ((input[61]) >= (457.5)) {
		if ((input[54]) >= (37.5)) {
			if ((input[13]) >= (0.5)) {
				var91 = -0.016488707;
			}
			else {
				var91 = 0.013070941;
			}
		}
		else {
			if ((input[12]) >= (0.0875)) {
				var91 = 0.05687175;
			}
			else {
				var91 = -0.0050971876;
			}
		}
	}
	else {
		if ((input[37]) >= (0.28785)) {
			if ((input[60]) >= (0.5)) {
				var91 = -0.07057425;
			}
			else {
				var91 = 0.060817398;
			}
		}
		else {
			if ((input[54]) >= (35.5)) {
				var91 = -0.083210856;
			}
			else {
				var91 = 0.00407176;
			}
		}
	}
	double var92;
	if ((input[61]) >= (27997.0)) {
		if ((input[46]) >= (2850.7495)) {
			if ((input[61]) >= (206758.0)) {
				var92 = -0.016217118;
			}
			else {
				var92 = -0.25456455;
			}
		}
		else {
			if ((input[1]) >= (0.00035000002)) {
				var92 = 0.04176493;
			}
			else {
				var92 = -0.057501912;
			}
		}
	}
	else {
		if ((input[54]) >= (40.5)) {
			if ((input[37]) >= (0.98485)) {
				var92 = 0.049570125;
			}
			else {
				var92 = -0.023692347;
			}
		}
		else {
			if ((input[12]) >= (0.46324998)) {
				var92 = 0.029704256;
			}
			else {
				var92 = -0.005439987;
			}
		}
	}
	double var93;
	if ((input[0]) >= (1.5)) {
		if ((input[61]) >= (815.5)) {
			if ((input[54]) >= (40.5)) {
				var93 = 0.0023963433;
			}
			else {
				var93 = 0.032136787;
			}
		}
		else {
			if ((input[51]) >= (0.107)) {
				var93 = -0.09049355;
			}
			else {
				var93 = -0.0031559027;
			}
		}
	}
	else {
		if ((input[54]) >= (44.5)) {
			if ((input[56]) >= (39066.0)) {
				var93 = -0.0037794579;
			}
			else {
				var93 = -0.09634676;
			}
		}
		else {
			if ((input[61]) >= (17789.0)) {
				var93 = 0.041592237;
			}
			else {
				var93 = -0.010642725;
			}
		}
	}
	double var94;
	if ((input[61]) >= (8565.0)) {
		if ((input[47]) >= (2.16355)) {
			if ((input[54]) >= (42.5)) {
				var94 = -0.011781139;
			}
			else {
				var94 = 0.04055415;
			}
		}
		else {
			if ((input[48]) >= (56.4308)) {
				var94 = 0.037423655;
			}
			else {
				var94 = 0.13516228;
			}
		}
	}
	else {
		if ((input[51]) >= (0.14335)) {
			if ((input[32]) >= (2.5)) {
				var94 = -0.052577507;
			}
			else {
				var94 = 0.011426127;
			}
		}
		else {
			if ((input[12]) >= (14.738649)) {
				var94 = -0.091393694;
			}
			else {
				var94 = 0.0025042575;
			}
		}
	}
	double var95;
	if ((input[56]) >= (324163.5)) {
		if ((input[54]) >= (48.5)) {
			if ((input[62]) >= (32.0)) {
				var95 = 0.030137867;
			}
			else {
				var95 = -0.11799874;
			}
		}
		else {
			if ((input[36]) >= (6.00465)) {
				var95 = 0.08250187;
			}
			else {
				var95 = -0.047375005;
			}
		}
	}
	else {
		if ((input[54]) >= (47.5)) {
			if ((input[12]) >= (3.5353)) {
				var95 = 0.009035288;
			}
			else {
				var95 = -0.09764668;
			}
		}
		else {
			if ((input[3]) >= (0.36665)) {
				var95 = -0.0067100385;
			}
			else {
				var95 = 0.013038608;
			}
		}
	}
	double var96;
	if ((input[25]) >= (1.45)) {
		if ((input[38]) >= (0.125)) {
			if ((input[37]) >= (-0.5)) {
				var96 = -0.006074687;
			}
			else {
				var96 = 0.079760164;
			}
		}
		else {
			if ((input[61]) >= (183.5)) {
				var96 = 0.05482277;
			}
			else {
				var96 = -0.108222544;
			}
		}
	}
	else {
		if ((input[36]) >= (6.0075502)) {
			if ((input[61]) >= (474.5)) {
				var96 = -0.0091856895;
			}
			else {
				var96 = -0.07435625;
			}
		}
		else {
			if ((input[37]) >= (0.28785)) {
				var96 = 0.02285068;
			}
			else {
				var96 = -0.010947779;
			}
		}
	}
	double var97;
	if ((input[45]) >= (170.5885)) {
		if ((input[56]) >= (15076.5)) {
			var97 = 0.05484348;
		}
		else {
			if ((input[42]) >= (1.5)) {
				var97 = 0.012917655;
			}
			else {
				var97 = -0.15885977;
			}
		}
	}
	else {
		if ((input[24]) >= (8.0)) {
			if ((input[57]) >= (0.5)) {
				var97 = 0.0706496;
			}
			else {
				var97 = -0.003923563;
			}
		}
		else {
			if ((input[60]) >= (19.5)) {
				var97 = 0.049223933;
			}
			else {
				var97 = -0.0031971235;
			}
		}
	}
	double var98;
	if ((input[54]) >= (35.5)) {
		if ((input[61]) >= (2375.5)) {
			if ((input[38]) >= (0.125)) {
				var98 = -0.013601369;
			}
			else {
				var98 = 0.025076522;
			}
		}
		else {
			if ((input[37]) >= (0.28785)) {
				var98 = 0.005929903;
			}
			else {
				var98 = -0.03476755;
			}
		}
	}
	else {
		if ((input[1]) >= (0.0033)) {
			if ((input[49]) >= (0.47855002)) {
				var98 = 0.080697864;
			}
			else {
				var98 = -0.040428814;
			}
		}
		else {
			if ((input[10]) >= (4.5)) {
				var98 = -0.062094178;
			}
			else {
				var98 = 0.0060117934;
			}
		}
	}
	double var99;
	if ((input[51]) >= (0.01375)) {
		if ((input[48]) >= (1.4545)) {
			if ((input[12]) >= (0.3268)) {
				var99 = 0.013078565;
			}
			else {
				var99 = -0.0152174905;
			}
		}
		else {
			if ((input[38]) >= (0.125)) {
				var99 = 0.06637169;
			}
			else {
				var99 = -0.036639363;
			}
		}
	}
	else {
		if ((input[12]) >= (0.22525)) {
			if ((input[36]) >= (6.20525)) {
				var99 = 0.0031058348;
			}
			else {
				var99 = -0.06946925;
			}
		}
		else {
			if ((input[13]) >= (0.5)) {
				var99 = 0.04664087;
			}
			else {
				var99 = -0.016157066;
			}
		}
	}
	double var100;
	var100 = (1.0) / ((1.0) + (exp((0.0) - ((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((var0)+(var1)) + (var2)) + (var3)) + (var4)) + (var5)) + (var6)) + (var7)) + (var8)) + (var9)) + (var10)) + (var11)) + (var12)) + (var13)) + (var14)) + (var15)) + (var16)) + (var17)) + (var18)) + (var19)) + (var20)) + (var21)) + (var22)) + (var23)) + (var24)) + (var25)) + (var26)) + (var27)) + (var28)) + (var29)) + (var30)) + (var31)) + (var32)) + (var33)) + (var34)) + (var35)) + (var36)) + (var37)) + (var38)) + (var39)) + (var40)) + (var41)) + (var42)) + (var43)) + (var44)) + (var45)) + (var46)) + (var47)) + (var48)) + (var49)) + (var50)) + (var51)) + (var52)) + (var53)) + (var54)) + (var55)) + (var56)) + (var57)) + (var58)) + (var59)) + (var60)) + (var61)) + (var62)) + (var63)) + (var64)) + (var65)) + (var66)) + (var67)) + (var68)) + (var69)) + (var70)) + (var71)) + (var72)) + (var73)) + (var74)) + (var75)) + (var76)) + (var77)) + (var78)) + (var79)) + (var80)) + (var81)) + (var82)) + (var83)) + (var84)) + (var85)) + (var86)) + (var87)) + (var88)) + (var89)) + (var90)) + (var91)) + (var92)) + (var93)) + (var94)) + (var95)) + (var96)) + (var97)) + (var98)) + (var99)))));
	*output = var100 < 0.5 ? 0 : 1;
}

void CUclf2(double * input, double * output) {
	double var0;
	if ((input[61]) >= (2071.5)) {
		if ((input[13]) >= (1.5)) {
			if ((input[38]) >= (1.41665)) {
				var0 = 0.16799074;
			}
			else {
				var0 = 0.08461054;
			}
		}
		else {
			if ((input[54]) >= (40.5)) {
				var0 = -0.05969757;
			}
			else {
				var0 = 0.1155762;
			}
		}
	}
	else {
		if ((input[37]) >= (1.1470499)) {
			if ((input[3]) >= (0.1)) {
				var0 = -0.004290311;
			}
			else {
				var0 = 0.18283139;
			}
		}
		else {
			if ((input[60]) >= (3.5)) {
				var0 = -0.06994801;
			}
			else {
				var0 = -0.17608754;
			}
		}
	}
	double var1;
	if ((input[38]) >= (1.41665)) {
		if ((input[50]) >= (2.1831)) {
			if ((input[38]) >= (1.875)) {
				var1 = 0.1552246;
			}
			else {
				var1 = 0.09255738;
			}
		}
		else {
			if ((input[14]) >= (0.225)) {
				var1 = -0.016776515;
			}
			else {
				var1 = 0.15743053;
			}
		}
	}
	else {
		if ((input[61]) >= (1198.5)) {
			if ((input[54]) >= (40.5)) {
				var1 = -0.040374797;
			}
			else {
				var1 = 0.09536489;
			}
		}
		else {
			if ((input[60]) >= (22.5)) {
				var1 = 0.020348035;
			}
			else {
				var1 = -0.15195803;
			}
		}
	}
	double var2;
	if ((input[61]) >= (2255.5)) {
		if ((input[2]) >= (1.5)) {
			if ((input[13]) >= (1.5)) {
				var2 = 0.12863429;
			}
			else {
				var2 = 0.007063667;
			}
		}
		else {
			if ((input[54]) >= (41.5)) {
				var2 = -0.064543806;
			}
			else {
				var2 = 0.092766225;
			}
		}
	}
	else {
		if ((input[37]) >= (1.1470499)) {
			if ((input[3]) >= (0.225)) {
				var2 = -0.0053140996;
			}
			else {
				var2 = 0.14691257;
			}
		}
		else {
			if ((input[60]) >= (22.5)) {
				var2 = 0.040356692;
			}
			else {
				var2 = -0.13486175;
			}
		}
	}
	double var3;
	if ((input[38]) >= (1.41665)) {
		if ((input[50]) >= (2.1831)) {
			if ((input[13]) >= (2.5)) {
				var3 = 0.14176835;
			}
			else {
				var3 = 0.088350385;
			}
		}
		else {
			if ((input[38]) >= (1.875)) {
				var3 = 0.08041965;
			}
			else {
				var3 = -0.04407309;
			}
		}
	}
	else {
		if ((input[61]) >= (787.5)) {
			if ((input[54]) >= (40.5)) {
				var3 = -0.0467388;
			}
			else {
				var3 = 0.067000836;
			}
		}
		else {
			if ((input[54]) >= (35.5)) {
				var3 = -0.14367668;
			}
			else {
				var3 = -0.05305326;
			}
		}
	}
	double var4;
	if ((input[61]) >= (1684.5)) {
		if ((input[60]) >= (1.5)) {
			if ((input[3]) >= (1.63335)) {
				var4 = 0.12422947;
			}
			else {
				var4 = 0.05305147;
			}
		}
		else {
			if ((input[2]) >= (1.5)) {
				var4 = 0.04184759;
			}
			else {
				var4 = -0.08740469;
			}
		}
	}
	else {
		if ((input[37]) >= (1.7353001)) {
			if ((input[41]) >= (3.5)) {
				var4 = 0.14172755;
			}
			else {
				var4 = 0.007568185;
			}
		}
		else {
			if ((input[60]) >= (3.5)) {
				var4 = -0.034142386;
			}
			else {
				var4 = -0.12151293;
			}
		}
	}
	double var5;
	if ((input[38]) >= (1.125)) {
		if ((input[38]) >= (1.875)) {
			if ((input[56]) >= (45976.0)) {
				var5 = 0.13844542;
			}
			else {
				var5 = 0.092457995;
			}
		}
		else {
			if ((input[61]) >= (696.0)) {
				var5 = 0.06869965;
			}
			else {
				var5 = -0.053067412;
			}
		}
	}
	else {
		if ((input[56]) >= (978.5)) {
			if ((input[54]) >= (40.5)) {
				var5 = -0.05456992;
			}
			else {
				var5 = 0.051803954;
			}
		}
		else {
			if ((input[54]) >= (35.5)) {
				var5 = -0.134654;
			}
			else {
				var5 = -0.044140335;
			}
		}
	}
	double var6;
	if ((input[61]) >= (3341.5)) {
		if ((input[54]) >= (42.5)) {
			if ((input[56]) >= (52071.5)) {
				var6 = 0.083890356;
			}
			else {
				var6 = -0.06495776;
			}
		}
		else {
			if ((input[56]) >= (8672.5)) {
				var6 = 0.13074513;
			}
			else {
				var6 = 0.069035165;
			}
		}
	}
	else {
		if ((input[37]) >= (1.1470499)) {
			if ((input[14]) >= (0.1)) {
				var6 = 0.006468394;
			}
			else {
				var6 = 0.13532308;
			}
		}
		else {
			if ((input[60]) >= (2.5)) {
				var6 = -0.032532256;
			}
			else {
				var6 = -0.11902028;
			}
		}
	}
	double var7;
	if ((input[38]) >= (1.125)) {
		if ((input[50]) >= (2.271)) {
			if ((input[60]) >= (7.5)) {
				var7 = 0.11428882;
			}
			else {
				var7 = 0.057071127;
			}
		}
		else {
			if ((input[14]) >= (0.1)) {
				var7 = -0.031525336;
			}
			else {
				var7 = 0.12517223;
			}
		}
	}
	else {
		if ((input[61]) >= (612.5)) {
			if ((input[54]) >= (37.5)) {
				var7 = -0.033915278;
			}
			else {
				var7 = 0.08677535;
			}
		}
		else {
			if ((input[60]) >= (22.5)) {
				var7 = 0.0050112046;
			}
			else {
				var7 = -0.12146427;
			}
		}
	}
	double var8;
	if ((input[61]) >= (2551.5)) {
		if ((input[54]) >= (40.5)) {
			if ((input[56]) >= (51435.5)) {
				var8 = 0.08653461;
			}
			else {
				var8 = -0.031282436;
			}
		}
		else {
			if ((input[56]) >= (8672.5)) {
				var8 = 0.1276896;
			}
			else {
				var8 = 0.075767614;
			}
		}
	}
	else {
		if ((input[37]) >= (1.1470499)) {
			if ((input[3]) >= (0.1)) {
				var8 = -0.0022228265;
			}
			else {
				var8 = 0.1250227;
			}
		}
		else {
			if ((input[54]) >= (37.5)) {
				var8 = -0.11581061;
			}
			else {
				var8 = -0.042835396;
			}
		}
	}
	double var9;
	if ((input[61]) >= (1461.5)) {
		if ((input[60]) >= (1.5)) {
			if ((input[54]) >= (37.5)) {
				var9 = 0.05934449;
			}
			else {
				var9 = 0.118148044;
			}
		}
		else {
			if ((input[2]) >= (1.5)) {
				var9 = 0.025322536;
			}
			else {
				var9 = -0.075533524;
			}
		}
	}
	else {
		if ((input[37]) >= (1.7353001)) {
			if ((input[41]) >= (3.5)) {
				var9 = 0.11794647;
			}
			else {
				var9 = -0.00596435;
			}
		}
		else {
			if ((input[54]) >= (35.5)) {
				var9 = -0.09262187;
			}
			else {
				var9 = 0.009818987;
			}
		}
	}
	double var10;
	if ((input[13]) >= (1.5)) {
		if ((input[2]) >= (1.5)) {
			if ((input[61]) >= (344.0)) {
				var10 = 0.07544811;
			}
			else {
				var10 = -0.052052405;
			}
		}
		else {
			if ((input[60]) >= (5.5)) {
				var10 = 0.025722027;
			}
			else {
				var10 = -0.06862326;
			}
		}
	}
	else {
		if ((input[38]) >= (1.875)) {
			if ((input[41]) >= (3.5)) {
				var10 = 0.12113569;
			}
			else {
				var10 = -0.01185779;
			}
		}
		else {
			if ((input[60]) >= (28.5)) {
				var10 = 0.06923418;
			}
			else {
				var10 = -0.08265653;
			}
		}
	}
	double var11;
	if ((input[38]) >= (1.41665)) {
		if ((input[38]) >= (1.875)) {
			if ((input[56]) >= (47167.5)) {
				var11 = 0.1131285;
			}
			else {
				var11 = 0.06558356;
			}
		}
		else {
			if ((input[61]) >= (562.5)) {
				var11 = 0.050030358;
			}
			else {
				var11 = -0.0345444;
			}
		}
	}
	else {
		if ((input[61]) >= (434.5)) {
			if ((input[54]) >= (37.5)) {
				var11 = -0.027210105;
			}
			else {
				var11 = 0.07617789;
			}
		}
		else {
			if ((input[60]) >= (22.5)) {
				var11 = -0.0068753245;
			}
			else {
				var11 = -0.10518382;
			}
		}
	}
	double var12;
	if ((input[61]) >= (4918.5)) {
		if ((input[54]) >= (43.5)) {
			if ((input[56]) >= (92981.0)) {
				var12 = 0.0563129;
			}
			else {
				var12 = -0.07711723;
			}
		}
		else {
			if ((input[56]) >= (20310.0)) {
				var12 = 0.10906406;
			}
			else {
				var12 = 0.058884915;
			}
		}
	}
	else {
		if ((input[37]) >= (1.1470499)) {
			if ((input[3]) >= (0.1)) {
				var12 = 0.00705546;
			}
			else {
				var12 = 0.11359664;
			}
		}
		else {
			if ((input[60]) >= (2.5)) {
				var12 = -0.01478934;
			}
			else {
				var12 = -0.09343983;
			}
		}
	}
	double var13;
	if ((input[13]) >= (1.5)) {
		if ((input[2]) >= (1.5)) {
			if ((input[61]) >= (315.5)) {
				var13 = 0.06390198;
			}
			else {
				var13 = -0.046562355;
			}
		}
		else {
			if ((input[60]) >= (15.5)) {
				var13 = 0.07784312;
			}
			else {
				var13 = -0.04905551;
			}
		}
	}
	else {
		if ((input[37]) >= (1.9118)) {
			if ((input[41]) >= (3.5)) {
				var13 = 0.11304116;
			}
			else {
				var13 = 0.00012629376;
			}
		}
		else {
			if ((input[60]) >= (22.5)) {
				var13 = 0.05212704;
			}
			else {
				var13 = -0.07188248;
			}
		}
	}
	double var14;
	if ((input[61]) >= (5240.0)) {
		if ((input[54]) >= (42.5)) {
			if ((input[56]) >= (88295.5)) {
				var14 = 0.06973923;
			}
			else {
				var14 = -0.036061876;
			}
		}
		else {
			if ((input[56]) >= (20310.0)) {
				var14 = 0.11645434;
			}
			else {
				var14 = 0.07055778;
			}
		}
	}
	else {
		if ((input[37]) >= (1.0294)) {
			if ((input[14]) >= (0.1)) {
				var14 = 0.0030309313;
			}
			else {
				var14 = 0.105439775;
			}
		}
		else {
			if ((input[60]) >= (2.5)) {
				var14 = -0.016466295;
			}
			else {
				var14 = -0.0877254;
			}
		}
	}
	double var15;
	if ((input[22]) >= (1.5)) {
		if ((input[61]) >= (301.5)) {
			if ((input[54]) >= (37.5)) {
				var15 = 0.03047618;
			}
			else {
				var15 = 0.09176539;
			}
		}
		else {
			if ((input[54]) >= (35.5)) {
				var15 = -0.10255362;
			}
			else {
				var15 = -0.024411475;
			}
		}
	}
	else {
		if ((input[38]) >= (1.875)) {
			if ((input[3]) >= (0.9)) {
				var15 = 0.0246003;
			}
			else {
				var15 = 0.11759241;
			}
		}
		else {
			if ((input[2]) >= (1.5)) {
				var15 = -0.015388521;
			}
			else {
				var15 = -0.07782511;
			}
		}
	}
	double var16;
	if ((input[61]) >= (5453.5)) {
		if ((input[54]) >= (42.5)) {
			if ((input[56]) >= (77235.5)) {
				var16 = 0.0604704;
			}
			else {
				var16 = -0.034797266;
			}
		}
		else {
			if ((input[56]) >= (20310.0)) {
				var16 = 0.11220204;
			}
			else {
				var16 = 0.06484485;
			}
		}
	}
	else {
		if ((input[37]) >= (1.0882)) {
			if ((input[3]) >= (0.1)) {
				var16 = 0.0046325177;
			}
			else {
				var16 = 0.102524124;
			}
		}
		else {
			if ((input[61]) >= (344.5)) {
				var16 = -0.023550434;
			}
			else {
				var16 = -0.09302377;
			}
		}
	}
	double var17;
	if ((input[60]) >= (5.5)) {
		if ((input[61]) >= (138.5)) {
			if ((input[60]) >= (24.5)) {
				var17 = 0.08820825;
			}
			else {
				var17 = 0.03439417;
			}
		}
		else {
			if ((input[50]) >= (1.88815)) {
				var17 = -0.025076857;
			}
			else {
				var17 = -0.121862784;
			}
		}
	}
	else {
		if ((input[38]) >= (1.875)) {
			if ((input[3]) >= (0.9)) {
				var17 = 0.01988348;
			}
			else {
				var17 = 0.11287924;
			}
		}
		else {
			if ((input[13]) >= (1.5)) {
				var17 = -0.007270382;
			}
			else {
				var17 = -0.07108962;
			}
		}
	}
	double var18;
	if ((input[22]) >= (1.5)) {
		if ((input[61]) >= (8496.5)) {
			if ((input[54]) >= (44.5)) {
				var18 = 0.019119091;
			}
			else {
				var18 = 0.09337639;
			}
		}
		else {
			if ((input[54]) >= (37.5)) {
				var18 = -0.03106524;
			}
			else {
				var18 = 0.04883593;
			}
		}
	}
	else {
		if ((input[38]) >= (1.125)) {
			if ((input[3]) >= (0.225)) {
				var18 = -0.010154485;
			}
			else {
				var18 = 0.11041607;
			}
		}
		else {
			if ((input[61]) >= (348.5)) {
				var18 = -0.03630062;
			}
			else {
				var18 = -0.09726635;
			}
		}
	}
	double var19;
	if ((input[61]) >= (726.5)) {
		if ((input[54]) >= (37.5)) {
			if ((input[61]) >= (9410.0)) {
				var19 = 0.042937037;
			}
			else {
				var19 = -0.038931683;
			}
		}
		else {
			if ((input[1]) >= (0.01025)) {
				var19 = 0.08890485;
			}
			else {
				var19 = -0.02384845;
			}
		}
	}
	else {
		if ((input[37]) >= (1.9118)) {
			if ((input[60]) >= (0.5)) {
				var19 = -0.023172734;
			}
			else {
				var19 = 0.10455262;
			}
		}
		else {
			if ((input[54]) >= (35.5)) {
				var19 = -0.07673208;
			}
			else {
				var19 = 0.0066961213;
			}
		}
	}
	double var20;
	if ((input[60]) >= (5.5)) {
		if ((input[61]) >= (137.5)) {
			if ((input[54]) >= (35.5)) {
				var20 = 0.037213713;
			}
			else {
				var20 = 0.104343295;
			}
		}
		else {
			if ((input[50]) >= (1.88815)) {
				var20 = -0.0197139;
			}
			else {
				var20 = -0.113930084;
			}
		}
	}
	else {
		if ((input[37]) >= (1.8529501)) {
			if ((input[14]) >= (1.1)) {
				var20 = 0.01966208;
			}
			else {
				var20 = 0.1030967;
			}
		}
		else {
			if ((input[13]) >= (1.5)) {
				var20 = -0.0057746153;
			}
			else {
				var20 = -0.06276844;
			}
		}
	}
	double var21;
	if ((input[61]) >= (1071.5)) {
		if ((input[54]) >= (40.5)) {
			if ((input[56]) >= (15194.5)) {
				var21 = 0.028159741;
			}
			else {
				var21 = -0.06220494;
			}
		}
		else {
			if ((input[56]) >= (8672.5)) {
				var21 = 0.096623234;
			}
			else {
				var21 = 0.039828483;
			}
		}
	}
	else {
		if ((input[37]) >= (1.8529501)) {
			if ((input[60]) >= (0.5)) {
				var21 = -0.016947486;
			}
			else {
				var21 = 0.096465155;
			}
		}
		else {
			if ((input[54]) >= (35.5)) {
				var21 = -0.06500055;
			}
			else {
				var21 = 0.01916818;
			}
		}
	}
	double var22;
	if ((input[57]) >= (0.5)) {
		if ((input[37]) >= (1.1470499)) {
			if ((input[14]) >= (0.225)) {
				var22 = 0.0032505703;
			}
			else {
				var22 = 0.09489176;
			}
		}
		else {
			if ((input[61]) >= (396.5)) {
				var22 = -0.017783483;
			}
			else {
				var22 = -0.08548885;
			}
		}
	}
	else {
		if ((input[52]) >= (2.80425)) {
			if ((input[3]) >= (1.45)) {
				var22 = 0.076781675;
			}
			else {
				var22 = 0.025784303;
			}
		}
		else {
			if ((input[61]) >= (123.5)) {
				var22 = -0.029552845;
			}
			else {
				var22 = -0.1243095;
			}
		}
	}
	double var23;
	if ((input[22]) >= (1.5)) {
		if ((input[61]) >= (268.5)) {
			if ((input[54]) >= (35.5)) {
				var23 = 0.029167688;
			}
			else {
				var23 = 0.10557046;
			}
		}
		else {
			if ((input[61]) >= (60.5)) {
				var23 = -0.03448201;
			}
			else {
				var23 = -0.11146297;
			}
		}
	}
	else {
		if ((input[38]) >= (1.875)) {
			if ((input[14]) >= (0.9)) {
				var23 = 0.008910305;
			}
			else {
				var23 = 0.107640766;
			}
		}
		else {
			if ((input[2]) >= (1.5)) {
				var23 = -0.011397367;
			}
			else {
				var23 = -0.05824506;
			}
		}
	}
	double var24;
	if ((input[61]) >= (4487.5)) {
		if ((input[54]) >= (44.5)) {
			if ((input[56]) >= (126076.0)) {
				var24 = 0.030635238;
			}
			else {
				var24 = -0.06893799;
			}
		}
		else {
			if ((input[61]) >= (56724.0)) {
				var24 = 0.10199125;
			}
			else {
				var24 = 0.037661076;
			}
		}
	}
	else {
		if ((input[54]) >= (35.5)) {
			if ((input[37]) >= (1.0294)) {
				var24 = -0.0034455177;
			}
			else {
				var24 = -0.051677454;
			}
		}
		else {
			if ((input[61]) >= (132.5)) {
				var24 = 0.085928366;
			}
			else {
				var24 = -0.06929981;
			}
		}
	}
	double var25;
	if ((input[60]) >= (8.5)) {
		if ((input[52]) >= (2.67555)) {
			if ((input[3]) >= (1.63335)) {
				var25 = 0.07287714;
			}
			else {
				var25 = 0.027258152;
			}
		}
		else {
			if ((input[52]) >= (0.42430001)) {
				var25 = -0.030033996;
			}
			else {
				var25 = -0.12385853;
			}
		}
	}
	else {
		if ((input[38]) >= (1.125)) {
			if ((input[14]) >= (0.1)) {
				var25 = -0.0018817055;
			}
			else {
				var25 = 0.115904965;
			}
		}
		else {
			if ((input[61]) >= (396.5)) {
				var25 = -0.016158948;
			}
			else {
				var25 = -0.08556452;
			}
		}
	}
	double var26;
	if ((input[13]) >= (2.5)) {
		if ((input[2]) >= (2.5)) {
			if ((input[1]) >= (0.0054)) {
				var26 = 0.07096465;
			}
			else {
				var26 = -0.065002546;
			}
		}
		else {
			if ((input[56]) >= (1822.5)) {
				var26 = 0.01904731;
			}
			else {
				var26 = -0.033826075;
			}
		}
	}
	else {
		if ((input[60]) >= (20.5)) {
			if ((input[46]) >= (1.41795)) {
				var26 = 0.05051997;
			}
			else {
				var26 = -0.10425352;
			}
		}
		else {
			if ((input[37]) >= (1.0294)) {
				var26 = -0.0009567206;
			}
			else {
				var26 = -0.039297495;
			}
		}
	}
	double var27;
	if ((input[22]) >= (1.5)) {
		if ((input[61]) >= (12050.5)) {
			if ((input[54]) >= (43.5)) {
				var27 = 0.021089038;
			}
			else {
				var27 = 0.08628465;
			}
		}
		else {
			if ((input[54]) >= (40.5)) {
				var27 = -0.04327766;
			}
			else {
				var27 = 0.025286345;
			}
		}
	}
	else {
		if ((input[38]) >= (2.70835)) {
			if ((input[1]) >= (0.0112)) {
				var27 = 0.057670515;
			}
			else {
				var27 = 0.11060089;
			}
		}
		else {
			if ((input[37]) >= (1.8529501)) {
				var27 = 0.020455536;
			}
			else {
				var27 = -0.034153093;
			}
		}
	}
	double var28;
	if ((input[61]) >= (389.5)) {
		if ((input[54]) >= (37.5)) {
			if ((input[61]) >= (5631.5)) {
				var28 = 0.022670707;
			}
			else {
				var28 = -0.043025456;
			}
		}
		else {
			if ((input[1]) >= (0.02)) {
				var28 = 0.06850648;
			}
			else {
				var28 = -0.01562512;
			}
		}
	}
	else {
		if ((input[38]) >= (1.875)) {
			if ((input[41]) >= (3.5)) {
				var28 = 0.093202285;
			}
			else {
				var28 = -0.06967081;
			}
		}
		else {
			if ((input[37]) >= (1.9118)) {
				var28 = 0.034205373;
			}
			else {
				var28 = -0.06081843;
			}
		}
	}
	double var29;
	if ((input[61]) >= (582.5)) {
		if ((input[54]) >= (37.5)) {
			if ((input[61]) >= (7420.5)) {
				var29 = 0.02397207;
			}
			else {
				var29 = -0.03313583;
			}
		}
		else {
			if ((input[1]) >= (0.021000002)) {
				var29 = 0.06793168;
			}
			else {
				var29 = -0.015230327;
			}
		}
	}
	else {
		if ((input[38]) >= (1.125)) {
			if ((input[32]) >= (4.5)) {
				var29 = 0.09272095;
			}
			else {
				var29 = -0.040440816;
			}
		}
		else {
			if ((input[54]) >= (35.5)) {
				var29 = -0.07656763;
			}
			else {
				var29 = 0.011193185;
			}
		}
	}
	double var30;
	if ((input[60]) >= (7.5)) {
		if ((input[52]) >= (1.0506)) {
			if ((input[61]) >= (12283.5)) {
				var30 = 0.07499509;
			}
			else {
				var30 = 0.025754184;
			}
		}
		else {
			if ((input[12]) >= (0.5835)) {
				var30 = -0.107451454;
			}
			else {
				var30 = 0.011165468;
			}
		}
	}
	else {
		if ((input[2]) >= (2.5)) {
			if ((input[13]) >= (2.5)) {
				var30 = 0.04873749;
			}
			else {
				var30 = -0.009209843;
			}
		}
		else {
			if ((input[54]) >= (42.5)) {
				var30 = -0.051653653;
			}
			else {
				var30 = -0.012247236;
			}
		}
	}
	double var31;
	if ((input[51]) >= (0.00915)) {
		if ((input[57]) >= (0.5)) {
			if ((input[54]) >= (45.5)) {
				var31 = -0.06414739;
			}
			else {
				var31 = -0.00027080261;
			}
		}
		else {
			if ((input[51]) >= (0.06505)) {
				var31 = 0.055485915;
			}
			else {
				var31 = 0.008792248;
			}
		}
	}
	else {
		if ((input[40]) >= (3.5)) {
			if ((input[38]) >= (1.125)) {
				var31 = 0.03571431;
			}
			else {
				var31 = -0.07543139;
			}
		}
		else {
			if ((input[61]) >= (248.5)) {
				var31 = -0.04352641;
			}
			else {
				var31 = -0.10253408;
			}
		}
	}
	double var32;
	if ((input[22]) >= (1.5)) {
		if ((input[61]) >= (5451.5)) {
			if ((input[54]) >= (43.5)) {
				var32 = 0.006255414;
			}
			else {
				var32 = 0.062567286;
			}
		}
		else {
			if ((input[54]) >= (40.5)) {
				var32 = -0.054895904;
			}
			else {
				var32 = 0.015356712;
			}
		}
	}
	else {
		if ((input[38]) >= (2.875)) {
			if ((input[37]) >= (0.3125)) {
				var32 = 0.09593447;
			}
			else {
				var32 = 0.03630934;
			}
		}
		else {
			if ((input[60]) >= (4.5)) {
				var32 = 0.0035524168;
			}
			else {
				var32 = -0.03242232;
			}
		}
	}
	double var33;
	if ((input[61]) >= (78599.5)) {
		if ((input[54]) >= (45.5)) {
			if ((input[56]) >= (230760.5)) {
				var33 = 0.05132739;
			}
			else {
				var33 = -0.052384686;
			}
		}
		else {
			if ((input[56]) >= (98623.0)) {
				var33 = 0.102121465;
			}
			else {
				var33 = 0.0702837;
			}
		}
	}
	else {
		if ((input[54]) >= (42.5)) {
			if ((input[54]) >= (45.5)) {
				var33 = -0.087361865;
			}
			else {
				var33 = -0.03692706;
			}
		}
		else {
			if ((input[61]) >= (344.5)) {
				var33 = 0.020870967;
			}
			else {
				var33 = -0.034145433;
			}
		}
	}
	double var34;
	if ((input[51]) >= (0.00855)) {
		if ((input[60]) >= (19.5)) {
			if ((input[24]) >= (8.0)) {
				var34 = 0.018316528;
			}
			else {
				var34 = 0.06375447;
			}
		}
		else {
			if ((input[61]) >= (60381.5)) {
				var34 = 0.045941062;
			}
			else {
				var34 = -0.008147589;
			}
		}
	}
	else {
		if ((input[60]) >= (0.5)) {
			if ((input[61]) >= (344.5)) {
				var34 = -0.030601714;
			}
			else {
				var34 = -0.09962622;
			}
		}
		else {
			if ((input[38]) >= (1.125)) {
				var34 = 0.034778163;
			}
			else {
				var34 = -0.07422792;
			}
		}
	}
	double var35;
	if ((input[38]) >= (2.70835)) {
		if ((input[61]) >= (35803.5)) {
			if ((input[54]) >= (44.5)) {
				var35 = -0.0014248;
			}
			else {
				var35 = 0.09923847;
			}
		}
		else {
			if ((input[60]) >= (0.5)) {
				var35 = 0.016728636;
			}
			else {
				var35 = 0.10896961;
			}
		}
	}
	else {
		if ((input[61]) >= (254.5)) {
			if ((input[54]) >= (37.5)) {
				var35 = -0.0127284555;
			}
			else {
				var35 = 0.042271845;
			}
		}
		else {
			if ((input[54]) >= (41.5)) {
				var35 = 0.026416851;
			}
			else {
				var35 = -0.052730706;
			}
		}
	}
	double var36;
	if ((input[61]) >= (3460.5)) {
		if ((input[54]) >= (42.5)) {
			if ((input[61]) >= (90087.0)) {
				var36 = 0.038891457;
			}
			else {
				var36 = -0.034039013;
			}
		}
		else {
			if ((input[61]) >= (23549.5)) {
				var36 = 0.09614458;
			}
			else {
				var36 = 0.031427868;
			}
		}
	}
	else {
		if ((input[54]) >= (35.5)) {
			if ((input[37]) >= (1.9118)) {
				var36 = 0.026634628;
			}
			else {
				var36 = -0.032196578;
			}
		}
		else {
			if ((input[61]) >= (108.5)) {
				var36 = 0.0704004;
			}
			else {
				var36 = -0.056001037;
			}
		}
	}
	double var37;
	if ((input[51]) >= (0.00775)) {
		if ((input[54]) >= (35.5)) {
			if ((input[61]) >= (2152.5)) {
				var37 = 0.014984903;
			}
			else {
				var37 = -0.022080686;
			}
		}
		else {
			if ((input[56]) >= (183.5)) {
				var37 = 0.068823814;
			}
			else {
				var37 = -0.037319016;
			}
		}
	}
	else {
		if ((input[18]) >= (4.5)) {
			if ((input[38]) >= (1.125)) {
				var37 = 0.10687759;
			}
			else {
				var37 = -0.08240751;
			}
		}
		else {
			if ((input[47]) >= (0.17005)) {
				var37 = -0.0457718;
			}
			else {
				var37 = -0.09219591;
			}
		}
	}
	double var38;
	if ((input[61]) >= (23684.5)) {
		if ((input[54]) >= (45.5)) {
			if ((input[0]) >= (6.0)) {
				var38 = 0.06362978;
			}
			else {
				var38 = -0.056525458;
			}
		}
		else {
			if ((input[54]) >= (42.5)) {
				var38 = 0.037562203;
			}
			else {
				var38 = 0.093679264;
			}
		}
	}
	else {
		if ((input[54]) >= (42.5)) {
			if ((input[2]) >= (0.5)) {
				var38 = -0.06859655;
			}
			else {
				var38 = -0.01418605;
			}
		}
		else {
			if ((input[61]) >= (498.5)) {
				var38 = 0.014820859;
			}
			else {
				var38 = -0.021942267;
			}
		}
	}
	double var39;
	if ((input[60]) >= (25.5)) {
		if ((input[48]) >= (3.7364001)) {
			if ((input[60]) >= (146.5)) {
				var39 = 0.090259984;
			}
			else {
				var39 = 0.037610024;
			}
		}
		else {
			if ((input[36]) >= (5.6876)) {
				var39 = -0.022190917;
			}
			else {
				var39 = -0.115907885;
			}
		}
	}
	else {
		if ((input[37]) >= (2.9118)) {
			if ((input[38]) >= (2.125)) {
				var39 = 0.09175042;
			}
			else {
				var39 = -0.059091564;
			}
		}
		else {
			if ((input[61]) >= (78740.0)) {
				var39 = 0.04044189;
			}
			else {
				var39 = -0.011474821;
			}
		}
	}
	double var40;
	if ((input[22]) >= (1.5)) {
		if ((input[51]) >= (0.043250002)) {
			if ((input[61]) >= (9754.0)) {
				var40 = 0.050701387;
			}
			else {
				var40 = 0.013757259;
			}
		}
		else {
			if ((input[54]) >= (35.5)) {
				var40 = -0.03845463;
			}
			else {
				var40 = 0.02747773;
			}
		}
	}
	else {
		if ((input[37]) >= (1.1470499)) {
			if ((input[12]) >= (0.0005)) {
				var40 = -0.016314965;
			}
			else {
				var40 = 0.09198107;
			}
		}
		else {
			if ((input[56]) >= (336.5)) {
				var40 = -0.01578874;
			}
			else {
				var40 = -0.06438937;
			}
		}
	}
	double var41;
	if ((input[54]) >= (45.5)) {
		if ((input[61]) >= (219227.0)) {
			if ((input[54]) >= (48.5)) {
				var41 = -0.08958017;
			}
			else {
				var41 = 0.057248533;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var41 = -0.01999317;
			}
			else {
				var41 = -0.0978116;
			}
		}
	}
	else {
		if ((input[61]) >= (57873.5)) {
			if ((input[56]) >= (98623.0)) {
				var41 = 0.09399792;
			}
			else {
				var41 = 0.051992297;
			}
		}
		else {
			if ((input[54]) >= (35.5)) {
				var41 = -0.0070948377;
			}
			else {
				var41 = 0.034060493;
			}
		}
	}
	double var42;
	if ((input[51]) >= (0.00765)) {
		if ((input[54]) >= (42.5)) {
			if ((input[61]) >= (90002.0)) {
				var42 = 0.029490724;
			}
			else {
				var42 = -0.03506599;
			}
		}
		else {
			if ((input[61]) >= (18928.5)) {
				var42 = 0.084158845;
			}
			else {
				var42 = 0.006779805;
			}
		}
	}
	else {
		if ((input[40]) >= (3.5)) {
			if ((input[38]) >= (1.125)) {
				var42 = 0.045184504;
			}
			else {
				var42 = -0.06212918;
			}
		}
		else {
			if ((input[52]) >= (0.48325002)) {
				var42 = -0.037914034;
			}
			else {
				var42 = -0.09600524;
			}
		}
	}
	double var43;
	if ((input[38]) >= (2.875)) {
		if ((input[37]) >= (1.7353001)) {
			if ((input[62]) >= (2.5)) {
				var43 = 0.09191964;
			}
			else {
				var43 = -0.07815572;
			}
		}
		else {
			if ((input[61]) >= (12914.5)) {
				var43 = 0.07024812;
			}
			else {
				var43 = -0.045139246;
			}
		}
	}
	else {
		if ((input[61]) >= (1429.5)) {
			if ((input[54]) >= (40.5)) {
				var43 = -0.014226458;
			}
			else {
				var43 = 0.03690069;
			}
		}
		else {
			if ((input[54]) >= (35.5)) {
				var43 = -0.028822575;
			}
			else {
				var43 = 0.02413298;
			}
		}
	}
	double var44;
	if ((input[54]) >= (45.5)) {
		if ((input[61]) >= (262388.5)) {
			if ((input[54]) >= (48.5)) {
				var44 = -0.05974564;
			}
			else {
				var44 = 0.06370349;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var44 = -0.0122676585;
			}
			else {
				var44 = -0.08554583;
			}
		}
	}
	else {
		if ((input[61]) >= (57873.5)) {
			if ((input[56]) >= (98623.0)) {
				var44 = 0.090449475;
			}
			else {
				var44 = 0.048208024;
			}
		}
		else {
			if ((input[51]) >= (0.00575)) {
				var44 = 0.0021961532;
			}
			else {
				var44 = -0.050021585;
			}
		}
	}
	double var45;
	if ((input[60]) >= (19.5)) {
		if ((input[48]) >= (3.9164)) {
			if ((input[12]) >= (13.3403)) {
				var45 = 0.0066104084;
			}
			else {
				var45 = 0.05051602;
			}
		}
		else {
			if ((input[36]) >= (5.74115)) {
				var45 = -0.015855731;
			}
			else {
				var45 = -0.09459628;
			}
		}
	}
	else {
		if ((input[37]) >= (2.9118)) {
			if ((input[40]) >= (3.5)) {
				var45 = 0.10643176;
			}
			else {
				var45 = 0.054329384;
			}
		}
		else {
			if ((input[61]) >= (2147.5)) {
				var45 = 0.0053237197;
			}
			else {
				var45 = -0.017296081;
			}
		}
	}
	double var46;
	if ((input[54]) >= (45.5)) {
		if ((input[61]) >= (219227.0)) {
			if ((input[54]) >= (48.5)) {
				var46 = -0.07608989;
			}
			else {
				var46 = 0.048951842;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var46 = -0.013506441;
			}
			else {
				var46 = -0.08180889;
			}
		}
	}
	else {
		if ((input[61]) >= (57873.5)) {
			if ((input[54]) >= (42.5)) {
				var46 = 0.054696303;
			}
			else {
				var46 = 0.10141207;
			}
		}
		else {
			if ((input[54]) >= (36.5)) {
				var46 = -0.007909889;
			}
			else {
				var46 = 0.021088075;
			}
		}
	}
	double var47;
	if ((input[50]) >= (11.605)) {
		if ((input[23]) >= (1.225)) {
			if ((input[56]) >= (5397.0)) {
				var47 = 0.043952506;
			}
			else {
				var47 = 0.005877309;
			}
		}
		else {
			if ((input[37]) >= (1.0294)) {
				var47 = 0.018064326;
			}
			else {
				var47 = -0.018587356;
			}
		}
	}
	else {
		if ((input[54]) >= (42.5)) {
			if ((input[2]) >= (0.5)) {
				var47 = -0.057944287;
			}
			else {
				var47 = -0.0023135843;
			}
		}
		else {
			if ((input[61]) >= (240.5)) {
				var47 = 0.008832827;
			}
			else {
				var47 = -0.024663195;
			}
		}
	}
	double var48;
	if ((input[38]) >= (2.875)) {
		if ((input[37]) >= (1.7353001)) {
			if ((input[62]) >= (5.5)) {
				var48 = 0.08941009;
			}
			else {
				var48 = -0.026874507;
			}
		}
		else {
			if ((input[47]) >= (2.8031502)) {
				var48 = 0.06676293;
			}
			else {
				var48 = -0.035933666;
			}
		}
	}
	else {
		if ((input[0]) >= (6.0)) {
			if ((input[61]) >= (114.5)) {
				var48 = 0.031326566;
			}
			else {
				var48 = -0.060080744;
			}
		}
		else {
			if ((input[14]) >= (0.1)) {
				var48 = -0.011873926;
			}
			else {
				var48 = 0.032852136;
			}
		}
	}
	double var49;
	if ((input[51]) >= (0.00915)) {
		if ((input[54]) >= (37.5)) {
			if ((input[1]) >= (0.0005)) {
				var49 = -0.01136306;
			}
			else {
				var49 = 0.042813595;
			}
		}
		else {
			if ((input[56]) >= (343.5)) {
				var49 = 0.037571996;
			}
			else {
				var49 = -0.035804924;
			}
		}
	}
	else {
		if ((input[60]) >= (0.5)) {
			if ((input[61]) >= (98.5)) {
				var49 = -0.028720027;
			}
			else {
				var49 = -0.09691173;
			}
		}
		else {
			if ((input[38]) >= (1.125)) {
				var49 = 0.047410123;
			}
			else {
				var49 = -0.055675566;
			}
		}
	}
	double var50;
	if ((input[61]) >= (7431.0)) {
		if ((input[54]) >= (41.5)) {
			if ((input[12]) >= (2.52635)) {
				var50 = 0.01905037;
			}
			else {
				var50 = -0.029980872;
			}
		}
		else {
			if ((input[61]) >= (23485.5)) {
				var50 = 0.09454083;
			}
			else {
				var50 = 0.040123813;
			}
		}
	}
	else {
		if ((input[54]) >= (35.5)) {
			if ((input[37]) >= (0.61765)) {
				var50 = -0.003290385;
			}
			else {
				var50 = -0.034251895;
			}
		}
		else {
			if ((input[61]) >= (101.5)) {
				var50 = 0.049040474;
			}
			else {
				var50 = -0.0453409;
			}
		}
	}
	double var51;
	if ((input[37]) >= (2.9118)) {
		if ((input[40]) >= (3.5)) {
			if ((input[48]) >= (534.66705)) {
				var51 = 0.030844277;
			}
			else {
				var51 = 0.109698884;
			}
		}
		else {
			if ((input[56]) >= (9982.0)) {
				var51 = 0.08214873;
			}
			else {
				var51 = -0.021073414;
			}
		}
	}
	else {
		if ((input[61]) >= (1269.5)) {
			if ((input[54]) >= (40.5)) {
				var51 = -0.012347886;
			}
			else {
				var51 = 0.030570174;
			}
		}
		else {
			if ((input[54]) >= (35.5)) {
				var51 = -0.024354348;
			}
			else {
				var51 = 0.018090008;
			}
		}
	}
	double var52;
	if ((input[61]) >= (9753.5)) {
		if ((input[54]) >= (44.5)) {
			if ((input[56]) >= (230716.0)) {
				var52 = 0.044097546;
			}
			else {
				var52 = -0.02683117;
			}
		}
		else {
			if ((input[56]) >= (88862.5)) {
				var52 = 0.08881448;
			}
			else {
				var52 = 0.026701296;
			}
		}
	}
	else {
		if ((input[54]) >= (42.5)) {
			if ((input[37]) >= (1.1470499)) {
				var52 = 0.021657512;
			}
			else {
				var52 = -0.068202846;
			}
		}
		else {
			if ((input[13]) >= (0.5)) {
				var52 = -0.00534588;
			}
			else {
				var52 = 0.03044307;
			}
		}
	}
	double var53;
	if ((input[60]) >= (5.5)) {
		if ((input[52]) >= (0.9936)) {
			if ((input[61]) >= (12246.5)) {
				var53 = 0.040643297;
			}
			else {
				var53 = 0.009629163;
			}
		}
		else {
			if ((input[12]) >= (1.0634999)) {
				var53 = -0.0942179;
			}
			else {
				var53 = 0.00073083374;
			}
		}
	}
	else {
		if ((input[13]) >= (0.5)) {
			if ((input[12]) >= (3.31495)) {
				var53 = 0.011965046;
			}
			else {
				var53 = -0.023684224;
			}
		}
		else {
			if ((input[38]) >= (1.125)) {
				var53 = 0.093267195;
			}
			else {
				var53 = -0.033250906;
			}
		}
	}
	double var54;
	if ((input[54]) >= (45.5)) {
		if ((input[61]) >= (175030.5)) {
			if ((input[54]) >= (48.5)) {
				var54 = -0.08914526;
			}
			else {
				var54 = 0.031379834;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var54 = -0.013136192;
			}
			else {
				var54 = -0.076556675;
			}
		}
	}
	else {
		if ((input[56]) >= (88862.5)) {
			if ((input[61]) >= (144480.5)) {
				var54 = 0.09405531;
			}
			else {
				var54 = 0.06208635;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var54 = 0.01790884;
			}
			else {
				var54 = -0.0059081237;
			}
		}
	}
	double var55;
	if ((input[47]) >= (0.2818)) {
		if ((input[54]) >= (36.5)) {
			if ((input[61]) >= (3934.5)) {
				var55 = 0.010350583;
			}
			else {
				var55 = -0.014539041;
			}
		}
		else {
			if ((input[1]) >= (0.0083)) {
				var55 = 0.035912037;
			}
			else {
				var55 = -0.051042475;
			}
		}
	}
	else {
		if ((input[45]) >= (66.03955)) {
			if ((input[47]) >= (0.097100005)) {
				var55 = -0.031998288;
			}
			else {
				var55 = -0.09650276;
			}
		}
		else {
			if ((input[36]) >= (3.25655)) {
				var55 = -0.034120113;
			}
			else {
				var55 = 0.11351051;
			}
		}
	}
	double var56;
	if ((input[37]) >= (2.9118)) {
		if ((input[40]) >= (3.5)) {
			if ((input[56]) >= (4766.0)) {
				var56 = 0.035402305;
			}
			else {
				var56 = 0.10991426;
			}
		}
		else {
			if ((input[56]) >= (9982.0)) {
				var56 = 0.077059664;
			}
			else {
				var56 = -0.019810073;
			}
		}
	}
	else {
		if ((input[61]) >= (1071.5)) {
			if ((input[54]) >= (40.5)) {
				var56 = -0.012929687;
			}
			else {
				var56 = 0.026348863;
			}
		}
		else {
			if ((input[18]) >= (4.5)) {
				var56 = 0.021775302;
			}
			else {
				var56 = -0.022378258;
			}
		}
	}
	double var57;
	if ((input[50]) >= (12.543449)) {
		if ((input[23]) >= (1.45)) {
			if ((input[38]) >= (2.125)) {
				var57 = 0.06752014;
			}
			else {
				var57 = 0.025287146;
			}
		}
		else {
			if ((input[37]) >= (0.7353)) {
				var57 = 0.015879923;
			}
			else {
				var57 = -0.018491365;
			}
		}
	}
	else {
		if ((input[54]) >= (42.5)) {
			if ((input[2]) >= (0.5)) {
				var57 = -0.045811888;
			}
			else {
				var57 = 0.00012372817;
			}
		}
		else {
			if ((input[13]) >= (0.5)) {
				var57 = -0.0050058346;
			}
			else {
				var57 = 0.029206907;
			}
		}
	}
	double var58;
	if ((input[0]) >= (6.0)) {
		if ((input[1]) >= (0.0044)) {
			if ((input[61]) >= (58.5)) {
				var58 = 0.03229372;
			}
			else {
				var58 = -0.08153429;
			}
		}
		else {
			if ((input[37]) >= (2.82355)) {
				var58 = 0.09300311;
			}
			else {
				var58 = -0.06370149;
			}
		}
	}
	else {
		if ((input[2]) >= (0.5)) {
			if ((input[56]) >= (644.5)) {
				var58 = -0.0023760155;
			}
			else {
				var58 = -0.04791022;
			}
		}
		else {
			if ((input[38]) >= (1.125)) {
				var58 = 0.10484209;
			}
			else {
				var58 = -0.033714075;
			}
		}
	}
	double var59;
	if ((input[54]) >= (45.5)) {
		if ((input[61]) >= (8009.5)) {
			if ((input[0]) >= (6.0)) {
				var59 = 0.035240766;
			}
			else {
				var59 = -0.03762779;
			}
		}
		else {
			if ((input[56]) >= (8563.5)) {
				var59 = -0.06616665;
			}
			else {
				var59 = -0.10912686;
			}
		}
	}
	else {
		if ((input[61]) >= (60220.5)) {
			if ((input[54]) >= (42.5)) {
				var59 = 0.047436915;
			}
			else {
				var59 = 0.10015207;
			}
		}
		else {
			if ((input[54]) >= (37.5)) {
				var59 = -0.008389775;
			}
			else {
				var59 = 0.011337602;
			}
		}
	}
	double var60;
	if ((input[61]) >= (4428.5)) {
		if ((input[54]) >= (40.5)) {
			if ((input[45]) >= (113.751495)) {
				var60 = -0.0343632;
			}
			else {
				var60 = 0.012471947;
			}
		}
		else {
			if ((input[1]) >= (0.0044)) {
				var60 = 0.046125617;
			}
			else {
				var60 = -0.079189174;
			}
		}
	}
	else {
		if ((input[54]) >= (36.5)) {
			if ((input[1]) >= (0.0005)) {
				var60 = -0.023830308;
			}
			else {
				var60 = 0.03258071;
			}
		}
		else {
			if ((input[56]) >= (183.5)) {
				var60 = 0.029379362;
			}
			else {
				var60 = -0.050921913;
			}
		}
	}
	double var61;
	if ((input[61]) >= (23684.5)) {
		if ((input[54]) >= (42.5)) {
			if ((input[47]) >= (2.43315)) {
				var61 = -0.0010840127;
			}
			else {
				var61 = 0.07979883;
			}
		}
		else {
			if ((input[54]) >= (40.5)) {
				var61 = 0.0575049;
			}
			else {
				var61 = 0.09484016;
			}
		}
	}
	else {
		if ((input[54]) >= (42.5)) {
			if ((input[14]) >= (1.55)) {
				var61 = -0.07707743;
			}
			else {
				var61 = -0.0185528;
			}
		}
		else {
			if ((input[61]) >= (504.5)) {
				var61 = 0.009472431;
			}
			else {
				var61 = -0.013704764;
			}
		}
	}
	double var62;
	if ((input[0]) >= (6.0)) {
		if ((input[1]) >= (0.0161)) {
			if ((input[25]) >= (7.1)) {
				var62 = -0.027493699;
			}
			else {
				var62 = 0.037356567;
			}
		}
		else {
			if ((input[62]) >= (36.0)) {
				var62 = -0.067993574;
			}
			else {
				var62 = -0.014761602;
			}
		}
	}
	else {
		if ((input[13]) >= (0.5)) {
			if ((input[56]) >= (655.5)) {
				var62 = -0.0035001584;
			}
			else {
				var62 = -0.037977993;
			}
		}
		else {
			if ((input[38]) >= (1.125)) {
				var62 = 0.09622918;
			}
			else {
				var62 = -0.029351879;
			}
		}
	}
	double var63;
	if ((input[56]) >= (112305.0)) {
		if ((input[54]) >= (45.5)) {
			if ((input[61]) >= (394609.5)) {
				var63 = 0.080142215;
			}
			else {
				var63 = -0.010950166;
			}
		}
		else {
			if ((input[54]) >= (44.5)) {
				var63 = 0.051088262;
			}
			else {
				var63 = 0.08951716;
			}
		}
	}
	else {
		if ((input[54]) >= (43.5)) {
			if ((input[49]) >= (1.05445)) {
				var63 = -0.046821274;
			}
			else {
				var63 = -0.0068605663;
			}
		}
		else {
			if ((input[0]) >= (1.5)) {
				var63 = 0.008442636;
			}
			else {
				var63 = -0.013062996;
			}
		}
	}
	double var64;
	if ((input[47]) >= (0.16874999)) {
		if ((input[54]) >= (37.5)) {
			if ((input[1]) >= (0.0005)) {
				var64 = -0.010187424;
			}
			else {
				var64 = 0.0426666;
			}
		}
		else {
			if ((input[56]) >= (360.5)) {
				var64 = 0.02946212;
			}
			else {
				var64 = -0.032146398;
			}
		}
	}
	else {
		if ((input[45]) >= (59.01465)) {
			if ((input[61]) >= (942.5)) {
				var64 = 0.0053247176;
			}
			else {
				var64 = -0.08518622;
			}
		}
		else {
			if ((input[36]) >= (3.4018)) {
				var64 = -0.035570625;
			}
			else {
				var64 = 0.11381997;
			}
		}
	}
	double var65;
	if ((input[61]) >= (7444.5)) {
		if ((input[51]) >= (0.04955)) {
			if ((input[32]) >= (3.5)) {
				var65 = -0.00394324;
			}
			else {
				var65 = 0.03408441;
			}
		}
		else {
			if ((input[47]) >= (0.88890004)) {
				var65 = -0.059108227;
			}
			else {
				var65 = 0.07376187;
			}
		}
	}
	else {
		if ((input[54]) >= (42.5)) {
			if ((input[37]) >= (1.0294)) {
				var65 = 0.01910529;
			}
			else {
				var65 = -0.065525286;
			}
		}
		else {
			if ((input[45]) >= (89.4282)) {
				var65 = 0.014578632;
			}
			else {
				var65 = -0.008377016;
			}
		}
	}
	double var66;
	if ((input[61]) >= (2152.5)) {
		if ((input[54]) >= (40.5)) {
			if ((input[51]) >= (0.04175)) {
				var66 = 0.0011524704;
			}
			else {
				var66 = -0.06664114;
			}
		}
		else {
			if ((input[1]) >= (0.021000002)) {
				var66 = 0.03344858;
			}
			else {
				var66 = -0.06573153;
			}
		}
	}
	else {
		if ((input[54]) >= (35.5)) {
			if ((input[23]) >= (0.225)) {
				var66 = -0.02589341;
			}
			else {
				var66 = 0.018863693;
			}
		}
		else {
			if ((input[61]) >= (138.5)) {
				var66 = 0.039420813;
			}
			else {
				var66 = -0.027681628;
			}
		}
	}
	double var67;
	if ((input[37]) >= (2.9118)) {
		if ((input[40]) >= (3.5)) {
			if ((input[56]) >= (4766.0)) {
				var67 = 0.027765049;
			}
			else {
				var67 = 0.10980411;
			}
		}
		else {
			if ((input[36]) >= (5.2953)) {
				var67 = 0.061686426;
			}
			else {
				var67 = -0.039928343;
			}
		}
	}
	else {
		if ((input[61]) >= (1429.5)) {
			if ((input[54]) >= (40.5)) {
				var67 = -0.007496898;
			}
			else {
				var67 = 0.022759726;
			}
		}
		else {
			if ((input[23]) >= (0.225)) {
				var67 = -0.016696446;
			}
			else {
				var67 = 0.0160514;
			}
		}
	}
	double var68;
	if ((input[0]) >= (1.5)) {
		if ((input[61]) >= (254.5)) {
			if ((input[54]) >= (40.5)) {
				var68 = -0.004620734;
			}
			else {
				var68 = 0.02446132;
			}
		}
		else {
			if ((input[54]) >= (41.5)) {
				var68 = 0.056817826;
			}
			else {
				var68 = -0.038957793;
			}
		}
	}
	else {
		if ((input[2]) >= (0.5)) {
			if ((input[56]) >= (1821.5)) {
				var68 = -0.007305884;
			}
			else {
				var68 = -0.059298407;
			}
		}
		else {
			if ((input[37]) >= (1.0294)) {
				var68 = 0.11645615;
			}
			else {
				var68 = -0.038255434;
			}
		}
	}
	double var69;
	if ((input[61]) >= (9759.5)) {
		if ((input[54]) >= (39.0)) {
			if ((input[38]) >= (1.41665)) {
				var69 = -0.0163359;
			}
			else {
				var69 = 0.0231277;
			}
		}
		else {
			if ((input[1]) >= (0.01025)) {
				var69 = 0.06542881;
			}
			else {
				var69 = -0.0501166;
			}
		}
	}
	else {
		if ((input[54]) >= (43.5)) {
			if ((input[38]) >= (1.125)) {
				var69 = 0.039759543;
			}
			else {
				var69 = -0.08352014;
			}
		}
		else {
			if ((input[54]) >= (35.5)) {
				var69 = -0.006205737;
			}
			else {
				var69 = 0.019738903;
			}
		}
	}
	double var70;
	if ((input[54]) >= (46.5)) {
		if ((input[56]) >= (405452.5)) {
			if ((input[46]) >= (1243.2639)) {
				var70 = 0.06540677;
			}
			else {
				var70 = -0.11453048;
			}
		}
		else {
			if ((input[51]) >= (0.29315)) {
				var70 = -0.023726065;
			}
			else {
				var70 = -0.08764184;
			}
		}
	}
	else {
		if ((input[56]) >= (100093.0)) {
			if ((input[54]) >= (45.5)) {
				var70 = 0.019819938;
			}
			else {
				var70 = 0.07402011;
			}
		}
		else {
			if ((input[0]) >= (1.5)) {
				var70 = 0.0044447524;
			}
			else {
				var70 = -0.014666994;
			}
		}
	}
	double var71;
	if ((input[61]) >= (23684.5)) {
		if ((input[54]) >= (42.5)) {
			if ((input[47]) >= (2.43315)) {
				var71 = -0.00064422074;
			}
			else {
				var71 = 0.074408434;
			}
		}
		else {
			if ((input[61]) >= (51264.5)) {
				var71 = 0.094915144;
			}
			else {
				var71 = 0.053372353;
			}
		}
	}
	else {
		if ((input[54]) >= (43.5)) {
			if ((input[62]) >= (77.5)) {
				var71 = 0.009503328;
			}
			else {
				var71 = -0.05940873;
			}
		}
		else {
			if ((input[14]) >= (0.55)) {
				var71 = -0.0038566564;
			}
			else {
				var71 = 0.023551745;
			}
		}
	}
	double var72;
	if ((input[0]) >= (6.0)) {
		if ((input[1]) >= (0.0044)) {
			if ((input[61]) >= (58.5)) {
				var72 = 0.02670562;
			}
			else {
				var72 = -0.07079091;
			}
		}
		else {
			if ((input[37]) >= (2.82355)) {
				var72 = 0.0896646;
			}
			else {
				var72 = -0.05483044;
			}
		}
	}
	else {
		if ((input[3]) >= (0.775)) {
			if ((input[56]) >= (644.5)) {
				var72 = -0.0030927577;
			}
			else {
				var72 = -0.038975608;
			}
		}
		else {
			if ((input[38]) >= (1.125)) {
				var72 = 0.098134026;
			}
			else {
				var72 = -0.024991013;
			}
		}
	}
	double var73;
	if ((input[61]) >= (163572.5)) {
		if ((input[54]) >= (47.5)) {
			if ((input[50]) >= (18.4004)) {
				var73 = -0.0012306907;
			}
			else {
				var73 = -0.110235825;
			}
		}
		else {
			if ((input[54]) >= (45.5)) {
				var73 = 0.038319506;
			}
			else {
				var73 = 0.09022003;
			}
		}
	}
	else {
		if ((input[54]) >= (45.5)) {
			if ((input[56]) >= (8563.5)) {
				var73 = -0.029918218;
			}
			else {
				var73 = -0.10519248;
			}
		}
		else {
			if ((input[56]) >= (15196.5)) {
				var73 = 0.022112546;
			}
			else {
				var73 = -0.0023374017;
			}
		}
	}
	double var74;
	if ((input[4]) >= (1.5)) {
		if ((input[50]) >= (4.6821003)) {
			if ((input[15]) >= (6.5)) {
				var74 = 0.025494635;
			}
			else {
				var74 = 0.0010790665;
			}
		}
		else {
			if ((input[1]) >= (2.4673)) {
				var74 = -0.039872404;
			}
			else {
				var74 = 0.011353302;
			}
		}
	}
	else {
		if ((input[2]) >= (0.5)) {
			if ((input[12]) >= (2.38475)) {
				var74 = 0.005023046;
			}
			else {
				var74 = -0.024680318;
			}
		}
		else {
			if ((input[38]) >= (1.125)) {
				var74 = 0.07514333;
			}
			else {
				var74 = -0.023891928;
			}
		}
	}
	double var75;
	if ((input[47]) >= (0.13645)) {
		if ((input[54]) >= (37.5)) {
			if ((input[1]) >= (0.0005)) {
				var75 = -0.008805412;
			}
			else {
				var75 = 0.036825318;
			}
		}
		else {
			if ((input[56]) >= (431.5)) {
				var75 = 0.02605235;
			}
			else {
				var75 = -0.02202713;
			}
		}
	}
	else {
		if ((input[45]) >= (59.01465)) {
			if ((input[38]) >= (2.41665)) {
				var75 = 0.051980425;
			}
			else {
				var75 = -0.086123616;
			}
		}
		else {
			if ((input[36]) >= (3.47455)) {
				var75 = -0.029979212;
			}
			else {
				var75 = 0.10428002;
			}
		}
	}
	double var76;
	if ((input[0]) >= (6.0)) {
		if ((input[1]) >= (0.0161)) {
			if ((input[25]) >= (6.9)) {
				var76 = -0.0259572;
			}
			else {
				var76 = 0.03267127;
			}
		}
		else {
			if ((input[62]) >= (36.0)) {
				var76 = -0.061766166;
			}
			else {
				var76 = -0.010745174;
			}
		}
	}
	else {
		if ((input[49]) >= (0.75135)) {
			if ((input[56]) >= (30473.5)) {
				var76 = 0.011920314;
			}
			else {
				var76 = -0.012228338;
			}
		}
		else {
			if ((input[37]) >= (1.0294)) {
				var76 = 0.04697881;
			}
			else {
				var76 = -0.0053632096;
			}
		}
	}
	double var77;
	if ((input[54]) >= (46.5)) {
		if ((input[56]) >= (405452.5)) {
			if ((input[46]) >= (1243.2639)) {
				var77 = 0.060639895;
			}
			else {
				var77 = -0.10652124;
			}
		}
		else {
			if ((input[1]) >= (21.402351)) {
				var77 = 0.01893175;
			}
			else {
				var77 = -0.06844899;
			}
		}
	}
	else {
		if ((input[56]) >= (100093.0)) {
			if ((input[54]) >= (44.5)) {
				var77 = 0.023483684;
			}
			else {
				var77 = 0.07817748;
			}
		}
		else {
			if ((input[53]) >= (0.66575)) {
				var77 = 0.001359016;
			}
			else {
				var77 = -0.02959126;
			}
		}
	}
	double var78;
	if ((input[54]) >= (35.5)) {
		if ((input[61]) >= (1550.5)) {
			if ((input[54]) >= (40.5)) {
				var78 = -0.0062358747;
			}
			else {
				var78 = 0.02023648;
			}
		}
		else {
			if ((input[1]) >= (0.0005)) {
				var78 = -0.023540715;
			}
			else {
				var78 = 0.021512108;
			}
		}
	}
	else {
		if ((input[1]) >= (0.0054)) {
			if ((input[61]) >= (58.5)) {
				var78 = 0.040550895;
			}
			else {
				var78 = -0.056468792;
			}
		}
		else {
			if ((input[51]) >= (0.024750002)) {
				var78 = -0.058330398;
			}
			else {
				var78 = 0.013647476;
			}
		}
	}
	double var79;
	if ((input[0]) >= (1.5)) {
		if ((input[61]) >= (254.5)) {
			if ((input[54]) >= (40.5)) {
				var79 = -0.003567053;
			}
			else {
				var79 = 0.020234797;
			}
		}
		else {
			if ((input[54]) >= (41.5)) {
				var79 = 0.050950773;
			}
			else {
				var79 = -0.03378002;
			}
		}
	}
	else {
		if ((input[3]) >= (0.775)) {
			if ((input[56]) >= (1821.5)) {
				var79 = -0.0065538245;
			}
			else {
				var79 = -0.051501065;
			}
		}
		else {
			if ((input[37]) >= (1.0294)) {
				var79 = 0.11080625;
			}
			else {
				var79 = -0.033120926;
			}
		}
	}
	double var80;
	if ((input[61]) >= (5542.5)) {
		if ((input[53]) >= (0.82375)) {
			if ((input[45]) >= (114.27295)) {
				var80 = -0.011745261;
			}
			else {
				var80 = 0.027576039;
			}
		}
		else {
			if ((input[56]) >= (46981.0)) {
				var80 = 0.005672766;
			}
			else {
				var80 = -0.053335097;
			}
		}
	}
	else {
		if ((input[45]) >= (89.4282)) {
			if ((input[47]) >= (0.0975)) {
				var80 = 0.015042632;
			}
			else {
				var80 = -0.08616696;
			}
		}
		else {
			if ((input[37]) >= (0.61765)) {
				var80 = -0.0025855883;
			}
			else {
				var80 = -0.032568276;
			}
		}
	}
	double var81;
	if ((input[54]) >= (43.5)) {
		if ((input[56]) >= (13379.0)) {
			if ((input[61]) >= (340289.0)) {
				var81 = 0.060630154;
			}
			else {
				var81 = -0.00952318;
			}
		}
		else {
			if ((input[38]) >= (1.125)) {
				var81 = 0.018320112;
			}
			else {
				var81 = -0.078260385;
			}
		}
	}
	else {
		if ((input[56]) >= (51879.0)) {
			if ((input[54]) >= (42.5)) {
				var81 = 0.036296766;
			}
			else {
				var81 = 0.089181624;
			}
		}
		else {
			if ((input[37]) >= (1.5)) {
				var81 = -0.012942043;
			}
			else {
				var81 = 0.005509333;
			}
		}
	}
	double var82;
	if ((input[54]) >= (35.5)) {
		if ((input[61]) >= (2855.5)) {
			if ((input[32]) >= (3.5)) {
				var82 = -0.012613288;
			}
			else {
				var82 = 0.017128428;
			}
		}
		else {
			if ((input[45]) >= (90.0127)) {
				var82 = 0.007701286;
			}
			else {
				var82 = -0.020131378;
			}
		}
	}
	else {
		if ((input[1]) >= (0.0054)) {
			if ((input[56]) >= (114.5)) {
				var82 = 0.037131764;
			}
			else {
				var82 = -0.058203813;
			}
		}
		else {
			if ((input[40]) >= (2.5)) {
				var82 = -0.016206024;
			}
			else {
				var82 = -0.12051213;
			}
		}
	}
	double var83;
	if ((input[37]) >= (2.9118)) {
		if ((input[60]) >= (0.5)) {
			if ((input[56]) >= (9982.0)) {
				var83 = 0.061604347;
			}
			else {
				var83 = -0.031552047;
			}
		}
		else {
			if ((input[40]) >= (2.5)) {
				var83 = 0.1105929;
			}
			else {
				var83 = -0.11727276;
			}
		}
	}
	else {
		if ((input[0]) >= (1.5)) {
			if ((input[61]) >= (344.5)) {
				var83 = 0.009010801;
			}
			else {
				var83 = -0.014626706;
			}
		}
		else {
			if ((input[3]) >= (0.775)) {
				var83 = -0.016126039;
			}
			else {
				var83 = 0.025906047;
			}
		}
	}
	double var84;
	if ((input[54]) >= (46.5)) {
		if ((input[51]) >= (0.30865002)) {
			if ((input[56]) >= (66250.0)) {
				var84 = 0.015446477;
			}
			else {
				var84 = -0.09854955;
			}
		}
		else {
			if ((input[61]) >= (394680.0)) {
				var84 = 0.020661743;
			}
			else {
				var84 = -0.074496865;
			}
		}
	}
	else {
		if ((input[56]) >= (172614.5)) {
			if ((input[54]) >= (45.5)) {
				var84 = 0.037225503;
			}
			else {
				var84 = 0.085620366;
			}
		}
		else {
			if ((input[53]) >= (0.81065)) {
				var84 = 0.00274495;
			}
			else {
				var84 = -0.015066342;
			}
		}
	}
	double var85;
	if ((input[54]) >= (43.5)) {
		if ((input[56]) >= (13379.0)) {
			if ((input[48]) >= (504.97095)) {
				var85 = -0.013904612;
			}
			else {
				var85 = 0.03171319;
			}
		}
		else {
			if ((input[38]) >= (1.125)) {
				var85 = 0.016492663;
			}
			else {
				var85 = -0.0753813;
			}
		}
	}
	else {
		if ((input[56]) >= (20310.0)) {
			if ((input[54]) >= (41.5)) {
				var85 = 0.018895568;
			}
			else {
				var85 = 0.07212661;
			}
		}
		else {
			if ((input[1]) >= (36.99805)) {
				var85 = -0.06388466;
			}
			else {
				var85 = 0.00054929074;
			}
		}
	}
	double var86;
	if ((input[13]) >= (0.5)) {
		if ((input[54]) >= (37.5)) {
			if ((input[56]) >= (2228.0)) {
				var86 = -0.0016867568;
			}
			else {
				var86 = -0.029211674;
			}
		}
		else {
			if ((input[61]) >= (344.5)) {
				var86 = 0.020579834;
			}
			else {
				var86 = -0.019569173;
			}
		}
	}
	else {
		if ((input[38]) >= (1.125)) {
			if ((input[32]) >= (3.5)) {
				var86 = 0.108859956;
			}
			else {
				var86 = -0.022765176;
			}
		}
		else {
			if ((input[61]) >= (6399.5)) {
				var86 = 0.066377126;
			}
			else {
				var86 = -0.020452257;
			}
		}
	}
	double var87;
	if ((input[0]) >= (6.0)) {
		if ((input[61]) >= (905.5)) {
			if ((input[1]) >= (0.0015)) {
				var87 = 0.033856332;
			}
			else {
				var87 = -0.060433507;
			}
		}
		else {
			if ((input[54]) >= (37.5)) {
				var87 = -0.051006455;
			}
			else {
				var87 = 0.00964711;
			}
		}
	}
	else {
		if ((input[14]) >= (0.45)) {
			if ((input[12]) >= (2.40185)) {
				var87 = 0.003682032;
			}
			else {
				var87 = -0.01429469;
			}
		}
		else {
			if ((input[38]) >= (1.125)) {
				var87 = 0.091258995;
			}
			else {
				var87 = -0.021988641;
			}
		}
	}
	double var88;
	if ((input[54]) >= (42.5)) {
		if ((input[51]) >= (0.07195)) {
			if ((input[0]) >= (6.0)) {
				var88 = 0.031211078;
			}
			else {
				var88 = -0.011628232;
			}
		}
		else {
			if ((input[3]) >= (0.45)) {
				var88 = -0.057284374;
			}
			else {
				var88 = -0.0056734793;
			}
		}
	}
	else {
		if ((input[61]) >= (28169.5)) {
			if ((input[54]) >= (39.0)) {
				var88 = 0.050579872;
			}
			else {
				var88 = 0.09378518;
			}
		}
		else {
			if ((input[62]) >= (46.5)) {
				var88 = -0.005732372;
			}
			else {
				var88 = 0.00872017;
			}
		}
	}
	double var89;
	if ((input[61]) >= (262369.0)) {
		if ((input[54]) >= (47.5)) {
			if ((input[51]) >= (0.13264999)) {
				var89 = 0.0104367575;
			}
			else {
				var89 = -0.14171724;
			}
		}
		else {
			if ((input[61]) >= (339737.5)) {
				var89 = 0.09228139;
			}
			else {
				var89 = 0.049767956;
			}
		}
	}
	else {
		if ((input[54]) >= (45.5)) {
			if ((input[56]) >= (8563.5)) {
				var89 = -0.02222517;
			}
			else {
				var89 = -0.102857016;
			}
		}
		else {
			if ((input[61]) >= (50697.0)) {
				var89 = 0.034865182;
			}
			else {
				var89 = -0.00040028893;
			}
		}
	}
	double var90;
	if ((input[37]) >= (1.5)) {
		if ((input[60]) >= (0.5)) {
			if ((input[61]) >= (78088.5)) {
				var90 = 0.04668219;
			}
			else {
				var90 = -0.027901579;
			}
		}
		else {
			if ((input[37]) >= (1.8529501)) {
				var90 = 0.06925695;
			}
			else {
				var90 = -0.022368452;
			}
		}
	}
	else {
		if ((input[37]) >= (1.1470499)) {
			if ((input[1]) >= (0.0005)) {
				var90 = 0.010639717;
			}
			else {
				var90 = 0.1130945;
			}
		}
		else {
			if ((input[1]) >= (0.0044)) {
				var90 = 0.0044261105;
			}
			else {
				var90 = -0.04536933;
			}
		}
	}
	double var91;
	if ((input[54]) >= (35.5)) {
		if ((input[61]) >= (1228.5)) {
			if ((input[54]) >= (40.5)) {
				var91 = -0.007925849;
			}
			else {
				var91 = 0.01619103;
			}
		}
		else {
			if ((input[18]) >= (4.5)) {
				var91 = 0.020661749;
			}
			else {
				var91 = -0.022599135;
			}
		}
	}
	else {
		if ((input[1]) >= (0.0054)) {
			if ((input[1]) >= (5.8711)) {
				var91 = -0.020558374;
			}
			else {
				var91 = 0.04009017;
			}
		}
		else {
			if ((input[51]) >= (0.019650001)) {
				var91 = -0.04535332;
			}
			else {
				var91 = 0.029295582;
			}
		}
	}
	double var92;
	if ((input[5]) >= (4.675)) {
		if ((input[12]) >= (36.941902)) {
			if ((input[48]) >= (181.39331)) {
				var92 = -0.010644296;
			}
			else {
				var92 = -0.09036389;
			}
		}
		else {
			if ((input[18]) >= (4.5)) {
				var92 = 0.036850203;
			}
			else {
				var92 = 0.004531769;
			}
		}
	}
	else {
		if ((input[60]) >= (18.5)) {
			if ((input[1]) >= (1.7646501)) {
				var92 = 0.01193093;
			}
			else {
				var92 = 0.068438746;
			}
		}
		else {
			if ((input[47]) >= (2.11115)) {
				var92 = -0.016262956;
			}
			else {
				var92 = 0.0027229704;
			}
		}
	}
	double var93;
	if ((input[25]) >= (7.1)) {
		if ((input[46]) >= (49.57745)) {
			if ((input[12]) >= (11.9995)) {
				var93 = -0.0089730015;
			}
			else {
				var93 = 0.046290435;
			}
		}
		else {
			if ((input[1]) >= (4.48485)) {
				var93 = -0.072779864;
			}
			else {
				var93 = -0.005914834;
			}
		}
	}
	else {
		if ((input[0]) >= (6.0)) {
			if ((input[1]) >= (0.017099999)) {
				var93 = 0.025766907;
			}
			else {
				var93 = -0.027368737;
			}
		}
		else {
			if ((input[3]) >= (0.775)) {
				var93 = -0.0064567523;
			}
			else {
				var93 = 0.018437847;
			}
		}
	}
	double var94;
	if ((input[61]) >= (262369.0)) {
		if ((input[54]) >= (47.5)) {
			if ((input[51]) >= (0.46495)) {
				var94 = 0.0653202;
			}
			else {
				var94 = -0.040993076;
			}
		}
		else {
			if ((input[61]) >= (339737.5)) {
				var94 = 0.08960608;
			}
			else {
				var94 = 0.04669611;
			}
		}
	}
	else {
		if ((input[54]) >= (45.5)) {
			if ((input[56]) >= (8563.5)) {
				var94 = -0.019866986;
			}
			else {
				var94 = -0.10135281;
			}
		}
		else {
			if ((input[56]) >= (15196.5)) {
				var94 = 0.019427367;
			}
			else {
				var94 = -0.0018365545;
			}
		}
	}
	double var95;
	if ((input[53]) >= (0.81065)) {
		if ((input[61]) >= (5631.5)) {
			if ((input[51]) >= (0.03555)) {
				var95 = 0.019448789;
			}
			else {
				var95 = -0.048447363;
			}
		}
		else {
			if ((input[54]) >= (37.5)) {
				var95 = -0.012445629;
			}
			else {
				var95 = 0.008343316;
			}
		}
	}
	else {
		if ((input[56]) >= (227265.5)) {
			if ((input[54]) >= (47.5)) {
				var95 = -0.021730622;
			}
			else {
				var95 = 0.06539711;
			}
		}
		else {
			if ((input[48]) >= (562.692)) {
				var95 = -0.042130705;
			}
			else {
				var95 = -0.006548901;
			}
		}
	}
	double var96;
	if ((input[54]) >= (42.5)) {
		if ((input[56]) >= (13183.0)) {
			if ((input[38]) >= (1.41665)) {
				var96 = -0.02263904;
			}
			else {
				var96 = 0.017823415;
			}
		}
		else {
			if ((input[3]) >= (1.63335)) {
				var96 = -0.0935519;
			}
			else {
				var96 = -0.021016093;
			}
		}
	}
	else {
		if ((input[56]) >= (36082.0)) {
			if ((input[54]) >= (40.5)) {
				var96 = 0.05612105;
			}
			else {
				var96 = 0.09526288;
			}
		}
		else {
			if ((input[45]) >= (77.954605)) {
				var96 = 0.011610831;
			}
			else {
				var96 = -0.0035541502;
			}
		}
	}
	double var97;
	if ((input[62]) >= (14.5)) {
		if ((input[62]) >= (46.5)) {
			if ((input[1]) >= (0.06885)) {
				var97 = 0.00025407944;
			}
			else {
				var97 = -0.03434055;
			}
		}
		else {
			if ((input[1]) >= (0.0005)) {
				var97 = 0.0070482525;
			}
			else {
				var97 = 0.051728874;
			}
		}
	}
	else {
		if ((input[61]) >= (1080.5)) {
			if ((input[54]) >= (41.5)) {
				var97 = -0.01921135;
			}
			else {
				var97 = 0.008046969;
			}
		}
		else {
			if ((input[54]) >= (39.0)) {
				var97 = -0.08251603;
			}
			else {
				var97 = -0.020811098;
			}
		}
	}
	double var98;
	if ((input[37]) >= (1.5)) {
		if ((input[60]) >= (0.5)) {
			if ((input[61]) >= (78088.5)) {
				var98 = 0.04400438;
			}
			else {
				var98 = -0.02539899;
			}
		}
		else {
			if ((input[37]) >= (1.8529501)) {
				var98 = 0.0647537;
			}
			else {
				var98 = -0.018749438;
			}
		}
	}
	else {
		if ((input[37]) >= (1.1470499)) {
			if ((input[1]) >= (0.0005)) {
				var98 = 0.009523178;
			}
			else {
				var98 = 0.10225487;
			}
		}
		else {
			if ((input[1]) >= (0.0044)) {
				var98 = 0.0039919293;
			}
			else {
				var98 = -0.04145648;
			}
		}
	}
	double var99;
	if ((input[0]) >= (6.0)) {
		if ((input[1]) >= (13.3374)) {
			if ((input[50]) >= (10.94285)) {
				var99 = 0.021226833;
			}
			else {
				var99 = -0.057833057;
			}
		}
		else {
			if ((input[1]) >= (0.017099999)) {
				var99 = 0.025623722;
			}
			else {
				var99 = -0.023602424;
			}
		}
	}
	else {
		if ((input[3]) >= (0.775)) {
			if ((input[56]) >= (644.5)) {
				var99 = -0.002613872;
			}
			else {
				var99 = -0.02799823;
			}
		}
		else {
			if ((input[38]) >= (1.125)) {
				var99 = 0.07729566;
			}
			else {
				var99 = -0.015872832;
			}
		}
	}
	double var100;
	var100 = (1.0) / ((1.0) + (exp((0.0) - ((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((var0)+(var1)) + (var2)) + (var3)) + (var4)) + (var5)) + (var6)) + (var7)) + (var8)) + (var9)) + (var10)) + (var11)) + (var12)) + (var13)) + (var14)) + (var15)) + (var16)) + (var17)) + (var18)) + (var19)) + (var20)) + (var21)) + (var22)) + (var23)) + (var24)) + (var25)) + (var26)) + (var27)) + (var28)) + (var29)) + (var30)) + (var31)) + (var32)) + (var33)) + (var34)) + (var35)) + (var36)) + (var37)) + (var38)) + (var39)) + (var40)) + (var41)) + (var42)) + (var43)) + (var44)) + (var45)) + (var46)) + (var47)) + (var48)) + (var49)) + (var50)) + (var51)) + (var52)) + (var53)) + (var54)) + (var55)) + (var56)) + (var57)) + (var58)) + (var59)) + (var60)) + (var61)) + (var62)) + (var63)) + (var64)) + (var65)) + (var66)) + (var67)) + (var68)) + (var69)) + (var70)) + (var71)) + (var72)) + (var73)) + (var74)) + (var75)) + (var76)) + (var77)) + (var78)) + (var79)) + (var80)) + (var81)) + (var82)) + (var83)) + (var84)) + (var85)) + (var86)) + (var87)) + (var88)) + (var89)) + (var90)) + (var91)) + (var92)) + (var93)) + (var94)) + (var95)) + (var96)) + (var97)) + (var98)) + (var99)))));
	*output = var100 < 0.5 ? 0 : 1;
}

void CUclf3(double * input, double * output) {
	double var0;
	if ((input[38]) >= (2.125)) {
		if ((input[37]) >= (2.6111498)) {
			if ((input[60]) >= (0.5)) {
				var0 = 0.15117484;
			}
			else {
				var0 = 0.19596775;
			}
		}
		else {
			if ((input[56]) >= (8080.5)) {
				var0 = 0.12735671;
			}
			else {
				var0 = 0.003713331;
			}
		}
	}
	else {
		if ((input[61]) >= (732.5)) {
			if ((input[57]) >= (0.5)) {
				var0 = -0.02888301;
			}
			else {
				var0 = 0.08081212;
			}
		}
		else {
			if ((input[54]) >= (35.5)) {
				var0 = -0.17471476;
			}
			else {
				var0 = -0.09379383;
			}
		}
	}
	double var1;
	if ((input[38]) >= (2.125)) {
		if ((input[37]) >= (2.6111498)) {
			if ((input[60]) >= (0.5)) {
				var1 = 0.13686775;
			}
			else {
				var1 = 0.17814085;
			}
		}
		else {
			if ((input[56]) >= (5176.5)) {
				var1 = 0.10868657;
			}
			else {
				var1 = -0.0043428987;
			}
		}
	}
	else {
		if ((input[61]) >= (542.5)) {
			if ((input[57]) >= (0.5)) {
				var1 = -0.03157784;
			}
			else {
				var1 = 0.06812977;
			}
		}
		else {
			if ((input[54]) >= (35.5)) {
				var1 = -0.16314772;
			}
			else {
				var1 = -0.094921164;
			}
		}
	}
	double var2;
	if ((input[38]) >= (2.125)) {
		if ((input[37]) >= (2.94445)) {
			if ((input[14]) >= (1.225)) {
				var2 = 0.12958929;
			}
			else {
				var2 = 0.16671644;
			}
		}
		else {
			if ((input[56]) >= (9269.5)) {
				var2 = 0.10988901;
			}
			else {
				var2 = 0.0071141045;
			}
		}
	}
	else {
		if ((input[61]) >= (732.5)) {
			if ((input[54]) >= (40.5)) {
				var2 = -0.04382298;
			}
			else {
				var2 = 0.052987684;
			}
		}
		else {
			if ((input[61]) >= (167.5)) {
				var2 = -0.08580276;
			}
			else {
				var2 = -0.15159075;
			}
		}
	}
	double var3;
	if ((input[38]) >= (2.125)) {
		if ((input[38]) >= (2.875)) {
			if ((input[14]) >= (1.225)) {
				var3 = 0.12594226;
			}
			else {
				var3 = 0.16747807;
			}
		}
		else {
			if ((input[37]) >= (2.6111498)) {
				var3 = 0.114189975;
			}
			else {
				var3 = 0.019457439;
			}
		}
	}
	else {
		if ((input[61]) >= (1095.5)) {
			if ((input[54]) >= (40.5)) {
				var3 = -0.033418488;
			}
			else {
				var3 = 0.06475264;
			}
		}
		else {
			if ((input[60]) >= (26.5)) {
				var3 = 0.03732913;
			}
			else {
				var3 = -0.12293643;
			}
		}
	}
	double var4;
	if ((input[38]) >= (2.125)) {
		if ((input[37]) >= (2.94445)) {
			if ((input[60]) >= (0.5)) {
				var4 = 0.11003365;
			}
			else {
				var4 = 0.14710093;
			}
		}
		else {
			if ((input[60]) >= (39.5)) {
				var4 = 0.14330478;
			}
			else {
				var4 = 0.019532982;
			}
		}
	}
	else {
		if ((input[61]) >= (382.5)) {
			if ((input[60]) >= (1.5)) {
				var4 = 0.028761098;
			}
			else {
				var4 = -0.05979372;
			}
		}
		else {
			if ((input[54]) >= (35.5)) {
				var4 = -0.13643484;
			}
			else {
				var4 = -0.077190265;
			}
		}
	}
	double var5;
	if ((input[38]) >= (2.125)) {
		if ((input[38]) >= (2.875)) {
			if ((input[41]) >= (3.5)) {
				var5 = 0.14999124;
			}
			else {
				var5 = 0.10897297;
			}
		}
		else {
			if ((input[60]) >= (37.5)) {
				var5 = 0.13097118;
			}
			else {
				var5 = 0.022269996;
			}
		}
	}
	else {
		if ((input[61]) >= (1502.5)) {
			if ((input[54]) >= (41.5)) {
				var5 = -0.028685918;
			}
			else {
				var5 = 0.06710171;
			}
		}
		else {
			if ((input[61]) >= (174.5)) {
				var5 = -0.057584215;
			}
			else {
				var5 = -0.12507932;
			}
		}
	}
	double var6;
	if ((input[38]) >= (2.125)) {
		if ((input[37]) >= (2.6111498)) {
			if ((input[60]) >= (0.5)) {
				var6 = 0.093376584;
			}
			else {
				var6 = 0.13532354;
			}
		}
		else {
			if ((input[56]) >= (2053.5)) {
				var6 = 0.06883232;
			}
			else {
				var6 = -0.031371463;
			}
		}
	}
	else {
		if ((input[61]) >= (383.5)) {
			if ((input[54]) >= (37.5)) {
				var6 = -0.0282732;
			}
			else {
				var6 = 0.059760045;
			}
		}
		else {
			if ((input[54]) >= (35.5)) {
				var6 = -0.122687854;
			}
			else {
				var6 = -0.06161639;
			}
		}
	}
	double var7;
	if ((input[61]) >= (3760.5)) {
		if ((input[54]) >= (42.5)) {
			if ((input[56]) >= (27300.5)) {
				var7 = 0.050583124;
			}
			else {
				var7 = -0.09592245;
			}
		}
		else {
			if ((input[56]) >= (12853.5)) {
				var7 = 0.12874302;
			}
			else {
				var7 = 0.06838234;
			}
		}
	}
	else {
		if ((input[37]) >= (2.6111498)) {
			if ((input[60]) >= (0.5)) {
				var7 = 0.029270744;
			}
			else {
				var7 = 0.13199669;
			}
		}
		else {
			if ((input[60]) >= (25.5)) {
				var7 = 0.053489458;
			}
			else {
				var7 = -0.075891174;
			}
		}
	}
	double var8;
	if ((input[38]) >= (2.125)) {
		if ((input[38]) >= (2.875)) {
			if ((input[14]) >= (1.225)) {
				var8 = 0.09206213;
			}
			else {
				var8 = 0.14254376;
			}
		}
		else {
			if ((input[60]) >= (39.5)) {
				var8 = 0.11396189;
			}
			else {
				var8 = 0.013497787;
			}
		}
	}
	else {
		if ((input[61]) >= (256.5)) {
			if ((input[13]) >= (2.5)) {
				var8 = 0.033884175;
			}
			else {
				var8 = -0.042304263;
			}
		}
		else {
			if ((input[54]) >= (35.5)) {
				var8 = -0.11581581;
			}
			else {
				var8 = -0.0691963;
			}
		}
	}
	double var9;
	if ((input[61]) >= (2379.5)) {
		if ((input[54]) >= (41.5)) {
			if ((input[56]) >= (19261.0)) {
				var9 = 0.044530205;
			}
			else {
				var9 = -0.08552035;
			}
		}
		else {
			if ((input[56]) >= (9616.5)) {
				var9 = 0.119606994;
			}
			else {
				var9 = 0.057177108;
			}
		}
	}
	else {
		if ((input[37]) >= (2.6111498)) {
			if ((input[41]) >= (2.5)) {
				var9 = 0.11594238;
			}
			else {
				var9 = -0.003273362;
			}
		}
		else {
			if ((input[38]) >= (2.875)) {
				var9 = 0.12372036;
			}
			else {
				var9 = -0.06958002;
			}
		}
	}
	double var10;
	if ((input[61]) >= (4131.5)) {
		if ((input[54]) >= (42.5)) {
			if ((input[56]) >= (34891.5)) {
				var10 = 0.049771406;
			}
			else {
				var10 = -0.06906552;
			}
		}
		else {
			if ((input[56]) >= (13505.5)) {
				var10 = 0.11375904;
			}
			else {
				var10 = 0.058789488;
			}
		}
	}
	else {
		if ((input[37]) >= (2.6111498)) {
			if ((input[60]) >= (0.5)) {
				var10 = 0.019939952;
			}
			else {
				var10 = 0.121803224;
			}
		}
		else {
			if ((input[60]) >= (5.5)) {
				var10 = -0.000044270648;
			}
			else {
				var10 = -0.07593973;
			}
		}
	}
	double var11;
	if ((input[2]) >= (2.5)) {
		if ((input[13]) >= (2.5)) {
			if ((input[46]) >= (260.19345)) {
				var11 = 0.0824822;
			}
			else {
				var11 = 0.019859279;
			}
		}
		else {
			if ((input[61]) >= (408.5)) {
				var11 = -0.017860336;
			}
			else {
				var11 = -0.10121497;
			}
		}
	}
	else {
		if ((input[37]) >= (2.6111498)) {
			if ((input[41]) >= (2.5)) {
				var11 = 0.11003947;
			}
			else {
				var11 = -0.006397178;
			}
		}
		else {
			if ((input[38]) >= (2.875)) {
				var11 = 0.11753972;
			}
			else {
				var11 = -0.063548006;
			}
		}
	}
	double var12;
	if ((input[60]) >= (34.5)) {
		if ((input[46]) >= (10.44605)) {
			if ((input[60]) >= (79.5)) {
				var12 = 0.11505715;
			}
			else {
				var12 = 0.070936345;
			}
		}
		else {
			if ((input[51]) >= (0.04925)) {
				var12 = 0.05308296;
			}
			else {
				var12 = -0.10203879;
			}
		}
	}
	else {
		if ((input[37]) >= (2.1666498)) {
			if ((input[3]) >= (0.3)) {
				var12 = 0.021414924;
			}
			else {
				var12 = 0.1148543;
			}
		}
		else {
			if ((input[38]) >= (2.58335)) {
				var12 = 0.07481589;
			}
			else {
				var12 = -0.050324827;
			}
		}
	}
	double var13;
	if ((input[61]) >= (1775.5)) {
		if ((input[54]) >= (40.5)) {
			if ((input[56]) >= (18522.0)) {
				var13 = 0.03925227;
			}
			else {
				var13 = -0.06898947;
			}
		}
		else {
			if ((input[56]) >= (8355.0)) {
				var13 = 0.10545566;
			}
			else {
				var13 = 0.05076387;
			}
		}
	}
	else {
		if ((input[38]) >= (2.875)) {
			if ((input[41]) >= (2.5)) {
				var13 = 0.12443864;
			}
			else {
				var13 = -0.037578207;
			}
		}
		else {
			if ((input[37]) >= (2.6111498)) {
				var13 = 0.058382194;
			}
			else {
				var13 = -0.058578353;
			}
		}
	}
	double var14;
	if ((input[2]) >= (2.5)) {
		if ((input[13]) >= (2.5)) {
			if ((input[61]) >= (145.5)) {
				var14 = 0.05944505;
			}
			else {
				var14 = -0.0811931;
			}
		}
		else {
			if ((input[61]) >= (408.5)) {
				var14 = -0.013934106;
			}
			else {
				var14 = -0.09180071;
			}
		}
	}
	else {
		if ((input[37]) >= (2.6111498)) {
			if ((input[60]) >= (0.5)) {
				var14 = 0.0037949127;
			}
			else {
				var14 = 0.11196271;
			}
		}
		else {
			if ((input[60]) >= (5.5)) {
				var14 = 0.0010898506;
			}
			else {
				var14 = -0.06893899;
			}
		}
	}
	double var15;
	if ((input[60]) >= (30.5)) {
		if ((input[52]) >= (6.8992)) {
			if ((input[60]) >= (79.5)) {
				var15 = 0.1057809;
			}
			else {
				var15 = 0.059270628;
			}
		}
		else {
			if ((input[36]) >= (6.23175)) {
				var15 = 0.06324444;
			}
			else {
				var15 = -0.10321676;
			}
		}
	}
	else {
		if ((input[37]) >= (2.1666498)) {
			if ((input[3]) >= (0.3)) {
				var15 = 0.01621507;
			}
			else {
				var15 = 0.108790755;
			}
		}
		else {
			if ((input[61]) >= (246.5)) {
				var15 = -0.010440324;
			}
			else {
				var15 = -0.07607153;
			}
		}
	}
	double var16;
	if ((input[50]) >= (11.11525)) {
		if ((input[54]) >= (44.5)) {
			if ((input[56]) >= (66561.5)) {
				var16 = 0.026055861;
			}
			else {
				var16 = -0.08424868;
			}
		}
		else {
			if ((input[56]) >= (14716.5)) {
				var16 = 0.0852721;
			}
			else {
				var16 = 0.023929002;
			}
		}
	}
	else {
		if ((input[38]) >= (2.875)) {
			if ((input[41]) >= (3.5)) {
				var16 = 0.12608722;
			}
			else {
				var16 = 0.017217042;
			}
		}
		else {
			if ((input[61]) >= (230.5)) {
				var16 = -0.010154654;
			}
			else {
				var16 = -0.0695129;
			}
		}
	}
	double var17;
	if ((input[60]) >= (43.5)) {
		if ((input[50]) >= (2.00585)) {
			if ((input[60]) >= (112.5)) {
				var17 = 0.105307;
			}
			else {
				var17 = 0.06966356;
			}
		}
		else {
			if ((input[46]) >= (3.97895)) {
				var17 = 0.0046144957;
			}
			else {
				var17 = -0.11828145;
			}
		}
	}
	else {
		if ((input[37]) >= (2.1666498)) {
			if ((input[3]) >= (0.3)) {
				var17 = 0.015153189;
			}
			else {
				var17 = 0.105718665;
			}
		}
		else {
			if ((input[61]) >= (235.5)) {
				var17 = -0.009378319;
			}
			else {
				var17 = -0.06839325;
			}
		}
	}
	double var18;
	if ((input[60]) >= (5.5)) {
		if ((input[61]) >= (125.5)) {
			if ((input[54]) >= (37.5)) {
				var18 = 0.021463206;
			}
			else {
				var18 = 0.07177113;
			}
		}
		else {
			if ((input[50]) >= (2.31835)) {
				var18 = -0.003550984;
			}
			else {
				var18 = -0.11341732;
			}
		}
	}
	else {
		if ((input[38]) >= (2.875)) {
			if ((input[41]) >= (3.5)) {
				var18 = 0.12352093;
			}
			else {
				var18 = 0.014568006;
			}
		}
		else {
			if ((input[37]) >= (2.6111498)) {
				var18 = 0.05209734;
			}
			else {
				var18 = -0.043528747;
			}
		}
	}
	double var19;
	if ((input[56]) >= (10194.5)) {
		if ((input[54]) >= (42.5)) {
			if ((input[61]) >= (38680.5)) {
				var19 = 0.039835714;
			}
			else {
				var19 = -0.038116816;
			}
		}
		else {
			if ((input[56]) >= (26938.5)) {
				var19 = 0.10276886;
			}
			else {
				var19 = 0.06938558;
			}
		}
	}
	else {
		if ((input[54]) >= (39.0)) {
			if ((input[38]) >= (2.875)) {
				var19 = 0.05507588;
			}
			else {
				var19 = -0.0760603;
			}
		}
		else {
			if ((input[61]) >= (265.5)) {
				var19 = 0.028386725;
			}
			else {
				var19 = -0.035400573;
			}
		}
	}
	double var20;
	if ((input[61]) >= (2041.5)) {
		if ((input[54]) >= (40.5)) {
			if ((input[56]) >= (24716.5)) {
				var20 = 0.034482494;
			}
			else {
				var20 = -0.04827567;
			}
		}
		else {
			if ((input[56]) >= (11057.5)) {
				var20 = 0.09361671;
			}
			else {
				var20 = 0.04351906;
			}
		}
	}
	else {
		if ((input[38]) >= (2.875)) {
			if ((input[41]) >= (2.5)) {
				var20 = 0.118138365;
			}
			else {
				var20 = -0.04074923;
			}
		}
		else {
			if ((input[54]) >= (35.5)) {
				var20 = -0.052234966;
			}
			else {
				var20 = 0.02399909;
			}
		}
	}
	double var21;
	if ((input[51]) >= (0.07845)) {
		if ((input[57]) >= (0.5)) {
			if ((input[54]) >= (45.5)) {
				var21 = -0.07585961;
			}
			else {
				var21 = 0.009932428;
			}
		}
		else {
			if ((input[60]) >= (110.5)) {
				var21 = 0.09750767;
			}
			else {
				var21 = 0.0441196;
			}
		}
	}
	else {
		if ((input[38]) >= (2.875)) {
			if ((input[41]) >= (3.5)) {
				var21 = 0.120949246;
			}
			else {
				var21 = -0.011169283;
			}
		}
		else {
			if ((input[37]) >= (2.6111498)) {
				var21 = 0.04253209;
			}
			else {
				var21 = -0.049373742;
			}
		}
	}
	double var22;
	if ((input[13]) >= (2.5)) {
		if ((input[2]) >= (2.5)) {
			if ((input[46]) >= (759.23035)) {
				var22 = 0.06422779;
			}
			else {
				var22 = 0.02178913;
			}
		}
		else {
			if ((input[61]) >= (504.5)) {
				var22 = -0.0108159585;
			}
			else {
				var22 = -0.07197758;
			}
		}
	}
	else {
		if ((input[37]) >= (2.94445)) {
			if ((input[41]) >= (2.5)) {
				var22 = 0.098522924;
			}
			else {
				var22 = -0.04103769;
			}
		}
		else {
			if ((input[60]) >= (4.5)) {
				var22 = 0.00020617763;
			}
			else {
				var22 = -0.051515914;
			}
		}
	}
	double var23;
	if ((input[60]) >= (47.5)) {
		if ((input[50]) >= (2.20895)) {
			if ((input[60]) >= (144.5)) {
				var23 = 0.09592174;
			}
			else {
				var23 = 0.059393253;
			}
		}
		else {
			if ((input[48]) >= (7.80165)) {
				var23 = -0.0045140474;
			}
			else {
				var23 = -0.10637865;
			}
		}
	}
	else {
		if ((input[61]) >= (10252.5)) {
			if ((input[54]) >= (44.5)) {
				var23 = -0.041455794;
			}
			else {
				var23 = 0.049306735;
			}
		}
		else {
			if ((input[54]) >= (39.0)) {
				var23 = -0.0588454;
			}
			else {
				var23 = -0.0008710284;
			}
		}
	}
	double var24;
	if ((input[51]) >= (0.05715)) {
		if ((input[54]) >= (37.5)) {
			if ((input[61]) >= (6035.0)) {
				var24 = 0.023662163;
			}
			else {
				var24 = -0.046036437;
			}
		}
		else {
			if ((input[56]) >= (222.5)) {
				var24 = 0.06418358;
			}
			else {
				var24 = -0.049182512;
			}
		}
	}
	else {
		if ((input[7]) >= (4.5)) {
			if ((input[38]) >= (2.125)) {
				var24 = 0.11168816;
			}
			else {
				var24 = -0.044396125;
			}
		}
		else {
			if ((input[61]) >= (140.5)) {
				var24 = -0.02225477;
			}
			else {
				var24 = -0.07596141;
			}
		}
	}
	double var25;
	if ((input[60]) >= (28.5)) {
		if ((input[48]) >= (907.2927)) {
			if ((input[60]) >= (182.5)) {
				var25 = 0.100580156;
			}
			else {
				var25 = 0.07035447;
			}
		}
		else {
			if ((input[46]) >= (3.4421)) {
				var25 = 0.040342156;
			}
			else {
				var25 = -0.083990656;
			}
		}
	}
	else {
		if ((input[61]) >= (1195.5)) {
			if ((input[54]) >= (40.5)) {
				var25 = -0.01858372;
			}
			else {
				var25 = 0.041879594;
			}
		}
		else {
			if ((input[38]) >= (2.875)) {
				var25 = 0.07458164;
			}
			else {
				var25 = -0.036311973;
			}
		}
	}
	double var26;
	if ((input[50]) >= (14.564449)) {
		if ((input[54]) >= (44.5)) {
			if ((input[61]) >= (68484.0)) {
				var26 = 0.028152669;
			}
			else {
				var26 = -0.058661252;
			}
		}
		else {
			if ((input[56]) >= (22568.5)) {
				var26 = 0.07298567;
			}
			else {
				var26 = 0.021642428;
			}
		}
	}
	else {
		if ((input[54]) >= (39.0)) {
			if ((input[61]) >= (29163.0)) {
				var26 = 0.01875352;
			}
			else {
				var26 = -0.052505683;
			}
		}
		else {
			if ((input[61]) >= (180.5)) {
				var26 = 0.023345899;
			}
			else {
				var26 = -0.030163368;
			}
		}
	}
	double var27;
	if ((input[37]) >= (2.1666498)) {
		if ((input[3]) >= (0.3)) {
			if ((input[50]) >= (13.224649)) {
				var27 = 0.050043132;
			}
			else {
				var27 = -0.0021874935;
			}
		}
		else {
			if ((input[0]) >= (3.0)) {
				var27 = -0.023244888;
			}
			else {
				var27 = 0.11838986;
			}
		}
	}
	else {
		if ((input[38]) >= (2.58335)) {
			if ((input[37]) >= (1.94445)) {
				var27 = 0.093902074;
			}
			else {
				var27 = 0.009366299;
			}
		}
		else {
			if ((input[61]) >= (139.5)) {
				var27 = -0.0065992563;
			}
			else {
				var27 = -0.07037052;
			}
		}
	}
	double var28;
	if ((input[60]) >= (49.5)) {
		if ((input[47]) >= (2.6349)) {
			if ((input[60]) >= (182.5)) {
				var28 = 0.09785621;
			}
			else {
				var28 = 0.06817152;
			}
		}
		else {
			if ((input[48]) >= (5.663)) {
				var28 = 0.04138912;
			}
			else {
				var28 = -0.08977672;
			}
		}
	}
	else {
		if ((input[37]) >= (1.5)) {
			if ((input[38]) >= (2.875)) {
				var28 = 0.049472477;
			}
			else {
				var28 = -0.00041725562;
			}
		}
		else {
			if ((input[61]) >= (96.5)) {
				var28 = -0.017369453;
			}
			else {
				var28 = -0.073543034;
			}
		}
	}
	double var29;
	if ((input[60]) >= (5.5)) {
		if ((input[51]) >= (0.01765)) {
			if ((input[54]) >= (35.5)) {
				var29 = 0.019615779;
			}
			else {
				var29 = 0.07471909;
			}
		}
		else {
			if ((input[61]) >= (95.5)) {
				var29 = -0.026048267;
			}
			else {
				var29 = -0.11291416;
			}
		}
	}
	else {
		if ((input[3]) >= (0.1)) {
			if ((input[2]) >= (2.5)) {
				var29 = 0.0001400974;
			}
			else {
				var29 = -0.04297974;
			}
		}
		else {
			if ((input[38]) >= (2.125)) {
				var29 = 0.121229135;
			}
			else {
				var29 = -0.041840263;
			}
		}
	}
	double var30;
	if ((input[15]) >= (9.5)) {
		if ((input[46]) >= (1097.2316)) {
			if ((input[5]) >= (4.1)) {
				var30 = 0.088355206;
			}
			else {
				var30 = 0.04741116;
			}
		}
		else {
			if ((input[51]) >= (0.01655)) {
				var30 = 0.036849942;
			}
			else {
				var30 = -0.053469278;
			}
		}
	}
	else {
		if ((input[54]) >= (35.5)) {
			if ((input[61]) >= (1493.5)) {
				var30 = 0.004074409;
			}
			else {
				var30 = -0.032845005;
			}
		}
		else {
			if ((input[61]) >= (94.5)) {
				var30 = 0.086293824;
			}
			else {
				var30 = -0.05392185;
			}
		}
	}
	double var31;
	if ((input[61]) >= (26751.5)) {
		if ((input[54]) >= (45.5)) {
			if ((input[0]) >= (6.0)) {
				var31 = 0.03672947;
			}
			else {
				var31 = -0.06732771;
			}
		}
		else {
			if ((input[54]) >= (42.5)) {
				var31 = 0.04018485;
			}
			else {
				var31 = 0.08446136;
			}
		}
	}
	else {
		if ((input[54]) >= (40.5)) {
			if ((input[0]) >= (1.5)) {
				var31 = -0.030907596;
			}
			else {
				var31 = -0.087106995;
			}
		}
		else {
			if ((input[61]) >= (445.5)) {
				var31 = 0.025779575;
			}
			else {
				var31 = -0.018719155;
			}
		}
	}
	double var32;
	if ((input[50]) >= (24.974651)) {
		if ((input[54]) >= (45.5)) {
			if ((input[0]) >= (6.0)) {
				var32 = 0.04881492;
			}
			else {
				var32 = -0.04119753;
			}
		}
		else {
			if ((input[61]) >= (56121.0)) {
				var32 = 0.08614339;
			}
			else {
				var32 = 0.037258536;
			}
		}
	}
	else {
		if ((input[54]) >= (39.0)) {
			if ((input[61]) >= (38708.5)) {
				var32 = 0.02320972;
			}
			else {
				var32 = -0.038942758;
			}
		}
		else {
			if ((input[47]) >= (0.36400002)) {
				var32 = 0.014046604;
			}
			else {
				var32 = -0.059756007;
			}
		}
	}
	double var33;
	if ((input[37]) >= (2.1666498)) {
		if ((input[3]) >= (0.3)) {
			if ((input[50]) >= (14.82615)) {
				var33 = 0.042447582;
			}
			else {
				var33 = -0.0019666625;
			}
		}
		else {
			if ((input[0]) >= (3.0)) {
				var33 = -0.020412406;
			}
			else {
				var33 = 0.11706706;
			}
		}
	}
	else {
		if ((input[38]) >= (2.58335)) {
			if ((input[3]) >= (1.225)) {
				var33 = 0.02369836;
			}
			else {
				var33 = 0.12503903;
			}
		}
		else {
			if ((input[61]) >= (126.5)) {
				var33 = -0.005753405;
			}
			else {
				var33 = -0.060722765;
			}
		}
	}
	double var34;
	if ((input[54]) >= (35.5)) {
		if ((input[61]) >= (4033.5)) {
			if ((input[54]) >= (42.5)) {
				var34 = -0.01563174;
			}
			else {
				var34 = 0.04344168;
			}
		}
		else {
			if ((input[3]) >= (0.1)) {
				var34 = -0.033620488;
			}
			else {
				var34 = 0.045382977;
			}
		}
	}
	else {
		if ((input[61]) >= (71.5)) {
			if ((input[1]) >= (5.28325)) {
				var34 = 0.04199182;
			}
			else {
				var34 = 0.09126334;
			}
		}
		else {
			if ((input[60]) >= (0.5)) {
				var34 = -0.07704919;
			}
			else {
				var34 = 0.026366571;
			}
		}
	}
	double var35;
	if ((input[54]) >= (37.5)) {
		if ((input[61]) >= (13568.5)) {
			if ((input[54]) >= (43.5)) {
				var35 = -0.022102432;
			}
			else {
				var35 = 0.043100804;
			}
		}
		else {
			if ((input[2]) >= (0.5)) {
				var35 = -0.041229535;
			}
			else {
				var35 = 0.03863629;
			}
		}
	}
	else {
		if ((input[61]) >= (180.5)) {
			if ((input[0]) >= (6.0)) {
				var35 = 0.07826036;
			}
			else {
				var35 = 0.031063884;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var35 = -0.022551728;
			}
			else {
				var35 = -0.07096732;
			}
		}
	}
	double var36;
	if ((input[54]) >= (41.5)) {
		if ((input[61]) >= (33874.5)) {
			if ((input[54]) >= (45.5)) {
				var36 = -0.03046379;
			}
			else {
				var36 = 0.046711244;
			}
		}
		else {
			if ((input[0]) >= (1.5)) {
				var36 = -0.025691269;
			}
			else {
				var36 = -0.071239;
			}
		}
	}
	else {
		if ((input[61]) >= (4992.5)) {
			if ((input[56]) >= (25423.5)) {
				var36 = 0.080507986;
			}
			else {
				var36 = 0.039486196;
			}
		}
		else {
			if ((input[13]) >= (0.5)) {
				var36 = -0.007418287;
			}
			else {
				var36 = 0.06269383;
			}
		}
	}
	double var37;
	if ((input[22]) >= (2.5)) {
		if ((input[51]) >= (0.03765)) {
			if ((input[54]) >= (35.5)) {
				var37 = 0.03012061;
			}
			else {
				var37 = 0.06418247;
			}
		}
		else {
			if ((input[61]) >= (59.5)) {
				var37 = -0.009528655;
			}
			else {
				var37 = -0.07706024;
			}
		}
	}
	else {
		if ((input[37]) >= (2.6111498)) {
			if ((input[60]) >= (0.5)) {
				var37 = 0.006493803;
			}
			else {
				var37 = 0.09921445;
			}
		}
		else {
			if ((input[60]) >= (3.5)) {
				var37 = 0.0034959258;
			}
			else {
				var37 = -0.026795601;
			}
		}
	}
	double var38;
	if ((input[50]) >= (24.99415)) {
		if ((input[32]) >= (3.5)) {
			if ((input[50]) >= (36.5254)) {
				var38 = 0.034326006;
			}
			else {
				var38 = -0.013760313;
			}
		}
		else {
			if ((input[54]) >= (46.5)) {
				var38 = -0.051523637;
			}
			else {
				var38 = 0.047545932;
			}
		}
	}
	else {
		if ((input[54]) >= (40.5)) {
			if ((input[61]) >= (49899.0)) {
				var38 = 0.026407173;
			}
			else {
				var38 = -0.037175845;
			}
		}
		else {
			if ((input[61]) >= (920.5)) {
				var38 = 0.025607198;
			}
			else {
				var38 = -0.010708968;
			}
		}
	}
	double var39;
	if ((input[54]) >= (37.5)) {
		if ((input[61]) >= (10252.0)) {
			if ((input[54]) >= (43.5)) {
				var39 = -0.022407455;
			}
			else {
				var39 = 0.034162913;
			}
		}
		else {
			if ((input[7]) >= (4.5)) {
				var39 = 0.038242057;
			}
			else {
				var39 = -0.04040498;
			}
		}
	}
	else {
		if ((input[61]) >= (140.5)) {
			if ((input[0]) >= (6.0)) {
				var39 = 0.0691802;
			}
			else {
				var39 = 0.023674246;
			}
		}
		else {
			if ((input[23]) >= (1.45)) {
				var39 = -0.06135572;
			}
			else {
				var39 = -0.011771551;
			}
		}
	}
	double var40;
	if ((input[37]) >= (1.5)) {
		if ((input[37]) >= (1.94445)) {
			if ((input[38]) >= (2.875)) {
				var40 = 0.040315837;
			}
			else {
				var40 = -0.0031152954;
			}
		}
		else {
			if ((input[3]) >= (0.1)) {
				var40 = 0.032954186;
			}
			else {
				var40 = 0.36619708;
			}
		}
	}
	else {
		if ((input[61]) >= (2443.5)) {
			if ((input[54]) >= (44.5)) {
				var40 = -0.0398915;
			}
			else {
				var40 = 0.01663909;
			}
		}
		else {
			if ((input[54]) >= (35.5)) {
				var40 = -0.04676748;
			}
			else {
				var40 = 0.02535964;
			}
		}
	}
	double var41;
	if ((input[61]) >= (60934.0)) {
		if ((input[54]) >= (47.5)) {
			if ((input[36]) >= (6.0483)) {
				var41 = -0.035306424;
			}
			else {
				var41 = -0.1077673;
			}
		}
		else {
			if ((input[54]) >= (45.5)) {
				var41 = 0.019847868;
			}
			else {
				var41 = 0.06823372;
			}
		}
	}
	else {
		if ((input[54]) >= (42.5)) {
			if ((input[51]) >= (0.36725)) {
				var41 = 0.009364068;
			}
			else {
				var41 = -0.045510735;
			}
		}
		else {
			if ((input[56]) >= (7612.5)) {
				var41 = 0.03967656;
			}
			else {
				var41 = -0.0024802343;
			}
		}
	}
	double var42;
	if ((input[54]) >= (37.5)) {
		if ((input[61]) >= (16324.5)) {
			if ((input[54]) >= (45.5)) {
				var42 = -0.034076378;
			}
			else {
				var42 = 0.03023145;
			}
		}
		else {
			if ((input[3]) >= (0.1)) {
				var42 = -0.030803988;
			}
			else {
				var42 = 0.04379338;
			}
		}
	}
	else {
		if ((input[61]) >= (125.5)) {
			if ((input[0]) >= (6.0)) {
				var42 = 0.061421294;
			}
			else {
				var42 = 0.020652233;
			}
		}
		else {
			if ((input[37]) >= (2.94445)) {
				var42 = 0.02946574;
			}
			else {
				var42 = -0.05153581;
			}
		}
	}
	double var43;
	if ((input[50]) >= (24.99415)) {
		if ((input[32]) >= (3.5)) {
			if ((input[50]) >= (44.36915)) {
				var43 = 0.04638768;
			}
			else {
				var43 = -0.007888701;
			}
		}
		else {
			if ((input[4]) >= (9.5)) {
				var43 = 0.070015825;
			}
			else {
				var43 = 0.030666355;
			}
		}
	}
	else {
		if ((input[54]) >= (42.5)) {
			if ((input[61]) >= (49899.0)) {
				var43 = 0.013569365;
			}
			else {
				var43 = -0.042608347;
			}
		}
		else {
			if ((input[61]) >= (3254.5)) {
				var43 = 0.02607323;
			}
			else {
				var43 = -0.0058297208;
			}
		}
	}
	double var44;
	if ((input[22]) >= (2.5)) {
		if ((input[50]) >= (1.28325)) {
			if ((input[0]) >= (6.0)) {
				var44 = 0.043308027;
			}
			else {
				var44 = 0.018073767;
			}
		}
		else {
			if ((input[1]) >= (1.49805)) {
				var44 = -0.06360048;
			}
			else {
				var44 = -0.011019778;
			}
		}
	}
	else {
		if ((input[13]) >= (0.5)) {
			if ((input[61]) >= (712.5)) {
				var44 = 0.00035244937;
			}
			else {
				var44 = -0.030617362;
			}
		}
		else {
			if ((input[38]) >= (2.125)) {
				var44 = 0.11281782;
			}
			else {
				var44 = -0.023434136;
			}
		}
	}
	double var45;
	if ((input[54]) >= (37.5)) {
		if ((input[56]) >= (70.5)) {
			if ((input[61]) >= (3197.5)) {
				var45 = 0.004049796;
			}
			else {
				var45 = -0.045734655;
			}
		}
		else {
			if ((input[38]) >= (2.125)) {
				var45 = 0.113882065;
			}
			else {
				var45 = -0.03384406;
			}
		}
	}
	else {
		if ((input[61]) >= (246.5)) {
			if ((input[0]) >= (6.0)) {
				var45 = 0.06126648;
			}
			else {
				var45 = 0.02539486;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var45 = -0.004219274;
			}
			else {
				var45 = -0.05836463;
			}
		}
	}
	double var46;
	if ((input[54]) >= (40.5)) {
		if ((input[61]) >= (68855.0)) {
			if ((input[54]) >= (47.5)) {
				var46 = -0.06704118;
			}
			else {
				var46 = 0.04913182;
			}
		}
		else {
			if ((input[54]) >= (45.5)) {
				var46 = -0.07467909;
			}
			else {
				var46 = -0.016125942;
			}
		}
	}
	else {
		if ((input[56]) >= (8332.5)) {
			if ((input[47]) >= (2.3161998)) {
				var46 = 0.059838112;
			}
			else {
				var46 = 0.0033446949;
			}
		}
		else {
			if ((input[47]) >= (0.21005)) {
				var46 = 0.005685993;
			}
			else {
				var46 = -0.058374878;
			}
		}
	}
	double var47;
	if ((input[54]) >= (35.5)) {
		if ((input[61]) >= (1492.5)) {
			if ((input[54]) >= (43.5)) {
				var47 = -0.028326435;
			}
			else {
				var47 = 0.015305497;
			}
		}
		else {
			if ((input[7]) >= (4.5)) {
				var47 = 0.03857434;
			}
			else {
				var47 = -0.03596967;
			}
		}
	}
	else {
		if ((input[61]) >= (71.5)) {
			if ((input[1]) >= (5.28325)) {
				var47 = 0.014636302;
			}
			else {
				var47 = 0.06622069;
			}
		}
		else {
			if ((input[60]) >= (0.5)) {
				var47 = -0.062324334;
			}
			else {
				var47 = 0.03925206;
			}
		}
	}
	double var48;
	if ((input[61]) >= (50101.0)) {
		if ((input[54]) >= (45.5)) {
			if ((input[61]) >= (111739.0)) {
				var48 = 0.042399097;
			}
			else {
				var48 = -0.034807157;
			}
		}
		else {
			if ((input[61]) >= (105210.0)) {
				var48 = 0.08433783;
			}
			else {
				var48 = 0.042785518;
			}
		}
	}
	else {
		if ((input[54]) >= (42.5)) {
			if ((input[0]) >= (6.0)) {
				var48 = 0.001434015;
			}
			else {
				var48 = -0.041166596;
			}
		}
		else {
			if ((input[13]) >= (0.5)) {
				var48 = 0.00017991613;
			}
			else {
				var48 = 0.044855036;
			}
		}
	}
	double var49;
	if ((input[50]) >= (30.94725)) {
		if ((input[0]) >= (6.0)) {
			if ((input[3]) >= (2.1)) {
				var49 = 0.086991765;
			}
			else {
				var49 = 0.04752107;
			}
		}
		else {
			if ((input[15]) >= (9.5)) {
				var49 = 0.06124493;
			}
			else {
				var49 = 0.018235212;
			}
		}
	}
	else {
		if ((input[54]) >= (37.5)) {
			if ((input[56]) >= (97.5)) {
				var49 = -0.018456439;
			}
			else {
				var49 = 0.033970136;
			}
		}
		else {
			if ((input[61]) >= (180.5)) {
				var49 = 0.02729841;
			}
			else {
				var49 = -0.029431297;
			}
		}
	}
	double var50;
	if ((input[61]) >= (29169.5)) {
		if ((input[54]) >= (45.5)) {
			if ((input[0]) >= (6.0)) {
				var50 = 0.04867122;
			}
			else {
				var50 = -0.038674247;
			}
		}
		else {
			if ((input[54]) >= (41.5)) {
				var50 = 0.029623985;
			}
			else {
				var50 = 0.06992584;
			}
		}
	}
	else {
		if ((input[54]) >= (40.5)) {
			if ((input[0]) >= (1.5)) {
				var50 = -0.01451322;
			}
			else {
				var50 = -0.058233656;
			}
		}
		else {
			if ((input[47]) >= (0.21005)) {
				var50 = 0.0075401613;
			}
			else {
				var50 = -0.053341687;
			}
		}
	}
	double var51;
	if ((input[61]) >= (5992.5)) {
		if ((input[54]) >= (43.5)) {
			if ((input[61]) >= (68865.0)) {
				var51 = 0.028349368;
			}
			else {
				var51 = -0.037684746;
			}
		}
		else {
			if ((input[0]) >= (1.5)) {
				var51 = 0.037480995;
			}
			else {
				var51 = 0.011662673;
			}
		}
	}
	else {
		if ((input[54]) >= (40.5)) {
			if ((input[38]) >= (2.875)) {
				var51 = 0.05341372;
			}
			else {
				var51 = -0.046079945;
			}
		}
		else {
			if ((input[2]) >= (0.5)) {
				var51 = -0.004043541;
			}
			else {
				var51 = 0.0471577;
			}
		}
	}
	double var52;
	if ((input[49]) >= (0.99645)) {
		if ((input[51]) >= (0.04905)) {
			if ((input[0]) >= (6.0)) {
				var52 = 0.031128768;
			}
			else {
				var52 = -0.004375189;
			}
		}
		else {
			if ((input[60]) >= (0.5)) {
				var52 = -0.0352535;
			}
			else {
				var52 = 0.018732954;
			}
		}
	}
	else {
		if ((input[50]) >= (0.52535)) {
			if ((input[61]) >= (3243.5)) {
				var52 = 0.03248878;
			}
			else {
				var52 = 0.008320204;
			}
		}
		else {
			if ((input[12]) >= (0.00585)) {
				var52 = -0.000676006;
			}
			else {
				var52 = 0.14493418;
			}
		}
	}
	double var53;
	if ((input[36]) >= (5.52055)) {
		if ((input[0]) >= (6.0)) {
			if ((input[51]) >= (0.00305)) {
				var53 = 0.036394283;
			}
			else {
				var53 = -0.089783095;
			}
		}
		else {
			if ((input[61]) >= (932.5)) {
				var53 = 0.008976051;
			}
			else {
				var53 = -0.026252454;
			}
		}
	}
	else {
		if ((input[7]) >= (4.5)) {
			if ((input[38]) >= (2.125)) {
				var53 = 0.090600125;
			}
			else {
				var53 = -0.021689493;
			}
		}
		else {
			if ((input[51]) >= (0.051349998)) {
				var53 = -0.0026329493;
			}
			else {
				var53 = -0.036268044;
			}
		}
	}
	double var54;
	if ((input[54]) >= (37.5)) {
		if ((input[56]) >= (13540.0)) {
			if ((input[54]) >= (44.5)) {
				var54 = -0.015800714;
			}
			else {
				var54 = 0.023110962;
			}
		}
		else {
			if ((input[2]) >= (1.5)) {
				var54 = -0.031099474;
			}
			else {
				var54 = 0.016169248;
			}
		}
	}
	else {
		if ((input[61]) >= (252.5)) {
			if ((input[48]) >= (1559.4359)) {
				var54 = 0.084166236;
			}
			else {
				var54 = 0.02444151;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var54 = -0.00033545602;
			}
			else {
				var54 = -0.04957815;
			}
		}
	}
	double var55;
	if ((input[61]) >= (106243.0)) {
		if ((input[54]) >= (47.5)) {
			if ((input[52]) >= (2270.626)) {
				var55 = 0.054712486;
			}
			else {
				var55 = -0.06947636;
			}
		}
		else {
			if ((input[45]) >= (146.71875)) {
				var55 = -0.086507775;
			}
			else {
				var55 = 0.07097285;
			}
		}
	}
	else {
		if ((input[54]) >= (45.5)) {
			if ((input[0]) >= (6.0)) {
				var55 = -0.016187826;
			}
			else {
				var55 = -0.07722811;
			}
		}
		else {
			if ((input[56]) >= (17585.5)) {
				var55 = 0.02092885;
			}
			else {
				var55 = -0.0028065785;
			}
		}
	}
	double var56;
	if ((input[49]) >= (0.96815)) {
		if ((input[61]) >= (1200.5)) {
			if ((input[54]) >= (37.5)) {
				var56 = -0.00528649;
			}
			else {
				var56 = 0.036348924;
			}
		}
		else {
			if ((input[41]) >= (2.5)) {
				var56 = 0.0013326511;
			}
			else {
				var56 = -0.035918664;
			}
		}
	}
	else {
		if ((input[37]) >= (2.3888502)) {
			if ((input[60]) >= (0.5)) {
				var56 = 0.016072942;
			}
			else {
				var56 = 0.0945717;
			}
		}
		else {
			if ((input[54]) >= (35.5)) {
				var56 = 0.0024510608;
			}
			else {
				var56 = 0.044717267;
			}
		}
	}
	double var57;
	if ((input[0]) >= (6.0)) {
		if ((input[61]) >= (64.5)) {
			if ((input[24]) >= (8.0)) {
				var57 = 0.0014027874;
			}
			else {
				var57 = 0.039465982;
			}
		}
		else {
			if ((input[35]) >= (4.5)) {
				var57 = 0.07149054;
			}
			else {
				var57 = -0.05562141;
			}
		}
	}
	else {
		if ((input[13]) >= (0.5)) {
			if ((input[61]) >= (936.5)) {
				var57 = 0.00079508935;
			}
			else {
				var57 = -0.03316696;
			}
		}
		else {
			if ((input[62]) >= (33.5)) {
				var57 = 0.08566013;
			}
			else {
				var57 = -0.047720846;
			}
		}
	}
	double var58;
	if ((input[54]) >= (41.5)) {
		if ((input[56]) >= (10171.5)) {
			if ((input[38]) >= (2.125)) {
				var58 = -0.02162419;
			}
			else {
				var58 = 0.017401643;
			}
		}
		else {
			if ((input[0]) >= (1.5)) {
				var58 = -0.022684697;
			}
			else {
				var58 = -0.09227624;
			}
		}
	}
	else {
		if ((input[56]) >= (13539.5)) {
			if ((input[46]) >= (522.84863)) {
				var58 = 0.057103295;
			}
			else {
				var58 = 0.013655704;
			}
		}
		else {
			if ((input[2]) >= (1.5)) {
				var58 = -0.0034417317;
			}
			else {
				var58 = 0.02358465;
			}
		}
	}
	double var59;
	if ((input[54]) >= (36.5)) {
		if ((input[61]) >= (1927.5)) {
			if ((input[54]) >= (43.5)) {
				var59 = -0.020910025;
			}
			else {
				var59 = 0.011354836;
			}
		}
		else {
			if ((input[7]) >= (4.5)) {
				var59 = 0.032448288;
			}
			else {
				var59 = -0.03177234;
			}
		}
	}
	else {
		if ((input[56]) >= (108.5)) {
			if ((input[51]) >= (0.00915)) {
				var59 = 0.032325383;
			}
			else {
				var59 = -0.035054754;
			}
		}
		else {
			if ((input[60]) >= (0.5)) {
				var59 = -0.06042356;
			}
			else {
				var59 = 0.011645833;
			}
		}
	}
	double var60;
	if ((input[54]) >= (37.5)) {
		if ((input[56]) >= (97.5)) {
			if ((input[61]) >= (1740.5)) {
				var60 = 0.0005340238;
			}
			else {
				var60 = -0.044125598;
			}
		}
		else {
			if ((input[38]) >= (2.125)) {
				var60 = 0.10361695;
			}
			else {
				var60 = -0.029552875;
			}
		}
	}
	else {
		if ((input[61]) >= (58.5)) {
			if ((input[61]) >= (520.5)) {
				var60 = 0.027101919;
			}
			else {
				var60 = 0.003597406;
			}
		}
		else {
			if ((input[37]) >= (2.6667)) {
				var60 = 0.053735156;
			}
			else {
				var60 = -0.04963934;
			}
		}
	}
	double var61;
	if ((input[0]) >= (6.0)) {
		if ((input[51]) >= (0.01165)) {
			if ((input[56]) >= (95.5)) {
				var61 = 0.027283255;
			}
			else {
				var61 = -0.027364476;
			}
		}
		else {
			if ((input[41]) >= (3.5)) {
				var61 = 0.05559332;
			}
			else {
				var61 = -0.064127244;
			}
		}
	}
	else {
		if ((input[13]) >= (0.5)) {
			if ((input[61]) >= (883.5)) {
				var61 = -0.00017081886;
			}
			else {
				var61 = -0.029374722;
			}
		}
		else {
			if ((input[38]) >= (2.125)) {
				var61 = 0.109855115;
			}
			else {
				var61 = -0.014707659;
			}
		}
	}
	double var62;
	if ((input[61]) >= (74847.5)) {
		if ((input[54]) >= (47.5)) {
			if ((input[46]) >= (988.7711)) {
				var62 = -0.06758597;
			}
			else {
				var62 = 0.024894955;
			}
		}
		else {
			if ((input[61]) >= (132811.5)) {
				var62 = 0.075299524;
			}
			else {
				var62 = 0.03253186;
			}
		}
	}
	else {
		if ((input[54]) >= (42.5)) {
			if ((input[56]) >= (10289.5)) {
				var62 = -0.010706692;
			}
			else {
				var62 = -0.045982517;
			}
		}
		else {
			if ((input[56]) >= (7612.5)) {
				var62 = 0.023007972;
			}
			else {
				var62 = -0.00039983168;
			}
		}
	}
	double var63;
	if ((input[49]) >= (0.96815)) {
		if ((input[54]) >= (37.5)) {
			if ((input[61]) >= (29169.5)) {
				var63 = 0.014292565;
			}
			else {
				var63 = -0.017735917;
			}
		}
		else {
			if ((input[61]) >= (463.5)) {
				var63 = 0.024458406;
			}
			else {
				var63 = -0.015232741;
			}
		}
	}
	else {
		if ((input[49]) >= (0.2175)) {
			if ((input[50]) >= (0.52535)) {
				var63 = 0.011465415;
			}
			else {
				var63 = 0.09416037;
			}
		}
		else {
			if ((input[12]) >= (0.07225)) {
				var63 = 0.074261665;
			}
			else {
				var63 = -0.06875139;
			}
		}
	}
	double var64;
	if ((input[0]) >= (6.0)) {
		if ((input[61]) >= (64.5)) {
			if ((input[24]) >= (8.0)) {
				var64 = -0.0016043399;
			}
			else {
				var64 = 0.033958405;
			}
		}
		else {
			if ((input[35]) >= (4.5)) {
				var64 = 0.06939634;
			}
			else {
				var64 = -0.04637481;
			}
		}
	}
	else {
		if ((input[13]) >= (0.5)) {
			if ((input[61]) >= (936.5)) {
				var64 = -0.00054253446;
			}
			else {
				var64 = -0.026194692;
			}
		}
		else {
			if ((input[38]) >= (2.125)) {
				var64 = 0.10710193;
			}
			else {
				var64 = -0.012067482;
			}
		}
	}
	double var65;
	if ((input[54]) >= (45.5)) {
		if ((input[56]) >= (85226.5)) {
			if ((input[54]) >= (47.5)) {
				var65 = -0.045617722;
			}
			else {
				var65 = 0.026236702;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var65 = -0.022207096;
			}
			else {
				var65 = -0.08048076;
			}
		}
	}
	else {
		if ((input[61]) >= (34188.5)) {
			if ((input[54]) >= (41.5)) {
				var65 = 0.023526235;
			}
			else {
				var65 = 0.06419572;
			}
		}
		else {
			if ((input[54]) >= (40.5)) {
				var65 = -0.01499329;
			}
			else {
				var65 = 0.0042922082;
			}
		}
	}
	double var66;
	if ((input[50]) >= (38.632797)) {
		if ((input[39]) >= (4.125)) {
			if ((input[60]) >= (0.5)) {
				var66 = 0.059458543;
			}
			else {
				var66 = -0.022003612;
			}
		}
		else {
			if ((input[49]) >= (0.50485003)) {
				var66 = 0.015053265;
			}
			else {
				var66 = 0.089317925;
			}
		}
	}
	else {
		if ((input[54]) >= (42.5)) {
			if ((input[56]) >= (10200.5)) {
				var66 = -0.006992268;
			}
			else {
				var66 = -0.042748723;
			}
		}
		else {
			if ((input[41]) >= (2.5)) {
				var66 = 0.012281831;
			}
			else {
				var66 = -0.0049572987;
			}
		}
	}
	double var67;
	if ((input[37]) >= (1.5)) {
		if ((input[37]) >= (1.94445)) {
			if ((input[37]) >= (2.1666498)) {
				var67 = 0.008403701;
			}
			else {
				var67 = -0.013947767;
			}
		}
		else {
			if ((input[3]) >= (0.1)) {
				var67 = 0.03055498;
			}
			else {
				var67 = 0.2874782;
			}
		}
	}
	else {
		if ((input[61]) >= (3557.5)) {
			if ((input[48]) >= (285.0287)) {
				var67 = -0.0016103905;
			}
			else {
				var67 = 0.034794692;
			}
		}
		else {
			if ((input[54]) >= (36.5)) {
				var67 = -0.03340071;
			}
			else {
				var67 = 0.008170637;
			}
		}
	}
	double var68;
	if ((input[54]) >= (45.5)) {
		if ((input[56]) >= (66671.5)) {
			if ((input[54]) >= (47.5)) {
				var68 = -0.049126;
			}
			else {
				var68 = 0.014199005;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var68 = -0.022804514;
			}
			else {
				var68 = -0.08467778;
			}
		}
	}
	else {
		if ((input[61]) >= (35177.5)) {
			if ((input[61]) >= (105210.0)) {
				var68 = 0.06844514;
			}
			else {
				var68 = 0.022694936;
			}
		}
		else {
			if ((input[2]) >= (1.5)) {
				var68 = -0.0052878554;
			}
			else {
				var68 = 0.016255049;
			}
		}
	}
	double var69;
	if ((input[49]) >= (0.96815)) {
		if ((input[45]) >= (157.27539)) {
			if ((input[1]) >= (11.839849)) {
				var69 = -0.09724564;
			}
			else {
				var69 = -0.026485896;
			}
		}
		else {
			if ((input[37]) >= (1.61115)) {
				var69 = 0.003266882;
			}
			else {
				var69 = -0.0135775795;
			}
		}
	}
	else {
		if ((input[45]) >= (88.34575)) {
			if ((input[53]) >= (0.99985003)) {
				var69 = 0.05330093;
			}
			else {
				var69 = 0.015211458;
			}
		}
		else {
			if ((input[50]) >= (0.52535)) {
				var69 = 0.00483969;
			}
			else {
				var69 = 0.08636289;
			}
		}
	}
	double var70;
	if ((input[36]) >= (6.03365)) {
		if ((input[22]) >= (2.5)) {
			if ((input[51]) >= (0.038450003)) {
				var70 = 0.037009284;
			}
			else {
				var70 = -0.005336258;
			}
		}
		else {
			if ((input[61]) >= (346.5)) {
				var70 = 0.010944922;
			}
			else {
				var70 = -0.022714684;
			}
		}
	}
	else {
		if ((input[43]) >= (0.5)) {
			if ((input[51]) >= (0.02225)) {
				var70 = -0.004793332;
			}
			else {
				var70 = -0.05493469;
			}
		}
		else {
			if ((input[13]) >= (1.5)) {
				var70 = -0.0040173563;
			}
			else {
				var70 = 0.029354293;
			}
		}
	}
	double var71;
	if ((input[0]) >= (6.0)) {
		if ((input[61]) >= (56.5)) {
			if ((input[24]) >= (8.0)) {
				var71 = -0.0008534219;
			}
			else {
				var71 = 0.030488053;
			}
		}
		else {
			if ((input[31]) >= (4.5)) {
				var71 = 0.04054671;
			}
			else {
				var71 = -0.05098359;
			}
		}
	}
	else {
		if ((input[14]) >= (0.675)) {
			if ((input[61]) >= (732.5)) {
				var71 = -0.00101641;
			}
			else {
				var71 = -0.025913438;
			}
		}
		else {
			if ((input[38]) >= (2.125)) {
				var71 = 0.10497425;
			}
			else {
				var71 = -0.013206634;
			}
		}
	}
	double var72;
	if ((input[54]) >= (37.5)) {
		if ((input[56]) >= (99.5)) {
			if ((input[56]) >= (1790.5)) {
				var72 = -0.0007008483;
			}
			else {
				var72 = -0.041909512;
			}
		}
		else {
			if ((input[62]) >= (33.5)) {
				var72 = 0.06325706;
			}
			else {
				var72 = -0.06419623;
			}
		}
	}
	else {
		if ((input[56]) >= (223.5)) {
			if ((input[12]) >= (0.009749999)) {
				var72 = 0.020550234;
			}
			else {
				var72 = -0.045412965;
			}
		}
		else {
			if ((input[23]) >= (1.225)) {
				var72 = -0.03139959;
			}
			else {
				var72 = 0.024605054;
			}
		}
	}
	double var73;
	if ((input[54]) >= (42.5)) {
		if ((input[56]) >= (10289.5)) {
			if ((input[38]) >= (2.125)) {
				var73 = -0.02781021;
			}
			else {
				var73 = 0.013265717;
			}
		}
		else {
			if ((input[0]) >= (1.5)) {
				var73 = -0.019525293;
			}
			else {
				var73 = -0.0818843;
			}
		}
	}
	else {
		if ((input[56]) >= (17579.0)) {
			if ((input[46]) >= (134.44455)) {
				var73 = 0.036437523;
			}
			else {
				var73 = -0.05408138;
			}
		}
		else {
			if ((input[41]) >= (2.5)) {
				var73 = 0.010448086;
			}
			else {
				var73 = -0.007297173;
			}
		}
	}
	double var74;
	if ((input[0]) >= (6.0)) {
		if ((input[51]) >= (0.00755)) {
			if ((input[56]) >= (95.5)) {
				var74 = 0.021621536;
			}
			else {
				var74 = -0.021321489;
			}
		}
		else {
			if ((input[45]) >= (34.0586)) {
				var74 = -0.06902032;
			}
			else {
				var74 = 0.036836185;
			}
		}
	}
	else {
		if ((input[13]) >= (0.5)) {
			if ((input[56]) >= (665.5)) {
				var74 = -0.0018530624;
			}
			else {
				var74 = -0.026251813;
			}
		}
		else {
			if ((input[62]) >= (33.5)) {
				var74 = 0.068782;
			}
			else {
				var74 = -0.040483963;
			}
		}
	}
	double var75;
	if ((input[54]) >= (37.5)) {
		if ((input[61]) >= (3763.5)) {
			if ((input[38]) >= (2.125)) {
				var75 = -0.014419704;
			}
			else {
				var75 = 0.016522609;
			}
		}
		else {
			if ((input[56]) >= (138.5)) {
				var75 = -0.028224757;
			}
			else {
				var75 = 0.020485876;
			}
		}
	}
	else {
		if ((input[61]) >= (264.5)) {
			if ((input[1]) >= (0.00585)) {
				var75 = 0.020796886;
			}
			else {
				var75 = -0.05153913;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var75 = 0.0047072046;
			}
			else {
				var75 = -0.033354457;
			}
		}
	}
	double var76;
	if ((input[54]) >= (45.5)) {
		if ((input[61]) >= (111739.0)) {
			if ((input[54]) >= (47.5)) {
				var76 = -0.030115634;
			}
			else {
				var76 = 0.042161584;
			}
		}
		else {
			if ((input[50]) >= (19.205101)) {
				var76 = -0.016971769;
			}
			else {
				var76 = -0.06754886;
			}
		}
	}
	else {
		if ((input[61]) >= (46835.0)) {
			if ((input[61]) >= (105210.0)) {
				var76 = 0.06459526;
			}
			else {
				var76 = 0.022760382;
			}
		}
		else {
			if ((input[54]) >= (41.5)) {
				var76 = -0.01251251;
			}
			else {
				var76 = 0.0036985036;
			}
		}
	}
	double var77;
	if ((input[50]) >= (35.849648)) {
		if ((input[39]) >= (2.625)) {
			if ((input[46]) >= (2475.6042)) {
				var77 = 0.064261205;
			}
			else {
				var77 = 0.027598282;
			}
		}
		else {
			if ((input[49]) >= (0.49395)) {
				var77 = 0.006447237;
			}
			else {
				var77 = 0.07416963;
			}
		}
	}
	else {
		if ((input[54]) >= (41.5)) {
			if ((input[56]) >= (6071.5)) {
				var77 = -0.0053929947;
			}
			else {
				var77 = -0.032343715;
			}
		}
		else {
			if ((input[60]) >= (0.5)) {
				var77 = -0.00059981673;
			}
			else {
				var77 = 0.023596354;
			}
		}
	}
	double var78;
	if ((input[0]) >= (6.0)) {
		if ((input[52]) >= (3.68685)) {
			if ((input[1]) >= (0.017549999)) {
				var78 = 0.024286851;
			}
			else {
				var78 = -0.03810949;
			}
		}
		else {
			if ((input[1]) >= (2.4297)) {
				var78 = -0.06837086;
			}
			else {
				var78 = 0.0063249464;
			}
		}
	}
	else {
		if ((input[2]) >= (1.5)) {
			if ((input[56]) >= (553.5)) {
				var78 = -0.0024050854;
			}
			else {
				var78 = -0.039470106;
			}
		}
		else {
			if ((input[38]) >= (2.125)) {
				var78 = 0.07883537;
			}
			else {
				var78 = -0.007988635;
			}
		}
	}
	double var79;
	if ((input[45]) >= (154.89255)) {
		if ((input[1]) >= (11.832001)) {
			if ((input[47]) >= (2.1075)) {
				var79 = -0.0439108;
			}
			else {
				var79 = -0.112916626;
			}
		}
		else {
			if ((input[60]) >= (21.5)) {
				var79 = 0.057534654;
			}
			else {
				var79 = -0.024600854;
			}
		}
	}
	else {
		if ((input[54]) >= (45.5)) {
			if ((input[56]) >= (39541.5)) {
				var79 = -0.006857653;
			}
			else {
				var79 = -0.06029736;
			}
		}
		else {
			if ((input[61]) >= (25864.5)) {
				var79 = 0.022058329;
			}
			else {
				var79 = 0.00025958798;
			}
		}
	}
	double var80;
	if ((input[54]) >= (42.5)) {
		if ((input[51]) >= (0.39125)) {
			if ((input[0]) >= (6.0)) {
				var80 = 0.056188215;
			}
			else {
				var80 = 0.010564971;
			}
		}
		else {
			if ((input[56]) >= (10289.5)) {
				var80 = -0.009981846;
			}
			else {
				var80 = -0.039940048;
			}
		}
	}
	else {
		if ((input[61]) >= (8322.5)) {
			if ((input[60]) >= (220.5)) {
				var80 = 0.07367026;
			}
			else {
				var80 = 0.018754398;
			}
		}
		else {
			if ((input[1]) >= (3.8418)) {
				var80 = -0.011265379;
			}
			else {
				var80 = 0.006570092;
			}
		}
	}
	double var81;
	if ((input[62]) >= (56.5)) {
		if ((input[47]) >= (0.15905)) {
			if ((input[54]) >= (37.5)) {
				var81 = -0.013444038;
			}
			else {
				var81 = 0.007843373;
			}
		}
		else {
			if ((input[37]) >= (2.72225)) {
				var81 = 0.035447445;
			}
			else {
				var81 = -0.08742732;
			}
		}
	}
	else {
		if ((input[14]) >= (0.675)) {
			if ((input[0]) >= (3.0)) {
				var81 = 0.011426412;
			}
			else {
				var81 = -0.005958015;
			}
		}
		else {
			if ((input[62]) >= (33.5)) {
				var81 = 0.10595622;
			}
			else {
				var81 = -0.023932043;
			}
		}
	}
	double var82;
	if ((input[36]) >= (5.52055)) {
		if ((input[47]) >= (0.13525)) {
			if ((input[48]) >= (0.2499)) {
				var82 = 0.006147397;
			}
			else {
				var82 = 0.10766508;
			}
		}
		else {
			if ((input[13]) >= (0.5)) {
				var82 = -0.10055915;
			}
			else {
				var82 = 0.04399987;
			}
		}
	}
	else {
		if ((input[43]) >= (0.5)) {
			if ((input[51]) >= (0.03395)) {
				var82 = -0.007986196;
			}
			else {
				var82 = -0.04837371;
			}
		}
		else {
			if ((input[37]) >= (2.1666498)) {
				var82 = 0.025269;
			}
			else {
				var82 = -0.007847401;
			}
		}
	}
	double var83;
	if ((input[22]) >= (2.5)) {
		if ((input[52]) >= (1.7504499)) {
			if ((input[38]) >= (2.125)) {
				var83 = -0.00059647043;
			}
			else {
				var83 = 0.0218732;
			}
		}
		else {
			if ((input[12]) >= (2.6992002)) {
				var83 = -0.06703014;
			}
			else {
				var83 = -0.011091199;
			}
		}
	}
	else {
		if ((input[50]) >= (39.13865)) {
			if ((input[12]) >= (4.65625)) {
				var83 = 0.039493375;
			}
			else {
				var83 = 0.002107956;
			}
		}
		else {
			if ((input[54]) >= (42.5)) {
				var83 = -0.019249931;
			}
			else {
				var83 = 0.00004453434;
			}
		}
	}
	double var84;
	if ((input[61]) >= (106243.0)) {
		if ((input[54]) >= (47.5)) {
			if ((input[38]) >= (1.875)) {
				var84 = 0.033558678;
			}
			else {
				var84 = -0.060629107;
			}
		}
		else {
			if ((input[1]) >= (0.9766)) {
				var84 = 0.05419848;
			}
			else {
				var84 = -0.03488373;
			}
		}
	}
	else {
		if ((input[54]) >= (45.5)) {
			if ((input[0]) >= (6.0)) {
				var84 = -0.008965633;
			}
			else {
				var84 = -0.055790883;
			}
		}
		else {
			if ((input[60]) >= (0.5)) {
				var84 = -0.0021013925;
			}
			else {
				var84 = 0.0137363775;
			}
		}
	}
	double var85;
	if ((input[0]) >= (6.0)) {
		if ((input[56]) >= (95.5)) {
			if ((input[1]) >= (8.61525)) {
				var85 = -0.0042358954;
			}
			else {
				var85 = 0.02798715;
			}
		}
		else {
			if ((input[23]) >= (1.45)) {
				var85 = -0.054401238;
			}
			else {
				var85 = 0.028086796;
			}
		}
	}
	else {
		if ((input[2]) >= (1.5)) {
			if ((input[56]) >= (553.5)) {
				var85 = -0.0020667082;
			}
			else {
				var85 = -0.036597557;
			}
		}
		else {
			if ((input[38]) >= (2.125)) {
				var85 = 0.071712345;
			}
			else {
				var85 = -0.006616641;
			}
		}
	}
	double var86;
	if ((input[45]) >= (154.89255)) {
		if ((input[1]) >= (11.832001)) {
			if ((input[47]) >= (2.1075)) {
				var86 = -0.037475407;
			}
			else {
				var86 = -0.10200341;
			}
		}
		else {
			if ((input[60]) >= (21.5)) {
				var86 = 0.053352248;
			}
			else {
				var86 = -0.02221084;
			}
		}
	}
	else {
		if ((input[37]) >= (1.61115)) {
			if ((input[37]) >= (1.94445)) {
				var86 = -0.0003768528;
			}
			else {
				var86 = 0.04701769;
			}
		}
		else {
			if ((input[61]) >= (2614.5)) {
				var86 = 0.0059596584;
			}
			else {
				var86 = -0.015990386;
			}
		}
	}
	double var87;
	if ((input[54]) >= (37.5)) {
		if ((input[56]) >= (3947.5)) {
			if ((input[38]) >= (2.125)) {
				var87 = -0.013344516;
			}
			else {
				var87 = 0.015234239;
			}
		}
		else {
			if ((input[56]) >= (138.5)) {
				var87 = -0.025915643;
			}
			else {
				var87 = 0.016777739;
			}
		}
	}
	else {
		if ((input[56]) >= (223.5)) {
			if ((input[60]) >= (12.5)) {
				var87 = -0.0008012538;
			}
			else {
				var87 = 0.02207089;
			}
		}
		else {
			if ((input[23]) >= (1.225)) {
				var87 = -0.025986198;
			}
			else {
				var87 = 0.02396551;
			}
		}
	}
	double var88;
	if ((input[49]) >= (0.30745)) {
		if ((input[36]) >= (5.39915)) {
			if ((input[0]) >= (6.0)) {
				var88 = 0.01903024;
			}
			else {
				var88 = -0.00060341175;
			}
		}
		else {
			if ((input[43]) >= (0.5)) {
				var88 = -0.018724915;
			}
			else {
				var88 = 0.0010539327;
			}
		}
	}
	else {
		if ((input[1]) >= (1.84575)) {
			if ((input[38]) >= (2.875)) {
				var88 = -0.02861323;
			}
			else {
				var88 = 0.0769955;
			}
		}
		else {
			if ((input[47]) >= (2.37555)) {
				var88 = -0.054326963;
			}
			else {
				var88 = 0.025978094;
			}
		}
	}
	double var89;
	if ((input[49]) >= (0.91045)) {
		if ((input[60]) >= (262.5)) {
			if ((input[48]) >= (279.43115)) {
				var89 = 0.072499804;
			}
			else {
				var89 = -0.11592243;
			}
		}
		else {
			if ((input[61]) >= (132811.5)) {
				var89 = 0.048453707;
			}
			else {
				var89 = -0.0038225825;
			}
		}
	}
	else {
		if ((input[47]) >= (1.023)) {
			if ((input[61]) >= (3197.5)) {
				var89 = 0.024654938;
			}
			else {
				var89 = 0.0032838713;
			}
		}
		else {
			if ((input[36]) >= (5.52935)) {
				var89 = 0.11147596;
			}
			else {
				var89 = 0.024549795;
			}
		}
	}
	double var90;
	if ((input[54]) >= (45.5)) {
		if ((input[56]) >= (39541.5)) {
			if ((input[0]) >= (6.0)) {
				var90 = 0.038505267;
			}
			else {
				var90 = -0.018544106;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var90 = -0.03318589;
			}
			else {
				var90 = -0.08685898;
			}
		}
	}
	else {
		if ((input[61]) >= (25887.5)) {
			if ((input[54]) >= (40.5)) {
				var90 = 0.012635158;
			}
			else {
				var90 = 0.05176223;
			}
		}
		else {
			if ((input[54]) >= (37.5)) {
				var90 = -0.0066548497;
			}
			else {
				var90 = 0.005965918;
			}
		}
	}
	double var91;
	if ((input[62]) >= (56.5)) {
		if ((input[47]) >= (0.15905)) {
			if ((input[54]) >= (37.5)) {
				var91 = -0.011163139;
			}
			else {
				var91 = 0.005669486;
			}
		}
		else {
			if ((input[37]) >= (2.72225)) {
				var91 = 0.03217388;
			}
			else {
				var91 = -0.08334341;
			}
		}
	}
	else {
		if ((input[56]) >= (3.5)) {
			if ((input[47]) >= (0.13525)) {
				var91 = 0.0040647606;
			}
			else {
				var91 = -0.05016922;
			}
		}
		else {
			if ((input[37]) >= (0.94445)) {
				var91 = 0.0391751;
			}
			else {
				var91 = 0.25977907;
			}
		}
	}
	double var92;
	if ((input[36]) >= (6.6372004)) {
		if ((input[47]) >= (0.57015)) {
			if ((input[45]) >= (94.8926)) {
				var92 = 0.08004141;
			}
			else {
				var92 = 0.029146645;
			}
		}
		else {
			if ((input[61]) >= (261.5)) {
				var92 = -0.11452838;
			}
			else {
				var92 = 0.014480668;
			}
		}
	}
	else {
		if ((input[37]) >= (2.1666498)) {
			if ((input[60]) >= (0.5)) {
				var92 = -0.00029107276;
			}
			else {
				var92 = 0.053759683;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var92 = 0.0089805955;
			}
			else {
				var92 = -0.00884831;
			}
		}
	}
	double var93;
	if ((input[50]) >= (45.24415)) {
		if ((input[18]) >= (1.5)) {
			if ((input[56]) >= (15215.5)) {
				var93 = 0.027471552;
			}
			else {
				var93 = -0.036261972;
			}
		}
		else {
			if ((input[62]) >= (94.5)) {
				var93 = -0.09956272;
			}
			else {
				var93 = 0.055735398;
			}
		}
	}
	else {
		if ((input[1]) >= (32.79885)) {
			if ((input[50]) >= (13.804701)) {
				var93 = -0.0042578364;
			}
			else {
				var93 = -0.043290876;
			}
		}
		else {
			if ((input[13]) >= (1.5)) {
				var93 = -0.0022587895;
			}
			else {
				var93 = 0.012928962;
			}
		}
	}
	double var94;
	if ((input[56]) >= (105709.5)) {
		if ((input[54]) >= (47.5)) {
			if ((input[60]) >= (5.5)) {
				var94 = -0.09009992;
			}
			else {
				var94 = 0.0036136997;
			}
		}
		else {
			if ((input[45]) >= (136.14255)) {
				var94 = -0.05300377;
			}
			else {
				var94 = 0.0469454;
			}
		}
	}
	else {
		if ((input[54]) >= (45.5)) {
			if ((input[50]) >= (20.148449)) {
				var94 = -0.00984729;
			}
			else {
				var94 = -0.054452326;
			}
		}
		else {
			if ((input[61]) >= (4033.5)) {
				var94 = 0.0075244145;
			}
			else {
				var94 = -0.003388834;
			}
		}
	}
	double var95;
	if ((input[54]) >= (42.5)) {
		if ((input[56]) >= (10182.5)) {
			if ((input[38]) >= (2.125)) {
				var95 = -0.02208955;
			}
			else {
				var95 = 0.011541142;
			}
		}
		else {
			if ((input[0]) >= (1.5)) {
				var95 = -0.014344758;
			}
			else {
				var95 = -0.0725137;
			}
		}
	}
	else {
		if ((input[47]) >= (3.4214501)) {
			if ((input[56]) >= (18588.0)) {
				var95 = 0.04670416;
			}
			else {
				var95 = 0.016043132;
			}
		}
		else {
			if ((input[1]) >= (13.251949)) {
				var95 = -0.016357813;
			}
			else {
				var95 = 0.0037729272;
			}
		}
	}
	double var96;
	if ((input[49]) >= (0.30745)) {
		if ((input[36]) >= (5.95525)) {
			if ((input[47]) >= (0.1435)) {
				var96 = 0.008789231;
			}
			else {
				var96 = -0.092237234;
			}
		}
		else {
			if ((input[44]) >= (2.5)) {
				var96 = 0.003136312;
			}
			else {
				var96 = -0.009774345;
			}
		}
	}
	else {
		if ((input[12]) >= (0.087850004)) {
			if ((input[45]) >= (72.14455)) {
				var96 = 0.07508999;
			}
			else {
				var96 = 0.032523233;
			}
		}
		else {
			if ((input[16]) >= (9.3)) {
				var96 = -0.1722387;
			}
			else {
				var96 = -0.022122573;
			}
		}
	}
	double var97;
	if ((input[0]) >= (6.0)) {
		if ((input[50]) >= (1.6543)) {
			if ((input[1]) >= (0.02145)) {
				var97 = 0.021546355;
			}
			else {
				var97 = -0.041804586;
			}
		}
		else {
			if ((input[1]) >= (2.63865)) {
				var97 = -0.05989147;
			}
			else {
				var97 = 0.0048366166;
			}
		}
	}
	else {
		if ((input[13]) >= (1.5)) {
			if ((input[56]) >= (930.5)) {
				var97 = -0.0007323235;
			}
			else {
				var97 = -0.02629388;
			}
		}
		else {
			if ((input[38]) >= (2.125)) {
				var97 = 0.07069868;
			}
			else {
				var97 = -0.0065046647;
			}
		}
	}
	double var98;
	if ((input[54]) >= (41.5)) {
		if ((input[56]) >= (3934.5)) {
			if ((input[38]) >= (2.125)) {
				var98 = -0.02143313;
			}
			else {
				var98 = 0.011558724;
			}
		}
		else {
			if ((input[38]) >= (2.125)) {
				var98 = 0.02407672;
			}
			else {
				var98 = -0.054387927;
			}
		}
	}
	else {
		if ((input[48]) >= (1574.8168)) {
			if ((input[54]) >= (37.5)) {
				var98 = 0.022765083;
			}
			else {
				var98 = 0.07036157;
			}
		}
		else {
			if ((input[1]) >= (13.70895)) {
				var98 = -0.016916929;
			}
			else {
				var98 = 0.004414932;
			}
		}
	}
	double var99;
	if ((input[22]) >= (2.5)) {
		if ((input[57]) >= (0.5)) {
			if ((input[38]) >= (2.125)) {
				var99 = -0.00069949793;
			}
			else {
				var99 = 0.029313892;
			}
		}
		else {
			if ((input[1]) >= (1.00195)) {
				var99 = -0.0077215554;
			}
			else {
				var99 = 0.053098317;
			}
		}
	}
	else {
		if ((input[24]) >= (8.0)) {
			if ((input[50]) >= (2.29105)) {
				var99 = -0.008875117;
			}
			else {
				var99 = -0.06770607;
			}
		}
		else {
			if ((input[54]) >= (37.5)) {
				var99 = -0.006223816;
			}
			else {
				var99 = 0.009547319;
			}
		}
	}
	double var100;
	var100 = (1.0) / ((1.0) + (exp((0.0) - ((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((var0)+(var1)) + (var2)) + (var3)) + (var4)) + (var5)) + (var6)) + (var7)) + (var8)) + (var9)) + (var10)) + (var11)) + (var12)) + (var13)) + (var14)) + (var15)) + (var16)) + (var17)) + (var18)) + (var19)) + (var20)) + (var21)) + (var22)) + (var23)) + (var24)) + (var25)) + (var26)) + (var27)) + (var28)) + (var29)) + (var30)) + (var31)) + (var32)) + (var33)) + (var34)) + (var35)) + (var36)) + (var37)) + (var38)) + (var39)) + (var40)) + (var41)) + (var42)) + (var43)) + (var44)) + (var45)) + (var46)) + (var47)) + (var48)) + (var49)) + (var50)) + (var51)) + (var52)) + (var53)) + (var54)) + (var55)) + (var56)) + (var57)) + (var58)) + (var59)) + (var60)) + (var61)) + (var62)) + (var63)) + (var64)) + (var65)) + (var66)) + (var67)) + (var68)) + (var69)) + (var70)) + (var71)) + (var72)) + (var73)) + (var74)) + (var75)) + (var76)) + (var77)) + (var78)) + (var79)) + (var80)) + (var81)) + (var82)) + (var83)) + (var84)) + (var85)) + (var86)) + (var87)) + (var88)) + (var89)) + (var90)) + (var91)) + (var92)) + (var93)) + (var94)) + (var95)) + (var96)) + (var97)) + (var98)) + (var99)))));
	*output = var100 < 0.5 ? 0 : 1;
}

void CUclf5(double * input, double * output) {
	double var0;
	if ((input[24]) >= (0.1)) {
		if ((input[13]) >= (0.5)) {
			if ((input[2]) >= (0.5)) {
				var0 = 0.18623982;
			}
			else {
				var0 = 0.09018472;
			}
		}
		else {
			if ((input[70]) >= (1.5)) {
				var0 = 0.13893993;
			}
			else {
				var0 = -0.043782838;
			}
		}
	}
	else {
		if ((input[48]) >= (0.125)) {
			if ((input[47]) >= (0.07575)) {
				var0 = 0.12434506;
			}
			else {
				var0 = -0.008247423;
			}
		}
		else {
			if ((input[70]) >= (2.5)) {
				var0 = 0.038131226;
			}
			else {
				var0 = -0.18970864;
			}
		}
	}
	double var1;
	if ((input[24]) >= (0.1)) {
		if ((input[14]) >= (0.29165)) {
			if ((input[66]) >= (623.5)) {
				var1 = 0.16499676;
			}
			else {
				var1 = 0.054594178;
			}
		}
		else {
			if ((input[66]) >= (3430.0)) {
				var1 = 0.099204324;
			}
			else {
				var1 = -0.07905846;
			}
		}
	}
	else {
		if ((input[48]) >= (0.125)) {
			if ((input[47]) >= (0.07575)) {
				var1 = 0.11237512;
			}
			else {
				var1 = -0.0074252407;
			}
		}
		else {
			if ((input[70]) >= (2.5)) {
				var1 = 0.03433628;
			}
			else {
				var1 = -0.1723371;
			}
		}
	}
	double var2;
	if ((input[24]) >= (0.1)) {
		if ((input[2]) >= (0.5)) {
			if ((input[13]) >= (0.5)) {
				var2 = 0.1566455;
			}
			else {
				var2 = 0.075933225;
			}
		}
		else {
			if ((input[70]) >= (2.5)) {
				var2 = 0.123217426;
			}
			else {
				var2 = -0.044659328;
			}
		}
	}
	else {
		if ((input[48]) >= (0.125)) {
			if ((input[47]) >= (0.28785)) {
				var2 = 0.10234252;
			}
			else {
				var2 = -0.0064665684;
			}
		}
		else {
			if ((input[70]) >= (1.5)) {
				var2 = 0.02202858;
			}
			else {
				var2 = -0.15965502;
			}
		}
	}
	double var3;
	if ((input[24]) >= (0.1)) {
		if ((input[3]) >= (0.45)) {
			if ((input[71]) >= (122.5)) {
				var3 = 0.14179157;
			}
			else {
				var3 = -0.029359741;
			}
		}
		else {
			if ((input[32]) >= (0.5)) {
				var3 = 0.09239463;
			}
			else {
				var3 = -0.062160935;
			}
		}
	}
	else {
		if ((input[70]) >= (1.5)) {
			if ((input[61]) >= (0.00985)) {
				var3 = 0.07778638;
			}
			else {
				var3 = -0.18177976;
			}
		}
		else {
			if ((input[47]) >= (0.045450002)) {
				var3 = 0.0774167;
			}
			else {
				var3 = -0.14794524;
			}
		}
	}
	double var4;
	if ((input[33]) >= (0.225)) {
		if ((input[71]) >= (172.5)) {
			if ((input[14]) >= (0.45)) {
				var4 = 0.13678037;
			}
			else {
				var4 = 0.061337568;
			}
		}
		else {
			if ((input[47]) >= (0.045450002)) {
				var4 = 0.03125305;
			}
			else {
				var4 = -0.123381115;
			}
		}
	}
	else {
		if ((input[48]) >= (0.125)) {
			if ((input[66]) >= (3343.0)) {
				var4 = 0.11914744;
			}
			else {
				var4 = 0.0064926455;
			}
		}
		else {
			if ((input[70]) >= (2.5)) {
				var4 = 0.02132528;
			}
			else {
				var4 = -0.14345956;
			}
		}
	}
	double var5;
	if ((input[14]) >= (0.36665)) {
		if ((input[3]) >= (0.225)) {
			if ((input[66]) >= (2088.5)) {
				var5 = 0.13307151;
			}
			else {
				var5 = 0.067405626;
			}
		}
		else {
			if ((input[47]) >= (0.07575)) {
				var5 = 0.077669315;
			}
			else {
				var5 = -0.09339633;
			}
		}
	}
	else {
		if ((input[47]) >= (0.045450002)) {
			if ((input[47]) >= (0.56065)) {
				var5 = 0.13057405;
			}
			else {
				var5 = 0.04098709;
			}
		}
		else {
			if ((input[70]) >= (3.5)) {
				var5 = 0.017047068;
			}
			else {
				var5 = -0.13641022;
			}
		}
	}
	double var6;
	if ((input[33]) >= (0.225)) {
		if ((input[71]) >= (149.5)) {
			if ((input[24]) >= (0.45)) {
				var6 = 0.12778899;
			}
			else {
				var6 = 0.06564908;
			}
		}
		else {
			if ((input[47]) >= (0.045450002)) {
				var6 = 0.015371041;
			}
			else {
				var6 = -0.116505556;
			}
		}
	}
	else {
		if ((input[47]) >= (0.045450002)) {
			if ((input[47]) >= (0.80305)) {
				var6 = 0.12821876;
			}
			else {
				var6 = 0.051957138;
			}
		}
		else {
			if ((input[13]) >= (0.5)) {
				var6 = -0.011026212;
			}
			else {
				var6 = -0.13250016;
			}
		}
	}
	double var7;
	if ((input[3]) >= (0.225)) {
		if ((input[13]) >= (0.5)) {
			if ((input[66]) >= (2085.5)) {
				var7 = 0.123617016;
			}
			else {
				var7 = 0.075557716;
			}
		}
		else {
			if ((input[32]) >= (0.5)) {
				var7 = 0.08348327;
			}
			else {
				var7 = -0.06460497;
			}
		}
	}
	else {
		if ((input[47]) >= (0.045450002)) {
			if ((input[47]) >= (0.95455)) {
				var7 = 0.13234483;
			}
			else {
				var7 = 0.04364049;
			}
		}
		else {
			if ((input[70]) >= (2.5)) {
				var7 = 0.0036328253;
			}
			else {
				var7 = -0.12699373;
			}
		}
	}
	double var8;
	if ((input[33]) >= (0.225)) {
		if ((input[71]) >= (262.5)) {
			if ((input[48]) >= (0.125)) {
				var8 = 0.11601756;
			}
			else {
				var8 = 0.0524864;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var8 = -0.11210276;
			}
			else {
				var8 = 0.051695265;
			}
		}
	}
	else {
		if ((input[2]) >= (0.5)) {
			if ((input[66]) >= (2848.5)) {
				var8 = 0.078425825;
			}
			else {
				var8 = -0.0732524;
			}
		}
		else {
			if ((input[47]) >= (0.28785)) {
				var8 = 0.04303436;
			}
			else {
				var8 = -0.122634105;
			}
		}
	}
	double var9;
	if ((input[14]) >= (0.225)) {
		if ((input[2]) >= (0.5)) {
			if ((input[66]) >= (3162.5)) {
				var9 = 0.11653848;
			}
			else {
				var9 = 0.07258588;
			}
		}
		else {
			if ((input[70]) >= (1.5)) {
				var9 = 0.071779795;
			}
			else {
				var9 = -0.060834285;
			}
		}
	}
	else {
		if ((input[47]) >= (0.28785)) {
			if ((input[47]) >= (0.80305)) {
				var9 = 0.119980216;
			}
			else {
				var9 = 0.030407256;
			}
		}
		else {
			if ((input[70]) >= (2.5)) {
				var9 = 0.007887567;
			}
			else {
				var9 = -0.11865981;
			}
		}
	}
	double var10;
	if ((input[33]) >= (0.225)) {
		if ((input[2]) >= (0.5)) {
			if ((input[24]) >= (0.70835)) {
				var10 = 0.11450131;
			}
			else {
				var10 = 0.076611094;
			}
		}
		else {
			if ((input[33]) >= (0.70835)) {
				var10 = 0.060882688;
			}
			else {
				var10 = -0.037406545;
			}
		}
	}
	else {
		if ((input[48]) >= (0.125)) {
			if ((input[56]) >= (30.2446)) {
				var10 = 0.07904164;
			}
			else {
				var10 = -0.02342954;
			}
		}
		else {
			if ((input[70]) >= (2.5)) {
				var10 = 0.019115446;
			}
			else {
				var10 = -0.113645904;
			}
		}
	}
	double var11;
	if ((input[24]) >= (0.225)) {
		if ((input[66]) >= (1053.5)) {
			if ((input[32]) >= (0.5)) {
				var11 = 0.110294715;
			}
			else {
				var11 = 0.06805371;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var11 = -0.089047536;
			}
			else {
				var11 = 0.09283074;
			}
		}
	}
	else {
		if ((input[70]) >= (1.5)) {
			if ((input[61]) >= (0.01385)) {
				var11 = 0.056642216;
			}
			else {
				var11 = -0.13361892;
			}
		}
		else {
			if ((input[47]) >= (0.28785)) {
				var11 = 0.033256188;
			}
			else {
				var11 = -0.10548425;
			}
		}
	}
	double var12;
	if ((input[48]) >= (0.125)) {
		if ((input[66]) >= (2487.5)) {
			if ((input[32]) >= (0.5)) {
				var12 = 0.11175225;
			}
			else {
				var12 = 0.07985367;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var12 = -0.012218879;
			}
			else {
				var12 = 0.097404875;
			}
		}
	}
	else {
		if ((input[13]) >= (0.5)) {
			if ((input[66]) >= (2785.5)) {
				var12 = 0.07345868;
			}
			else {
				var12 = -0.05676451;
			}
		}
		else {
			if ((input[70]) >= (4.5)) {
				var12 = 0.01177649;
			}
			else {
				var12 = -0.10810741;
			}
		}
	}
	double var13;
	if ((input[32]) >= (0.5)) {
		if ((input[71]) >= (348.5)) {
			if ((input[48]) >= (0.29165)) {
				var13 = 0.10795593;
			}
			else {
				var13 = 0.06780753;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var13 = -0.08572995;
			}
			else {
				var13 = 0.05190018;
			}
		}
	}
	else {
		if ((input[62]) >= (67.0662)) {
			if ((input[70]) >= (0.5)) {
				var13 = 0.071491145;
			}
			else {
				var13 = -0.043079767;
			}
		}
		else {
			if ((input[47]) >= (0.92425)) {
				var13 = 0.0926634;
			}
			else {
				var13 = -0.10509733;
			}
		}
	}
	double var14;
	if ((input[3]) >= (0.225)) {
		if ((input[13]) >= (0.5)) {
			if ((input[66]) >= (3445.5)) {
				var14 = 0.10148223;
			}
			else {
				var14 = 0.053153742;
			}
		}
		else {
			if ((input[70]) >= (3.5)) {
				var14 = 0.06425532;
			}
			else {
				var14 = -0.04113246;
			}
		}
	}
	else {
		if ((input[48]) >= (0.125)) {
			if ((input[47]) >= (0.98485)) {
				var14 = 0.11470198;
			}
			else {
				var14 = 0.004885693;
			}
		}
		else {
			if ((input[32]) >= (0.5)) {
				var14 = -0.0011310475;
			}
			else {
				var14 = -0.10503832;
			}
		}
	}
	double var15;
	if ((input[33]) >= (0.45)) {
		if ((input[71]) >= (344.5)) {
			if ((input[48]) >= (0.41665)) {
				var15 = 0.101065;
			}
			else {
				var15 = 0.058881965;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var15 = -0.08776914;
			}
			else {
				var15 = 0.042551044;
			}
		}
	}
	else {
		if ((input[62]) >= (54.33025)) {
			if ((input[64]) >= (47.5)) {
				var15 = -0.062265694;
			}
			else {
				var15 = 0.049903676;
			}
		}
		else {
			if ((input[47]) >= (0.98485)) {
				var15 = 0.102936916;
			}
			else {
				var15 = -0.10111604;
			}
		}
	}
	double var16;
	if ((input[14]) >= (0.225)) {
		if ((input[2]) >= (0.5)) {
			if ((input[66]) >= (3162.5)) {
				var16 = 0.09581778;
			}
			else {
				var16 = 0.045886785;
			}
		}
		else {
			if ((input[70]) >= (5.5)) {
				var16 = 0.07052875;
			}
			else {
				var16 = -0.035017192;
			}
		}
	}
	else {
		if ((input[70]) >= (1.5)) {
			if ((input[56]) >= (2.04515)) {
				var16 = 0.044668864;
			}
			else {
				var16 = -0.10334637;
			}
		}
		else {
			if ((input[47]) >= (0.86364996)) {
				var16 = 0.11340667;
			}
			else {
				var16 = -0.09669073;
			}
		}
	}
	double var17;
	if ((input[32]) >= (0.5)) {
		if ((input[24]) >= (0.45)) {
			if ((input[71]) >= (3346.5)) {
				var17 = 0.10320014;
			}
			else {
				var17 = 0.07100143;
			}
		}
		else {
			if ((input[71]) >= (127.5)) {
				var17 = 0.044858236;
			}
			else {
				var17 = -0.06491846;
			}
		}
	}
	else {
		if ((input[62]) >= (71.269455)) {
			if ((input[64]) >= (48.5)) {
				var17 = -0.07191329;
			}
			else {
				var17 = 0.045941114;
			}
		}
		else {
			if ((input[47]) >= (0.28785)) {
				var17 = 0.015937978;
			}
			else {
				var17 = -0.099265166;
			}
		}
	}
	double var18;
	if ((input[48]) >= (0.125)) {
		if ((input[66]) >= (3568.5)) {
			if ((input[64]) >= (45.5)) {
				var18 = 0.060944237;
			}
			else {
				var18 = 0.10115888;
			}
		}
		else {
			if ((input[64]) >= (43.5)) {
				var18 = -0.13512826;
			}
			else {
				var18 = 0.049156487;
			}
		}
	}
	else {
		if ((input[71]) >= (3606.5)) {
			if ((input[64]) >= (46.5)) {
				var18 = -0.047580626;
			}
			else {
				var18 = 0.072044656;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var18 = -0.098344356;
			}
			else {
				var18 = 0.068969816;
			}
		}
	}
	double var19;
	if ((input[3]) >= (0.1)) {
		if ((input[66]) >= (2471.5)) {
			if ((input[64]) >= (45.5)) {
				var19 = 0.02514246;
			}
			else {
				var19 = 0.09068231;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var19 = -0.056832742;
			}
			else {
				var19 = 0.07842199;
			}
		}
	}
	else {
		if ((input[48]) >= (0.125)) {
			if ((input[48]) >= (0.875)) {
				var19 = 0.10475234;
			}
			else {
				var19 = 0.000033986114;
			}
		}
		else {
			if ((input[70]) >= (6.5)) {
				var19 = 0.025641417;
			}
			else {
				var19 = -0.0974324;
			}
		}
	}
	double var20;
	if ((input[32]) >= (0.5)) {
		if ((input[24]) >= (0.70835)) {
			if ((input[66]) >= (723.0)) {
				var20 = 0.09449217;
			}
			else {
				var20 = 0.043410122;
			}
		}
		else {
			if ((input[66]) >= (8045.5)) {
				var20 = 0.077314004;
			}
			else {
				var20 = 0.01163818;
			}
		}
	}
	else {
		if ((input[70]) >= (1.5)) {
			if ((input[61]) >= (0.0058500003)) {
				var20 = 0.047070313;
			}
			else {
				var20 = -0.13647057;
			}
		}
		else {
			if ((input[48]) >= (0.58335)) {
				var20 = 0.054021098;
			}
			else {
				var20 = -0.08401552;
			}
		}
	}
	double var21;
	if ((input[24]) >= (0.225)) {
		if ((input[66]) >= (1290.5)) {
			if ((input[64]) >= (43.5)) {
				var21 = 0.048201714;
			}
			else {
				var21 = 0.08982289;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var21 = -0.06117331;
			}
			else {
				var21 = 0.066604346;
			}
		}
	}
	else {
		if ((input[3]) >= (0.1)) {
			if ((input[66]) >= (16422.5)) {
				var21 = 0.07474021;
			}
			else {
				var21 = -0.021100868;
			}
		}
		else {
			if ((input[47]) >= (0.045450002)) {
				var21 = 0.035785392;
			}
			else {
				var21 = -0.08905007;
			}
		}
	}
	double var22;
	if ((input[14]) >= (0.1)) {
		if ((input[66]) >= (4565.5)) {
			if ((input[64]) >= (45.5)) {
				var22 = 0.03015661;
			}
			else {
				var22 = 0.09329719;
			}
		}
		else {
			if ((input[64]) >= (43.5)) {
				var22 = -0.12510851;
			}
			else {
				var22 = 0.025305241;
			}
		}
	}
	else {
		if ((input[48]) >= (0.125)) {
			if ((input[48]) >= (0.875)) {
				var22 = 0.103389494;
			}
			else {
				var22 = -0.0001861503;
			}
		}
		else {
			if ((input[49]) >= (0.125)) {
				var22 = 0.024640994;
			}
			else {
				var22 = -0.093017615;
			}
		}
	}
	double var23;
	if ((input[32]) >= (0.5)) {
		if ((input[48]) >= (0.58335)) {
			if ((input[66]) >= (592.5)) {
				var23 = 0.0939348;
			}
			else {
				var23 = 0.04178987;
			}
		}
		else {
			if ((input[66]) >= (5369.5)) {
				var23 = 0.069352746;
			}
			else {
				var23 = -0.0023892291;
			}
		}
	}
	else {
		if ((input[62]) >= (97.3357)) {
			if ((input[28]) >= (4.5)) {
				var23 = -0.029194564;
			}
			else {
				var23 = 0.054490503;
			}
		}
		else {
			if ((input[64]) >= (43.5)) {
				var23 = -0.10327864;
			}
			else {
				var23 = -0.026981642;
			}
		}
	}
	double var24;
	if ((input[48]) >= (0.125)) {
		if ((input[71]) >= (3421.5)) {
			if ((input[64]) >= (45.5)) {
				var24 = 0.048530336;
			}
			else {
				var24 = 0.09237029;
			}
		}
		else {
			if ((input[47]) >= (0.86364996)) {
				var24 = 0.07956233;
			}
			else {
				var24 = -0.0052629802;
			}
		}
	}
	else {
		if ((input[71]) >= (2305.5)) {
			if ((input[64]) >= (45.5)) {
				var24 = -0.03360176;
			}
			else {
				var24 = 0.06846073;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var24 = -0.09277504;
			}
			else {
				var24 = 0.06008136;
			}
		}
	}
	double var25;
	if ((input[70]) >= (1.5)) {
		if ((input[56]) >= (0.3409)) {
			if ((input[70]) >= (19.5)) {
				var25 = 0.10158302;
			}
			else {
				var25 = 0.046352476;
			}
		}
		else {
			if ((input[60]) >= (0.1102)) {
				var25 = -0.05824263;
			}
			else {
				var25 = -0.14695571;
			}
		}
	}
	else {
		if ((input[71]) >= (11070.5)) {
			if ((input[64]) >= (48.5)) {
				var25 = -0.024884002;
			}
			else {
				var25 = 0.08645951;
			}
		}
		else {
			if ((input[64]) >= (43.5)) {
				var25 = -0.10378601;
			}
			else {
				var25 = -0.010033092;
			}
		}
	}
	double var26;
	if ((input[48]) >= (0.29165)) {
		if ((input[24]) >= (0.9)) {
			if ((input[66]) >= (622.5)) {
				var26 = 0.08967482;
			}
			else {
				var26 = 0.03262741;
			}
		}
		else {
			if ((input[57]) >= (0.08595)) {
				var26 = 0.04185311;
			}
			else {
				var26 = -0.14808597;
			}
		}
	}
	else {
		if ((input[66]) >= (1822.5)) {
			if ((input[64]) >= (43.5)) {
				var26 = -0.02680998;
			}
			else {
				var26 = 0.07274725;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var26 = -0.09274545;
			}
			else {
				var26 = 0.04199528;
			}
		}
	}
	double var27;
	if ((input[32]) >= (0.5)) {
		if ((input[24]) >= (0.9)) {
			if ((input[71]) >= (564.5)) {
				var27 = 0.08786785;
			}
			else {
				var27 = 0.033183973;
			}
		}
		else {
			if ((input[66]) >= (17849.5)) {
				var27 = 0.080613516;
			}
			else {
				var27 = 0.012842933;
			}
		}
	}
	else {
		if ((input[62]) >= (52.8204)) {
			if ((input[51]) >= (3.5)) {
				var27 = -0.026441768;
			}
			else {
				var27 = 0.046882514;
			}
		}
		else {
			if ((input[64]) >= (43.5)) {
				var27 = -0.09980359;
			}
			else {
				var27 = -0.026105214;
			}
		}
	}
	double var28;
	if ((input[70]) >= (0.5)) {
		if ((input[71]) >= (573.5)) {
			if ((input[64]) >= (40.5)) {
				var28 = 0.032550063;
			}
			else {
				var28 = 0.083462365;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var28 = -0.07840511;
			}
			else {
				var28 = 0.03215803;
			}
		}
	}
	else {
		if ((input[66]) >= (18931.5)) {
			if ((input[64]) >= (49.5)) {
				var28 = -0.041476548;
			}
			else {
				var28 = 0.08315744;
			}
		}
		else {
			if ((input[47]) >= (0.28785)) {
				var28 = 0.02304811;
			}
			else {
				var28 = -0.08630149;
			}
		}
	}
	double var29;
	if ((input[48]) >= (0.41665)) {
		if ((input[66]) >= (8314.0)) {
			if ((input[64]) >= (45.5)) {
				var29 = 0.060034852;
			}
			else {
				var29 = 0.09718355;
			}
		}
		else {
			if ((input[64]) >= (43.5)) {
				var29 = -0.07446518;
			}
			else {
				var29 = 0.04746553;
			}
		}
	}
	else {
		if ((input[71]) >= (5165.5)) {
			if ((input[64]) >= (48.5)) {
				var29 = -0.04255512;
			}
			else {
				var29 = 0.056045424;
			}
		}
		else {
			if ((input[64]) >= (43.5)) {
				var29 = -0.103768244;
			}
			else {
				var29 = -0.014168533;
			}
		}
	}
	double var30;
	if ((input[33]) >= (0.70835)) {
		if ((input[71]) >= (319.5)) {
			if ((input[24]) >= (1.1)) {
				var30 = 0.0873173;
			}
			else {
				var30 = 0.042388074;
			}
		}
		else {
			if ((input[56]) >= (1.0803499)) {
				var30 = 0.005611393;
			}
			else {
				var30 = -0.09943724;
			}
		}
	}
	else {
		if ((input[66]) >= (16464.0)) {
			if ((input[64]) >= (48.5)) {
				var30 = -0.011189664;
			}
			else {
				var30 = 0.08344904;
			}
		}
		else {
			if ((input[64]) >= (43.5)) {
				var30 = -0.09187227;
			}
			else {
				var30 = -0.006275358;
			}
		}
	}
	double var31;
	if ((input[70]) >= (3.5)) {
		if ((input[56]) >= (0.32235)) {
			if ((input[70]) >= (19.5)) {
				var31 = 0.095432244;
			}
			else {
				var31 = 0.037212864;
			}
		}
		else {
			if ((input[55]) >= (39.142197)) {
				var31 = -0.14419435;
			}
			else {
				var31 = -0.057754178;
			}
		}
	}
	else {
		if ((input[71]) >= (8850.0)) {
			if ((input[64]) >= (48.5)) {
				var31 = -0.026669992;
			}
			else {
				var31 = 0.06707984;
			}
		}
		else {
			if ((input[64]) >= (43.5)) {
				var31 = -0.09832179;
			}
			else {
				var31 = -0.0083603775;
			}
		}
	}
	double var32;
	if ((input[48]) >= (0.41665)) {
		if ((input[47]) >= (0.98485)) {
			if ((input[57]) >= (2.72535)) {
				var32 = 0.09403;
			}
			else {
				var32 = 0.06368912;
			}
		}
		else {
			if ((input[71]) >= (3331.5)) {
				var32 = 0.06774873;
			}
			else {
				var32 = -0.003327567;
			}
		}
	}
	else {
		if ((input[66]) >= (1318.5)) {
			if ((input[64]) >= (43.5)) {
				var32 = -0.026491424;
			}
			else {
				var32 = 0.05574307;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var32 = -0.09045579;
			}
			else {
				var32 = 0.03119784;
			}
		}
	}
	double var33;
	if ((input[1]) >= (0.037699997)) {
		if ((input[71]) >= (18807.5)) {
			if ((input[64]) >= (49.5)) {
				var33 = -0.022392297;
			}
			else {
				var33 = 0.08579465;
			}
		}
		else {
			if ((input[64]) >= (45.5)) {
				var33 = -0.08627641;
			}
			else {
				var33 = 0.03113444;
			}
		}
	}
	else {
		if ((input[64]) >= (38.5)) {
			if ((input[47]) >= (0.98485)) {
				var33 = 0.08914017;
			}
			else {
				var33 = -0.07869623;
			}
		}
		else {
			if ((input[71]) >= (126.5)) {
				var33 = 0.05893114;
			}
			else {
				var33 = -0.08543317;
			}
		}
	}
	double var34;
	if ((input[12]) >= (0.01525)) {
		if ((input[64]) >= (40.5)) {
			if ((input[66]) >= (5380.0)) {
				var34 = 0.038863968;
			}
			else {
				var34 = -0.053009655;
			}
		}
		else {
			if ((input[66]) >= (659.5)) {
				var34 = 0.08162912;
			}
			else {
				var34 = -0.0021252243;
			}
		}
	}
	else {
		if ((input[0]) >= (6.0)) {
			if ((input[71]) >= (268.5)) {
				var34 = 0.044520397;
			}
			else {
				var34 = -0.050114304;
			}
		}
		else {
			if ((input[47]) >= (0.28785)) {
				var34 = 0.041319538;
			}
			else {
				var34 = -0.09428189;
			}
		}
	}
	double var35;
	if ((input[70]) >= (4.5)) {
		if ((input[56]) >= (0.32235)) {
			if ((input[70]) >= (19.5)) {
				var35 = 0.090135984;
			}
			else {
				var35 = 0.03294327;
			}
		}
		else {
			if ((input[55]) >= (39.142197)) {
				var35 = -0.13990186;
			}
			else {
				var35 = -0.0514169;
			}
		}
	}
	else {
		if ((input[71]) >= (3118.5)) {
			if ((input[64]) >= (46.5)) {
				var35 = -0.038452256;
			}
			else {
				var35 = 0.048730545;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var35 = -0.06405443;
			}
			else {
				var35 = 0.039734274;
			}
		}
	}
	double var36;
	if ((input[48]) >= (0.70835)) {
		if ((input[47]) >= (0.98485)) {
			if ((input[57]) >= (2.72535)) {
				var36 = 0.091289766;
			}
			else {
				var36 = 0.056236412;
			}
		}
		else {
			if ((input[71]) >= (1647.5)) {
				var36 = 0.061409533;
			}
			else {
				var36 = -0.011794213;
			}
		}
	}
	else {
		if ((input[66]) >= (1317.5)) {
			if ((input[64]) >= (43.5)) {
				var36 = -0.022159519;
			}
			else {
				var36 = 0.049767125;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var36 = -0.0802119;
			}
			else {
				var36 = 0.02473295;
			}
		}
	}
	double var37;
	if ((input[66]) >= (16916.0)) {
		if ((input[64]) >= (49.5)) {
			if ((input[71]) >= (65956.0)) {
				var37 = 0.024700904;
			}
			else {
				var37 = -0.1079852;
			}
		}
		else {
			if ((input[66]) >= (34302.0)) {
				var37 = 0.086357065;
			}
			else {
				var37 = 0.0454881;
			}
		}
	}
	else {
		if ((input[64]) >= (43.5)) {
			if ((input[64]) >= (46.5)) {
				var37 = -0.106958106;
			}
			else {
				var37 = -0.051453557;
			}
		}
		else {
			if ((input[66]) >= (653.5)) {
				var37 = 0.040086012;
			}
			else {
				var37 = -0.02971083;
			}
		}
	}
	double var38;
	if ((input[71]) >= (12896.5)) {
		if ((input[64]) >= (48.5)) {
			if ((input[71]) >= (50578.5)) {
				var38 = 0.03980548;
			}
			else {
				var38 = -0.064184934;
			}
		}
		else {
			if ((input[66]) >= (37538.5)) {
				var38 = 0.092621006;
			}
			else {
				var38 = 0.052874077;
			}
		}
	}
	else {
		if ((input[64]) >= (43.5)) {
			if ((input[64]) >= (46.5)) {
				var38 = -0.103962414;
			}
			else {
				var38 = -0.049486674;
			}
		}
		else {
			if ((input[66]) >= (653.5)) {
				var38 = 0.03569597;
			}
			else {
				var38 = -0.02677743;
			}
		}
	}
	double var39;
	if ((input[37]) >= (0.5)) {
		if ((input[71]) >= (7917.0)) {
			if ((input[64]) >= (46.5)) {
				var39 = -0.01565138;
			}
			else {
				var39 = 0.060424663;
			}
		}
		else {
			if ((input[64]) >= (43.5)) {
				var39 = -0.0844247;
			}
			else {
				var39 = -0.006785056;
			}
		}
	}
	else {
		if ((input[71]) >= (141.5)) {
			if ((input[64]) >= (40.5)) {
				var39 = 0.040753096;
			}
			else {
				var39 = 0.07465064;
			}
		}
		else {
			if ((input[58]) >= (11.558849)) {
				var39 = 0.06486319;
			}
			else {
				var39 = -0.066173635;
			}
		}
	}
	double var40;
	if ((input[47]) >= (0.80305)) {
		if ((input[63]) >= (0.99995)) {
			if ((input[60]) >= (2.9872)) {
				var40 = 0.054339927;
			}
			else {
				var40 = 0.003579265;
			}
		}
		else {
			if ((input[48]) >= (0.875)) {
				var40 = 0.080017015;
			}
			else {
				var40 = 0.046226963;
			}
		}
	}
	else {
		if ((input[71]) >= (2182.5)) {
			if ((input[64]) >= (45.5)) {
				var40 = -0.015176921;
			}
			else {
				var40 = 0.04700458;
			}
		}
		else {
			if ((input[64]) >= (40.5)) {
				var40 = -0.072739676;
			}
			else {
				var40 = 0.0011042603;
			}
		}
	}
	double var41;
	if ((input[70]) >= (7.5)) {
		if ((input[61]) >= (0.01155)) {
			if ((input[70]) >= (19.5)) {
				var41 = 0.084462166;
			}
			else {
				var41 = 0.040174026;
			}
		}
		else {
			if ((input[56]) >= (1.53525)) {
				var41 = 0.0053260447;
			}
			else {
				var41 = -0.10545526;
			}
		}
	}
	else {
		if ((input[71]) >= (22781.5)) {
			if ((input[64]) >= (49.5)) {
				var41 = -0.023531506;
			}
			else {
				var41 = 0.06607852;
			}
		}
		else {
			if ((input[64]) >= (45.5)) {
				var41 = -0.08365654;
			}
			else {
				var41 = -0.0033740923;
			}
		}
	}
	double var42;
	if ((input[12]) >= (0.1454)) {
		if ((input[71]) >= (22781.5)) {
			if ((input[64]) >= (48.5)) {
				var42 = 0.022270365;
			}
			else {
				var42 = 0.08424809;
			}
		}
		else {
			if ((input[64]) >= (46.5)) {
				var42 = -0.0872536;
			}
			else {
				var42 = 0.019582337;
			}
		}
	}
	else {
		if ((input[0]) >= (6.0)) {
			if ((input[71]) >= (267.5)) {
				var42 = 0.043789655;
			}
			else {
				var42 = -0.039355703;
			}
		}
		else {
			if ((input[47]) >= (0.28785)) {
				var42 = 0.02335812;
			}
			else {
				var42 = -0.07653489;
			}
		}
	}
	double var43;
	if ((input[32]) >= (1.5)) {
		if ((input[12]) >= (0.0167)) {
			if ((input[66]) >= (30503.0)) {
				var43 = 0.09580117;
			}
			else {
				var43 = 0.052221067;
			}
		}
		else {
			if ((input[62]) >= (0.3513)) {
				var43 = 0.014807293;
			}
			else {
				var43 = -0.13405862;
			}
		}
	}
	else {
		if ((input[71]) >= (2953.5)) {
			if ((input[64]) >= (45.5)) {
				var43 = -0.014068208;
			}
			else {
				var43 = 0.047042042;
			}
		}
		else {
			if ((input[64]) >= (40.5)) {
				var43 = -0.05788479;
			}
			else {
				var43 = 0.006459677;
			}
		}
	}
	double var44;
	if ((input[37]) >= (0.5)) {
		if ((input[66]) >= (17862.5)) {
			if ((input[64]) >= (48.5)) {
				var44 = -0.013843979;
			}
			else {
				var44 = 0.06188981;
			}
		}
		else {
			if ((input[64]) >= (45.5)) {
				var44 = -0.083343215;
			}
			else {
				var44 = -0.0068413313;
			}
		}
	}
	else {
		if ((input[71]) >= (141.5)) {
			if ((input[66]) >= (53971.5)) {
				var44 = 0.10455163;
			}
			else {
				var44 = 0.04485895;
			}
		}
		else {
			if ((input[58]) >= (9.9826)) {
				var44 = 0.041326072;
			}
			else {
				var44 = -0.06779645;
			}
		}
	}
	double var45;
	if ((input[47]) >= (0.95455)) {
		if ((input[63]) >= (0.99995)) {
			if ((input[64]) >= (40.5)) {
				var45 = -0.0004947501;
			}
			else {
				var45 = 0.04601696;
			}
		}
		else {
			if ((input[46]) >= (4.5112)) {
				var45 = 0.07000759;
			}
			else {
				var45 = -0.058321506;
			}
		}
	}
	else {
		if ((input[66]) >= (927.5)) {
			if ((input[64]) >= (40.5)) {
				var45 = -0.0054345736;
			}
			else {
				var45 = 0.052763898;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var45 = -0.07518585;
			}
			else {
				var45 = 0.014191537;
			}
		}
	}
	double var46;
	if ((input[70]) >= (21.5)) {
		if ((input[46]) >= (3.2178)) {
			if ((input[57]) >= (0.32880002)) {
				var46 = 0.08340591;
			}
			else {
				var46 = -0.020426644;
			}
		}
		else {
			if ((input[42]) >= (4.5)) {
				var46 = -0.1493045;
			}
			else {
				var46 = 0.03547732;
			}
		}
	}
	else {
		if ((input[71]) >= (8674.5)) {
			if ((input[64]) >= (45.5)) {
				var46 = 0.008538703;
			}
			else {
				var46 = 0.06749804;
			}
		}
		else {
			if ((input[64]) >= (44.5)) {
				var46 = -0.07358854;
			}
			else {
				var46 = -0.00063868746;
			}
		}
	}
	double var47;
	if ((input[47]) >= (0.95455)) {
		if ((input[57]) >= (2.69535)) {
			if ((input[48]) >= (0.875)) {
				var47 = 0.08401775;
			}
			else {
				var47 = 0.045596987;
			}
		}
		else {
			if ((input[64]) >= (40.5)) {
				var47 = 0.008760963;
			}
			else {
				var47 = 0.05242903;
			}
		}
	}
	else {
		if ((input[66]) >= (653.5)) {
			if ((input[0]) >= (6.0)) {
				var47 = 0.055011135;
			}
			else {
				var47 = -0.005583183;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var47 = -0.074653;
			}
			else {
				var47 = 0.003385643;
			}
		}
	}
	double var48;
	if ((input[1]) >= (0.0646)) {
		if ((input[70]) >= (27.5)) {
			if ((input[46]) >= (3.2178)) {
				var48 = 0.08167149;
			}
			else {
				var48 = -0.040624578;
			}
		}
		else {
			if ((input[71]) >= (32729.5)) {
				var48 = 0.058808036;
			}
			else {
				var48 = -0.000954156;
			}
		}
	}
	else {
		if ((input[47]) >= (0.28785)) {
			if ((input[33]) >= (0.1)) {
				var48 = -0.006201617;
			}
			else {
				var48 = 0.07468741;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var48 = 0.021858979;
			}
			else {
				var48 = -0.0777387;
			}
		}
	}
	double var49;
	if ((input[32]) >= (1.5)) {
		if ((input[12]) >= (0.0167)) {
			if ((input[66]) >= (30503.0)) {
				var49 = 0.090940766;
			}
			else {
				var49 = 0.04516191;
			}
		}
		else {
			if ((input[57]) >= (3.55035)) {
				var49 = 0.057505038;
			}
			else {
				var49 = -0.025979033;
			}
		}
	}
	else {
		if ((input[37]) >= (0.5)) {
			if ((input[71]) >= (2925.5)) {
				var49 = 0.0040618335;
			}
			else {
				var49 = -0.03405497;
			}
		}
		else {
			if ((input[62]) >= (0.065249994)) {
				var49 = 0.032537922;
			}
			else {
				var49 = -0.14044853;
			}
		}
	}
	double var50;
	if ((input[70]) >= (7.5)) {
		if ((input[61]) >= (0.01155)) {
			if ((input[70]) >= (26.5)) {
				var50 = 0.07706631;
			}
			else {
				var50 = 0.033401597;
			}
		}
		else {
			if ((input[71]) >= (1204.5)) {
				var50 = 0.024970055;
			}
			else {
				var50 = -0.094826385;
			}
		}
	}
	else {
		if ((input[47]) >= (0.98485)) {
			if ((input[47]) >= (1.0151501)) {
				var50 = 0.029894829;
			}
			else {
				var50 = 0.08216377;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var50 = 0.024368389;
			}
			else {
				var50 = -0.026858374;
			}
		}
	}
	double var51;
	if ((input[66]) >= (34304.5)) {
		if ((input[64]) >= (48.5)) {
			if ((input[12]) >= (1.58335)) {
				var51 = 0.042241026;
			}
			else {
				var51 = -0.04192578;
			}
		}
		else {
			if ((input[61]) >= (1.05365)) {
				var51 = -0.084042996;
			}
			else {
				var51 = 0.07806588;
			}
		}
	}
	else {
		if ((input[64]) >= (46.5)) {
			if ((input[1]) >= (3.93615)) {
				var51 = -0.033479568;
			}
			else {
				var51 = -0.096073695;
			}
		}
		else {
			if ((input[71]) >= (1104.5)) {
				var51 = 0.021329973;
			}
			else {
				var51 = -0.01686368;
			}
		}
	}
	double var52;
	if ((input[32]) >= (1.5)) {
		if ((input[12]) >= (0.0282)) {
			if ((input[24]) >= (1.29165)) {
				var52 = 0.08008697;
			}
			else {
				var52 = 0.039638013;
			}
		}
		else {
			if ((input[62]) >= (0.3435)) {
				var52 = 0.008081979;
			}
			else {
				var52 = -0.13235602;
			}
		}
	}
	else {
		if ((input[71]) >= (4569.5)) {
			if ((input[64]) >= (46.5)) {
				var52 = -0.019455891;
			}
			else {
				var52 = 0.03595578;
			}
		}
		else {
			if ((input[64]) >= (43.5)) {
				var52 = -0.07029853;
			}
			else {
				var52 = -0.0035921545;
			}
		}
	}
	double var53;
	if ((input[70]) >= (21.5)) {
		if ((input[46]) >= (3.2178)) {
			if ((input[56]) >= (1.61655)) {
				var53 = 0.0756889;
			}
			else {
				var53 = 0.0015237476;
			}
		}
		else {
			if ((input[42]) >= (4.5)) {
				var53 = -0.12897308;
			}
			else {
				var53 = 0.031644434;
			}
		}
	}
	else {
		if ((input[71]) >= (32729.5)) {
			if ((input[64]) >= (49.5)) {
				var53 = -0.010367158;
			}
			else {
				var53 = 0.059550613;
			}
		}
		else {
			if ((input[64]) >= (46.5)) {
				var53 = -0.07627254;
			}
			else {
				var53 = -0.00085972634;
			}
		}
	}
	double var54;
	if ((input[37]) >= (0.5)) {
		if ((input[70]) >= (9.5)) {
			if ((input[61]) >= (0.02265)) {
				var54 = 0.055511992;
			}
			else {
				var54 = -0.0533871;
			}
		}
		else {
			if ((input[47]) >= (0.95455)) {
				var54 = 0.03513288;
			}
			else {
				var54 = -0.021233333;
			}
		}
	}
	else {
		if ((input[71]) >= (141.5)) {
			if ((input[66]) >= (53971.5)) {
				var54 = 0.09938817;
			}
			else {
				var54 = 0.033617254;
			}
		}
		else {
			if ((input[58]) >= (11.558849)) {
				var54 = 0.06613953;
			}
			else {
				var54 = -0.054230656;
			}
		}
	}
	double var55;
	if ((input[64]) >= (40.5)) {
		if ((input[71]) >= (3351.5)) {
			if ((input[26]) >= (0.45)) {
				var55 = 0.04537598;
			}
			else {
				var55 = -0.005825349;
			}
		}
		else {
			if ((input[47]) >= (0.28785)) {
				var55 = -0.0027700288;
			}
			else {
				var55 = -0.05961136;
			}
		}
	}
	else {
		if ((input[71]) >= (328.5)) {
			if ((input[1]) >= (0.07045)) {
				var55 = 0.057746496;
			}
			else {
				var55 = 0.019335527;
			}
		}
		else {
			if ((input[57]) >= (0.010299999)) {
				var55 = -0.017810283;
			}
			else {
				var55 = -0.12319427;
			}
		}
	}
	double var56;
	if ((input[57]) >= (0.28935)) {
		if ((input[49]) >= (2.125)) {
			if ((input[61]) >= (0.044150002)) {
				var56 = 0.048581857;
			}
			else {
				var56 = -0.005032709;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var56 = 0.027738428;
			}
			else {
				var56 = -0.01688858;
			}
		}
	}
	else {
		if ((input[47]) >= (0.98485)) {
			if ((input[55]) >= (100.910904)) {
				var56 = -0.060584214;
			}
			else {
				var56 = 0.10107491;
			}
		}
		else {
			if ((input[5]) >= (6.875)) {
				var56 = -0.11393452;
			}
			else {
				var56 = -0.07899036;
			}
		}
	}
	double var57;
	if ((input[71]) >= (22781.5)) {
		if ((input[64]) >= (48.5)) {
			if ((input[1]) >= (3.93505)) {
				var57 = 0.053153094;
			}
			else {
				var57 = -0.025691649;
			}
		}
		else {
			if ((input[1]) >= (0.0001)) {
				var57 = 0.06638515;
			}
			else {
				var57 = -0.023588406;
			}
		}
	}
	else {
		if ((input[64]) >= (46.5)) {
			if ((input[1]) >= (3.93615)) {
				var57 = -0.037588596;
			}
			else {
				var57 = -0.0915548;
			}
		}
		else {
			if ((input[71]) >= (922.5)) {
				var57 = 0.015552307;
			}
			else {
				var57 = -0.015711471;
			}
		}
	}
	double var58;
	if ((input[47]) >= (0.98485)) {
		if ((input[63]) >= (0.99995)) {
			if ((input[64]) >= (37.5)) {
				var58 = -0.0022197883;
			}
			else {
				var58 = 0.06284245;
			}
		}
		else {
			if ((input[51]) >= (3.5)) {
				var58 = 0.07923143;
			}
			else {
				var58 = 0.040305372;
			}
		}
	}
	else {
		if ((input[71]) >= (1485.5)) {
			if ((input[64]) >= (43.5)) {
				var58 = -0.008715137;
			}
			else {
				var58 = 0.03332388;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var58 = -0.042014435;
			}
			else {
				var58 = 0.016527921;
			}
		}
	}
	double var59;
	if ((input[70]) >= (21.5)) {
		if ((input[46]) >= (3.2178)) {
			if ((input[61]) >= (0.05395)) {
				var59 = 0.07450633;
			}
			else {
				var59 = 0.035967264;
			}
		}
		else {
			if ((input[24]) >= (0.1)) {
				var59 = 0.03383368;
			}
			else {
				var59 = -0.120338954;
			}
		}
	}
	else {
		if ((input[71]) >= (39336.0)) {
			if ((input[58]) >= (649.5345)) {
				var59 = 0.027702942;
			}
			else {
				var59 = 0.091479816;
			}
		}
		else {
			if ((input[64]) >= (45.5)) {
				var59 = -0.04801045;
			}
			else {
				var59 = 0.0025718764;
			}
		}
	}
	double var60;
	if ((input[71]) >= (10058.5)) {
		if ((input[12]) >= (2.24605)) {
			if ((input[64]) >= (49.5)) {
				var60 = -0.0037338508;
			}
			else {
				var60 = 0.06098597;
			}
		}
		else {
			if ((input[64]) >= (47.5)) {
				var60 = -0.04787132;
			}
			else {
				var60 = 0.030158391;
			}
		}
	}
	else {
		if ((input[64]) >= (44.5)) {
			if ((input[0]) >= (6.0)) {
				var60 = 0.0190448;
			}
			else {
				var60 = -0.064850144;
			}
		}
		else {
			if ((input[57]) >= (0.05895)) {
				var60 = 0.006847355;
			}
			else {
				var60 = -0.09998812;
			}
		}
	}
	double var61;
	if ((input[12]) >= (0.14515)) {
		if ((input[64]) >= (40.5)) {
			if ((input[71]) >= (5014.5)) {
				var61 = 0.022276413;
			}
			else {
				var61 = -0.026796177;
			}
		}
		else {
			if ((input[71]) >= (605.5)) {
				var61 = 0.058623154;
			}
			else {
				var61 = 0.0037127677;
			}
		}
	}
	else {
		if ((input[64]) >= (44.5)) {
			if ((input[66]) >= (106338.0)) {
				var61 = 0.13205509;
			}
			else {
				var61 = -0.0716286;
			}
		}
		else {
			if ((input[47]) >= (0.98485)) {
				var61 = 0.04704858;
			}
			else {
				var61 = -0.013361878;
			}
		}
	}
	double var62;
	if ((input[37]) >= (0.5)) {
		if ((input[0]) >= (6.0)) {
			if ((input[66]) >= (337.5)) {
				var62 = 0.041981947;
			}
			else {
				var62 = -0.033080187;
			}
		}
		else {
			if ((input[47]) >= (0.28785)) {
				var62 = 0.009295749;
			}
			else {
				var62 = -0.026690414;
			}
		}
	}
	else {
		if ((input[60]) >= (0.1102)) {
			if ((input[71]) >= (66673.0)) {
				var62 = 0.10345042;
			}
			else {
				var62 = 0.02452449;
			}
		}
		else {
			if ((input[24]) >= (0.225)) {
				var62 = -0.16269815;
			}
			else {
				var62 = -0.03326011;
			}
		}
	}
	double var63;
	if ((input[70]) >= (26.5)) {
		if ((input[71]) >= (984.5)) {
			if ((input[61]) >= (0.86765003)) {
				var63 = -0.04979324;
			}
			else {
				var63 = 0.081002004;
			}
		}
		else {
			if ((input[47]) >= (0.01515)) {
				var63 = 0.0076733367;
			}
			else {
				var63 = 0.064314134;
			}
		}
	}
	else {
		if ((input[57]) >= (0.060000002)) {
			if ((input[0]) >= (6.0)) {
				var63 = 0.02421296;
			}
			else {
				var63 = -0.007939646;
			}
		}
		else {
			if ((input[18]) >= (4.5)) {
				var63 = -0.04485616;
			}
			else {
				var63 = -0.10873031;
			}
		}
	}
	double var64;
	if ((input[1]) >= (0.0167)) {
		if ((input[64]) >= (40.5)) {
			if ((input[66]) >= (8629.5)) {
				var64 = 0.021248015;
			}
			else {
				var64 = -0.0219276;
			}
		}
		else {
			if ((input[71]) >= (605.5)) {
				var64 = 0.054233372;
			}
			else {
				var64 = -0.0033065346;
			}
		}
	}
	else {
		if ((input[47]) >= (0.28785)) {
			if ((input[33]) >= (0.1)) {
				var64 = -0.013253758;
			}
			else {
				var64 = 0.08536855;
			}
		}
		else {
			if ((input[0]) >= (3.0)) {
				var64 = 0.0023729582;
			}
			else {
				var64 = -0.085054725;
			}
		}
	}
	double var65;
	if ((input[49]) >= (2.29165)) {
		if ((input[61]) >= (0.045649998)) {
			if ((input[70]) >= (2.5)) {
				var65 = 0.04210469;
			}
			else {
				var65 = 0.09810025;
			}
		}
		else {
			if ((input[46]) >= (4.0708)) {
				var65 = -0.0008250801;
			}
			else {
				var65 = -0.09029502;
			}
		}
	}
	else {
		if ((input[70]) >= (7.5)) {
			if ((input[57]) >= (0.74895)) {
				var65 = 0.036378212;
			}
			else {
				var65 = -0.04101977;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var65 = 0.016031787;
			}
			else {
				var65 = -0.017958147;
			}
		}
	}
	double var66;
	if ((input[71]) >= (3368.5)) {
		if ((input[64]) >= (45.5)) {
			if ((input[71]) >= (15334.0)) {
				var66 = 0.012997294;
			}
			else {
				var66 = -0.047511768;
			}
		}
		else {
			if ((input[12]) >= (0.24329999)) {
				var66 = 0.052537628;
			}
			else {
				var66 = 0.0069102864;
			}
		}
	}
	else {
		if ((input[64]) >= (43.5)) {
			if ((input[47]) >= (0.07575)) {
				var66 = 0.0030934678;
			}
			else {
				var66 = -0.07370663;
			}
		}
		else {
			if ((input[57]) >= (0.05625)) {
				var66 = 0.0012909445;
			}
			else {
				var66 = -0.08908411;
			}
		}
	}
	double var67;
	if ((input[71]) >= (199898.0)) {
		if ((input[56]) >= (1587.9824)) {
			if ((input[16]) >= (2.325)) {
				var67 = 0.088873215;
			}
			else {
				var67 = -0.0006896407;
			}
		}
		else {
			if ((input[61]) >= (0.05435)) {
				var67 = 0.106898546;
			}
			else {
				var67 = -0.012408318;
			}
		}
	}
	else {
		if ((input[64]) >= (47.5)) {
			if ((input[12]) >= (2.39115)) {
				var67 = -0.0029403146;
			}
			else {
				var67 = -0.072822005;
			}
		}
		else {
			if ((input[71]) >= (3368.5)) {
				var67 = 0.024293361;
			}
			else {
				var67 = -0.006216318;
			}
		}
	}
	double var68;
	if ((input[70]) >= (19.5)) {
		if ((input[46]) >= (3.2178)) {
			if ((input[60]) >= (0.25245)) {
				var68 = 0.057497155;
			}
			else {
				var68 = -0.052233674;
			}
		}
		else {
			if ((input[66]) >= (4387.0)) {
				var68 = -0.1855237;
			}
			else {
				var68 = -0.005801331;
			}
		}
	}
	else {
		if ((input[71]) >= (39336.0)) {
			if ((input[62]) >= (1498.6306)) {
				var68 = -0.023015102;
			}
			else {
				var68 = 0.051872052;
			}
		}
		else {
			if ((input[64]) >= (47.5)) {
				var68 = -0.060885735;
			}
			else {
				var68 = -0.0009575531;
			}
		}
	}
	double var69;
	if ((input[32]) >= (1.5)) {
		if ((input[12]) >= (0.0282)) {
			if ((input[66]) >= (30503.0)) {
				var69 = 0.07516322;
			}
			else {
				var69 = 0.030508876;
			}
		}
		else {
			if ((input[57]) >= (3.55035)) {
				var69 = 0.04492994;
			}
			else {
				var69 = -0.027852941;
			}
		}
	}
	else {
		if ((input[0]) >= (6.0)) {
			if ((input[71]) >= (173.5)) {
				var69 = 0.032262456;
			}
			else {
				var69 = -0.044968616;
			}
		}
		else {
			if ((input[47]) >= (0.95455)) {
				var69 = 0.033747207;
			}
			else {
				var69 = -0.015936777;
			}
		}
	}
	double var70;
	if ((input[37]) >= (0.5)) {
		if ((input[0]) >= (1.5)) {
			if ((input[66]) >= (663.5)) {
				var70 = 0.01643881;
			}
			else {
				var70 = -0.027372027;
			}
		}
		else {
			if ((input[47]) >= (0.98485)) {
				var70 = 0.037195217;
			}
			else {
				var70 = -0.0281292;
			}
		}
	}
	else {
		if ((input[46]) >= (4.0019503)) {
			if ((input[14]) >= (0.29165)) {
				var70 = 0.013126443;
			}
			else {
				var70 = 0.051255465;
			}
		}
		else {
			if ((input[66]) >= (182.5)) {
				var70 = -0.026563033;
			}
			else {
				var70 = -0.1411048;
			}
		}
	}
	double var71;
	if ((input[57]) >= (0.28935)) {
		if ((input[49]) >= (2.29165)) {
			if ((input[61]) >= (0.045649998)) {
				var71 = 0.052113503;
			}
			else {
				var71 = -0.0059883776;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var71 = 0.0184579;
			}
			else {
				var71 = -0.009745961;
			}
		}
	}
	else {
		if ((input[47]) >= (0.98485)) {
			if ((input[55]) >= (127.936905)) {
				var71 = -0.070606016;
			}
			else {
				var71 = 0.084190734;
			}
		}
		else {
			if ((input[51]) >= (3.5)) {
				var71 = -0.048770685;
			}
			else {
				var71 = -0.088986665;
			}
		}
	}
	double var72;
	if ((input[71]) >= (12887.0)) {
		if ((input[64]) >= (49.5)) {
			if ((input[71]) >= (65956.0)) {
				var72 = 0.0092015825;
			}
			else {
				var72 = -0.100280896;
			}
		}
		else {
			if ((input[1]) >= (0.00035000002)) {
				var72 = 0.037116528;
			}
			else {
				var72 = -0.038416382;
			}
		}
	}
	else {
		if ((input[64]) >= (45.5)) {
			if ((input[0]) >= (6.0)) {
				var72 = 0.016344575;
			}
			else {
				var72 = -0.07671215;
			}
		}
		else {
			if ((input[3]) >= (0.29165)) {
				var72 = -0.0070442767;
			}
			else {
				var72 = 0.01996987;
			}
		}
	}
	double var73;
	if ((input[64]) >= (40.5)) {
		if ((input[71]) >= (3351.5)) {
			if ((input[64]) >= (45.5)) {
				var73 = -0.008096742;
			}
			else {
				var73 = 0.026600717;
			}
		}
		else {
			if ((input[47]) >= (0.07575)) {
				var73 = -0.00081037654;
			}
			else {
				var73 = -0.042674664;
			}
		}
	}
	else {
		if ((input[71]) >= (334.5)) {
			if ((input[12]) >= (0.01525)) {
				var73 = 0.04302026;
			}
			else {
				var73 = 0.0087767085;
			}
		}
		else {
			if ((input[46]) >= (4.5088997)) {
				var73 = -0.008289749;
			}
			else {
				var73 = -0.053073496;
			}
		}
	}
	double var74;
	if ((input[71]) >= (199898.0)) {
		if ((input[56]) >= (1587.9824)) {
			if ((input[16]) >= (2.325)) {
				var74 = 0.08592545;
			}
			else {
				var74 = -0.0018826615;
			}
		}
		else {
			if ((input[61]) >= (0.05435)) {
				var74 = 0.10303906;
			}
			else {
				var74 = -0.02031426;
			}
		}
	}
	else {
		if ((input[64]) >= (49.5)) {
			if ((input[3]) >= (1.1)) {
				var74 = -0.27855492;
			}
			else {
				var74 = -0.063146636;
			}
		}
		else {
			if ((input[14]) >= (0.29165)) {
				var74 = -0.0073108333;
			}
			else {
				var74 = 0.014531085;
			}
		}
	}
	double var75;
	if ((input[70]) >= (26.5)) {
		if ((input[71]) >= (984.5)) {
			if ((input[61]) >= (0.034649998)) {
				var75 = 0.07705354;
			}
			else {
				var75 = 0.0024980244;
			}
		}
		else {
			if ((input[12]) >= (13.7815)) {
				var75 = -0.021230638;
			}
			else {
				var75 = 0.04492374;
			}
		}
	}
	else {
		if ((input[57]) >= (0.060000002)) {
			if ((input[64]) >= (40.5)) {
				var75 = -0.008196427;
			}
			else {
				var75 = 0.012669288;
			}
		}
		else {
			if ((input[18]) >= (4.5)) {
				var75 = -0.025417676;
			}
			else {
				var75 = -0.09767003;
			}
		}
	}
	double var76;
	if ((input[71]) >= (22761.5)) {
		if ((input[12]) >= (10.39415)) {
			if ((input[33]) >= (0.1)) {
				var76 = 0.07084229;
			}
			else {
				var76 = 0.15289913;
			}
		}
		else {
			if ((input[64]) >= (45.5)) {
				var76 = 0.0026087584;
			}
			else {
				var76 = 0.07032847;
			}
		}
	}
	else {
		if ((input[64]) >= (46.5)) {
			if ((input[71]) >= (-3984.0)) {
				var76 = -0.061576456;
			}
			else {
				var76 = 0.16365223;
			}
		}
		else {
			if ((input[71]) >= (2182.5)) {
				var76 = 0.014798777;
			}
			else {
				var76 = -0.007360503;
			}
		}
	}
	double var77;
	if ((input[12]) >= (0.14515)) {
		if ((input[61]) >= (0.02925)) {
			if ((input[49]) >= (0.875)) {
				var77 = 0.033359982;
			}
			else {
				var77 = 0.0028785958;
			}
		}
		else {
			if ((input[46]) >= (4.0099497)) {
				var77 = -0.013346642;
			}
			else {
				var77 = -0.08170228;
			}
		}
	}
	else {
		if ((input[58]) >= (21.648499)) {
			if ((input[71]) >= (62837.5)) {
				var77 = 0.07809927;
			}
			else {
				var77 = -0.043956965;
			}
		}
		else {
			if ((input[71]) >= (413.5)) {
				var77 = 0.021766389;
			}
			else {
				var77 = -0.024798803;
			}
		}
	}
	double var78;
	if ((input[33]) >= (1.45)) {
		if ((input[66]) >= (608.5)) {
			if ((input[24]) >= (1.29165)) {
				var78 = 0.07992858;
			}
			else {
				var78 = 0.032830104;
			}
		}
		else {
			if ((input[62]) >= (0.45735002)) {
				var78 = 0.0138490945;
			}
			else {
				var78 = -0.10603771;
			}
		}
	}
	else {
		if ((input[3]) >= (0.29165)) {
			if ((input[24]) >= (0.1)) {
				var78 = -0.01998676;
			}
			else {
				var78 = 0.00854313;
			}
		}
		else {
			if ((input[71]) >= (908.5)) {
				var78 = 0.025907317;
			}
			else {
				var78 = -0.014115703;
			}
		}
	}
	double var79;
	if ((input[64]) >= (40.5)) {
		if ((input[71]) >= (5265.5)) {
			if ((input[12]) >= (2.2536502)) {
				var79 = 0.028377179;
			}
			else {
				var79 = -0.0067018303;
			}
		}
		else {
			if ((input[47]) >= (0.95455)) {
				var79 = 0.02767905;
			}
			else {
				var79 = -0.02673298;
			}
		}
	}
	else {
		if ((input[71]) >= (573.5)) {
			if ((input[1]) >= (0.07045)) {
				var79 = 0.046743136;
			}
			else {
				var79 = 0.012285036;
			}
		}
		else {
			if ((input[66]) >= (177.5)) {
				var79 = 0.0031066914;
			}
			else {
				var79 = -0.033816297;
			}
		}
	}
	double var80;
	if ((input[64]) >= (43.5)) {
		if ((input[66]) >= (15191.5)) {
			if ((input[57]) >= (4.50705)) {
				var80 = -0.07972821;
			}
			else {
				var80 = 0.016255207;
			}
		}
		else {
			if ((input[0]) >= (1.5)) {
				var80 = -0.0154063925;
			}
			else {
				var80 = -0.059042346;
			}
		}
	}
	else {
		if ((input[66]) >= (1855.5)) {
			if ((input[3]) >= (0.29165)) {
				var80 = 0.016111372;
			}
			else {
				var80 = 0.06314932;
			}
		}
		else {
			if ((input[58]) >= (94.990555)) {
				var80 = -0.06258467;
			}
			else {
				var80 = 0.00008797399;
			}
		}
	}
	double var81;
	if ((input[71]) >= (199898.0)) {
		if ((input[56]) >= (1587.9824)) {
			if ((input[64]) >= (50.5)) {
				var81 = -0.0350351;
			}
			else {
				var81 = 0.06118538;
			}
		}
		else {
			if ((input[55]) >= (124.6688)) {
				var81 = 0.02377977;
			}
			else {
				var81 = 0.10821878;
			}
		}
	}
	else {
		if ((input[64]) >= (49.5)) {
			if ((input[12]) >= (19.14735)) {
				var81 = 0.057492804;
			}
			else {
				var81 = -0.07902559;
			}
		}
		else {
			if ((input[14]) >= (0.29165)) {
				var81 = -0.0074661346;
			}
			else {
				var81 = 0.014982009;
			}
		}
	}
	double var82;
	if ((input[37]) >= (0.5)) {
		if ((input[70]) >= (9.5)) {
			if ((input[46]) >= (3.18755)) {
				var82 = 0.033831846;
			}
			else {
				var82 = -0.08701992;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var82 = 0.013260135;
			}
			else {
				var82 = -0.013667098;
			}
		}
	}
	else {
		if ((input[71]) >= (66673.0)) {
			if ((input[5]) >= (4.45)) {
				var82 = 0.048282895;
			}
			else {
				var82 = 0.12505966;
			}
		}
		else {
			if ((input[63]) >= (0.99995)) {
				var82 = -0.01157238;
			}
			else {
				var82 = 0.023278477;
			}
		}
	}
	double var83;
	if ((input[47]) >= (0.98485)) {
		if ((input[47]) >= (1.0151501)) {
			if ((input[47]) >= (1.25755)) {
				var83 = 0.027931143;
			}
			else {
				var83 = -0.030309662;
			}
		}
		else {
			if ((input[70]) >= (1.5)) {
				var83 = -0.0065192915;
			}
			else {
				var83 = 0.09538587;
			}
		}
	}
	else {
		if ((input[1]) >= (0.0133)) {
			if ((input[24]) >= (0.1)) {
				var83 = -0.006470208;
			}
			else {
				var83 = 0.02296501;
			}
		}
		else {
			if ((input[0]) >= (3.0)) {
				var83 = 0.00092554325;
			}
			else {
				var83 = -0.055918884;
			}
		}
	}
	double var84;
	if ((input[64]) >= (45.5)) {
		if ((input[66]) >= (15769.0)) {
			if ((input[3]) >= (0.55)) {
				var84 = -0.025478745;
			}
			else {
				var84 = 0.029635796;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var84 = 0.01439012;
			}
			else {
				var84 = -0.06989471;
			}
		}
	}
	else {
		if ((input[71]) >= (6900.5)) {
			if ((input[12]) >= (0.066300005)) {
				var84 = 0.0503555;
			}
			else {
				var84 = -0.00015847826;
			}
		}
		else {
			if ((input[3]) >= (0.29165)) {
				var84 = -0.008994216;
			}
			else {
				var84 = 0.016137771;
			}
		}
	}
	double var85;
	if ((input[71]) >= (199898.0)) {
		if ((input[57]) >= (2.85905)) {
			if ((input[66]) >= (650390.5)) {
				var85 = 0.09916681;
			}
			else {
				var85 = 0.03340939;
			}
		}
		else {
			if ((input[28]) >= (2.5)) {
				var85 = 0.15353398;
			}
			else {
				var85 = 0.015370841;
			}
		}
	}
	else {
		if ((input[64]) >= (48.5)) {
			if ((input[3]) >= (1.36665)) {
				var85 = -0.20152132;
			}
			else {
				var85 = -0.034137685;
			}
		}
		else {
			if ((input[14]) >= (0.29165)) {
				var85 = -0.0054121776;
			}
			else {
				var85 = 0.015392645;
			}
		}
	}
	double var86;
	if ((input[57]) >= (0.060000002)) {
		if ((input[49]) >= (2.29165)) {
			if ((input[70]) >= (2.5)) {
				var86 = 0.0073109055;
			}
			else {
				var86 = 0.06813092;
			}
		}
		else {
			if ((input[42]) >= (3.5)) {
				var86 = -0.008431968;
			}
			else {
				var86 = 0.013651753;
			}
		}
	}
	else {
		if ((input[18]) >= (4.5)) {
			if ((input[47]) >= (0.2727)) {
				var86 = 0.11529409;
			}
			else {
				var86 = -0.08986672;
			}
		}
		else {
			if ((input[70]) >= (32.0)) {
				var86 = 0.048450425;
			}
			else {
				var86 = -0.09086838;
			}
		}
	}
	double var87;
	if ((input[71]) >= (50130.5)) {
		if ((input[58]) >= (649.5345)) {
			if ((input[57]) >= (3.02405)) {
				var87 = -0.00825363;
			}
			else {
				var87 = 0.067112245;
			}
		}
		else {
			if ((input[12]) >= (0.6173)) {
				var87 = 0.12573029;
			}
			else {
				var87 = 0.019498235;
			}
		}
	}
	else {
		if ((input[64]) >= (48.5)) {
			if ((input[0]) >= (6.0)) {
				var87 = 0.0031624683;
			}
			else {
				var87 = -0.0901645;
			}
		}
		else {
			if ((input[14]) >= (0.225)) {
				var87 = -0.005710014;
			}
			else {
				var87 = 0.013724436;
			}
		}
	}
	double var88;
	if ((input[64]) >= (40.5)) {
		if ((input[66]) >= (5383.0)) {
			if ((input[24]) >= (0.1)) {
				var88 = -0.0059928047;
			}
			else {
				var88 = 0.02416567;
			}
		}
		else {
			if ((input[56]) >= (409.24384)) {
				var88 = -0.10173096;
			}
			else {
				var88 = -0.015789963;
			}
		}
	}
	else {
		if ((input[71]) >= (573.5)) {
			if ((input[3]) >= (0.29165)) {
				var88 = 0.02274262;
			}
			else {
				var88 = 0.06422346;
			}
		}
		else {
			if ((input[66]) >= (152.5)) {
				var88 = 0.001143927;
			}
			else {
				var88 = -0.034362253;
			}
		}
	}
	double var89;
	if ((input[64]) >= (43.5)) {
		if ((input[66]) >= (5386.5)) {
			if ((input[14]) >= (0.36665)) {
				var89 = -0.016281074;
			}
			else {
				var89 = 0.02246768;
			}
		}
		else {
			if ((input[47]) >= (0.28785)) {
				var89 = 0.0037501757;
			}
			else {
				var89 = -0.0601757;
			}
		}
	}
	else {
		if ((input[71]) >= (2082.5)) {
			if ((input[1]) >= (0.09925)) {
				var89 = 0.03981908;
			}
			else {
				var89 = -0.0012365234;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var89 = -0.012192228;
			}
			else {
				var89 = 0.014776118;
			}
		}
	}
	double var90;
	if ((input[71]) >= (199898.0)) {
		if ((input[57]) >= (4.55445)) {
			if ((input[64]) >= (49.5)) {
				var90 = -0.10572519;
			}
			else {
				var90 = 0.063929394;
			}
		}
		else {
			if ((input[61]) >= (0.05435)) {
				var90 = 0.07243028;
			}
			else {
				var90 = -0.035699572;
			}
		}
	}
	else {
		if ((input[57]) >= (4.44985)) {
			if ((input[37]) >= (0.5)) {
				var90 = -0.12262865;
			}
			else {
				var90 = 0.07058828;
			}
		}
		else {
			if ((input[64]) >= (49.5)) {
				var90 = -0.053991385;
			}
			else {
				var90 = 0.0012320125;
			}
		}
	}
	double var91;
	if ((input[47]) >= (0.98485)) {
		if ((input[47]) >= (1.0151501)) {
			if ((input[47]) >= (1.92425)) {
				var91 = 0.06872835;
			}
			else {
				var91 = -0.0052956603;
			}
		}
		else {
			if ((input[70]) >= (1.5)) {
				var91 = -0.005606577;
			}
			else {
				var91 = 0.09264799;
			}
		}
	}
	else {
		if ((input[12]) >= (0.00035000002)) {
			if ((input[3]) >= (0.29165)) {
				var91 = -0.0067178854;
			}
			else {
				var91 = 0.019822385;
			}
		}
		else {
			if ((input[47]) >= (0.28785)) {
				var91 = 0.023679411;
			}
			else {
				var91 = -0.038659807;
			}
		}
	}
	double var92;
	if ((input[62]) >= (0.06765)) {
		if ((input[64]) >= (38.5)) {
			if ((input[62]) >= (0.19154999)) {
				var92 = -0.003815462;
			}
			else {
				var92 = 0.09720556;
			}
		}
		else {
			if ((input[66]) >= (148.5)) {
				var92 = 0.03202602;
			}
			else {
				var92 = -0.037154924;
			}
		}
	}
	else {
		if ((input[47]) >= (0.2727)) {
			if ((input[55]) >= (47.4397)) {
				var92 = -0.08358319;
			}
			else {
				var92 = 0.08105984;
			}
		}
		else {
			if ((input[3]) >= (0.55)) {
				var92 = -0.14534138;
			}
			else {
				var92 = -0.082228236;
			}
		}
	}
	double var93;
	if ((input[71]) >= (39336.0)) {
		if ((input[57]) >= (2.38805)) {
			if ((input[64]) >= (47.5)) {
				var93 = -0.0075335214;
			}
			else {
				var93 = 0.05256998;
			}
		}
		else {
			if ((input[70]) >= (4.5)) {
				var93 = 0.03472016;
			}
			else {
				var93 = 0.11807954;
			}
		}
	}
	else {
		if ((input[64]) >= (47.5)) {
			if ((input[33]) >= (0.1)) {
				var93 = -0.022780044;
			}
			else {
				var93 = -0.078280866;
			}
		}
		else {
			if ((input[14]) >= (0.29165)) {
				var93 = -0.005211794;
			}
			else {
				var93 = 0.0133311;
			}
		}
	}
	double var94;
	if ((input[70]) >= (27.5)) {
		if ((input[71]) >= (984.5)) {
			if ((input[61]) >= (0.86765003)) {
				var94 = -0.09375613;
			}
			else {
				var94 = 0.06473314;
			}
		}
		else {
			if ((input[70]) >= (59.5)) {
				var94 = -0.0025036538;
			}
			else {
				var94 = 0.07024054;
			}
		}
	}
	else {
		if ((input[59]) >= (0.95505)) {
			if ((input[71]) >= (56884.0)) {
				var94 = 0.029342338;
			}
			else {
				var94 = -0.010170258;
			}
		}
		else {
			if ((input[66]) >= (371.5)) {
				var94 = 0.019655887;
			}
			else {
				var94 = -0.013747716;
			}
		}
	}
	double var95;
	if ((input[64]) >= (44.5)) {
		if ((input[71]) >= (8962.5)) {
			if ((input[12]) >= (2.39115)) {
				var95 = 0.024385294;
			}
			else {
				var95 = -0.016431538;
			}
		}
		else {
			if ((input[71]) >= (-3557.5)) {
				var95 = -0.033130836;
			}
			else {
				var95 = 0.1797068;
			}
		}
	}
	else {
		if ((input[71]) >= (3191.0)) {
			if ((input[12]) >= (0.24814999)) {
				var95 = 0.04505192;
			}
			else {
				var95 = 0.004573399;
			}
		}
		else {
			if ((input[58]) >= (74.7563)) {
				var95 = -0.03179403;
			}
			else {
				var95 = 0.0040327176;
			}
		}
	}
	double var96;
	if ((input[60]) >= (0.0673)) {
		if ((input[0]) >= (6.0)) {
			if ((input[3]) >= (0.45)) {
				var96 = -0.0067413165;
			}
			else {
				var96 = 0.034447588;
			}
		}
		else {
			if ((input[64]) >= (45.5)) {
				var96 = -0.023088826;
			}
			else {
				var96 = 0.0028235929;
			}
		}
	}
	else {
		if ((input[47]) >= (0.2727)) {
			if ((input[55]) >= (61.0)) {
				var96 = -0.07335301;
			}
			else {
				var96 = 0.063068025;
			}
		}
		else {
			if ((input[66]) >= (6172.0)) {
				var96 = -0.010354729;
			}
			else {
				var96 = -0.09918817;
			}
		}
	}
	double var97;
	if ((input[1]) >= (0.03575)) {
		if ((input[24]) >= (0.1)) {
			if ((input[64]) >= (40.5)) {
				var97 = -0.013705132;
			}
			else {
				var97 = 0.018344983;
			}
		}
		else {
			if ((input[71]) >= (1154.0)) {
				var97 = 0.035103768;
			}
			else {
				var97 = -0.013727747;
			}
		}
	}
	else {
		if ((input[47]) >= (0.28785)) {
			if ((input[33]) >= (0.1)) {
				var97 = -0.015893715;
			}
			else {
				var97 = 0.07902827;
			}
		}
		else {
			if ((input[0]) >= (3.0)) {
				var97 = 0.004144792;
			}
			else {
				var97 = -0.064901195;
			}
		}
	}
	double var98;
	if ((input[71]) >= (-187.5)) {
		if ((input[71]) >= (1104.5)) {
			if ((input[64]) >= (40.5)) {
				var98 = -0.0007581979;
			}
			else {
				var98 = 0.030767305;
			}
		}
		else {
			if ((input[47]) >= (0.28785)) {
				var98 = 0.004439893;
			}
			else {
				var98 = -0.027108151;
			}
		}
	}
	else {
		if ((input[33]) >= (0.1)) {
			if ((input[20]) >= (2.5)) {
				var98 = -0.022023695;
			}
			else {
				var98 = 0.11043189;
			}
		}
		else {
			if ((input[47]) >= (0.25755)) {
				var98 = 0.018285709;
			}
			else {
				var98 = -0.0851098;
			}
		}
	}
	double var99;
	if ((input[71]) >= (199898.0)) {
		if ((input[56]) >= (1587.9824)) {
			if ((input[66]) >= (683184.0)) {
				var99 = 0.09497329;
			}
			else {
				var99 = -0.006048945;
			}
		}
		else {
			if ((input[55]) >= (124.6688)) {
				var99 = 0.009855781;
			}
			else {
				var99 = 0.097820096;
			}
		}
	}
	else {
		if ((input[57]) >= (4.44985)) {
			if ((input[37]) >= (0.5)) {
				var99 = -0.109597065;
			}
			else {
				var99 = 0.06633884;
			}
		}
		else {
			if ((input[64]) >= (48.5)) {
				var99 = -0.029639928;
			}
			else {
				var99 = 0.0019275204;
			}
		}
	}
	double var100;
	var100 = (1.0) / ((1.0) + (exp((0.0) - ((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((var0)+(var1)) + (var2)) + (var3)) + (var4)) + (var5)) + (var6)) + (var7)) + (var8)) + (var9)) + (var10)) + (var11)) + (var12)) + (var13)) + (var14)) + (var15)) + (var16)) + (var17)) + (var18)) + (var19)) + (var20)) + (var21)) + (var22)) + (var23)) + (var24)) + (var25)) + (var26)) + (var27)) + (var28)) + (var29)) + (var30)) + (var31)) + (var32)) + (var33)) + (var34)) + (var35)) + (var36)) + (var37)) + (var38)) + (var39)) + (var40)) + (var41)) + (var42)) + (var43)) + (var44)) + (var45)) + (var46)) + (var47)) + (var48)) + (var49)) + (var50)) + (var51)) + (var52)) + (var53)) + (var54)) + (var55)) + (var56)) + (var57)) + (var58)) + (var59)) + (var60)) + (var61)) + (var62)) + (var63)) + (var64)) + (var65)) + (var66)) + (var67)) + (var68)) + (var69)) + (var70)) + (var71)) + (var72)) + (var73)) + (var74)) + (var75)) + (var76)) + (var77)) + (var78)) + (var79)) + (var80)) + (var81)) + (var82)) + (var83)) + (var84)) + (var85)) + (var86)) + (var87)) + (var88)) + (var89)) + (var90)) + (var91)) + (var92)) + (var93)) + (var94)) + (var95)) + (var96)) + (var97)) + (var98)) + (var99)))));
	*output = var100 < 0.5 ? 0 : 1;
}

void CUclf6(double * input, double * output) {
	double var0;
	if ((input[71]) >= (848.5)) {
		if ((input[48]) >= (1.125)) {
			if ((input[2]) >= (1.5)) {
				var0 = 0.15539065;
			}
			else {
				var0 = 0.056089707;
			}
		}
		else {
			if ((input[64]) >= (43.5)) {
				var0 = -0.053175744;
			}
			else {
				var0 = 0.060802817;
			}
		}
	}
	else {
		if ((input[64]) >= (38.5)) {
			if ((input[47]) >= (1.8529501)) {
				var0 = -0.033945467;
			}
			else {
				var0 = -0.17589293;
			}
		}
		else {
			if ((input[66]) >= (215.5)) {
				var0 = 0.07417371;
			}
			else {
				var0 = -0.13439085;
			}
		}
	}
	double var1;
	if ((input[71]) >= (1445.5)) {
		if ((input[64]) >= (43.5)) {
			if ((input[71]) >= (18616.0)) {
				var1 = 0.097893775;
			}
			else {
				var1 = -0.08928481;
			}
		}
		else {
			if ((input[66]) >= (5643.5)) {
				var1 = 0.16432796;
			}
			else {
				var1 = 0.078852184;
			}
		}
	}
	else {
		if ((input[64]) >= (38.5)) {
			if ((input[70]) >= (22.5)) {
				var1 = 0.026437018;
			}
			else {
				var1 = -0.14889291;
			}
		}
		else {
			if ((input[66]) >= (266.5)) {
				var1 = 0.09739932;
			}
			else {
				var1 = -0.1108305;
			}
		}
	}
	double var2;
	if ((input[71]) >= (547.5)) {
		if ((input[24]) >= (1.45)) {
			if ((input[2]) >= (1.5)) {
				var2 = 0.13270429;
			}
			else {
				var2 = 0.04235702;
			}
		}
		else {
			if ((input[64]) >= (40.5)) {
				var2 = -0.03260291;
			}
			else {
				var2 = 0.07977941;
			}
		}
	}
	else {
		if ((input[64]) >= (38.5)) {
			if ((input[48]) >= (1.875)) {
				var2 = 0.0012837386;
			}
			else {
				var2 = -0.1551936;
			}
		}
		else {
			if ((input[71]) >= (121.5)) {
				var2 = 0.031917997;
			}
			else {
				var2 = -0.1222551;
			}
		}
	}
	double var3;
	if ((input[71]) >= (1813.5)) {
		if ((input[64]) >= (43.5)) {
			if ((input[71]) >= (33386.0)) {
				var3 = 0.10217344;
			}
			else {
				var3 = -0.05584624;
			}
		}
		else {
			if ((input[66]) >= (7562.5)) {
				var3 = 0.14948809;
			}
			else {
				var3 = 0.08076641;
			}
		}
	}
	else {
		if ((input[64]) >= (38.5)) {
			if ((input[66]) >= (1043.5)) {
				var3 = -0.069398135;
			}
			else {
				var3 = -0.13808687;
			}
		}
		else {
			if ((input[66]) >= (304.5)) {
				var3 = 0.09575553;
			}
			else {
				var3 = -0.08802553;
			}
		}
	}
	double var4;
	if ((input[71]) >= (2010.5)) {
		if ((input[64]) >= (44.5)) {
			if ((input[66]) >= (46441.0)) {
				var4 = 0.09794298;
			}
			else {
				var4 = -0.05117136;
			}
		}
		else {
			if ((input[66]) >= (9624.5)) {
				var4 = 0.1421071;
			}
			else {
				var4 = 0.074826315;
			}
		}
	}
	else {
		if ((input[64]) >= (38.5)) {
			if ((input[66]) >= (1069.5)) {
				var4 = -0.060492285;
			}
			else {
				var4 = -0.128433;
			}
		}
		else {
			if ((input[71]) >= (164.5)) {
				var4 = 0.079071745;
			}
			else {
				var4 = -0.09332329;
			}
		}
	}
	double var5;
	if ((input[71]) >= (1132.5)) {
		if ((input[33]) >= (1.225)) {
			if ((input[64]) >= (43.5)) {
				var5 = 0.05339707;
			}
			else {
				var5 = 0.11595829;
			}
		}
		else {
			if ((input[71]) >= (5794.0)) {
				var5 = 0.046164855;
			}
			else {
				var5 = -0.054583408;
			}
		}
	}
	else {
		if ((input[64]) >= (38.5)) {
			if ((input[70]) >= (23.5)) {
				var5 = 0.041741207;
			}
			else {
				var5 = -0.116947725;
			}
		}
		else {
			if ((input[71]) >= (99.5)) {
				var5 = 0.0442914;
			}
			else {
				var5 = -0.10710259;
			}
		}
	}
	double var6;
	if ((input[71]) >= (473.5)) {
		if ((input[48]) >= (1.41665)) {
			if ((input[70]) >= (1.5)) {
				var6 = 0.11140958;
			}
			else {
				var6 = 0.040652167;
			}
		}
		else {
			if ((input[32]) >= (1.5)) {
				var6 = 0.053773034;
			}
			else {
				var6 = -0.03938228;
			}
		}
	}
	else {
		if ((input[47]) >= (1.0882)) {
			if ((input[3]) >= (0.1)) {
				var6 = -0.031229038;
			}
			else {
				var6 = 0.11812732;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var6 = -0.13023418;
			}
			else {
				var6 = -0.06985525;
			}
		}
	}
	double var7;
	if ((input[13]) >= (1.5)) {
		if ((input[71]) >= (2801.5)) {
			if ((input[2]) >= (1.5)) {
				var7 = 0.09930686;
			}
			else {
				var7 = 0.0036597846;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var7 = -0.048393544;
			}
			else {
				var7 = 0.05329336;
			}
		}
	}
	else {
		if ((input[70]) >= (23.5)) {
			if ((input[71]) >= (354.0)) {
				var7 = 0.10357777;
			}
			else {
				var7 = 0.0077549107;
			}
		}
		else {
			if ((input[47]) >= (1.0882)) {
				var7 = -0.015549163;
			}
			else {
				var7 = -0.091042735;
			}
		}
	}
	double var8;
	if ((input[71]) >= (400.5)) {
		if ((input[33]) >= (1.225)) {
			if ((input[48]) >= (1.41665)) {
				var8 = 0.1004824;
			}
			else {
				var8 = 0.042204104;
			}
		}
		else {
			if ((input[71]) >= (5764.5)) {
				var8 = 0.036616754;
			}
			else {
				var8 = -0.05388701;
			}
		}
	}
	else {
		if ((input[48]) >= (1.125)) {
			if ((input[3]) >= (0.1)) {
				var8 = -0.040053874;
			}
			else {
				var8 = 0.1254692;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var8 = -0.12340919;
			}
			else {
				var8 = -0.068291746;
			}
		}
	}
	double var9;
	if ((input[13]) >= (1.5)) {
		if ((input[71]) >= (4024.5)) {
			if ((input[64]) >= (45.5)) {
				var9 = 0.02227685;
			}
			else {
				var9 = 0.10086989;
			}
		}
		else {
			if ((input[64]) >= (40.5)) {
				var9 = -0.07665606;
			}
			else {
				var9 = 0.03538222;
			}
		}
	}
	else {
		if ((input[70]) >= (4.5)) {
			if ((input[71]) >= (74.5)) {
				var9 = 0.019394798;
			}
			else {
				var9 = -0.09966401;
			}
		}
		else {
			if ((input[47]) >= (1.7353001)) {
				var9 = 0.015803738;
			}
			else {
				var9 = -0.0897456;
			}
		}
	}
	double var10;
	if ((input[71]) >= (391.5)) {
		if ((input[67]) >= (0.5)) {
			if ((input[2]) >= (1.5)) {
				var10 = 0.03557114;
			}
			else {
				var10 = -0.050243467;
			}
		}
		else {
			if ((input[48]) >= (1.125)) {
				var10 = 0.104829244;
			}
			else {
				var10 = 0.05696204;
			}
		}
	}
	else {
		if ((input[64]) >= (38.5)) {
			if ((input[48]) >= (1.875)) {
				var10 = 0.050597634;
			}
			else {
				var10 = -0.110681295;
			}
		}
		else {
			if ((input[71]) >= (74.5)) {
				var10 = 0.0064731957;
			}
			else {
				var10 = -0.096552156;
			}
		}
	}
	double var11;
	if ((input[71]) >= (2801.5)) {
		if ((input[64]) >= (45.5)) {
			if ((input[71]) >= (58198.0)) {
				var11 = 0.06845735;
			}
			else {
				var11 = -0.06931151;
			}
		}
		else {
			if ((input[71]) >= (11213.5)) {
				var11 = 0.11085576;
			}
			else {
				var11 = 0.048894297;
			}
		}
	}
	else {
		if ((input[64]) >= (39.5)) {
			if ((input[64]) >= (43.5)) {
				var11 = -0.12900652;
			}
			else {
				var11 = -0.06408709;
			}
		}
		else {
			if ((input[71]) >= (269.5)) {
				var11 = 0.06591277;
			}
			else {
				var11 = -0.052733775;
			}
		}
	}
	double var12;
	if ((input[71]) >= (660.5)) {
		if ((input[64]) >= (40.5)) {
			if ((input[71]) >= (9585.5)) {
				var12 = 0.05631717;
			}
			else {
				var12 = -0.05150195;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var12 = 0.06275733;
			}
			else {
				var12 = 0.11220645;
			}
		}
	}
	else {
		if ((input[64]) >= (38.5)) {
			if ((input[48]) >= (1.875)) {
				var12 = 0.037602633;
			}
			else {
				var12 = -0.098617055;
			}
		}
		else {
			if ((input[71]) >= (74.5)) {
				var12 = 0.020766407;
			}
			else {
				var12 = -0.0875089;
			}
		}
	}
	double var13;
	if ((input[32]) >= (1.5)) {
		if ((input[66]) >= (414.5)) {
			if ((input[48]) >= (1.41665)) {
				var13 = 0.088257395;
			}
			else {
				var13 = 0.036437612;
			}
		}
		else {
			if ((input[71]) >= (74.5)) {
				var13 = -0.0038410798;
			}
			else {
				var13 = -0.073436595;
			}
		}
	}
	else {
		if ((input[71]) >= (5176.0)) {
			if ((input[64]) >= (45.5)) {
				var13 = -0.03592111;
			}
			else {
				var13 = 0.06654767;
			}
		}
		else {
			if ((input[64]) >= (43.5)) {
				var13 = -0.12113017;
			}
			else {
				var13 = -0.043170255;
			}
		}
	}
	double var14;
	if ((input[48]) >= (1.41665)) {
		if ((input[57]) >= (2.06525)) {
			if ((input[66]) >= (50022.5)) {
				var14 = 0.10532486;
			}
			else {
				var14 = 0.057962548;
			}
		}
		else {
			if ((input[71]) >= (97.5)) {
				var14 = 0.026212478;
			}
			else {
				var14 = -0.05570368;
			}
		}
	}
	else {
		if ((input[71]) >= (269.5)) {
			if ((input[67]) >= (0.5)) {
				var14 = -0.025544582;
			}
			else {
				var14 = 0.052461028;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var14 = -0.10477556;
			}
			else {
				var14 = -0.050914854;
			}
		}
	}
	double var15;
	if ((input[32]) >= (1.5)) {
		if ((input[71]) >= (3627.5)) {
			if ((input[64]) >= (46.5)) {
				var15 = -0.0052496297;
			}
			else {
				var15 = 0.08588987;
			}
		}
		else {
			if ((input[64]) >= (40.5)) {
				var15 = -0.048361618;
			}
			else {
				var15 = 0.031456962;
			}
		}
	}
	else {
		if ((input[47]) >= (1.1470499)) {
			if ((input[71]) >= (35710.5)) {
				var15 = 0.08524793;
			}
			else {
				var15 = 0.0029088347;
			}
		}
		else {
			if ((input[70]) >= (3.5)) {
				var15 = -0.01905193;
			}
			else {
				var15 = -0.0783928;
			}
		}
	}
	double var16;
	if ((input[71]) >= (2230.5)) {
		if ((input[64]) >= (43.5)) {
			if ((input[71]) >= (47834.0)) {
				var16 = 0.062086362;
			}
			else {
				var16 = -0.041969202;
			}
		}
		else {
			if ((input[71]) >= (17728.0)) {
				var16 = 0.11187511;
			}
			else {
				var16 = 0.056487113;
			}
		}
	}
	else {
		if ((input[64]) >= (38.5)) {
			if ((input[70]) >= (21.5)) {
				var16 = 0.051885027;
			}
			else {
				var16 = -0.06899749;
			}
		}
		else {
			if ((input[66]) >= (477.5)) {
				var16 = 0.07903513;
			}
			else {
				var16 = -0.02833477;
			}
		}
	}
	double var17;
	if ((input[47]) >= (1.0882)) {
		if ((input[60]) >= (2.2641501)) {
			if ((input[70]) >= (6.5)) {
				var17 = 0.08088515;
			}
			else {
				var17 = 0.035180293;
			}
		}
		else {
			if ((input[64]) >= (39.5)) {
				var17 = -0.05627952;
			}
			else {
				var17 = 0.018167986;
			}
		}
	}
	else {
		if ((input[66]) >= (1035.5)) {
			if ((input[64]) >= (40.5)) {
				var17 = -0.019708851;
			}
			else {
				var17 = 0.06146232;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var17 = -0.09679825;
			}
			else {
				var17 = -0.02679585;
			}
		}
	}
	double var18;
	if ((input[32]) >= (1.5)) {
		if ((input[71]) >= (274.5)) {
			if ((input[71]) >= (38082.5)) {
				var18 = 0.09409925;
			}
			else {
				var18 = 0.03711084;
			}
		}
		else {
			if ((input[71]) >= (74.5)) {
				var18 = -0.0023508107;
			}
			else {
				var18 = -0.06025734;
			}
		}
	}
	else {
		if ((input[13]) >= (1.5)) {
			if ((input[71]) >= (11245.0)) {
				var18 = 0.043561034;
			}
			else {
				var18 = -0.016741786;
			}
		}
		else {
			if ((input[37]) >= (0.5)) {
				var18 = -0.07189654;
			}
			else {
				var18 = -0.02117187;
			}
		}
	}
	double var19;
	if ((input[24]) >= (1.775)) {
		if ((input[61]) >= (0.03715)) {
			if ((input[71]) >= (47718.5)) {
				var19 = 0.095942564;
			}
			else {
				var19 = 0.047775414;
			}
		}
		else {
			if ((input[64]) >= (40.5)) {
				var19 = -0.07028386;
			}
			else {
				var19 = 0.012387085;
			}
		}
	}
	else {
		if ((input[70]) >= (23.5)) {
			if ((input[61]) >= (0.0104)) {
				var19 = 0.06707891;
			}
			else {
				var19 = -0.09394964;
			}
		}
		else {
			if ((input[71]) >= (91.5)) {
				var19 = -0.014301202;
			}
			else {
				var19 = -0.07598399;
			}
		}
	}
	double var20;
	if ((input[47]) >= (1.1470499)) {
		if ((input[57]) >= (2.0462499)) {
			if ((input[70]) >= (7.5)) {
				var20 = 0.08346945;
			}
			else {
				var20 = 0.037902795;
			}
		}
		else {
			if ((input[71]) >= (126.5)) {
				var20 = 0.020966968;
			}
			else {
				var20 = -0.04267327;
			}
		}
	}
	else {
		if ((input[71]) >= (127.5)) {
			if ((input[67]) >= (0.5)) {
				var20 = -0.025760407;
			}
			else {
				var20 = 0.03731678;
			}
		}
		else {
			if ((input[70]) >= (23.5)) {
				var20 = 0.013414251;
			}
			else {
				var20 = -0.08438101;
			}
		}
	}
	double var21;
	if ((input[32]) >= (1.5)) {
		if ((input[71]) >= (6936.5)) {
			if ((input[64]) >= (46.5)) {
				var21 = -0.006695171;
			}
			else {
				var21 = 0.08189099;
			}
		}
		else {
			if ((input[64]) >= (43.5)) {
				var21 = -0.09114947;
			}
			else {
				var21 = 0.02251328;
			}
		}
	}
	else {
		if ((input[48]) >= (1.70835)) {
			if ((input[3]) >= (0.225)) {
				var21 = 0.017938128;
			}
			else {
				var21 = 0.22864662;
			}
		}
		else {
			if ((input[70]) >= (1.5)) {
				var21 = -0.01265648;
			}
			else {
				var21 = -0.0620658;
			}
		}
	}
	double var22;
	if ((input[71]) >= (3024.5)) {
		if ((input[64]) >= (43.5)) {
			if ((input[71]) >= (59368.5)) {
				var22 = 0.056765437;
			}
			else {
				var22 = -0.030581383;
			}
		}
		else {
			if ((input[71]) >= (17728.0)) {
				var22 = 0.10380343;
			}
			else {
				var22 = 0.05193397;
			}
		}
	}
	else {
		if ((input[64]) >= (40.5)) {
			if ((input[64]) >= (43.5)) {
				var22 = -0.10782211;
			}
			else {
				var22 = -0.045965668;
			}
		}
		else {
			if ((input[71]) >= (74.5)) {
				var22 = 0.020865738;
			}
			else {
				var22 = -0.065951325;
			}
		}
	}
	double var23;
	if ((input[70]) >= (4.5)) {
		if ((input[56]) >= (0.3682)) {
			if ((input[70]) >= (26.5)) {
				var23 = 0.07021138;
			}
			else {
				var23 = 0.019075727;
			}
		}
		else {
			if ((input[57]) >= (0.6804)) {
				var23 = -0.030939389;
			}
			else {
				var23 = -0.11732594;
			}
		}
	}
	else {
		if ((input[71]) >= (4129.5)) {
			if ((input[64]) >= (45.5)) {
				var23 = -0.035500202;
			}
			else {
				var23 = 0.05186395;
			}
		}
		else {
			if ((input[64]) >= (43.5)) {
				var23 = -0.11287122;
			}
			else {
				var23 = -0.026460996;
			}
		}
	}
	double var24;
	if ((input[33]) >= (1.45)) {
		if ((input[66]) >= (480.5)) {
			if ((input[64]) >= (38.5)) {
				var24 = 0.026315792;
			}
			else {
				var24 = 0.07981505;
			}
		}
		else {
			if ((input[71]) >= (52.5)) {
				var24 = 0.0019431142;
			}
			else {
				var24 = -0.05478972;
			}
		}
	}
	else {
		if ((input[71]) >= (5978.5)) {
			if ((input[64]) >= (46.5)) {
				var24 = -0.051960327;
			}
			else {
				var24 = 0.041795302;
			}
		}
		else {
			if ((input[64]) >= (43.5)) {
				var24 = -0.095773935;
			}
			else {
				var24 = -0.022134922;
			}
		}
	}
	double var25;
	if ((input[71]) >= (1259.5)) {
		if ((input[64]) >= (40.5)) {
			if ((input[71]) >= (37296.5)) {
				var25 = 0.0516642;
			}
			else {
				var25 = -0.018391773;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var25 = 0.05458014;
			}
			else {
				var25 = 0.098558255;
			}
		}
	}
	else {
		if ((input[64]) >= (38.5)) {
			if ((input[47]) >= (1.8529501)) {
				var25 = 0.041689284;
			}
			else {
				var25 = -0.06288358;
			}
		}
		else {
			if ((input[71]) >= (73.5)) {
				var25 = 0.024959033;
			}
			else {
				var25 = -0.056682207;
			}
		}
	}
	double var26;
	if ((input[47]) >= (1.1470499)) {
		if ((input[66]) >= (47216.0)) {
			if ((input[64]) >= (45.5)) {
				var26 = 0.048953157;
			}
			else {
				var26 = 0.10052915;
			}
		}
		else {
			if ((input[64]) >= (43.5)) {
				var26 = -0.051955696;
			}
			else {
				var26 = 0.030305136;
			}
		}
	}
	else {
		if ((input[71]) >= (209.5)) {
			if ((input[64]) >= (40.5)) {
				var26 = -0.021165585;
			}
			else {
				var26 = 0.025991892;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var26 = -0.08953772;
			}
			else {
				var26 = -0.03784459;
			}
		}
	}
	double var27;
	if ((input[66]) >= (817.5)) {
		if ((input[64]) >= (40.5)) {
			if ((input[71]) >= (11199.5)) {
				var27 = 0.030921072;
			}
			else {
				var27 = -0.03188445;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var27 = 0.038533796;
			}
			else {
				var27 = 0.08276036;
			}
		}
	}
	else {
		if ((input[64]) >= (38.5)) {
			if ((input[47]) >= (1.9706)) {
				var27 = 0.06478835;
			}
			else {
				var27 = -0.077185;
			}
		}
		else {
			if ((input[71]) >= (50.5)) {
				var27 = 0.010116;
			}
			else {
				var27 = -0.060784567;
			}
		}
	}
	double var28;
	if ((input[57]) >= (0.96865)) {
		if ((input[70]) >= (9.5)) {
			if ((input[68]) >= (0.5)) {
				var28 = 0.022939531;
			}
			else {
				var28 = 0.06313666;
			}
		}
		else {
			if ((input[37]) >= (0.5)) {
				var28 = -0.02073774;
			}
			else {
				var28 = 0.019944288;
			}
		}
	}
	else {
		if ((input[60]) >= (0.2407)) {
			if ((input[64]) >= (39.5)) {
				var28 = -0.05524541;
			}
			else {
				var28 = -0.012596555;
			}
		}
		else {
			if ((input[18]) >= (4.5)) {
				var28 = -0.04070423;
			}
			else {
				var28 = -0.10083307;
			}
		}
	}
	double var29;
	if ((input[71]) >= (2442.5)) {
		if ((input[64]) >= (46.5)) {
			if ((input[71]) >= (83119.0)) {
				var29 = 0.020646276;
			}
			else {
				var29 = -0.07922728;
			}
		}
		else {
			if ((input[66]) >= (30188.5)) {
				var29 = 0.076650396;
			}
			else {
				var29 = 0.022684881;
			}
		}
	}
	else {
		if ((input[64]) >= (39.5)) {
			if ((input[64]) >= (43.5)) {
				var29 = -0.09677898;
			}
			else {
				var29 = -0.03190818;
			}
		}
		else {
			if ((input[66]) >= (477.5)) {
				var29 = 0.039486777;
			}
			else {
				var29 = -0.01845646;
			}
		}
	}
	double var30;
	if ((input[32]) >= (1.5)) {
		if ((input[71]) >= (6936.5)) {
			if ((input[64]) >= (46.5)) {
				var30 = -0.0048634233;
			}
			else {
				var30 = 0.06642943;
			}
		}
		else {
			if ((input[64]) >= (43.5)) {
				var30 = -0.06676751;
			}
			else {
				var30 = 0.014606848;
			}
		}
	}
	else {
		if ((input[57]) >= (0.40625)) {
			if ((input[67]) >= (0.5)) {
				var30 = -0.02247349;
			}
			else {
				var30 = 0.023254639;
			}
		}
		else {
			if ((input[18]) >= (4.5)) {
				var30 = -0.04118951;
			}
			else {
				var30 = -0.09475556;
			}
		}
	}
	double var31;
	if ((input[48]) >= (1.70835)) {
		if ((input[3]) >= (0.1)) {
			if ((input[71]) >= (34901.5)) {
				var31 = 0.0743274;
			}
			else {
				var31 = 0.014144435;
			}
		}
		else {
			if ((input[72]) >= (59.5)) {
				var31 = 0.24040823;
			}
			else {
				var31 = -0.023203405;
			}
		}
	}
	else {
		if ((input[66]) >= (798.5)) {
			if ((input[64]) >= (43.5)) {
				var31 = -0.024679331;
			}
			else {
				var31 = 0.022615178;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var31 = -0.0737915;
			}
			else {
				var31 = -0.0112405475;
			}
		}
	}
	double var32;
	if ((input[61]) >= (0.015749998)) {
		if ((input[37]) >= (0.5)) {
			if ((input[71]) >= (9589.0)) {
				var32 = 0.021499028;
			}
			else {
				var32 = -0.023479722;
			}
		}
		else {
			if ((input[71]) >= (547.5)) {
				var32 = 0.038378116;
			}
			else {
				var32 = 0.0048553315;
			}
		}
	}
	else {
		if ((input[57]) >= (0.35055)) {
			if ((input[64]) >= (41.5)) {
				var32 = -0.0743965;
			}
			else {
				var32 = -0.018684855;
			}
		}
		else {
			if ((input[21]) >= (4.5)) {
				var32 = -0.03821534;
			}
			else {
				var32 = -0.09653186;
			}
		}
	}
	double var33;
	if ((input[47]) >= (1.0882)) {
		if ((input[71]) >= (59626.5)) {
			if ((input[64]) >= (47.5)) {
				var33 = 0.022357553;
			}
			else {
				var33 = 0.08844178;
			}
		}
		else {
			if ((input[64]) >= (43.5)) {
				var33 = -0.04299253;
			}
			else {
				var33 = 0.02537368;
			}
		}
	}
	else {
		if ((input[71]) >= (1697.5)) {
			if ((input[64]) >= (46.5)) {
				var33 = -0.052532386;
			}
			else {
				var33 = 0.01966673;
			}
		}
		else {
			if ((input[64]) >= (40.5)) {
				var33 = -0.06724759;
			}
			else {
				var33 = -0.015190706;
			}
		}
	}
	double var34;
	if ((input[70]) >= (26.5)) {
		if ((input[56]) >= (2.1179)) {
			if ((input[61]) >= (0.12015)) {
				var34 = 0.06785268;
			}
			else {
				var34 = 0.029076988;
			}
		}
		else {
			if ((input[22]) >= (10.799299)) {
				var34 = -0.202137;
			}
			else {
				var34 = -0.040507346;
			}
		}
	}
	else {
		if ((input[71]) >= (9258.5)) {
			if ((input[64]) >= (45.5)) {
				var34 = -0.012124118;
			}
			else {
				var34 = 0.060025204;
			}
		}
		else {
			if ((input[64]) >= (43.5)) {
				var34 = -0.074188575;
			}
			else {
				var34 = -0.0043732277;
			}
		}
	}
	double var35;
	if ((input[57]) >= (0.36189997)) {
		if ((input[70]) >= (5.5)) {
			if ((input[57]) >= (2.08385)) {
				var35 = 0.0367877;
			}
			else {
				var35 = 0.004605699;
			}
		}
		else {
			if ((input[37]) >= (0.5)) {
				var35 = -0.023269964;
			}
			else {
				var35 = 0.012736361;
			}
		}
	}
	else {
		if ((input[56]) >= (0.44945002)) {
			if ((input[33]) >= (1.45)) {
				var35 = 0.009088226;
			}
			else {
				var35 = -0.06493746;
			}
		}
		else {
			if ((input[21]) >= (4.5)) {
				var35 = -0.02601002;
			}
			else {
				var35 = -0.10327082;
			}
		}
	}
	double var36;
	if ((input[71]) >= (83815.5)) {
		if ((input[64]) >= (48.5)) {
			if ((input[0]) >= (6.0)) {
				var36 = 0.059600014;
			}
			else {
				var36 = -0.051648136;
			}
		}
		else {
			if ((input[56]) >= (753.0228)) {
				var36 = 0.09546693;
			}
			else {
				var36 = 0.056033906;
			}
		}
	}
	else {
		if ((input[64]) >= (44.5)) {
			if ((input[66]) >= (7246.0)) {
				var36 = -0.019800005;
			}
			else {
				var36 = -0.08213561;
			}
		}
		else {
			if ((input[66]) >= (1534.5)) {
				var36 = 0.02811924;
			}
			else {
				var36 = -0.01290666;
			}
		}
	}
	double var37;
	if ((input[32]) >= (1.5)) {
		if ((input[71]) >= (4554.5)) {
			if ((input[64]) >= (43.5)) {
				var37 = 0.019843357;
			}
			else {
				var37 = 0.06449743;
			}
		}
		else {
			if ((input[64]) >= (39.5)) {
				var37 = -0.01701293;
			}
			else {
				var37 = 0.018709643;
			}
		}
	}
	else {
		if ((input[57]) >= (0.36374998)) {
			if ((input[64]) >= (46.5)) {
				var37 = -0.06015993;
			}
			else {
				var37 = -0.0044012326;
			}
		}
		else {
			if ((input[18]) >= (4.5)) {
				var37 = -0.025183816;
			}
			else {
				var37 = -0.08651303;
			}
		}
	}
	double var38;
	if ((input[71]) >= (70846.5)) {
		if ((input[64]) >= (48.5)) {
			if ((input[71]) >= (172517.5)) {
				var38 = 0.015802795;
			}
			else {
				var38 = -0.07258383;
			}
		}
		else {
			if ((input[56]) >= (773.0277)) {
				var38 = 0.08861619;
			}
			else {
				var38 = 0.04781901;
			}
		}
	}
	else {
		if ((input[64]) >= (45.5)) {
			if ((input[0]) >= (6.0)) {
				var38 = 0.011992932;
			}
			else {
				var38 = -0.07079783;
			}
		}
		else {
			if ((input[71]) >= (3627.5)) {
				var38 = 0.03192424;
			}
			else {
				var38 = -0.007450321;
			}
		}
	}
	double var39;
	if ((input[56]) >= (0.221)) {
		if ((input[37]) >= (0.5)) {
			if ((input[71]) >= (29929.0)) {
				var39 = 0.029820982;
			}
			else {
				var39 = -0.017757801;
			}
		}
		else {
			if ((input[71]) >= (257.5)) {
				var39 = 0.02778627;
			}
			else {
				var39 = -0.0083438335;
			}
		}
	}
	else {
		if ((input[18]) >= (4.5)) {
			if ((input[48]) >= (1.125)) {
				var39 = 0.2541716;
			}
			else {
				var39 = -0.087570295;
			}
		}
		else {
			if ((input[57]) >= (0.7896)) {
				var39 = -0.017629689;
			}
			else {
				var39 = -0.09637102;
			}
		}
	}
	double var40;
	if ((input[47]) >= (1.1470499)) {
		if ((input[3]) >= (0.1)) {
			if ((input[64]) >= (40.5)) {
				var40 = -0.004428292;
			}
			else {
				var40 = 0.028018592;
			}
		}
		else {
			if ((input[33]) >= (0.225)) {
				var40 = -0.008741189;
			}
			else {
				var40 = 0.19994253;
			}
		}
	}
	else {
		if ((input[71]) >= (55.5)) {
			if ((input[42]) >= (4.5)) {
				var40 = -0.031007523;
			}
			else {
				var40 = 0.004077146;
			}
		}
		else {
			if ((input[56]) >= (38.03755)) {
				var40 = 0.07312844;
			}
			else {
				var40 = -0.07081701;
			}
		}
	}
	double var41;
	if ((input[70]) >= (21.5)) {
		if ((input[60]) >= (1.20655)) {
			if ((input[68]) >= (0.5)) {
				var41 = -0.032076534;
			}
			else {
				var41 = 0.047040164;
			}
		}
		else {
			if ((input[12]) >= (10.8291)) {
				var41 = -0.13475575;
			}
			else {
				var41 = -0.019113287;
			}
		}
	}
	else {
		if ((input[71]) >= (83815.5)) {
			if ((input[64]) >= (48.5)) {
				var41 = -0.023559017;
			}
			else {
				var41 = 0.08061614;
			}
		}
		else {
			if ((input[64]) >= (43.5)) {
				var41 = -0.035727296;
			}
			else {
				var41 = 0.0008411445;
			}
		}
	}
	double var42;
	if ((input[71]) >= (11341.0)) {
		if ((input[64]) >= (45.5)) {
			if ((input[64]) >= (49.5)) {
				var42 = -0.101917684;
			}
			else {
				var42 = 0.0048912875;
			}
		}
		else {
			if ((input[71]) >= (25874.5)) {
				var42 = 0.07717043;
			}
			else {
				var42 = 0.036418024;
			}
		}
	}
	else {
		if ((input[64]) >= (43.5)) {
			if ((input[66]) >= (4355.0)) {
				var42 = -0.029873762;
			}
			else {
				var42 = -0.085546955;
			}
		}
		else {
			if ((input[66]) >= (677.5)) {
				var42 = 0.013922139;
			}
			else {
				var42 = -0.016441496;
			}
		}
	}
	double var43;
	if ((input[56]) >= (0.18090001)) {
		if ((input[64]) >= (40.5)) {
			if ((input[71]) >= (10889.0)) {
				var43 = 0.017144637;
			}
			else {
				var43 = -0.02657992;
			}
		}
		else {
			if ((input[66]) >= (672.5)) {
				var43 = 0.03661828;
			}
			else {
				var43 = -0.0067853862;
			}
		}
	}
	else {
		if ((input[18]) >= (4.5)) {
			if ((input[48]) >= (1.125)) {
				var43 = 0.21007024;
			}
			else {
				var43 = -0.08845409;
			}
		}
		else {
			if ((input[56]) >= (0.06805)) {
				var43 = -0.06723883;
			}
			else {
				var43 = -0.10197468;
			}
		}
	}
	double var44;
	if ((input[47]) >= (1.8529501)) {
		if ((input[3]) >= (0.225)) {
			if ((input[64]) >= (40.5)) {
				var44 = -0.0014880275;
			}
			else {
				var44 = 0.042998184;
			}
		}
		else {
			if ((input[33]) >= (0.775)) {
				var44 = 0.025525365;
			}
			else {
				var44 = 0.21132647;
			}
		}
	}
	else {
		if ((input[71]) >= (127.5)) {
			if ((input[42]) >= (4.5)) {
				var44 = -0.02342805;
			}
			else {
				var44 = 0.009614167;
			}
		}
		else {
			if ((input[60]) >= (2.46435)) {
				var44 = -0.007351808;
			}
			else {
				var44 = -0.05392279;
			}
		}
	}
	double var45;
	if ((input[32]) >= (1.5)) {
		if ((input[61]) >= (0.10505)) {
			if ((input[71]) >= (17716.5)) {
				var45 = 0.054267872;
			}
			else {
				var45 = 0.019371914;
			}
		}
		else {
			if ((input[64]) >= (40.5)) {
				var45 = -0.026411703;
			}
			else {
				var45 = 0.0101096295;
			}
		}
	}
	else {
		if ((input[37]) >= (0.5)) {
			if ((input[47]) >= (1.8529501)) {
				var45 = 0.023944365;
			}
			else {
				var45 = -0.027494764;
			}
		}
		else {
			if ((input[71]) >= (50.5)) {
				var45 = 0.01001727;
			}
			else {
				var45 = -0.04759333;
			}
		}
	}
	double var46;
	if ((input[61]) >= (0.016849998)) {
		if ((input[26]) >= (3.70835)) {
			if ((input[61]) >= (0.100150004)) {
				var46 = 0.03718771;
			}
			else {
				var46 = 0.0067225727;
			}
		}
		else {
			if ((input[67]) >= (0.5)) {
				var46 = -0.011659523;
			}
			else {
				var46 = 0.026689446;
			}
		}
	}
	else {
		if ((input[57]) >= (0.26515)) {
			if ((input[56]) >= (10.86255)) {
				var46 = -0.08120295;
			}
			else {
				var46 = -0.015197188;
			}
		}
		else {
			if ((input[48]) >= (1.125)) {
				var46 = -0.04555885;
			}
			else {
				var46 = -0.09412419;
			}
		}
	}
	double var47;
	if ((input[71]) >= (83815.5)) {
		if ((input[64]) >= (48.5)) {
			if ((input[66]) >= (380464.0)) {
				var47 = 0.07543119;
			}
			else {
				var47 = -0.034007307;
			}
		}
		else {
			if ((input[56]) >= (753.0228)) {
				var47 = 0.08383664;
			}
			else {
				var47 = 0.036648426;
			}
		}
	}
	else {
		if ((input[64]) >= (46.5)) {
			if ((input[66]) >= (17853.0)) {
				var47 = -0.034898423;
			}
			else {
				var47 = -0.097263314;
			}
		}
		else {
			if ((input[71]) >= (3627.5)) {
				var47 = 0.021360142;
			}
			else {
				var47 = -0.0070578614;
			}
		}
	}
	double var48;
	if ((input[56]) >= (0.18090001)) {
		if ((input[64]) >= (38.5)) {
			if ((input[66]) >= (2224.5)) {
				var48 = 0.006335204;
			}
			else {
				var48 = -0.029115204;
			}
		}
		else {
			if ((input[71]) >= (551.5)) {
				var48 = 0.06321206;
			}
			else {
				var48 = 0.0031567966;
			}
		}
	}
	else {
		if ((input[18]) >= (4.5)) {
			if ((input[48]) >= (1.125)) {
				var48 = 0.17954047;
			}
			else {
				var48 = -0.08243012;
			}
		}
		else {
			if ((input[56]) >= (0.06805)) {
				var48 = -0.059499968;
			}
			else {
				var48 = -0.09790522;
			}
		}
	}
	double var49;
	if ((input[46]) >= (5.93515)) {
		if ((input[26]) >= (1.675)) {
			if ((input[64]) >= (38.5)) {
				var49 = 0.014490046;
			}
			else {
				var49 = 0.047634717;
			}
		}
		else {
			if ((input[71]) >= (12578.0)) {
				var49 = 0.017600236;
			}
			else {
				var49 = -0.0154412305;
			}
		}
	}
	else {
		if ((input[64]) >= (44.5)) {
			if ((input[61]) >= (0.12935)) {
				var49 = -0.014522736;
			}
			else {
				var49 = -0.076294914;
			}
		}
		else {
			if ((input[71]) >= (2217.5)) {
				var49 = 0.021736713;
			}
			else {
				var49 = -0.012608002;
			}
		}
	}
	double var50;
	if ((input[32]) >= (2.5)) {
		if ((input[1]) >= (0.0083)) {
			if ((input[46]) >= (5.52175)) {
				var50 = 0.04157815;
			}
			else {
				var50 = 0.006341873;
			}
		}
		else {
			if ((input[49]) >= (2.125)) {
				var50 = 0.013424958;
			}
			else {
				var50 = -0.05913199;
			}
		}
	}
	else {
		if ((input[64]) >= (47.5)) {
			if ((input[71]) >= (159933.5)) {
				var50 = 0.031710457;
			}
			else {
				var50 = -0.05643447;
			}
		}
		else {
			if ((input[66]) >= (22580.0)) {
				var50 = 0.040546797;
			}
			else {
				var50 = -0.005376004;
			}
		}
	}
	double var51;
	if ((input[64]) >= (40.5)) {
		if ((input[71]) >= (6003.5)) {
			if ((input[64]) >= (46.5)) {
				var51 = -0.024011694;
			}
			else {
				var51 = 0.020122785;
			}
		}
		else {
			if ((input[64]) >= (43.5)) {
				var51 = -0.059518863;
			}
			else {
				var51 = -0.015423293;
			}
		}
	}
	else {
		if ((input[66]) >= (900.5)) {
			if ((input[64]) >= (38.5)) {
				var51 = 0.02117439;
			}
			else {
				var51 = 0.06127521;
			}
		}
		else {
			if ((input[46]) >= (4.8414)) {
				var51 = 0.004672272;
			}
			else {
				var51 = -0.041945547;
			}
		}
	}
	double var52;
	if ((input[71]) >= (83815.5)) {
		if ((input[64]) >= (48.5)) {
			if ((input[0]) >= (6.0)) {
				var52 = 0.05612519;
			}
			else {
				var52 = -0.031237364;
			}
		}
		else {
			if ((input[56]) >= (753.0228)) {
				var52 = 0.07888774;
			}
			else {
				var52 = 0.0295516;
			}
		}
	}
	else {
		if ((input[64]) >= (43.5)) {
			if ((input[66]) >= (4374.5)) {
				var52 = -0.013145118;
			}
			else {
				var52 = -0.07536961;
			}
		}
		else {
			if ((input[71]) >= (17728.0)) {
				var52 = 0.073989026;
			}
			else {
				var52 = 0.0016862929;
			}
		}
	}
	double var53;
	if ((input[37]) >= (0.5)) {
		if ((input[71]) >= (58193.5)) {
			if ((input[64]) >= (48.5)) {
				var53 = -0.02829261;
			}
			else {
				var53 = 0.05608384;
			}
		}
		else {
			if ((input[64]) >= (45.5)) {
				var53 = -0.04508594;
			}
			else {
				var53 = -0.0071540545;
			}
		}
	}
	else {
		if ((input[61]) >= (0.02705)) {
			if ((input[64]) >= (37.5)) {
				var53 = 0.012932764;
			}
			else {
				var53 = 0.041209456;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var53 = -0.044874292;
			}
			else {
				var53 = -0.0013548203;
			}
		}
	}
	double var54;
	if ((input[56]) >= (0.18090001)) {
		if ((input[64]) >= (38.5)) {
			if ((input[66]) >= (1343.5)) {
				var54 = 0.0033725624;
			}
			else {
				var54 = -0.031398717;
			}
		}
		else {
			if ((input[71]) >= (551.5)) {
				var54 = 0.05498295;
			}
			else {
				var54 = 0.0025488874;
			}
		}
	}
	else {
		if ((input[21]) >= (4.5)) {
			if ((input[48]) >= (1.125)) {
				var54 = 0.15454917;
			}
			else {
				var54 = -0.08036235;
			}
		}
		else {
			if ((input[56]) >= (0.06805)) {
				var54 = -0.05579197;
			}
			else {
				var54 = -0.09599821;
			}
		}
	}
	double var55;
	if ((input[57]) >= (0.35215002)) {
		if ((input[64]) >= (40.5)) {
			if ((input[0]) >= (6.0)) {
				var55 = 0.024014613;
			}
			else {
				var55 = -0.014317222;
			}
		}
		else {
			if ((input[71]) >= (2615.5)) {
				var55 = 0.048179984;
			}
			else {
				var55 = 0.0036907836;
			}
		}
	}
	else {
		if ((input[48]) >= (1.125)) {
			if ((input[33]) >= (0.1)) {
				var55 = -0.050505556;
			}
			else {
				var55 = 0.16167262;
			}
		}
		else {
			if ((input[60]) >= (0.4092)) {
				var55 = -0.029096289;
			}
			else {
				var55 = -0.09018745;
			}
		}
	}
	double var56;
	if ((input[71]) >= (97905.5)) {
		if ((input[64]) >= (48.5)) {
			if ((input[66]) >= (380464.0)) {
				var56 = 0.07116183;
			}
			else {
				var56 = -0.022161841;
			}
		}
		else {
			if ((input[56]) >= (773.0277)) {
				var56 = 0.078714035;
			}
			else {
				var56 = 0.028114274;
			}
		}
	}
	else {
		if ((input[64]) >= (46.5)) {
			if ((input[66]) >= (17853.0)) {
				var56 = -0.023512682;
			}
			else {
				var56 = -0.08732843;
			}
		}
		else {
			if ((input[71]) >= (3627.5)) {
				var56 = 0.016348535;
			}
			else {
				var56 = -0.0052169203;
			}
		}
	}
	double var57;
	if ((input[59]) >= (0.98565)) {
		if ((input[71]) >= (135195.5)) {
			if ((input[64]) >= (49.5)) {
				var57 = -0.05030662;
			}
			else {
				var57 = 0.068763845;
			}
		}
		else {
			if ((input[64]) >= (40.5)) {
				var57 = -0.019595442;
			}
			else {
				var57 = 0.004826348;
			}
		}
	}
	else {
		if ((input[47]) >= (2.82355)) {
			if ((input[66]) >= (301.0)) {
				var57 = 0.08381892;
			}
			else {
				var57 = 0.18184398;
			}
		}
		else {
			if ((input[46]) >= (5.10765)) {
				var57 = 0.018053502;
			}
			else {
				var57 = -0.0073412657;
			}
		}
	}
	double var58;
	if ((input[56]) >= (0.30275)) {
		if ((input[26]) >= (3.70835)) {
			if ((input[61]) >= (0.08355)) {
				var58 = 0.029443298;
			}
			else {
				var58 = 0.00076212914;
			}
		}
		else {
			if ((input[70]) >= (5.5)) {
				var58 = 0.011215914;
			}
			else {
				var58 = -0.012947676;
			}
		}
	}
	else {
		if ((input[28]) >= (4.5)) {
			if ((input[48]) >= (1.125)) {
				var58 = 0.13840239;
			}
			else {
				var58 = -0.05004364;
			}
		}
		else {
			if ((input[57]) >= (0.67665005)) {
				var58 = -0.026466569;
			}
			else {
				var58 = -0.0821146;
			}
		}
	}
	double var59;
	if ((input[46]) >= (5.93515)) {
		if ((input[26]) >= (1.675)) {
			if ((input[64]) >= (38.5)) {
				var59 = 0.012347593;
			}
			else {
				var59 = 0.037878677;
			}
		}
		else {
			if ((input[71]) >= (12578.0)) {
				var59 = 0.015023743;
			}
			else {
				var59 = -0.012355488;
			}
		}
	}
	else {
		if ((input[47]) >= (2.9706001)) {
			if ((input[3]) >= (0.4)) {
				var59 = 0.023085978;
			}
			else {
				var59 = 0.1730655;
			}
		}
		else {
			if ((input[64]) >= (40.5)) {
				var59 = -0.02005127;
			}
			else {
				var59 = -0.0009514809;
			}
		}
	}
	double var60;
	if ((input[63]) >= (0.99985003)) {
		if ((input[61]) >= (0.03725)) {
			if ((input[12]) >= (0.0288)) {
				var60 = 0.0024871042;
			}
			else {
				var60 = -0.0494588;
			}
		}
		else {
			if ((input[64]) >= (43.5)) {
				var60 = -0.07180221;
			}
			else {
				var60 = -0.025236635;
			}
		}
	}
	else {
		if ((input[47]) >= (1.8529501)) {
			if ((input[3]) >= (0.1)) {
				var60 = 0.025263363;
			}
			else {
				var60 = 0.16873908;
			}
		}
		else {
			if ((input[37]) >= (0.5)) {
				var60 = -0.0067514502;
			}
			else {
				var60 = 0.012915407;
			}
		}
	}
	double var61;
	if ((input[32]) >= (2.5)) {
		if ((input[1]) >= (0.0083)) {
			if ((input[46]) >= (5.37755)) {
				var61 = 0.03331001;
			}
			else {
				var61 = -0.0010381833;
			}
		}
		else {
			if ((input[46]) >= (6.8875504)) {
				var61 = -0.13787785;
			}
			else {
				var61 = -0.02708796;
			}
		}
	}
	else {
		if ((input[59]) >= (0.99485)) {
			if ((input[66]) >= (419.5)) {
				var61 = -0.002707904;
			}
			else {
				var61 = -0.027485682;
			}
		}
		else {
			if ((input[48]) >= (2.875)) {
				var61 = 0.12523709;
			}
			else {
				var61 = 0.008048856;
			}
		}
	}
	double var62;
	if ((input[71]) >= (26290.5)) {
		if ((input[64]) >= (43.5)) {
			if ((input[64]) >= (49.5)) {
				var62 = -0.07530526;
			}
			else {
				var62 = 0.0130766155;
			}
		}
		else {
			if ((input[60]) >= (1.82615)) {
				var62 = 0.088528335;
			}
			else {
				var62 = -0.056159843;
			}
		}
	}
	else {
		if ((input[64]) >= (44.5)) {
			if ((input[61]) >= (0.12035)) {
				var62 = -0.010182478;
			}
			else {
				var62 = -0.061032105;
			}
		}
		else {
			if ((input[66]) >= (1147.5)) {
				var62 = 0.010936207;
			}
			else {
				var62 = -0.007837567;
			}
		}
	}
	double var63;
	if ((input[47]) >= (0.61765)) {
		if ((input[59]) >= (1.38555)) {
			if ((input[64]) >= (43.5)) {
				var63 = -0.02844773;
			}
			else {
				var63 = 0.0028892548;
			}
		}
		else {
			if ((input[48]) >= (2.875)) {
				var63 = 0.12520464;
			}
			else {
				var63 = 0.012433121;
			}
		}
	}
	else {
		if ((input[71]) >= (5087.5)) {
			if ((input[64]) >= (46.5)) {
				var63 = -0.020337457;
			}
			else {
				var63 = 0.02498263;
			}
		}
		else {
			if ((input[64]) >= (43.5)) {
				var63 = -0.060444184;
			}
			else {
				var63 = -0.01648076;
			}
		}
	}
	double var64;
	if ((input[0]) >= (6.0)) {
		if ((input[71]) >= (1256.5)) {
			if ((input[64]) >= (47.5)) {
				var64 = 0.006441512;
			}
			else {
				var64 = 0.049697105;
			}
		}
		else {
			if ((input[1]) >= (0.0083)) {
				var64 = 0.008528462;
			}
			else {
				var64 = -0.022679394;
			}
		}
	}
	else {
		if ((input[64]) >= (45.5)) {
			if ((input[66]) >= (21538.5)) {
				var64 = -0.010383429;
			}
			else {
				var64 = -0.08068576;
			}
		}
		else {
			if ((input[71]) >= (17780.0)) {
				var64 = 0.04868896;
			}
			else {
				var64 = -0.004123567;
			}
		}
	}
	double var65;
	if ((input[64]) >= (39.5)) {
		if ((input[66]) >= (1534.5)) {
			if ((input[0]) >= (6.0)) {
				var65 = 0.033786844;
			}
			else {
				var65 = -0.0040467232;
			}
		}
		else {
			if ((input[3]) >= (0.1)) {
				var65 = -0.04063772;
			}
			else {
				var65 = 0.04542363;
			}
		}
	}
	else {
		if ((input[71]) >= (1065.5)) {
			if ((input[64]) >= (38.5)) {
				var65 = 0.020898337;
			}
			else {
				var65 = 0.060922563;
			}
		}
		else {
			if ((input[46]) >= (4.8414)) {
				var65 = 0.009744669;
			}
			else {
				var65 = -0.028414724;
			}
		}
	}
	double var66;
	if ((input[71]) >= (178857.0)) {
		if ((input[64]) >= (49.5)) {
			if ((input[63]) >= (0.9886)) {
				var66 = 0.12444778;
			}
			else {
				var66 = -0.041368414;
			}
		}
		else {
			if ((input[64]) >= (48.5)) {
				var66 = 0.043036535;
			}
			else {
				var66 = 0.08108645;
			}
		}
	}
	else {
		if ((input[64]) >= (48.5)) {
			if ((input[0]) >= (6.0)) {
				var66 = -0.0049971356;
			}
			else {
				var66 = -0.08591711;
			}
		}
		else {
			if ((input[71]) >= (11199.5)) {
				var66 = 0.018608809;
			}
			else {
				var66 = -0.0029366128;
			}
		}
	}
	double var67;
	if ((input[56]) >= (0.1662)) {
		if ((input[64]) >= (38.5)) {
			if ((input[66]) >= (900.5)) {
				var67 = 0.0008423016;
			}
			else {
				var67 = -0.031359736;
			}
		}
		else {
			if ((input[71]) >= (551.5)) {
				var67 = 0.045602333;
			}
			else {
				var67 = 0.0027981794;
			}
		}
	}
	else {
		if ((input[51]) >= (3.5)) {
			if ((input[48]) >= (1.125)) {
				var67 = 0.13802932;
			}
			else {
				var67 = -0.06831891;
			}
		}
		else {
			if ((input[59]) >= (1.7932999)) {
				var67 = -0.051300265;
			}
			else {
				var67 = -0.095047705;
			}
		}
	}
	double var68;
	if ((input[46]) >= (6.14935)) {
		if ((input[12]) >= (0.0425)) {
			if ((input[0]) >= (6.0)) {
				var68 = 0.039088115;
			}
			else {
				var68 = 0.008156029;
			}
		}
		else {
			if ((input[13]) >= (0.5)) {
				var68 = -0.038538568;
			}
			else {
				var68 = 0.010777301;
			}
		}
	}
	else {
		if ((input[2]) >= (0.5)) {
			if ((input[64]) >= (40.5)) {
				var68 = -0.022776663;
			}
			else {
				var68 = 0.0013584428;
			}
		}
		else {
			if ((input[48]) >= (1.875)) {
				var68 = 0.13885453;
			}
			else {
				var68 = 0.005769353;
			}
		}
	}
	double var69;
	if ((input[71]) >= (97905.5)) {
		if ((input[64]) >= (48.5)) {
			if ((input[71]) >= (391556.0)) {
				var69 = 0.072937064;
			}
			else {
				var69 = -0.015435095;
			}
		}
		else {
			if ((input[56]) >= (773.0277)) {
				var69 = 0.069865756;
			}
			else {
				var69 = 0.014852661;
			}
		}
	}
	else {
		if ((input[64]) >= (47.5)) {
			if ((input[66]) >= (11179.5)) {
				var69 = -0.02734789;
			}
			else {
				var69 = -0.09383497;
			}
		}
		else {
			if ((input[66]) >= (2860.5)) {
				var69 = 0.010127194;
			}
			else {
				var69 = -0.005236167;
			}
		}
	}
	double var70;
	if ((input[56]) >= (0.1662)) {
		if ((input[64]) >= (40.5)) {
			if ((input[2]) >= (0.5)) {
				var70 = -0.011588368;
			}
			else {
				var70 = 0.023354206;
			}
		}
		else {
			if ((input[71]) >= (2693.5)) {
				var70 = 0.03825593;
			}
			else {
				var70 = 0.0033578288;
			}
		}
	}
	else {
		if ((input[70]) >= (0.5)) {
			if ((input[56]) >= (0.06805)) {
				var70 = -0.03818597;
			}
			else {
				var70 = -0.08693414;
			}
		}
		else {
			if ((input[48]) >= (1.125)) {
				var70 = 0.14181376;
			}
			else {
				var70 = -0.065072514;
			}
		}
	}
	double var71;
	if ((input[71]) >= (42668.5)) {
		if ((input[64]) >= (43.5)) {
			if ((input[64]) >= (49.5)) {
				var71 = -0.05638163;
			}
			else {
				var71 = 0.017809592;
			}
		}
		else {
			if ((input[57]) >= (1.8155501)) {
				var71 = 0.095101885;
			}
			else {
				var71 = 0.005326332;
			}
		}
	}
	else {
		if ((input[64]) >= (45.5)) {
			if ((input[0]) >= (6.0)) {
				var71 = 0.02098556;
			}
			else {
				var71 = -0.052412577;
			}
		}
		else {
			if ((input[71]) >= (6003.5)) {
				var71 = 0.018541917;
			}
			else {
				var71 = -0.0022969178;
			}
		}
	}
	double var72;
	if ((input[59]) >= (1.1784501)) {
		if ((input[64]) >= (40.5)) {
			if ((input[66]) >= (2005.5)) {
				var72 = -0.007587805;
			}
			else {
				var72 = -0.04421717;
			}
		}
		else {
			if ((input[66]) >= (419.5)) {
				var72 = 0.016514102;
			}
			else {
				var72 = -0.017878938;
			}
		}
	}
	else {
		if ((input[56]) >= (0.1832)) {
			if ((input[47]) >= (1.8529501)) {
				var72 = 0.039747335;
			}
			else {
				var72 = 0.005934941;
			}
		}
		else {
			if ((input[70]) >= (0.5)) {
				var72 = -0.089798376;
			}
			else {
				var72 = 0.027429545;
			}
		}
	}
	double var73;
	if ((input[37]) >= (0.5)) {
		if ((input[13]) >= (0.5)) {
			if ((input[70]) >= (3.5)) {
				var73 = 0.0017451681;
			}
			else {
				var73 = -0.020704344;
			}
		}
		else {
			if ((input[47]) >= (1.7353001)) {
				var73 = 0.13811831;
			}
			else {
				var73 = 0.0064661107;
			}
		}
	}
	else {
		if ((input[46]) >= (4.6487503)) {
			if ((input[55]) >= (141.01706)) {
				var73 = -0.024711799;
			}
			else {
				var73 = 0.013017656;
			}
		}
		else {
			if ((input[66]) >= (89.5)) {
				var73 = -0.013059291;
			}
			else {
				var73 = -0.08359335;
			}
		}
	}
	double var74;
	if ((input[71]) >= (130136.5)) {
		if ((input[64]) >= (48.5)) {
			if ((input[71]) >= (391556.0)) {
				var74 = 0.069954894;
			}
			else {
				var74 = -0.005341706;
			}
		}
		else {
			if ((input[56]) >= (773.0277)) {
				var74 = 0.07252129;
			}
			else {
				var74 = 0.009260506;
			}
		}
	}
	else {
		if ((input[64]) >= (48.5)) {
			if ((input[0]) >= (6.0)) {
				var74 = -0.00328677;
			}
			else {
				var74 = -0.08275156;
			}
		}
		else {
			if ((input[71]) >= (11341.0)) {
				var74 = 0.015824175;
			}
			else {
				var74 = -0.0026831334;
			}
		}
	}
	double var75;
	if ((input[59]) >= (1.65875)) {
		if ((input[64]) >= (40.5)) {
			if ((input[66]) >= (2009.5)) {
				var75 = -0.011368125;
			}
			else {
				var75 = -0.044935733;
			}
		}
		else {
			if ((input[71]) >= (1731.5)) {
				var75 = 0.026213795;
			}
			else {
				var75 = -0.0071975933;
			}
		}
	}
	else {
		if ((input[47]) >= (1.8529501)) {
			if ((input[3]) >= (0.1)) {
				var75 = 0.019425208;
			}
			else {
				var75 = 0.143255;
			}
		}
		else {
			if ((input[66]) >= (108.5)) {
				var75 = 0.004146028;
			}
			else {
				var75 = -0.027553592;
			}
		}
	}
	double var76;
	if ((input[71]) >= (83815.5)) {
		if ((input[64]) >= (46.5)) {
			if ((input[55]) >= (143.27591)) {
				var76 = -0.11457761;
			}
			else {
				var76 = 0.013788392;
			}
		}
		else {
			if ((input[57]) >= (2.9759)) {
				var76 = 0.0707952;
			}
			else {
				var76 = 0.012748065;
			}
		}
	}
	else {
		if ((input[64]) >= (44.5)) {
			if ((input[61]) >= (0.12035)) {
				var76 = -0.0033230544;
			}
			else {
				var76 = -0.04644195;
			}
		}
		else {
			if ((input[71]) >= (21938.5)) {
				var76 = 0.058639385;
			}
			else {
				var76 = 0.00079866045;
			}
		}
	}
	double var77;
	if ((input[56]) >= (0.32669997)) {
		if ((input[64]) >= (38.5)) {
			if ((input[66]) >= (900.5)) {
				var77 = 0.000748233;
			}
			else {
				var77 = -0.028855829;
			}
		}
		else {
			if ((input[71]) >= (862.5)) {
				var77 = 0.051639404;
			}
			else {
				var77 = 0.0054696724;
			}
		}
	}
	else {
		if ((input[13]) >= (0.5)) {
			if ((input[70]) >= (24.5)) {
				var77 = -0.1471649;
			}
			else {
				var77 = -0.047374073;
			}
		}
		else {
			if ((input[51]) >= (3.5)) {
				var77 = 0.10027369;
			}
			else {
				var77 = -0.032902382;
			}
		}
	}
	double var78;
	if ((input[46]) >= (6.14935)) {
		if ((input[12]) >= (0.0425)) {
			if ((input[0]) >= (6.0)) {
				var78 = 0.03465281;
			}
			else {
				var78 = 0.0067447023;
			}
		}
		else {
			if ((input[22]) >= (4.8823)) {
				var78 = -0.06446418;
			}
			else {
				var78 = -0.017764257;
			}
		}
	}
	else {
		if ((input[13]) >= (0.5)) {
			if ((input[64]) >= (38.5)) {
				var78 = -0.014552995;
			}
			else {
				var78 = 0.004577981;
			}
		}
		else {
			if ((input[47]) >= (1.7353001)) {
				var78 = 0.10033213;
			}
			else {
				var78 = 0.006566949;
			}
		}
	}
	double var79;
	if ((input[22]) >= (2.2807999)) {
		if ((input[61]) >= (0.09025)) {
			if ((input[12]) >= (3.8734999)) {
				var79 = 0.016843012;
			}
			else {
				var79 = -0.009031242;
			}
		}
		else {
			if ((input[64]) >= (43.5)) {
				var79 = -0.05014325;
			}
			else {
				var79 = -0.013878885;
			}
		}
	}
	else {
		if ((input[71]) >= (42.5)) {
			if ((input[31]) >= (0.5)) {
				var79 = 0.0009651578;
			}
			else {
				var79 = 0.029261667;
			}
		}
		else {
			if ((input[33]) >= (0.1)) {
				var79 = -0.033806633;
			}
			else {
				var79 = 0.08148903;
			}
		}
	}
	double var80;
	if ((input[56]) >= (0.06805)) {
		if ((input[59]) >= (1.14715)) {
			if ((input[55]) >= (140.33055)) {
				var80 = -0.039750766;
			}
			else {
				var80 = -0.0021905021;
			}
		}
		else {
			if ((input[47]) >= (2.82355)) {
				var80 = 0.11037009;
			}
			else {
				var80 = 0.0068785595;
			}
		}
	}
	else {
		if ((input[21]) >= (4.5)) {
			if ((input[48]) >= (1.125)) {
				var80 = 0.13069306;
			}
			else {
				var80 = -0.08085924;
			}
		}
		else {
			if ((input[71]) >= (203.5)) {
				var80 = -0.13004039;
			}
			else {
				var80 = -0.06784794;
			}
		}
	}
	double var81;
	if ((input[71]) >= (26232.5)) {
		if ((input[64]) >= (43.5)) {
			if ((input[64]) >= (49.5)) {
				var81 = -0.050924785;
			}
			else {
				var81 = 0.009624809;
			}
		}
		else {
			if ((input[57]) >= (2.5498)) {
				var81 = 0.08996925;
			}
			else {
				var81 = 0.045069464;
			}
		}
	}
	else {
		if ((input[64]) >= (46.5)) {
			if ((input[66]) >= (11157.0)) {
				var81 = -0.022512062;
			}
			else {
				var81 = -0.08883917;
			}
		}
		else {
			if ((input[22]) >= (2.2807999)) {
				var81 = -0.0057905093;
			}
			else {
				var81 = 0.008065114;
			}
		}
	}
	double var82;
	if ((input[71]) >= (178857.0)) {
		if ((input[64]) >= (48.5)) {
			if ((input[63]) >= (0.9554)) {
				var82 = 0.08491113;
			}
			else {
				var82 = 0.0000188198;
			}
		}
		else {
			if ((input[63]) >= (0.98165)) {
				var82 = 0.021590278;
			}
			else {
				var82 = 0.07905565;
			}
		}
	}
	else {
		if ((input[64]) >= (48.5)) {
			if ((input[0]) >= (6.0)) {
				var82 = -0.0012602276;
			}
			else {
				var82 = -0.066892676;
			}
		}
		else {
			if ((input[71]) >= (6003.5)) {
				var82 = 0.011734293;
			}
			else {
				var82 = -0.0031544273;
			}
		}
	}
	double var83;
	if ((input[2]) >= (0.5)) {
		if ((input[12]) >= (0.0112)) {
			if ((input[0]) >= (6.0)) {
				var83 = 0.02051857;
			}
			else {
				var83 = -0.0041016517;
			}
		}
		else {
			if ((input[61]) >= (0.1422)) {
				var83 = -0.06627083;
			}
			else {
				var83 = -0.023103258;
			}
		}
	}
	else {
		if ((input[48]) >= (1.875)) {
			if ((input[33]) >= (0.55)) {
				var83 = -0.004558004;
			}
			else {
				var83 = 0.16254203;
			}
		}
		else {
			if ((input[71]) >= (10320.5)) {
				var83 = 0.059138753;
			}
			else {
				var83 = 0.0019163542;
			}
		}
	}
	double var84;
	if ((input[46]) >= (5.93515)) {
		if ((input[12]) >= (0.0112)) {
			if ((input[64]) >= (39.5)) {
				var84 = 0.0020964341;
			}
			else {
				var84 = 0.025551766;
			}
		}
		else {
			if ((input[13]) >= (0.5)) {
				var84 = -0.03666065;
			}
			else {
				var84 = 0.015557586;
			}
		}
	}
	else {
		if ((input[2]) >= (0.5)) {
			if ((input[4]) >= (1.5)) {
				var84 = -0.0012227613;
			}
			else {
				var84 = -0.019763805;
			}
		}
		else {
			if ((input[48]) >= (1.875)) {
				var84 = 0.11156229;
			}
			else {
				var84 = 0.0037894403;
			}
		}
	}
	double var85;
	if ((input[32]) >= (2.5)) {
		if ((input[56]) >= (1.63305)) {
			if ((input[12]) >= (0.0054)) {
				var85 = 0.022953438;
			}
			else {
				var85 = -0.028987218;
			}
		}
		else {
			if ((input[70]) >= (15.5)) {
				var85 = -0.1112289;
			}
			else {
				var85 = -0.00582088;
			}
		}
	}
	else {
		if ((input[59]) >= (0.78815)) {
			if ((input[66]) >= (378.5)) {
				var85 = -0.0008301067;
			}
			else {
				var85 = -0.019268585;
			}
		}
		else {
			if ((input[48]) >= (2.875)) {
				var85 = 0.103045486;
			}
			else {
				var85 = 0.00871925;
			}
		}
	}
	double var86;
	if ((input[64]) >= (40.5)) {
		if ((input[66]) >= (2750.5)) {
			if ((input[14]) >= (1.45)) {
				var86 = -0.010065949;
			}
			else {
				var86 = 0.01488123;
			}
		}
		else {
			if ((input[47]) >= (1.1470499)) {
				var86 = 0.00808541;
			}
			else {
				var86 = -0.036512922;
			}
		}
	}
	else {
		if ((input[46]) >= (5.10815)) {
			if ((input[1]) >= (0.047399998)) {
				var86 = 0.016659614;
			}
			else {
				var86 = -0.009572052;
			}
		}
		else {
			if ((input[66]) >= (98.5)) {
				var86 = -0.0034381014;
			}
			else {
				var86 = -0.05026535;
			}
		}
	}
	double var87;
	if ((input[71]) >= (-1803.5)) {
		if ((input[64]) >= (43.5)) {
			if ((input[66]) >= (3771.0)) {
				var87 = -0.0025429667;
			}
			else {
				var87 = -0.06668463;
			}
		}
		else {
			if ((input[66]) >= (23421.5)) {
				var87 = 0.071884535;
			}
			else {
				var87 = 0.0011909812;
			}
		}
	}
	else {
		if ((input[58]) >= (463.661)) {
			var87 = 0.56758404;
		}
		else {
			if ((input[24]) >= (0.9)) {
				var87 = 0.23086058;
			}
			else {
				var87 = -0.051272046;
			}
		}
	}
	double var88;
	if ((input[22]) >= (0.84424996)) {
		if ((input[61]) >= (0.086449996)) {
			if ((input[1]) >= (0.62839997)) {
				var88 = 0.008744352;
			}
			else {
				var88 = -0.019848427;
			}
		}
		else {
			if ((input[22]) >= (7.4961)) {
				var88 = -0.029372735;
			}
			else {
				var88 = -0.0016893744;
			}
		}
	}
	else {
		if ((input[48]) >= (1.29165)) {
			if ((input[3]) >= (0.1)) {
				var88 = 0.024582272;
			}
			else {
				var88 = 0.1285888;
			}
		}
		else {
			if ((input[66]) >= (77.5)) {
				var88 = 0.009998968;
			}
			else {
				var88 = -0.034863733;
			}
		}
	}
	double var89;
	if ((input[0]) >= (6.0)) {
		if ((input[71]) >= (1256.5)) {
			if ((input[22]) >= (0.0049)) {
				var89 = 0.032818697;
			}
			else {
				var89 = -0.108727865;
			}
		}
		else {
			if ((input[46]) >= (4.9912996)) {
				var89 = 0.0046570133;
			}
			else {
				var89 = -0.02289156;
			}
		}
	}
	else {
		if ((input[64]) >= (45.5)) {
			if ((input[66]) >= (12006.5)) {
				var89 = -0.012185806;
			}
			else {
				var89 = -0.09042318;
			}
		}
		else {
			if ((input[71]) >= (25874.5)) {
				var89 = 0.043688934;
			}
			else {
				var89 = -0.0019434029;
			}
		}
	}
	double var90;
	if ((input[71]) >= (177945.5)) {
		if ((input[64]) >= (48.5)) {
			if ((input[47]) >= (0.0882)) {
				var90 = -0.004198042;
			}
			else {
				var90 = 0.063624874;
			}
		}
		else {
			if ((input[22]) >= (9.39945)) {
				var90 = 0.076439165;
			}
			else {
				var90 = 0.01253775;
			}
		}
	}
	else {
		if ((input[64]) >= (48.5)) {
			if ((input[61]) >= (0.18525)) {
				var90 = -0.017364593;
			}
			else {
				var90 = -0.075190336;
			}
		}
		else {
			if ((input[2]) >= (0.5)) {
				var90 = -0.0016736102;
			}
			else {
				var90 = 0.013851105;
			}
		}
	}
	double var91;
	if ((input[64]) >= (40.5)) {
		if ((input[66]) >= (1531.5)) {
			if ((input[0]) >= (6.0)) {
				var91 = 0.025299033;
			}
			else {
				var91 = -0.0059080864;
			}
		}
		else {
			if ((input[47]) >= (1.7353001)) {
				var91 = 0.044192422;
			}
			else {
				var91 = -0.039365493;
			}
		}
	}
	else {
		if ((input[71]) >= (2219.5)) {
			if ((input[64]) >= (38.5)) {
				var91 = 0.019524086;
			}
			else {
				var91 = 0.060021825;
			}
		}
		else {
			if ((input[46]) >= (4.8414)) {
				var91 = 0.0055297464;
			}
			else {
				var91 = -0.018343857;
			}
		}
	}
	double var92;
	if ((input[71]) >= (70476.0)) {
		if ((input[64]) >= (43.5)) {
			if ((input[55]) >= (141.58301)) {
				var92 = -0.061378982;
			}
			else {
				var92 = 0.021422464;
			}
		}
		else {
			if ((input[14]) >= (2.225)) {
				var92 = 0.09729584;
			}
			else {
				var92 = 0.046454463;
			}
		}
	}
	else {
		if ((input[64]) >= (44.5)) {
			if ((input[66]) >= (4355.0)) {
				var92 = -0.0075231567;
			}
			else {
				var92 = -0.058305662;
			}
		}
		else {
			if ((input[71]) >= (17727.5)) {
				var92 = 0.04127856;
			}
			else {
				var92 = 0.0006138548;
			}
		}
	}
	double var93;
	if ((input[71]) >= (-1803.5)) {
		if ((input[13]) >= (0.5)) {
			if ((input[1]) >= (0.01415)) {
				var93 = 0.00025664247;
			}
			else {
				var93 = -0.026862418;
			}
		}
		else {
			if ((input[47]) >= (1.9706)) {
				var93 = 0.07947463;
			}
			else {
				var93 = 0.009288516;
			}
		}
	}
	else {
		if ((input[61]) >= (0.1061)) {
			if ((input[59]) >= (1.12895)) {
				var93 = 0.43945208;
			}
			else {
				var93 = 0.02339842;
			}
		}
		else {
			if ((input[12]) >= (2.7505)) {
				var93 = 0.124767564;
			}
			else {
				var93 = -0.07437533;
			}
		}
	}
	double var94;
	if ((input[22]) >= (0.84424996)) {
		if ((input[61]) >= (0.03745)) {
			if ((input[12]) >= (0.63619995)) {
				var94 = 0.0054569133;
			}
			else {
				var94 = -0.015989011;
			}
		}
		else {
			if ((input[12]) >= (3.626)) {
				var94 = -0.036205787;
			}
			else {
				var94 = -0.0052850056;
			}
		}
	}
	else {
		if ((input[48]) >= (1.29165)) {
			if ((input[3]) >= (0.1)) {
				var94 = 0.022062508;
			}
			else {
				var94 = 0.11827501;
			}
		}
		else {
			if ((input[66]) >= (67.5)) {
				var94 = 0.009297767;
			}
			else {
				var94 = -0.033965316;
			}
		}
	}
	double var95;
	if ((input[64]) >= (39.5)) {
		if ((input[66]) >= (904.5)) {
			if ((input[0]) >= (6.0)) {
				var95 = 0.021455267;
			}
			else {
				var95 = -0.0039597177;
			}
		}
		else {
			if ((input[47]) >= (1.0882)) {
				var95 = 0.012774387;
			}
			else {
				var95 = -0.054054774;
			}
		}
	}
	else {
		if ((input[46]) >= (5.10785)) {
			if ((input[12]) >= (0.0288)) {
				var95 = 0.01719858;
			}
			else {
				var95 = -0.004891654;
			}
		}
		else {
			if ((input[66]) >= (89.5)) {
				var95 = -0.001536623;
			}
			else {
				var95 = -0.046722617;
			}
		}
	}
	double var96;
	if ((input[59]) >= (0.43555)) {
		if ((input[55]) >= (140.32614)) {
			if ((input[64]) >= (41.5)) {
				var96 = -0.04741069;
			}
			else {
				var96 = -0.0036667031;
			}
		}
		else {
			if ((input[71]) >= (12575.5)) {
				var96 = 0.014298135;
			}
			else {
				var96 = -0.0025078428;
			}
		}
	}
	else {
		if ((input[59]) >= (0.1733)) {
			if ((input[46]) >= (5.06555)) {
				var96 = 0.030759929;
			}
			else {
				var96 = -0.0070104385;
			}
		}
		else {
			if ((input[12]) >= (4.5225)) {
				var96 = 0.11021733;
			}
			else {
				var96 = 0.015246943;
			}
		}
	}
	double var97;
	if ((input[64]) >= (48.5)) {
		if ((input[61]) >= (0.2213)) {
			if ((input[0]) >= (6.0)) {
				var97 = 0.029523417;
			}
			else {
				var97 = -0.026485259;
			}
		}
		else {
			if ((input[66]) >= (37611.0)) {
				var97 = -0.041407447;
			}
			else {
				var97 = -0.084489524;
			}
		}
	}
	else {
		if ((input[66]) >= (73826.0)) {
			if ((input[64]) >= (43.5)) {
				var97 = 0.027530048;
			}
			else {
				var97 = 0.0889555;
			}
		}
		else {
			if ((input[22]) >= (3.3804)) {
				var97 = -0.0047343196;
			}
			else {
				var97 = 0.006261264;
			}
		}
	}
	double var98;
	if ((input[13]) >= (0.5)) {
		if ((input[56]) >= (0.32669997)) {
			if ((input[26]) >= (4.775)) {
				var98 = 0.008826915;
			}
			else {
				var98 = -0.0062294193;
			}
		}
		else {
			if ((input[70]) >= (23.5)) {
				var98 = -0.12735286;
			}
			else {
				var98 = -0.039258916;
			}
		}
	}
	else {
		if ((input[51]) >= (3.5)) {
			if ((input[48]) >= (1.125)) {
				var98 = 0.11210443;
			}
			else {
				var98 = 0.007843342;
			}
		}
		else {
			if ((input[71]) >= (61.5)) {
				var98 = 0.008722296;
			}
			else {
				var98 = -0.041873213;
			}
		}
	}
	double var99;
	if ((input[71]) >= (-768.5)) {
		if ((input[64]) >= (40.5)) {
			if ((input[66]) >= (1531.5)) {
				var99 = -0.0008082359;
			}
			else {
				var99 = -0.02661468;
			}
		}
		else {
			if ((input[66]) >= (618.5)) {
				var99 = 0.013745007;
			}
			else {
				var99 = -0.004116657;
			}
		}
	}
	else {
		if ((input[62]) >= (89.50975)) {
			if ((input[21]) >= (4.5)) {
				var99 = -0.033823006;
			}
			else {
				var99 = 0.3186485;
			}
		}
		else {
			if ((input[24]) >= (0.775)) {
				var99 = 0.07512766;
			}
			else {
				var99 = -0.08500435;
			}
		}
	}
	double var100;
	var100 = (1.0) / ((1.0) + (exp((0.0) - ((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((var0)+(var1)) + (var2)) + (var3)) + (var4)) + (var5)) + (var6)) + (var7)) + (var8)) + (var9)) + (var10)) + (var11)) + (var12)) + (var13)) + (var14)) + (var15)) + (var16)) + (var17)) + (var18)) + (var19)) + (var20)) + (var21)) + (var22)) + (var23)) + (var24)) + (var25)) + (var26)) + (var27)) + (var28)) + (var29)) + (var30)) + (var31)) + (var32)) + (var33)) + (var34)) + (var35)) + (var36)) + (var37)) + (var38)) + (var39)) + (var40)) + (var41)) + (var42)) + (var43)) + (var44)) + (var45)) + (var46)) + (var47)) + (var48)) + (var49)) + (var50)) + (var51)) + (var52)) + (var53)) + (var54)) + (var55)) + (var56)) + (var57)) + (var58)) + (var59)) + (var60)) + (var61)) + (var62)) + (var63)) + (var64)) + (var65)) + (var66)) + (var67)) + (var68)) + (var69)) + (var70)) + (var71)) + (var72)) + (var73)) + (var74)) + (var75)) + (var76)) + (var77)) + (var78)) + (var79)) + (var80)) + (var81)) + (var82)) + (var83)) + (var84)) + (var85)) + (var86)) + (var87)) + (var88)) + (var89)) + (var90)) + (var91)) + (var92)) + (var93)) + (var94)) + (var95)) + (var96)) + (var97)) + (var98)) + (var99)))));
	*output = var100 < 0.5 ? 0 : 1;
}

void CUclf7(double * input, double * output) {
	double var0;
	if ((input[71]) >= (472.5)) {
		if ((input[48]) >= (2.125)) {
			if ((input[66]) >= (8063.5)) {
				var0 = 0.16038659;
			}
			else {
				var0 = 0.089665115;
			}
		}
		else {
			if ((input[67]) >= (0.5)) {
				var0 = -0.0031272415;
			}
			else {
				var0 = 0.10212727;
			}
		}
	}
	else {
		if ((input[71]) >= (107.5)) {
			if ((input[64]) >= (38.5)) {
				var0 = -0.16262779;
			}
			else {
				var0 = 0.036665447;
			}
		}
		else {
			if ((input[47]) >= (2.72225)) {
				var0 = 0.066029415;
			}
			else {
				var0 = -0.16524054;
			}
		}
	}
	double var1;
	if ((input[71]) >= (551.5)) {
		if ((input[64]) >= (43.5)) {
			if ((input[71]) >= (13411.0)) {
				var1 = 0.07505014;
			}
			else {
				var1 = -0.107959785;
			}
		}
		else {
			if ((input[66]) >= (4796.0)) {
				var1 = 0.15113841;
			}
			else {
				var1 = 0.051349282;
			}
		}
	}
	else {
		if ((input[71]) >= (118.5)) {
			if ((input[64]) >= (38.5)) {
				var1 = -0.14526509;
			}
			else {
				var1 = 0.045027386;
			}
		}
		else {
			if ((input[47]) >= (2.94445)) {
				var1 = 0.08478957;
			}
			else {
				var1 = -0.14718835;
			}
		}
	}
	double var2;
	if ((input[71]) >= (344.5)) {
		if ((input[67]) >= (0.5)) {
			if ((input[71]) >= (6030.5)) {
				var2 = 0.077869974;
			}
			else {
				var2 = -0.025711564;
			}
		}
		else {
			if ((input[70]) >= (52.5)) {
				var2 = 0.15549876;
			}
			else {
				var2 = 0.09349731;
			}
		}
	}
	else {
		if ((input[47]) >= (2.6111498)) {
			if ((input[3]) >= (0.1)) {
				var2 = 0.015887232;
			}
			else {
				var2 = 0.17923753;
			}
		}
		else {
			if ((input[71]) >= (77.5)) {
				var2 = -0.06883123;
			}
			else {
				var2 = -0.14227669;
			}
		}
	}
	double var3;
	if ((input[71]) >= (645.5)) {
		if ((input[64]) >= (43.5)) {
			if ((input[71]) >= (11726.5)) {
				var3 = 0.05542013;
			}
			else {
				var3 = -0.105138384;
			}
		}
		else {
			if ((input[66]) >= (4789.5)) {
				var3 = 0.13034783;
			}
			else {
				var3 = 0.047785066;
			}
		}
	}
	else {
		if ((input[64]) >= (38.5)) {
			if ((input[47]) >= (2.6111498)) {
				var3 = 0.067969196;
			}
			else {
				var3 = -0.13979195;
			}
		}
		else {
			if ((input[66]) >= (193.5)) {
				var3 = 0.05444894;
			}
			else {
				var3 = -0.107192375;
			}
		}
	}
	double var4;
	if ((input[71]) >= (298.5)) {
		if ((input[48]) >= (2.125)) {
			if ((input[67]) >= (0.5)) {
				var4 = 0.068139665;
			}
			else {
				var4 = 0.12494446;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var4 = -0.009388736;
			}
			else {
				var4 = 0.10682223;
			}
		}
	}
	else {
		if ((input[47]) >= (2.72225)) {
			if ((input[3]) >= (0.1)) {
				var4 = 0.010959933;
			}
			else {
				var4 = 0.16025442;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var4 = -0.13445121;
			}
			else {
				var4 = -0.072982244;
			}
		}
	}
	double var5;
	if ((input[71]) >= (888.5)) {
		if ((input[64]) >= (43.5)) {
			if ((input[66]) >= (20538.5)) {
				var5 = 0.06322438;
			}
			else {
				var5 = -0.074920535;
			}
		}
		else {
			if ((input[66]) >= (7380.5)) {
				var5 = 0.12393222;
			}
			else {
				var5 = 0.05328467;
			}
		}
	}
	else {
		if ((input[64]) >= (38.5)) {
			if ((input[48]) >= (2.875)) {
				var5 = 0.10234871;
			}
			else {
				var5 = -0.119238496;
			}
		}
		else {
			if ((input[66]) >= (159.5)) {
				var5 = 0.048667252;
			}
			else {
				var5 = -0.10163937;
			}
		}
	}
	double var6;
	if ((input[71]) >= (1384.5)) {
		if ((input[64]) >= (44.5)) {
			if ((input[66]) >= (23427.0)) {
				var6 = 0.055199075;
			}
			else {
				var6 = -0.06719306;
			}
		}
		else {
			if ((input[66]) >= (11951.5)) {
				var6 = 0.1225461;
			}
			else {
				var6 = 0.060258355;
			}
		}
	}
	else {
		if ((input[64]) >= (38.5)) {
			if ((input[47]) >= (2.6111498)) {
				var6 = 0.04792733;
			}
			else {
				var6 = -0.108172454;
			}
		}
		else {
			if ((input[66]) >= (219.5)) {
				var6 = 0.067873105;
			}
			else {
				var6 = -0.08321043;
			}
		}
	}
	double var7;
	if ((input[71]) >= (792.5)) {
		if ((input[64]) >= (40.5)) {
			if ((input[71]) >= (6323.5)) {
				var7 = 0.05779424;
			}
			else {
				var7 = -0.057844568;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var7 = 0.08054092;
			}
			else {
				var7 = 0.1269083;
			}
		}
	}
	else {
		if ((input[64]) >= (38.5)) {
			if ((input[47]) >= (2.6111498)) {
				var7 = 0.048605777;
			}
			else {
				var7 = -0.108392216;
			}
		}
		else {
			if ((input[71]) >= (82.5)) {
				var7 = 0.03518134;
			}
			else {
				var7 = -0.094472386;
			}
		}
	}
	double var8;
	if ((input[71]) >= (231.5)) {
		if ((input[14]) >= (2.55)) {
			if ((input[3]) >= (2.55)) {
				var8 = 0.084898904;
			}
			else {
				var8 = 0.006644991;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var8 = -0.024665853;
			}
			else {
				var8 = 0.08169113;
			}
		}
	}
	else {
		if ((input[48]) >= (2.125)) {
			if ((input[3]) >= (0.225)) {
				var8 = -0.03730115;
			}
			else {
				var8 = 0.1601991;
			}
		}
		else {
			if ((input[71]) >= (52.5)) {
				var8 = -0.06058039;
			}
			else {
				var8 = -0.11196647;
			}
		}
	}
	double var9;
	if ((input[71]) >= (1662.5)) {
		if ((input[64]) >= (45.5)) {
			if ((input[66]) >= (31026.0)) {
				var9 = 0.033590425;
			}
			else {
				var9 = -0.085573666;
			}
		}
		else {
			if ((input[66]) >= (12582.0)) {
				var9 = 0.10405333;
			}
			else {
				var9 = 0.044186644;
			}
		}
	}
	else {
		if ((input[64]) >= (38.5)) {
			if ((input[48]) >= (2.875)) {
				var9 = 0.07477872;
			}
			else {
				var9 = -0.08789162;
			}
		}
		else {
			if ((input[71]) >= (93.5)) {
				var9 = 0.0460583;
			}
			else {
				var9 = -0.07966369;
			}
		}
	}
	double var10;
	if ((input[71]) >= (429.5)) {
		if ((input[64]) >= (40.5)) {
			if ((input[71]) >= (4035.5)) {
				var10 = 0.039216604;
			}
			else {
				var10 = -0.07538615;
			}
		}
		else {
			if ((input[66]) >= (3393.5)) {
				var10 = 0.1045259;
			}
			else {
				var10 = 0.051740497;
			}
		}
	}
	else {
		if ((input[48]) >= (2.125)) {
			if ((input[3]) >= (0.225)) {
				var10 = -0.017032344;
			}
			else {
				var10 = 0.14487766;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var10 = -0.1077822;
			}
			else {
				var10 = -0.046592396;
			}
		}
	}
	double var11;
	if ((input[71]) >= (238.5)) {
		if ((input[70]) >= (1.5)) {
			if ((input[70]) >= (27.5)) {
				var11 = 0.09569888;
			}
			else {
				var11 = 0.03054784;
			}
		}
		else {
			if ((input[64]) >= (40.5)) {
				var11 = -0.054237444;
			}
			else {
				var11 = 0.021194926;
			}
		}
	}
	else {
		if ((input[47]) >= (2.72225)) {
			if ((input[70]) >= (0.5)) {
				var11 = 0.012150652;
			}
			else {
				var11 = 0.12916245;
			}
		}
		else {
			if ((input[70]) >= (27.5)) {
				var11 = 0.058520623;
			}
			else {
				var11 = -0.075887986;
			}
		}
	}
	double var12;
	if ((input[71]) >= (2004.5)) {
		if ((input[64]) >= (45.5)) {
			if ((input[71]) >= (32208.0)) {
				var12 = 0.03250205;
			}
			else {
				var12 = -0.07289931;
			}
		}
		else {
			if ((input[66]) >= (14961.5)) {
				var12 = 0.09615355;
			}
			else {
				var12 = 0.040138945;
			}
		}
	}
	else {
		if ((input[64]) >= (38.5)) {
			if ((input[47]) >= (2.6111498)) {
				var12 = 0.030074585;
			}
			else {
				var12 = -0.07658104;
			}
		}
		else {
			if ((input[71]) >= (64.5)) {
				var12 = 0.039927777;
			}
			else {
				var12 = -0.078012876;
			}
		}
	}
	double var13;
	if ((input[71]) >= (648.5)) {
		if ((input[64]) >= (40.5)) {
			if ((input[71]) >= (9351.5)) {
				var13 = 0.04448581;
			}
			else {
				var13 = -0.040694926;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var13 = 0.054894168;
			}
			else {
				var13 = 0.09899071;
			}
		}
	}
	else {
		if ((input[64]) >= (38.5)) {
			if ((input[48]) >= (2.125)) {
				var13 = -0.0069531165;
			}
			else {
				var13 = -0.094515115;
			}
		}
		else {
			if ((input[71]) >= (52.5)) {
				var13 = 0.016792601;
			}
			else {
				var13 = -0.07830247;
			}
		}
	}
	double var14;
	if ((input[71]) >= (2615.5)) {
		if ((input[64]) >= (43.5)) {
			if ((input[71]) >= (28377.0)) {
				var14 = 0.045208067;
			}
			else {
				var14 = -0.037321936;
			}
		}
		else {
			if ((input[66]) >= (14961.5)) {
				var14 = 0.099357374;
			}
			else {
				var14 = 0.05174447;
			}
		}
	}
	else {
		if ((input[64]) >= (39.5)) {
			if ((input[48]) >= (2.875)) {
				var14 = 0.061282057;
			}
			else {
				var14 = -0.07119776;
			}
		}
		else {
			if ((input[66]) >= (143.5)) {
				var14 = 0.031877514;
			}
			else {
				var14 = -0.06669766;
			}
		}
	}
	double var15;
	if ((input[61]) >= (0.07705)) {
		if ((input[67]) >= (0.5)) {
			if ((input[13]) >= (2.5)) {
				var15 = 0.032014553;
			}
			else {
				var15 = -0.030773258;
			}
		}
		else {
			if ((input[70]) >= (48.5)) {
				var15 = 0.098572016;
			}
			else {
				var15 = 0.048770268;
			}
		}
	}
	else {
		if ((input[48]) >= (2.125)) {
			if ((input[3]) >= (0.1)) {
				var15 = -0.0045008785;
			}
			else {
				var15 = 0.1456468;
			}
		}
		else {
			if ((input[71]) >= (52.5)) {
				var15 = -0.036063995;
			}
			else {
				var15 = -0.09257423;
			}
		}
	}
	double var16;
	if ((input[47]) >= (2.1666498)) {
		if ((input[61]) >= (0.06685)) {
			if ((input[70]) >= (27.5)) {
				var16 = 0.098501526;
			}
			else {
				var16 = 0.049701568;
			}
		}
		else {
			if ((input[24]) >= (0.1)) {
				var16 = -0.0027739895;
			}
			else {
				var16 = 0.10395674;
			}
		}
	}
	else {
		if ((input[70]) >= (4.5)) {
			if ((input[61]) >= (0.022550002)) {
				var16 = 0.020122368;
			}
			else {
				var16 = -0.054402847;
			}
		}
		else {
			if ((input[71]) >= (1047.5)) {
				var16 = -0.019201204;
			}
			else {
				var16 = -0.07317209;
			}
		}
	}
	double var17;
	if ((input[13]) >= (2.5)) {
		if ((input[2]) >= (2.5)) {
			if ((input[61]) >= (0.03945)) {
				var17 = 0.054225218;
			}
			else {
				var17 = -0.012616485;
			}
		}
		else {
			if ((input[70]) >= (7.5)) {
				var17 = 0.028539708;
			}
			else {
				var17 = -0.037421383;
			}
		}
	}
	else {
		if ((input[70]) >= (5.5)) {
			if ((input[70]) >= (32.5)) {
				var17 = 0.061409917;
			}
			else {
				var17 = -0.006553162;
			}
		}
		else {
			if ((input[47]) >= (2.6111498)) {
				var17 = 0.046758927;
			}
			else {
				var17 = -0.05468869;
			}
		}
	}
	double var18;
	if ((input[47]) >= (2.1666498)) {
		if ((input[57]) >= (1.7955501)) {
			if ((input[70]) >= (27.5)) {
				var18 = 0.093397714;
			}
			else {
				var18 = 0.0444782;
			}
		}
		else {
			if ((input[47]) >= (2.6111498)) {
				var18 = 0.034688935;
			}
			else {
				var18 = -0.012727822;
			}
		}
	}
	else {
		if ((input[71]) >= (98.5)) {
			if ((input[64]) >= (40.5)) {
				var18 = -0.02870578;
			}
			else {
				var18 = 0.020736938;
			}
		}
		else {
			if ((input[48]) >= (2.875)) {
				var18 = 0.18348639;
			}
			else {
				var18 = -0.07397027;
			}
		}
	}
	double var19;
	if ((input[71]) >= (3012.5)) {
		if ((input[64]) >= (45.5)) {
			if ((input[71]) >= (50203.0)) {
				var19 = 0.033332985;
			}
			else {
				var19 = -0.05171432;
			}
		}
		else {
			if ((input[66]) >= (21388.5)) {
				var19 = 0.08525216;
			}
			else {
				var19 = 0.036498215;
			}
		}
	}
	else {
		if ((input[64]) >= (39.5)) {
			if ((input[64]) >= (43.5)) {
				var19 = -0.105516694;
			}
			else {
				var19 = -0.042518526;
			}
		}
		else {
			if ((input[71]) >= (52.5)) {
				var19 = 0.024751136;
			}
			else {
				var19 = -0.060869373;
			}
		}
	}
	double var20;
	if ((input[66]) >= (610.5)) {
		if ((input[64]) >= (40.5)) {
			if ((input[71]) >= (7085.5)) {
				var20 = 0.025801186;
			}
			else {
				var20 = -0.040736325;
			}
		}
		else {
			if ((input[66]) >= (8268.0)) {
				var20 = 0.08900141;
			}
			else {
				var20 = 0.042936664;
			}
		}
	}
	else {
		if ((input[64]) >= (38.5)) {
			if ((input[48]) >= (2.125)) {
				var20 = 0.0001801063;
			}
			else {
				var20 = -0.09177572;
			}
		}
		else {
			if ((input[71]) >= (35.5)) {
				var20 = 0.007081193;
			}
			else {
				var20 = -0.06462946;
			}
		}
	}
	double var21;
	if ((input[47]) >= (2.1666498)) {
		if ((input[61]) >= (0.12135)) {
			if ((input[70]) >= (34.5)) {
				var21 = 0.090217166;
			}
			else {
				var21 = 0.043309756;
			}
		}
		else {
			if ((input[64]) >= (41.5)) {
				var21 = -0.033521507;
			}
			else {
				var21 = 0.019355386;
			}
		}
	}
	else {
		if ((input[70]) >= (3.5)) {
			if ((input[57]) >= (0.89665)) {
				var21 = 0.0115269;
			}
			else {
				var21 = -0.061515268;
			}
		}
		else {
			if ((input[71]) >= (113.5)) {
				var21 = -0.02891077;
			}
			else {
				var21 = -0.079070725;
			}
		}
	}
	double var22;
	if ((input[61]) >= (0.09465)) {
		if ((input[70]) >= (27.5)) {
			if ((input[70]) >= (99.5)) {
				var22 = 0.096434265;
			}
			else {
				var22 = 0.05260191;
			}
		}
		else {
			if ((input[64]) >= (46.5)) {
				var22 = -0.062408585;
			}
			else {
				var22 = 0.017383931;
			}
		}
	}
	else {
		if ((input[57]) >= (0.75405)) {
			if ((input[64]) >= (38.5)) {
				var22 = -0.03836192;
			}
			else {
				var22 = 0.00988989;
			}
		}
		else {
			if ((input[47]) >= (2.6667)) {
				var22 = 0.014822999;
			}
			else {
				var22 = -0.07890387;
			}
		}
	}
	double var23;
	if ((input[48]) >= (1.58335)) {
		if ((input[66]) >= (10723.5)) {
			if ((input[64]) >= (45.5)) {
				var23 = -0.0010965228;
			}
			else {
				var23 = 0.06698789;
			}
		}
		else {
			if ((input[64]) >= (43.5)) {
				var23 = -0.066133276;
			}
			else {
				var23 = 0.009610575;
			}
		}
	}
	else {
		if ((input[71]) >= (69.5)) {
			if ((input[32]) >= (2.5)) {
				var23 = 0.021240177;
			}
			else {
				var23 = -0.02990219;
			}
		}
		else {
			if ((input[70]) >= (4.5)) {
				var23 = -0.041541927;
			}
			else {
				var23 = -0.09136429;
			}
		}
	}
	double var24;
	if ((input[57]) >= (1.68855)) {
		if ((input[70]) >= (21.5)) {
			if ((input[70]) >= (99.5)) {
				var24 = 0.09102719;
			}
			else {
				var24 = 0.04808192;
			}
		}
		else {
			if ((input[47]) >= (2.1666498)) {
				var24 = 0.032075007;
			}
			else {
				var24 = -0.0078124213;
			}
		}
	}
	else {
		if ((input[57]) >= (0.38905)) {
			if ((input[64]) >= (38.5)) {
				var24 = -0.043539908;
			}
			else {
				var24 = 0.0015960757;
			}
		}
		else {
			if ((input[47]) >= (2.6667)) {
				var24 = 0.021019155;
			}
			else {
				var24 = -0.087959826;
			}
		}
	}
	double var25;
	if ((input[71]) >= (1176.5)) {
		if ((input[64]) >= (43.5)) {
			if ((input[71]) >= (27836.0)) {
				var25 = 0.028601436;
			}
			else {
				var25 = -0.041400637;
			}
		}
		else {
			if ((input[66]) >= (14961.5)) {
				var25 = 0.077269584;
			}
			else {
				var25 = 0.028264923;
			}
		}
	}
	else {
		if ((input[64]) >= (38.5)) {
			if ((input[70]) >= (20.5)) {
				var25 = 0.06498383;
			}
			else {
				var25 = -0.055103537;
			}
		}
		else {
			if ((input[71]) >= (35.5)) {
				var25 = 0.01610521;
			}
			else {
				var25 = -0.05504262;
			}
		}
	}
	double var26;
	if ((input[48]) >= (2.125)) {
		if ((input[70]) >= (34.5)) {
			if ((input[56]) >= (8.061399)) {
				var26 = 0.07618039;
			}
			else {
				var26 = -0.07135476;
			}
		}
		else {
			if ((input[3]) >= (0.225)) {
				var26 = 0.014776431;
			}
			else {
				var26 = 0.13075197;
			}
		}
	}
	else {
		if ((input[46]) >= (5.13925)) {
			if ((input[70]) >= (1.5)) {
				var26 = 0.011842356;
			}
			else {
				var26 = -0.027756635;
			}
		}
		else {
			if ((input[60]) >= (9.77145)) {
				var26 = -0.0115837185;
			}
			else {
				var26 = -0.062311795;
			}
		}
	}
	double var27;
	if ((input[66]) >= (496.5)) {
		if ((input[64]) >= (40.5)) {
			if ((input[71]) >= (3484.5)) {
				var27 = 0.011709391;
			}
			else {
				var27 = -0.046787858;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var27 = 0.027270213;
			}
			else {
				var27 = 0.06075271;
			}
		}
	}
	else {
		if ((input[64]) >= (38.5)) {
			if ((input[3]) >= (0.1)) {
				var27 = -0.07374297;
			}
			else {
				var27 = 0.05993503;
			}
		}
		else {
			if ((input[46]) >= (5.04305)) {
				var27 = 0.007242027;
			}
			else {
				var27 = -0.049932316;
			}
		}
	}
	double var28;
	if ((input[47]) >= (2.6111498)) {
		if ((input[56]) >= (1277.6995)) {
			if ((input[64]) >= (43.5)) {
				var28 = 0.049935725;
			}
			else {
				var28 = 0.08288183;
			}
		}
		else {
			if ((input[70]) >= (0.5)) {
				var28 = 0.030609855;
			}
			else {
				var28 = 0.10274477;
			}
		}
	}
	else {
		if ((input[32]) >= (2.5)) {
			if ((input[61]) >= (0.07195)) {
				var28 = 0.041824155;
			}
			else {
				var28 = 0.0010659206;
			}
		}
		else {
			if ((input[70]) >= (3.5)) {
				var28 = -0.00019076966;
			}
			else {
				var28 = -0.03456108;
			}
		}
	}
	double var29;
	if ((input[57]) >= (1.40275)) {
		if ((input[64]) >= (46.5)) {
			if ((input[71]) >= (60213.0)) {
				var29 = 0.009606163;
			}
			else {
				var29 = -0.080329284;
			}
		}
		else {
			if ((input[71]) >= (19650.0)) {
				var29 = 0.056822963;
			}
			else {
				var29 = 0.005742905;
			}
		}
	}
	else {
		if ((input[57]) >= (0.34335)) {
			if ((input[64]) >= (38.5)) {
				var29 = -0.042589817;
			}
			else {
				var29 = -0.0037363835;
			}
		}
		else {
			if ((input[47]) >= (2.6667)) {
				var29 = 0.016369857;
			}
			else {
				var29 = -0.08191744;
			}
		}
	}
	double var30;
	if ((input[48]) >= (2.70835)) {
		if ((input[3]) >= (0.1)) {
			if ((input[61]) >= (0.14934999)) {
				var30 = 0.05940292;
			}
			else {
				var30 = 0.016535677;
			}
		}
		else {
			if ((input[47]) >= (2.2778)) {
				var30 = 0.10095717;
			}
			else {
				var30 = 0.2388831;
			}
		}
	}
	else {
		if ((input[46]) >= (5.43545)) {
			if ((input[33]) >= (1.9)) {
				var30 = 0.02511397;
			}
			else {
				var30 = -0.008623793;
			}
		}
		else {
			if ((input[61]) >= (0.18685001)) {
				var30 = 0.0031508345;
			}
			else {
				var30 = -0.0400458;
			}
		}
	}
	double var31;
	if ((input[48]) >= (1.58335)) {
		if ((input[61]) >= (0.17455)) {
			if ((input[70]) >= (42.5)) {
				var31 = 0.07315071;
			}
			else {
				var31 = 0.024575857;
			}
		}
		else {
			if ((input[46]) >= (5.66545)) {
				var31 = 0.017934127;
			}
			else {
				var31 = -0.015232138;
			}
		}
	}
	else {
		if ((input[66]) >= (83.5)) {
			if ((input[64]) >= (43.5)) {
				var31 = -0.04370255;
			}
			else {
				var31 = -0.0045217886;
			}
		}
		else {
			if ((input[46]) >= (4.88135)) {
				var31 = -0.056810547;
			}
			else {
				var31 = -0.09340492;
			}
		}
	}
	double var32;
	if ((input[57]) >= (2.0251498)) {
		if ((input[64]) >= (45.5)) {
			if ((input[71]) >= (60169.5)) {
				var32 = 0.033432417;
			}
			else {
				var32 = -0.04176824;
			}
		}
		else {
			if ((input[71]) >= (9283.5)) {
				var32 = 0.0536397;
			}
			else {
				var32 = 0.009279363;
			}
		}
	}
	else {
		if ((input[57]) >= (0.34335)) {
			if ((input[64]) >= (38.5)) {
				var32 = -0.02794368;
			}
			else {
				var32 = 0.006739413;
			}
		}
		else {
			if ((input[47]) >= (2.6667)) {
				var32 = 0.014872238;
			}
			else {
				var32 = -0.07615847;
			}
		}
	}
	double var33;
	if ((input[47]) >= (2.1666498)) {
		if ((input[60]) >= (24.970749)) {
			if ((input[64]) >= (40.5)) {
				var33 = 0.038195383;
			}
			else {
				var33 = 0.077685885;
			}
		}
		else {
			if ((input[47]) >= (2.6111498)) {
				var33 = 0.032782298;
			}
			else {
				var33 = 0.0033607052;
			}
		}
	}
	else {
		if ((input[48]) >= (2.70835)) {
			if ((input[3]) >= (0.1)) {
				var33 = 0.048648026;
			}
			else {
				var33 = 0.21150365;
			}
		}
		else {
			if ((input[66]) >= (83.5)) {
				var33 = -0.0092418855;
			}
			else {
				var33 = -0.06251488;
			}
		}
	}
	double var34;
	if ((input[32]) >= (2.5)) {
		if ((input[61]) >= (0.07195)) {
			if ((input[70]) >= (137.5)) {
				var34 = 0.087756984;
			}
			else {
				var34 = 0.036032822;
			}
		}
		else {
			if ((input[64]) >= (40.5)) {
				var34 = -0.028247282;
			}
			else {
				var34 = 0.015440772;
			}
		}
	}
	else {
		if ((input[60]) >= (27.75195)) {
			if ((input[64]) >= (45.5)) {
				var34 = -0.001293431;
			}
			else {
				var34 = 0.042354513;
			}
		}
		else {
			if ((input[64]) >= (43.5)) {
				var34 = -0.047358736;
			}
			else {
				var34 = -0.0057331533;
			}
		}
	}
	double var35;
	if ((input[46]) >= (5.13895)) {
		if ((input[64]) >= (40.5)) {
			if ((input[71]) >= (19551.5)) {
				var35 = 0.028432423;
			}
			else {
				var35 = -0.021790482;
			}
		}
		else {
			if ((input[66]) >= (413.5)) {
				var35 = 0.03922875;
			}
			else {
				var35 = -0.001065238;
			}
		}
	}
	else {
		if ((input[66]) >= (1325.5)) {
			if ((input[64]) >= (43.5)) {
				var35 = -0.03296664;
			}
			else {
				var35 = 0.017107861;
			}
		}
		else {
			if ((input[48]) >= (2.875)) {
				var35 = 0.045856692;
			}
			else {
				var35 = -0.041583203;
			}
		}
	}
	double var36;
	if ((input[57]) >= (0.95075)) {
		if ((input[70]) >= (21.5)) {
			if ((input[56]) >= (5.5818)) {
				var36 = 0.042332854;
			}
			else {
				var36 = -0.07496751;
			}
		}
		else {
			if ((input[64]) >= (46.5)) {
				var36 = -0.053240564;
			}
			else {
				var36 = 0.0027300983;
			}
		}
	}
	else {
		if ((input[56]) >= (0.0634)) {
			if ((input[64]) >= (38.5)) {
				var36 = -0.04556736;
			}
			else {
				var36 = -0.012510649;
			}
		}
		else {
			if ((input[66]) >= (5.5)) {
				var36 = -0.09027444;
			}
			else {
				var36 = -0.009977332;
			}
		}
	}
	double var37;
	if ((input[71]) >= (2004.5)) {
		if ((input[64]) >= (43.5)) {
			if ((input[71]) >= (40770.0)) {
				var37 = 0.029782895;
			}
			else {
				var37 = -0.023796575;
			}
		}
		else {
			if ((input[66]) >= (21388.5)) {
				var37 = 0.06806821;
			}
			else {
				var37 = 0.024635201;
			}
		}
	}
	else {
		if ((input[64]) >= (39.5)) {
			if ((input[3]) >= (0.1)) {
				var37 = -0.0437425;
			}
			else {
				var37 = 0.033398654;
			}
		}
		else {
			if ((input[71]) >= (186.5)) {
				var37 = 0.022638751;
			}
			else {
				var37 = -0.016582457;
			}
		}
	}
	double var38;
	if ((input[46]) >= (5.43545)) {
		if ((input[42]) >= (3.5)) {
			if ((input[13]) >= (2.5)) {
				var38 = 0.004255743;
			}
			else {
				var38 = -0.028266607;
			}
		}
		else {
			if ((input[64]) >= (46.5)) {
				var38 = -0.030330373;
			}
			else {
				var38 = 0.023151997;
			}
		}
	}
	else {
		if ((input[60]) >= (15.88085)) {
			if ((input[70]) >= (27.5)) {
				var38 = 0.04975697;
			}
			else {
				var38 = 0.002123447;
			}
		}
		else {
			if ((input[48]) >= (2.875)) {
				var38 = 0.03027377;
			}
			else {
				var38 = -0.027923465;
			}
		}
	}
	double var39;
	if ((input[47]) >= (2.1666498)) {
		if ((input[70]) >= (105.5)) {
			if ((input[56]) >= (69.487656)) {
				var39 = 0.07973551;
			}
			else {
				var39 = -0.11481618;
			}
		}
		else {
			if ((input[3]) >= (0.225)) {
				var39 = 0.012354094;
			}
			else {
				var39 = 0.08421663;
			}
		}
	}
	else {
		if ((input[48]) >= (2.70835)) {
			if ((input[3]) >= (0.9)) {
				var39 = 0.03999089;
			}
			else {
				var39 = 0.17458495;
			}
		}
		else {
			if ((input[61]) >= (0.01115)) {
				var39 = -0.007889739;
			}
			else {
				var39 = -0.059780724;
			}
		}
	}
	double var40;
	if ((input[61]) >= (0.21335)) {
		if ((input[26]) >= (1.9)) {
			if ((input[70]) >= (148.5)) {
				var40 = 0.08648617;
			}
			else {
				var40 = 0.029404307;
			}
		}
		else {
			if ((input[64]) >= (48.5)) {
				var40 = -0.081103705;
			}
			else {
				var40 = 0.001998179;
			}
		}
	}
	else {
		if ((input[64]) >= (43.5)) {
			if ((input[66]) >= (5375.5)) {
				var40 = -0.02068682;
			}
			else {
				var40 = -0.08858216;
			}
		}
		else {
			if ((input[46]) >= (4.3719997)) {
				var40 = 0.0034558591;
			}
			else {
				var40 = -0.041155662;
			}
		}
	}
	double var41;
	if ((input[32]) >= (2.5)) {
		if ((input[66]) >= (493.5)) {
			if ((input[64]) >= (38.5)) {
				var41 = 0.019561762;
			}
			else {
				var41 = 0.057422746;
			}
		}
		else {
			if ((input[64]) >= (39.5)) {
				var41 = -0.04468753;
			}
			else {
				var41 = 0.0025247713;
			}
		}
	}
	else {
		if ((input[71]) >= (20938.5)) {
			if ((input[64]) >= (45.5)) {
				var41 = -0.0004365365;
			}
			else {
				var41 = 0.052747127;
			}
		}
		else {
			if ((input[64]) >= (43.5)) {
				var41 = -0.045559328;
			}
			else {
				var41 = -0.004307156;
			}
		}
	}
	double var42;
	if ((input[71]) >= (3615.5)) {
		if ((input[64]) >= (46.5)) {
			if ((input[66]) >= (18388.0)) {
				var42 = -0.019054024;
			}
			else {
				var42 = -0.08866607;
			}
		}
		else {
			if ((input[71]) >= (48912.0)) {
				var42 = 0.06111169;
			}
			else {
				var42 = 0.016649952;
			}
		}
	}
	else {
		if ((input[64]) >= (38.5)) {
			if ((input[64]) >= (43.5)) {
				var42 = -0.07096827;
			}
			else {
				var42 = -0.018274527;
			}
		}
		else {
			if ((input[71]) >= (619.5)) {
				var42 = 0.055457454;
			}
			else {
				var42 = -0.0018398302;
			}
		}
	}
	double var43;
	if ((input[46]) >= (5.69535)) {
		if ((input[42]) >= (4.5)) {
			if ((input[70]) >= (4.5)) {
				var43 = 0.009613248;
			}
			else {
				var43 = -0.024937354;
			}
		}
		else {
			if ((input[71]) >= (54425.5)) {
				var43 = 0.059071925;
			}
			else {
				var43 = 0.016042856;
			}
		}
	}
	else {
		if ((input[47]) >= (2.6111498)) {
			if ((input[70]) >= (0.5)) {
				var43 = 0.01880048;
			}
			else {
				var43 = 0.10196054;
			}
		}
		else {
			if ((input[70]) >= (2.5)) {
				var43 = -0.004230944;
			}
			else {
				var43 = -0.030439207;
			}
		}
	}
	double var44;
	if ((input[61]) >= (0.09655)) {
		if ((input[33]) >= (1.1)) {
			if ((input[70]) >= (99.5)) {
				var44 = 0.06841153;
			}
			else {
				var44 = 0.014987597;
			}
		}
		else {
			if ((input[64]) >= (48.5)) {
				var44 = -0.09112436;
			}
			else {
				var44 = -0.009730219;
			}
		}
	}
	else {
		if ((input[64]) >= (38.5)) {
			if ((input[3]) >= (0.225)) {
				var44 = -0.03254214;
			}
			else {
				var44 = 0.032473035;
			}
		}
		else {
			if ((input[71]) >= (30.5)) {
				var44 = 0.015709484;
			}
			else {
				var44 = -0.041162934;
			}
		}
	}
	double var45;
	if ((input[66]) >= (1030.5)) {
		if ((input[64]) >= (40.5)) {
			if ((input[59]) >= (1.2318499)) {
				var45 = -0.014888075;
			}
			else {
				var45 = 0.017324066;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var45 = 0.022414923;
			}
			else {
				var45 = 0.055446338;
			}
		}
	}
	else {
		if ((input[64]) >= (38.5)) {
			if ((input[3]) >= (0.225)) {
				var45 = -0.046330027;
			}
			else {
				var45 = 0.034803685;
			}
		}
		else {
			if ((input[46]) >= (4.47265)) {
				var45 = 0.008967755;
			}
			else {
				var45 = -0.04199734;
			}
		}
	}
	double var46;
	if ((input[56]) >= (0.48045)) {
		if ((input[37]) >= (0.5)) {
			if ((input[70]) >= (5.5)) {
				var46 = 0.01153855;
			}
			else {
				var46 = -0.018296296;
			}
		}
		else {
			if ((input[71]) >= (336.5)) {
				var46 = 0.018924115;
			}
			else {
				var46 = -0.004806505;
			}
		}
	}
	else {
		if ((input[47]) >= (2.6667)) {
			if ((input[50]) >= (2.5)) {
				var46 = 0.10777735;
			}
			else {
				var46 = -0.035093214;
			}
		}
		else {
			if ((input[58]) >= (0.13315)) {
				var46 = -0.03567005;
			}
			else {
				var46 = -0.07783882;
			}
		}
	}
	double var47;
	if ((input[47]) >= (1.5)) {
		if ((input[61]) >= (0.27925)) {
			if ((input[24]) >= (2.1)) {
				var47 = 0.039887868;
			}
			else {
				var47 = 0.01084314;
			}
		}
		else {
			if ((input[64]) >= (43.5)) {
				var47 = -0.026158726;
			}
			else {
				var47 = 0.005963525;
			}
		}
	}
	else {
		if ((input[66]) >= (610.5)) {
			if ((input[64]) >= (43.5)) {
				var47 = -0.023894353;
			}
			else {
				var47 = 0.0068742977;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var47 = -0.07734528;
			}
			else {
				var47 = -0.011084509;
			}
		}
	}
	double var48;
	if ((input[71]) >= (28377.0)) {
		if ((input[64]) >= (45.5)) {
			if ((input[71]) >= (110153.0)) {
				var48 = 0.05372454;
			}
			else {
				var48 = -0.004980333;
			}
		}
		else {
			if ((input[62]) >= (16.8376)) {
				var48 = 0.054494645;
			}
			else {
				var48 = -0.2033911;
			}
		}
	}
	else {
		if ((input[64]) >= (43.5)) {
			if ((input[66]) >= (4334.0)) {
				var48 = -0.01419537;
			}
			else {
				var48 = -0.07428547;
			}
		}
		else {
			if ((input[66]) >= (852.5)) {
				var48 = 0.0131240785;
			}
			else {
				var48 = -0.009236676;
			}
		}
	}
	double var49;
	if ((input[46]) >= (5.66105)) {
		if ((input[64]) >= (38.5)) {
			if ((input[57]) >= (2.41115)) {
				var49 = 0.01702303;
			}
			else {
				var49 = -0.01220901;
			}
		}
		else {
			if ((input[71]) >= (619.5)) {
				var49 = 0.05834607;
			}
			else {
				var49 = 0.014978063;
			}
		}
	}
	else {
		if ((input[60]) >= (28.326149)) {
			if ((input[70]) >= (27.5)) {
				var49 = 0.06508744;
			}
			else {
				var49 = 0.015352619;
			}
		}
		else {
			if ((input[64]) >= (43.5)) {
				var49 = -0.038807403;
			}
			else {
				var49 = -0.00803699;
			}
		}
	}
	double var50;
	if ((input[47]) >= (2.6111498)) {
		if ((input[70]) >= (0.5)) {
			if ((input[56]) >= (1287.3298)) {
				var50 = 0.045987587;
			}
			else {
				var50 = 0.012449829;
			}
		}
		else {
			if ((input[51]) >= (2.5)) {
				var50 = 0.11230701;
			}
			else {
				var50 = 0.014028621;
			}
		}
	}
	else {
		if ((input[56]) >= (0.45505)) {
			if ((input[37]) >= (0.5)) {
				var50 = -0.012289784;
			}
			else {
				var50 = 0.007744656;
			}
		}
		else {
			if ((input[70]) >= (32.5)) {
				var50 = -0.1731258;
			}
			else {
				var50 = -0.04371796;
			}
		}
	}
	double var51;
	if ((input[59]) >= (0.99944997)) {
		if ((input[71]) >= (7079.5)) {
			if ((input[64]) >= (46.5)) {
				var51 = -0.036204096;
			}
			else {
				var51 = 0.020214511;
			}
		}
		else {
			if ((input[64]) >= (40.5)) {
				var51 = -0.034320507;
			}
			else {
				var51 = -0.0013632778;
			}
		}
	}
	else {
		if ((input[71]) >= (1407.5)) {
			if ((input[0]) >= (6.0)) {
				var51 = 0.056287058;
			}
			else {
				var51 = 0.023490824;
			}
		}
		else {
			if ((input[3]) >= (0.1)) {
				var51 = 0.0026730162;
			}
			else {
				var51 = 0.05551967;
			}
		}
	}
	double var52;
	if ((input[32]) >= (2.5)) {
		if ((input[58]) >= (1.5453)) {
			if ((input[55]) >= (152.28714)) {
				var52 = -0.021975273;
			}
			else {
				var52 = 0.021975549;
			}
		}
		else {
			if ((input[70]) >= (40.5)) {
				var52 = -0.19510828;
			}
			else {
				var52 = -0.020091942;
			}
		}
	}
	else {
		if ((input[60]) >= (38.13085)) {
			if ((input[71]) >= (281.0)) {
				var52 = 0.027089853;
			}
			else {
				var52 = 0.14348073;
			}
		}
		else {
			if ((input[64]) >= (46.5)) {
				var52 = -0.047838103;
			}
			else {
				var52 = -0.00489301;
			}
		}
	}
	double var53;
	if ((input[71]) >= (60163.5)) {
		if ((input[64]) >= (48.5)) {
			if ((input[0]) >= (6.0)) {
				var53 = 0.041246574;
			}
			else {
				var53 = -0.05376748;
			}
		}
		else {
			if ((input[46]) >= (6.08325)) {
				var53 = 0.070887856;
			}
			else {
				var53 = 0.041832715;
			}
		}
	}
	else {
		if ((input[64]) >= (44.5)) {
			if ((input[66]) >= (5869.5)) {
				var53 = -0.011136579;
			}
			else {
				var53 = -0.06264457;
			}
		}
		else {
			if ((input[71]) >= (1581.5)) {
				var53 = 0.016206283;
			}
			else {
				var53 = -0.0056998464;
			}
		}
	}
	double var54;
	if ((input[71]) >= (-1072.0)) {
		if ((input[47]) >= (1.5)) {
			if ((input[59]) >= (1.53755)) {
				var54 = -0.0053184032;
			}
			else {
				var54 = 0.013094335;
			}
		}
		else {
			if ((input[66]) >= (71.5)) {
				var54 = -0.008408683;
			}
			else {
				var54 = -0.05616617;
			}
		}
	}
	else {
		if ((input[70]) >= (3.5)) {
			if ((input[47]) >= (2.72225)) {
				var54 = 0.08139812;
			}
			else {
				var54 = 0.2914774;
			}
		}
		else {
			if ((input[55]) >= (109.8633)) {
				var54 = -0.061806645;
			}
			else {
				var54 = 0.13906196;
			}
		}
	}
	double var55;
	if ((input[0]) >= (6.0)) {
		if ((input[71]) >= (670.5)) {
			if ((input[64]) >= (47.5)) {
				var55 = -0.015067081;
			}
			else {
				var55 = 0.038790695;
			}
		}
		else {
			if ((input[46]) >= (5.04835)) {
				var55 = 0.011994747;
			}
			else {
				var55 = -0.022132477;
			}
		}
	}
	else {
		if ((input[64]) >= (45.5)) {
			if ((input[66]) >= (18382.0)) {
				var55 = -0.010471765;
			}
			else {
				var55 = -0.078131676;
			}
		}
		else {
			if ((input[71]) >= (3635.5)) {
				var55 = 0.017501254;
			}
			else {
				var55 = -0.010318386;
			}
		}
	}
	double var56;
	if ((input[57]) >= (0.1814)) {
		if ((input[59]) >= (1.2671499)) {
			if ((input[64]) >= (40.5)) {
				var56 = -0.017201923;
			}
			else {
				var56 = 0.0048005795;
			}
		}
		else {
			if ((input[48]) >= (2.875)) {
				var56 = 0.047750145;
			}
			else {
				var56 = 0.008033437;
			}
		}
	}
	else {
		if ((input[66]) >= (5.5)) {
			if ((input[60]) >= (1.18555)) {
				var56 = 0.022728313;
			}
			else {
				var56 = -0.07306682;
			}
		}
		else {
			if ((input[47]) >= (2.3888502)) {
				var56 = 0.17905305;
			}
			else {
				var56 = -0.03419187;
			}
		}
	}
	double var57;
	if ((input[71]) >= (35855.5)) {
		if ((input[64]) >= (48.5)) {
			if ((input[0]) >= (6.0)) {
				var57 = 0.026730949;
			}
			else {
				var57 = -0.07304924;
			}
		}
		else {
			if ((input[64]) >= (44.5)) {
				var57 = 0.02413345;
			}
			else {
				var57 = 0.056084253;
			}
		}
	}
	else {
		if ((input[64]) >= (40.5)) {
			if ((input[66]) >= (2548.5)) {
				var57 = -0.0039742324;
			}
			else {
				var57 = -0.032174613;
			}
		}
		else {
			if ((input[66]) >= (360.5)) {
				var57 = 0.016706018;
			}
			else {
				var57 = -0.009276797;
			}
		}
	}
	double var58;
	if ((input[46]) >= (5.69535)) {
		if ((input[71]) >= (-851.5)) {
			if ((input[64]) >= (38.5)) {
				var58 = 0.0027117913;
			}
			else {
				var58 = 0.023656769;
			}
		}
		else {
			if ((input[24]) >= (1.775)) {
				var58 = 0.22735308;
			}
			else {
				var58 = 0.04908606;
			}
		}
	}
	else {
		if ((input[48]) >= (2.875)) {
			if ((input[7]) >= (4.5)) {
				var58 = 0.14089994;
			}
			else {
				var58 = 0.016782835;
			}
		}
		else {
			if ((input[59]) >= (0.74105)) {
				var58 = -0.013077306;
			}
			else {
				var58 = 0.01491666;
			}
		}
	}
	double var59;
	if ((input[71]) >= (72617.5)) {
		if ((input[3]) >= (2.325)) {
			if ((input[41]) >= (3.5)) {
				var59 = 0.041524325;
			}
			else {
				var59 = 0.07152369;
			}
		}
		else {
			if ((input[33]) >= (1.225)) {
				var59 = 0.034446757;
			}
			else {
				var59 = -0.0180343;
			}
		}
	}
	else {
		if ((input[64]) >= (47.5)) {
			if ((input[61]) >= (0.36955)) {
				var59 = -0.013704439;
			}
			else {
				var59 = -0.06415214;
			}
		}
		else {
			if ((input[71]) >= (-403.5)) {
				var59 = -0.00046545904;
			}
			else {
				var59 = 0.11497547;
			}
		}
	}
	double var60;
	if ((input[46]) >= (4.33815)) {
		if ((input[0]) >= (6.0)) {
			if ((input[71]) >= (678.5)) {
				var60 = 0.033679966;
			}
			else {
				var60 = 0.0065536;
			}
		}
		else {
			if ((input[64]) >= (45.5)) {
				var60 = -0.028115874;
			}
			else {
				var60 = 0.0013027607;
			}
		}
	}
	else {
		if ((input[66]) >= (992.5)) {
			if ((input[59]) >= (0.4334)) {
				var60 = -0.0075593293;
			}
			else {
				var60 = 0.071741864;
			}
		}
		else {
			if ((input[33]) >= (0.1)) {
				var60 = -0.04820945;
			}
			else {
				var60 = 0.03325013;
			}
		}
	}
	double var61;
	if ((input[71]) >= (28367.5)) {
		if ((input[64]) >= (48.5)) {
			if ((input[0]) >= (6.0)) {
				var61 = 0.02268834;
			}
			else {
				var61 = -0.06714887;
			}
		}
		else {
			if ((input[64]) >= (43.5)) {
				var61 = 0.019090299;
			}
			else {
				var61 = 0.05151535;
			}
		}
	}
	else {
		if ((input[64]) >= (40.5)) {
			if ((input[66]) >= (2548.5)) {
				var61 = -0.0038064085;
			}
			else {
				var61 = -0.030081084;
			}
		}
		else {
			if ((input[66]) >= (492.5)) {
				var61 = 0.016359365;
			}
			else {
				var61 = -0.006533687;
			}
		}
	}
	double var62;
	if ((input[55]) >= (148.09964)) {
		if ((input[1]) >= (0.009749999)) {
			if ((input[46]) >= (6.0667)) {
				var62 = 0.0028646467;
			}
			else {
				var62 = -0.04387805;
			}
		}
		else {
			if ((input[0]) >= (1.5)) {
				var62 = -0.019151606;
			}
			else {
				var62 = 0.13534848;
			}
		}
	}
	else {
		if ((input[56]) >= (0.1743)) {
			if ((input[26]) >= (1.9)) {
				var62 = 0.008828284;
			}
			else {
				var62 = -0.0062468317;
			}
		}
		else {
			if ((input[51]) >= (1.5)) {
				var62 = -0.016889526;
			}
			else {
				var62 = -0.076849826;
			}
		}
	}
	double var63;
	if ((input[59]) >= (0.99944997)) {
		if ((input[71]) >= (10981.5)) {
			if ((input[64]) >= (43.5)) {
				var63 = -0.00017150368;
			}
			else {
				var63 = 0.03341832;
			}
		}
		else {
			if ((input[64]) >= (40.5)) {
				var63 = -0.023687484;
			}
			else {
				var63 = -0.0004951669;
			}
		}
	}
	else {
		if ((input[60]) >= (40.05665)) {
			if ((input[71]) >= (227.0)) {
				var63 = 0.035407368;
			}
			else {
				var63 = 0.14205788;
			}
		}
		else {
			if ((input[3]) >= (0.1)) {
				var63 = 0.006465412;
			}
			else {
				var63 = 0.046972945;
			}
		}
	}
	double var64;
	if ((input[71]) >= (-195.5)) {
		if ((input[0]) >= (6.0)) {
			if ((input[71]) >= (1161.5)) {
				var64 = 0.03278542;
			}
			else {
				var64 = 0.0020950416;
			}
		}
		else {
			if ((input[47]) >= (2.8333502)) {
				var64 = 0.018804727;
			}
			else {
				var64 = -0.0076330053;
			}
		}
	}
	else {
		if ((input[70]) >= (13.5)) {
			if ((input[63]) >= (0.8133)) {
				var64 = 0.29496568;
			}
			else {
				var64 = -0.0047913315;
			}
		}
		else {
			if ((input[24]) >= (1.9)) {
				var64 = 0.11250466;
			}
			else {
				var64 = -0.0035270595;
			}
		}
	}
	double var65;
	if ((input[71]) >= (65967.0)) {
		if ((input[64]) >= (48.5)) {
			if ((input[71]) >= (130441.5)) {
				var65 = 0.036151066;
			}
			else {
				var65 = -0.03827356;
			}
		}
		else {
			if ((input[71]) >= (111920.5)) {
				var65 = 0.060645618;
			}
			else {
				var65 = 0.032247793;
			}
		}
	}
	else {
		if ((input[64]) >= (47.5)) {
			if ((input[66]) >= (16440.0)) {
				var65 = -0.027376214;
			}
			else {
				var65 = -0.075358324;
			}
		}
		else {
			if ((input[60]) >= (0.06835)) {
				var65 = 0.0008344142;
			}
			else {
				var65 = -0.05512007;
			}
		}
	}
	double var66;
	if ((input[46]) >= (5.69535)) {
		if ((input[33]) >= (2.29165)) {
			if ((input[57]) >= (2.5327501)) {
				var66 = 0.036976926;
			}
			else {
				var66 = 0.012694605;
			}
		}
		else {
			if ((input[71]) >= (74846.0)) {
				var66 = 0.044309985;
			}
			else {
				var66 = 0.0016802481;
			}
		}
	}
	else {
		if ((input[59]) >= (0.72975004)) {
			if ((input[71]) >= (5480.5)) {
				var66 = 0.005737943;
			}
			else {
				var66 = -0.014144167;
			}
		}
		else {
			if ((input[55]) >= (81.8164)) {
				var66 = 0.03254611;
			}
			else {
				var66 = 0.0020407971;
			}
		}
	}
	double var67;
	if ((input[55]) >= (145.4668)) {
		if ((input[1]) >= (0.009749999)) {
			if ((input[70]) >= (65.5)) {
				var67 = 0.034208607;
			}
			else {
				var67 = -0.0338736;
			}
		}
		else {
			if ((input[0]) >= (1.5)) {
				var67 = -0.019279392;
			}
			else {
				var67 = 0.13064173;
			}
		}
	}
	else {
		if ((input[46]) >= (4.33815)) {
			if ((input[0]) >= (6.0)) {
				var67 = 0.017261025;
			}
			else {
				var67 = -0.00017738299;
			}
		}
		else {
			if ((input[66]) >= (992.5)) {
				var67 = 0.004667949;
			}
			else {
				var67 = -0.03486861;
			}
		}
	}
	double var68;
	if ((input[47]) >= (1.5)) {
		if ((input[55]) >= (128.82614)) {
			if ((input[1]) >= (0.009749999)) {
				var68 = -0.01738137;
			}
			else {
				var68 = 0.094762556;
			}
		}
		else {
			if ((input[71]) >= (-303.5)) {
				var68 = 0.007013628;
			}
			else {
				var68 = 0.09328636;
			}
		}
	}
	else {
		if ((input[66]) >= (71.5)) {
			if ((input[42]) >= (3.5)) {
				var68 = -0.019831778;
			}
			else {
				var68 = 0.0014469305;
			}
		}
		else {
			if ((input[58]) >= (244.87735)) {
				var68 = 0.11769473;
			}
			else {
				var68 = -0.05068132;
			}
		}
	}
	double var69;
	if ((input[60]) >= (38.13085)) {
		if ((input[12]) >= (2.98635)) {
			if ((input[71]) >= (281.0)) {
				var69 = 0.028618569;
			}
			else {
				var69 = 0.12748572;
			}
		}
		else {
			if ((input[49]) >= (0.125)) {
				var69 = 0.006696841;
			}
			else {
				var69 = -0.046322305;
			}
		}
	}
	else {
		if ((input[64]) >= (38.5)) {
			if ((input[66]) >= (836.5)) {
				var69 = -0.0015240567;
			}
			else {
				var69 = -0.029575441;
			}
		}
		else {
			if ((input[71]) >= (697.5)) {
				var69 = 0.04027514;
			}
			else {
				var69 = 0.0008978721;
			}
		}
	}
	double var70;
	if ((input[71]) >= (-195.5)) {
		if ((input[71]) >= (2766.5)) {
			if ((input[64]) >= (46.5)) {
				var70 = -0.021642182;
			}
			else {
				var70 = 0.011116864;
			}
		}
		else {
			if ((input[64]) >= (43.5)) {
				var70 = -0.065821804;
			}
			else {
				var70 = -0.0028451313;
			}
		}
	}
	else {
		if ((input[70]) >= (14.5)) {
			if ((input[63]) >= (0.8433)) {
				var70 = 0.23816982;
			}
			else {
				var70 = 0.036014847;
			}
		}
		else {
			if ((input[13]) >= (2.5)) {
				var70 = 0.109319866;
			}
			else {
				var70 = 0.00458566;
			}
		}
	}
	double var71;
	if ((input[59]) >= (0.92885)) {
		if ((input[71]) >= (60163.5)) {
			if ((input[64]) >= (48.5)) {
				var71 = -0.016094929;
			}
			else {
				var71 = 0.036246713;
			}
		}
		else {
			if ((input[64]) >= (43.5)) {
				var71 = -0.019683793;
			}
			else {
				var71 = -0.00081832794;
			}
		}
	}
	else {
		if ((input[55]) >= (81.78125)) {
			if ((input[7]) >= (4.5)) {
				var71 = 0.08561554;
			}
			else {
				var71 = 0.015803315;
			}
		}
		else {
			if ((input[71]) >= (34.5)) {
				var71 = 0.010382846;
			}
			else {
				var71 = -0.043068822;
			}
		}
	}
	double var72;
	if ((input[32]) >= (2.5)) {
		if ((input[58]) >= (1.5453)) {
			if ((input[70]) >= (206.5)) {
				var72 = 0.070138104;
			}
			else {
				var72 = 0.012959619;
			}
		}
		else {
			if ((input[70]) >= (27.5)) {
				var72 = -0.15706104;
			}
			else {
				var72 = -0.016626319;
			}
		}
	}
	else {
		if ((input[37]) >= (0.5)) {
			if ((input[48]) >= (2.875)) {
				var72 = 0.03308138;
			}
			else {
				var72 = -0.013929474;
			}
		}
		else {
			if ((input[57]) >= (0.29225)) {
				var72 = 0.0041407975;
			}
			else {
				var72 = -0.056477744;
			}
		}
	}
	double var73;
	if ((input[46]) >= (4.7859497)) {
		if ((input[64]) >= (38.5)) {
			if ((input[66]) >= (688.5)) {
				var73 = 0.0013343276;
			}
			else {
				var73 = -0.028769432;
			}
		}
		else {
			if ((input[71]) >= (789.5)) {
				var73 = 0.04151399;
			}
			else {
				var73 = 0.0071547776;
			}
		}
	}
	else {
		if ((input[60]) >= (39.998047)) {
			if ((input[12]) >= (15.722651)) {
				var73 = 0.06801583;
			}
			else {
				var73 = 0.002264421;
			}
		}
		else {
			if ((input[3]) >= (0.55)) {
				var73 = -0.019908814;
			}
			else {
				var73 = 0.021516768;
			}
		}
	}
	double var74;
	if ((input[71]) >= (-196.5)) {
		if ((input[71]) >= (1581.5)) {
			if ((input[64]) >= (38.5)) {
				var74 = 0.0031616767;
			}
			else {
				var74 = 0.046973698;
			}
		}
		else {
			if ((input[64]) >= (43.5)) {
				var74 = -0.06878021;
			}
			else {
				var74 = -0.0040954463;
			}
		}
	}
	else {
		if ((input[70]) >= (3.5)) {
			if ((input[70]) >= (16.5)) {
				var74 = 0.20328966;
			}
			else {
				var74 = 0.0853655;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var74 = -0.012182622;
			}
			else {
				var74 = 0.3351379;
			}
		}
	}
	double var75;
	if ((input[55]) >= (151.02539)) {
		if ((input[1]) >= (0.00585)) {
			if ((input[46]) >= (6.0851)) {
				var75 = 0.0051677483;
			}
			else {
				var75 = -0.036597352;
			}
		}
		else {
			if ((input[0]) >= (1.5)) {
				var75 = -0.021385787;
			}
			else {
				var75 = 0.12256948;
			}
		}
	}
	else {
		if ((input[33]) >= (2.36665)) {
			if ((input[60]) >= (1.83395)) {
				var75 = 0.019607166;
			}
			else {
				var75 = -0.0069811917;
			}
		}
		else {
			if ((input[71]) >= (-853.5)) {
				var75 = -0.0018500983;
			}
			else {
				var75 = 0.09687808;
			}
		}
	}
	double var76;
	if ((input[59]) >= (0.75265)) {
		if ((input[71]) >= (14506.5)) {
			if ((input[64]) >= (43.5)) {
				var76 = 0.0023014816;
			}
			else {
				var76 = 0.03313642;
			}
		}
		else {
			if ((input[64]) >= (39.5)) {
				var76 = -0.013356447;
			}
			else {
				var76 = 0.0034737797;
			}
		}
	}
	else {
		if ((input[55]) >= (74.541046)) {
			if ((input[7]) >= (4.5)) {
				var76 = 0.08413984;
			}
			else {
				var76 = 0.020517828;
			}
		}
		else {
			if ((input[66]) >= (66.5)) {
				var76 = 0.009690445;
			}
			else {
				var76 = -0.05258736;
			}
		}
	}
	double var77;
	if ((input[62]) >= (0.079050004)) {
		if ((input[59]) >= (1.29245)) {
			if ((input[64]) >= (40.5)) {
				var77 = -0.011700933;
			}
			else {
				var77 = 0.0024400454;
			}
		}
		else {
			if ((input[46]) >= (6.05895)) {
				var77 = 0.02085056;
			}
			else {
				var77 = 0.0027818864;
			}
		}
	}
	else {
		if ((input[66]) >= (5.5)) {
			if ((input[47]) >= (2.6667)) {
				var77 = 0.017412504;
			}
			else {
				var77 = -0.07178694;
			}
		}
		else {
			if ((input[47]) >= (2.27775)) {
				var77 = 0.21263412;
			}
			else {
				var77 = -0.016623463;
			}
		}
	}
	double var78;
	if ((input[61]) >= (0.54105)) {
		if ((input[24]) >= (2.1)) {
			if ((input[60]) >= (1.4824)) {
				var78 = 0.037311535;
			}
			else {
				var78 = -0.107449524;
			}
		}
		else {
			if ((input[60]) >= (80.4922)) {
				var78 = 0.14775804;
			}
			else {
				var78 = 0.009487211;
			}
		}
	}
	else {
		if ((input[46]) >= (3.4942)) {
			if ((input[64]) >= (48.5)) {
				var78 = -0.047543045;
			}
			else {
				var78 = 0.00041388426;
			}
		}
		else {
			if ((input[66]) >= (14.5)) {
				var78 = -0.04122474;
			}
			else {
				var78 = 0.06113263;
			}
		}
	}
	double var79;
	if ((input[71]) >= (72617.5)) {
		if ((input[3]) >= (2.325)) {
			if ((input[66]) >= (179411.0)) {
				var79 = 0.077052265;
			}
			else {
				var79 = 0.03436008;
			}
		}
		else {
			if ((input[57]) >= (1.5543001)) {
				var79 = 0.007897445;
			}
			else {
				var79 = -0.1387677;
			}
		}
	}
	else {
		if ((input[64]) >= (47.5)) {
			if ((input[66]) >= (16440.0)) {
				var79 = -0.020538135;
			}
			else {
				var79 = -0.06812944;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var79 = 0.009925379;
			}
			else {
				var79 = -0.0030651612;
			}
		}
	}
	double var80;
	if ((input[47]) >= (2.1666498)) {
		if ((input[3]) >= (0.325)) {
			if ((input[55]) >= (125.3984)) {
				var80 = -0.009761991;
			}
			else {
				var80 = 0.00942286;
			}
		}
		else {
			if ((input[33]) >= (0.225)) {
				var80 = -0.022398083;
			}
			else {
				var80 = 0.15460995;
			}
		}
	}
	else {
		if ((input[48]) >= (2.70835)) {
			if ((input[17]) >= (4.5)) {
				var80 = 0.14767918;
			}
			else {
				var80 = 0.024074463;
			}
		}
		else {
			if ((input[66]) >= (83.5)) {
				var80 = -0.0027768647;
			}
			else {
				var80 = -0.037788775;
			}
		}
	}
	double var81;
	if ((input[71]) >= (-195.5)) {
		if ((input[71]) >= (3402.5)) {
			if ((input[62]) >= (1.6794)) {
				var81 = 0.0074754576;
			}
			else {
				var81 = -0.14543179;
			}
		}
		else {
			if ((input[64]) >= (43.5)) {
				var81 = -0.053026795;
			}
			else {
				var81 = -0.0023700248;
			}
		}
	}
	else {
		if ((input[70]) >= (3.5)) {
			if ((input[24]) >= (1.775)) {
				var81 = 0.12295492;
			}
			else {
				var81 = 0.0447604;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var81 = -0.009344115;
			}
			else {
				var81 = 0.27291974;
			}
		}
	}
	double var82;
	if ((input[0]) >= (6.0)) {
		if ((input[71]) >= (1161.5)) {
			if ((input[57]) >= (2.9383998)) {
				var82 = 0.043571662;
			}
			else {
				var82 = 0.016972497;
			}
		}
		else {
			if ((input[66]) >= (57.5)) {
				var82 = 0.005587516;
			}
			else {
				var82 = -0.02323263;
			}
		}
	}
	else {
		if ((input[64]) >= (45.5)) {
			if ((input[66]) >= (16847.5)) {
				var82 = -0.008886735;
			}
			else {
				var82 = -0.06656099;
			}
		}
		else {
			if ((input[71]) >= (3468.5)) {
				var82 = 0.011769592;
			}
			else {
				var82 = -0.0061970702;
			}
		}
	}
	double var83;
	if ((input[22]) >= (1.81055)) {
		if ((input[60]) >= (2.33395)) {
			if ((input[33]) >= (1.1)) {
				var83 = 0.0055513415;
			}
			else {
				var83 = -0.010749525;
			}
		}
		else {
			if ((input[70]) >= (40.5)) {
				var83 = -0.12837102;
			}
			else {
				var83 = -0.01779529;
			}
		}
	}
	else {
		if ((input[71]) >= (30.5)) {
			if ((input[58]) >= (18.614601)) {
				var83 = -0.0015761944;
			}
			else {
				var83 = 0.025458751;
			}
		}
		else {
			if ((input[33]) >= (0.45)) {
				var83 = -0.03449723;
			}
			else {
				var83 = 0.09061351;
			}
		}
	}
	double var84;
	if ((input[55]) >= (141.75975)) {
		if ((input[1]) >= (0.009749999)) {
			if ((input[46]) >= (6.06885)) {
				var84 = 0.009566024;
			}
			else {
				var84 = -0.027952785;
			}
		}
		else {
			if ((input[0]) >= (1.5)) {
				var84 = -0.016679427;
			}
			else {
				var84 = 0.12276136;
			}
		}
	}
	else {
		if ((input[66]) >= (1030.5)) {
			if ((input[71]) >= (293.5)) {
				var84 = 0.005088784;
			}
			else {
				var84 = 0.08686792;
			}
		}
		else {
			if ((input[64]) >= (39.5)) {
				var84 = -0.027649362;
			}
			else {
				var84 = 0.0023772793;
			}
		}
	}
	double var85;
	if ((input[71]) >= (-185.5)) {
		if ((input[59]) >= (0.75265)) {
			if ((input[46]) >= (3.91945)) {
				var85 = -0.0010976582;
			}
			else {
				var85 = -0.027109116;
			}
		}
		else {
			if ((input[55]) >= (81.8164)) {
				var85 = 0.025110915;
			}
			else {
				var85 = 0.0049652695;
			}
		}
	}
	else {
		if ((input[3]) >= (1.9)) {
			if ((input[47]) >= (2.72225)) {
				var85 = -0.003926065;
			}
			else {
				var85 = 0.105997935;
			}
		}
		else {
			if ((input[70]) >= (12.5)) {
				var85 = 0.117314875;
			}
			else {
				var85 = -0.024018785;
			}
		}
	}
	double var86;
	if ((input[71]) >= (102869.0)) {
		if ((input[57]) >= (1.6348)) {
			if ((input[71]) >= (163465.5)) {
				var86 = 0.06451452;
			}
			else {
				var86 = 0.030738039;
			}
		}
		else {
			if ((input[48]) >= (1.625)) {
				var86 = 0.009658875;
			}
			else {
				var86 = -0.1664086;
			}
		}
	}
	else {
		if ((input[64]) >= (48.5)) {
			if ((input[0]) >= (6.0)) {
				var86 = -0.014075448;
			}
			else {
				var86 = -0.09438407;
			}
		}
		else {
			if ((input[60]) >= (46.232452)) {
				var86 = 0.030969331;
			}
			else {
				var86 = -0.0008079844;
			}
		}
	}
	double var87;
	if ((input[46]) >= (6.06135)) {
		if ((input[71]) >= (51548.5)) {
			if ((input[64]) >= (48.5)) {
				var87 = -0.017262232;
			}
			else {
				var87 = 0.049148247;
			}
		}
		else {
			if ((input[59]) >= (1.35535)) {
				var87 = -0.00083810923;
			}
			else {
				var87 = 0.01478871;
			}
		}
	}
	else {
		if ((input[55]) >= (137.02536)) {
			if ((input[1]) >= (0.009749999)) {
				var87 = -0.023992922;
			}
			else {
				var87 = 0.0656881;
			}
		}
		else {
			if ((input[47]) >= (1.5)) {
				var87 = 0.0031638287;
			}
			else {
				var87 = -0.008082518;
			}
		}
	}
	double var88;
	if ((input[22]) >= (2.82225)) {
		if ((input[60]) >= (2.10355)) {
			if ((input[70]) >= (162.5)) {
				var88 = 0.053288307;
			}
			else {
				var88 = -0.00014135188;
			}
		}
		else {
			if ((input[70]) >= (40.5)) {
				var88 = -0.11486816;
			}
			else {
				var88 = -0.020564973;
			}
		}
	}
	else {
		if ((input[71]) >= (27.5)) {
			if ((input[64]) >= (38.5)) {
				var88 = -0.0023725096;
			}
			else {
				var88 = 0.021637844;
			}
		}
		else {
			if ((input[32]) >= (0.5)) {
				var88 = -0.032327637;
			}
			else {
				var88 = 0.07998014;
			}
		}
	}
	double var89;
	if ((input[61]) >= (0.60675)) {
		if ((input[12]) >= (0.017549999)) {
			if ((input[58]) >= (347.787)) {
				var89 = 0.016961807;
			}
			else {
				var89 = 0.050649296;
			}
		}
		else {
			if ((input[16]) >= (4.95)) {
				var89 = -0.022694139;
			}
			else {
				var89 = -0.12367099;
			}
		}
	}
	else {
		if ((input[64]) >= (40.5)) {
			if ((input[66]) >= (2549.5)) {
				var89 = -0.0013844733;
			}
			else {
				var89 = -0.02296675;
			}
		}
		else {
			if ((input[66]) >= (492.5)) {
				var89 = 0.012426293;
			}
			else {
				var89 = -0.0040318314;
			}
		}
	}
	double var90;
	if ((input[71]) >= (-91.5)) {
		if ((input[22]) >= (2.82225)) {
			if ((input[60]) >= (2.10355)) {
				var90 = -0.00010428244;
			}
			else {
				var90 = -0.023254229;
			}
		}
		else {
			if ((input[71]) >= (27.5)) {
				var90 = 0.01153209;
			}
			else {
				var90 = -0.015079953;
			}
		}
	}
	else {
		if ((input[70]) >= (13.5)) {
			if ((input[65]) >= (40.5)) {
				var90 = 0.025622282;
			}
			else {
				var90 = 0.17113334;
			}
		}
		else {
			if ((input[24]) >= (2.29165)) {
				var90 = 0.07867477;
			}
			else {
				var90 = 0.0030669607;
			}
		}
	}
	double var91;
	if ((input[0]) >= (6.0)) {
		if ((input[1]) >= (0.041050002)) {
			if ((input[71]) >= (1151.5)) {
				var91 = 0.024469603;
			}
			else {
				var91 = 0.006260421;
			}
		}
		else {
			if ((input[70]) >= (4.5)) {
				var91 = 0.018124446;
			}
			else {
				var91 = -0.023399768;
			}
		}
	}
	else {
		if ((input[64]) >= (45.5)) {
			if ((input[66]) >= (17717.0)) {
				var91 = -0.008030253;
			}
			else {
				var91 = -0.060579043;
			}
		}
		else {
			if ((input[71]) >= (4606.5)) {
				var91 = 0.011368301;
			}
			else {
				var91 = -0.00475656;
			}
		}
	}
	double var92;
	if ((input[59]) >= (0.42075002)) {
		if ((input[46]) >= (3.4852)) {
			if ((input[47]) >= (2.6111498)) {
				var92 = 0.013955916;
			}
			else {
				var92 = -0.0016495489;
			}
		}
		else {
			if ((input[66]) >= (5.5)) {
				var92 = -0.032896176;
			}
			else {
				var92 = 0.076612614;
			}
		}
	}
	else {
		if ((input[63]) >= (0.96445)) {
			if ((input[70]) >= (11.5)) {
				var92 = 0.075864576;
			}
			else {
				var92 = 0.0227255;
			}
		}
		else {
			if ((input[22]) >= (11.738251)) {
				var92 = -0.034485117;
			}
			else {
				var92 = 0.020407366;
			}
		}
	}
	double var93;
	if ((input[64]) >= (48.5)) {
		if ((input[0]) >= (6.0)) {
			if ((input[66]) >= (16517.0)) {
				var93 = 0.012959923;
			}
			else {
				var93 = -0.06534452;
			}
		}
		else {
			if ((input[66]) >= (77073.0)) {
				var93 = -0.012452202;
			}
			else {
				var93 = -0.1015501;
			}
		}
	}
	else {
		if ((input[71]) >= (48838.5)) {
			if ((input[46]) >= (6.0811)) {
				var93 = 0.04515763;
			}
			else {
				var93 = 0.016733855;
			}
		}
		else {
			if ((input[13]) >= (1.5)) {
				var93 = -0.0033418895;
			}
			else {
				var93 = 0.00961828;
			}
		}
	}
	double var94;
	if ((input[71]) >= (-60.5)) {
		if ((input[22]) >= (3.31445)) {
			if ((input[66]) >= (2064.5)) {
				var94 = 0.0030293148;
			}
			else {
				var94 = -0.012756313;
			}
		}
		else {
			if ((input[71]) >= (27.5)) {
				var94 = 0.009890842;
			}
			else {
				var94 = -0.013686399;
			}
		}
	}
	else {
		if ((input[70]) >= (14.5)) {
			if ((input[64]) >= (39.5)) {
				var94 = 0.093140654;
			}
			else {
				var94 = 0.2140063;
			}
		}
		else {
			if ((input[58]) >= (288.16394)) {
				var94 = 0.08870694;
			}
			else {
				var94 = 0.006366582;
			}
		}
	}
	double var95;
	if ((input[64]) >= (40.5)) {
		if ((input[66]) >= (1489.5)) {
			if ((input[0]) >= (6.0)) {
				var95 = 0.021577498;
			}
			else {
				var95 = -0.0053787394;
			}
		}
		else {
			if ((input[3]) >= (0.1)) {
				var95 = -0.035278283;
			}
			else {
				var95 = 0.047504988;
			}
		}
	}
	else {
		if ((input[58]) >= (1761.8065)) {
			if ((input[63]) >= (0.92175)) {
				var95 = 0.032870106;
			}
			else {
				var95 = 0.0669628;
			}
		}
		else {
			if ((input[1]) >= (10.849649)) {
				var95 = -0.011390491;
			}
			else {
				var95 = 0.0062666717;
			}
		}
	}
	double var96;
	if ((input[55]) >= (145.46286)) {
		if ((input[1]) >= (0.009749999)) {
			if ((input[57]) >= (2.50405)) {
				var96 = 0.00032577576;
			}
			else {
				var96 = -0.028633922;
			}
		}
		else {
			if ((input[0]) >= (1.5)) {
				var96 = -0.022485698;
			}
			else {
				var96 = 0.1185084;
			}
		}
	}
	else {
		if ((input[26]) >= (1.9)) {
			if ((input[61]) >= (0.026749998)) {
				var96 = 0.008585344;
			}
			else {
				var96 = -0.00938259;
			}
		}
		else {
			if ((input[13]) >= (0.5)) {
				var96 = -0.0069316323;
			}
			else {
				var96 = 0.026290078;
			}
		}
	}
	double var97;
	if ((input[64]) >= (48.5)) {
		if ((input[71]) >= (130441.5)) {
			if ((input[58]) >= (1766.3358)) {
				var97 = 0.0055564847;
			}
			else {
				var97 = 0.10917213;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var97 = -0.015745003;
			}
			else {
				var97 = -0.077343866;
			}
		}
	}
	else {
		if ((input[71]) >= (40680.0)) {
			if ((input[56]) >= (39.90805)) {
				var97 = 0.020953907;
			}
			else {
				var97 = -0.14410566;
			}
		}
		else {
			if ((input[64]) >= (43.5)) {
				var97 = -0.011215822;
			}
			else {
				var97 = 0.0015765246;
			}
		}
	}
	double var98;
	if ((input[59]) >= (0.32265002)) {
		if ((input[46]) >= (3.4852)) {
			if ((input[71]) >= (-1072.0)) {
				var98 = -0.000015798372;
			}
			else {
				var98 = 0.07584636;
			}
		}
		else {
			if ((input[70]) >= (0.5)) {
				var98 = -0.03193381;
			}
			else {
				var98 = 0.039668113;
			}
		}
	}
	else {
		if ((input[63]) >= (0.96445)) {
			if ((input[70]) >= (10.5)) {
				var98 = 0.075801134;
			}
			else {
				var98 = 0.02911496;
			}
		}
		else {
			if ((input[22]) >= (34.9629)) {
				var98 = -0.091152415;
			}
			else {
				var98 = 0.011614063;
			}
		}
	}
	double var99;
	if ((input[61]) >= (0.60675)) {
		if ((input[12]) >= (0.017549999)) {
			if ((input[61]) >= (0.9497)) {
				var99 = 0.04819263;
			}
			else {
				var99 = 0.015772179;
			}
		}
		else {
			if ((input[16]) >= (4.95)) {
				var99 = -0.016686296;
			}
			else {
				var99 = -0.114992104;
			}
		}
	}
	else {
		if ((input[64]) >= (40.5)) {
			if ((input[12]) >= (0.00195)) {
				var99 = -0.0076897903;
			}
			else {
				var99 = 0.044117093;
			}
		}
		else {
			if ((input[66]) >= (360.5)) {
				var99 = 0.009868675;
			}
			else {
				var99 = -0.0046783634;
			}
		}
	}
	double var100;
	var100 = (1.0) / ((1.0) + (exp((0.0) - ((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((var0)+(var1)) + (var2)) + (var3)) + (var4)) + (var5)) + (var6)) + (var7)) + (var8)) + (var9)) + (var10)) + (var11)) + (var12)) + (var13)) + (var14)) + (var15)) + (var16)) + (var17)) + (var18)) + (var19)) + (var20)) + (var21)) + (var22)) + (var23)) + (var24)) + (var25)) + (var26)) + (var27)) + (var28)) + (var29)) + (var30)) + (var31)) + (var32)) + (var33)) + (var34)) + (var35)) + (var36)) + (var37)) + (var38)) + (var39)) + (var40)) + (var41)) + (var42)) + (var43)) + (var44)) + (var45)) + (var46)) + (var47)) + (var48)) + (var49)) + (var50)) + (var51)) + (var52)) + (var53)) + (var54)) + (var55)) + (var56)) + (var57)) + (var58)) + (var59)) + (var60)) + (var61)) + (var62)) + (var63)) + (var64)) + (var65)) + (var66)) + (var67)) + (var68)) + (var69)) + (var70)) + (var71)) + (var72)) + (var73)) + (var74)) + (var75)) + (var76)) + (var77)) + (var78)) + (var79)) + (var80)) + (var81)) + (var82)) + (var83)) + (var84)) + (var85)) + (var86)) + (var87)) + (var88)) + (var89)) + (var90)) + (var91)) + (var92)) + (var93)) + (var94)) + (var95)) + (var96)) + (var97)) + (var98)) + (var99)))));
	*output = var100 < 0.5 ? 0 : 1;
}

void PUclf1(double * input, double * output) {
	double var0;
	if ((input[60]) >= (1.5)) {
		if ((input[39]) >= (0.58335)) {
			if ((input[39]) >= (5.29165)) {
				var0 = -0.19702123;
			}
			else {
				var0 = -0.18153328;
			}
		}
		else {
			if ((input[1]) >= (0.02315)) {
				var0 = -0.15359019;
			}
			else {
				var0 = -0.03068783;
			}
		}
	}
	else {
		if ((input[12]) >= (0.18725)) {
			if ((input[41]) >= (3.5)) {
				var0 = 0.073127754;
			}
			else {
				var0 = -0.09170014;
			}
		}
		else {
			if ((input[39]) >= (0.58335)) {
				var0 = -0.03535109;
			}
			else {
				var0 = 0.18990888;
			}
		}
	}
	double var1;
	if ((input[60]) >= (1.5)) {
		if ((input[39]) >= (0.58335)) {
			if ((input[41]) >= (1.5)) {
				var1 = -0.16258012;
			}
			else {
				var1 = -0.17798631;
			}
		}
		else {
			if ((input[1]) >= (0.02315)) {
				var1 = -0.13909109;
			}
			else {
				var1 = -0.027658194;
			}
		}
	}
	else {
		if ((input[1]) >= (0.198)) {
			if ((input[41]) >= (3.5)) {
				var1 = 0.064888924;
			}
			else {
				var1 = -0.08424126;
			}
		}
		else {
			if ((input[39]) >= (0.125)) {
				var1 = -0.010182413;
			}
			else {
				var1 = 0.17248175;
			}
		}
	}
	double var2;
	if ((input[60]) >= (1.5)) {
		if ((input[39]) >= (1.41665)) {
			if ((input[41]) >= (1.5)) {
				var2 = -0.14987326;
			}
			else {
				var2 = -0.16452701;
			}
		}
		else {
			if ((input[57]) >= (0.5)) {
				var2 = -0.09535327;
			}
			else {
				var2 = -0.15038405;
			}
		}
	}
	else {
		if ((input[12]) >= (0.12145)) {
			if ((input[54]) >= (44.5)) {
				var2 = 0.08457405;
			}
			else {
				var2 = -0.05974701;
			}
		}
		else {
			if ((input[39]) >= (0.58335)) {
				var2 = -0.024181083;
			}
			else {
				var2 = 0.16136762;
			}
		}
	}
	double var3;
	if ((input[60]) >= (1.5)) {
		if ((input[45]) >= (35.1481)) {
			if ((input[41]) >= (2.5)) {
				var3 = -0.11891409;
			}
			else {
				var3 = -0.15080993;
			}
		}
		else {
			if ((input[12]) >= (0.0028)) {
				var3 = -0.0822341;
			}
			else {
				var3 = 0.10219727;
			}
		}
	}
	else {
		if ((input[1]) >= (0.25744998)) {
			if ((input[39]) >= (0.125)) {
				var3 = -0.11261542;
			}
			else {
				var3 = 0.03240928;
			}
		}
		else {
			if ((input[12]) >= (0.037699997)) {
				var3 = 0.07052209;
			}
			else {
				var3 = 0.1539015;
			}
		}
	}
	double var4;
	if ((input[60]) >= (1.5)) {
		if ((input[39]) >= (0.58335)) {
			if ((input[39]) >= (5.29165)) {
				var4 = -0.14685401;
			}
			else {
				var4 = -0.13237752;
			}
		}
		else {
			if ((input[1]) >= (0.02315)) {
				var4 = -0.108630076;
			}
			else {
				var4 = -0.008436494;
			}
		}
	}
	else {
		if ((input[12]) >= (0.29065)) {
			if ((input[54]) >= (44.5)) {
				var4 = 0.06507673;
			}
			else {
				var4 = -0.066691935;
			}
		}
		else {
			if ((input[39]) >= (0.58335)) {
				var4 = -0.0290993;
			}
			else {
				var4 = 0.13927323;
			}
		}
	}
	double var5;
	if ((input[60]) >= (1.5)) {
		if ((input[39]) >= (1.41665)) {
			if ((input[39]) >= (5.29165)) {
				var5 = -0.13982342;
			}
			else {
				var5 = -0.12512097;
			}
		}
		else {
			if ((input[60]) >= (8.5)) {
				var5 = -0.12216171;
			}
			else {
				var5 = -0.06791304;
			}
		}
	}
	else {
		if ((input[1]) >= (0.25744998)) {
			if ((input[41]) >= (3.5)) {
				var5 = 0.047075056;
			}
			else {
				var5 = -0.07304568;
			}
		}
		else {
			if ((input[13]) >= (0.5)) {
				var5 = 0.052485324;
			}
			else {
				var5 = 0.13781814;
			}
		}
	}
	double var6;
	if ((input[60]) >= (1.5)) {
		if ((input[41]) >= (2.5)) {
			if ((input[1]) >= (0.0443)) {
				var6 = -0.094542556;
			}
			else {
				var6 = 0.012744235;
			}
		}
		else {
			if ((input[45]) >= (30.9674)) {
				var6 = -0.12899101;
			}
			else {
				var6 = -0.057593714;
			}
		}
	}
	else {
		if ((input[12]) >= (0.12145)) {
			if ((input[2]) >= (0.5)) {
				var6 = -0.071923785;
			}
			else {
				var6 = 0.050583597;
			}
		}
		else {
			if ((input[39]) >= (0.125)) {
				var6 = -0.018416286;
			}
			else {
				var6 = 0.12950656;
			}
		}
	}
	double var7;
	if ((input[60]) >= (1.5)) {
		if ((input[39]) >= (0.58335)) {
			if ((input[39]) >= (5.29165)) {
				var7 = -0.12918642;
			}
			else {
				var7 = -0.112807356;
			}
		}
		else {
			if ((input[45]) >= (39.10205)) {
				var7 = -0.09251824;
			}
			else {
				var7 = -0.017413285;
			}
		}
	}
	else {
		if ((input[12]) >= (0.0724)) {
			if ((input[38]) >= (0.125)) {
				var7 = -0.081579216;
			}
			else {
				var7 = 0.043493833;
			}
		}
		else {
			if ((input[39]) >= (0.58335)) {
				var7 = -0.03054404;
			}
			else {
				var7 = 0.12570493;
			}
		}
	}
	double var8;
	if ((input[60]) >= (1.5)) {
		if ((input[39]) >= (1.41665)) {
			if ((input[39]) >= (5.29165)) {
				var8 = -0.12489823;
			}
			else {
				var8 = -0.1078321;
			}
		}
		else {
			if ((input[57]) >= (0.5)) {
				var8 = -0.050932344;
			}
			else {
				var8 = -0.10729934;
			}
		}
	}
	else {
		if ((input[1]) >= (0.06825)) {
			if ((input[60]) >= (0.5)) {
				var8 = -0.07656337;
			}
			else {
				var8 = 0.042029914;
			}
		}
		else {
			if ((input[39]) >= (1.125)) {
				var8 = -0.03702027;
			}
			else {
				var8 = 0.121101335;
			}
		}
	}
	double var9;
	if ((input[60]) >= (1.5)) {
		if ((input[41]) >= (2.5)) {
			if ((input[1]) >= (0.0443)) {
				var9 = -0.077384375;
			}
			else {
				var9 = 0.025085062;
			}
		}
		else {
			if ((input[45]) >= (30.9674)) {
				var9 = -0.11498535;
			}
			else {
				var9 = -0.04149407;
			}
		}
	}
	else {
		if ((input[12]) >= (0.03895)) {
			if ((input[13]) >= (0.5)) {
				var9 = -0.06281596;
			}
			else {
				var9 = 0.053423442;
			}
		}
		else {
			if ((input[39]) >= (2.625)) {
				var9 = -0.0949954;
			}
			else {
				var9 = 0.11791867;
			}
		}
	}
	double var10;
	if ((input[60]) >= (0.5)) {
		if ((input[1]) >= (0.01575)) {
			if ((input[41]) >= (2.5)) {
				var10 = -0.061779797;
			}
			else {
				var10 = -0.10853118;
			}
		}
		else {
			if ((input[39]) >= (1.875)) {
				var10 = -0.11234768;
			}
			else {
				var10 = 0.08878489;
			}
		}
	}
	else {
		if ((input[56]) >= (8527.0)) {
			if ((input[54]) >= (47.5)) {
				var10 = 0.09469939;
			}
			else {
				var10 = -0.06704384;
			}
		}
		else {
			if ((input[39]) >= (1.375)) {
				var10 = -0.019179238;
			}
			else {
				var10 = 0.113835804;
			}
		}
	}
	double var11;
	if ((input[60]) >= (1.5)) {
		if ((input[39]) >= (1.41665)) {
			if ((input[39]) >= (6.29165)) {
				var11 = -0.115714006;
			}
			else {
				var11 = -0.09529577;
			}
		}
		else {
			if ((input[60]) >= (8.5)) {
				var11 = -0.091804795;
			}
			else {
				var11 = -0.034420386;
			}
		}
	}
	else {
		if ((input[12]) >= (0.037699997)) {
			if ((input[2]) >= (0.5)) {
				var11 = -0.05719411;
			}
			else {
				var11 = 0.044861697;
			}
		}
		else {
			if ((input[1]) >= (0.31405002)) {
				var11 = -0.030919677;
			}
			else {
				var11 = 0.11317086;
			}
		}
	}
	double var12;
	if ((input[60]) >= (0.5)) {
		if ((input[39]) >= (0.125)) {
			if ((input[39]) >= (5.29165)) {
				var12 = -0.11392367;
			}
			else {
				var12 = -0.08567746;
			}
		}
		else {
			if ((input[12]) >= (0.00575)) {
				var12 = -0.053007066;
			}
			else {
				var12 = 0.09390951;
			}
		}
	}
	else {
		if ((input[56]) >= (5949.5)) {
			if ((input[54]) >= (45.5)) {
				var12 = 0.05433925;
			}
			else {
				var12 = -0.07695239;
			}
		}
		else {
			if ((input[39]) >= (1.375)) {
				var12 = -0.009265694;
			}
			else {
				var12 = 0.1085095;
			}
		}
	}
	double var13;
	if ((input[60]) >= (0.5)) {
		if ((input[41]) >= (3.5)) {
			if ((input[12]) >= (0.00915)) {
				var13 = -0.030060062;
			}
			else {
				var13 = 0.10512859;
			}
		}
		else {
			if ((input[45]) >= (31.705551)) {
				var13 = -0.09068362;
			}
			else {
				var13 = 0.012500124;
			}
		}
	}
	else {
		if ((input[13]) >= (0.5)) {
			if ((input[3]) >= (0.1)) {
				var13 = -0.04609003;
			}
			else {
				var13 = 0.08817988;
			}
		}
		else {
			if ((input[39]) >= (1.875)) {
				var13 = -0.038455226;
			}
			else {
				var13 = 0.1065128;
			}
		}
	}
	double var14;
	if ((input[60]) >= (1.5)) {
		if ((input[39]) >= (2.625)) {
			if ((input[45]) >= (17.096802)) {
				var14 = -0.103222705;
			}
			else {
				var14 = 0.109659545;
			}
		}
		else {
			if ((input[60]) >= (8.5)) {
				var14 = -0.08393928;
			}
			else {
				var14 = -0.031274937;
			}
		}
	}
	else {
		if ((input[12]) >= (0.00915)) {
			if ((input[7]) >= (4.5)) {
				var14 = 0.07641547;
			}
			else {
				var14 = -0.025766378;
			}
		}
		else {
			if ((input[1]) >= (0.3169)) {
				var14 = -0.040706918;
			}
			else {
				var14 = 0.10850754;
			}
		}
	}
	double var15;
	if ((input[60]) >= (0.5)) {
		if ((input[39]) >= (0.58335)) {
			if ((input[39]) >= (5.29165)) {
				var15 = -0.107174985;
			}
			else {
				var15 = -0.075371444;
			}
		}
		else {
			if ((input[12]) >= (0.00255)) {
				var15 = -0.040907804;
			}
			else {
				var15 = 0.08715632;
			}
		}
	}
	else {
		if ((input[56]) >= (4789.0)) {
			if ((input[54]) >= (45.5)) {
				var15 = 0.053210795;
			}
			else {
				var15 = -0.06547851;
			}
		}
		else {
			if ((input[1]) >= (2.0045)) {
				var15 = 0.01915776;
			}
			else {
				var15 = 0.103086494;
			}
		}
	}
	double var16;
	if ((input[60]) >= (1.5)) {
		if ((input[41]) >= (2.5)) {
			if ((input[60]) >= (7.5)) {
				var16 = -0.066657245;
			}
			else {
				var16 = -0.010998632;
			}
		}
		else {
			if ((input[51]) >= (0.00945)) {
				var16 = -0.08178746;
			}
			else {
				var16 = -0.11010017;
			}
		}
	}
	else {
		if ((input[12]) >= (0.00915)) {
			if ((input[54]) >= (44.5)) {
				var16 = 0.054664552;
			}
			else {
				var16 = -0.032148898;
			}
		}
		else {
			if ((input[1]) >= (0.3169)) {
				var16 = -0.037943244;
			}
			else {
				var16 = 0.104425035;
			}
		}
	}
	double var17;
	if ((input[39]) >= (0.125)) {
		if ((input[39]) >= (2.58335)) {
			if ((input[48]) >= (1.66735)) {
				var17 = -0.084157705;
			}
			else {
				var17 = -0.114023626;
			}
		}
		else {
			if ((input[56]) >= (1617.0)) {
				var17 = -0.06669987;
			}
			else {
				var17 = -0.0054279375;
			}
		}
	}
	else {
		if ((input[14]) >= (0.225)) {
			if ((input[3]) >= (0.225)) {
				var17 = -0.043788187;
			}
			else {
				var17 = 0.049352348;
			}
		}
		else {
			if ((input[60]) >= (2.5)) {
				var17 = -0.039012548;
			}
			else {
				var17 = 0.099189244;
			}
		}
	}
	double var18;
	if ((input[1]) >= (0.35975)) {
		if ((input[41]) >= (3.5)) {
			if ((input[14]) >= (0.225)) {
				var18 = -0.03700113;
			}
			else {
				var18 = 0.05824706;
			}
		}
		else {
			if ((input[39]) >= (2.625)) {
				var18 = -0.09505569;
			}
			else {
				var18 = -0.05027358;
			}
		}
	}
	else {
		if ((input[39]) >= (0.58335)) {
			if ((input[47]) >= (0.068450004)) {
				var18 = -0.005676443;
			}
			else {
				var18 = -0.101928174;
			}
		}
		else {
			if ((input[13]) >= (0.5)) {
				var18 = 0.0056135375;
			}
			else {
				var18 = 0.09789007;
			}
		}
	}
	double var19;
	if ((input[60]) >= (0.5)) {
		if ((input[39]) >= (0.58335)) {
			if ((input[39]) >= (6.29165)) {
				var19 = -0.09997736;
			}
			else {
				var19 = -0.06168247;
			}
		}
		else {
			if ((input[12]) >= (0.00255)) {
				var19 = -0.032474555;
			}
			else {
				var19 = 0.07565574;
			}
		}
	}
	else {
		if ((input[56]) >= (3837.5)) {
			if ((input[54]) >= (46.5)) {
				var19 = 0.07425253;
			}
			else {
				var19 = -0.03866595;
			}
		}
		else {
			if ((input[54]) >= (42.5)) {
				var19 = 0.11017964;
			}
			else {
				var19 = 0.053672936;
			}
		}
	}
	double var20;
	if ((input[1]) >= (0.35975)) {
		if ((input[54]) >= (46.5)) {
			if ((input[56]) >= (12080.0)) {
				var20 = -0.0053434437;
			}
			else {
				var20 = 0.13067418;
			}
		}
		else {
			if ((input[41]) >= (2.5)) {
				var20 = -0.028629279;
			}
			else {
				var20 = -0.08149653;
			}
		}
	}
	else {
		if ((input[12]) >= (0.00255)) {
			if ((input[54]) >= (42.5)) {
				var20 = 0.06309622;
			}
			else {
				var20 = -0.028854473;
			}
		}
		else {
			if ((input[39]) >= (3.75)) {
				var20 = -0.07790966;
			}
			else {
				var20 = 0.10005295;
			}
		}
	}
	double var21;
	if ((input[12]) >= (0.7666)) {
		if ((input[39]) >= (0.58335)) {
			if ((input[47]) >= (0.47324997)) {
				var21 = -0.06772431;
			}
			else {
				var21 = -0.113217294;
			}
		}
		else {
			if ((input[54]) >= (45.5)) {
				var21 = 0.043455575;
			}
			else {
				var21 = -0.043952707;
			}
		}
	}
	else {
		if ((input[3]) >= (0.225)) {
			if ((input[14]) >= (0.1)) {
				var21 = -0.019835537;
			}
			else {
				var21 = 0.06531237;
			}
		}
		else {
			if ((input[60]) >= (1.5)) {
				var21 = -0.059358414;
			}
			else {
				var21 = 0.09704666;
			}
		}
	}
	double var22;
	if ((input[41]) >= (3.5)) {
		if ((input[14]) >= (0.1)) {
			if ((input[56]) >= (13686.0)) {
				var22 = -0.056889206;
			}
			else {
				var22 = 0.02839464;
			}
		}
		else {
			if ((input[12]) >= (2.3667002)) {
				var22 = 0.019387959;
			}
			else {
				var22 = 0.10129937;
			}
		}
	}
	else {
		if ((input[1]) >= (1.47535)) {
			if ((input[39]) >= (2.625)) {
				var22 = -0.09125847;
			}
			else {
				var22 = -0.051899016;
			}
		}
		else {
			if ((input[5]) >= (2.225)) {
				var22 = -0.039988574;
			}
			else {
				var22 = 0.031198665;
			}
		}
	}
	double var23;
	if ((input[5]) >= (0.1)) {
		if ((input[18]) >= (3.5)) {
			if ((input[56]) >= (2713.0)) {
				var23 = -0.019805053;
			}
			else {
				var23 = 0.053852063;
			}
		}
		else {
			if ((input[41]) >= (2.5)) {
				var23 = -0.030625422;
			}
			else {
				var23 = -0.073873885;
			}
		}
	}
	else {
		if ((input[39]) >= (0.125)) {
			if ((input[47]) >= (0.91425)) {
				var23 = -0.0036332742;
			}
			else {
				var23 = -0.09968019;
			}
		}
		else {
			if ((input[2]) >= (0.5)) {
				var23 = -0.025181482;
			}
			else {
				var23 = 0.09060508;
			}
		}
	}
	double var24;
	if ((input[12]) >= (0.7666)) {
		if ((input[39]) >= (2.875)) {
			if ((input[51]) >= (0.00955)) {
				var24 = -0.07233731;
			}
			else {
				var24 = -0.1081621;
			}
		}
		else {
			if ((input[54]) >= (47.5)) {
				var24 = 0.07606723;
			}
			else {
				var24 = -0.034560695;
			}
		}
	}
	else {
		if ((input[1]) >= (0.00255)) {
			if ((input[54]) >= (43.5)) {
				var24 = 0.07211422;
			}
			else {
				var24 = -0.017494118;
			}
		}
		else {
			if ((input[41]) >= (0.5)) {
				var24 = 0.09282368;
			}
			else {
				var24 = -0.08774961;
			}
		}
	}
	double var25;
	if ((input[60]) >= (0.5)) {
		if ((input[60]) >= (7.5)) {
			if ((input[45]) >= (46.000397)) {
				var25 = -0.07736789;
			}
			else {
				var25 = -0.0060357875;
			}
		}
		else {
			if ((input[51]) >= (0.01255)) {
				var25 = 0.008877492;
			}
			else {
				var25 = -0.06486648;
			}
		}
	}
	else {
		if ((input[56]) >= (5898.5)) {
			if ((input[54]) >= (48.5)) {
				var25 = 0.09618014;
			}
			else {
				var25 = -0.040199567;
			}
		}
		else {
			if ((input[54]) >= (40.5)) {
				var25 = 0.09217549;
			}
			else {
				var25 = 0.021573711;
			}
		}
	}
	double var26;
	if ((input[1]) >= (1.5899501)) {
		if ((input[39]) >= (2.625)) {
			if ((input[47]) >= (0.90275)) {
				var26 = -0.06579369;
			}
			else {
				var26 = -0.10398521;
			}
		}
		else {
			if ((input[54]) >= (47.5)) {
				var26 = 0.055829145;
			}
			else {
				var26 = -0.039227083;
			}
		}
	}
	else {
		if ((input[14]) >= (0.1)) {
			if ((input[56]) >= (2327.5)) {
				var26 = -0.025605539;
			}
			else {
				var26 = 0.032773774;
			}
		}
		else {
			if ((input[39]) >= (0.125)) {
				var26 = -0.05068594;
			}
			else {
				var26 = 0.091657095;
			}
		}
	}
	double var27;
	if ((input[5]) >= (0.1)) {
		if ((input[41]) >= (2.5)) {
			if ((input[56]) >= (2432.0)) {
				var27 = -0.028375486;
			}
			else {
				var27 = 0.02843583;
			}
		}
		else {
			if ((input[51]) >= (0.00945)) {
				var27 = -0.045375418;
			}
			else {
				var27 = -0.09453184;
			}
		}
	}
	else {
		if ((input[56]) >= (2974.0)) {
			if ((input[54]) >= (45.5)) {
				var27 = 0.050095648;
			}
			else {
				var27 = -0.032961395;
			}
		}
		else {
			if ((input[60]) >= (2.5)) {
				var27 = -0.040877342;
			}
			else {
				var27 = 0.09422844;
			}
		}
	}
	double var28;
	if ((input[1]) >= (1.5899501)) {
		if ((input[39]) >= (2.625)) {
			if ((input[47]) >= (0.90275)) {
				var28 = -0.06076398;
			}
			else {
				var28 = -0.10153951;
			}
		}
		else {
			if ((input[54]) >= (48.5)) {
				var28 = 0.08390996;
			}
			else {
				var28 = -0.032795835;
			}
		}
	}
	else {
		if ((input[12]) >= (0.00255)) {
			if ((input[54]) >= (43.5)) {
				var28 = 0.06324918;
			}
			else {
				var28 = -0.01700477;
			}
		}
		else {
			if ((input[41]) >= (0.5)) {
				var28 = 0.087567255;
			}
			else {
				var28 = -0.081031;
			}
		}
	}
	double var29;
	if ((input[10]) >= (3.5)) {
		if ((input[56]) >= (2972.0)) {
			if ((input[54]) >= (48.5)) {
				var29 = 0.09950281;
			}
			else {
				var29 = -0.012385706;
			}
		}
		else {
			if ((input[60]) >= (2.5)) {
				var29 = -0.03499313;
			}
			else {
				var29 = 0.08718058;
			}
		}
	}
	else {
		if ((input[21]) >= (3.5)) {
			if ((input[13]) >= (0.5)) {
				var29 = -0.028967498;
			}
			else {
				var29 = 0.040583044;
			}
		}
		else {
			if ((input[51]) >= (0.01275)) {
				var29 = -0.033586908;
			}
			else {
				var29 = -0.08218852;
			}
		}
	}
	double var30;
	if ((input[12]) >= (1.89145)) {
		if ((input[47]) >= (0.95135)) {
			if ((input[0]) >= (6.0)) {
				var30 = -0.07252195;
			}
			else {
				var30 = -0.01250374;
			}
		}
		else {
			if ((input[56]) >= (55.5)) {
				var30 = -0.08431601;
			}
			else {
				var30 = 0.09710297;
			}
		}
	}
	else {
		if ((input[3]) >= (0.1)) {
			if ((input[61]) >= (17400.0)) {
				var30 = -0.051339775;
			}
			else {
				var30 = 0.010810872;
			}
		}
		else {
			if ((input[60]) >= (2.5)) {
				var30 = -0.072106294;
			}
			else {
				var30 = 0.08402946;
			}
		}
	}
	double var31;
	if ((input[41]) >= (3.5)) {
		if ((input[14]) >= (0.1)) {
			if ((input[61]) >= (17732.5)) {
				var31 = -0.050536048;
			}
			else {
				var31 = 0.017941616;
			}
		}
		else {
			if ((input[12]) >= (2.3667002)) {
				var31 = 0.014129149;
			}
			else {
				var31 = 0.08979865;
			}
		}
	}
	else {
		if ((input[39]) >= (5.29165)) {
			if ((input[46]) >= (1.52555)) {
				var31 = -0.06414737;
			}
			else {
				var31 = -0.104547255;
			}
		}
		else {
			if ((input[12]) >= (1.42905)) {
				var31 = -0.040473294;
			}
			else {
				var31 = 0.0048861383;
			}
		}
	}
	double var32;
	if ((input[60]) >= (7.5)) {
		if ((input[47]) >= (1.12785)) {
			if ((input[45]) >= (42.79225)) {
				var32 = -0.049029425;
			}
			else {
				var32 = 0.029088084;
			}
		}
		else {
			if ((input[62]) >= (2.5)) {
				var32 = -0.09383526;
			}
			else {
				var32 = 0.008207361;
			}
		}
	}
	else {
		if ((input[61]) >= (4786.0)) {
			if ((input[47]) >= (1.67605)) {
				var32 = -0.0071722753;
			}
			else {
				var32 = -0.089515634;
			}
		}
		else {
			if ((input[47]) >= (0.79355)) {
				var32 = 0.06463364;
			}
			else {
				var32 = -0.017276928;
			}
		}
	}
	double var33;
	if ((input[10]) >= (3.5)) {
		if ((input[56]) >= (2052.0)) {
			if ((input[54]) >= (45.5)) {
				var33 = 0.05007403;
			}
			else {
				var33 = -0.0206668;
			}
		}
		else {
			if ((input[60]) >= (2.5)) {
				var33 = -0.033478342;
			}
			else {
				var33 = 0.08531965;
			}
		}
	}
	else {
		if ((input[51]) >= (0.01275)) {
			if ((input[56]) >= (1346.5)) {
				var33 = -0.026007859;
			}
			else {
				var33 = 0.042351533;
			}
		}
		else {
			if ((input[56]) >= (49.5)) {
				var33 = -0.07882852;
			}
			else {
				var33 = 0.07201293;
			}
		}
	}
	double var34;
	if ((input[16]) >= (0.1)) {
		if ((input[47]) >= (0.5094)) {
			if ((input[56]) >= (737.5)) {
				var34 = -0.017235693;
			}
			else {
				var34 = 0.057475604;
			}
		}
		else {
			if ((input[56]) >= (49.5)) {
				var34 = -0.08246487;
			}
			else {
				var34 = 0.06326923;
			}
		}
	}
	else {
		if ((input[13]) >= (0.5)) {
			if ((input[56]) >= (767.0)) {
				var34 = -0.03180263;
			}
			else {
				var34 = 0.10388564;
			}
		}
		else {
			if ((input[39]) >= (2.125)) {
				var34 = -0.052657403;
			}
			else {
				var34 = 0.07490268;
			}
		}
	}
	double var35;
	if ((input[60]) >= (8.5)) {
		if ((input[45]) >= (42.79225)) {
			if ((input[3]) >= (0.45)) {
				var35 = -0.047156848;
			}
			else {
				var35 = -0.090947784;
			}
		}
		else {
			if ((input[47]) >= (1.6584)) {
				var35 = 0.04415733;
			}
			else {
				var35 = -0.05917687;
			}
		}
	}
	else {
		if ((input[61]) >= (13495.5)) {
			if ((input[47]) >= (1.67605)) {
				var35 = -0.025498886;
			}
			else {
				var35 = -0.10564828;
			}
		}
		else {
			if ((input[54]) >= (42.5)) {
				var35 = 0.067306474;
			}
			else {
				var35 = -0.003999724;
			}
		}
	}
	double var36;
	if ((input[1]) >= (1.5899501)) {
		if ((input[39]) >= (5.29165)) {
			if ((input[50]) >= (1.6716499)) {
				var36 = -0.06684911;
			}
			else {
				var36 = -0.10016956;
			}
		}
		else {
			if ((input[47]) >= (0.9522)) {
				var36 = -0.009603474;
			}
			else {
				var36 = -0.055195075;
			}
		}
	}
	else {
		if ((input[54]) >= (40.5)) {
			if ((input[61]) >= (17400.0)) {
				var36 = -0.027162304;
			}
			else {
				var36 = 0.070709065;
			}
		}
		else {
			if ((input[12]) >= (0.0006)) {
				var36 = -0.028218498;
			}
			else {
				var36 = 0.045072027;
			}
		}
	}
	double var37;
	if ((input[7]) >= (4.5)) {
		if ((input[56]) >= (2479.5)) {
			if ((input[54]) >= (43.5)) {
				var37 = 0.04320827;
			}
			else {
				var37 = -0.033356942;
			}
		}
		else {
			if ((input[41]) >= (2.5)) {
				var37 = 0.088691644;
			}
			else {
				var37 = -0.06478932;
			}
		}
	}
	else {
		if ((input[51]) >= (0.01275)) {
			if ((input[56]) >= (1588.5)) {
				var37 = -0.014921608;
			}
			else {
				var37 = 0.042097494;
			}
		}
		else {
			if ((input[56]) >= (49.5)) {
				var37 = -0.06591976;
			}
			else {
				var37 = 0.060252994;
			}
		}
	}
	double var38;
	if ((input[12]) >= (2.34535)) {
		if ((input[39]) >= (2.875)) {
			if ((input[47]) >= (0.70145)) {
				var38 = -0.055028953;
			}
			else {
				var38 = -0.09888169;
			}
		}
		else {
			if ((input[54]) >= (48.5)) {
				var38 = 0.082671516;
			}
			else {
				var38 = -0.022827521;
			}
		}
	}
	else {
		if ((input[54]) >= (41.5)) {
			if ((input[56]) >= (10749.5)) {
				var38 = -0.0071666846;
			}
			else {
				var38 = 0.073402174;
			}
		}
		else {
			if ((input[56]) >= (1176.5)) {
				var38 = -0.037757445;
			}
			else {
				var38 = 0.017761366;
			}
		}
	}
	double var39;
	if ((input[39]) >= (5.29165)) {
		if ((input[46]) >= (1.52555)) {
			if ((input[56]) >= (414.5)) {
				var39 = -0.060525723;
			}
			else {
				var39 = 0.056731608;
			}
		}
		else {
			if ((input[56]) >= (25.5)) {
				var39 = -0.10426235;
			}
			else {
				var39 = -0.024108412;
			}
		}
	}
	else {
		if ((input[12]) >= (0.0006)) {
			if ((input[54]) >= (46.5)) {
				var39 = 0.063148394;
			}
			else {
				var39 = -0.010437518;
			}
		}
		else {
			if ((input[1]) >= (0.05455)) {
				var39 = -0.05462527;
			}
			else {
				var39 = 0.07741804;
			}
		}
	}
	double var40;
	if ((input[41]) >= (0.5)) {
		if ((input[12]) >= (0.00035000002)) {
			if ((input[47]) >= (1.10515)) {
				var40 = 0.008232168;
			}
			else {
				var40 = -0.037397247;
			}
		}
		else {
			if ((input[1]) >= (0.0121)) {
				var40 = -0.047518928;
			}
			else {
				var40 = 0.07788458;
			}
		}
	}
	else {
		if ((input[51]) >= (0.69445)) {
			if ((input[16]) >= (3.3)) {
				var40 = -0.036377873;
			}
			else {
				var40 = 0.18063824;
			}
		}
		else {
			if ((input[48]) >= (4.26075)) {
				var40 = -0.05932808;
			}
			else {
				var40 = -0.09758767;
			}
		}
	}
	double var41;
	if ((input[10]) >= (3.5)) {
		if ((input[56]) >= (2052.0)) {
			if ((input[54]) >= (46.5)) {
				var41 = 0.06370963;
			}
			else {
				var41 = -0.00998773;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var41 = -0.03986523;
			}
			else {
				var41 = 0.07274044;
			}
		}
	}
	else {
		if ((input[21]) >= (3.5)) {
			if ((input[54]) >= (39.5)) {
				var41 = 0.031698175;
			}
			else {
				var41 = -0.020005224;
			}
		}
		else {
			if ((input[47]) >= (0.34845)) {
				var41 = -0.021954224;
			}
			else {
				var41 = -0.06770916;
			}
		}
	}
	double var42;
	if ((input[54]) >= (42.5)) {
		if ((input[56]) >= (3372.5)) {
			if ((input[47]) >= (1.7061)) {
				var42 = 0.019375456;
			}
			else {
				var42 = -0.061026424;
			}
		}
		else {
			if ((input[39]) >= (0.875)) {
				var42 = -0.0031472188;
			}
			else {
				var42 = 0.099255286;
			}
		}
	}
	else {
		if ((input[51]) >= (0.00405)) {
			if ((input[56]) >= (740.5)) {
				var42 = -0.026109695;
			}
			else {
				var42 = 0.053448398;
			}
		}
		else {
			if ((input[56]) >= (49.5)) {
				var42 = -0.097408965;
			}
			else {
				var42 = 0.04269376;
			}
		}
	}
	double var43;
	if ((input[41]) >= (0.5)) {
		if ((input[12]) >= (0.00035000002)) {
			if ((input[47]) >= (1.81265)) {
				var43 = 0.013680816;
			}
			else {
				var43 = -0.023102503;
			}
		}
		else {
			if ((input[39]) >= (2.0)) {
				var43 = -0.0464447;
			}
			else {
				var43 = 0.0738685;
			}
		}
	}
	else {
		if ((input[15]) >= (3.0)) {
			if ((input[60]) >= (0.5)) {
				var43 = -0.08696114;
			}
			else {
				var43 = 0.024873212;
			}
		}
		else {
			if ((input[49]) >= (0.98775)) {
				var43 = -0.057362773;
			}
			else {
				var43 = 0.08425649;
			}
		}
	}
	double var44;
	if ((input[54]) >= (43.5)) {
		if ((input[56]) >= (4779.0)) {
			if ((input[51]) >= (0.05905)) {
				var44 = 0.021914605;
			}
			else {
				var44 = -0.06479811;
			}
		}
		else {
			if ((input[43]) >= (1.5)) {
				var44 = -0.062273413;
			}
			else {
				var44 = 0.09740084;
			}
		}
	}
	else {
		if ((input[53]) >= (0.98725)) {
			if ((input[56]) >= (390.5)) {
				var44 = -0.039611578;
			}
			else {
				var44 = 0.012689925;
			}
		}
		else {
			if ((input[56]) >= (31846.5)) {
				var44 = -0.05341458;
			}
			else {
				var44 = 0.026415551;
			}
		}
	}
	double var45;
	if ((input[39]) >= (6.29165)) {
		if ((input[46]) >= (1.6361)) {
			if ((input[4]) >= (3.0)) {
				var45 = -0.06238376;
			}
			else {
				var45 = 0.007987845;
			}
		}
		else {
			if ((input[36]) >= (0.84319997)) {
				var45 = -0.10062625;
			}
			else {
				var45 = -0.0016561976;
			}
		}
	}
	else {
		if ((input[56]) >= (7640.0)) {
			if ((input[51]) >= (0.042850003)) {
				var45 = -0.012002155;
			}
			else {
				var45 = -0.081498794;
			}
		}
		else {
			if ((input[54]) >= (40.5)) {
				var45 = 0.056577183;
			}
			else {
				var45 = -0.011664945;
			}
		}
	}
	double var46;
	if ((input[0]) >= (6.0)) {
		if ((input[1]) >= (0.29005)) {
			if ((input[50]) >= (2.09705)) {
				var46 = -0.04734097;
			}
			else {
				var46 = -0.08975191;
			}
		}
		else {
			if ((input[51]) >= (0.1352)) {
				var46 = 0.06874054;
			}
			else {
				var46 = -0.03115958;
			}
		}
	}
	else {
		if ((input[54]) >= (42.5)) {
			if ((input[56]) >= (15668.5)) {
				var46 = -0.021114504;
			}
			else {
				var46 = 0.06617651;
			}
		}
		else {
			if ((input[56]) >= (738.5)) {
				var46 = -0.025347574;
			}
			else {
				var46 = 0.031925768;
			}
		}
	}
	double var47;
	if ((input[47]) >= (0.88865)) {
		if ((input[56]) >= (1588.5)) {
			if ((input[38]) >= (0.125)) {
				var47 = -0.022846749;
			}
			else {
				var47 = 0.018646425;
			}
		}
		else {
			if ((input[54]) >= (37.5)) {
				var47 = 0.08551823;
			}
			else {
				var47 = 0.00396302;
			}
		}
	}
	else {
		if ((input[56]) >= (434.5)) {
			if ((input[54]) >= (46.5)) {
				var47 = 0.024474159;
			}
			else {
				var47 = -0.06625736;
			}
		}
		else {
			if ((input[54]) >= (40.5)) {
				var47 = 0.11244382;
			}
			else {
				var47 = -0.024013959;
			}
		}
	}
	double var48;
	if ((input[51]) >= (0.01275)) {
		if ((input[56]) >= (2464.5)) {
			if ((input[54]) >= (46.5)) {
				var48 = 0.051435042;
			}
			else {
				var48 = -0.014363055;
			}
		}
		else {
			if ((input[54]) >= (40.5)) {
				var48 = 0.09220306;
			}
			else {
				var48 = 0.011237342;
			}
		}
	}
	else {
		if ((input[56]) >= (49.5)) {
			if ((input[54]) >= (44.5)) {
				var48 = -0.0031196994;
			}
			else {
				var48 = -0.062401026;
			}
		}
		else {
			if ((input[60]) >= (1.5)) {
				var48 = -0.09807774;
			}
			else {
				var48 = 0.109790266;
			}
		}
	}
	double var49;
	if ((input[47]) >= (1.81265)) {
		if ((input[56]) >= (13928.5)) {
			if ((input[45]) >= (31.560299)) {
				var49 = -0.023177326;
			}
			else {
				var49 = 0.05099348;
			}
		}
		else {
			if ((input[54]) >= (41.5)) {
				var49 = 0.08405378;
			}
			else {
				var49 = 0.012363225;
			}
		}
	}
	else {
		if ((input[56]) >= (574.5)) {
			if ((input[54]) >= (44.5)) {
				var49 = 0.0038978679;
			}
			else {
				var49 = -0.045789134;
			}
		}
		else {
			if ((input[60]) >= (2.5)) {
				var49 = -0.056984585;
			}
			else {
				var49 = 0.05462713;
			}
		}
	}
	double var50;
	if ((input[0]) >= (6.0)) {
		if ((input[1]) >= (2.9475)) {
			if ((input[45]) >= (31.41795)) {
				var50 = -0.070760064;
			}
			else {
				var50 = 0.025715211;
			}
		}
		else {
			if ((input[50]) >= (4.9046497)) {
				var50 = 0.009376018;
			}
			else {
				var50 = -0.042218987;
			}
		}
	}
	else {
		if ((input[47]) >= (0.26735002)) {
			if ((input[56]) >= (742.5)) {
				var50 = 0.0048330915;
			}
			else {
				var50 = 0.083398774;
			}
		}
		else {
			if ((input[56]) >= (31.5)) {
				var50 = -0.053641643;
			}
			else {
				var50 = 0.11645945;
			}
		}
	}
	double var51;
	if ((input[41]) >= (0.5)) {
		if ((input[56]) >= (575.5)) {
			if ((input[47]) >= (1.7066)) {
				var51 = 0.01234881;
			}
			else {
				var51 = -0.028050626;
			}
		}
		else {
			if ((input[54]) >= (40.5)) {
				var51 = 0.105362915;
			}
			else {
				var51 = 0.022452356;
			}
		}
	}
	else {
		if ((input[15]) >= (3.0)) {
			if ((input[60]) >= (1.5)) {
				var51 = -0.08527889;
			}
			else {
				var51 = -0.033364844;
			}
		}
		else {
			if ((input[38]) >= (1.125)) {
				var51 = 0.09394199;
			}
			else {
				var51 = -0.045595583;
			}
		}
	}
	double var52;
	if ((input[6]) >= (2.5)) {
		if ((input[61]) >= (1991.5)) {
			if ((input[47]) >= (1.7061)) {
				var52 = 0.013732848;
			}
			else {
				var52 = -0.03768382;
			}
		}
		else {
			if ((input[60]) >= (7.5)) {
				var52 = -0.06133939;
			}
			else {
				var52 = 0.05381694;
			}
		}
	}
	else {
		if ((input[21]) >= (3.5)) {
			if ((input[1]) >= (7.4674)) {
				var52 = -0.054020613;
			}
			else {
				var52 = 0.02151288;
			}
		}
		else {
			if ((input[50]) >= (1.1925)) {
				var52 = -0.018274443;
			}
			else {
				var52 = -0.056289136;
			}
		}
	}
	double var53;
	if ((input[54]) >= (48.5)) {
		if ((input[56]) >= (569376.5)) {
			if ((input[13]) >= (1.5)) {
				var53 = 0.037976135;
			}
			else {
				var53 = -0.11208754;
			}
		}
		else {
			if ((input[40]) >= (0.5)) {
				var53 = 0.09688842;
			}
			else {
				var53 = -0.038666766;
			}
		}
	}
	else {
		if ((input[61]) >= (31405.0)) {
			if ((input[60]) >= (1.5)) {
				var53 = -0.0125718815;
			}
			else {
				var53 = -0.06931696;
			}
		}
		else {
			if ((input[47]) >= (1.85875)) {
				var53 = 0.027010238;
			}
			else {
				var53 = -0.016550288;
			}
		}
	}
	double var54;
	if ((input[54]) >= (40.5)) {
		if ((input[56]) >= (5337.5)) {
			if ((input[47]) >= (1.81265)) {
				var54 = 0.005404988;
			}
			else {
				var54 = -0.059663035;
			}
		}
		else {
			if ((input[47]) >= (0.9511)) {
				var54 = 0.07915435;
			}
			else {
				var54 = 0.0009757583;
			}
		}
	}
	else {
		if ((input[50]) >= (0.38244998)) {
			if ((input[56]) >= (382.5)) {
				var54 = -0.019829845;
			}
			else {
				var54 = 0.07150813;
			}
		}
		else {
			if ((input[56]) >= (49.5)) {
				var54 = -0.08685639;
			}
			else {
				var54 = 0.03737747;
			}
		}
	}
	double var55;
	if ((input[0]) >= (6.0)) {
		if ((input[1]) >= (0.29005)) {
			if ((input[46]) >= (36.71685)) {
				var55 = -0.031061877;
			}
			else {
				var55 = -0.06711867;
			}
		}
		else {
			if ((input[51]) >= (0.1352)) {
				var55 = 0.06239885;
			}
			else {
				var55 = -0.030035218;
			}
		}
	}
	else {
		if ((input[54]) >= (45.5)) {
			if ((input[56]) >= (9703.5)) {
				var55 = 0.005132387;
			}
			else {
				var55 = 0.0975207;
			}
		}
		else {
			if ((input[56]) >= (1111.5)) {
				var55 = -0.013445796;
			}
			else {
				var55 = 0.03177066;
			}
		}
	}
	double var56;
	if ((input[47]) >= (0.31485)) {
		if ((input[56]) >= (1588.5)) {
			if ((input[54]) >= (46.5)) {
				var56 = 0.044694126;
			}
			else {
				var56 = -0.010545499;
			}
		}
		else {
			if ((input[54]) >= (37.5)) {
				var56 = 0.065955095;
			}
			else {
				var56 = -0.0053998455;
			}
		}
	}
	else {
		if ((input[56]) >= (27.5)) {
			if ((input[54]) >= (44.5)) {
				var56 = 0.0039851028;
			}
			else {
				var56 = -0.06756703;
			}
		}
		else {
			if ((input[54]) >= (35.5)) {
				var56 = 0.115280665;
			}
			else {
				var56 = -0.03744505;
			}
		}
	}
	double var57;
	if ((input[0]) >= (6.0)) {
		if ((input[1]) >= (2.9475)) {
			if ((input[45]) >= (31.41795)) {
				var57 = -0.06368627;
			}
			else {
				var57 = 0.030989427;
			}
		}
		else {
			if ((input[51]) >= (0.0783)) {
				var57 = 0.008706653;
			}
			else {
				var57 = -0.03733961;
			}
		}
	}
	else {
		if ((input[54]) >= (42.5)) {
			if ((input[56]) >= (18138.0)) {
				var57 = -0.020601582;
			}
			else {
				var57 = 0.05705294;
			}
		}
		else {
			if ((input[53]) >= (0.98715)) {
				var57 = -0.025827637;
			}
			else {
				var57 = 0.013951569;
			}
		}
	}
	double var58;
	if ((input[1]) >= (5.5665503)) {
		if ((input[39]) >= (1.875)) {
			if ((input[25]) >= (0.225)) {
				var58 = -0.027233684;
			}
			else {
				var58 = -0.075640045;
			}
		}
		else {
			if ((input[25]) >= (1.29165)) {
				var58 = -0.059339143;
			}
			else {
				var58 = 0.009566848;
			}
		}
	}
	else {
		if ((input[54]) >= (42.5)) {
			if ((input[61]) >= (17642.0)) {
				var58 = -0.01746883;
			}
			else {
				var58 = 0.051729828;
			}
		}
		else {
			if ((input[51]) >= (0.00084999995)) {
				var58 = -0.0012114417;
			}
			else {
				var58 = -0.069517285;
			}
		}
	}
	double var59;
	if ((input[0]) >= (6.0)) {
		if ((input[36]) >= (6.9739)) {
			if ((input[45]) >= (102.71385)) {
				var59 = -0.060351707;
			}
			else {
				var59 = 0.07514293;
			}
		}
		else {
			if ((input[1]) >= (0.00305)) {
				var59 = -0.044522908;
			}
			else {
				var59 = 0.0028516906;
			}
		}
	}
	else {
		if ((input[56]) >= (740.5)) {
			if ((input[47]) >= (1.08955)) {
				var59 = 0.00834192;
			}
			else {
				var59 = -0.038614895;
			}
		}
		else {
			if ((input[47]) >= (0.20664999)) {
				var59 = 0.07102981;
			}
			else {
				var59 = -0.013467659;
			}
		}
	}
	double var60;
	if ((input[54]) >= (48.5)) {
		if ((input[56]) >= (375017.0)) {
			if ((input[46]) >= (848.78125)) {
				var60 = 0.0117697725;
			}
			else {
				var60 = -0.14161807;
			}
		}
		else {
			if ((input[12]) >= (10.596649)) {
				var60 = -0.008597716;
			}
			else {
				var60 = 0.09828372;
			}
		}
	}
	else {
		if ((input[56]) >= (2469.5)) {
			if ((input[51]) >= (0.042850003)) {
				var60 = -0.0031244918;
			}
			else {
				var60 = -0.054838635;
			}
		}
		else {
			if ((input[54]) >= (42.5)) {
				var60 = 0.07178011;
			}
			else {
				var60 = -0.000498102;
			}
		}
	}
	double var61;
	if ((input[43]) >= (2.5)) {
		if ((input[47]) >= (0.71385)) {
			if ((input[22]) >= (0.5)) {
				var61 = -0.07101858;
			}
			else {
				var61 = 0.025287842;
			}
		}
		else {
			if ((input[52]) >= (30.2583)) {
				var61 = 0.019818313;
			}
			else {
				var61 = -0.10239408;
			}
		}
	}
	else {
		if ((input[61]) >= (31405.0)) {
			if ((input[46]) >= (896.45435)) {
				var61 = -0.0011097682;
			}
			else {
				var61 = -0.049337726;
			}
		}
		else {
			if ((input[54]) >= (43.5)) {
				var61 = 0.04649434;
			}
			else {
				var61 = -0.0021224315;
			}
		}
	}
	double var62;
	if ((input[51]) >= (0.01275)) {
		if ((input[56]) >= (7633.0)) {
			if ((input[60]) >= (1.5)) {
				var62 = 0.00645013;
			}
			else {
				var62 = -0.026246509;
			}
		}
		else {
			if ((input[54]) >= (40.5)) {
				var62 = 0.056511708;
			}
			else {
				var62 = -0.0032655236;
			}
		}
	}
	else {
		if ((input[56]) >= (434.5)) {
			if ((input[54]) >= (44.5)) {
				var62 = -0.012454301;
			}
			else {
				var62 = -0.06698511;
			}
		}
		else {
			if ((input[54]) >= (40.5)) {
				var62 = 0.108589545;
			}
			else {
				var62 = -0.013853787;
			}
		}
	}
	double var63;
	if ((input[47]) >= (1.85865)) {
		if ((input[56]) >= (7563.5)) {
			if ((input[32]) >= (1.5)) {
				var63 = 0.0039550797;
			}
			else {
				var63 = -0.044853885;
			}
		}
		else {
			if ((input[54]) >= (40.5)) {
				var63 = 0.07463183;
			}
			else {
				var63 = 0.0155108115;
			}
		}
	}
	else {
		if ((input[56]) >= (390.5)) {
			if ((input[54]) >= (48.5)) {
				var63 = 0.06696402;
			}
			else {
				var63 = -0.025430942;
			}
		}
		else {
			if ((input[60]) >= (1.5)) {
				var63 = -0.034368798;
			}
			else {
				var63 = 0.060384;
			}
		}
	}
	double var64;
	if ((input[0]) >= (6.0)) {
		if ((input[3]) >= (0.45)) {
			if ((input[36]) >= (6.363)) {
				var64 = 0.021528134;
			}
			else {
				var64 = -0.031448126;
			}
		}
		else {
			if ((input[61]) >= (2740.0)) {
				var64 = -0.08858391;
			}
			else {
				var64 = -0.029819233;
			}
		}
	}
	else {
		if ((input[56]) >= (2048.5)) {
			if ((input[54]) >= (45.5)) {
				var64 = 0.0286583;
			}
			else {
				var64 = -0.014326604;
			}
		}
		else {
			if ((input[54]) >= (42.5)) {
				var64 = 0.09780977;
			}
			else {
				var64 = 0.010394389;
			}
		}
	}
	double var65;
	if ((input[50]) >= (0.3639)) {
		if ((input[56]) >= (1176.5)) {
			if ((input[54]) >= (39.0)) {
				var65 = 0.007274527;
			}
			else {
				var65 = -0.020904908;
			}
		}
		else {
			if ((input[54]) >= (35.5)) {
				var65 = 0.046798255;
			}
			else {
				var65 = -0.03976387;
			}
		}
	}
	else {
		if ((input[56]) >= (49.5)) {
			if ((input[54]) >= (44.5)) {
				var65 = -0.0018265834;
			}
			else {
				var65 = -0.062278748;
			}
		}
		else {
			if ((input[60]) >= (1.5)) {
				var65 = -0.08640386;
			}
			else {
				var65 = 0.102873825;
			}
		}
	}
	double var66;
	if ((input[47]) >= (1.85865)) {
		if ((input[61]) >= (31405.0)) {
			if ((input[45]) >= (30.9203)) {
				var66 = -0.02687268;
			}
			else {
				var66 = 0.09222893;
			}
		}
		else {
			if ((input[37]) >= (0.01515)) {
				var66 = 0.0012740842;
			}
			else {
				var66 = 0.04162107;
			}
		}
	}
	else {
		if ((input[56]) >= (390.5)) {
			if ((input[48]) >= (73.83295)) {
				var66 = 0.009775932;
			}
			else {
				var66 = -0.029787851;
			}
		}
		else {
			if ((input[54]) >= (37.5)) {
				var66 = 0.06763714;
			}
			else {
				var66 = -0.016934698;
			}
		}
	}
	double var67;
	if ((input[43]) >= (2.5)) {
		if ((input[47]) >= (0.71385)) {
			if ((input[22]) >= (0.5)) {
				var67 = -0.0682226;
			}
			else {
				var67 = 0.022081686;
			}
		}
		else {
			if ((input[52]) >= (30.2583)) {
				var67 = 0.01899592;
			}
			else {
				var67 = -0.10174497;
			}
		}
	}
	else {
		if ((input[56]) >= (574.5)) {
			if ((input[47]) >= (1.1233001)) {
				var67 = 0.0049686898;
			}
			else {
				var67 = -0.030036882;
			}
		}
		else {
			if ((input[54]) >= (40.5)) {
				var67 = 0.10310619;
			}
			else {
				var67 = 0.0145290615;
			}
		}
	}
	double var68;
	if ((input[0]) >= (6.0)) {
		if ((input[3]) >= (0.45)) {
			if ((input[1]) >= (3.3336)) {
				var68 = -0.0463841;
			}
			else {
				var68 = 0.005771897;
			}
		}
		else {
			if ((input[61]) >= (2740.0)) {
				var68 = -0.083198115;
			}
			else {
				var68 = -0.028284136;
			}
		}
	}
	else {
		if ((input[54]) >= (42.5)) {
			if ((input[56]) >= (13931.0)) {
				var68 = -0.012161354;
			}
			else {
				var68 = 0.051885393;
			}
		}
		else {
			if ((input[56]) >= (406.5)) {
				var68 = -0.013035933;
			}
			else {
				var68 = 0.036990568;
			}
		}
	}
	double var69;
	if ((input[47]) >= (0.26735002)) {
		if ((input[56]) >= (357.5)) {
			if ((input[54]) >= (40.5)) {
				var69 = 0.011890901;
			}
			else {
				var69 = -0.012592419;
			}
		}
		else {
			if ((input[54]) >= (35.5)) {
				var69 = 0.085288405;
			}
			else {
				var69 = -0.027543098;
			}
		}
	}
	else {
		if ((input[56]) >= (27.5)) {
			if ((input[54]) >= (43.5)) {
				var69 = -0.0047607003;
			}
			else {
				var69 = -0.060516335;
			}
		}
		else {
			if ((input[54]) >= (35.5)) {
				var69 = 0.11280997;
			}
			else {
				var69 = -0.025762504;
			}
		}
	}
	double var70;
	if ((input[51]) >= (0.02395)) {
		if ((input[56]) >= (2460.5)) {
			if ((input[61]) >= (536605.5)) {
				var70 = -0.063598275;
			}
			else {
				var70 = 0.00078968704;
			}
		}
		else {
			if ((input[54]) >= (37.5)) {
				var70 = 0.059355695;
			}
			else {
				var70 = -0.0049972394;
			}
		}
	}
	else {
		if ((input[56]) >= (5545.5)) {
			if ((input[37]) >= (1.10605)) {
				var70 = 0.085686244;
			}
			else {
				var70 = -0.07791107;
			}
		}
		else {
			if ((input[54]) >= (45.5)) {
				var70 = 0.06655069;
			}
			else {
				var70 = -0.020507429;
			}
		}
	}
	double var71;
	if ((input[54]) >= (48.5)) {
		if ((input[56]) >= (375017.0)) {
			if ((input[48]) >= (1295.4694)) {
				var71 = 0.0101059815;
			}
			else {
				var71 = -0.14583002;
			}
		}
		else {
			if ((input[40]) >= (0.5)) {
				var71 = 0.091370575;
			}
			else {
				var71 = -0.040960144;
			}
		}
	}
	else {
		if ((input[47]) >= (1.85875)) {
			if ((input[56]) >= (3836.5)) {
				var71 = -0.002947239;
			}
			else {
				var71 = 0.03829184;
			}
		}
		else {
			if ((input[56]) >= (2132.5)) {
				var71 = -0.03414299;
			}
			else {
				var71 = 0.0012271035;
			}
		}
	}
	double var72;
	if ((input[43]) >= (2.5)) {
		if ((input[36]) >= (5.4262)) {
			if ((input[22]) >= (0.5)) {
				var72 = -0.059263565;
			}
			else {
				var72 = 0.061394304;
			}
		}
		else {
			if ((input[53]) >= (0.75345004)) {
				var72 = -0.086049;
			}
			else {
				var72 = 0.031109056;
			}
		}
	}
	else {
		if ((input[61]) >= (329690.0)) {
			if ((input[60]) >= (1.5)) {
				var72 = -0.016498871;
			}
			else {
				var72 = -0.07862775;
			}
		}
		else {
			if ((input[47]) >= (2.7366)) {
				var72 = 0.026791772;
			}
			else {
				var72 = -0.004827942;
			}
		}
	}
	double var73;
	if ((input[0]) >= (6.0)) {
		if ((input[2]) >= (1.5)) {
			if ((input[38]) >= (1.70835)) {
				var73 = -0.076310694;
			}
			else {
				var73 = 0.038916446;
			}
		}
		else {
			if ((input[46]) >= (32.12185)) {
				var73 = -0.010603874;
			}
			else {
				var73 = -0.04827972;
			}
		}
	}
	else {
		if ((input[54]) >= (45.5)) {
			if ((input[56]) >= (8391.5)) {
				var73 = 0.0054778117;
			}
			else {
				var73 = 0.09395548;
			}
		}
		else {
			if ((input[56]) >= (2000.5)) {
				var73 = -0.011986489;
			}
			else {
				var73 = 0.018645054;
			}
		}
	}
	double var74;
	if ((input[1]) >= (12.7711)) {
		if ((input[62]) >= (7.5)) {
			if ((input[45]) >= (58.039047)) {
				var74 = -0.0679121;
			}
			else {
				var74 = 0.019591456;
			}
		}
		else {
			if ((input[25]) >= (1.225)) {
				var74 = -0.056262698;
			}
			else {
				var74 = 0.11451385;
			}
		}
	}
	else {
		if ((input[54]) >= (46.5)) {
			if ((input[56]) >= (33929.0)) {
				var74 = -0.007112544;
			}
			else {
				var74 = 0.086147204;
			}
		}
		else {
			if ((input[47]) >= (0.12405)) {
				var74 = 0.0025809829;
			}
			else {
				var74 = -0.034956522;
			}
		}
	}
	double var75;
	if ((input[6]) >= (2.5)) {
		if ((input[12]) >= (0.00035000002)) {
			if ((input[60]) >= (1.5)) {
				var75 = 0.024326127;
			}
			else {
				var75 = -0.0050581302;
			}
		}
		else {
			if ((input[39]) >= (0.75)) {
				var75 = -0.034070905;
			}
			else {
				var75 = 0.066003494;
			}
		}
	}
	else {
		if ((input[41]) >= (1.5)) {
			if ((input[60]) >= (1.5)) {
				var75 = 0.013861472;
			}
			else {
				var75 = -0.016841505;
			}
		}
		else {
			if ((input[60]) >= (1.5)) {
				var75 = -0.04814877;
			}
			else {
				var75 = -0.005038706;
			}
		}
	}
	double var76;
	if ((input[47]) >= (1.2039)) {
		if ((input[56]) >= (1588.5)) {
			if ((input[54]) >= (39.0)) {
				var76 = 0.010183427;
			}
			else {
				var76 = -0.020157753;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var76 = -0.02239625;
			}
			else {
				var76 = 0.060495626;
			}
		}
	}
	else {
		if ((input[56]) >= (204.5)) {
			if ((input[14]) >= (0.9)) {
				var76 = 0.01409079;
			}
			else {
				var76 = -0.030642677;
			}
		}
		else {
			if ((input[60]) >= (1.5)) {
				var76 = -0.055607807;
			}
			else {
				var76 = 0.079048716;
			}
		}
	}
	double var77;
	if ((input[54]) >= (48.5)) {
		if ((input[56]) >= (569376.5)) {
			if ((input[13]) >= (1.5)) {
				var77 = 0.040588934;
			}
			else {
				var77 = -0.087002166;
			}
		}
		else {
			if ((input[45]) >= (116.881)) {
				var77 = 0.024151333;
			}
			else {
				var77 = 0.097467944;
			}
		}
	}
	else {
		if ((input[56]) >= (4787.5)) {
			if ((input[60]) >= (1.5)) {
				var77 = 0.010400299;
			}
			else {
				var77 = -0.029541;
			}
		}
		else {
			if ((input[54]) >= (40.5)) {
				var77 = 0.036718853;
			}
			else {
				var77 = -0.008607181;
			}
		}
	}
	double var78;
	if ((input[47]) >= (2.4780998)) {
		if ((input[61]) >= (42850.0)) {
			if ((input[46]) >= (881.93567)) {
				var78 = 0.0034170211;
			}
			else {
				var78 = -0.050319653;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var78 = -0.01917263;
			}
			else {
				var78 = 0.03668141;
			}
		}
	}
	else {
		if ((input[56]) >= (1176.5)) {
			if ((input[48]) >= (80.237)) {
				var78 = 0.0049446383;
			}
			else {
				var78 = -0.03138119;
			}
		}
		else {
			if ((input[54]) >= (42.5)) {
				var78 = 0.09555418;
			}
			else {
				var78 = -0.00084052293;
			}
		}
	}
	double var79;
	if ((input[50]) >= (0.3639)) {
		if ((input[56]) >= (332.5)) {
			if ((input[54]) >= (39.0)) {
				var79 = 0.0097002825;
			}
			else {
				var79 = -0.011865369;
			}
		}
		else {
			if ((input[36]) >= (4.1792)) {
				var79 = 0.085447714;
			}
			else {
				var79 = -0.026543984;
			}
		}
	}
	else {
		if ((input[56]) >= (49.5)) {
			if ((input[43]) >= (1.5)) {
				var79 = -0.10089471;
			}
			else {
				var79 = -0.025028823;
			}
		}
		else {
			if ((input[60]) >= (1.5)) {
				var79 = -0.075933985;
			}
			else {
				var79 = 0.09578907;
			}
		}
	}
	double var80;
	if ((input[1]) >= (12.7711)) {
		if ((input[45]) >= (58.039047)) {
			if ((input[62]) >= (28.5)) {
				var80 = -0.077385;
			}
			else {
				var80 = -0.018114468;
			}
		}
		else {
			if ((input[4]) >= (5.5)) {
				var80 = -0.022140881;
			}
			else {
				var80 = 0.12286141;
			}
		}
	}
	else {
		if ((input[54]) >= (46.5)) {
			if ((input[61]) >= (23874.0)) {
				var80 = -0.0014884893;
			}
			else {
				var80 = 0.08763605;
			}
		}
		else {
			if ((input[51]) >= (0.02395)) {
				var80 = 0.0048052934;
			}
			else {
				var80 = -0.017524544;
			}
		}
	}
	double var81;
	if ((input[61]) >= (536605.5)) {
		if ((input[45]) >= (138.07675)) {
			var81 = 0.05176645;
		}
		else {
			if ((input[52]) >= (1948.9587)) {
				var81 = 0.018243147;
			}
			else {
				var81 = -0.06870518;
			}
		}
	}
	else {
		if ((input[47]) >= (2.7366)) {
			if ((input[49]) >= (1.8309)) {
				var81 = -0.02385336;
			}
			else {
				var81 = 0.026871493;
			}
		}
		else {
			if ((input[56]) >= (2132.5)) {
				var81 = -0.017551249;
			}
			else {
				var81 = 0.008301145;
			}
		}
	}
	double var82;
	if ((input[0]) >= (6.0)) {
		if ((input[3]) >= (0.45)) {
			if ((input[36]) >= (6.363)) {
				var82 = 0.026046133;
			}
			else {
				var82 = -0.02389179;
			}
		}
		else {
			if ((input[61]) >= (2740.0)) {
				var82 = -0.07351669;
			}
			else {
				var82 = -0.025196835;
			}
		}
	}
	else {
		if ((input[54]) >= (42.5)) {
			if ((input[56]) >= (12083.5)) {
				var82 = -0.009215885;
			}
			else {
				var82 = 0.050396997;
			}
		}
		else {
			if ((input[39]) >= (0.29165)) {
				var82 = 0.018194042;
			}
			else {
				var82 = -0.017983975;
			}
		}
	}
	double var83;
	if ((input[43]) >= (2.5)) {
		if ((input[47]) >= (0.71385)) {
			if ((input[46]) >= (8.529551)) {
				var83 = -0.044481017;
			}
			else {
				var83 = 0.061651636;
			}
		}
		else {
			if ((input[52]) >= (30.2583)) {
				var83 = 0.026906127;
			}
			else {
				var83 = -0.09928232;
			}
		}
	}
	else {
		if ((input[54]) >= (43.5)) {
			if ((input[56]) >= (3097.5)) {
				var83 = 0.0046232548;
			}
			else {
				var83 = 0.09130307;
			}
		}
		else {
			if ((input[60]) >= (1.5)) {
				var83 = 0.010737833;
			}
			else {
				var83 = -0.015487358;
			}
		}
	}
	double var84;
	if ((input[56]) >= (231.5)) {
		if ((input[47]) >= (0.37)) {
			if ((input[36]) >= (3.10145)) {
				var84 = 0.002624211;
			}
			else {
				var84 = -0.11650013;
			}
		}
		else {
			if ((input[54]) >= (43.5)) {
				var84 = -0.0032175102;
			}
			else {
				var84 = -0.06289212;
			}
		}
	}
	else {
		if ((input[54]) >= (37.5)) {
			if ((input[60]) >= (3.5)) {
				var84 = -0.07430368;
			}
			else {
				var84 = 0.10206943;
			}
		}
		else {
			if ((input[47]) >= (0.09819999)) {
				var84 = 0.045182604;
			}
			else {
				var84 = -0.063639194;
			}
		}
	}
	double var85;
	if ((input[25]) >= (2.36665)) {
		if ((input[3]) >= (1.225)) {
			if ((input[41]) >= (1.5)) {
				var85 = 0.036493056;
			}
			else {
				var85 = -0.053372066;
			}
		}
		else {
			if ((input[56]) >= (36041.5)) {
				var85 = -0.095629886;
			}
			else {
				var85 = -0.02994839;
			}
		}
	}
	else {
		if ((input[47]) >= (1.7038)) {
			if ((input[61]) >= (1992.5)) {
				var85 = 0.005038973;
			}
			else {
				var85 = 0.046883885;
			}
		}
		else {
			if ((input[56]) >= (204.5)) {
				var85 = -0.01484058;
			}
			else {
				var85 = 0.04080023;
			}
		}
	}
	double var86;
	if ((input[61]) >= (609648.5)) {
		if ((input[47]) >= (4.8554)) {
			if ((input[12]) >= (3.7557502)) {
				var86 = -0.057538237;
			}
			else {
				var86 = 0.092697255;
			}
		}
		else {
			if ((input[51]) >= (0.45515)) {
				var86 = -0.10883304;
			}
			else {
				var86 = -0.05012309;
			}
		}
	}
	else {
		if ((input[47]) >= (2.7366)) {
			if ((input[3]) >= (2.45)) {
				var86 = -0.050166238;
			}
			else {
				var86 = 0.021197675;
			}
		}
		else {
			if ((input[56]) >= (2132.5)) {
				var86 = -0.015909398;
			}
			else {
				var86 = 0.007862246;
			}
		}
	}
	double var87;
	if ((input[0]) >= (6.0)) {
		if ((input[36]) >= (7.0037003)) {
			if ((input[45]) >= (99.91065)) {
				var87 = -0.047098573;
			}
			else {
				var87 = 0.08754197;
			}
		}
		else {
			if ((input[2]) >= (0.5)) {
				var87 = -0.010664768;
			}
			else {
				var87 = -0.03912544;
			}
		}
	}
	else {
		if ((input[54]) >= (41.5)) {
			if ((input[56]) >= (4126.0)) {
				var87 = 0.00048040505;
			}
			else {
				var87 = 0.05337147;
			}
		}
		else {
			if ((input[39]) >= (0.29165)) {
				var87 = 0.01889064;
			}
			else {
				var87 = -0.021600164;
			}
		}
	}
	double var88;
	if ((input[48]) >= (8.991501)) {
		if ((input[56]) >= (7633.0)) {
			if ((input[60]) >= (1.5)) {
				var88 = 0.012083919;
			}
			else {
				var88 = -0.023545917;
			}
		}
		else {
			if ((input[54]) >= (39.0)) {
				var88 = 0.044852193;
			}
			else {
				var88 = 0.0008779545;
			}
		}
	}
	else {
		if ((input[61]) >= (1698.5)) {
			if ((input[54]) >= (46.5)) {
				var88 = 0.013900781;
			}
			else {
				var88 = -0.062591575;
			}
		}
		else {
			if ((input[60]) >= (5.5)) {
				var88 = -0.049572624;
			}
			else {
				var88 = 0.010401385;
			}
		}
	}
	double var89;
	if ((input[36]) >= (4.6136503)) {
		if ((input[56]) >= (1094.5)) {
			if ((input[48]) >= (10.84945)) {
				var89 = 0.0035135378;
			}
			else {
				var89 = -0.033900738;
			}
		}
		else {
			if ((input[54]) >= (40.5)) {
				var89 = 0.0867112;
			}
			else {
				var89 = 0.014583595;
			}
		}
	}
	else {
		if ((input[45]) >= (25.052)) {
			if ((input[54]) >= (44.5)) {
				var89 = 0.014580654;
			}
			else {
				var89 = -0.043728724;
			}
		}
		else {
			if ((input[54]) >= (36.5)) {
				var89 = 0.09174898;
			}
			else {
				var89 = -0.014401368;
			}
		}
	}
	double var90;
	if ((input[54]) >= (48.5)) {
		if ((input[61]) >= (55506.5)) {
			if ((input[36]) >= (5.23765)) {
				var90 = 0.03923236;
			}
			else {
				var90 = -0.1543302;
			}
		}
		else {
			if ((input[39]) >= (3.375)) {
				var90 = -0.067145966;
			}
			else {
				var90 = 0.09536691;
			}
		}
	}
	else {
		if ((input[61]) >= (31253.5)) {
			if ((input[60]) >= (1.5)) {
				var90 = 0.0035191455;
			}
			else {
				var90 = -0.04778027;
			}
		}
		else {
			if ((input[13]) >= (1.5)) {
				var90 = 0.027963653;
			}
			else {
				var90 = -0.0034455073;
			}
		}
	}
	double var91;
	if ((input[54]) >= (43.5)) {
		if ((input[56]) >= (2578.5)) {
			if ((input[51]) >= (0.1521)) {
				var91 = 0.03482979;
			}
			else {
				var91 = -0.018871501;
			}
		}
		else {
			if ((input[39]) >= (1.375)) {
				var91 = 0.032300815;
			}
			else {
				var91 = 0.10747619;
			}
		}
	}
	else {
		if ((input[60]) >= (1.5)) {
			if ((input[1]) >= (5.5665503)) {
				var91 = -0.03256778;
			}
			else {
				var91 = 0.016007053;
			}
		}
		else {
			if ((input[61]) >= (515.5)) {
				var91 = -0.026077682;
			}
			else {
				var91 = 0.034926586;
			}
		}
	}
	double var92;
	if ((input[54]) >= (37.5)) {
		if ((input[56]) >= (2132.5)) {
			if ((input[51]) >= (0.07875)) {
				var92 = 0.0065977843;
			}
			else {
				var92 = -0.025654448;
			}
		}
		else {
			if ((input[36]) >= (6.02435)) {
				var92 = 0.06814395;
			}
			else {
				var92 = 0.012247832;
			}
		}
	}
	else {
		if ((input[60]) >= (1.5)) {
			if ((input[45]) >= (47.1741)) {
				var92 = -0.013142335;
			}
			else {
				var92 = 0.055376917;
			}
		}
		else {
			if ((input[39]) >= (0.375)) {
				var92 = 0.0072093806;
			}
			else {
				var92 = -0.06578346;
			}
		}
	}
	double var93;
	if ((input[61]) >= (490888.0)) {
		if ((input[40]) >= (2.5)) {
			if ((input[1]) >= (0.40235)) {
				var93 = -0.09244566;
			}
			else {
				var93 = 0.026334671;
			}
		}
		else {
			if ((input[5]) >= (6.55)) {
				var93 = -0.0692125;
			}
			else {
				var93 = 0.010177824;
			}
		}
	}
	else {
		if ((input[54]) >= (43.5)) {
			if ((input[56]) >= (3086.5)) {
				var93 = 0.009488149;
			}
			else {
				var93 = 0.077855796;
			}
		}
		else {
			if ((input[14]) >= (0.70835)) {
				var93 = 0.006234136;
			}
			else {
				var93 = -0.01617918;
			}
		}
	}
	double var94;
	if ((input[0]) >= (6.0)) {
		if ((input[36]) >= (7.0037003)) {
			if ((input[57]) >= (0.5)) {
				var94 = 0.10341239;
			}
			else {
				var94 = -0.0138585;
			}
		}
		else {
			if ((input[45]) >= (31.32175)) {
				var94 = -0.031475715;
			}
			else {
				var94 = 0.000034982386;
			}
		}
	}
	else {
		if ((input[54]) >= (42.5)) {
			if ((input[56]) >= (13931.0)) {
				var94 = -0.008967002;
			}
			else {
				var94 = 0.043981623;
			}
		}
		else {
			if ((input[39]) >= (0.29165)) {
				var94 = 0.014669537;
			}
			else {
				var94 = -0.014776637;
			}
		}
	}
	double var95;
	if ((input[51]) >= (0.03575)) {
		if ((input[56]) >= (367.5)) {
			if ((input[54]) >= (39.0)) {
				var95 = 0.013573739;
			}
			else {
				var95 = -0.010012883;
			}
		}
		else {
			if ((input[60]) >= (5.5)) {
				var95 = -0.016580759;
			}
			else {
				var95 = 0.099799775;
			}
		}
	}
	else {
		if ((input[61]) >= (5031.0)) {
			if ((input[52]) >= (33.295647)) {
				var95 = -0.012619485;
			}
			else {
				var95 = -0.0742185;
			}
		}
		else {
			if ((input[12]) >= (0.75415003)) {
				var95 = -0.022049949;
			}
			else {
				var95 = 0.010208177;
			}
		}
	}
	double var96;
	if ((input[48]) >= (1.51955)) {
		if ((input[56]) >= (1176.5)) {
			if ((input[60]) >= (1.5)) {
				var96 = 0.010649412;
			}
			else {
				var96 = -0.011253705;
			}
		}
		else {
			if ((input[36]) >= (5.41)) {
				var96 = 0.053165782;
			}
			else {
				var96 = -0.008290759;
			}
		}
	}
	else {
		if ((input[16]) >= (0.6)) {
			if ((input[61]) >= (40.5)) {
				var96 = -0.05964117;
			}
			else {
				var96 = 0.014546252;
			}
		}
		else {
			if ((input[43]) >= (0.5)) {
				var96 = -0.05248911;
			}
			else {
				var96 = 0.044660952;
			}
		}
	}
	double var97;
	if ((input[43]) >= (2.5)) {
		if ((input[47]) >= (0.71385)) {
			if ((input[22]) >= (0.5)) {
				var97 = -0.056954693;
			}
			else {
				var97 = 0.028930131;
			}
		}
		else {
			if ((input[52]) >= (30.2583)) {
				var97 = 0.025555262;
			}
			else {
				var97 = -0.09826892;
			}
		}
	}
	else {
		if ((input[61]) >= (31405.0)) {
			if ((input[46]) >= (896.45435)) {
				var97 = 0.010271258;
			}
			else {
				var97 = -0.03813215;
			}
		}
		else {
			if ((input[54]) >= (43.5)) {
				var97 = 0.03242707;
			}
			else {
				var97 = -0.0012667173;
			}
		}
	}
	double var98;
	if ((input[1]) >= (12.7711)) {
		if ((input[45]) >= (58.039047)) {
			if ((input[62]) >= (30.5)) {
				var98 = -0.071407065;
			}
			else {
				var98 = -0.01639927;
			}
		}
		else {
			if ((input[47]) >= (3.4516)) {
				var98 = 0.14057049;
			}
			else {
				var98 = -0.008386807;
			}
		}
	}
	else {
		if ((input[54]) >= (46.5)) {
			if ((input[32]) >= (0.5)) {
				var98 = 0.04782097;
			}
			else {
				var98 = -0.12134903;
			}
		}
		else {
			if ((input[48]) >= (8.991751)) {
				var98 = 0.0046610227;
			}
			else {
				var98 = -0.01522551;
			}
		}
	}
	double var99;
	if ((input[61]) >= (609648.5)) {
		if ((input[47]) >= (4.8554)) {
			if ((input[32]) >= (2.5)) {
				var99 = 0.061826535;
			}
			else {
				var99 = -0.07101397;
			}
		}
		else {
			if ((input[1]) >= (9.0503)) {
				var99 = 0.0077150725;
			}
			else {
				var99 = -0.07637315;
			}
		}
	}
	else {
		if ((input[47]) >= (2.7366)) {
			if ((input[3]) >= (2.55)) {
				var99 = -0.05254034;
			}
			else {
				var99 = 0.018368628;
			}
		}
		else {
			if ((input[49]) >= (1.4051499)) {
				var99 = 0.007609889;
			}
			else {
				var99 = -0.01541208;
			}
		}
	}
	double var100;
	var100 = (1.0) / ((1.0) + (exp((0.0) - ((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((var0)+(var1)) + (var2)) + (var3)) + (var4)) + (var5)) + (var6)) + (var7)) + (var8)) + (var9)) + (var10)) + (var11)) + (var12)) + (var13)) + (var14)) + (var15)) + (var16)) + (var17)) + (var18)) + (var19)) + (var20)) + (var21)) + (var22)) + (var23)) + (var24)) + (var25)) + (var26)) + (var27)) + (var28)) + (var29)) + (var30)) + (var31)) + (var32)) + (var33)) + (var34)) + (var35)) + (var36)) + (var37)) + (var38)) + (var39)) + (var40)) + (var41)) + (var42)) + (var43)) + (var44)) + (var45)) + (var46)) + (var47)) + (var48)) + (var49)) + (var50)) + (var51)) + (var52)) + (var53)) + (var54)) + (var55)) + (var56)) + (var57)) + (var58)) + (var59)) + (var60)) + (var61)) + (var62)) + (var63)) + (var64)) + (var65)) + (var66)) + (var67)) + (var68)) + (var69)) + (var70)) + (var71)) + (var72)) + (var73)) + (var74)) + (var75)) + (var76)) + (var77)) + (var78)) + (var79)) + (var80)) + (var81)) + (var82)) + (var83)) + (var84)) + (var85)) + (var86)) + (var87)) + (var88)) + (var89)) + (var90)) + (var91)) + (var92)) + (var93)) + (var94)) + (var95)) + (var96)) + (var97)) + (var98)) + (var99)))));
	*output = var100 < 0.5 ? 0 : 1;
}

void PUclf2(double * input, double * output) {
	double var0;
	if ((input[60]) >= (1.5)) {
		if ((input[39]) >= (4.375)) {
			if ((input[60]) >= (7.5)) {
				var0 = -0.16902025;
			}
			else {
				var0 = -0.104347825;
			}
		}
		else {
			if ((input[60]) >= (7.5)) {
				var0 = -0.08948638;
			}
			else {
				var0 = -0.008228443;
			}
		}
	}
	else {
		if ((input[61]) >= (977.5)) {
			if ((input[41]) >= (3.5)) {
				var0 = 0.08744366;
			}
			else {
				var0 = 0.02024024;
			}
		}
		else {
			if ((input[39]) >= (2.125)) {
				var0 = 0.1104262;
			}
			else {
				var0 = 0.18646169;
			}
		}
	}
	double var1;
	if ((input[60]) >= (1.5)) {
		if ((input[39]) >= (3.125)) {
			if ((input[60]) >= (7.5)) {
				var1 = -0.15210155;
			}
			else {
				var1 = -0.09095308;
			}
		}
		else {
			if ((input[57]) >= (0.5)) {
				var1 = -0.010544513;
			}
			else {
				var1 = -0.085147284;
			}
		}
	}
	else {
		if ((input[61]) >= (977.5)) {
			if ((input[1]) >= (0.1528)) {
				var1 = 0.029631719;
			}
			else {
				var1 = 0.11642785;
			}
		}
		else {
			if ((input[1]) >= (0.69044995)) {
				var1 = 0.100207284;
			}
			else {
				var1 = 0.17072986;
			}
		}
	}
	double var2;
	if ((input[60]) >= (1.5)) {
		if ((input[39]) >= (4.375)) {
			if ((input[60]) >= (7.5)) {
				var2 = -0.1407564;
			}
			else {
				var2 = -0.085706554;
			}
		}
		else {
			if ((input[60]) >= (6.5)) {
				var2 = -0.06822466;
			}
			else {
				var2 = 0.0012435982;
			}
		}
	}
	else {
		if ((input[61]) >= (790.5)) {
			if ((input[41]) >= (3.5)) {
				var2 = 0.07924519;
			}
			else {
				var2 = 0.016175982;
			}
		}
		else {
			if ((input[39]) >= (2.125)) {
				var2 = 0.091729954;
			}
			else {
				var2 = 0.15829901;
			}
		}
	}
	double var3;
	if ((input[60]) >= (1.5)) {
		if ((input[39]) >= (4.375)) {
			if ((input[60]) >= (7.5)) {
				var3 = -0.13035986;
			}
			else {
				var3 = -0.07797083;
			}
		}
		else {
			if ((input[57]) >= (0.5)) {
				var3 = -0.009028307;
			}
			else {
				var3 = -0.07242953;
			}
		}
	}
	else {
		if ((input[61]) >= (665.5)) {
			if ((input[1]) >= (0.123500004)) {
				var3 = 0.027206471;
			}
			else {
				var3 = 0.11083815;
			}
		}
		else {
			if ((input[1]) >= (0.8735)) {
				var3 = 0.081534795;
			}
			else {
				var3 = 0.14867148;
			}
		}
	}
	double var4;
	if ((input[60]) >= (1.5)) {
		if ((input[39]) >= (5.29165)) {
			if ((input[60]) >= (8.5)) {
				var4 = -0.13382243;
			}
			else {
				var4 = -0.094679676;
			}
		}
		else {
			if ((input[57]) >= (0.5)) {
				var4 = -0.013755256;
			}
			else {
				var4 = -0.07428499;
			}
		}
	}
	else {
		if ((input[61]) >= (977.5)) {
			if ((input[39]) >= (2.41665)) {
				var4 = -0.034126434;
			}
			else {
				var4 = 0.0430894;
			}
		}
		else {
			if ((input[39]) >= (2.125)) {
				var4 = 0.0703114;
			}
			else {
				var4 = 0.13814464;
			}
		}
	}
	double var5;
	if ((input[60]) >= (1.5)) {
		if ((input[39]) >= (3.125)) {
			if ((input[60]) >= (7.5)) {
				var5 = -0.113497905;
			}
			else {
				var5 = -0.06372032;
			}
		}
		else {
			if ((input[60]) >= (6.5)) {
				var5 = -0.04984008;
			}
			else {
				var5 = 0.004945257;
			}
		}
	}
	else {
		if ((input[1]) >= (0.1528)) {
			if ((input[61]) >= (2029.5)) {
				var5 = 0.0084837815;
			}
			else {
				var5 = 0.072192095;
			}
		}
		else {
			if ((input[12]) >= (0.023899999)) {
				var5 = 0.09530807;
			}
			else {
				var5 = 0.14079666;
			}
		}
	}
	double var6;
	if ((input[60]) >= (1.5)) {
		if ((input[41]) >= (1.5)) {
			if ((input[60]) >= (7.5)) {
				var6 = -0.05174088;
			}
			else {
				var6 = 0.0006932119;
			}
		}
		else {
			if ((input[60]) >= (8.5)) {
				var6 = -0.11190388;
			}
			else {
				var6 = -0.06498034;
			}
		}
	}
	else {
		if ((input[12]) >= (0.61085)) {
			if ((input[41]) >= (2.5)) {
				var6 = 0.045896128;
			}
			else {
				var6 = -0.012752193;
			}
		}
		else {
			if ((input[2]) >= (1.5)) {
				var6 = 0.06366315;
			}
			else {
				var6 = 0.12665498;
			}
		}
	}
	double var7;
	if ((input[60]) >= (1.5)) {
		if ((input[39]) >= (5.29165)) {
			if ((input[60]) >= (5.5)) {
				var7 = -0.11209407;
			}
			else {
				var7 = -0.06883414;
			}
		}
		else {
			if ((input[45]) >= (34.838852)) {
				var7 = -0.041045588;
			}
			else {
				var7 = 0.03331416;
			}
		}
	}
	else {
		if ((input[61]) >= (463.5)) {
			if ((input[41]) >= (3.5)) {
				var7 = 0.06204688;
			}
			else {
				var7 = 0.0043154648;
			}
		}
		else {
			if ((input[1]) >= (1.4956)) {
				var7 = 0.055886548;
			}
			else {
				var7 = 0.12239792;
			}
		}
	}
	double var8;
	if ((input[60]) >= (1.5)) {
		if ((input[41]) >= (2.5)) {
			if ((input[1]) >= (1.2974)) {
				var8 = -0.025747431;
			}
			else {
				var8 = 0.028160676;
			}
		}
		else {
			if ((input[39]) >= (6.58335)) {
				var8 = -0.10074282;
			}
			else {
				var8 = -0.051867653;
			}
		}
	}
	else {
		if ((input[1]) >= (0.1216)) {
			if ((input[61]) >= (248.5)) {
				var8 = 0.017103221;
			}
			else {
				var8 = 0.09180996;
			}
		}
		else {
			if ((input[61]) >= (2539.5)) {
				var8 = 0.03694941;
			}
			else {
				var8 = 0.12033927;
			}
		}
	}
	double var9;
	if ((input[60]) >= (1.5)) {
		if ((input[39]) >= (5.625)) {
			if ((input[60]) >= (8.5)) {
				var9 = -0.10459745;
			}
			else {
				var9 = -0.06339594;
			}
		}
		else {
			if ((input[57]) >= (0.5)) {
				var9 = -0.0034613886;
			}
			else {
				var9 = -0.051569697;
			}
		}
	}
	else {
		if ((input[12]) >= (0.07665)) {
			if ((input[61]) >= (2666.0)) {
				var9 = -0.0029589278;
			}
			else {
				var9 = 0.054150797;
			}
		}
		else {
			if ((input[1]) >= (0.01025)) {
				var9 = 0.08039301;
			}
			else {
				var9 = 0.12262628;
			}
		}
	}
	double var10;
	if ((input[60]) >= (1.5)) {
		if ((input[41]) >= (2.5)) {
			if ((input[1]) >= (1.0015)) {
				var10 = -0.018992333;
			}
			else {
				var10 = 0.030804995;
			}
		}
		else {
			if ((input[12]) >= (4.0532)) {
				var10 = -0.08471819;
			}
			else {
				var10 = -0.037772696;
			}
		}
	}
	else {
		if ((input[12]) >= (0.07665)) {
			if ((input[61]) >= (299.5)) {
				var10 = 0.013116427;
			}
			else {
				var10 = 0.07921816;
			}
		}
		else {
			if ((input[1]) >= (0.0072999997)) {
				var10 = 0.0766008;
			}
			else {
				var10 = 0.11926303;
			}
		}
	}
	double var11;
	if ((input[60]) >= (4.5)) {
		if ((input[39]) >= (5.375)) {
			if ((input[47]) >= (2.5713)) {
				var11 = -0.06896937;
			}
			else {
				var11 = -0.105121516;
			}
		}
		else {
			if ((input[47]) >= (1.2732)) {
				var11 = -0.019760435;
			}
			else {
				var11 = -0.077464856;
			}
		}
	}
	else {
		if ((input[1]) >= (0.23585)) {
			if ((input[41]) >= (2.5)) {
				var11 = 0.03377813;
			}
			else {
				var11 = -0.020393798;
			}
		}
		else {
			if ((input[61]) >= (2578.5)) {
				var11 = 0.020675441;
			}
			else {
				var11 = 0.10214472;
			}
		}
	}
	double var12;
	if ((input[12]) >= (0.3794)) {
		if ((input[41]) >= (1.5)) {
			if ((input[60]) >= (5.5)) {
				var12 = -0.031323314;
			}
			else {
				var12 = 0.017762132;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var12 = -0.095840044;
			}
			else {
				var12 = -0.047348093;
			}
		}
	}
	else {
		if ((input[1]) >= (0.02)) {
			if ((input[60]) >= (2.5)) {
				var12 = 0.00061671267;
			}
			else {
				var12 = 0.056795396;
			}
		}
		else {
			if ((input[39]) >= (4.375)) {
				var12 = -0.0032018356;
			}
			else {
				var12 = 0.11003021;
			}
		}
	}
	double var13;
	if ((input[1]) >= (1.02)) {
		if ((input[39]) >= (2.375)) {
			if ((input[0]) >= (6.0)) {
				var13 = -0.088338606;
			}
			else {
				var13 = -0.04385018;
			}
		}
		else {
			if ((input[54]) >= (41.5)) {
				var13 = 0.017972993;
			}
			else {
				var13 = -0.024507703;
			}
		}
	}
	else {
		if ((input[12]) >= (0.023899999)) {
			if ((input[60]) >= (4.5)) {
				var13 = -0.009980305;
			}
			else {
				var13 = 0.043329354;
			}
		}
		else {
			if ((input[39]) >= (4.375)) {
				var13 = -0.009943275;
			}
			else {
				var13 = 0.1056775;
			}
		}
	}
	double var14;
	if ((input[60]) >= (0.5)) {
		if ((input[1]) >= (2.8003001)) {
			if ((input[46]) >= (12.8889)) {
				var14 = -0.034327682;
			}
			else {
				var14 = -0.09555499;
			}
		}
		else {
			if ((input[45]) >= (37.46975)) {
				var14 = -0.006033036;
			}
			else {
				var14 = 0.053100117;
			}
		}
	}
	else {
		if ((input[56]) >= (2025.0)) {
			if ((input[54]) >= (43.5)) {
				var14 = 0.056204308;
			}
			else {
				var14 = -0.016887477;
			}
		}
		else {
			if ((input[56]) >= (559.5)) {
				var14 = 0.06481059;
			}
			else {
				var14 = 0.107172124;
			}
		}
	}
	double var15;
	if ((input[41]) >= (3.5)) {
		if ((input[56]) >= (1242.5)) {
			if ((input[47]) >= (2.5766)) {
				var15 = 0.044191036;
			}
			else {
				var15 = -0.0036957858;
			}
		}
		else {
			if ((input[54]) >= (37.5)) {
				var15 = 0.107203424;
			}
			else {
				var15 = 0.05571982;
			}
		}
	}
	else {
		if ((input[60]) >= (5.5)) {
			if ((input[39]) >= (6.375)) {
				var15 = -0.0831229;
			}
			else {
				var15 = -0.0343957;
			}
		}
		else {
			if ((input[12]) >= (1.32665)) {
				var15 = -0.018008206;
			}
			else {
				var15 = 0.030847505;
			}
		}
	}
	double var16;
	if ((input[12]) >= (0.1616)) {
		if ((input[57]) >= (0.5)) {
			if ((input[47]) >= (0.25855)) {
				var16 = 0.0075643705;
			}
			else {
				var16 = -0.09031114;
			}
		}
		else {
			if ((input[51]) >= (0.060149997)) {
				var16 = -0.030904064;
			}
			else {
				var16 = -0.082544945;
			}
		}
	}
	else {
		if ((input[1]) >= (0.0181)) {
			if ((input[52]) >= (0.62505)) {
				var16 = 0.044384368;
			}
			else {
				var16 = -0.037608813;
			}
		}
		else {
			if ((input[39]) >= (2.125)) {
				var16 = 0.031487975;
			}
			else {
				var16 = 0.10425617;
			}
		}
	}
	double var17;
	if ((input[60]) >= (0.5)) {
		if ((input[39]) >= (5.29165)) {
			if ((input[51]) >= (0.01345)) {
				var17 = -0.05370441;
			}
			else {
				var17 = -0.10016847;
			}
		}
		else {
			if ((input[1]) >= (2.2847)) {
				var17 = -0.024359556;
			}
			else {
				var17 = 0.015437761;
			}
		}
	}
	else {
		if ((input[56]) >= (791.5)) {
			if ((input[54]) >= (42.5)) {
				var17 = 0.05296286;
			}
			else {
				var17 = -0.008249776;
			}
		}
		else {
			if ((input[54]) >= (36.5)) {
				var17 = 0.10454307;
			}
			else {
				var17 = 0.04915189;
			}
		}
	}
	double var18;
	if ((input[41]) >= (3.5)) {
		if ((input[56]) >= (832.5)) {
			if ((input[47]) >= (2.2229)) {
				var18 = 0.037322838;
			}
			else {
				var18 = -0.007006451;
			}
		}
		else {
			if ((input[54]) >= (37.5)) {
				var18 = 0.10438905;
			}
			else {
				var18 = 0.053385776;
			}
		}
	}
	else {
		if ((input[12]) >= (4.46335)) {
			if ((input[0]) >= (6.0)) {
				var18 = -0.07939251;
			}
			else {
				var18 = -0.0315683;
			}
		}
		else {
			if ((input[60]) >= (4.5)) {
				var18 = -0.021504112;
			}
			else {
				var18 = 0.018053237;
			}
		}
	}
	double var19;
	if ((input[1]) >= (0.096200004)) {
		if ((input[0]) >= (6.0)) {
			if ((input[45]) >= (38.9263)) {
				var19 = -0.06423802;
			}
			else {
				var19 = 0.00050000823;
			}
		}
		else {
			if ((input[54]) >= (41.5)) {
				var19 = 0.03195565;
			}
			else {
				var19 = -0.014693002;
			}
		}
	}
	else {
		if ((input[12]) >= (0.0112)) {
			if ((input[52]) >= (0.60580003)) {
				var19 = 0.041421857;
			}
			else {
				var19 = -0.03848408;
			}
		}
		else {
			if ((input[39]) >= (4.375)) {
				var19 = -0.011147971;
			}
			else {
				var19 = 0.09752344;
			}
		}
	}
	double var20;
	if ((input[41]) >= (2.5)) {
		if ((input[12]) >= (0.023899999)) {
			if ((input[56]) >= (3214.5)) {
				var20 = -0.0047705234;
			}
			else {
				var20 = 0.028763954;
			}
		}
		else {
			if ((input[1]) >= (0.0072999997)) {
				var20 = 0.05101749;
			}
			else {
				var20 = 0.09896944;
			}
		}
	}
	else {
		if ((input[24]) >= (8.0)) {
			if ((input[41]) >= (0.5)) {
				var20 = -0.06055669;
			}
			else {
				var20 = -0.09263044;
			}
		}
		else {
			if ((input[47]) >= (0.29084998)) {
				var20 = -0.011224775;
			}
			else {
				var20 = -0.08778937;
			}
		}
	}
	double var21;
	if ((input[60]) >= (0.5)) {
		if ((input[39]) >= (5.29165)) {
			if ((input[51]) >= (0.01345)) {
				var21 = -0.04304746;
			}
			else {
				var21 = -0.08996881;
			}
		}
		else {
			if ((input[45]) >= (37.22075)) {
				var21 = -0.010768598;
			}
			else {
				var21 = 0.03306155;
			}
		}
	}
	else {
		if ((input[56]) >= (599.5)) {
			if ((input[54]) >= (41.5)) {
				var21 = 0.041848313;
			}
			else {
				var21 = -0.014385275;
			}
		}
		else {
			if ((input[54]) >= (37.5)) {
				var21 = 0.106664255;
			}
			else {
				var21 = 0.06078762;
			}
		}
	}
	double var22;
	if ((input[12]) >= (1.2573)) {
		if ((input[0]) >= (6.0)) {
			if ((input[45]) >= (38.9258)) {
				var22 = -0.06388026;
			}
			else {
				var22 = -0.0056183278;
			}
		}
		else {
			if ((input[54]) >= (42.5)) {
				var22 = 0.03145242;
			}
			else {
				var22 = -0.021480689;
			}
		}
	}
	else {
		if ((input[1]) >= (0.0093)) {
			if ((input[54]) >= (40.5)) {
				var22 = 0.0465461;
			}
			else {
				var22 = 0.003906631;
			}
		}
		else {
			if ((input[39]) >= (2.125)) {
				var22 = 0.022839395;
			}
			else {
				var22 = 0.09310853;
			}
		}
	}
	double var23;
	if ((input[1]) >= (1.3296001)) {
		if ((input[51]) >= (0.01555)) {
			if ((input[60]) >= (10.5)) {
				var23 = -0.036381423;
			}
			else {
				var23 = 0.0027248368;
			}
		}
		else {
			if ((input[56]) >= (59.5)) {
				var23 = -0.079969294;
			}
			else {
				var23 = 0.05026699;
			}
		}
	}
	else {
		if ((input[12]) >= (0.0072999997)) {
			if ((input[54]) >= (40.5)) {
				var23 = 0.04611862;
			}
			else {
				var23 = 0.0026060462;
			}
		}
		else {
			if ((input[39]) >= (2.125)) {
				var23 = 0.023185557;
			}
			else {
				var23 = 0.09212065;
			}
		}
	}
	double var24;
	if ((input[60]) >= (4.5)) {
		if ((input[47]) >= (1.2732999)) {
			if ((input[25]) >= (5.225)) {
				var24 = -0.05697613;
			}
			else {
				var24 = -0.0032421555;
			}
		}
		else {
			if ((input[48]) >= (1.37965)) {
				var24 = -0.051443953;
			}
			else {
				var24 = -0.10670872;
			}
		}
	}
	else {
		if ((input[61]) >= (463.5)) {
			if ((input[54]) >= (41.5)) {
				var24 = 0.026687657;
			}
			else {
				var24 = -0.021882432;
			}
		}
		else {
			if ((input[47]) >= (0.65680003)) {
				var24 = 0.083529815;
			}
			else {
				var24 = 0.010472157;
			}
		}
	}
	double var25;
	if ((input[41]) >= (3.5)) {
		if ((input[56]) >= (1905.5)) {
			if ((input[47]) >= (2.5766)) {
				var25 = 0.025599543;
			}
			else {
				var25 = -0.013206373;
			}
		}
		else {
			if ((input[54]) >= (37.5)) {
				var25 = 0.08668821;
			}
			else {
				var25 = 0.02397727;
			}
		}
	}
	else {
		if ((input[12]) >= (4.46335)) {
			if ((input[48]) >= (11.76705)) {
				var25 = -0.026217831;
			}
			else {
				var25 = -0.0834275;
			}
		}
		else {
			if ((input[51]) >= (0.00735)) {
				var25 = 0.0067314357;
			}
			else {
				var25 = -0.042053625;
			}
		}
	}
	double var26;
	if ((input[12]) >= (0.02)) {
		if ((input[0]) >= (6.0)) {
			if ((input[45]) >= (38.9263)) {
				var26 = -0.048520215;
			}
			else {
				var26 = -0.00054612925;
			}
		}
		else {
			if ((input[54]) >= (42.5)) {
				var26 = 0.0330936;
			}
			else {
				var26 = -0.0058000726;
			}
		}
	}
	else {
		if ((input[60]) >= (2.5)) {
			if ((input[50]) >= (0.09815)) {
				var26 = 0.04916079;
			}
			else {
				var26 = -0.122649305;
			}
		}
		else {
			if ((input[32]) >= (1.5)) {
				var26 = 0.088484325;
			}
			else {
				var26 = 0.028355598;
			}
		}
	}
	double var27;
	if ((input[1]) >= (2.2876)) {
		if ((input[47]) >= (1.11995)) {
			if ((input[25]) >= (7.1)) {
				var27 = -0.07181862;
			}
			else {
				var27 = -0.00673376;
			}
		}
		else {
			if ((input[56]) >= (58.5)) {
				var27 = -0.067209505;
			}
			else {
				var27 = 0.058436066;
			}
		}
	}
	else {
		if ((input[12]) >= (0.0072999997)) {
			if ((input[54]) >= (40.5)) {
				var27 = 0.033914614;
			}
			else {
				var27 = 0.00048294463;
			}
		}
		else {
			if ((input[60]) >= (1.5)) {
				var27 = 0.016717589;
			}
			else {
				var27 = 0.08588102;
			}
		}
	}
	double var28;
	if ((input[41]) >= (1.5)) {
		if ((input[56]) >= (276.5)) {
			if ((input[47]) >= (1.1292)) {
				var28 = 0.009167661;
			}
			else {
				var28 = -0.04645611;
			}
		}
		else {
			if ((input[60]) >= (3.5)) {
				var28 = -0.0321499;
			}
			else {
				var28 = 0.076250724;
			}
		}
	}
	else {
		if ((input[45]) >= (33.11815)) {
			if ((input[48]) >= (1.87625)) {
				var28 = -0.028222788;
			}
			else {
				var28 = -0.08336165;
			}
		}
		else {
			if ((input[1]) >= (18.7207)) {
				var28 = -0.10472528;
			}
			else {
				var28 = 0.0640808;
			}
		}
	}
	double var29;
	if ((input[12]) >= (0.4985)) {
		if ((input[47]) >= (1.1800499)) {
			if ((input[38]) >= (2.125)) {
				var29 = -0.032443818;
			}
			else {
				var29 = 0.0057287565;
			}
		}
		else {
			if ((input[54]) >= (40.5)) {
				var29 = -0.0038674797;
			}
			else {
				var29 = -0.055489715;
			}
		}
	}
	else {
		if ((input[6]) >= (4.5)) {
			if ((input[56]) >= (2514.5)) {
				var29 = 0.022341268;
			}
			else {
				var29 = 0.085168704;
			}
		}
		else {
			if ((input[61]) >= (747.5)) {
				var29 = -0.003757452;
			}
			else {
				var29 = 0.036772683;
			}
		}
	}
	double var30;
	if ((input[60]) >= (0.5)) {
		if ((input[51]) >= (0.0069500003)) {
			if ((input[24]) >= (8.0)) {
				var30 = -0.041435536;
			}
			else {
				var30 = 0.0028041631;
			}
		}
		else {
			if ((input[56]) >= (60.5)) {
				var30 = -0.07465262;
			}
			else {
				var30 = 0.017115137;
			}
		}
	}
	else {
		if ((input[56]) >= (2999.5)) {
			if ((input[54]) >= (43.5)) {
				var30 = 0.022849005;
			}
			else {
				var30 = -0.03777295;
			}
		}
		else {
			if ((input[54]) >= (40.5)) {
				var30 = 0.0984279;
			}
			else {
				var30 = 0.03770206;
			}
		}
	}
	double var31;
	if ((input[16]) >= (0.1)) {
		if ((input[10]) >= (1.5)) {
			if ((input[56]) >= (923.5)) {
				var31 = -0.0017117327;
			}
			else {
				var31 = 0.039988954;
			}
		}
		else {
			if ((input[41]) >= (0.5)) {
				var31 = -0.013298106;
			}
			else {
				var31 = -0.054580417;
			}
		}
	}
	else {
		if ((input[56]) >= (2665.0)) {
			if ((input[54]) >= (42.5)) {
				var31 = 0.03244937;
			}
			else {
				var31 = -0.024522943;
			}
		}
		else {
			if ((input[32]) >= (3.5)) {
				var31 = 0.084987074;
			}
			else {
				var31 = 0.04000469;
			}
		}
	}
	double var32;
	if ((input[1]) >= (2.5571)) {
		if ((input[50]) >= (1.5962)) {
			if ((input[60]) >= (13.5)) {
				var32 = -0.029873922;
			}
			else {
				var32 = 0.000065105385;
			}
		}
		else {
			if ((input[56]) >= (59.5)) {
				var32 = -0.06765398;
			}
			else {
				var32 = 0.060221035;
			}
		}
	}
	else {
		if ((input[61]) >= (804.5)) {
			if ((input[47]) >= (2.2948)) {
				var32 = 0.014965407;
			}
			else {
				var32 = -0.021272618;
			}
		}
		else {
			if ((input[54]) >= (37.5)) {
				var32 = 0.070193216;
			}
			else {
				var32 = 0.007813884;
			}
		}
	}
	double var33;
	if ((input[39]) >= (2.125)) {
		if ((input[51]) >= (0.01305)) {
			if ((input[1]) >= (4.9883)) {
				var33 = -0.0277913;
			}
			else {
				var33 = 0.00615612;
			}
		}
		else {
			if ((input[56]) >= (59.5)) {
				var33 = -0.062925965;
			}
			else {
				var33 = 0.020241402;
			}
		}
	}
	else {
		if ((input[61]) >= (2658.5)) {
			if ((input[54]) >= (42.5)) {
				var33 = 0.012434613;
			}
			else {
				var33 = -0.019172953;
			}
		}
		else {
			if ((input[54]) >= (40.5)) {
				var33 = 0.088264056;
			}
			else {
				var33 = 0.021910854;
			}
		}
	}
	double var34;
	if ((input[56]) >= (159.5)) {
		if ((input[47]) >= (1.11995)) {
			if ((input[61]) >= (66392.5)) {
				var34 = -0.029119113;
			}
			else {
				var34 = 0.010244716;
			}
		}
		else {
			if ((input[47]) >= (0.1449)) {
				var34 = -0.0308796;
			}
			else {
				var34 = -0.10681315;
			}
		}
	}
	else {
		if ((input[60]) >= (2.5)) {
			if ((input[47]) >= (0.53235)) {
				var34 = 0.007721579;
			}
			else {
				var34 = -0.09684865;
			}
		}
		else {
			if ((input[54]) >= (37.5)) {
				var34 = 0.104130246;
			}
			else {
				var34 = 0.05296541;
			}
		}
	}
	double var35;
	if ((input[16]) >= (0.1)) {
		if ((input[0]) >= (6.0)) {
			if ((input[45]) >= (61.76905)) {
				var35 = -0.044658646;
			}
			else {
				var35 = -0.008391549;
			}
		}
		else {
			if ((input[56]) >= (272.5)) {
				var35 = -0.0033895639;
			}
			else {
				var35 = 0.045288365;
			}
		}
	}
	else {
		if ((input[56]) >= (1778.0)) {
			if ((input[54]) >= (42.5)) {
				var35 = 0.031286675;
			}
			else {
				var35 = -0.014275682;
			}
		}
		else {
			if ((input[32]) >= (3.5)) {
				var35 = 0.08261087;
			}
			else {
				var35 = 0.03511566;
			}
		}
	}
	double var36;
	if ((input[41]) >= (2.5)) {
		if ((input[56]) >= (189.5)) {
			if ((input[47]) >= (1.4592)) {
				var36 = 0.014361893;
			}
			else {
				var36 = -0.024755454;
			}
		}
		else {
			if ((input[60]) >= (1.5)) {
				var36 = -0.0020001535;
			}
			else {
				var36 = 0.080258235;
			}
		}
	}
	else {
		if ((input[24]) >= (8.0)) {
			if ((input[44]) >= (0.5)) {
				var36 = -0.037125047;
			}
			else {
				var36 = -0.07337111;
			}
		}
		else {
			if ((input[47]) >= (0.1449)) {
				var36 = -0.0029614698;
			}
			else {
				var36 = -0.072154246;
			}
		}
	}
	double var37;
	if ((input[12]) >= (1.2573)) {
		if ((input[52]) >= (9.433901)) {
			if ((input[37]) >= (2.5147)) {
				var37 = -0.043584246;
			}
			else {
				var37 = 0.0016235253;
			}
		}
		else {
			if ((input[60]) >= (6.5)) {
				var37 = -0.07527575;
			}
			else {
				var37 = -0.019364636;
			}
		}
	}
	else {
		if ((input[56]) >= (273.5)) {
			if ((input[46]) >= (2.37965)) {
				var37 = 0.0125501845;
			}
			else {
				var37 = -0.046084713;
			}
		}
		else {
			if ((input[60]) >= (2.5)) {
				var37 = -0.020244854;
			}
			else {
				var37 = 0.06815379;
			}
		}
	}
	double var38;
	if ((input[4]) >= (0.5)) {
		if ((input[47]) >= (0.22915)) {
			if ((input[56]) >= (161.5)) {
				var38 = -0.009100832;
			}
			else {
				var38 = 0.051833894;
			}
		}
		else {
			if ((input[56]) >= (60.5)) {
				var38 = -0.09024558;
			}
			else {
				var38 = 0.019520132;
			}
		}
	}
	else {
		if ((input[56]) >= (977.5)) {
			if ((input[54]) >= (43.5)) {
				var38 = 0.035477746;
			}
			else {
				var38 = -0.0045788568;
			}
		}
		else {
			if ((input[60]) >= (7.5)) {
				var38 = -0.03680656;
			}
			else {
				var38 = 0.054348834;
			}
		}
	}
	double var39;
	if ((input[0]) >= (6.0)) {
		if ((input[45]) >= (38.76025)) {
			if ((input[60]) >= (10.5)) {
				var39 = -0.04948095;
			}
			else {
				var39 = -0.017846383;
			}
		}
		else {
			if ((input[51]) >= (0.12205)) {
				var39 = 0.04028754;
			}
			else {
				var39 = -0.024307474;
			}
		}
	}
	else {
		if ((input[56]) >= (273.5)) {
			if ((input[54]) >= (41.5)) {
				var39 = 0.021275176;
			}
			else {
				var39 = -0.009989007;
			}
		}
		else {
			if ((input[51]) >= (0.00825)) {
				var39 = 0.08940194;
			}
			else {
				var39 = -0.00043733456;
			}
		}
	}
	double var40;
	if ((input[56]) >= (101852.5)) {
		if ((input[38]) >= (1.875)) {
			if ((input[54]) >= (42.5)) {
				var40 = -0.023473848;
			}
			else {
				var40 = -0.05841613;
			}
		}
		else {
			if ((input[50]) >= (42.16505)) {
				var40 = -0.08627241;
			}
			else {
				var40 = 0.004782475;
			}
		}
	}
	else {
		if ((input[47]) >= (2.2948499)) {
			if ((input[56]) >= (2373.5)) {
				var40 = 0.009831716;
			}
			else {
				var40 = 0.054004908;
			}
		}
		else {
			if ((input[60]) >= (7.5)) {
				var40 = -0.03198765;
			}
			else {
				var40 = 0.0014987421;
			}
		}
	}
	double var41;
	if ((input[1]) >= (0.0054)) {
		if ((input[47]) >= (0.13175)) {
			if ((input[56]) >= (101852.5)) {
				var41 = -0.028467461;
			}
			else {
				var41 = 0.0037816286;
			}
		}
		else {
			if ((input[61]) >= (43.5)) {
				var41 = -0.10319885;
			}
			else {
				var41 = -0.014131643;
			}
		}
	}
	else {
		if ((input[51]) >= (0.00005)) {
			if ((input[56]) >= (4347.5)) {
				var41 = -0.009117569;
			}
			else {
				var41 = 0.07832008;
			}
		}
		else {
			if ((input[60]) >= (1.5)) {
				var41 = -0.085440725;
			}
			else {
				var41 = 0.045079794;
			}
		}
	}
	double var42;
	if ((input[54]) >= (40.5)) {
		if ((input[56]) >= (3806.5)) {
			if ((input[47]) >= (2.15)) {
				var42 = 0.005757561;
			}
			else {
				var42 = -0.042873155;
			}
		}
		else {
			if ((input[60]) >= (5.5)) {
				var42 = -0.013054922;
			}
			else {
				var42 = 0.076115556;
			}
		}
	}
	else {
		if ((input[45]) >= (49.17675)) {
			if ((input[47]) >= (0.11745)) {
				var42 = -0.0130111715;
			}
			else {
				var42 = -0.076446004;
			}
		}
		else {
			if ((input[61]) >= (805.5)) {
				var42 = 0.0020206277;
			}
			else {
				var42 = 0.040893592;
			}
		}
	}
	double var43;
	if ((input[0]) >= (6.0)) {
		if ((input[39]) >= (8.375)) {
			if ((input[25]) >= (6.775)) {
				var43 = -0.08607771;
			}
			else {
				var43 = -0.04531232;
			}
		}
		else {
			if ((input[48]) >= (23.367199)) {
				var43 = -0.0012342798;
			}
			else {
				var43 = -0.030763209;
			}
		}
	}
	else {
		if ((input[56]) >= (208.5)) {
			if ((input[47]) >= (0.96535003)) {
				var43 = 0.00604343;
			}
			else {
				var43 = -0.03985497;
			}
		}
		else {
			if ((input[51]) >= (0.00005)) {
				var43 = 0.07880944;
			}
			else {
				var43 = -0.016224284;
			}
		}
	}
	double var44;
	if ((input[12]) >= (7.5708)) {
		if ((input[25]) >= (7.1)) {
			if ((input[51]) >= (0.27850002)) {
				var44 = 0.0069723236;
			}
			else {
				var44 = -0.07252555;
			}
		}
		else {
			if ((input[23]) >= (0.675)) {
				var44 = -0.011739634;
			}
			else {
				var44 = -0.056232035;
			}
		}
	}
	else {
		if ((input[54]) >= (40.5)) {
			if ((input[56]) >= (3986.0)) {
				var44 = 0.0030467773;
			}
			else {
				var44 = 0.06433535;
			}
		}
		else {
			if ((input[51]) >= (0.01025)) {
				var44 = 0.0017727147;
			}
			else {
				var44 = -0.036461867;
			}
		}
	}
	double var45;
	if ((input[56]) >= (118.5)) {
		if ((input[47]) >= (0.24285)) {
			if ((input[61]) >= (66392.5)) {
				var45 = -0.022353932;
			}
			else {
				var45 = 0.004210214;
			}
		}
		else {
			if ((input[54]) >= (40.5)) {
				var45 = -0.0010673004;
			}
			else {
				var45 = -0.1079962;
			}
		}
	}
	else {
		if ((input[60]) >= (2.5)) {
			if ((input[47]) >= (0.5183)) {
				var45 = 0.017527651;
			}
			else {
				var45 = -0.07644033;
			}
		}
		else {
			if ((input[54]) >= (36.5)) {
				var45 = 0.08846723;
			}
			else {
				var45 = 0.029479077;
			}
		}
	}
	double var46;
	if ((input[25]) >= (5.29165)) {
		if ((input[1]) >= (16.1436)) {
			if ((input[56]) >= (67.5)) {
				var46 = -0.066572;
			}
			else {
				var46 = 0.15104733;
			}
		}
		else {
			if ((input[47]) >= (2.67945)) {
				var46 = 0.008213664;
			}
			else {
				var46 = -0.03503829;
			}
		}
	}
	else {
		if ((input[56]) >= (118.5)) {
			if ((input[46]) >= (0.5223)) {
				var46 = 0.0026847993;
			}
			else {
				var46 = -0.065756164;
			}
		}
		else {
			if ((input[60]) >= (1.5)) {
				var46 = -0.010573774;
			}
			else {
				var46 = 0.07400432;
			}
		}
	}
	double var47;
	if ((input[54]) >= (43.5)) {
		if ((input[56]) >= (8973.0)) {
			if ((input[47]) >= (2.1767502)) {
				var47 = 0.013764034;
			}
			else {
				var47 = -0.055279832;
			}
		}
		else {
			if ((input[60]) >= (2.5)) {
				var47 = 0.00715667;
			}
			else {
				var47 = 0.096612245;
			}
		}
	}
	else {
		if ((input[56]) >= (75.5)) {
			if ((input[46]) >= (0.13575)) {
				var47 = -0.0034603295;
			}
			else {
				var47 = -0.09002751;
			}
		}
		else {
			if ((input[60]) >= (2.5)) {
				var47 = -0.042918023;
			}
			else {
				var47 = 0.073185064;
			}
		}
	}
	double var48;
	if ((input[0]) >= (6.0)) {
		if ((input[48]) >= (23.367199)) {
			if ((input[45]) >= (29.139648)) {
				var48 = -0.013195405;
			}
			else {
				var48 = 0.055556547;
			}
		}
		else {
			if ((input[56]) >= (11.5)) {
				var48 = -0.036143377;
			}
			else {
				var48 = 0.09237502;
			}
		}
	}
	else {
		if ((input[56]) >= (621.5)) {
			if ((input[54]) >= (42.5)) {
				var48 = 0.021936178;
			}
			else {
				var48 = -0.0097403815;
			}
		}
		else {
			if ((input[54]) >= (37.5)) {
				var48 = 0.079494916;
			}
			else {
				var48 = 0.0048737735;
			}
		}
	}
	double var49;
	if ((input[56]) >= (101852.5)) {
		if ((input[38]) >= (2.625)) {
			if ((input[36]) >= (6.3374004)) {
				var49 = -0.0765334;
			}
			else {
				var49 = -0.03434403;
			}
		}
		else {
			if ((input[60]) >= (3.5)) {
				var49 = 0.0011405724;
			}
			else {
				var49 = -0.035305213;
			}
		}
	}
	else {
		if ((input[47]) >= (2.3115501)) {
			if ((input[56]) >= (1500.5)) {
				var49 = 0.0102341855;
			}
			else {
				var49 = 0.05538234;
			}
		}
		else {
			if ((input[56]) >= (159.5)) {
				var49 = -0.012253879;
			}
			else {
				var49 = 0.028053973;
			}
		}
	}
	double var50;
	if ((input[47]) >= (1.1800499)) {
		if ((input[56]) >= (986.5)) {
			if ((input[37]) >= (2.5147)) {
				var50 = -0.029797008;
			}
			else {
				var50 = 0.0013795225;
			}
		}
		else {
			if ((input[57]) >= (0.5)) {
				var50 = 0.05046159;
			}
			else {
				var50 = -0.025180167;
			}
		}
	}
	else {
		if ((input[56]) >= (58.5)) {
			if ((input[46]) >= (0.13575)) {
				var50 = -0.016867602;
			}
			else {
				var50 = -0.07194614;
			}
		}
		else {
			if ((input[60]) >= (1.5)) {
				var50 = -0.038305074;
			}
			else {
				var50 = 0.081307724;
			}
		}
	}
	double var51;
	if ((input[24]) >= (8.0)) {
		if ((input[22]) >= (1.5)) {
			if ((input[41]) >= (0.5)) {
				var51 = -0.006569092;
			}
			else {
				var51 = -0.05229315;
			}
		}
		else {
			if ((input[56]) >= (32.5)) {
				var51 = -0.06316381;
			}
			else {
				var51 = 0.053133883;
			}
		}
	}
	else {
		if ((input[61]) >= (2957.5)) {
			if ((input[54]) >= (44.5)) {
				var51 = 0.018169839;
			}
			else {
				var51 = -0.01087965;
			}
		}
		else {
			if ((input[54]) >= (40.5)) {
				var51 = 0.06000181;
			}
			else {
				var51 = 0.0023052057;
			}
		}
	}
	double var52;
	if ((input[47]) >= (1.57935)) {
		if ((input[56]) >= (1500.5)) {
			if ((input[61]) >= (66392.5)) {
				var52 = -0.018786876;
			}
			else {
				var52 = 0.0046459963;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var52 = -0.002655467;
			}
			else {
				var52 = 0.053552408;
			}
		}
	}
	else {
		if ((input[56]) >= (75.5)) {
			if ((input[47]) >= (0.1261)) {
				var52 = -0.013317771;
			}
			else {
				var52 = -0.06541484;
			}
		}
		else {
			if ((input[60]) >= (1.5)) {
				var52 = -0.02518402;
			}
			else {
				var52 = 0.070176706;
			}
		}
	}
	double var53;
	if ((input[54]) >= (37.5)) {
		if ((input[56]) >= (352.5)) {
			if ((input[56]) >= (3635.5)) {
				var53 = -0.005575087;
			}
			else {
				var53 = 0.017677685;
			}
		}
		else {
			if ((input[60]) >= (2.5)) {
				var53 = -0.020393485;
			}
			else {
				var53 = 0.082209;
			}
		}
	}
	else {
		if ((input[45]) >= (57.65675)) {
			if ((input[48]) >= (1.1315501)) {
				var53 = -0.02024991;
			}
			else {
				var53 = -0.07207856;
			}
		}
		else {
			if ((input[60]) >= (1.5)) {
				var53 = 0.023495372;
			}
			else {
				var53 = -0.013916189;
			}
		}
	}
	double var54;
	if ((input[0]) >= (6.0)) {
		if ((input[48]) >= (23.367199)) {
			if ((input[45]) >= (29.139648)) {
				var54 = -0.010760694;
			}
			else {
				var54 = 0.049135044;
			}
		}
		else {
			if ((input[56]) >= (9.5)) {
				var54 = -0.033973873;
			}
			else {
				var54 = 0.091373175;
			}
		}
	}
	else {
		if ((input[56]) >= (484.5)) {
			if ((input[54]) >= (42.5)) {
				var54 = 0.021387987;
			}
			else {
				var54 = -0.0076495255;
			}
		}
		else {
			if ((input[54]) >= (37.5)) {
				var54 = 0.078488454;
			}
			else {
				var54 = 0.0073101367;
			}
		}
	}
	double var55;
	if ((input[47]) >= (1.70725)) {
		if ((input[56]) >= (971.5)) {
			if ((input[56]) >= (49996.5)) {
				var55 = -0.015001935;
			}
			else {
				var55 = 0.0075234524;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var55 = 0.0067689787;
			}
			else {
				var55 = 0.06535583;
			}
		}
	}
	else {
		if ((input[56]) >= (159.5)) {
			if ((input[48]) >= (12.6495)) {
				var55 = -0.00374555;
			}
			else {
				var55 = -0.037270375;
			}
		}
		else {
			if ((input[54]) >= (37.5)) {
				var55 = 0.081183955;
			}
			else {
				var55 = -0.0023849863;
			}
		}
	}
	double var56;
	if ((input[54]) >= (40.5)) {
		if ((input[56]) >= (4635.5)) {
			if ((input[47]) >= (2.15)) {
				var56 = 0.0045597753;
			}
			else {
				var56 = -0.04125587;
			}
		}
		else {
			if ((input[60]) >= (5.5)) {
				var56 = -0.010302575;
			}
			else {
				var56 = 0.060385253;
			}
		}
	}
	else {
		if ((input[47]) >= (0.25805002)) {
			if ((input[56]) >= (251.5)) {
				var56 = -0.007523808;
			}
			else {
				var56 = 0.035089232;
			}
		}
		else {
			if ((input[56]) >= (60.5)) {
				var56 = -0.08546166;
			}
			else {
				var56 = 0.020608272;
			}
		}
	}
	double var57;
	if ((input[1]) >= (9.32665)) {
		if ((input[25]) >= (8.1)) {
			if ((input[1]) >= (11.2847)) {
				var57 = -0.08810192;
			}
			else {
				var57 = 0.011323275;
			}
		}
		else {
			if ((input[23]) >= (0.225)) {
				var57 = -0.0116342;
			}
			else {
				var57 = -0.0663642;
			}
		}
	}
	else {
		if ((input[54]) >= (43.5)) {
			if ((input[56]) >= (11812.5)) {
				var57 = 0.012827391;
			}
			else {
				var57 = 0.07818068;
			}
		}
		else {
			if ((input[46]) >= (4.6188)) {
				var57 = 0.004246461;
			}
			else {
				var57 = -0.018692296;
			}
		}
	}
	double var58;
	if ((input[61]) >= (320741.5)) {
		if ((input[36]) >= (6.17065)) {
			if ((input[36]) >= (6.6991)) {
				var58 = -0.05348758;
			}
			else {
				var58 = 0.007124774;
			}
		}
		else {
			if ((input[37]) >= (-0.4706)) {
				var58 = -0.08412504;
			}
			else {
				var58 = 0.0027026494;
			}
		}
	}
	else {
		if ((input[47]) >= (2.5321002)) {
			if ((input[56]) >= (44753.5)) {
				var58 = -0.0077822655;
			}
			else {
				var58 = 0.023387609;
			}
		}
		else {
			if ((input[56]) >= (759.5)) {
				var58 = -0.0138706295;
			}
			else {
				var58 = 0.008650877;
			}
		}
	}
	double var59;
	if ((input[0]) >= (6.0)) {
		if ((input[48]) >= (23.367199)) {
			if ((input[1]) >= (10.1343)) {
				var59 = -0.030603144;
			}
			else {
				var59 = 0.0077430694;
			}
		}
		else {
			if ((input[56]) >= (9.5)) {
				var59 = -0.03084354;
			}
			else {
				var59 = 0.0869782;
			}
		}
	}
	else {
		if ((input[56]) >= (272.5)) {
			if ((input[54]) >= (42.5)) {
				var59 = 0.0201257;
			}
			else {
				var59 = -0.005229376;
			}
		}
		else {
			if ((input[51]) >= (0.00835)) {
				var59 = 0.07279083;
			}
			else {
				var59 = -0.0019262154;
			}
		}
	}
	double var60;
	if ((input[24]) >= (8.0)) {
		if ((input[22]) >= (1.5)) {
			if ((input[51]) >= (0.15525001)) {
				var60 = 0.01282185;
			}
			else {
				var60 = -0.02235607;
			}
		}
		else {
			if ((input[56]) >= (32.5)) {
				var60 = -0.0581066;
			}
			else {
				var60 = 0.05139162;
			}
		}
	}
	else {
		if ((input[61]) >= (3895.5)) {
			if ((input[57]) >= (0.5)) {
				var60 = -0.01148516;
			}
			else {
				var60 = 0.010224913;
			}
		}
		else {
			if ((input[54]) >= (40.5)) {
				var60 = 0.04912365;
			}
			else {
				var60 = 0.00074019295;
			}
		}
	}
	double var61;
	if ((input[47]) >= (1.1800499)) {
		if ((input[56]) >= (663.5)) {
			if ((input[48]) >= (11.43795)) {
				var61 = 0.0019981614;
			}
			else {
				var61 = -0.038518474;
			}
		}
		else {
			if ((input[57]) >= (0.5)) {
				var61 = 0.04791143;
			}
			else {
				var61 = -0.023052117;
			}
		}
	}
	else {
		if ((input[45]) >= (73.9966)) {
			if ((input[60]) >= (9.5)) {
				var61 = -0.061044037;
			}
			else {
				var61 = -0.019076584;
			}
		}
		else {
			if ((input[56]) >= (75.5)) {
				var61 = 0.0033051774;
			}
			else {
				var61 = 0.074474104;
			}
		}
	}
	double var62;
	if ((input[7]) >= (0.5)) {
		if ((input[56]) >= (386.5)) {
			if ((input[48]) >= (12.445999)) {
				var62 = 0.0066286153;
			}
			else {
				var62 = -0.028413612;
			}
		}
		else {
			if ((input[47]) >= (0.92974997)) {
				var62 = 0.058010872;
			}
			else {
				var62 = -0.00012498646;
			}
		}
	}
	else {
		if ((input[16]) >= (3.675)) {
			if ((input[56]) >= (133941.5)) {
				var62 = -0.04746235;
			}
			else {
				var62 = -0.012250302;
			}
		}
		else {
			if ((input[56]) >= (297.0)) {
				var62 = 0.0013056837;
			}
			else {
				var62 = 0.052940823;
			}
		}
	}
	double var63;
	if ((input[61]) >= (320741.5)) {
		if ((input[36]) >= (6.17065)) {
			if ((input[36]) >= (6.6991)) {
				var63 = -0.049554635;
			}
			else {
				var63 = 0.0069659497;
			}
		}
		else {
			if ((input[37]) >= (-0.4706)) {
				var63 = -0.07776257;
			}
			else {
				var63 = 0.0025612675;
			}
		}
	}
	else {
		if ((input[54]) >= (43.5)) {
			if ((input[56]) >= (3576.5)) {
				var63 = 0.01160432;
			}
			else {
				var63 = 0.094799906;
			}
		}
		else {
			if ((input[48]) >= (0.00585)) {
				var63 = -0.00039191588;
			}
			else {
				var63 = -0.047768008;
			}
		}
	}
	double var64;
	if ((input[12]) >= (7.5747004)) {
		if ((input[50]) >= (6.4941)) {
			if ((input[37]) >= (2.5147)) {
				var64 = -0.035377502;
			}
			else {
				var64 = -0.0027006862;
			}
		}
		else {
			if ((input[60]) >= (13.5)) {
				var64 = -0.061109908;
			}
			else {
				var64 = -0.019835783;
			}
		}
	}
	else {
		if ((input[47]) >= (1.7125499)) {
			if ((input[56]) >= (1152.5)) {
				var64 = 0.004283362;
			}
			else {
				var64 = 0.038738433;
			}
		}
		else {
			if ((input[61]) >= (187.5)) {
				var64 = -0.015969602;
			}
			else {
				var64 = 0.013661861;
			}
		}
	}
	double var65;
	if ((input[54]) >= (37.5)) {
		if ((input[56]) >= (1176.5)) {
			if ((input[48]) >= (12.346649)) {
				var65 = 0.0013172983;
			}
			else {
				var65 = -0.05014648;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var65 = -0.001661693;
			}
			else {
				var65 = 0.054590225;
			}
		}
	}
	else {
		if ((input[45]) >= (50.711403)) {
			if ((input[48]) >= (1.1315501)) {
				var65 = -0.015589423;
			}
			else {
				var65 = -0.060925808;
			}
		}
		else {
			if ((input[57]) >= (0.5)) {
				var65 = -0.0006677264;
			}
			else {
				var65 = 0.04476033;
			}
		}
	}
	double var66;
	if ((input[56]) >= (63.5)) {
		if ((input[48]) >= (6.32915)) {
			if ((input[56]) >= (1501.5)) {
				var66 = -0.0024514347;
			}
			else {
				var66 = 0.023190117;
			}
		}
		else {
			if ((input[54]) >= (40.5)) {
				var66 = 0.012860789;
			}
			else {
				var66 = -0.033005897;
			}
		}
	}
	else {
		if ((input[60]) >= (1.5)) {
			if ((input[46]) >= (0.3239)) {
				var66 = 0.07860824;
			}
			else {
				var66 = -0.041757684;
			}
		}
		else {
			if ((input[54]) >= (35.5)) {
				var66 = 0.08658314;
			}
			else {
				var66 = 0.01667079;
			}
		}
	}
	double var67;
	if ((input[25]) >= (7.1)) {
		if ((input[16]) >= (5.0)) {
			if ((input[56]) >= (32.5)) {
				var67 = -0.04735557;
			}
			else {
				var67 = 0.07568448;
			}
		}
		else {
			if ((input[12]) >= (25.0952)) {
				var67 = -0.08729518;
			}
			else {
				var67 = 0.032830473;
			}
		}
	}
	else {
		if ((input[56]) >= (118.5)) {
			if ((input[48]) >= (6.4348)) {
				var67 = 0.0033327327;
			}
			else {
				var67 = -0.02304574;
			}
		}
		else {
			if ((input[54]) >= (36.5)) {
				var67 = 0.065193914;
			}
			else {
				var67 = 0.0024512566;
			}
		}
	}
	double var68;
	if ((input[61]) >= (179137.5)) {
		if ((input[60]) >= (6.5)) {
			if ((input[16]) >= (7.775)) {
				var68 = -0.032827947;
			}
			else {
				var68 = 0.022456277;
			}
		}
		else {
			if ((input[3]) >= (2.675)) {
				var68 = -0.06228874;
			}
			else {
				var68 = -0.027053718;
			}
		}
	}
	else {
		if ((input[47]) >= (2.65235)) {
			if ((input[56]) >= (44753.5)) {
				var68 = -0.0050176154;
			}
			else {
				var68 = 0.022765152;
			}
		}
		else {
			if ((input[56]) >= (791.5)) {
				var68 = -0.010892286;
			}
			else {
				var68 = 0.0074053328;
			}
		}
	}
	double var69;
	if ((input[54]) >= (43.5)) {
		if ((input[56]) >= (8973.0)) {
			if ((input[46]) >= (55.3603)) {
				var69 = 0.00635539;
			}
			else {
				var69 = -0.10615289;
			}
		}
		else {
			if ((input[43]) >= (0.5)) {
				var69 = 0.024110135;
			}
			else {
				var69 = 0.09192267;
			}
		}
	}
	else {
		if ((input[51]) >= (0.00025)) {
			if ((input[56]) >= (190.5)) {
				var69 = -0.0034384523;
			}
			else {
				var69 = 0.036115814;
			}
		}
		else {
			if ((input[61]) >= (28.5)) {
				var69 = -0.101594046;
			}
			else {
				var69 = 0.0061035035;
			}
		}
	}
	double var70;
	if ((input[54]) >= (37.5)) {
		if ((input[56]) >= (1811.5)) {
			if ((input[47]) >= (2.0932999)) {
				var70 = 0.0031188398;
			}
			else {
				var70 = -0.019334285;
			}
		}
		else {
			if ((input[48]) >= (30.72005)) {
				var70 = 0.066903464;
			}
			else {
				var70 = 0.013614829;
			}
		}
	}
	else {
		if ((input[45]) >= (100.9468)) {
			if ((input[60]) >= (1.5)) {
				var70 = -0.02190339;
			}
			else {
				var70 = -0.06277056;
			}
		}
		else {
			if ((input[37]) >= (2.7353)) {
				var70 = -0.051591326;
			}
			else {
				var70 = 0.0037416555;
			}
		}
	}
	double var71;
	if ((input[0]) >= (6.0)) {
		if ((input[2]) >= (2.5)) {
			if ((input[37]) >= (1.9118)) {
				var71 = -0.025902957;
			}
			else {
				var71 = 0.035224598;
			}
		}
		else {
			if ((input[51]) >= (0.4084)) {
				var71 = 0.020549864;
			}
			else {
				var71 = -0.022194203;
			}
		}
	}
	else {
		if ((input[56]) >= (484.5)) {
			if ((input[54]) >= (42.5)) {
				var71 = 0.016322864;
			}
			else {
				var71 = -0.0056514028;
			}
		}
		else {
			if ((input[54]) >= (37.5)) {
				var71 = 0.06766022;
			}
			else {
				var71 = 0.0065002344;
			}
		}
	}
	double var72;
	if ((input[61]) >= (320741.5)) {
		if ((input[51]) >= (0.49675)) {
			if ((input[53]) >= (0.99590003)) {
				var72 = 0.052875977;
			}
			else {
				var72 = -0.09125524;
			}
		}
		else {
			if ((input[47]) >= (3.4306002)) {
				var72 = -0.036301598;
			}
			else {
				var72 = 0.07744128;
			}
		}
	}
	else {
		if ((input[13]) >= (2.5)) {
			if ((input[37]) >= (2.5147)) {
				var72 = -0.021828813;
			}
			else {
				var72 = 0.018630255;
			}
		}
		else {
			if ((input[54]) >= (40.5)) {
				var72 = 0.008062283;
			}
			else {
				var72 = -0.0103178;
			}
		}
	}
	double var73;
	if ((input[12]) >= (4.9033003)) {
		if ((input[50]) >= (6.47755)) {
			if ((input[56]) >= (6325.5)) {
				var73 = -0.0070750243;
			}
			else {
				var73 = 0.015625002;
			}
		}
		else {
			if ((input[22]) >= (0.5)) {
				var73 = -0.026053408;
			}
			else {
				var73 = -0.076375395;
			}
		}
	}
	else {
		if ((input[48]) >= (0.23144999)) {
			if ((input[56]) >= (118.5)) {
				var73 = 0.003856382;
			}
			else {
				var73 = 0.054702174;
			}
		}
		else {
			if ((input[56]) >= (34.5)) {
				var73 = -0.0672542;
			}
			else {
				var73 = 0.046960663;
			}
		}
	}
	double var74;
	if ((input[45]) >= (20.21925)) {
		if ((input[0]) >= (6.0)) {
			if ((input[2]) >= (2.5)) {
				var74 = 0.00826517;
			}
			else {
				var74 = -0.01841706;
			}
		}
		else {
			if ((input[56]) >= (272.5)) {
				var74 = -0.0002791039;
			}
			else {
				var74 = 0.032067396;
			}
		}
	}
	else {
		if ((input[48]) >= (4.22745)) {
			if ((input[23]) >= (0.225)) {
				var74 = 0.06145524;
			}
			else {
				var74 = -0.038167607;
			}
		}
		else {
			if ((input[1]) >= (0.0112)) {
				var74 = -0.06787314;
			}
			else {
				var74 = 0.03795592;
			}
		}
	}
	double var75;
	if ((input[46]) >= (4.6148)) {
		if ((input[56]) >= (969.5)) {
			if ((input[45]) >= (15.878401)) {
				var75 = -0.002126776;
			}
			else {
				var75 = 0.0777013;
			}
		}
		else {
			if ((input[60]) >= (10.5)) {
				var75 = -0.035599392;
			}
			else {
				var75 = 0.036290284;
			}
		}
	}
	else {
		if ((input[12]) >= (0.2466)) {
			if ((input[54]) >= (40.5)) {
				var75 = 0.0026810667;
			}
			else {
				var75 = -0.038082108;
			}
		}
		else {
			if ((input[48]) >= (1.4384999)) {
				var75 = 0.03870411;
			}
			else {
				var75 = -0.011257352;
			}
		}
	}
	double var76;
	if ((input[54]) >= (44.5)) {
		if ((input[56]) >= (15689.5)) {
			if ((input[47]) >= (2.60295)) {
				var76 = 0.010213625;
			}
			else {
				var76 = -0.053420007;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var76 = 0.011727492;
			}
			else {
				var76 = 0.09310897;
			}
		}
	}
	else {
		if ((input[23]) >= (0.9)) {
			if ((input[61]) >= (122465.5)) {
				var76 = -0.027273346;
			}
			else {
				var76 = 0.00416759;
			}
		}
		else {
			if ((input[38]) >= (0.875)) {
				var76 = 0.0022255126;
			}
			else {
				var76 = -0.03226974;
			}
		}
	}
	double var77;
	if ((input[1]) >= (17.565899)) {
		if ((input[60]) >= (13.5)) {
			if ((input[51]) >= (0.1714)) {
				var77 = -0.015759729;
			}
			else {
				var77 = -0.068725064;
			}
		}
		else {
			if ((input[61]) >= (5275.5)) {
				var77 = -0.031068087;
			}
			else {
				var77 = 0.03363749;
			}
		}
	}
	else {
		if ((input[54]) >= (41.5)) {
			if ((input[56]) >= (6043.5)) {
				var77 = -0.00069836626;
			}
			else {
				var77 = 0.04314999;
			}
		}
		else {
			if ((input[60]) >= (1.5)) {
				var77 = 0.0042959983;
			}
			else {
				var77 = -0.017957104;
			}
		}
	}
	double var78;
	if ((input[56]) >= (63.5)) {
		if ((input[46]) >= (0.5223)) {
			if ((input[56]) >= (791.5)) {
				var78 = -0.0023162724;
			}
			else {
				var78 = 0.014785148;
			}
		}
		else {
			if ((input[54]) >= (40.5)) {
				var78 = 0.015984349;
			}
			else {
				var78 = -0.0651294;
			}
		}
	}
	else {
		if ((input[60]) >= (1.5)) {
			if ((input[46]) >= (0.3239)) {
				var78 = 0.06326413;
			}
			else {
				var78 = -0.03685181;
			}
		}
		else {
			if ((input[61]) >= (12.5)) {
				var78 = 0.034019288;
			}
			else {
				var78 = 0.097012095;
			}
		}
	}
	double var79;
	if ((input[1]) >= (0.00245)) {
		if ((input[48]) >= (6.139)) {
			if ((input[56]) >= (752.5)) {
				var79 = -0.0010146186;
			}
			else {
				var79 = 0.024543455;
			}
		}
		else {
			if ((input[61]) >= (297.5)) {
				var79 = -0.035588704;
			}
			else {
				var79 = -0.0031512466;
			}
		}
	}
	else {
		if ((input[51]) >= (0.00005)) {
			if ((input[62]) >= (16.5)) {
				var79 = 0.06432476;
			}
			else {
				var79 = 0.0047251666;
			}
		}
		else {
			if ((input[61]) >= (0.5)) {
				var79 = -0.081072524;
			}
			else {
				var79 = 0.038071435;
			}
		}
	}
	double var80;
	if ((input[54]) >= (37.5)) {
		if ((input[56]) >= (352.5)) {
			if ((input[48]) >= (22.561249)) {
				var80 = 0.0047843615;
			}
			else {
				var80 = -0.018888934;
			}
		}
		else {
			if ((input[60]) >= (2.5)) {
				var80 = -0.014192519;
			}
			else {
				var80 = 0.06783377;
			}
		}
	}
	else {
		if ((input[60]) >= (1.5)) {
			if ((input[45]) >= (36.9043)) {
				var80 = -0.007990248;
			}
			else {
				var80 = 0.03529873;
			}
		}
		else {
			if ((input[56]) >= (48.5)) {
				var80 = -0.03755487;
			}
			else {
				var80 = 0.07328143;
			}
		}
	}
	double var81;
	if ((input[0]) >= (6.0)) {
		if ((input[48]) >= (23.367199)) {
			if ((input[45]) >= (29.139648)) {
				var81 = -0.007077402;
			}
			else {
				var81 = 0.037455495;
			}
		}
		else {
			if ((input[56]) >= (9.5)) {
				var81 = -0.024270615;
			}
			else {
				var81 = 0.08430513;
			}
		}
	}
	else {
		if ((input[56]) >= (208.5)) {
			if ((input[47]) >= (0.96535003)) {
				var81 = 0.00350147;
			}
			else {
				var81 = -0.022593385;
			}
		}
		else {
			if ((input[54]) >= (37.5)) {
				var81 = 0.08727511;
			}
			else {
				var81 = 0.01662339;
			}
		}
	}
	double var82;
	if ((input[61]) >= (320741.5)) {
		if ((input[36]) >= (6.17065)) {
			if ((input[36]) >= (7.00805)) {
				var82 = -0.09517156;
			}
			else {
				var82 = -0.0077956277;
			}
		}
		else {
			if ((input[35]) >= (0.5)) {
				var82 = -0.06297829;
			}
			else {
				var82 = 0.049619194;
			}
		}
	}
	else {
		if ((input[13]) >= (2.5)) {
			if ((input[37]) >= (2.5147)) {
				var82 = -0.019229569;
			}
			else {
				var82 = 0.016849091;
			}
		}
		else {
			if ((input[54]) >= (40.5)) {
				var82 = 0.0064532273;
			}
			else {
				var82 = -0.009059332;
			}
		}
	}
	double var83;
	if ((input[56]) >= (101852.5)) {
		if ((input[51]) >= (0.17645)) {
			if ((input[45]) >= (129.75879)) {
				var83 = 0.09460303;
			}
			else {
				var83 = -0.010294377;
			}
		}
		else {
			if ((input[54]) >= (40.5)) {
				var83 = -0.024283892;
			}
			else {
				var83 = -0.064191505;
			}
		}
	}
	else {
		if ((input[47]) >= (2.6290002)) {
			if ((input[36]) >= (6.3130503)) {
				var83 = 0.03554288;
			}
			else {
				var83 = 0.005598203;
			}
		}
		else {
			if ((input[2]) >= (2.5)) {
				var83 = 0.013654056;
			}
			else {
				var83 = -0.0074182646;
			}
		}
	}
	double var84;
	if ((input[56]) >= (3975.5)) {
		if ((input[57]) >= (0.5)) {
			if ((input[54]) >= (42.5)) {
				var84 = 0.0014813214;
			}
			else {
				var84 = -0.023463389;
			}
		}
		else {
			if ((input[3]) >= (1.55)) {
				var84 = 0.016179867;
			}
			else {
				var84 = -0.021074535;
			}
		}
	}
	else {
		if ((input[54]) >= (40.5)) {
			if ((input[48]) >= (37.2184)) {
				var84 = 0.070815064;
			}
			else {
				var84 = 0.016332174;
			}
		}
		else {
			if ((input[47]) >= (3.02455)) {
				var84 = 0.040485308;
			}
			else {
				var84 = -0.005108084;
			}
		}
	}
	double var85;
	if ((input[1]) >= (17.565899)) {
		if ((input[60]) >= (13.5)) {
			if ((input[52]) >= (243.23865)) {
				var85 = -0.01802239;
			}
			else {
				var85 = -0.07074405;
			}
		}
		else {
			if ((input[61]) >= (5275.5)) {
				var85 = -0.028094277;
			}
			else {
				var85 = 0.030063022;
			}
		}
	}
	else {
		if ((input[54]) >= (44.5)) {
			if ((input[56]) >= (24847.0)) {
				var85 = 0.0034929905;
			}
			else {
				var85 = 0.051332343;
			}
		}
		else {
			if ((input[60]) >= (1.5)) {
				var85 = 0.004646207;
			}
			else {
				var85 = -0.010237607;
			}
		}
	}
	double var86;
	if ((input[24]) >= (8.0)) {
		if ((input[22]) >= (1.5)) {
			if ((input[36]) >= (6.2597)) {
				var86 = 0.014513894;
			}
			else {
				var86 = -0.014659698;
			}
		}
		else {
			if ((input[49]) >= (3.4343)) {
				var86 = 0.054720845;
			}
			else {
				var86 = -0.05217853;
			}
		}
	}
	else {
		if ((input[61]) >= (2666.0)) {
			if ((input[57]) >= (0.5)) {
				var86 = -0.009834959;
			}
			else {
				var86 = 0.010576977;
			}
		}
		else {
			if ((input[48]) >= (31.970451)) {
				var86 = 0.028493835;
			}
			else {
				var86 = -0.0021161698;
			}
		}
	}
	double var87;
	if ((input[16]) >= (0.1)) {
		if ((input[13]) >= (2.5)) {
			if ((input[37]) >= (2.5147)) {
				var87 = -0.018094787;
			}
			else {
				var87 = 0.0120887635;
			}
		}
		else {
			if ((input[23]) >= (0.675)) {
				var87 = -0.0029348114;
			}
			else {
				var87 = -0.02160911;
			}
		}
	}
	else {
		if ((input[13]) >= (1.5)) {
			if ((input[14]) >= (1.45)) {
				var87 = 0.00408961;
			}
			else {
				var87 = -0.02950804;
			}
		}
		else {
			if ((input[13]) >= (0.5)) {
				var87 = 0.049096078;
			}
			else {
				var87 = 0.004082268;
			}
		}
	}
	double var88;
	if ((input[56]) >= (975.5)) {
		if ((input[48]) >= (11.66275)) {
			if ((input[45]) >= (20.171902)) {
				var88 = -0.0018753465;
			}
			else {
				var88 = 0.04430635;
			}
		}
		else {
			if ((input[1]) >= (0.20165)) {
				var88 = -0.038995184;
			}
			else {
				var88 = 0.0010867049;
			}
		}
	}
	else {
		if ((input[54]) >= (37.5)) {
			if ((input[50]) >= (3.91065)) {
				var88 = 0.075250365;
			}
			else {
				var88 = 0.02356792;
			}
		}
		else {
			if ((input[51]) >= (0.05945)) {
				var88 = 0.028450279;
			}
			else {
				var88 = -0.014321688;
			}
		}
	}
	double var89;
	if ((input[56]) >= (118.5)) {
		if ((input[48]) >= (1.2271)) {
			if ((input[45]) >= (20.41165)) {
				var89 = -0.00062127924;
			}
			else {
				var89 = 0.035575744;
			}
		}
		else {
			if ((input[54]) >= (41.5)) {
				var89 = 0.02640566;
			}
			else {
				var89 = -0.05445558;
			}
		}
	}
	else {
		if ((input[54]) >= (36.5)) {
			if ((input[51]) >= (0.00005)) {
				var89 = 0.08314642;
			}
			else {
				var89 = 0.02193628;
			}
		}
		else {
			if ((input[51]) >= (0.01565)) {
				var89 = 0.056196917;
			}
			else {
				var89 = -0.02843136;
			}
		}
	}
	double var90;
	if ((input[0]) >= (6.0)) {
		if ((input[39]) >= (8.375)) {
			if ((input[45]) >= (137.3135)) {
				var90 = 0.026215717;
			}
			else {
				var90 = -0.045791205;
			}
		}
		else {
			if ((input[2]) >= (2.5)) {
				var90 = 0.01494169;
			}
			else {
				var90 = -0.012129105;
			}
		}
	}
	else {
		if ((input[56]) >= (600.5)) {
			if ((input[54]) >= (44.5)) {
				var90 = 0.020850543;
			}
			else {
				var90 = -0.0036454306;
			}
		}
		else {
			if ((input[51]) >= (0.02265)) {
				var90 = 0.050329566;
			}
			else {
				var90 = 0.0024956015;
			}
		}
	}
	double var91;
	if ((input[56]) >= (63.5)) {
		if ((input[47]) >= (0.14504999)) {
			if ((input[56]) >= (352.5)) {
				var91 = -0.0013015566;
			}
			else {
				var91 = 0.018411927;
			}
		}
		else {
			if ((input[54]) >= (40.5)) {
				var91 = 0.015440518;
			}
			else {
				var91 = -0.07105463;
			}
		}
	}
	else {
		if ((input[60]) >= (5.5)) {
			if ((input[56]) >= (60.5)) {
				var91 = 0.042934123;
			}
			else {
				var91 = -0.08712083;
			}
		}
		else {
			if ((input[47]) >= (0.02135)) {
				var91 = 0.07509064;
			}
			else {
				var91 = 0.019110853;
			}
		}
	}
	double var92;
	if ((input[60]) >= (827.5)) {
		if ((input[45]) >= (140.0127)) {
			if ((input[46]) >= (1160.1293)) {
				var92 = 0.19926302;
			}
			else {
				var92 = -0.0730324;
			}
		}
		else {
			if ((input[49]) >= (0.9676)) {
				var92 = -0.075659625;
			}
			else {
				var92 = 0.024980348;
			}
		}
	}
	else {
		if ((input[13]) >= (2.5)) {
			if ((input[60]) >= (2.5)) {
				var92 = 0.016565247;
			}
			else {
				var92 = -0.012183344;
			}
		}
		else {
			if ((input[56]) >= (2373.5)) {
				var92 = -0.009714888;
			}
			else {
				var92 = 0.004329948;
			}
		}
	}
	double var93;
	if ((input[54]) >= (37.5)) {
		if ((input[56]) >= (2511.5)) {
			if ((input[38]) >= (1.29165)) {
				var93 = 0.006574593;
			}
			else {
				var93 = -0.010910388;
			}
		}
		else {
			if ((input[48]) >= (37.51365)) {
				var93 = 0.047391195;
			}
			else {
				var93 = 0.00840279;
			}
		}
	}
	else {
		if ((input[45]) >= (100.9468)) {
			if ((input[60]) >= (1.5)) {
				var93 = -0.016670262;
			}
			else {
				var93 = -0.052042264;
			}
		}
		else {
			if ((input[37]) >= (2.7353)) {
				var93 = -0.04276507;
			}
			else {
				var93 = 0.0028996489;
			}
		}
	}
	double var94;
	if ((input[62]) >= (15.5)) {
		if ((input[61]) >= (179137.5)) {
			if ((input[51]) >= (0.18875)) {
				var94 = -0.012789954;
			}
			else {
				var94 = -0.054656584;
			}
		}
		else {
			if ((input[54]) >= (44.5)) {
				var94 = 0.019641383;
			}
			else {
				var94 = -0.0027796065;
			}
		}
	}
	else {
		if ((input[5]) >= (8.225)) {
			if ((input[53]) >= (0.6876)) {
				var94 = -0.0059559345;
			}
			else {
				var94 = 0.03653315;
			}
		}
		else {
			if ((input[48]) >= (3887.7593)) {
				var94 = -0.08268771;
			}
			else {
				var94 = 0.02405583;
			}
		}
	}
	double var95;
	if ((input[56]) >= (159.5)) {
		if ((input[48]) >= (3.3611)) {
			if ((input[61]) >= (66392.5)) {
				var95 = -0.011122117;
			}
			else {
				var95 = 0.003205166;
			}
		}
		else {
			if ((input[54]) >= (41.5)) {
				var95 = 0.011296759;
			}
			else {
				var95 = -0.03524844;
			}
		}
	}
	else {
		if ((input[54]) >= (37.5)) {
			if ((input[60]) >= (2.5)) {
				var95 = -0.10386874;
			}
			else {
				var95 = 0.08062343;
			}
		}
		else {
			if ((input[47]) >= (0.54365003)) {
				var95 = 0.03523014;
			}
			else {
				var95 = -0.023668524;
			}
		}
	}
	double var96;
	if ((input[23]) >= (0.225)) {
		if ((input[0]) >= (6.0)) {
			if ((input[39]) >= (8.375)) {
				var96 = -0.03607503;
			}
			else {
				var96 = -0.0053329053;
			}
		}
		else {
			if ((input[56]) >= (1270.5)) {
				var96 = 0.00028868867;
			}
			else {
				var96 = 0.020736191;
			}
		}
	}
	else {
		if ((input[47]) >= (1.53115)) {
			if ((input[56]) >= (706.0)) {
				var96 = -0.0014680774;
			}
			else {
				var96 = 0.05941776;
			}
		}
		else {
			if ((input[61]) >= (349.5)) {
				var96 = -0.059348583;
			}
			else {
				var96 = -0.002717545;
			}
		}
	}
	double var97;
	if ((input[49]) >= (0.4303)) {
		if ((input[1]) >= (0.00245)) {
			if ((input[23]) >= (0.225)) {
				var97 = 0.0018565977;
			}
			else {
				var97 = -0.014348211;
			}
		}
		else {
			if ((input[51]) >= (0.00005)) {
				var97 = 0.047065627;
			}
			else {
				var97 = -0.016502168;
			}
		}
	}
	else {
		if ((input[47]) >= (2.2730498)) {
			if ((input[0]) >= (3.0)) {
				var97 = -0.04242992;
			}
			else {
				var97 = 0.021255426;
			}
		}
		else {
			if ((input[12]) >= (0.09865)) {
				var97 = -0.058666993;
			}
			else {
				var97 = 0.06041528;
			}
		}
	}
	double var98;
	if ((input[54]) >= (46.5)) {
		if ((input[56]) >= (30203.0)) {
			if ((input[1]) >= (30.51805)) {
				var98 = -0.09721432;
			}
			else {
				var98 = 0.020164317;
			}
		}
		else {
			if ((input[60]) >= (7.5)) {
				var98 = -0.088842385;
			}
			else {
				var98 = 0.10815414;
			}
		}
	}
	else {
		if ((input[3]) >= (1.225)) {
			if ((input[60]) >= (1.5)) {
				var98 = 0.0073813605;
			}
			else {
				var98 = -0.0087004285;
			}
		}
		else {
			if ((input[60]) >= (11.5)) {
				var98 = -0.03612351;
			}
			else {
				var98 = -0.0027612215;
			}
		}
	}
	double var99;
	if ((input[56]) >= (52.5)) {
		if ((input[46]) >= (0.26725)) {
			if ((input[56]) >= (600.5)) {
				var99 = -0.001617101;
			}
			else {
				var99 = 0.012740857;
			}
		}
		else {
			if ((input[54]) >= (40.5)) {
				var99 = 0.014358758;
			}
			else {
				var99 = -0.061236855;
			}
		}
	}
	else {
		if ((input[60]) >= (1.5)) {
			if ((input[52]) >= (0.1426)) {
				var99 = 0.052200343;
			}
			else {
				var99 = -0.036393125;
			}
		}
		else {
			if ((input[61]) >= (12.5)) {
				var99 = 0.030235922;
			}
			else {
				var99 = 0.0934677;
			}
		}
	}
	double var100;
	var100 = (1.0) / ((1.0) + (exp((0.0) - ((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((var0)+(var1)) + (var2)) + (var3)) + (var4)) + (var5)) + (var6)) + (var7)) + (var8)) + (var9)) + (var10)) + (var11)) + (var12)) + (var13)) + (var14)) + (var15)) + (var16)) + (var17)) + (var18)) + (var19)) + (var20)) + (var21)) + (var22)) + (var23)) + (var24)) + (var25)) + (var26)) + (var27)) + (var28)) + (var29)) + (var30)) + (var31)) + (var32)) + (var33)) + (var34)) + (var35)) + (var36)) + (var37)) + (var38)) + (var39)) + (var40)) + (var41)) + (var42)) + (var43)) + (var44)) + (var45)) + (var46)) + (var47)) + (var48)) + (var49)) + (var50)) + (var51)) + (var52)) + (var53)) + (var54)) + (var55)) + (var56)) + (var57)) + (var58)) + (var59)) + (var60)) + (var61)) + (var62)) + (var63)) + (var64)) + (var65)) + (var66)) + (var67)) + (var68)) + (var69)) + (var70)) + (var71)) + (var72)) + (var73)) + (var74)) + (var75)) + (var76)) + (var77)) + (var78)) + (var79)) + (var80)) + (var81)) + (var82)) + (var83)) + (var84)) + (var85)) + (var86)) + (var87)) + (var88)) + (var89)) + (var90)) + (var91)) + (var92)) + (var93)) + (var94)) + (var95)) + (var96)) + (var97)) + (var98)) + (var99)))));
	*output = var100 < 0.5 ? 0 : 1;
}

void PUclf3(double * input, double * output) {
	double var0;
	if ((input[60]) >= (2.5)) {
		if ((input[60]) >= (7.5)) {
			if ((input[40]) >= (1.5)) {
				var0 = -0.102204956;
			}
			else {
				var0 = -0.16330674;
			}
		}
		else {
			if ((input[41]) >= (1.5)) {
				var0 = -0.0066794725;
			}
			else {
				var0 = -0.08227074;
			}
		}
	}
	else {
		if ((input[61]) >= (532.5)) {
			if ((input[41]) >= (2.5)) {
				var0 = 0.06595286;
			}
			else {
				var0 = 0.009226325;
			}
		}
		else {
			if ((input[43]) >= (0.5)) {
				var0 = 0.11134603;
			}
			else {
				var0 = 0.18691565;
			}
		}
	}
	double var1;
	if ((input[60]) >= (1.5)) {
		if ((input[60]) >= (7.5)) {
			if ((input[39]) >= (6.70835)) {
				var1 = -0.15232782;
			}
			else {
				var1 = -0.09652174;
			}
		}
		else {
			if ((input[40]) >= (1.5)) {
				var1 = 0.00044842606;
			}
			else {
				var1 = -0.06697812;
			}
		}
	}
	else {
		if ((input[61]) >= (494.5)) {
			if ((input[1]) >= (2.68945)) {
				var1 = 0.023315476;
			}
			else {
				var1 = 0.07623439;
			}
		}
		else {
			if ((input[61]) >= (100.5)) {
				var1 = 0.12842214;
			}
			else {
				var1 = 0.17696993;
			}
		}
	}
	double var2;
	if ((input[60]) >= (2.5)) {
		if ((input[60]) >= (8.5)) {
			if ((input[40]) >= (1.5)) {
				var2 = -0.08545943;
			}
			else {
				var2 = -0.13795744;
			}
		}
		else {
			if ((input[40]) >= (2.5)) {
				var2 = 0.0019337606;
			}
			else {
				var2 = -0.062173765;
			}
		}
	}
	else {
		if ((input[61]) >= (494.5)) {
			if ((input[1]) >= (2.68165)) {
				var2 = 0.010074687;
			}
			else {
				var2 = 0.061740678;
			}
		}
		else {
			if ((input[43]) >= (1.5)) {
				var2 = 0.044450086;
			}
			else {
				var2 = 0.15152894;
			}
		}
	}
	double var3;
	if ((input[60]) >= (1.5)) {
		if ((input[60]) >= (6.5)) {
			if ((input[39]) >= (4.29165)) {
				var3 = -0.11931332;
			}
			else {
				var3 = -0.06322126;
			}
		}
		else {
			if ((input[40]) >= (1.5)) {
				var3 = 0.007682949;
			}
			else {
				var3 = -0.05276333;
			}
		}
	}
	else {
		if ((input[61]) >= (729.5)) {
			if ((input[54]) >= (40.5)) {
				var3 = 0.054524947;
			}
			else {
				var3 = -0.006184136;
			}
		}
		else {
			if ((input[61]) >= (110.5)) {
				var3 = 0.10184004;
			}
			else {
				var3 = 0.15282881;
			}
		}
	}
	double var4;
	if ((input[60]) >= (1.5)) {
		if ((input[60]) >= (7.5)) {
			if ((input[40]) >= (1.5)) {
				var4 = -0.0682837;
			}
			else {
				var4 = -0.11793929;
			}
		}
		else {
			if ((input[1]) >= (2.92775)) {
				var4 = -0.0413485;
			}
			else {
				var4 = 0.011469326;
			}
		}
	}
	else {
		if ((input[61]) >= (235.5)) {
			if ((input[61]) >= (1319.5)) {
				var4 = 0.02198035;
			}
			else {
				var4 = 0.078156725;
			}
		}
		else {
			if ((input[54]) >= (36.5)) {
				var4 = 0.14632915;
			}
			else {
				var4 = 0.10996508;
			}
		}
	}
	double var5;
	if ((input[60]) >= (2.5)) {
		if ((input[57]) >= (0.5)) {
			if ((input[40]) >= (2.5)) {
				var5 = 0.0058012144;
			}
			else {
				var5 = -0.04943231;
			}
		}
		else {
			if ((input[39]) >= (6.70835)) {
				var5 = -0.11542014;
			}
			else {
				var5 = -0.068134755;
			}
		}
	}
	else {
		if ((input[61]) >= (216.5)) {
			if ((input[12]) >= (1.45505)) {
				var5 = 0.012590502;
			}
			else {
				var5 = 0.06763127;
			}
		}
		else {
			if ((input[43]) >= (0.5)) {
				var5 = 0.07837614;
			}
			else {
				var5 = 0.13767803;
			}
		}
	}
	double var6;
	if ((input[60]) >= (1.5)) {
		if ((input[60]) >= (5.5)) {
			if ((input[39]) >= (6.70835)) {
				var6 = -0.105488494;
			}
			else {
				var6 = -0.0539195;
			}
		}
		else {
			if ((input[40]) >= (1.5)) {
				var6 = 0.01751299;
			}
			else {
				var6 = -0.040151622;
			}
		}
	}
	else {
		if ((input[61]) >= (173.5)) {
			if ((input[61]) >= (1319.5)) {
				var6 = 0.017264051;
			}
			else {
				var6 = 0.06990512;
			}
		}
		else {
			if ((input[54]) >= (36.5)) {
				var6 = 0.1351131;
			}
			else {
				var6 = 0.100520454;
			}
		}
	}
	double var7;
	if ((input[60]) >= (1.5)) {
		if ((input[57]) >= (0.5)) {
			if ((input[1]) >= (2.45505)) {
				var7 = -0.03404892;
			}
			else {
				var7 = 0.012141909;
			}
		}
		else {
			if ((input[41]) >= (1.5)) {
				var7 = -0.04973847;
			}
			else {
				var7 = -0.096774004;
			}
		}
	}
	else {
		if ((input[61]) >= (173.5)) {
			if ((input[12]) >= (0.3418)) {
				var7 = 0.023724541;
			}
			else {
				var7 = 0.08606746;
			}
		}
		else {
			if ((input[54]) >= (36.5)) {
				var7 = 0.12978703;
			}
			else {
				var7 = 0.09460855;
			}
		}
	}
	double var8;
	if ((input[60]) >= (1.5)) {
		if ((input[60]) >= (8.5)) {
			if ((input[39]) >= (4.29165)) {
				var8 = -0.089503795;
			}
			else {
				var8 = -0.04074412;
			}
		}
		else {
			if ((input[43]) >= (1.5)) {
				var8 = -0.055640765;
			}
			else {
				var8 = -0.0009931724;
			}
		}
	}
	else {
		if ((input[61]) >= (173.5)) {
			if ((input[1]) >= (3.81835)) {
				var8 = 0.004811495;
			}
			else {
				var8 = 0.05306813;
			}
		}
		else {
			if ((input[43]) >= (0.5)) {
				var8 = 0.08285051;
			}
			else {
				var8 = 0.12375226;
			}
		}
	}
	double var9;
	if ((input[60]) >= (2.5)) {
		if ((input[41]) >= (1.5)) {
			if ((input[60]) >= (5.5)) {
				var9 = -0.035005234;
			}
			else {
				var9 = 0.015883781;
			}
		}
		else {
			if ((input[25]) >= (5.1)) {
				var9 = -0.111557536;
			}
			else {
				var9 = -0.058209416;
			}
		}
	}
	else {
		if ((input[61]) >= (175.5)) {
			if ((input[41]) >= (3.5)) {
				var9 = 0.06616882;
			}
			else {
				var9 = 0.008972517;
			}
		}
		else {
			if ((input[43]) >= (0.5)) {
				var9 = 0.060406413;
			}
			else {
				var9 = 0.11955189;
			}
		}
	}
	double var10;
	if ((input[60]) >= (1.5)) {
		if ((input[60]) >= (11.5)) {
			if ((input[25]) >= (5.29165)) {
				var10 = -0.105537355;
			}
			else {
				var10 = -0.056053348;
			}
		}
		else {
			if ((input[51]) >= (0.01095)) {
				var10 = -0.0059914715;
			}
			else {
				var10 = -0.07582449;
			}
		}
	}
	else {
		if ((input[61]) >= (110.5)) {
			if ((input[61]) >= (1319.5)) {
				var10 = 0.007795457;
			}
			else {
				var10 = 0.057261534;
			}
		}
		else {
			if ((input[54]) >= (35.5)) {
				var10 = 0.11867771;
			}
			else {
				var10 = 0.07810111;
			}
		}
	}
	double var11;
	if ((input[60]) >= (4.5)) {
		if ((input[41]) >= (0.5)) {
			if ((input[12]) >= (4.6269503)) {
				var11 = -0.05074811;
			}
			else {
				var11 = -0.012255126;
			}
		}
		else {
			if ((input[0]) >= (1.5)) {
				var11 = -0.093290195;
			}
			else {
				var11 = -0.034620162;
			}
		}
	}
	else {
		if ((input[1]) >= (0.16605)) {
			if ((input[41]) >= (2.5)) {
				var11 = 0.04244362;
			}
			else {
				var11 = -0.0045303074;
			}
		}
		else {
			if ((input[43]) >= (1.5)) {
				var11 = 0.008710724;
			}
			else {
				var11 = 0.10796201;
			}
		}
	}
	double var12;
	if ((input[60]) >= (1.5)) {
		if ((input[41]) >= (0.5)) {
			if ((input[1]) >= (3.75975)) {
				var12 = -0.037625592;
			}
			else {
				var12 = 0.0071802237;
			}
		}
		else {
			if ((input[25]) >= (4.55)) {
				var12 = -0.102651;
			}
			else {
				var12 = -0.058792125;
			}
		}
	}
	else {
		if ((input[61]) >= (100.5)) {
			if ((input[54]) >= (40.5)) {
				var12 = 0.049062714;
			}
			else {
				var12 = -0.0008078984;
			}
		}
		else {
			if ((input[54]) >= (35.5)) {
				var12 = 0.11415116;
			}
			else {
				var12 = 0.07239332;
			}
		}
	}
	double var13;
	if ((input[12]) >= (0.99025)) {
		if ((input[0]) >= (6.0)) {
			if ((input[41]) >= (2.5)) {
				var13 = -0.028175065;
			}
			else {
				var13 = -0.08388143;
			}
		}
		else {
			if ((input[60]) >= (4.5)) {
				var13 = -0.031643156;
			}
			else {
				var13 = 0.013964904;
			}
		}
	}
	else {
		if ((input[60]) >= (2.5)) {
			if ((input[51]) >= (0.00245)) {
				var13 = 0.014995723;
			}
			else {
				var13 = -0.09808855;
			}
		}
		else {
			if ((input[61]) >= (295.5)) {
				var13 = 0.040218882;
			}
			else {
				var13 = 0.10338906;
			}
		}
	}
	double var14;
	if ((input[1]) >= (0.32615)) {
		if ((input[60]) >= (5.5)) {
			if ((input[0]) >= (6.0)) {
				var14 = -0.07603586;
			}
			else {
				var14 = -0.030115241;
			}
		}
		else {
			if ((input[39]) >= (2.125)) {
				var14 = -0.0064524873;
			}
			else {
				var14 = 0.03392149;
			}
		}
	}
	else {
		if ((input[60]) >= (2.5)) {
			if ((input[51]) >= (0.00215)) {
				var14 = 0.031088958;
			}
			else {
				var14 = -0.096006416;
			}
		}
		else {
			if ((input[61]) >= (229.5)) {
				var14 = 0.04953759;
			}
			else {
				var14 = 0.105618656;
			}
		}
	}
	double var15;
	if ((input[12]) >= (0.24805)) {
		if ((input[60]) >= (6.5)) {
			if ((input[25]) >= (5.29165)) {
				var15 = -0.0860263;
			}
			else {
				var15 = -0.033258338;
			}
		}
		else {
			if ((input[41]) >= (2.5)) {
				var15 = 0.026329879;
			}
			else {
				var15 = -0.010578008;
			}
		}
	}
	else {
		if ((input[60]) >= (1.5)) {
			if ((input[51]) >= (0.00215)) {
				var15 = 0.039150715;
			}
			else {
				var15 = -0.0637398;
			}
		}
		else {
			if ((input[61]) >= (118.5)) {
				var15 = 0.06371107;
			}
			else {
				var15 = 0.10883217;
			}
		}
	}
	double var16;
	if ((input[1]) >= (1.16995)) {
		if ((input[0]) >= (6.0)) {
			if ((input[51]) >= (0.031549998)) {
				var16 = -0.052273482;
			}
			else {
				var16 = -0.09482426;
			}
		}
		else {
			if ((input[54]) >= (41.5)) {
				var16 = 0.019708028;
			}
			else {
				var16 = -0.023327587;
			}
		}
	}
	else {
		if ((input[12]) >= (0.04495)) {
			if ((input[4]) >= (3.5)) {
				var16 = 0.0046702544;
			}
			else {
				var16 = 0.0538508;
			}
		}
		else {
			if ((input[43]) >= (0.5)) {
				var16 = 0.027796505;
			}
			else {
				var16 = 0.10496962;
			}
		}
	}
	double var17;
	if ((input[12]) >= (1.45505)) {
		if ((input[41]) >= (0.5)) {
			if ((input[54]) >= (40.5)) {
				var17 = 0.01122399;
			}
			else {
				var17 = -0.02605532;
			}
		}
		else {
			if ((input[46]) >= (21.034199)) {
				var17 = -0.053388007;
			}
			else {
				var17 = -0.09904819;
			}
		}
	}
	else {
		if ((input[60]) >= (0.5)) {
			if ((input[15]) >= (3.5)) {
				var17 = 0.0011914;
			}
			else {
				var17 = 0.051817883;
			}
		}
		else {
			if ((input[56]) >= (469.5)) {
				var17 = 0.04394568;
			}
			else {
				var17 = 0.10424595;
			}
		}
	}
	double var18;
	if ((input[60]) >= (0.5)) {
		if ((input[1]) >= (3.75975)) {
			if ((input[0]) >= (6.0)) {
				var18 = -0.0694648;
			}
			else {
				var18 = -0.021507816;
			}
		}
		else {
			if ((input[4]) >= (3.5)) {
				var18 = -0.0058686743;
			}
			else {
				var18 = 0.03907638;
			}
		}
	}
	else {
		if ((input[56]) >= (729.5)) {
			if ((input[54]) >= (40.5)) {
				var18 = 0.041501246;
			}
			else {
				var18 = -0.024605343;
			}
		}
		else {
			if ((input[54]) >= (37.5)) {
				var18 = 0.10972264;
			}
			else {
				var18 = 0.0644237;
			}
		}
	}
	double var19;
	if ((input[1]) >= (0.06055)) {
		if ((input[60]) >= (5.5)) {
			if ((input[39]) >= (6.70835)) {
				var19 = -0.059093293;
			}
			else {
				var19 = -0.017754262;
			}
		}
		else {
			if ((input[61]) >= (1753.5)) {
				var19 = -0.008525932;
			}
			else {
				var19 = 0.031083647;
			}
		}
	}
	else {
		if ((input[43]) >= (0.5)) {
			if ((input[60]) >= (2.5)) {
				var19 = -0.04561409;
			}
			else {
				var19 = 0.047658604;
			}
		}
		else {
			if ((input[61]) >= (206.5)) {
				var19 = 0.06338033;
			}
			else {
				var19 = 0.10384321;
			}
		}
	}
	double var20;
	if ((input[41]) >= (2.5)) {
		if ((input[56]) >= (455.5)) {
			if ((input[54]) >= (40.5)) {
				var20 = 0.02634396;
			}
			else {
				var20 = -0.0050802077;
			}
		}
		else {
			if ((input[54]) >= (37.5)) {
				var20 = 0.10366782;
			}
			else {
				var20 = 0.045945156;
			}
		}
	}
	else {
		if ((input[24]) >= (8.0)) {
			if ((input[52]) >= (1.63665)) {
				var20 = -0.06419371;
			}
			else {
				var20 = -0.09649553;
			}
		}
		else {
			if ((input[15]) >= (3.5)) {
				var20 = -0.025174636;
			}
			else {
				var20 = 0.016054882;
			}
		}
	}
	double var21;
	if ((input[12]) >= (0.06055)) {
		if ((input[41]) >= (0.5)) {
			if ((input[1]) >= (4.8105497)) {
				var21 = -0.017117022;
			}
			else {
				var21 = 0.014214175;
			}
		}
		else {
			if ((input[54]) >= (37.5)) {
				var21 = -0.02716254;
			}
			else {
				var21 = -0.06832464;
			}
		}
	}
	else {
		if ((input[43]) >= (0.5)) {
			if ((input[45]) >= (73.90235)) {
				var21 = -0.030390834;
			}
			else {
				var21 = 0.044939723;
			}
		}
		else {
			if ((input[61]) >= (110.5)) {
				var21 = 0.06107358;
			}
			else {
				var21 = 0.10300845;
			}
		}
	}
	double var22;
	if ((input[60]) >= (0.5)) {
		if ((input[12]) >= (4.70115)) {
			if ((input[54]) >= (40.5)) {
				var22 = -0.0053501413;
			}
			else {
				var22 = -0.044478692;
			}
		}
		else {
			if ((input[15]) >= (3.5)) {
				var22 = -0.006734177;
			}
			else {
				var22 = 0.0327253;
			}
		}
	}
	else {
		if ((input[56]) >= (729.5)) {
			if ((input[54]) >= (40.5)) {
				var22 = 0.036601834;
			}
			else {
				var22 = -0.023999441;
			}
		}
		else {
			if ((input[54]) >= (37.5)) {
				var22 = 0.10453711;
			}
			else {
				var22 = 0.053262096;
			}
		}
	}
	double var23;
	if ((input[1]) >= (0.05275)) {
		if ((input[0]) >= (6.0)) {
			if ((input[46]) >= (1.9849999)) {
				var23 = -0.030688345;
			}
			else {
				var23 = -0.081363656;
			}
		}
		else {
			if ((input[56]) >= (837.5)) {
				var23 = -0.0069236867;
			}
			else {
				var23 = 0.036585;
			}
		}
	}
	else {
		if ((input[43]) >= (0.5)) {
			if ((input[45]) >= (81.70115)) {
				var23 = -0.04065221;
			}
			else {
				var23 = 0.04092082;
			}
		}
		else {
			if ((input[61]) >= (110.5)) {
				var23 = 0.057314154;
			}
			else {
				var23 = 0.10010642;
			}
		}
	}
	double var24;
	if ((input[41]) >= (2.5)) {
		if ((input[56]) >= (99.5)) {
			if ((input[56]) >= (1929.5)) {
				var24 = 0.0011227404;
			}
			else {
				var24 = 0.03142399;
			}
		}
		else {
			if ((input[54]) >= (35.5)) {
				var24 = 0.10278175;
			}
			else {
				var24 = 0.042848732;
			}
		}
	}
	else {
		if ((input[24]) >= (8.0)) {
			if ((input[57]) >= (0.5)) {
				var24 = -0.04290549;
			}
			else {
				var24 = -0.07186436;
			}
		}
		else {
			if ((input[4]) >= (3.5)) {
				var24 = -0.02019491;
			}
			else {
				var24 = 0.014205745;
			}
		}
	}
	double var25;
	if ((input[60]) >= (4.5)) {
		if ((input[46]) >= (6.31815)) {
			if ((input[60]) >= (200.5)) {
				var25 = -0.0672031;
			}
			else {
				var25 = -0.006354676;
			}
		}
		else {
			if ((input[47]) >= (0.77115)) {
				var25 = -0.051296104;
			}
			else {
				var25 = -0.09750178;
			}
		}
	}
	else {
		if ((input[61]) >= (61.5)) {
			if ((input[47]) >= (0.1392)) {
				var25 = 0.010991483;
			}
			else {
				var25 = -0.1522718;
			}
		}
		else {
			if ((input[60]) >= (1.5)) {
				var25 = 0.017054247;
			}
			else {
				var25 = 0.09419738;
			}
		}
	}
	double var26;
	if ((input[1]) >= (2.44725)) {
		if ((input[54]) >= (40.5)) {
			if ((input[56]) >= (3241.0)) {
				var26 = -0.0025658328;
			}
			else {
				var26 = 0.055723645;
			}
		}
		else {
			if ((input[40]) >= (0.5)) {
				var26 = -0.023302628;
			}
			else {
				var26 = -0.06465577;
			}
		}
	}
	else {
		if ((input[60]) >= (0.5)) {
			if ((input[32]) >= (1.5)) {
				var26 = 0.029639218;
			}
			else {
				var26 = -0.0053998525;
			}
		}
		else {
			if ((input[56]) >= (230.5)) {
				var26 = 0.03217271;
			}
			else {
				var26 = 0.09499781;
			}
		}
	}
	double var27;
	if ((input[39]) >= (2.125)) {
		if ((input[32]) >= (1.5)) {
			if ((input[1]) >= (10.337851)) {
				var27 = -0.025310053;
			}
			else {
				var27 = 0.017056838;
			}
		}
		else {
			if ((input[47]) >= (1.2222)) {
				var27 = -0.019755377;
			}
			else {
				var27 = -0.057106603;
			}
		}
	}
	else {
		if ((input[61]) >= (321.5)) {
			if ((input[54]) >= (40.5)) {
				var27 = 0.025862498;
			}
			else {
				var27 = -0.0049626543;
			}
		}
		else {
			if ((input[60]) >= (4.5)) {
				var27 = 0.025612528;
			}
			else {
				var27 = 0.08618224;
			}
		}
	}
	double var28;
	if ((input[15]) >= (3.5)) {
		if ((input[4]) >= (3.5)) {
			if ((input[41]) >= (0.5)) {
				var28 = -0.015679736;
			}
			else {
				var28 = -0.049784396;
			}
		}
		else {
			if ((input[56]) >= (93.5)) {
				var28 = 0.009529604;
			}
			else {
				var28 = 0.09311498;
			}
		}
	}
	else {
		if ((input[12]) >= (0.10744999)) {
			if ((input[12]) >= (10.86915)) {
				var28 = -0.015190412;
			}
			else {
				var28 = 0.022616338;
			}
		}
		else {
			if ((input[32]) >= (1.5)) {
				var28 = 0.08823597;
			}
			else {
				var28 = 0.028801916;
			}
		}
	}
	double var29;
	if ((input[12]) >= (1.62695)) {
		if ((input[54]) >= (37.5)) {
			if ((input[1]) >= (10.712851)) {
				var29 = -0.019345608;
			}
			else {
				var29 = 0.0113666495;
			}
		}
		else {
			if ((input[45]) >= (42.75975)) {
				var29 = -0.041739695;
			}
			else {
				var29 = -0.0017041346;
			}
		}
	}
	else {
		if ((input[1]) >= (0.017549999)) {
			if ((input[54]) >= (36.5)) {
				var29 = 0.024323814;
			}
			else {
				var29 = -0.012111951;
			}
		}
		else {
			if ((input[44]) >= (1.5)) {
				var29 = 0.08002058;
			}
			else {
				var29 = -0.017728;
			}
		}
	}
	double var30;
	if ((input[56]) >= (50.5)) {
		if ((input[54]) >= (37.5)) {
			if ((input[56]) >= (1590.5)) {
				var30 = -0.00362913;
			}
			else {
				var30 = 0.04456692;
			}
		}
		else {
			if ((input[51]) >= (0.00355)) {
				var30 = -0.018334394;
			}
			else {
				var30 = -0.1165415;
			}
		}
	}
	else {
		if ((input[60]) >= (2.5)) {
			if ((input[51]) >= (0.01445)) {
				var30 = 0.049233317;
			}
			else {
				var30 = -0.07555288;
			}
		}
		else {
			if ((input[54]) >= (35.5)) {
				var30 = 0.10179998;
			}
			else {
				var30 = 0.049299803;
			}
		}
	}
	double var31;
	if ((input[12]) >= (4.59575)) {
		if ((input[54]) >= (40.5)) {
			if ((input[56]) >= (3132.5)) {
				var31 = -0.0062742406;
			}
			else {
				var31 = 0.04868033;
			}
		}
		else {
			if ((input[51]) >= (0.02265)) {
				var31 = -0.024514234;
			}
			else {
				var31 = -0.06848968;
			}
		}
	}
	else {
		if ((input[1]) >= (0.02535)) {
			if ((input[47]) >= (1.40045)) {
				var31 = 0.016810572;
			}
			else {
				var31 = -0.01627871;
			}
		}
		else {
			if ((input[43]) >= (0.5)) {
				var31 = 0.006176974;
			}
			else {
				var31 = 0.07808408;
			}
		}
	}
	double var32;
	if ((input[15]) >= (3.5)) {
		if ((input[4]) >= (3.5)) {
			if ((input[56]) >= (50.5)) {
				var32 = -0.022945117;
			}
			else {
				var32 = 0.035528123;
			}
		}
		else {
			if ((input[56]) >= (181.5)) {
				var32 = 0.0074179037;
			}
			else {
				var32 = 0.0725014;
			}
		}
	}
	else {
		if ((input[56]) >= (449.5)) {
			if ((input[47]) >= (2.64745)) {
				var32 = 0.026946692;
			}
			else {
				var32 = -0.005505697;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var32 = -0.008985707;
			}
			else {
				var32 = 0.0759876;
			}
		}
	}
	double var33;
	if ((input[39]) >= (2.125)) {
		if ((input[25]) >= (5.1)) {
			if ((input[57]) >= (0.5)) {
				var33 = -0.031547572;
			}
			else {
				var33 = -0.059536275;
			}
		}
		else {
			if ((input[1]) >= (9.93165)) {
				var33 = -0.02418127;
			}
			else {
				var33 = 0.0036822588;
			}
		}
	}
	else {
		if ((input[61]) >= (321.5)) {
			if ((input[54]) >= (40.5)) {
				var33 = 0.021371892;
			}
			else {
				var33 = -0.0048640086;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var33 = 0.0260171;
			}
			else {
				var33 = 0.078297906;
			}
		}
	}
	double var34;
	if ((input[56]) >= (50.5)) {
		if ((input[47]) >= (1.11625)) {
			if ((input[56]) >= (828.5)) {
				var34 = -0.004629413;
			}
			else {
				var34 = 0.032803636;
			}
		}
		else {
			if ((input[50]) >= (0.02145)) {
				var34 = -0.029202586;
			}
			else {
				var34 = -0.10316926;
			}
		}
	}
	else {
		if ((input[60]) >= (2.5)) {
			if ((input[51]) >= (0.01445)) {
				var34 = 0.04112233;
			}
			else {
				var34 = -0.07150941;
			}
		}
		else {
			if ((input[54]) >= (35.5)) {
				var34 = 0.098970704;
			}
			else {
				var34 = 0.04312568;
			}
		}
	}
	double var35;
	if ((input[32]) >= (0.5)) {
		if ((input[56]) >= (699.5)) {
			if ((input[54]) >= (40.5)) {
				var35 = 0.012643183;
			}
			else {
				var35 = -0.0111737745;
			}
		}
		else {
			if ((input[54]) >= (37.5)) {
				var35 = 0.069922656;
			}
			else {
				var35 = 0.012100518;
			}
		}
	}
	else {
		if ((input[25]) >= (7.1)) {
			if ((input[56]) >= (18.5)) {
				var35 = -0.05445075;
			}
			else {
				var35 = 0.08103121;
			}
		}
		else {
			if ((input[47]) >= (1.0009)) {
				var35 = -0.008670153;
			}
			else {
				var35 = -0.04326259;
			}
		}
	}
	double var36;
	if ((input[41]) >= (2.5)) {
		if ((input[56]) >= (69.5)) {
			if ((input[61]) >= (22763.5)) {
				var36 = -0.02226057;
			}
			else {
				var36 = 0.015828488;
			}
		}
		else {
			if ((input[54]) >= (35.5)) {
				var36 = 0.0992768;
			}
			else {
				var36 = 0.037822425;
			}
		}
	}
	else {
		if ((input[24]) >= (8.0)) {
			if ((input[12]) >= (20.412151)) {
				var36 = -0.06564426;
			}
			else {
				var36 = -0.036085356;
			}
		}
		else {
			if ((input[60]) >= (254.5)) {
				var36 = -0.060982134;
			}
			else {
				var36 = -0.0019707435;
			}
		}
	}
	double var37;
	if ((input[15]) >= (3.5)) {
		if ((input[4]) >= (3.5)) {
			if ((input[54]) >= (40.5)) {
				var37 = 0.001574665;
			}
			else {
				var37 = -0.024263011;
			}
		}
		else {
			if ((input[56]) >= (93.5)) {
				var37 = 0.007574013;
			}
			else {
				var37 = 0.08233314;
			}
		}
	}
	else {
		if ((input[56]) >= (133.5)) {
			if ((input[47]) >= (1.6963)) {
				var37 = 0.020274198;
			}
			else {
				var37 = -0.014675478;
			}
		}
		else {
			if ((input[54]) >= (35.5)) {
				var37 = 0.085803665;
			}
			else {
				var37 = -0.0056960946;
			}
		}
	}
	double var38;
	if ((input[12]) >= (5.84575)) {
		if ((input[50]) >= (3.83395)) {
			if ((input[54]) >= (37.5)) {
				var38 = -0.0011100186;
			}
			else {
				var38 = -0.029779537;
			}
		}
		else {
			if ((input[56]) >= (59.5)) {
				var38 = -0.055000525;
			}
			else {
				var38 = 0.057080608;
			}
		}
	}
	else {
		if ((input[54]) >= (40.5)) {
			if ((input[56]) >= (3899.0)) {
				var38 = 0.009198146;
			}
			else {
				var38 = 0.07358022;
			}
		}
		else {
			if ((input[47]) >= (1.6029501)) {
				var38 = 0.012549325;
			}
			else {
				var38 = -0.014538179;
			}
		}
	}
	double var39;
	if ((input[56]) >= (50.5)) {
		if ((input[51]) >= (0.00315)) {
			if ((input[61]) >= (31223.5)) {
				var39 = -0.024712056;
			}
			else {
				var39 = 0.0047025485;
			}
		}
		else {
			if ((input[61]) >= (43.5)) {
				var39 = -0.10796892;
			}
			else {
				var39 = -0.04107268;
			}
		}
	}
	else {
		if ((input[60]) >= (1.5)) {
			if ((input[51]) >= (0.00175)) {
				var39 = 0.02734213;
			}
			else {
				var39 = -0.062886275;
			}
		}
		else {
			if ((input[54]) >= (35.5)) {
				var39 = 0.10067626;
			}
			else {
				var39 = 0.051882204;
			}
		}
	}
	double var40;
	if ((input[0]) >= (6.0)) {
		if ((input[56]) >= (11.5)) {
			if ((input[51]) >= (0.03345)) {
				var40 = -0.009572441;
			}
			else {
				var40 = -0.051133137;
			}
		}
		else {
			if ((input[60]) >= (1.5)) {
				var40 = 0.011393376;
			}
			else {
				var40 = 0.100508235;
			}
		}
	}
	else {
		if ((input[56]) >= (133.5)) {
			if ((input[54]) >= (41.5)) {
				var40 = 0.018007493;
			}
			else {
				var40 = -0.0066278423;
			}
		}
		else {
			if ((input[51]) >= (0.00055)) {
				var40 = 0.083326146;
			}
			else {
				var40 = 0.008175401;
			}
		}
	}
	double var41;
	if ((input[1]) >= (4.60745)) {
		if ((input[51]) >= (0.028949998)) {
			if ((input[60]) >= (349.5)) {
				var41 = -0.068255715;
			}
			else {
				var41 = -0.0045527453;
			}
		}
		else {
			if ((input[56]) >= (59.5)) {
				var41 = -0.0589575;
			}
			else {
				var41 = 0.041679833;
			}
		}
	}
	else {
		if ((input[12]) >= (0.017549999)) {
			if ((input[47]) >= (1.66165)) {
				var41 = 0.015028958;
			}
			else {
				var41 = -0.010156955;
			}
		}
		else {
			if ((input[43]) >= (0.5)) {
				var41 = -0.0005331147;
			}
			else {
				var41 = 0.06774525;
			}
		}
	}
	double var42;
	if ((input[12]) >= (12.123051)) {
		if ((input[25]) >= (6.775)) {
			if ((input[60]) >= (3.5)) {
				var42 = -0.06909255;
			}
			else {
				var42 = -0.022659127;
			}
		}
		else {
			if ((input[54]) >= (43.5)) {
				var42 = 0.0056008114;
			}
			else {
				var42 = -0.020717978;
			}
		}
	}
	else {
		if ((input[54]) >= (40.5)) {
			if ((input[56]) >= (2100.5)) {
				var42 = 0.009564592;
			}
			else {
				var42 = 0.07542127;
			}
		}
		else {
			if ((input[56]) >= (46.5)) {
				var42 = -0.0046345065;
			}
			else {
				var42 = 0.04607666;
			}
		}
	}
	double var43;
	if ((input[32]) >= (0.5)) {
		if ((input[56]) >= (1094.5)) {
			if ((input[61]) >= (71229.5)) {
				var43 = -0.03938196;
			}
			else {
				var43 = 0.0015710499;
			}
		}
		else {
			if ((input[54]) >= (37.5)) {
				var43 = 0.057224847;
			}
			else {
				var43 = 0.0056244126;
			}
		}
	}
	else {
		if ((input[47]) >= (1.16195)) {
			if ((input[60]) >= (335.5)) {
				var43 = -0.07259912;
			}
			else {
				var43 = -0.0072106062;
			}
		}
		else {
			if ((input[56]) >= (19.5)) {
				var43 = -0.04586588;
			}
			else {
				var43 = 0.049433015;
			}
		}
	}
	double var44;
	if ((input[43]) >= (2.5)) {
		if ((input[47]) >= (1.1286)) {
			if ((input[60]) >= (12.5)) {
				var44 = -0.048821572;
			}
			else {
				var44 = -0.007686246;
			}
		}
		else {
			if ((input[50]) >= (0.52145004)) {
				var44 = -0.055092942;
			}
			else {
				var44 = -0.09535705;
			}
		}
	}
	else {
		if ((input[56]) >= (87.5)) {
			if ((input[48]) >= (9.623899)) {
				var44 = 0.003921781;
			}
			else {
				var44 = -0.027752025;
			}
		}
		else {
			if ((input[54]) >= (35.5)) {
				var44 = 0.08053576;
			}
			else {
				var44 = 0.0017948613;
			}
		}
	}
	double var45;
	if ((input[15]) >= (3.5)) {
		if ((input[4]) >= (3.5)) {
			if ((input[60]) >= (311.5)) {
				var45 = -0.06518782;
			}
			else {
				var45 = -0.011146139;
			}
		}
		else {
			if ((input[56]) >= (434.5)) {
				var45 = 0.0032900914;
			}
			else {
				var45 = 0.041585095;
			}
		}
	}
	else {
		if ((input[56]) >= (819.5)) {
			if ((input[47]) >= (2.64745)) {
				var45 = 0.0194334;
			}
			else {
				var45 = -0.009604706;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var45 = -0.012910606;
			}
			else {
				var45 = 0.05257022;
			}
		}
	}
	double var46;
	if ((input[61]) >= (31223.5)) {
		if ((input[38]) >= (2.625)) {
			if ((input[60]) >= (383.5)) {
				var46 = -0.08782613;
			}
			else {
				var46 = -0.035080697;
			}
		}
		else {
			if ((input[60]) >= (3.5)) {
				var46 = 0.0025702338;
			}
			else {
				var46 = -0.03193989;
			}
		}
	}
	else {
		if ((input[54]) >= (41.5)) {
			if ((input[56]) >= (2351.5)) {
				var46 = 0.014131901;
			}
			else {
				var46 = 0.06854244;
			}
		}
		else {
			if ((input[47]) >= (1.82145)) {
				var46 = 0.0075720483;
			}
			else {
				var46 = -0.015857844;
			}
		}
	}
	double var47;
	if ((input[25]) >= (5.29165)) {
		if ((input[41]) >= (1.5)) {
			if ((input[51]) >= (0.41575)) {
				var47 = 0.04614862;
			}
			else {
				var47 = -0.017368514;
			}
		}
		else {
			if ((input[60]) >= (11.5)) {
				var47 = -0.0548648;
			}
			else {
				var47 = -0.028580798;
			}
		}
	}
	else {
		if ((input[56]) >= (1061.5)) {
			if ((input[47]) >= (2.21105)) {
				var47 = 0.0029574735;
			}
			else {
				var47 = -0.02049304;
			}
		}
		else {
			if ((input[54]) >= (37.5)) {
				var47 = 0.049498778;
			}
			else {
				var47 = 0.0010743728;
			}
		}
	}
	double var48;
	if ((input[1]) >= (10.810551)) {
		if ((input[40]) >= (0.5)) {
			if ((input[56]) >= (103.5)) {
				var48 = -0.0117160855;
			}
			else {
				var48 = 0.06451084;
			}
		}
		else {
			if ((input[47]) >= (0.99979997)) {
				var48 = -0.037031483;
			}
			else {
				var48 = -0.09123619;
			}
		}
	}
	else {
		if ((input[47]) >= (1.7873499)) {
			if ((input[61]) >= (11323.0)) {
				var48 = -0.006240304;
			}
			else {
				var48 = 0.024584595;
			}
		}
		else {
			if ((input[56]) >= (45.5)) {
				var48 = -0.01432494;
			}
			else {
				var48 = 0.041330587;
			}
		}
	}
	double var49;
	if ((input[41]) >= (3.5)) {
		if ((input[61]) >= (319.5)) {
			if ((input[46]) >= (10.5368)) {
				var49 = 0.014485629;
			}
			else {
				var49 = -0.03871431;
			}
		}
		else {
			if ((input[54]) >= (37.5)) {
				var49 = 0.08736665;
			}
			else {
				var49 = 0.03687685;
			}
		}
	}
	else {
		if ((input[51]) >= (0.0015499999)) {
			if ((input[12]) >= (15.79885)) {
				var49 = -0.021635817;
			}
			else {
				var49 = 0.0022648578;
			}
		}
		else {
			if ((input[61]) >= (9.5)) {
				var49 = -0.10058657;
			}
			else {
				var49 = -0.01336931;
			}
		}
	}
	double var50;
	if ((input[0]) >= (6.0)) {
		if ((input[56]) >= (7.5)) {
			if ((input[46]) >= (2.3737001)) {
				var50 = -0.008935783;
			}
			else {
				var50 = -0.048127946;
			}
		}
		else {
			if ((input[61]) >= (-5.5)) {
				var50 = 0.10169583;
			}
			else {
				var50 = -0.05539465;
			}
		}
	}
	else {
		if ((input[56]) >= (462.5)) {
			if ((input[54]) >= (42.5)) {
				var50 = 0.01719258;
			}
			else {
				var50 = -0.0078071128;
			}
		}
		else {
			if ((input[47]) >= (0.84239995)) {
				var50 = 0.058749445;
			}
			else {
				var50 = -0.004936189;
			}
		}
	}
	double var51;
	if ((input[61]) >= (24396.0)) {
		if ((input[38]) >= (2.375)) {
			if ((input[54]) >= (37.5)) {
				var51 = -0.024138672;
			}
			else {
				var51 = -0.061573382;
			}
		}
		else {
			if ((input[60]) >= (6.5)) {
				var51 = 0.017571477;
			}
			else {
				var51 = -0.016692266;
			}
		}
	}
	else {
		if ((input[47]) >= (2.6500502)) {
			if ((input[54]) >= (42.5)) {
				var51 = 0.047343243;
			}
			else {
				var51 = 0.015360425;
			}
		}
		else {
			if ((input[56]) >= (99.5)) {
				var51 = -0.008340413;
			}
			else {
				var51 = 0.02971876;
			}
		}
	}
	double var52;
	if ((input[43]) >= (2.5)) {
		if ((input[47]) >= (0.99615)) {
			if ((input[60]) >= (11.5)) {
				var52 = -0.041491054;
			}
			else {
				var52 = -0.007725887;
			}
		}
		else {
			if ((input[1]) >= (4.52735)) {
				var52 = -0.095720544;
			}
			else {
				var52 = -0.050639957;
			}
		}
	}
	else {
		if ((input[56]) >= (69.5)) {
			if ((input[48]) >= (9.623899)) {
				var52 = 0.003913917;
			}
			else {
				var52 = -0.023734974;
			}
		}
		else {
			if ((input[54]) >= (35.5)) {
				var52 = 0.08160307;
			}
			else {
				var52 = 0.004528917;
			}
		}
	}
	double var53;
	if ((input[1]) >= (1.41215)) {
		if ((input[48]) >= (17.091301)) {
			if ((input[56]) >= (837.5)) {
				var53 = -0.0042544142;
			}
			else {
				var53 = 0.030922366;
			}
		}
		else {
			if ((input[56]) >= (163.5)) {
				var53 = -0.048693165;
			}
			else {
				var53 = -0.005379127;
			}
		}
	}
	else {
		if ((input[61]) >= (235.5)) {
			if ((input[48]) >= (8.75135)) {
				var53 = 0.010571875;
			}
			else {
				var53 = -0.032644834;
			}
		}
		else {
			if ((input[0]) >= (3.0)) {
				var53 = 0.011457449;
			}
			else {
				var53 = 0.06302575;
			}
		}
	}
	double var54;
	if ((input[51]) >= (0.00005)) {
		if ((input[56]) >= (99.5)) {
			if ((input[54]) >= (37.5)) {
				var54 = 0.0050882692;
			}
			else {
				var54 = -0.0119123;
			}
		}
		else {
			if ((input[54]) >= (35.5)) {
				var54 = 0.07860019;
			}
			else {
				var54 = 0.0054565077;
			}
		}
	}
	else {
		if ((input[56]) >= (7.5)) {
			if ((input[61]) >= (29.5)) {
				var54 = -0.11237182;
			}
			else {
				var54 = -0.037307497;
			}
		}
		else {
			if ((input[62]) >= (91.5)) {
				var54 = 0.00699949;
			}
			else {
				var54 = 0.1000407;
			}
		}
	}
	double var55;
	if ((input[61]) >= (68096.0)) {
		if ((input[54]) >= (40.5)) {
			if ((input[61]) >= (143647.5)) {
				var55 = -0.056608558;
			}
			else {
				var55 = -0.014579932;
			}
		}
		else {
			if ((input[45]) >= (69.6289)) {
				var55 = -0.07058629;
			}
			else {
				var55 = -0.01672797;
			}
		}
	}
	else {
		if ((input[54]) >= (44.5)) {
			if ((input[56]) >= (12605.5)) {
				var55 = 0.017202925;
			}
			else {
				var55 = 0.075525574;
			}
		}
		else {
			if ((input[47]) >= (0.72515)) {
				var55 = 0.0022806772;
			}
			else {
				var55 = -0.026830662;
			}
		}
	}
	double var56;
	if ((input[12]) >= (0.53325)) {
		if ((input[46]) >= (10.2101)) {
			if ((input[56]) >= (837.5)) {
				var56 = -0.0037579422;
			}
			else {
				var56 = 0.031996544;
			}
		}
		else {
			if ((input[56]) >= (166.5)) {
				var56 = -0.04178094;
			}
			else {
				var56 = -0.003676346;
			}
		}
	}
	else {
		if ((input[61]) >= (252.5)) {
			if ((input[48]) >= (1.96365)) {
				var56 = 0.010096411;
			}
			else {
				var56 = -0.064633004;
			}
		}
		else {
			if ((input[47]) >= (0.5027)) {
				var56 = 0.053136017;
			}
			else {
				var56 = -0.00053822214;
			}
		}
	}
	double var57;
	if ((input[24]) >= (8.0)) {
		if ((input[51]) >= (0.33735)) {
			if ((input[44]) >= (1.5)) {
				var57 = 0.039247822;
			}
			else {
				var57 = -0.033288613;
			}
		}
		else {
			if ((input[56]) >= (27.5)) {
				var57 = -0.028736914;
			}
			else {
				var57 = 0.049236894;
			}
		}
	}
	else {
		if ((input[61]) >= (6604.5)) {
			if ((input[38]) >= (2.58335)) {
				var57 = -0.020951876;
			}
			else {
				var57 = -0.00091033906;
			}
		}
		else {
			if ((input[54]) >= (41.5)) {
				var57 = 0.04550713;
			}
			else {
				var57 = 0.0033720774;
			}
		}
	}
	double var58;
	if ((input[0]) >= (6.0)) {
		if ((input[39]) >= (2.125)) {
			if ((input[45]) >= (46.9707)) {
				var58 = -0.028857375;
			}
			else {
				var58 = -0.0022334314;
			}
		}
		else {
			if ((input[56]) >= (46.5)) {
				var58 = 0.0052986466;
			}
			else {
				var58 = 0.07796436;
			}
		}
	}
	else {
		if ((input[56]) >= (462.5)) {
			if ((input[47]) >= (2.14695)) {
				var58 = 0.0049562575;
			}
			else {
				var58 = -0.015052891;
			}
		}
		else {
			if ((input[54]) >= (37.5)) {
				var58 = 0.0779219;
			}
			else {
				var58 = 0.013101764;
			}
		}
	}
	double var59;
	if ((input[12]) >= (15.724649)) {
		if ((input[51]) >= (0.10955)) {
			if ((input[56]) >= (584.0)) {
				var59 = -0.011706138;
			}
			else {
				var59 = 0.09892758;
			}
		}
		else {
			if ((input[60]) >= (15.5)) {
				var59 = -0.06658203;
			}
			else {
				var59 = -0.018892968;
			}
		}
	}
	else {
		if ((input[47]) >= (1.1703501)) {
			if ((input[56]) >= (743.5)) {
				var59 = 0.0016271943;
			}
			else {
				var59 = 0.029253507;
			}
		}
		else {
			if ((input[56]) >= (20.5)) {
				var59 = -0.021016508;
			}
			else {
				var59 = 0.05084876;
			}
		}
	}
	double var60;
	if ((input[4]) >= (3.5)) {
		if ((input[54]) >= (40.5)) {
			if ((input[56]) >= (2564.0)) {
				var60 = -0.002149253;
			}
			else {
				var60 = 0.052000005;
			}
		}
		else {
			if ((input[56]) >= (22878.5)) {
				var60 = -0.035485473;
			}
			else {
				var60 = -0.007745371;
			}
		}
	}
	else {
		if ((input[1]) >= (0.017549999)) {
			if ((input[52]) >= (1.6983)) {
				var60 = 0.009983956;
			}
			else {
				var60 = -0.035367228;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var60 = 0.008404635;
			}
			else {
				var60 = 0.07298113;
			}
		}
	}
	double var61;
	if ((input[43]) >= (2.5)) {
		if ((input[50]) >= (0.56055)) {
			if ((input[56]) >= (71.5)) {
				var61 = -0.02322403;
			}
			else {
				var61 = 0.036990657;
			}
		}
		else {
			if ((input[36]) >= (5.3438)) {
				var61 = -0.02684158;
			}
			else {
				var61 = -0.0921649;
			}
		}
	}
	else {
		if ((input[56]) >= (33.5)) {
			if ((input[50]) >= (0.02145)) {
				var61 = 0.0015038;
			}
			else {
				var61 = -0.063230865;
			}
		}
		else {
			if ((input[60]) >= (1.5)) {
				var61 = -0.012677214;
			}
			else {
				var61 = 0.08644518;
			}
		}
	}
	double var62;
	if ((input[61]) >= (71198.5)) {
		if ((input[45]) >= (141.75586)) {
			if ((input[45]) >= (146.80275)) {
				var62 = -0.05857722;
			}
			else {
				var62 = -0.12532067;
			}
		}
		else {
			if ((input[61]) >= (141133.5)) {
				var62 = -0.050146896;
			}
			else {
				var62 = -0.017865537;
			}
		}
	}
	else {
		if ((input[54]) >= (44.5)) {
			if ((input[56]) >= (12605.5)) {
				var62 = 0.016197195;
			}
			else {
				var62 = 0.06813971;
			}
		}
		else {
			if ((input[47]) >= (2.2021499)) {
				var62 = 0.006618914;
			}
			else {
				var62 = -0.008219443;
			}
		}
	}
	double var63;
	if ((input[60]) >= (334.5)) {
		if ((input[38]) >= (2.125)) {
			if ((input[48]) >= (2648.769)) {
				var63 = -0.040826306;
			}
			else {
				var63 = -0.07328006;
			}
		}
		else {
			if ((input[45]) >= (95.96875)) {
				var63 = 0.10157724;
			}
			else {
				var63 = -0.04009535;
			}
		}
	}
	else {
		if ((input[43]) >= (1.5)) {
			if ((input[52]) >= (1.71015)) {
				var63 = -0.0071288147;
			}
			else {
				var63 = -0.052629586;
			}
		}
		else {
			if ((input[56]) >= (467.5)) {
				var63 = -0.000039743823;
			}
			else {
				var63 = 0.023057867;
			}
		}
	}
	double var64;
	if ((input[0]) >= (6.0)) {
		if ((input[48]) >= (33.609253)) {
			if ((input[1]) >= (4.8496504)) {
				var64 = -0.016856646;
			}
			else {
				var64 = 0.017418055;
			}
		}
		else {
			if ((input[56]) >= (11.5)) {
				var64 = -0.02832455;
			}
			else {
				var64 = 0.06716625;
			}
		}
	}
	else {
		if ((input[56]) >= (181.5)) {
			if ((input[47]) >= (1.69505)) {
				var64 = 0.004877479;
			}
			else {
				var64 = -0.0153269395;
			}
		}
		else {
			if ((input[47]) >= (0.39665002)) {
				var64 = 0.06508522;
			}
			else {
				var64 = 0.00039687022;
			}
		}
	}
	double var65;
	if ((input[54]) >= (37.5)) {
		if ((input[56]) >= (1592.5)) {
			if ((input[47]) >= (2.6536999)) {
				var65 = 0.0065862495;
			}
			else {
				var65 = -0.017407939;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var65 = -0.0010075967;
			}
			else {
				var65 = 0.05091387;
			}
		}
	}
	else {
		if ((input[45]) >= (42.86915)) {
			if ((input[47]) >= (0.1814)) {
				var65 = -0.013080234;
			}
			else {
				var65 = -0.06270113;
			}
		}
		else {
			if ((input[60]) >= (1.5)) {
				var65 = 0.022559643;
			}
			else {
				var65 = -0.01118923;
			}
		}
	}
	double var66;
	if ((input[61]) >= (24396.0)) {
		if ((input[60]) >= (3.5)) {
			if ((input[38]) >= (2.125)) {
				var66 = -0.014283384;
			}
			else {
				var66 = 0.015825724;
			}
		}
		else {
			if ((input[55]) >= (40.5)) {
				var66 = -0.011756722;
			}
			else {
				var66 = -0.045675866;
			}
		}
	}
	else {
		if ((input[47]) >= (2.66625)) {
			if ((input[56]) >= (1381.5)) {
				var66 = 0.013172219;
			}
			else {
				var66 = 0.04828371;
			}
		}
		else {
			if ((input[1]) >= (0.50584996)) {
				var66 = -0.007991828;
			}
			else {
				var66 = 0.014265939;
			}
		}
	}
	double var67;
	if ((input[12]) >= (20.611351)) {
		if ((input[60]) >= (16.5)) {
			if ((input[51]) >= (0.2225)) {
				var67 = -0.014265256;
			}
			else {
				var67 = -0.052516736;
			}
		}
		else {
			if ((input[61]) >= (2227.5)) {
				var67 = -0.021091098;
			}
			else {
				var67 = 0.015351697;
			}
		}
	}
	else {
		if ((input[54]) >= (40.5)) {
			if ((input[56]) >= (2452.5)) {
				var67 = 0.0042905626;
			}
			else {
				var67 = 0.05566552;
			}
		}
		else {
			if ((input[50]) >= (1.37695)) {
				var67 = 0.00089483504;
			}
			else {
				var67 = -0.019492902;
			}
		}
	}
	double var68;
	if ((input[56]) >= (50.5)) {
		if ((input[46]) >= (0.1909)) {
			if ((input[60]) >= (334.5)) {
				var68 = -0.049767967;
			}
			else {
				var68 = 0.00087467005;
			}
		}
		else {
			if ((input[61]) >= (107.5)) {
				var68 = -0.110586755;
			}
			else {
				var68 = -0.033875305;
			}
		}
	}
	else {
		if ((input[60]) >= (1.5)) {
			if ((input[48]) >= (0.0077)) {
				var68 = 0.025314895;
			}
			else {
				var68 = -0.048595384;
			}
		}
		else {
			if ((input[54]) >= (35.5)) {
				var68 = 0.093121566;
			}
			else {
				var68 = 0.03164114;
			}
		}
	}
	double var69;
	if ((input[15]) >= (3.5)) {
		if ((input[60]) >= (316.5)) {
			if ((input[52]) >= (1417.2815)) {
				var69 = -0.023722468;
			}
			else {
				var69 = -0.064047985;
			}
		}
		else {
			if ((input[47]) >= (1.5148499)) {
				var69 = 0.00021655462;
			}
			else {
				var69 = -0.014917053;
			}
		}
	}
	else {
		if ((input[56]) >= (201.5)) {
			if ((input[46]) >= (10.565149)) {
				var69 = 0.009389016;
			}
			else {
				var69 = -0.025464898;
			}
		}
		else {
			if ((input[54]) >= (36.5)) {
				var69 = 0.064008564;
			}
			else {
				var69 = -0.0010733571;
			}
		}
	}
	double var70;
	if ((input[61]) >= (93380.5)) {
		if ((input[47]) >= (2.7422)) {
			if ((input[10]) >= (0.5)) {
				var70 = -0.015176778;
			}
			else {
				var70 = -0.044685762;
			}
		}
		else {
			if ((input[36]) >= (5.8091497)) {
				var70 = -0.04126488;
			}
			else {
				var70 = -0.10665866;
			}
		}
	}
	else {
		if ((input[54]) >= (44.5)) {
			if ((input[56]) >= (7685.5)) {
				var70 = 0.01676119;
			}
			else {
				var70 = 0.07268863;
			}
		}
		else {
			if ((input[12]) >= (7.58005)) {
				var70 = -0.01070186;
			}
			else {
				var70 = 0.0033755729;
			}
		}
	}
	double var71;
	if ((input[56]) >= (33.5)) {
		if ((input[48]) >= (0.0077)) {
			if ((input[0]) >= (6.0)) {
				var71 = -0.012264807;
			}
			else {
				var71 = 0.0032293112;
			}
		}
		else {
			if ((input[61]) >= (46.5)) {
				var71 = -0.09182713;
			}
			else {
				var71 = -0.026860297;
			}
		}
	}
	else {
		if ((input[60]) >= (1.5)) {
			if ((input[46]) >= (2.1213999)) {
				var71 = 0.06920453;
			}
			else {
				var71 = -0.031486552;
			}
		}
		else {
			if ((input[61]) >= (14.5)) {
				var71 = 0.045535125;
			}
			else {
				var71 = 0.09188278;
			}
		}
	}
	double var72;
	if ((input[61]) >= (16645.0)) {
		if ((input[60]) >= (3.5)) {
			if ((input[54]) >= (37.5)) {
				var72 = 0.0024139462;
			}
			else {
				var72 = -0.030878186;
			}
		}
		else {
			if ((input[54]) >= (46.5)) {
				var72 = 0.028667256;
			}
			else {
				var72 = -0.02745677;
			}
		}
	}
	else {
		if ((input[47]) >= (2.29585)) {
			if ((input[56]) >= (516.5)) {
				var72 = 0.012157015;
			}
			else {
				var72 = 0.054928638;
			}
		}
		else {
			if ((input[56]) >= (106.5)) {
				var72 = -0.008733569;
			}
			else {
				var72 = 0.020624457;
			}
		}
	}
	double var73;
	if ((input[1]) >= (10.810551)) {
		if ((input[51]) >= (0.101050004)) {
			if ((input[56]) >= (647.5)) {
				var73 = -0.0060961065;
			}
			else {
				var73 = 0.05867448;
			}
		}
		else {
			if ((input[60]) >= (13.5)) {
				var73 = -0.056496203;
			}
			else {
				var73 = -0.01140288;
			}
		}
	}
	else {
		if ((input[54]) >= (40.5)) {
			if ((input[56]) >= (4031.0)) {
				var73 = 0.0032726084;
			}
			else {
				var73 = 0.048836004;
			}
		}
		else {
			if ((input[60]) >= (1.5)) {
				var73 = 0.0032055487;
			}
			else {
				var73 = -0.014817299;
			}
		}
	}
	double var74;
	if ((input[56]) >= (1313.5)) {
		if ((input[48]) >= (466.7334)) {
			if ((input[61]) >= (6592.5)) {
				var74 = -0.0032154496;
			}
			else {
				var74 = 0.026348416;
			}
		}
		else {
			if ((input[45]) >= (117.63475)) {
				var74 = 0.0039858767;
			}
			else {
				var74 = -0.021456733;
			}
		}
	}
	else {
		if ((input[54]) >= (37.5)) {
			if ((input[60]) >= (3.5)) {
				var74 = -0.0070762546;
			}
			else {
				var74 = 0.047650438;
			}
		}
		else {
			if ((input[51]) >= (0.048550002)) {
				var74 = 0.012281238;
			}
			else {
				var74 = -0.016732255;
			}
		}
	}
	double var75;
	if ((input[56]) >= (247.5)) {
		if ((input[48]) >= (16.49485)) {
			if ((input[56]) >= (1261.5)) {
				var75 = -0.0023261753;
			}
			else {
				var75 = 0.019330341;
			}
		}
		else {
			if ((input[12]) >= (1.91215)) {
				var75 = -0.04858815;
			}
			else {
				var75 = -0.015175539;
			}
		}
	}
	else {
		if ((input[54]) >= (36.5)) {
			if ((input[47]) >= (1.26395)) {
				var75 = 0.07400289;
			}
			else {
				var75 = 0.023080539;
			}
		}
		else {
			if ((input[51]) >= (0.03355)) {
				var75 = 0.028948998;
			}
			else {
				var75 = -0.019394131;
			}
		}
	}
	double var76;
	if ((input[54]) >= (43.5)) {
		if ((input[56]) >= (7637.0)) {
			if ((input[46]) >= (238.9595)) {
				var76 = 0.011553974;
			}
			else {
				var76 = -0.040309537;
			}
		}
		else {
			if ((input[60]) >= (1.5)) {
				var76 = -0.0037788611;
			}
			else {
				var76 = 0.09342339;
			}
		}
	}
	else {
		if ((input[56]) >= (467.5)) {
			if ((input[48]) >= (16.8701)) {
				var76 = -0.002505903;
			}
			else {
				var76 = -0.03741269;
			}
		}
		else {
			if ((input[48]) >= (33.9032)) {
				var76 = 0.051624365;
			}
			else {
				var76 = 0.0018661242;
			}
		}
	}
	double var77;
	if ((input[49]) >= (1.02805)) {
		if ((input[61]) >= (59703.5)) {
			if ((input[45]) >= (132.11136)) {
				var77 = -0.058773316;
			}
			else {
				var77 = -0.013134698;
			}
		}
		else {
			if ((input[32]) >= (0.5)) {
				var77 = 0.009308386;
			}
			else {
				var77 = -0.0052769426;
			}
		}
	}
	else {
		if ((input[53]) >= (0.98975)) {
			if ((input[56]) >= (6.5)) {
				var77 = -0.02444405;
			}
			else {
				var77 = 0.09360582;
			}
		}
		else {
			if ((input[56]) >= (697.5)) {
				var77 = -0.0054225377;
			}
			else {
				var77 = 0.022547958;
			}
		}
	}
	double var78;
	if ((input[56]) >= (33.5)) {
		if ((input[47]) >= (0.21535)) {
			if ((input[0]) >= (6.0)) {
				var78 = -0.011065561;
			}
			else {
				var78 = 0.003162358;
			}
		}
		else {
			if ((input[45]) >= (74.03515)) {
				var78 = -0.063701965;
			}
			else {
				var78 = 0.012298127;
			}
		}
	}
	else {
		if ((input[60]) >= (1.5)) {
			if ((input[46]) >= (2.1213999)) {
				var78 = 0.06060061;
			}
			else {
				var78 = -0.026371181;
			}
		}
		else {
			if ((input[61]) >= (14.5)) {
				var78 = 0.04098249;
			}
			else {
				var78 = 0.08965989;
			}
		}
	}
	double var79;
	if ((input[56]) >= (105.5)) {
		if ((input[48]) >= (5.9369)) {
			if ((input[56]) >= (837.5)) {
				var79 = -0.0020075326;
			}
			else {
				var79 = 0.017438924;
			}
		}
		else {
			if ((input[54]) >= (40.5)) {
				var79 = 0.0146948295;
			}
			else {
				var79 = -0.03243902;
			}
		}
	}
	else {
		if ((input[54]) >= (35.5)) {
			if ((input[51]) >= (0.00055)) {
				var79 = 0.065876;
			}
			else {
				var79 = 0.015676687;
			}
		}
		else {
			if ((input[51]) >= (0.03125)) {
				var79 = 0.040505208;
			}
			else {
				var79 = -0.022800049;
			}
		}
	}
	double var80;
	if ((input[54]) >= (37.5)) {
		if ((input[56]) >= (1756.5)) {
			if ((input[47]) >= (2.17645)) {
				var80 = 0.0035168212;
			}
			else {
				var80 = -0.022761147;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var80 = -0.0029982214;
			}
			else {
				var80 = 0.040863156;
			}
		}
	}
	else {
		if ((input[45]) >= (42.86915)) {
			if ((input[48]) >= (0.81630003)) {
				var80 = -0.010624043;
			}
			else {
				var80 = -0.04600501;
			}
		}
		else {
			if ((input[60]) >= (1.5)) {
				var80 = 0.019880472;
			}
			else {
				var80 = -0.012831138;
			}
		}
	}
	double var81;
	if ((input[56]) >= (50.5)) {
		if ((input[48]) >= (9.5933)) {
			if ((input[56]) >= (827.5)) {
				var81 = -0.0014266437;
			}
			else {
				var81 = 0.02141571;
			}
		}
		else {
			if ((input[36]) >= (4.7661)) {
				var81 = -0.007995191;
			}
			else {
				var81 = -0.04122946;
			}
		}
	}
	else {
		if ((input[60]) >= (3.5)) {
			if ((input[61]) >= (15.5)) {
				var81 = 0.03168186;
			}
			else {
				var81 = -0.04902654;
			}
		}
		else {
			if ((input[54]) >= (35.5)) {
				var81 = 0.076844126;
			}
			else {
				var81 = 0.024038522;
			}
		}
	}
	double var82;
	if ((input[61]) >= (96464.0)) {
		if ((input[46]) >= (3565.5857)) {
			if ((input[61]) >= (98243.0)) {
				var82 = -0.08376297;
			}
			else {
				var82 = 0.11166121;
			}
		}
		else {
			if ((input[52]) >= (2363.894)) {
				var82 = 0.027365772;
			}
			else {
				var82 = -0.027341545;
			}
		}
	}
	else {
		if ((input[54]) >= (44.5)) {
			if ((input[56]) >= (12605.5)) {
				var82 = 0.012551688;
			}
			else {
				var82 = 0.055350352;
			}
		}
		else {
			if ((input[2]) >= (1.5)) {
				var82 = 0.0015456069;
			}
			else {
				var82 = -0.0153286485;
			}
		}
	}
	double var83;
	if ((input[56]) >= (164.5)) {
		if ((input[48]) >= (33.904198)) {
			if ((input[61]) >= (916.5)) {
				var83 = -0.0012343797;
			}
			else {
				var83 = 0.022906868;
			}
		}
		else {
			if ((input[12]) >= (2.30665)) {
				var83 = -0.036733594;
			}
			else {
				var83 = -0.0051218723;
			}
		}
	}
	else {
		if ((input[54]) >= (36.5)) {
			if ((input[60]) >= (3.5)) {
				var83 = -0.019310018;
			}
			else {
				var83 = 0.05950102;
			}
		}
		else {
			if ((input[56]) >= (19.5)) {
				var83 = -0.009097926;
			}
			else {
				var83 = 0.047164153;
			}
		}
	}
	double var84;
	if ((input[54]) >= (41.5)) {
		if ((input[56]) >= (3884.0)) {
			if ((input[46]) >= (229.3917)) {
				var84 = 0.0059306016;
			}
			else {
				var84 = -0.031882998;
			}
		}
		else {
			if ((input[60]) >= (1.5)) {
				var84 = 0.0010633221;
			}
			else {
				var84 = 0.072183006;
			}
		}
	}
	else {
		if ((input[12]) >= (0.49025)) {
			if ((input[60]) >= (1.5)) {
				var84 = -0.0018391594;
			}
			else {
				var84 = -0.023279468;
			}
		}
		else {
			if ((input[56]) >= (249.5)) {
				var84 = 0.0018850825;
			}
			else {
				var84 = 0.025260136;
			}
		}
	}
	double var85;
	if ((input[61]) >= (31223.5)) {
		if ((input[50]) >= (8.33395)) {
			if ((input[60]) >= (6.5)) {
				var85 = 0.004839146;
			}
			else {
				var85 = -0.017719429;
			}
		}
		else {
			if ((input[47]) >= (3.08825)) {
				var85 = -0.023155618;
			}
			else {
				var85 = -0.077332444;
			}
		}
	}
	else {
		if ((input[47]) >= (2.8188)) {
			if ((input[61]) >= (7011.5)) {
				var85 = 0.00691306;
			}
			else {
				var85 = 0.02762785;
			}
		}
		else {
			if ((input[43]) >= (0.5)) {
				var85 = -0.010930472;
			}
			else {
				var85 = 0.004366809;
			}
		}
	}
	double var86;
	if ((input[60]) >= (334.5)) {
		if ((input[38]) >= (2.625)) {
			if ((input[61]) >= (8182.5)) {
				var86 = -0.06239117;
			}
			else {
				var86 = -0.0057217414;
			}
		}
		else {
			if ((input[52]) >= (816.36316)) {
				var86 = 0.012494033;
			}
			else {
				var86 = -0.07405668;
			}
		}
	}
	else {
		if ((input[47]) >= (2.81855)) {
			if ((input[61]) >= (17344.0)) {
				var86 = -0.002620888;
			}
			else {
				var86 = 0.019673994;
			}
		}
		else {
			if ((input[56]) >= (247.5)) {
				var86 = -0.006785297;
			}
			else {
				var86 = 0.011979463;
			}
		}
	}
	double var87;
	if ((input[49]) >= (1.02805)) {
		if ((input[12]) >= (16.712849)) {
			if ((input[51]) >= (0.1816)) {
				var87 = -0.000146377;
			}
			else {
				var87 = -0.023822408;
			}
		}
		else {
			if ((input[61]) >= (16416.5)) {
				var87 = -0.004233198;
			}
			else {
				var87 = 0.008802624;
			}
		}
	}
	else {
		if ((input[53]) >= (0.98364997)) {
			if ((input[61]) >= (2298.5)) {
				var87 = -0.036812477;
			}
			else {
				var87 = -0.009584042;
			}
		}
		else {
			if ((input[60]) >= (15.5)) {
				var87 = -0.02097475;
			}
			else {
				var87 = 0.008995143;
			}
		}
	}
	double var88;
	if ((input[54]) >= (43.5)) {
		if ((input[56]) >= (7637.0)) {
			if ((input[53]) >= (0.98434997)) {
				var88 = -0.013272722;
			}
			else {
				var88 = 0.017551335;
			}
		}
		else {
			if ((input[60]) >= (1.5)) {
				var88 = -0.004121429;
			}
			else {
				var88 = 0.084671535;
			}
		}
	}
	else {
		if ((input[2]) >= (1.5)) {
			if ((input[61]) >= (6604.5)) {
				var88 = -0.008034868;
			}
			else {
				var88 = 0.005866897;
			}
		}
		else {
			if ((input[41]) >= (2.5)) {
				var88 = 0.0020357675;
			}
			else {
				var88 = -0.028097903;
			}
		}
	}
	double var89;
	if ((input[54]) >= (37.5)) {
		if ((input[56]) >= (1058.5)) {
			if ((input[48]) >= (34.167953)) {
				var89 = 0.0019280469;
			}
			else {
				var89 = -0.041776814;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var89 = -0.002474263;
			}
			else {
				var89 = 0.046377767;
			}
		}
	}
	else {
		if ((input[60]) >= (1.5)) {
			if ((input[45]) >= (33.38085)) {
				var89 = -0.006385474;
			}
			else {
				var89 = 0.0241224;
			}
		}
		else {
			if ((input[61]) >= (39.5)) {
				var89 = -0.034495573;
			}
			else {
				var89 = 0.05452652;
			}
		}
	}
	double var90;
	if ((input[47]) >= (0.72515)) {
		if ((input[56]) >= (254.5)) {
			if ((input[48]) >= (33.7947)) {
				var90 = 0.0016141884;
			}
			else {
				var90 = -0.01533784;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var90 = 0.000508564;
			}
			else {
				var90 = 0.047913086;
			}
		}
	}
	else {
		if ((input[45]) >= (74.00195)) {
			if ((input[61]) >= (80.5)) {
				var90 = -0.057509016;
			}
			else {
				var90 = -0.017910684;
			}
		}
		else {
			if ((input[61]) >= (2.5)) {
				var90 = -0.0016465326;
			}
			else {
				var90 = 0.07753702;
			}
		}
	}
	double var91;
	if ((input[54]) >= (40.5)) {
		if ((input[56]) >= (2492.0)) {
			if ((input[48]) >= (317.43555)) {
				var91 = 0.006011666;
			}
			else {
				var91 = -0.024146533;
			}
		}
		else {
			if ((input[60]) >= (1.5)) {
				var91 = 0.0011190153;
			}
			else {
				var91 = 0.06867905;
			}
		}
	}
	else {
		if ((input[60]) >= (1.5)) {
			if ((input[49]) >= (1.8691499)) {
				var91 = 0.012330892;
			}
			else {
				var91 = -0.005194951;
			}
		}
		else {
			if ((input[61]) >= (39.5)) {
				var91 = -0.02186713;
			}
			else {
				var91 = 0.060605537;
			}
		}
	}
	double var92;
	if ((input[47]) >= (1.5783501)) {
		if ((input[56]) >= (516.5)) {
			if ((input[54]) >= (37.5)) {
				var92 = 0.0037703214;
			}
			else {
				var92 = -0.010408918;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var92 = 0.0047565387;
			}
			else {
				var92 = 0.051646896;
			}
		}
	}
	else {
		if ((input[45]) >= (74.4043)) {
			if ((input[47]) >= (0.23765)) {
				var92 = -0.014227608;
			}
			else {
				var92 = -0.04573595;
			}
		}
		else {
			if ((input[16]) >= (9.675)) {
				var92 = 0.064635314;
			}
			else {
				var92 = -0.0011365504;
			}
		}
	}
	double var93;
	if ((input[61]) >= (71198.5)) {
		if ((input[30]) >= (0.5)) {
			if ((input[56]) >= (77445.0)) {
				var93 = -0.026388172;
			}
			else {
				var93 = -0.08762338;
			}
		}
		else {
			if ((input[54]) >= (42.5)) {
				var93 = 0.005773131;
			}
			else {
				var93 = -0.029570922;
			}
		}
	}
	else {
		if ((input[54]) >= (44.5)) {
			if ((input[56]) >= (12605.5)) {
				var93 = 0.0120284;
			}
			else {
				var93 = 0.047167826;
			}
		}
		else {
			if ((input[3]) >= (2.225)) {
				var93 = 0.0042334176;
			}
			else {
				var93 = -0.006691976;
			}
		}
	}
	double var94;
	if ((input[4]) >= (3.5)) {
		if ((input[12]) >= (24.169949)) {
			if ((input[45]) >= (139.67775)) {
				var94 = -0.060725857;
			}
			else {
				var94 = -0.015903015;
			}
		}
		else {
			if ((input[54]) >= (40.5)) {
				var94 = 0.007888232;
			}
			else {
				var94 = -0.0061575994;
			}
		}
	}
	else {
		if ((input[47]) >= (1.13815)) {
			if ((input[56]) >= (397.5)) {
				var94 = 0.006427443;
			}
			else {
				var94 = 0.035616986;
			}
		}
		else {
			if ((input[1]) >= (0.13865)) {
				var94 = -0.021851217;
			}
			else {
				var94 = 0.020780459;
			}
		}
	}
	double var95;
	if ((input[61]) >= (140237.0)) {
		if ((input[48]) >= (2831.226)) {
			if ((input[56]) >= (175107.0)) {
				var95 = -0.050074395;
			}
			else {
				var95 = -0.09759524;
			}
		}
		else {
			if ((input[12]) >= (5.89255)) {
				var95 = 0.0028737814;
			}
			else {
				var95 = -0.05930185;
			}
		}
	}
	else {
		if ((input[54]) >= (46.5)) {
			if ((input[56]) >= (30663.0)) {
				var95 = 0.030241463;
			}
			else {
				var95 = 0.095151834;
			}
		}
		else {
			if ((input[14]) >= (1.225)) {
				var95 = 0.0016447817;
			}
			else {
				var95 = -0.013813985;
			}
		}
	}
	double var96;
	if ((input[60]) >= (456.5)) {
		if ((input[47]) >= (2.4703)) {
			if ((input[46]) >= (2203.892)) {
				var96 = -0.041717116;
			}
			else {
				var96 = -0.0859387;
			}
		}
		else {
			if ((input[60]) >= (522.0)) {
				var96 = -0.050575268;
			}
			else {
				var96 = 0.1719506;
			}
		}
	}
	else {
		if ((input[43]) >= (2.5)) {
			if ((input[36]) >= (4.35035)) {
				var96 = -0.010997169;
			}
			else {
				var96 = -0.04983064;
			}
		}
		else {
			if ((input[56]) >= (128.5)) {
				var96 = 0.00009161758;
			}
			else {
				var96 = 0.019975444;
			}
		}
	}
	double var97;
	if ((input[62]) >= (26.5)) {
		if ((input[1]) >= (0.27535)) {
			if ((input[54]) >= (37.5)) {
				var97 = -0.00047907582;
			}
			else {
				var97 = -0.012856907;
			}
		}
		else {
			if ((input[39]) >= (0.875)) {
				var97 = 0.0024341808;
			}
			else {
				var97 = 0.032364003;
			}
		}
	}
	else {
		if ((input[61]) >= (59317.5)) {
			if ((input[60]) >= (7.5)) {
				var97 = 0.010283237;
			}
			else {
				var97 = -0.03585767;
			}
		}
		else {
			if ((input[48]) >= (484.9398)) {
				var97 = 0.020742653;
			}
			else {
				var97 = 0.0024226566;
			}
		}
	}
	double var98;
	if ((input[61]) >= (6604.5)) {
		if ((input[60]) >= (7.5)) {
			if ((input[38]) >= (2.58335)) {
				var98 = -0.009809653;
			}
			else {
				var98 = 0.018591302;
			}
		}
		else {
			if ((input[54]) >= (42.5)) {
				var98 = -0.0014475061;
			}
			else {
				var98 = -0.025604377;
			}
		}
	}
	else {
		if ((input[48]) >= (703.19214)) {
			if ((input[53]) >= (0.9489)) {
				var98 = 0.026602585;
			}
			else {
				var98 = 0.07669154;
			}
		}
		else {
			if ((input[54]) >= (40.5)) {
				var98 = 0.017901203;
			}
			else {
				var98 = -0.0028673804;
			}
		}
	}
	double var99;
	if ((input[25]) >= (5.225)) {
		if ((input[51]) >= (0.44965)) {
			if ((input[40]) >= (2.5)) {
				var99 = 0.07834713;
			}
			else {
				var99 = -0.013594592;
			}
		}
		else {
			if ((input[56]) >= (9.5)) {
				var99 = -0.017020015;
			}
			else {
				var99 = 0.10587736;
			}
		}
	}
	else {
		if ((input[61]) >= (5866.5)) {
			if ((input[57]) >= (0.5)) {
				var99 = -0.010450383;
			}
			else {
				var99 = 0.0074046194;
			}
		}
		else {
			if ((input[47]) >= (2.2021499)) {
				var99 = 0.018776668;
			}
			else {
				var99 = -0.0012026965;
			}
		}
	}
	double var100;
	var100 = (1.0) / ((1.0) + (exp((0.0) - ((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((var0)+(var1)) + (var2)) + (var3)) + (var4)) + (var5)) + (var6)) + (var7)) + (var8)) + (var9)) + (var10)) + (var11)) + (var12)) + (var13)) + (var14)) + (var15)) + (var16)) + (var17)) + (var18)) + (var19)) + (var20)) + (var21)) + (var22)) + (var23)) + (var24)) + (var25)) + (var26)) + (var27)) + (var28)) + (var29)) + (var30)) + (var31)) + (var32)) + (var33)) + (var34)) + (var35)) + (var36)) + (var37)) + (var38)) + (var39)) + (var40)) + (var41)) + (var42)) + (var43)) + (var44)) + (var45)) + (var46)) + (var47)) + (var48)) + (var49)) + (var50)) + (var51)) + (var52)) + (var53)) + (var54)) + (var55)) + (var56)) + (var57)) + (var58)) + (var59)) + (var60)) + (var61)) + (var62)) + (var63)) + (var64)) + (var65)) + (var66)) + (var67)) + (var68)) + (var69)) + (var70)) + (var71)) + (var72)) + (var73)) + (var74)) + (var75)) + (var76)) + (var77)) + (var78)) + (var79)) + (var80)) + (var81)) + (var82)) + (var83)) + (var84)) + (var85)) + (var86)) + (var87)) + (var88)) + (var89)) + (var90)) + (var91)) + (var92)) + (var93)) + (var94)) + (var95)) + (var96)) + (var97)) + (var98)) + (var99)))));
	*output = var100 < 0.5 ? 0 : 1;
}

void PUclf4(double * input, double * output) {
	double var0;
	if ((input[60]) >= (2.5)) {
		if ((input[60]) >= (7.5)) {
			if ((input[60]) >= (13.5)) {
				var0 = -0.17476486;
			}
			else {
				var0 = -0.12518509;
			}
		}
		else {
			if ((input[60]) >= (4.5)) {
				var0 = -0.065671;
			}
			else {
				var0 = -0.009885817;
			}
		}
	}
	else {
		if ((input[61]) >= (400.5)) {
			if ((input[61]) >= (5077.5)) {
				var0 = -0.011931102;
			}
			else {
				var0 = 0.056883957;
			}
		}
		else {
			if ((input[61]) >= (57.5)) {
				var0 = 0.13735625;
			}
			else {
				var0 = 0.19363602;
			}
		}
	}
	double var1;
	if ((input[60]) >= (2.5)) {
		if ((input[60]) >= (7.5)) {
			if ((input[60]) >= (12.5)) {
				var1 = -0.15750726;
			}
			else {
				var1 = -0.1094988;
			}
		}
		else {
			if ((input[40]) >= (1.5)) {
				var1 = -0.010155753;
			}
			else {
				var1 = -0.063556604;
			}
		}
	}
	else {
		if ((input[61]) >= (472.5)) {
			if ((input[54]) >= (40.5)) {
				var1 = 0.0547675;
			}
			else {
				var1 = -0.01627479;
			}
		}
		else {
			if ((input[61]) >= (80.5)) {
				var1 = 0.1163724;
			}
			else {
				var1 = 0.17471516;
			}
		}
	}
	double var2;
	if ((input[60]) >= (3.5)) {
		if ((input[60]) >= (9.5)) {
			if ((input[39]) >= (6.58335)) {
				var2 = -0.15031219;
			}
			else {
				var2 = -0.12347058;
			}
		}
		else {
			if ((input[60]) >= (5.5)) {
				var2 = -0.07466714;
			}
			else {
				var2 = -0.026703639;
			}
		}
	}
	else {
		if ((input[61]) >= (488.5)) {
			if ((input[54]) >= (40.5)) {
				var2 = 0.042623814;
			}
			else {
				var2 = -0.018509692;
			}
		}
		else {
			if ((input[60]) >= (1.5)) {
				var2 = 0.07532926;
			}
			else {
				var2 = 0.15769474;
			}
		}
	}
	double var3;
	if ((input[60]) >= (2.5)) {
		if ((input[60]) >= (8.5)) {
			if ((input[60]) >= (13.5)) {
				var3 = -0.13565464;
			}
			else {
				var3 = -0.09807482;
			}
		}
		else {
			if ((input[60]) >= (4.5)) {
				var3 = -0.05426926;
			}
			else {
				var3 = -0.0055253073;
			}
		}
	}
	else {
		if ((input[61]) >= (223.5)) {
			if ((input[61]) >= (5057.5)) {
				var3 = -0.017900094;
			}
			else {
				var3 = 0.0526087;
			}
		}
		else {
			if ((input[43]) >= (1.5)) {
				var3 = 0.07659771;
			}
			else {
				var3 = 0.15012859;
			}
		}
	}
	double var4;
	if ((input[60]) >= (3.5)) {
		if ((input[60]) >= (9.5)) {
			if ((input[40]) >= (1.5)) {
				var4 = -0.10241293;
			}
			else {
				var4 = -0.12997577;
			}
		}
		else {
			if ((input[40]) >= (1.5)) {
				var4 = -0.022101138;
			}
			else {
				var4 = -0.06442111;
			}
		}
	}
	else {
		if ((input[61]) >= (289.5)) {
			if ((input[61]) >= (5099.5)) {
				var4 = -0.023429597;
			}
			else {
				var4 = 0.03952527;
			}
		}
		else {
			if ((input[41]) >= (0.5)) {
				var4 = 0.13664149;
			}
			else {
				var4 = 0.008588126;
			}
		}
	}
	double var5;
	if ((input[60]) >= (2.5)) {
		if ((input[57]) >= (0.5)) {
			if ((input[40]) >= (1.5)) {
				var5 = -0.0052923188;
			}
			else {
				var5 = -0.05076949;
			}
		}
		else {
			if ((input[60]) >= (14.5)) {
				var5 = -0.120334364;
			}
			else {
				var5 = -0.08537926;
			}
		}
	}
	else {
		if ((input[61]) >= (142.5)) {
			if ((input[61]) >= (3149.0)) {
				var5 = -0.0062770443;
			}
			else {
				var5 = 0.052361216;
			}
		}
		else {
			if ((input[60]) >= (1.5)) {
				var5 = 0.0776219;
			}
			else {
				var5 = 0.13775687;
			}
		}
	}
	double var6;
	if ((input[60]) >= (2.5)) {
		if ((input[60]) >= (7.5)) {
			if ((input[60]) >= (13.5)) {
				var6 = -0.11275692;
			}
			else {
				var6 = -0.07336116;
			}
		}
		else {
			if ((input[40]) >= (1.5)) {
				var6 = -0.0019999433;
			}
			else {
				var6 = -0.043059163;
			}
		}
	}
	else {
		if ((input[61]) >= (115.5)) {
			if ((input[54]) >= (37.5)) {
				var6 = 0.040299214;
			}
			else {
				var6 = -0.02990973;
			}
		}
		else {
			if ((input[43]) >= (1.5)) {
				var6 = 0.06089852;
			}
			else {
				var6 = 0.13163243;
			}
		}
	}
	double var7;
	if ((input[60]) >= (4.5)) {
		if ((input[60]) >= (11.5)) {
			if ((input[39]) >= (6.58335)) {
				var7 = -0.114100866;
			}
			else {
				var7 = -0.088355936;
			}
		}
		else {
			if ((input[47]) >= (2.34565)) {
				var7 = -0.018985337;
			}
			else {
				var7 = -0.0633431;
			}
		}
	}
	else {
		if ((input[61]) >= (560.5)) {
			if ((input[54]) >= (40.5)) {
				var7 = 0.027224991;
			}
			else {
				var7 = -0.029081315;
			}
		}
		else {
			if ((input[40]) >= (1.5)) {
				var7 = 0.11694046;
			}
			else {
				var7 = 0.02810947;
			}
		}
	}
	double var8;
	if ((input[60]) >= (1.5)) {
		if ((input[60]) >= (6.5)) {
			if ((input[60]) >= (13.5)) {
				var8 = -0.10135549;
			}
			else {
				var8 = -0.060124297;
			}
		}
		else {
			if ((input[61]) >= (3511.5)) {
				var8 = -0.039148096;
			}
			else {
				var8 = 0.013284044;
			}
		}
	}
	else {
		if ((input[61]) >= (161.5)) {
			if ((input[54]) >= (37.5)) {
				var8 = 0.04521063;
			}
			else {
				var8 = -0.03747297;
			}
		}
		else {
			if ((input[54]) >= (35.5)) {
				var8 = 0.12700409;
			}
			else {
				var8 = 0.08679124;
			}
		}
	}
	double var9;
	if ((input[60]) >= (1.5)) {
		if ((input[60]) >= (6.5)) {
			if ((input[60]) >= (13.5)) {
				var9 = -0.096291;
			}
			else {
				var9 = -0.05522408;
			}
		}
		else {
			if ((input[61]) >= (2797.5)) {
				var9 = -0.03261846;
			}
			else {
				var9 = 0.014004171;
			}
		}
	}
	else {
		if ((input[61]) >= (102.5)) {
			if ((input[61]) >= (10661.5)) {
				var9 = -0.031339124;
			}
			else {
				var9 = 0.042815745;
			}
		}
		else {
			if ((input[54]) >= (35.5)) {
				var9 = 0.1244747;
			}
			else {
				var9 = 0.090532996;
			}
		}
	}
	double var10;
	if ((input[60]) >= (1.5)) {
		if ((input[60]) >= (8.5)) {
			if ((input[39]) >= (6.58335)) {
				var10 = -0.096116655;
			}
			else {
				var10 = -0.06335167;
			}
		}
		else {
			if ((input[40]) >= (0.5)) {
				var10 = -0.000035556102;
			}
			else {
				var10 = -0.051781442;
			}
		}
	}
	else {
		if ((input[61]) >= (102.5)) {
			if ((input[54]) >= (37.5)) {
				var10 = 0.042012032;
			}
			else {
				var10 = -0.028859764;
			}
		}
		else {
			if ((input[54]) >= (35.5)) {
				var10 = 0.12112991;
			}
			else {
				var10 = 0.086053304;
			}
		}
	}
	double var11;
	if ((input[60]) >= (3.5)) {
		if ((input[60]) >= (11.5)) {
			if ((input[24]) >= (8.0)) {
				var11 = -0.10898413;
			}
			else {
				var11 = -0.07591759;
			}
		}
		else {
			if ((input[47]) >= (2.3452501)) {
				var11 = -0.0029436664;
			}
			else {
				var11 = -0.04206957;
			}
		}
	}
	else {
		if ((input[61]) >= (93.5)) {
			if ((input[61]) >= (1760.5)) {
				var11 = -0.01044752;
			}
			else {
				var11 = 0.04253259;
			}
		}
		else {
			if ((input[43]) >= (1.5)) {
				var11 = 0.017536737;
			}
			else {
				var11 = 0.11174818;
			}
		}
	}
	double var12;
	if ((input[60]) >= (1.5)) {
		if ((input[60]) >= (10.5)) {
			if ((input[40]) >= (0.5)) {
				var12 = -0.06601437;
			}
			else {
				var12 = -0.09770944;
			}
		}
		else {
			if ((input[40]) >= (0.5)) {
				var12 = -0.0017337017;
			}
			else {
				var12 = -0.04917489;
			}
		}
	}
	else {
		if ((input[61]) >= (81.5)) {
			if ((input[54]) >= (37.5)) {
				var12 = 0.038625635;
			}
			else {
				var12 = -0.026659457;
			}
		}
		else {
			if ((input[54]) >= (35.5)) {
				var12 = 0.116554;
			}
			else {
				var12 = 0.08138927;
			}
		}
	}
	double var13;
	if ((input[60]) >= (4.5)) {
		if ((input[60]) >= (15.5)) {
			if ((input[32]) >= (3.5)) {
				var13 = -0.03913502;
			}
			else {
				var13 = -0.087209396;
			}
		}
		else {
			if ((input[47]) >= (2.34425)) {
				var13 = -0.009635212;
			}
			else {
				var13 = -0.049904015;
			}
		}
	}
	else {
		if ((input[61]) >= (64.5)) {
			if ((input[61]) >= (6372.5)) {
				var13 = -0.0317489;
			}
			else {
				var13 = 0.025184369;
			}
		}
		else {
			if ((input[43]) >= (0.5)) {
				var13 = 0.040108763;
			}
			else {
				var13 = 0.11291998;
			}
		}
	}
	double var14;
	if ((input[60]) >= (1.5)) {
		if ((input[60]) >= (10.5)) {
			if ((input[24]) >= (8.0)) {
				var14 = -0.09900472;
			}
			else {
				var14 = -0.061187226;
			}
		}
		else {
			if ((input[24]) >= (8.0)) {
				var14 = -0.06910907;
			}
			else {
				var14 = -0.0039332756;
			}
		}
	}
	else {
		if ((input[61]) >= (229.5)) {
			if ((input[54]) >= (40.5)) {
				var14 = 0.040075026;
			}
			else {
				var14 = -0.023378896;
			}
		}
		else {
			if ((input[54]) >= (36.5)) {
				var14 = 0.11135794;
			}
			else {
				var14 = 0.06264037;
			}
		}
	}
	double var15;
	if ((input[60]) >= (1.5)) {
		if ((input[60]) >= (6.5)) {
			if ((input[60]) >= (15.5)) {
				var15 = -0.07451568;
			}
			else {
				var15 = -0.037816547;
			}
		}
		else {
			if ((input[61]) >= (4457.5)) {
				var15 = -0.030119544;
			}
			else {
				var15 = 0.010967547;
			}
		}
	}
	else {
		if ((input[61]) >= (49.5)) {
			if ((input[61]) >= (1046.5)) {
				var15 = -0.002081628;
			}
			else {
				var15 = 0.0509309;
			}
		}
		else {
			if ((input[54]) >= (35.5)) {
				var15 = 0.1120633;
			}
			else {
				var15 = 0.084697776;
			}
		}
	}
	double var16;
	if ((input[60]) >= (4.5)) {
		if ((input[60]) >= (15.5)) {
			if ((input[32]) >= (3.5)) {
				var16 = -0.025340555;
			}
			else {
				var16 = -0.07669272;
			}
		}
		else {
			if ((input[47]) >= (1.90855)) {
				var16 = -0.010251782;
			}
			else {
				var16 = -0.04937014;
			}
		}
	}
	else {
		if ((input[61]) >= (44.5)) {
			if ((input[61]) >= (1670.5)) {
				var16 = -0.01172094;
			}
			else {
				var16 = 0.032901656;
			}
		}
		else {
			if ((input[40]) >= (0.5)) {
				var16 = 0.099891685;
			}
			else {
				var16 = -0.037613776;
			}
		}
	}
	double var17;
	if ((input[1]) >= (1.1953)) {
		if ((input[57]) >= (0.5)) {
			if ((input[54]) >= (36.5)) {
				var17 = 0.0038360532;
			}
			else {
				var17 = -0.057563614;
			}
		}
		else {
			if ((input[54]) >= (36.5)) {
				var17 = -0.045662936;
			}
			else {
				var17 = -0.083865434;
			}
		}
	}
	else {
		if ((input[60]) >= (1.5)) {
			if ((input[60]) >= (5.5)) {
				var17 = -0.014619017;
			}
			else {
				var17 = 0.037059244;
			}
		}
		else {
			if ((input[61]) >= (49.5)) {
				var17 = 0.049862258;
			}
			else {
				var17 = 0.10762211;
			}
		}
	}
	double var18;
	if ((input[12]) >= (0.0703)) {
		if ((input[60]) >= (5.5)) {
			if ((input[24]) >= (8.0)) {
				var18 = -0.08217959;
			}
			else {
				var18 = -0.035532575;
			}
		}
		else {
			if ((input[61]) >= (1046.5)) {
				var18 = -0.009669214;
			}
			else {
				var18 = 0.03482434;
			}
		}
	}
	else {
		if ((input[44]) >= (2.5)) {
			if ((input[1]) >= (0.0391)) {
				var18 = 0.078021206;
			}
			else {
				var18 = 0.1083534;
			}
		}
		else {
			if ((input[60]) >= (2.5)) {
				var18 = -0.016711032;
			}
			else {
				var18 = 0.06998733;
			}
		}
	}
	double var19;
	if ((input[40]) >= (2.5)) {
		if ((input[61]) >= (308.5)) {
			if ((input[54]) >= (40.5)) {
				var19 = 0.034060042;
			}
			else {
				var19 = -0.014911853;
			}
		}
		else {
			if ((input[54]) >= (35.5)) {
				var19 = 0.09933911;
			}
			else {
				var19 = 0.02742963;
			}
		}
	}
	else {
		if ((input[60]) >= (10.5)) {
			if ((input[24]) >= (8.0)) {
				var19 = -0.088948615;
			}
			else {
				var19 = -0.05187012;
			}
		}
		else {
			if ((input[12]) >= (2.9453)) {
				var19 = -0.02300044;
			}
			else {
				var19 = 0.012937632;
			}
		}
	}
	double var20;
	if ((input[1]) >= (0.0703)) {
		if ((input[54]) >= (36.5)) {
			if ((input[61]) >= (1047.5)) {
				var20 = -0.016705178;
			}
			else {
				var20 = 0.040597998;
			}
		}
		else {
			if ((input[56]) >= (18.5)) {
				var20 = -0.060131457;
			}
			else {
				var20 = 0.05589942;
			}
		}
	}
	else {
		if ((input[44]) >= (1.5)) {
			if ((input[56]) >= (72.5)) {
				var20 = 0.071379304;
			}
			else {
				var20 = 0.10453039;
			}
		}
		else {
			if ((input[45]) >= (82.96875)) {
				var20 = -0.06689583;
			}
			else {
				var20 = 0.03505007;
			}
		}
	}
	double var21;
	if ((input[12]) >= (0.0703)) {
		if ((input[60]) >= (5.5)) {
			if ((input[24]) >= (8.0)) {
				var21 = -0.07432572;
			}
			else {
				var21 = -0.02932264;
			}
		}
		else {
			if ((input[61]) >= (10602.0)) {
				var21 = -0.035305258;
			}
			else {
				var21 = 0.01635657;
			}
		}
	}
	else {
		if ((input[60]) >= (1.5)) {
			if ((input[51]) >= (0.00135)) {
				var21 = 0.049492966;
			}
			else {
				var21 = -0.053501662;
			}
		}
		else {
			if ((input[61]) >= (30.5)) {
				var21 = 0.07024064;
			}
			else {
				var21 = 0.10609317;
			}
		}
	}
	double var22;
	if ((input[1]) >= (2.4766002)) {
		if ((input[54]) >= (37.5)) {
			if ((input[61]) >= (6372.5)) {
				var22 = -0.027609766;
			}
			else {
				var22 = 0.01624294;
			}
		}
		else {
			if ((input[12]) >= (2.4766002)) {
				var22 = -0.062160224;
			}
			else {
				var22 = -0.008681739;
			}
		}
	}
	else {
		if ((input[60]) >= (0.5)) {
			if ((input[32]) >= (3.5)) {
				var22 = 0.053798843;
			}
			else {
				var22 = 0.0061170454;
			}
		}
		else {
			if ((input[56]) >= (65.5)) {
				var22 = 0.05574155;
			}
			else {
				var22 = 0.105315164;
			}
		}
	}
	double var23;
	if ((input[40]) >= (2.5)) {
		if ((input[61]) >= (148.5)) {
			if ((input[54]) >= (37.5)) {
				var23 = 0.023190875;
			}
			else {
				var23 = -0.025690332;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var23 = 0.04019716;
			}
			else {
				var23 = 0.099520564;
			}
		}
	}
	else {
		if ((input[60]) >= (12.5)) {
			if ((input[0]) >= (6.0)) {
				var23 = -0.07657185;
			}
			else {
				var23 = -0.042973403;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var23 = -0.040622126;
			}
			else {
				var23 = -0.00093086326;
			}
		}
	}
	double var24;
	if ((input[56]) >= (23.5)) {
		if ((input[40]) >= (1.5)) {
			if ((input[61]) >= (1047.5)) {
				var24 = -0.006676331;
			}
			else {
				var24 = 0.034011;
			}
		}
		else {
			if ((input[24]) >= (8.0)) {
				var24 = -0.07302002;
			}
			else {
				var24 = -0.021307265;
			}
		}
	}
	else {
		if ((input[60]) >= (1.5)) {
			if ((input[51]) >= (0.00315)) {
				var24 = 0.053477544;
			}
			else {
				var24 = -0.07114252;
			}
		}
		else {
			if ((input[56]) >= (-148.0)) {
				var24 = 0.102818586;
			}
			else {
				var24 = -0.00068501855;
			}
		}
	}
	double var25;
	if ((input[1]) >= (0.0703)) {
		if ((input[54]) >= (37.5)) {
			if ((input[61]) >= (3890.5)) {
				var25 = -0.01679638;
			}
			else {
				var25 = 0.031544667;
			}
		}
		else {
			if ((input[61]) >= (443.5)) {
				var25 = -0.0557531;
			}
			else {
				var25 = -0.007755381;
			}
		}
	}
	else {
		if ((input[43]) >= (0.5)) {
			if ((input[60]) >= (2.5)) {
				var25 = -0.037134137;
			}
			else {
				var25 = 0.04639728;
			}
		}
		else {
			if ((input[56]) >= (72.5)) {
				var25 = 0.05916208;
			}
			else {
				var25 = 0.10195171;
			}
		}
	}
	double var26;
	if ((input[60]) >= (0.5)) {
		if ((input[40]) >= (0.5)) {
			if ((input[1]) >= (4.7578)) {
				var26 = -0.013023085;
			}
			else {
				var26 = 0.01883925;
			}
		}
		else {
			if ((input[47]) >= (0.32665002)) {
				var26 = -0.038240075;
			}
			else {
				var26 = -0.10757935;
			}
		}
	}
	else {
		if ((input[56]) >= (751.5)) {
			if ((input[54]) >= (40.5)) {
				var26 = 0.046606716;
			}
			else {
				var26 = -0.063117474;
			}
		}
		else {
			if ((input[54]) >= (37.5)) {
				var26 = 0.10468073;
			}
			else {
				var26 = 0.052267205;
			}
		}
	}
	double var27;
	if ((input[56]) >= (22.5)) {
		if ((input[54]) >= (36.5)) {
			if ((input[61]) >= (799.5)) {
				var27 = -0.008857443;
			}
			else {
				var27 = 0.03766239;
			}
		}
		else {
			if ((input[1]) >= (1.7890999)) {
				var27 = -0.05822865;
			}
			else {
				var27 = -0.01261826;
			}
		}
	}
	else {
		if ((input[60]) >= (1.5)) {
			if ((input[51]) >= (0.00315)) {
				var27 = 0.051488083;
			}
			else {
				var27 = -0.059768546;
			}
		}
		else {
			if ((input[56]) >= (-43.0)) {
				var27 = 0.101229504;
			}
			else {
				var27 = 0.011353689;
			}
		}
	}
	double var28;
	if ((input[56]) >= (22.5)) {
		if ((input[54]) >= (40.5)) {
			if ((input[61]) >= (12248.5)) {
				var28 = -0.020746758;
			}
			else {
				var28 = 0.04013175;
			}
		}
		else {
			if ((input[1]) >= (4.3515997)) {
				var28 = -0.036851816;
			}
			else {
				var28 = 0.005072538;
			}
		}
	}
	else {
		if ((input[60]) >= (1.5)) {
			if ((input[51]) >= (0.022950001)) {
				var28 = 0.09200631;
			}
			else {
				var28 = -0.02835396;
			}
		}
		else {
			if ((input[56]) >= (-43.0)) {
				var28 = 0.100325935;
			}
			else {
				var28 = 0.010394668;
			}
		}
	}
	double var29;
	if ((input[32]) >= (3.5)) {
		if ((input[56]) >= (884.5)) {
			if ((input[47]) >= (2.3590999)) {
				var29 = 0.029236717;
			}
			else {
				var29 = -0.017888196;
			}
		}
		else {
			if ((input[43]) >= (1.5)) {
				var29 = 0.0015529975;
			}
			else {
				var29 = 0.08216678;
			}
		}
	}
	else {
		if ((input[60]) >= (12.5)) {
			if ((input[54]) >= (40.5)) {
				var29 = -0.010495513;
			}
			else {
				var29 = -0.054578066;
			}
		}
		else {
			if ((input[54]) >= (41.5)) {
				var29 = 0.020830547;
			}
			else {
				var29 = -0.012617879;
			}
		}
	}
	double var30;
	if ((input[12]) >= (1.8672)) {
		if ((input[54]) >= (40.5)) {
			if ((input[61]) >= (16682.5)) {
				var30 = -0.029118387;
			}
			else {
				var30 = 0.026971525;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var30 = -0.062459625;
			}
			else {
				var30 = -0.015928233;
			}
		}
	}
	else {
		if ((input[56]) >= (11.5)) {
			if ((input[54]) >= (35.5)) {
				var30 = 0.03231707;
			}
			else {
				var30 = -0.023809174;
			}
		}
		else {
			if ((input[43]) >= (2.5)) {
				var30 = -0.017847842;
			}
			else {
				var30 = 0.10080623;
			}
		}
	}
	double var31;
	if ((input[40]) >= (3.5)) {
		if ((input[61]) >= (81.5)) {
			if ((input[61]) >= (9117.5)) {
				var31 = -0.019834442;
			}
			else {
				var31 = 0.033447802;
			}
		}
		else {
			if ((input[54]) >= (35.5)) {
				var31 = 0.10053235;
			}
			else {
				var31 = 0.06843825;
			}
		}
	}
	else {
		if ((input[24]) >= (8.0)) {
			if ((input[56]) >= (13.5)) {
				var31 = -0.05567036;
			}
			else {
				var31 = 0.06673235;
			}
		}
		else {
			if ((input[61]) >= (6233.5)) {
				var31 = -0.025839617;
			}
			else {
				var31 = 0.007758646;
			}
		}
	}
	double var32;
	if ((input[56]) >= (11.5)) {
		if ((input[0]) >= (6.0)) {
			if ((input[47]) >= (1.4672501)) {
				var32 = -0.018135887;
			}
			else {
				var32 = -0.06402599;
			}
		}
		else {
			if ((input[61]) >= (450.5)) {
				var32 = -0.0063751685;
			}
			else {
				var32 = 0.043914005;
			}
		}
	}
	else {
		if ((input[43]) >= (2.5)) {
			if ((input[47]) >= (0.2368)) {
				var32 = 0.07210023;
			}
			else {
				var32 = -0.09480789;
			}
		}
		else {
			if ((input[52]) >= (1381.3267)) {
				var32 = -0.11257142;
			}
			else {
				var32 = 0.09874846;
			}
		}
	}
	double var33;
	if ((input[56]) >= (23.5)) {
		if ((input[54]) >= (37.5)) {
			if ((input[61]) >= (2767.5)) {
				var33 = -0.008597693;
			}
			else {
				var33 = 0.030909443;
			}
		}
		else {
			if ((input[12]) >= (4.64845)) {
				var33 = -0.043732684;
			}
			else {
				var33 = -0.0058773747;
			}
		}
	}
	else {
		if ((input[60]) >= (1.5)) {
			if ((input[51]) >= (0.00315)) {
				var33 = 0.043088578;
			}
			else {
				var33 = -0.050298627;
			}
		}
		else {
			if ((input[60]) >= (0.5)) {
				var33 = 0.07621725;
			}
			else {
				var33 = 0.10188466;
			}
		}
	}
	double var34;
	if ((input[17]) >= (2.5)) {
		if ((input[12]) >= (3.71095)) {
			if ((input[47]) >= (1.7295)) {
				var34 = 0.021247959;
			}
			else {
				var34 = -0.03379209;
			}
		}
		else {
			if ((input[32]) >= (4.5)) {
				var34 = 0.077194184;
			}
			else {
				var34 = 0.032705225;
			}
		}
	}
	else {
		if ((input[40]) >= (0.5)) {
			if ((input[61]) >= (46.5)) {
				var34 = -0.0065796035;
			}
			else {
				var34 = 0.041982304;
			}
		}
		else {
			if ((input[51]) >= (0.01715)) {
				var34 = -0.027450174;
			}
			else {
				var34 = -0.07563906;
			}
		}
	}
	double var35;
	if ((input[56]) >= (11.5)) {
		if ((input[0]) >= (6.0)) {
			if ((input[46]) >= (1.04625)) {
				var35 = -0.02261751;
			}
			else {
				var35 = -0.07992369;
			}
		}
		else {
			if ((input[61]) >= (402.5)) {
				var35 = -0.0046875007;
			}
			else {
				var35 = 0.040656675;
			}
		}
	}
	else {
		if ((input[60]) >= (0.5)) {
			if ((input[45]) >= (95.8125)) {
				var35 = 0.01808944;
			}
			else {
				var35 = 0.081385;
			}
		}
		else {
			if ((input[56]) >= (-59.0)) {
				var35 = 0.10221501;
			}
			else {
				var35 = 0.012209161;
			}
		}
	}
	double var36;
	if ((input[39]) >= (0.875)) {
		if ((input[53]) >= (0.98975)) {
			if ((input[1]) >= (3.33595)) {
				var36 = -0.031032214;
			}
			else {
				var36 = 0.00037399065;
			}
		}
		else {
			if ((input[60]) >= (16.5)) {
				var36 = -0.03531633;
			}
			else {
				var36 = 0.018255083;
			}
		}
	}
	else {
		if ((input[61]) >= (178.5)) {
			if ((input[47]) >= (2.11815)) {
				var36 = 0.03192646;
			}
			else {
				var36 = -0.015378541;
			}
		}
		else {
			if ((input[54]) >= (35.5)) {
				var36 = 0.08584243;
			}
			else {
				var36 = 0.03869068;
			}
		}
	}
	double var37;
	if ((input[32]) >= (2.5)) {
		if ((input[61]) >= (3545.5)) {
			if ((input[48]) >= (1203.7109)) {
				var37 = 0.008773006;
			}
			else {
				var37 = -0.03688955;
			}
		}
		else {
			if ((input[56]) >= (70.5)) {
				var37 = 0.029779136;
			}
			else {
				var37 = 0.0881887;
			}
		}
	}
	else {
		if ((input[54]) >= (41.5)) {
			if ((input[61]) >= (21195.0)) {
				var37 = -0.026912317;
			}
			else {
				var37 = 0.027539948;
			}
		}
		else {
			if ((input[61]) >= (662.5)) {
				var37 = -0.034298815;
			}
			else {
				var37 = -0.00046876175;
			}
		}
	}
	double var38;
	if ((input[56]) >= (11.5)) {
		if ((input[47]) >= (2.3691502)) {
			if ((input[61]) >= (5122.5)) {
				var38 = -0.007030014;
			}
			else {
				var38 = 0.037300896;
			}
		}
		else {
			if ((input[61]) >= (496.5)) {
				var38 = -0.029219031;
			}
			else {
				var38 = 0.005236144;
			}
		}
	}
	else {
		if ((input[60]) >= (0.5)) {
			if ((input[45]) >= (95.8125)) {
				var38 = 0.014988367;
			}
			else {
				var38 = 0.07604759;
			}
		}
		else {
			if ((input[48]) >= (51.4121)) {
				var38 = 0.0050782803;
			}
			else {
				var38 = 0.10179023;
			}
		}
	}
	double var39;
	if ((input[54]) >= (41.5)) {
		if ((input[61]) >= (15808.0)) {
			if ((input[54]) >= (43.5)) {
				var39 = 0.00705003;
			}
			else {
				var39 = -0.035502806;
			}
		}
		else {
			if ((input[60]) >= (1.5)) {
				var39 = 0.01827275;
			}
			else {
				var39 = 0.05863582;
			}
		}
	}
	else {
		if ((input[61]) >= (2866.5)) {
			if ((input[53]) >= (0.97905)) {
				var39 = -0.05489127;
			}
			else {
				var39 = -0.011812194;
			}
		}
		else {
			if ((input[47]) >= (2.07285)) {
				var39 = 0.030836046;
			}
			else {
				var39 = -0.011369204;
			}
		}
	}
	double var40;
	if ((input[24]) >= (8.0)) {
		if ((input[54]) >= (43.5)) {
			if ((input[21]) >= (0.5)) {
				var40 = 0.06184208;
			}
			else {
				var40 = -0.0017860804;
			}
		}
		else {
			if ((input[1]) >= (19.773449)) {
				var40 = -0.06516254;
			}
			else {
				var40 = -0.036633745;
			}
		}
	}
	else {
		if ((input[56]) >= (65.5)) {
			if ((input[47]) >= (1.7458501)) {
				var40 = 0.010370931;
			}
			else {
				var40 = -0.022945916;
			}
		}
		else {
			if ((input[54]) >= (35.5)) {
				var40 = 0.087025754;
			}
			else {
				var40 = 0.0009790886;
			}
		}
	}
	double var41;
	if ((input[1]) >= (10.4766)) {
		if ((input[48]) >= (765.91797)) {
			if ((input[61]) >= (17430.0)) {
				var41 = -0.032152362;
			}
			else {
				var41 = 0.008531182;
			}
		}
		else {
			if ((input[60]) >= (17.5)) {
				var41 = -0.07321731;
			}
			else {
				var41 = -0.031983826;
			}
		}
	}
	else {
		if ((input[6]) >= (2.5)) {
			if ((input[12]) >= (0.0078)) {
				var41 = 0.02578178;
			}
			else {
				var41 = 0.08554926;
			}
		}
		else {
			if ((input[47]) >= (1.1099)) {
				var41 = 0.008067711;
			}
			else {
				var41 = -0.026371425;
			}
		}
	}
	double var42;
	if ((input[54]) >= (37.5)) {
		if ((input[56]) >= (1389.5)) {
			if ((input[47]) >= (2.69915)) {
				var42 = 0.020279871;
			}
			else {
				var42 = -0.015778912;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var42 = -0.012223617;
			}
			else {
				var42 = 0.055214334;
			}
		}
	}
	else {
		if ((input[61]) >= (93.5)) {
			if ((input[48]) >= (9.20115)) {
				var42 = -0.021189159;
			}
			else {
				var42 = -0.08090092;
			}
		}
		else {
			if ((input[43]) >= (0.5)) {
				var42 = -0.015831484;
			}
			else {
				var42 = 0.054640573;
			}
		}
	}
	double var43;
	if ((input[12]) >= (9.02345)) {
		if ((input[54]) >= (40.5)) {
			if ((input[61]) >= (30481.0)) {
				var43 = -0.04344009;
			}
			else {
				var43 = 0.011326189;
			}
		}
		else {
			if ((input[53]) >= (0.53985)) {
				var43 = -0.035361435;
			}
			else {
				var43 = 0.02059658;
			}
		}
	}
	else {
		if ((input[56]) >= (11.5)) {
			if ((input[47]) >= (1.1001999)) {
				var43 = 0.014870728;
			}
			else {
				var43 = -0.021342272;
			}
		}
		else {
			if ((input[60]) >= (1.5)) {
				var43 = 0.033599656;
			}
			else {
				var43 = 0.09794467;
			}
		}
	}
	double var44;
	if ((input[54]) >= (43.5)) {
		if ((input[61]) >= (20075.0)) {
			if ((input[61]) >= (44702.5)) {
				var44 = -0.04204583;
			}
			else {
				var44 = 0.0170013;
			}
		}
		else {
			if ((input[41]) >= (2.5)) {
				var44 = 0.08512401;
			}
			else {
				var44 = 0.039533474;
			}
		}
	}
	else {
		if ((input[61]) >= (5349.5)) {
			if ((input[47]) >= (2.90435)) {
				var44 = -0.0064292075;
			}
			else {
				var44 = -0.041639104;
			}
		}
		else {
			if ((input[48]) >= (705.1387)) {
				var44 = 0.03122519;
			}
			else {
				var44 = -0.0047858707;
			}
		}
	}
	double var45;
	if ((input[17]) >= (1.5)) {
		if ((input[61]) >= (289.5)) {
			if ((input[48]) >= (10.4492)) {
				var45 = 0.011849888;
			}
			else {
				var45 = -0.07612083;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var45 = -0.00633089;
			}
			else {
				var45 = 0.061749704;
			}
		}
	}
	else {
		if ((input[10]) >= (0.5)) {
			if ((input[56]) >= (298.5)) {
				var45 = -0.0005569537;
			}
			else {
				var45 = 0.0374281;
			}
		}
		else {
			if ((input[39]) >= (6.58335)) {
				var45 = -0.031191874;
			}
			else {
				var45 = -0.005635024;
			}
		}
	}
	double var46;
	if ((input[12]) >= (3.7422)) {
		if ((input[48]) >= (730.06055)) {
			if ((input[61]) >= (5784.5)) {
				var46 = -0.0107243;
			}
			else {
				var46 = 0.025999714;
			}
		}
		else {
			if ((input[61]) >= (1253.5)) {
				var46 = -0.042973906;
			}
			else {
				var46 = -0.013713528;
			}
		}
	}
	else {
		if ((input[56]) >= (11.5)) {
			if ((input[54]) >= (35.5)) {
				var46 = 0.017567366;
			}
			else {
				var46 = -0.019357955;
			}
		}
		else {
			if ((input[60]) >= (1.5)) {
				var46 = 0.022636306;
			}
			else {
				var46 = 0.09742139;
			}
		}
	}
	double var47;
	if ((input[53]) >= (0.98535)) {
		if ((input[61]) >= (1670.5)) {
			if ((input[54]) >= (41.5)) {
				var47 = -0.001374738;
			}
			else {
				var47 = -0.043432906;
			}
		}
		else {
			if ((input[54]) >= (37.5)) {
				var47 = 0.028679723;
			}
			else {
				var47 = -0.014525557;
			}
		}
	}
	else {
		if ((input[61]) >= (10565.5)) {
			if ((input[54]) >= (40.5)) {
				var47 = 0.0060625845;
			}
			else {
				var47 = -0.048987452;
			}
		}
		else {
			if ((input[46]) >= (129.614)) {
				var47 = 0.039007608;
			}
			else {
				var47 = 0.0040082703;
			}
		}
	}
	double var48;
	if ((input[1]) >= (10.4766)) {
		if ((input[47]) >= (2.00735)) {
			if ((input[54]) >= (43.5)) {
				var48 = 0.033617254;
			}
			else {
				var48 = -0.009015694;
			}
		}
		else {
			if ((input[56]) >= (61.5)) {
				var48 = -0.042808924;
			}
			else {
				var48 = 0.030767078;
			}
		}
	}
	else {
		if ((input[43]) >= (1.5)) {
			if ((input[47]) >= (0.81215)) {
				var48 = -0.006854493;
			}
			else {
				var48 = -0.067230605;
			}
		}
		else {
			if ((input[61]) >= (41.5)) {
				var48 = 0.00818327;
			}
			else {
				var48 = 0.05566327;
			}
		}
	}
	double var49;
	if ((input[54]) >= (40.5)) {
		if ((input[61]) >= (10212.5)) {
			if ((input[61]) >= (30107.5)) {
				var49 = -0.036285497;
			}
			else {
				var49 = 0.002925692;
			}
		}
		else {
			if ((input[60]) >= (1.5)) {
				var49 = 0.013381818;
			}
			else {
				var49 = 0.051408555;
			}
		}
	}
	else {
		if ((input[61]) >= (2002.5)) {
			if ((input[47]) >= (2.69915)) {
				var49 = -0.0074971425;
			}
			else {
				var49 = -0.044626568;
			}
		}
		else {
			if ((input[47]) >= (1.89445)) {
				var49 = 0.027077395;
			}
			else {
				var49 = -0.011322479;
			}
		}
	}
	double var50;
	if ((input[0]) >= (6.0)) {
		if ((input[1]) >= (0.0703)) {
			if ((input[46]) >= (1.0531)) {
				var50 = -0.020533668;
			}
			else {
				var50 = -0.07164039;
			}
		}
		else {
			if ((input[43]) >= (0.5)) {
				var50 = -0.017439937;
			}
			else {
				var50 = 0.062760025;
			}
		}
	}
	else {
		if ((input[56]) >= (263.5)) {
			if ((input[48]) >= (14.20115)) {
				var50 = 0.003499883;
			}
			else {
				var50 = -0.050777387;
			}
		}
		else {
			if ((input[43]) >= (0.5)) {
				var50 = 0.01465892;
			}
			else {
				var50 = 0.07317357;
			}
		}
	}
	double var51;
	if ((input[24]) >= (8.0)) {
		if ((input[39]) >= (0.125)) {
			if ((input[4]) >= (9.5)) {
				var51 = -0.05940796;
			}
			else {
				var51 = -0.030055061;
			}
		}
		else {
			if ((input[61]) >= (735.5)) {
				var51 = -0.011147966;
			}
			else {
				var51 = 0.042494863;
			}
		}
	}
	else {
		if ((input[56]) >= (109.5)) {
			if ((input[48]) >= (9.185551)) {
				var51 = 0.0044458914;
			}
			else {
				var51 = -0.040934246;
			}
		}
		else {
			if ((input[43]) >= (0.5)) {
				var51 = 0.003685267;
			}
			else {
				var51 = 0.06765999;
			}
		}
	}
	double var52;
	if ((input[54]) >= (43.5)) {
		if ((input[61]) >= (38633.0)) {
			if ((input[31]) >= (1.5)) {
				var52 = -0.040427994;
			}
			else {
				var52 = 0.008971758;
			}
		}
		else {
			if ((input[56]) >= (3005.0)) {
				var52 = 0.035012655;
			}
			else {
				var52 = 0.08542098;
			}
		}
	}
	else {
		if ((input[61]) >= (7912.5)) {
			if ((input[54]) >= (40.5)) {
				var52 = -0.014986879;
			}
			else {
				var52 = -0.04559261;
			}
		}
		else {
			if ((input[47]) >= (2.78765)) {
				var52 = 0.03226558;
			}
			else {
				var52 = -0.0034454581;
			}
		}
	}
	double var53;
	if ((input[12]) >= (13.6016)) {
		if ((input[46]) >= (1227.6785)) {
			if ((input[60]) >= (103.5)) {
				var53 = -0.052637886;
			}
			else {
				var53 = 0.009412355;
			}
		}
		else {
			if ((input[54]) >= (43.5)) {
				var53 = 0.008212427;
			}
			else {
				var53 = -0.032765422;
			}
		}
	}
	else {
		if ((input[54]) >= (37.5)) {
			if ((input[61]) >= (1203.5)) {
				var53 = 0.0038239032;
			}
			else {
				var53 = 0.0402253;
			}
		}
		else {
			if ((input[61]) >= (142.5)) {
				var53 = -0.01854813;
			}
			else {
				var53 = 0.012720219;
			}
		}
	}
	double var54;
	if ((input[53]) >= (0.97495)) {
		if ((input[61]) >= (595.5)) {
			if ((input[54]) >= (41.5)) {
				var54 = 0.005084223;
			}
			else {
				var54 = -0.029742667;
			}
		}
		else {
			if ((input[43]) >= (0.5)) {
				var54 = -0.014112115;
			}
			else {
				var54 = 0.034655306;
			}
		}
	}
	else {
		if ((input[61]) >= (18264.0)) {
			if ((input[54]) >= (40.5)) {
				var54 = -0.004187572;
			}
			else {
				var54 = -0.05576644;
			}
		}
		else {
			if ((input[46]) >= (179.9252)) {
				var54 = 0.031123156;
			}
			else {
				var54 = 0.002183959;
			}
		}
	}
	double var55;
	if ((input[0]) >= (6.0)) {
		if ((input[56]) >= (9.5)) {
			if ((input[51]) >= (0.00315)) {
				var55 = -0.01755209;
			}
			else {
				var55 = -0.0882936;
			}
		}
		else {
			if ((input[56]) >= (7.5)) {
				var55 = 0.037568834;
			}
			else {
				var55 = 0.09457844;
			}
		}
	}
	else {
		if ((input[56]) >= (59.5)) {
			if ((input[47]) >= (0.66845)) {
				var55 = 0.0046583116;
			}
			else {
				var55 = -0.046081893;
			}
		}
		else {
			if ((input[44]) >= (0.5)) {
				var55 = 0.08885377;
			}
			else {
				var55 = 0.0050142673;
			}
		}
	}
	double var56;
	if ((input[1]) >= (3.14845)) {
		if ((input[48]) >= (729.9746)) {
			if ((input[56]) >= (2956.5)) {
				var56 = -0.0017212833;
			}
			else {
				var56 = 0.042738426;
			}
		}
		else {
			if ((input[61]) >= (704.5)) {
				var56 = -0.03499685;
			}
			else {
				var56 = -0.0083664665;
			}
		}
	}
	else {
		if ((input[6]) >= (3.5)) {
			if ((input[54]) >= (35.5)) {
				var56 = 0.05070079;
			}
			else {
				var56 = -0.010859969;
			}
		}
		else {
			if ((input[61]) >= (4427.5)) {
				var56 = -0.015685324;
			}
			else {
				var56 = 0.012172584;
			}
		}
	}
	double var57;
	if ((input[54]) >= (37.5)) {
		if ((input[56]) >= (298.5)) {
			if ((input[48]) >= (14.873051)) {
				var57 = 0.0068608015;
			}
			else {
				var57 = -0.044197094;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var57 = -0.00012376814;
			}
			else {
				var57 = 0.08143332;
			}
		}
	}
	else {
		if ((input[61]) >= (350.5)) {
			if ((input[32]) >= (4.5)) {
				var57 = 0.031026168;
			}
			else {
				var57 = -0.03353453;
			}
		}
		else {
			if ((input[46]) >= (13.613001)) {
				var57 = 0.038915735;
			}
			else {
				var57 = -0.014686892;
			}
		}
	}
	double var58;
	if ((input[53]) >= (0.98945)) {
		if ((input[61]) >= (1827.5)) {
			if ((input[54]) >= (43.5)) {
				var58 = 0.018584859;
			}
			else {
				var58 = -0.02788482;
			}
		}
		else {
			if ((input[54]) >= (37.5)) {
				var58 = 0.02208819;
			}
			else {
				var58 = -0.012729446;
			}
		}
	}
	else {
		if ((input[56]) >= (6386.0)) {
			if ((input[54]) >= (40.5)) {
				var58 = 0.010000155;
			}
			else {
				var58 = -0.028903186;
			}
		}
		else {
			if ((input[46]) >= (429.62476)) {
				var58 = 0.046903815;
			}
			else {
				var58 = 0.00868652;
			}
		}
	}
	double var59;
	if ((input[12]) >= (21.242199)) {
		if ((input[54]) >= (43.5)) {
			if ((input[39]) >= (0.125)) {
				var59 = 0.00013053848;
			}
			else {
				var59 = 0.049104486;
			}
		}
		else {
			if ((input[60]) >= (16.5)) {
				var59 = -0.04671247;
			}
			else {
				var59 = -0.01771003;
			}
		}
	}
	else {
		if ((input[47]) >= (1.5522499)) {
			if ((input[61]) >= (3245.5)) {
				var59 = -0.0040672934;
			}
			else {
				var59 = 0.023876032;
			}
		}
		else {
			if ((input[61]) >= (20.5)) {
				var59 = -0.023340002;
			}
			else {
				var59 = 0.027407808;
			}
		}
	}
	double var60;
	if ((input[47]) >= (2.61685)) {
		if ((input[60]) >= (64.5)) {
			if ((input[61]) >= (2158.5)) {
				var60 = -0.05265349;
			}
			else {
				var60 = -0.00054759363;
			}
		}
		else {
			if ((input[61]) >= (3961.5)) {
				var60 = 0.006459606;
			}
			else {
				var60 = 0.036336076;
			}
		}
	}
	else {
		if ((input[61]) >= (261.5)) {
			if ((input[48]) >= (8.9492)) {
				var60 = -0.010471974;
			}
			else {
				var60 = -0.07024484;
			}
		}
		else {
			if ((input[43]) >= (0.5)) {
				var60 = -0.008017136;
			}
			else {
				var60 = 0.037699617;
			}
		}
	}
	double var61;
	if ((input[0]) >= (6.0)) {
		if ((input[56]) >= (14.5)) {
			if ((input[46]) >= (0.8456)) {
				var61 = -0.013972414;
			}
			else {
				var61 = -0.06858781;
			}
		}
		else {
			if ((input[56]) >= (7.5)) {
				var61 = 0.02761577;
			}
			else {
				var61 = 0.09199306;
			}
		}
	}
	else {
		if ((input[56]) >= (59.5)) {
			if ((input[54]) >= (42.5)) {
				var61 = 0.02275305;
			}
			else {
				var61 = -0.0028968595;
			}
		}
		else {
			if ((input[60]) >= (1.5)) {
				var61 = 0.031734016;
			}
			else {
				var61 = 0.09349312;
			}
		}
	}
	double var62;
	if ((input[61]) >= (18260.5)) {
		if ((input[54]) >= (43.5)) {
			if ((input[61]) >= (44702.5)) {
				var62 = -0.037644908;
			}
			else {
				var62 = 0.013576572;
			}
		}
		else {
			if ((input[47]) >= (2.30835)) {
				var62 = -0.030874332;
			}
			else {
				var62 = -0.067634635;
			}
		}
	}
	else {
		if ((input[54]) >= (41.5)) {
			if ((input[56]) >= (1863.5)) {
				var62 = 0.014616805;
			}
			else {
				var62 = 0.05351311;
			}
		}
		else {
			if ((input[53]) >= (0.85125)) {
				var62 = -0.008688419;
			}
			else {
				var62 = 0.018850587;
			}
		}
	}
	double var63;
	if ((input[1]) >= (16.5391)) {
		if ((input[54]) >= (43.5)) {
			if ((input[61]) >= (37324.0)) {
				var63 = -0.021373888;
			}
			else {
				var63 = 0.023376394;
			}
		}
		else {
			if ((input[47]) >= (2.6178498)) {
				var63 = -0.0049431804;
			}
			else {
				var63 = -0.030292137;
			}
		}
	}
	else {
		if ((input[48]) >= (705.1387)) {
			if ((input[61]) >= (1978.5)) {
				var63 = 0.006892805;
			}
			else {
				var63 = 0.054744195;
			}
		}
		else {
			if ((input[61]) >= (704.5)) {
				var63 = -0.018579861;
			}
			else {
				var63 = 0.009578034;
			}
		}
	}
	double var64;
	if ((input[43]) >= (1.5)) {
		if ((input[51]) >= (0.00315)) {
			if ((input[46]) >= (613.72217)) {
				var64 = 0.0030481922;
			}
			else {
				var64 = -0.021368962;
			}
		}
		else {
			if ((input[61]) >= (4.5)) {
				var64 = -0.11970482;
			}
			else {
				var64 = -0.03543761;
			}
		}
	}
	else {
		if ((input[61]) >= (1732.5)) {
			if ((input[48]) >= (1391.0)) {
				var64 = 0.007906114;
			}
			else {
				var64 = -0.019812634;
			}
		}
		else {
			if ((input[48]) >= (692.52344)) {
				var64 = 0.05048374;
			}
			else {
				var64 = 0.009864658;
			}
		}
	}
	double var65;
	if ((input[12]) >= (14.8828)) {
		if ((input[47]) >= (2.60145)) {
			if ((input[60]) >= (49.5)) {
				var65 = -0.039194312;
			}
			else {
				var65 = 0.0073285825;
			}
		}
		else {
			if ((input[53]) >= (0.32755)) {
				var65 = -0.025707064;
			}
			else {
				var65 = 0.058588088;
			}
		}
	}
	else {
		if ((input[47]) >= (0.96545)) {
			if ((input[61]) >= (93.5)) {
				var65 = 0.004989247;
			}
			else {
				var65 = 0.04186198;
			}
		}
		else {
			if ((input[61]) >= (13.5)) {
				var65 = -0.036324676;
			}
			else {
				var65 = 0.02014062;
			}
		}
	}
	double var66;
	if ((input[0]) >= (6.0)) {
		if ((input[1]) >= (0.02345)) {
			if ((input[48]) >= (1.36525)) {
				var66 = -0.0152100995;
			}
			else {
				var66 = -0.056148853;
			}
		}
		else {
			if ((input[4]) >= (9.5)) {
				var66 = 0.13695166;
			}
			else {
				var66 = 0.012355594;
			}
		}
	}
	else {
		if ((input[56]) >= (460.5)) {
			if ((input[48]) >= (64.63865)) {
				var66 = 0.003372301;
			}
			else {
				var66 = -0.035323985;
			}
		}
		else {
			if ((input[54]) >= (37.5)) {
				var66 = 0.061643846;
			}
			else {
				var66 = 0.01483185;
			}
		}
	}
	double var67;
	if ((input[54]) >= (40.5)) {
		if ((input[61]) >= (10212.5)) {
			if ((input[61]) >= (30107.5)) {
				var67 = -0.031961888;
			}
			else {
				var67 = 0.00088883;
			}
		}
		else {
			if ((input[1]) >= (21.898449)) {
				var67 = -0.0056519257;
			}
			else {
				var67 = 0.03151971;
			}
		}
	}
	else {
		if ((input[61]) >= (936.5)) {
			if ((input[47]) >= (2.00005)) {
				var67 = -0.0065098666;
			}
			else {
				var67 = -0.0454471;
			}
		}
		else {
			if ((input[48]) >= (32.74805)) {
				var67 = 0.026224226;
			}
			else {
				var67 = -0.010604701;
			}
		}
	}
	double var68;
	if ((input[53]) >= (0.68625)) {
		if ((input[1]) >= (2.71095)) {
			if ((input[48]) >= (786.74805)) {
				var68 = 0.002196686;
			}
			else {
				var68 = -0.020058645;
			}
		}
		else {
			if ((input[56]) >= (7.5)) {
				var68 = 0.008202765;
			}
			else {
				var68 = 0.08527691;
			}
		}
	}
	else {
		if ((input[56]) >= (6539.5)) {
			if ((input[60]) >= (115.5)) {
				var68 = -0.05778717;
			}
			else {
				var68 = 0.007862157;
			}
		}
		else {
			if ((input[48]) >= (204.56055)) {
				var68 = 0.05347788;
			}
			else {
				var68 = -0.0014044967;
			}
		}
	}
	double var69;
	if ((input[47]) >= (2.90315)) {
		if ((input[60]) >= (78.5)) {
			if ((input[16]) >= (10.55)) {
				var69 = -0.06294976;
			}
			else {
				var69 = -0.023675375;
			}
		}
		else {
			if ((input[61]) >= (4428.5)) {
				var69 = 0.008402822;
			}
			else {
				var69 = 0.04112808;
			}
		}
	}
	else {
		if ((input[61]) >= (2768.5)) {
			if ((input[54]) >= (43.5)) {
				var69 = 0.0136197405;
			}
			else {
				var69 = -0.023780834;
			}
		}
		else {
			if ((input[48]) >= (1257.2109)) {
				var69 = 0.03998407;
			}
			else {
				var69 = -0.0006609166;
			}
		}
	}
	double var70;
	if ((input[54]) >= (37.5)) {
		if ((input[56]) >= (875.5)) {
			if ((input[48]) >= (201.07419)) {
				var70 = 0.005744586;
			}
			else {
				var70 = -0.02594339;
			}
		}
		else {
			if ((input[60]) >= (1.5)) {
				var70 = 0.007729176;
			}
			else {
				var70 = 0.06468779;
			}
		}
	}
	else {
		if ((input[61]) >= (93.5)) {
			if ((input[46]) >= (0.4839)) {
				var70 = -0.017004458;
			}
			else {
				var70 = -0.14380918;
			}
		}
		else {
			if ((input[51]) >= (0.00084999995)) {
				var70 = 0.023790171;
			}
			else {
				var70 = -0.04816125;
			}
		}
	}
	double var71;
	if ((input[61]) >= (21716.5)) {
		if ((input[54]) >= (43.5)) {
			if ((input[61]) >= (44702.5)) {
				var71 = -0.032613255;
			}
			else {
				var71 = 0.0066647492;
			}
		}
		else {
			if ((input[47]) >= (2.51645)) {
				var71 = -0.028361056;
			}
			else {
				var71 = -0.060256895;
			}
		}
	}
	else {
		if ((input[54]) >= (42.5)) {
			if ((input[0]) >= (6.0)) {
				var71 = -0.0036986673;
			}
			else {
				var71 = 0.032124206;
			}
		}
		else {
			if ((input[53]) >= (0.71564996)) {
				var71 = -0.0055871406;
			}
			else {
				var71 = 0.020379527;
			}
		}
	}
	double var72;
	if ((input[12]) >= (21.242199)) {
		if ((input[45]) >= (145.64844)) {
			if ((input[56]) >= (358.5)) {
				var72 = -0.057326652;
			}
			else {
				var72 = 0.020187235;
			}
		}
		else {
			if ((input[60]) >= (16.5)) {
				var72 = -0.034736983;
			}
			else {
				var72 = -0.005934634;
			}
		}
	}
	else {
		if ((input[47]) >= (0.15834999)) {
			if ((input[61]) >= (45.5)) {
				var72 = 0.0024177753;
			}
			else {
				var72 = 0.033983085;
			}
		}
		else {
			if ((input[61]) >= (13.5)) {
				var72 = -0.10862023;
			}
			else {
				var72 = 0.0021476517;
			}
		}
	}
	double var73;
	if ((input[0]) >= (6.0)) {
		if ((input[56]) >= (14.5)) {
			if ((input[46]) >= (4.9613)) {
				var73 = -0.007898534;
			}
			else {
				var73 = -0.042810947;
			}
		}
		else {
			if ((input[60]) >= (1.5)) {
				var73 = 0.018479025;
			}
			else {
				var73 = 0.0800434;
			}
		}
	}
	else {
		if ((input[56]) >= (59.5)) {
			if ((input[51]) >= (0.00135)) {
				var73 = 0.0029930752;
			}
			else {
				var73 = -0.07581385;
			}
		}
		else {
			if ((input[60]) >= (1.5)) {
				var73 = 0.025273873;
			}
			else {
				var73 = 0.0910637;
			}
		}
	}
	double var74;
	if ((input[43]) >= (1.5)) {
		if ((input[46]) >= (0.0526)) {
			if ((input[46]) >= (1042.3885)) {
				var74 = 0.005416441;
			}
			else {
				var74 = -0.017602937;
			}
		}
		else {
			if ((input[61]) >= (3.5)) {
				var74 = -0.10874515;
			}
			else {
				var74 = -0.029635584;
			}
		}
	}
	else {
		if ((input[61]) >= (3239.5)) {
			if ((input[54]) >= (43.5)) {
				var74 = 0.020170307;
			}
			else {
				var74 = -0.013162871;
			}
		}
		else {
			if ((input[54]) >= (40.5)) {
				var74 = 0.04116634;
			}
			else {
				var74 = 0.00575624;
			}
		}
	}
	double var75;
	if ((input[47]) >= (1.80825)) {
		if ((input[61]) >= (2144.5)) {
			if ((input[54]) >= (37.5)) {
				var75 = 0.0013367898;
			}
			else {
				var75 = -0.02980991;
			}
		}
		else {
			if ((input[48]) >= (439.99023)) {
				var75 = 0.036782894;
			}
			else {
				var75 = 0.0110868625;
			}
		}
	}
	else {
		if ((input[61]) >= (43.5)) {
			if ((input[46]) >= (0.14144999)) {
				var75 = -0.015385406;
			}
			else {
				var75 = -0.104217306;
			}
		}
		else {
			if ((input[60]) >= (1.5)) {
				var75 = -0.0027545884;
			}
			else {
				var75 = 0.06437027;
			}
		}
	}
	double var76;
	if ((input[24]) >= (8.0)) {
		if ((input[4]) >= (9.5)) {
			if ((input[61]) >= (290.5)) {
				var76 = -0.05347655;
			}
			else {
				var76 = 0.0026797268;
			}
		}
		else {
			if ((input[38]) >= (2.875)) {
				var76 = 0.0071457075;
			}
			else {
				var76 = -0.024164375;
			}
		}
	}
	else {
		if ((input[61]) >= (29862.5)) {
			if ((input[55]) >= (40.5)) {
				var76 = -0.014128357;
			}
			else {
				var76 = -0.04695853;
			}
		}
		else {
			if ((input[54]) >= (42.5)) {
				var76 = 0.021603541;
			}
			else {
				var76 = -0.00009904821;
			}
		}
	}
	double var77;
	if ((input[12]) >= (6.6640997)) {
		if ((input[48]) >= (1336.1055)) {
			if ((input[56]) >= (4062.5)) {
				var77 = -0.0010225602;
			}
			else {
				var77 = 0.0328553;
			}
		}
		else {
			if ((input[61]) >= (559.5)) {
				var77 = -0.025073683;
			}
			else {
				var77 = -0.0027194947;
			}
		}
	}
	else {
		if ((input[52]) >= (13.5809)) {
			if ((input[61]) >= (306.5)) {
				var77 = 0.0063920603;
			}
			else {
				var77 = 0.048350316;
			}
		}
		else {
			if ((input[61]) >= (88.5)) {
				var77 = -0.032507706;
			}
			else {
				var77 = 0.011420465;
			}
		}
	}
	double var78;
	if ((input[47]) >= (2.95295)) {
		if ((input[56]) >= (6789.5)) {
			if ((input[54]) >= (37.5)) {
				var78 = 0.0074238777;
			}
			else {
				var78 = -0.041485284;
			}
		}
		else {
			if ((input[60]) >= (60.5)) {
				var78 = -0.01975377;
			}
			else {
				var78 = 0.033806864;
			}
		}
	}
	else {
		if ((input[61]) >= (229.5)) {
			if ((input[48]) >= (14.20115)) {
				var78 = -0.004753351;
			}
			else {
				var78 = -0.04816822;
			}
		}
		else {
			if ((input[46]) >= (29.323)) {
				var78 = 0.050495632;
			}
			else {
				var78 = 0.0013755038;
			}
		}
	}
	double var79;
	if ((input[53]) >= (0.85125)) {
		if ((input[1]) >= (0.8672)) {
			if ((input[54]) >= (36.5)) {
				var79 = -0.003695337;
			}
			else {
				var79 = -0.026268313;
			}
		}
		else {
			if ((input[40]) >= (3.5)) {
				var79 = 0.041164845;
			}
			else {
				var79 = 0.0090211155;
			}
		}
	}
	else {
		if ((input[61]) >= (9114.0)) {
			if ((input[61]) >= (42615.5)) {
				var79 = -0.044437833;
			}
			else {
				var79 = 0.0013217455;
			}
		}
		else {
			if ((input[48]) >= (85.71875)) {
				var79 = 0.031338584;
			}
			else {
				var79 = -0.0018825595;
			}
		}
	}
	double var80;
	if ((input[0]) >= (6.0)) {
		if ((input[56]) >= (9.5)) {
			if ((input[48]) >= (1.0429499)) {
				var80 = -0.009993525;
			}
			else {
				var80 = -0.046417933;
			}
		}
		else {
			if ((input[60]) >= (1.5)) {
				var80 = 0.028406987;
			}
			else {
				var80 = 0.08684213;
			}
		}
	}
	else {
		if ((input[56]) >= (603.5)) {
			if ((input[48]) >= (440.08398)) {
				var80 = 0.0050671487;
			}
			else {
				var80 = -0.017472088;
			}
		}
		else {
			if ((input[54]) >= (37.5)) {
				var80 = 0.047755934;
			}
			else {
				var80 = 0.011390212;
			}
		}
	}
	double var81;
	if ((input[1]) >= (8.2422)) {
		if ((input[46]) >= (709.3192)) {
			if ((input[61]) >= (3939.5)) {
				var81 = -0.005410834;
			}
			else {
				var81 = 0.020830376;
			}
		}
		else {
			if ((input[60]) >= (18.5)) {
				var81 = -0.047823805;
			}
			else {
				var81 = -0.014728735;
			}
		}
	}
	else {
		if ((input[47]) >= (0.81215)) {
			if ((input[61]) >= (4174.5)) {
				var81 = -0.006534904;
			}
			else {
				var81 = 0.014676656;
			}
		}
		else {
			if ((input[56]) >= (7.5)) {
				var81 = -0.021621697;
			}
			else {
				var81 = 0.09130949;
			}
		}
	}
	double var82;
	if ((input[54]) >= (44.5)) {
		if ((input[61]) >= (20075.0)) {
			if ((input[61]) >= (43771.5)) {
				var82 = -0.031774346;
			}
			else {
				var82 = 0.010180778;
			}
		}
		else {
			if ((input[60]) >= (1.5)) {
				var82 = 0.013382459;
			}
			else {
				var82 = 0.0640025;
			}
		}
	}
	else {
		if ((input[61]) >= (12273.5)) {
			if ((input[53]) >= (0.97925)) {
				var82 = -0.037643213;
			}
			else {
				var82 = -0.010598852;
			}
		}
		else {
			if ((input[47]) >= (2.0704498)) {
				var82 = 0.009237956;
			}
			else {
				var82 = -0.006932293;
			}
		}
	}
	double var83;
	if ((input[12]) >= (0.5078)) {
		if ((input[48]) >= (65.06055)) {
			if ((input[61]) >= (560.5)) {
				var83 = -0.0015426185;
			}
			else {
				var83 = 0.032927435;
			}
		}
		else {
			if ((input[56]) >= (121.5)) {
				var83 = -0.026604628;
			}
			else {
				var83 = 0.008728904;
			}
		}
	}
	else {
		if ((input[6]) >= (3.5)) {
			if ((input[23]) >= (0.9)) {
				var83 = 0.026545305;
			}
			else {
				var83 = 0.08305548;
			}
		}
		else {
			if ((input[47]) >= (0.4154)) {
				var83 = 0.017793326;
			}
			else {
				var83 = -0.018235527;
			}
		}
	}
	double var84;
	if ((input[54]) >= (40.5)) {
		if ((input[56]) >= (1863.5)) {
			if ((input[46]) >= (183.4375)) {
				var84 = 0.0084951185;
			}
			else {
				var84 = -0.023850499;
			}
		}
		else {
			if ((input[60]) >= (1.5)) {
				var84 = -0.0007357471;
			}
			else {
				var84 = 0.079094104;
			}
		}
	}
	else {
		if ((input[61]) >= (151.5)) {
			if ((input[48]) >= (14.20115)) {
				var84 = -0.006130491;
			}
			else {
				var84 = -0.04856849;
			}
		}
		else {
			if ((input[52]) >= (24.3202)) {
				var84 = 0.052204933;
			}
			else {
				var84 = 0.0022712636;
			}
		}
	}
	double var85;
	if ((input[60]) >= (97.5)) {
		if ((input[53]) >= (0.1448)) {
			if ((input[60]) >= (135.5)) {
				var85 = -0.059724163;
			}
			else {
				var85 = -0.028850341;
			}
		}
		else {
			if ((input[12]) >= (30.835949)) {
				var85 = 0.26568264;
			}
			else {
				var85 = -0.004891755;
			}
		}
	}
	else {
		if ((input[53]) >= (0.92904997)) {
			if ((input[61]) >= (1148.5)) {
				var85 = -0.0108981235;
			}
			else {
				var85 = 0.005024958;
			}
		}
		else {
			if ((input[36]) >= (4.64555)) {
				var85 = 0.017253196;
			}
			else {
				var85 = -0.007548522;
			}
		}
	}
	double var86;
	if ((input[43]) >= (1.5)) {
		if ((input[46]) >= (0.0526)) {
			if ((input[36]) >= (5.05345)) {
				var86 = 0.0062018516;
			}
			else {
				var86 = -0.015987573;
			}
		}
		else {
			if ((input[61]) >= (4.5)) {
				var86 = -0.097434126;
			}
			else {
				var86 = -0.027043892;
			}
		}
	}
	else {
		if ((input[61]) >= (1540.5)) {
			if ((input[48]) >= (1391.0)) {
				var86 = 0.0069710836;
			}
			else {
				var86 = -0.014068996;
			}
		}
		else {
			if ((input[54]) >= (37.5)) {
				var86 = 0.026903376;
			}
			else {
				var86 = 0.0013505955;
			}
		}
	}
	double var87;
	if ((input[12]) >= (24.757801)) {
		if ((input[56]) >= (87.5)) {
			if ((input[36]) >= (4.6883)) {
				var87 = -0.0085901525;
			}
			else {
				var87 = -0.028790807;
			}
		}
		else {
			if ((input[60]) >= (15.5)) {
				var87 = -0.07380319;
			}
			else {
				var87 = 0.0480573;
			}
		}
	}
	else {
		if ((input[54]) >= (40.5)) {
			if ((input[61]) >= (6238.5)) {
				var87 = 0.0012750438;
			}
			else {
				var87 = 0.025394326;
			}
		}
		else {
			if ((input[14]) >= (2.9)) {
				var87 = 0.0069677853;
			}
			else {
				var87 = -0.01208449;
			}
		}
	}
	double var88;
	if ((input[56]) >= (27.5)) {
		if ((input[46]) >= (0.087400004)) {
			if ((input[0]) >= (6.0)) {
				var88 = -0.013403784;
			}
			else {
				var88 = 0.0030857895;
			}
		}
		else {
			if ((input[45]) >= (77.015656)) {
				var88 = -0.08984407;
			}
			else {
				var88 = -0.022600971;
			}
		}
	}
	else {
		if ((input[60]) >= (1.5)) {
			if ((input[45]) >= (55.164047)) {
				var88 = -0.01865795;
			}
			else {
				var88 = 0.044782363;
			}
		}
		else {
			if ((input[56]) >= (-43.0)) {
				var88 = 0.07249187;
			}
			else {
				var88 = -0.36457744;
			}
		}
	}
	double var89;
	if ((input[56]) >= (109.5)) {
		if ((input[48]) >= (32.82615)) {
			if ((input[61]) >= (421.5)) {
				var89 = -0.000829708;
			}
			else {
				var89 = 0.030783473;
			}
		}
		else {
			if ((input[1]) >= (3.33595)) {
				var89 = -0.035862286;
			}
			else {
				var89 = -0.010864266;
			}
		}
	}
	else {
		if ((input[54]) >= (35.5)) {
			if ((input[60]) >= (1.5)) {
				var89 = 0.013808275;
			}
			else {
				var89 = 0.07297513;
			}
		}
		else {
			if ((input[51]) >= (0.022550002)) {
				var89 = 0.02736353;
			}
			else {
				var89 = -0.023011377;
			}
		}
	}
	double var90;
	if ((input[53]) >= (0.41365)) {
		if ((input[12]) >= (4.0078)) {
			if ((input[48]) >= (1019.3672)) {
				var90 = 0.002891426;
			}
			else {
				var90 = -0.014771153;
			}
		}
		else {
			if ((input[61]) >= (9114.0)) {
				var90 = -0.018954484;
			}
			else {
				var90 = 0.009538494;
			}
		}
	}
	else {
		if ((input[61]) >= (4306.0)) {
			if ((input[46]) >= (2326.0894)) {
				var90 = 0.03181304;
			}
			else {
				var90 = -0.0022767028;
			}
		}
		else {
			if ((input[48]) >= (218.97461)) {
				var90 = 0.065139785;
			}
			else {
				var90 = 0.010299314;
			}
		}
	}
	double var91;
	if ((input[54]) >= (44.5)) {
		if ((input[61]) >= (16876.5)) {
			if ((input[61]) >= (41550.5)) {
				var91 = -0.027617956;
			}
			else {
				var91 = 0.011716015;
			}
		}
		else {
			if ((input[60]) >= (0.5)) {
				var91 = 0.029039158;
			}
			else {
				var91 = 0.08928869;
			}
		}
	}
	else {
		if ((input[61]) >= (11574.5)) {
			if ((input[47]) >= (3.09375)) {
				var91 = -0.0011216202;
			}
			else {
				var91 = -0.026885852;
			}
		}
		else {
			if ((input[14]) >= (2.225)) {
				var91 = 0.00478744;
			}
			else {
				var91 = -0.014421779;
			}
		}
	}
	double var92;
	if ((input[60]) >= (68.5)) {
		if ((input[48]) >= (3409.6055)) {
			if ((input[56]) >= (1580.0)) {
				var92 = -0.005839202;
			}
			else {
				var92 = 0.2195361;
			}
		}
		else {
			if ((input[36]) >= (5.13845)) {
				var92 = -0.021588324;
			}
			else {
				var92 = -0.05186486;
			}
		}
	}
	else {
		if ((input[47]) >= (2.46705)) {
			if ((input[61]) >= (1749.5)) {
				var92 = 0.004714631;
			}
			else {
				var92 = 0.030014848;
			}
		}
		else {
			if ((input[61]) >= (261.5)) {
				var92 = -0.010439439;
			}
			else {
				var92 = 0.008390851;
			}
		}
	}
	double var93;
	if ((input[54]) >= (37.5)) {
		if ((input[56]) >= (298.5)) {
			if ((input[46]) >= (5.6547003)) {
				var93 = 0.003990318;
			}
			else {
				var93 = -0.038198836;
			}
		}
		else {
			if ((input[60]) >= (1.5)) {
				var93 = 0.012087645;
			}
			else {
				var93 = 0.07309189;
			}
		}
	}
	else {
		if ((input[61]) >= (175.5)) {
			if ((input[47]) >= (1.95175)) {
				var93 = -0.006160046;
			}
			else {
				var93 = -0.031970453;
			}
		}
		else {
			if ((input[47]) >= (0.70860004)) {
				var93 = 0.01815108;
			}
			else {
				var93 = -0.026150316;
			}
		}
	}
	double var94;
	if ((input[36]) >= (4.69445)) {
		if ((input[61]) >= (1304.5)) {
			if ((input[48]) >= (113.30855)) {
				var94 = 0.0017607812;
			}
			else {
				var94 = -0.046118375;
			}
		}
		else {
			if ((input[48]) >= (72.61525)) {
				var94 = 0.03210873;
			}
			else {
				var94 = 0.0030753096;
			}
		}
	}
	else {
		if ((input[61]) >= (18.5)) {
			if ((input[52]) >= (8.438351)) {
				var94 = -0.0053527206;
			}
			else {
				var94 = -0.045595143;
			}
		}
		else {
			if ((input[60]) >= (2.5)) {
				var94 = -0.009114004;
			}
			else {
				var94 = 0.06296943;
			}
		}
	}
	double var95;
	if ((input[54]) >= (40.5)) {
		if ((input[61]) >= (5438.5)) {
			if ((input[46]) >= (195.69876)) {
				var95 = 0.0018606838;
			}
			else {
				var95 = -0.040811684;
			}
		}
		else {
			if ((input[60]) >= (1.5)) {
				var95 = 0.0064622634;
			}
			else {
				var95 = 0.04331122;
			}
		}
	}
	else {
		if ((input[61]) >= (808.5)) {
			if ((input[14]) >= (2.775)) {
				var95 = -0.0065318174;
			}
			else {
				var95 = -0.04133398;
			}
		}
		else {
			if ((input[48]) >= (558.2949)) {
				var95 = 0.045936424;
			}
			else {
				var95 = -0.00046230477;
			}
		}
	}
	double var96;
	if ((input[0]) >= (6.0)) {
		if ((input[56]) >= (13.5)) {
			if ((input[48]) >= (1.7246001)) {
				var96 = -0.008638318;
			}
			else {
				var96 = -0.040554356;
			}
		}
		else {
			if ((input[45]) >= (56.84375)) {
				var96 = 0.016675161;
			}
			else {
				var96 = 0.07408542;
			}
		}
	}
	else {
		if ((input[56]) >= (77.5)) {
			if ((input[54]) >= (42.5)) {
				var96 = 0.014990187;
			}
			else {
				var96 = -0.001959605;
			}
		}
		else {
			if ((input[41]) >= (1.5)) {
				var96 = 0.08045725;
			}
			else {
				var96 = 0.014674807;
			}
		}
	}
	double var97;
	if ((input[61]) >= (29862.5)) {
		if ((input[54]) >= (43.5)) {
			if ((input[53]) >= (0.98875)) {
				var97 = -0.029996792;
			}
			else {
				var97 = 0.0036901017;
			}
		}
		else {
			if ((input[47]) >= (2.3162)) {
				var97 = -0.033033065;
			}
			else {
				var97 = -0.06443291;
			}
		}
	}
	else {
		if ((input[54]) >= (42.5)) {
			if ((input[56]) >= (5154.5)) {
				var97 = 0.0067723775;
			}
			else {
				var97 = 0.031347673;
			}
		}
		else {
			if ((input[38]) >= (2.875)) {
				var97 = 0.007378839;
			}
			else {
				var97 = -0.0072063874;
			}
		}
	}
	double var98;
	if ((input[1]) >= (11.8828)) {
		if ((input[46]) >= (1286.2963)) {
			if ((input[56]) >= (2998.0)) {
				var98 = -0.0019608897;
			}
			else {
				var98 = 0.047904253;
			}
		}
		else {
			if ((input[56]) >= (416.5)) {
				var98 = -0.019742958;
			}
			else {
				var98 = 0.008591896;
			}
		}
	}
	else {
		if ((input[47]) >= (1.34025)) {
			if ((input[61]) >= (3253.5)) {
				var98 = -0.00283273;
			}
			else {
				var98 = 0.015093817;
			}
		}
		else {
			if ((input[61]) >= (13.5)) {
				var98 = -0.015989382;
			}
			else {
				var98 = 0.01889823;
			}
		}
	}
	double var99;
	if ((input[47]) >= (3.04985)) {
		if ((input[60]) >= (106.5)) {
			if ((input[61]) >= (696.5)) {
				var99 = -0.05152908;
			}
			else {
				var99 = 0.1718484;
			}
		}
		else {
			if ((input[61]) >= (7742.0)) {
				var99 = 0.0053493767;
			}
			else {
				var99 = 0.026650125;
			}
		}
	}
	else {
		if ((input[61]) >= (2768.5)) {
			if ((input[54]) >= (44.5)) {
				var99 = 0.0130158765;
			}
			else {
				var99 = -0.014481711;
			}
		}
		else {
			if ((input[43]) >= (0.5)) {
				var99 = -0.006369736;
			}
			else {
				var99 = 0.012103517;
			}
		}
	}
	double var100;
	var100 = (1.0) / ((1.0) + (exp((0.0) - ((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((var0)+(var1)) + (var2)) + (var3)) + (var4)) + (var5)) + (var6)) + (var7)) + (var8)) + (var9)) + (var10)) + (var11)) + (var12)) + (var13)) + (var14)) + (var15)) + (var16)) + (var17)) + (var18)) + (var19)) + (var20)) + (var21)) + (var22)) + (var23)) + (var24)) + (var25)) + (var26)) + (var27)) + (var28)) + (var29)) + (var30)) + (var31)) + (var32)) + (var33)) + (var34)) + (var35)) + (var36)) + (var37)) + (var38)) + (var39)) + (var40)) + (var41)) + (var42)) + (var43)) + (var44)) + (var45)) + (var46)) + (var47)) + (var48)) + (var49)) + (var50)) + (var51)) + (var52)) + (var53)) + (var54)) + (var55)) + (var56)) + (var57)) + (var58)) + (var59)) + (var60)) + (var61)) + (var62)) + (var63)) + (var64)) + (var65)) + (var66)) + (var67)) + (var68)) + (var69)) + (var70)) + (var71)) + (var72)) + (var73)) + (var74)) + (var75)) + (var76)) + (var77)) + (var78)) + (var79)) + (var80)) + (var81)) + (var82)) + (var83)) + (var84)) + (var85)) + (var86)) + (var87)) + (var88)) + (var89)) + (var90)) + (var91)) + (var92)) + (var93)) + (var94)) + (var95)) + (var96)) + (var97)) + (var98)) + (var99)))));
	*output = var100 < 0.5 ? 0 : 1;
}

void PUclf5(double * input, double * output) {
	double var0;
	if ((input[70]) >= (1.5)) {
		if ((input[49]) >= (1.875)) {
			if ((input[49]) >= (4.125)) {
				var0 = -0.19358134;
			}
			else {
				var0 = -0.16700214;
			}
		}
		else {
			if ((input[67]) >= (0.5)) {
				var0 = -0.096840516;
			}
			else {
				var0 = -0.16222827;
			}
		}
	}
	else {
		if ((input[49]) >= (0.125)) {
			if ((input[71]) >= (160.5)) {
				var0 = -0.117757015;
			}
			else {
				var0 = 0.06406926;
			}
		}
		else {
			if ((input[33]) >= (0.225)) {
				var0 = 0.035441637;
			}
			else {
				var0 = 0.1842877;
			}
		}
	}
	double var1;
	if ((input[70]) >= (1.5)) {
		if ((input[50]) >= (3.5)) {
			if ((input[70]) >= (4.5)) {
				var1 = -0.113641284;
			}
			else {
				var1 = -0.033387963;
			}
		}
		else {
			if ((input[49]) >= (4.125)) {
				var1 = -0.17592399;
			}
			else {
				var1 = -0.14488426;
			}
		}
	}
	else {
		if ((input[49]) >= (0.125)) {
			if ((input[71]) >= (157.5)) {
				var1 = -0.10620608;
			}
			else {
				var1 = 0.059495963;
			}
		}
		else {
			if ((input[24]) >= (0.1)) {
				var1 = 0.03043573;
			}
			else {
				var1 = 0.1678364;
			}
		}
	}
	double var2;
	if ((input[70]) >= (1.5)) {
		if ((input[49]) >= (1.875)) {
			if ((input[51]) >= (1.5)) {
				var2 = -0.1427262;
			}
			else {
				var2 = -0.16376181;
			}
		}
		else {
			if ((input[12]) >= (0.2545)) {
				var2 = -0.11732166;
			}
			else {
				var2 = -0.043389127;
			}
		}
	}
	else {
		if ((input[49]) >= (0.125)) {
			if ((input[49]) >= (2.875)) {
				var2 = -0.13106704;
			}
			else {
				var2 = -0.034552973;
			}
		}
		else {
			if ((input[33]) >= (0.45)) {
				var2 = -0.0056355577;
			}
			else {
				var2 = 0.1517164;
			}
		}
	}
	double var3;
	if ((input[70]) >= (1.5)) {
		if ((input[51]) >= (2.5)) {
			if ((input[67]) >= (0.5)) {
				var3 = -0.06889262;
			}
			else {
				var3 = -0.1243001;
			}
		}
		else {
			if ((input[70]) >= (7.5)) {
				var3 = -0.15340656;
			}
			else {
				var3 = -0.1324436;
			}
		}
	}
	else {
		if ((input[26]) >= (0.1)) {
			if ((input[71]) >= (488.5)) {
				var3 = -0.08103414;
			}
			else {
				var3 = 0.0733334;
			}
		}
		else {
			if ((input[49]) >= (0.125)) {
				var3 = -0.035139244;
			}
			else {
				var3 = 0.14104472;
			}
		}
	}
	double var4;
	if ((input[70]) >= (1.5)) {
		if ((input[49]) >= (1.875)) {
			if ((input[51]) >= (1.5)) {
				var4 = -0.124092415;
			}
			else {
				var4 = -0.14475517;
			}
		}
		else {
			if ((input[12]) >= (0.2809)) {
				var4 = -0.099393226;
			}
			else {
				var4 = -0.03158994;
			}
		}
	}
	else {
		if ((input[1]) >= (0.35680002)) {
			if ((input[51]) >= (3.5)) {
				var4 = 0.06855553;
			}
			else {
				var4 = -0.076491304;
			}
		}
		else {
			if ((input[33]) >= (0.70835)) {
				var4 = -0.002856046;
			}
			else {
				var4 = 0.14043558;
			}
		}
	}
	double var5;
	if ((input[70]) >= (1.5)) {
		if ((input[51]) >= (2.5)) {
			if ((input[26]) >= (0.45)) {
				var5 = -0.09801041;
			}
			else {
				var5 = -0.046013284;
			}
		}
		else {
			if ((input[49]) >= (4.125)) {
				var5 = -0.13647717;
			}
			else {
				var5 = -0.11195903;
			}
		}
	}
	else {
		if ((input[12]) >= (0.3114)) {
			if ((input[51]) >= (3.5)) {
				var5 = 0.06353228;
			}
			else {
				var5 = -0.07035626;
			}
		}
		else {
			if ((input[33]) >= (0.55)) {
				var5 = 0.01598504;
			}
			else {
				var5 = 0.13423273;
			}
		}
	}
	double var6;
	if ((input[70]) >= (1.5)) {
		if ((input[49]) >= (1.875)) {
			if ((input[51]) >= (1.5)) {
				var6 = -0.110136285;
			}
			else {
				var6 = -0.1316858;
			}
		}
		else {
			if ((input[67]) >= (0.5)) {
				var6 = -0.047468327;
			}
			else {
				var6 = -0.10578668;
			}
		}
	}
	else {
		if ((input[12]) >= (0.1007)) {
			if ((input[51]) >= (3.5)) {
				var6 = 0.06632007;
			}
			else {
				var6 = -0.058588993;
			}
		}
		else {
			if ((input[49]) >= (1.875)) {
				var6 = -0.015668252;
			}
			else {
				var6 = 0.12981611;
			}
		}
	}
	double var7;
	if ((input[70]) >= (1.5)) {
		if ((input[51]) >= (2.5)) {
			if ((input[70]) >= (4.5)) {
				var7 = -0.078531444;
			}
			else {
				var7 = -0.01873607;
			}
		}
		else {
			if ((input[70]) >= (7.5)) {
				var7 = -0.12683943;
			}
			else {
				var7 = -0.10296271;
			}
		}
	}
	else {
		if ((input[26]) >= (0.1)) {
			if ((input[71]) >= (280.5)) {
				var7 = -0.0680598;
			}
			else {
				var7 = 0.078778744;
			}
		}
		else {
			if ((input[71]) >= (3796.5)) {
				var7 = 0.03706321;
			}
			else {
				var7 = 0.12178969;
			}
		}
	}
	double var8;
	if ((input[70]) >= (1.5)) {
		if ((input[49]) >= (2.125)) {
			if ((input[49]) >= (5.875)) {
				var8 = -0.124344446;
			}
			else {
				var8 = -0.102727704;
			}
		}
		else {
			if ((input[12]) >= (0.15540001)) {
				var8 = -0.07234256;
			}
			else {
				var8 = -0.002438889;
			}
		}
	}
	else {
		if ((input[1]) >= (0.1742)) {
			if ((input[51]) >= (3.5)) {
				var8 = 0.053757556;
			}
			else {
				var8 = -0.0566602;
			}
		}
		else {
			if ((input[33]) >= (0.70835)) {
				var8 = -0.01454968;
			}
			else {
				var8 = 0.11893399;
			}
		}
	}
	double var9;
	if ((input[70]) >= (1.5)) {
		if ((input[49]) >= (2.125)) {
			if ((input[49]) >= (5.875)) {
				var9 = -0.12049536;
			}
			else {
				var9 = -0.09773106;
			}
		}
		else {
			if ((input[67]) >= (0.5)) {
				var9 = -0.032642964;
			}
			else {
				var9 = -0.08948477;
			}
		}
	}
	else {
		if ((input[12]) >= (0.1007)) {
			if ((input[70]) >= (0.5)) {
				var9 = -0.061637707;
			}
			else {
				var9 = 0.047417473;
			}
		}
		else {
			if ((input[49]) >= (0.41665)) {
				var9 = -0.02000311;
			}
			else {
				var9 = 0.11560404;
			}
		}
	}
	double var10;
	if ((input[70]) >= (1.5)) {
		if ((input[51]) >= (2.5)) {
			if ((input[70]) >= (4.5)) {
				var10 = -0.06282293;
			}
			else {
				var10 = -0.0061188466;
			}
		}
		else {
			if ((input[70]) >= (7.5)) {
				var10 = -0.11503215;
			}
			else {
				var10 = -0.08786031;
			}
		}
	}
	else {
		if ((input[1]) >= (0.06385)) {
			if ((input[64]) >= (48.5)) {
				var10 = 0.083593994;
			}
			else {
				var10 = -0.026063625;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var10 = 0.113103025;
			}
			else {
				var10 = -0.033672236;
			}
		}
	}
	double var11;
	if ((input[70]) >= (0.5)) {
		if ((input[16]) >= (0.325)) {
			if ((input[51]) >= (2.5)) {
				var11 = -0.0464476;
			}
			else {
				var11 = -0.10371628;
			}
		}
		else {
			if ((input[1]) >= (0.1034)) {
				var11 = -0.029350398;
			}
			else {
				var11 = 0.0813139;
			}
		}
	}
	else {
		if ((input[66]) >= (7094.5)) {
			if ((input[64]) >= (48.5)) {
				var11 = 0.056760933;
			}
			else {
				var11 = -0.08121082;
			}
		}
		else {
			if ((input[26]) >= (0.65)) {
				var11 = 0.040986963;
			}
			else {
				var11 = 0.11218836;
			}
		}
	}
	double var12;
	if ((input[70]) >= (0.5)) {
		if ((input[49]) >= (0.70835)) {
			if ((input[49]) >= (4.125)) {
				var12 = -0.1070525;
			}
			else {
				var12 = -0.06799316;
			}
		}
		else {
			if ((input[12]) >= (0.02135)) {
				var12 = -0.041480687;
			}
			else {
				var12 = 0.06800221;
			}
		}
	}
	else {
		if ((input[66]) >= (5272.5)) {
			if ((input[24]) >= (0.1)) {
				var12 = -0.08560706;
			}
			else {
				var12 = 0.045418795;
			}
		}
		else {
			if ((input[49]) >= (2.875)) {
				var12 = -0.06680538;
			}
			else {
				var12 = 0.106605604;
			}
		}
	}
	double var13;
	if ((input[16]) >= (0.1)) {
		if ((input[51]) >= (3.5)) {
			if ((input[3]) >= (0.1)) {
				var13 = -0.030843427;
			}
			else {
				var13 = 0.074666984;
			}
		}
		else {
			if ((input[49]) >= (4.125)) {
				var13 = -0.103923075;
			}
			else {
				var13 = -0.057984352;
			}
		}
	}
	else {
		if ((input[70]) >= (2.5)) {
			if ((input[57]) >= (1.0081)) {
				var13 = -0.01177628;
			}
			else {
				var13 = -0.10125511;
			}
		}
		else {
			if ((input[13]) >= (0.5)) {
				var13 = -0.020447714;
			}
			else {
				var13 = 0.10133135;
			}
		}
	}
	double var14;
	if ((input[26]) >= (0.1)) {
		if ((input[51]) >= (2.5)) {
			if ((input[66]) >= (1674.5)) {
				var14 = -0.06424931;
			}
			else {
				var14 = 0.01547076;
			}
		}
		else {
			if ((input[49]) >= (5.875)) {
				var14 = -0.1075573;
			}
			else {
				var14 = -0.07898032;
			}
		}
	}
	else {
		if ((input[12]) >= (0.61405003)) {
			if ((input[64]) >= (46.5)) {
				var14 = 0.04068773;
			}
			else {
				var14 = -0.05435201;
			}
		}
		else {
			if ((input[33]) >= (0.1)) {
				var14 = 0.020945404;
			}
			else {
				var14 = 0.10018374;
			}
		}
	}
	double var15;
	if ((input[5]) >= (0.1)) {
		if ((input[51]) >= (3.5)) {
			if ((input[66]) >= (1829.5)) {
				var15 = -0.03737165;
			}
			else {
				var15 = 0.06491143;
			}
		}
		else {
			if ((input[49]) >= (4.125)) {
				var15 = -0.098495536;
			}
			else {
				var15 = -0.052967336;
			}
		}
	}
	else {
		if ((input[2]) >= (0.5)) {
			if ((input[71]) >= (5236.0)) {
				var15 = -0.062391;
			}
			else {
				var15 = 0.029620066;
			}
		}
		else {
			if ((input[49]) >= (1.875)) {
				var15 = -0.04409339;
			}
			else {
				var15 = 0.09724229;
			}
		}
	}
	double var16;
	if ((input[70]) >= (0.5)) {
		if ((input[49]) >= (2.125)) {
			if ((input[49]) >= (5.875)) {
				var16 = -0.10339447;
			}
			else {
				var16 = -0.06430694;
			}
		}
		else {
			if ((input[1]) >= (0.03115)) {
				var16 = -0.0338883;
			}
			else {
				var16 = 0.05131888;
			}
		}
	}
	else {
		if ((input[66]) >= (3802.0)) {
			if ((input[64]) >= (48.5)) {
				var16 = 0.05929718;
			}
			else {
				var16 = -0.049350057;
			}
		}
		else {
			if ((input[48]) >= (0.125)) {
				var16 = 0.020579558;
			}
			else {
				var16 = 0.1017921;
			}
		}
	}
	double var17;
	if ((input[16]) >= (0.1)) {
		if ((input[7]) >= (3.5)) {
			if ((input[12]) >= (0.0052500004)) {
				var17 = -0.009935631;
			}
			else {
				var17 = 0.08411517;
			}
		}
		else {
			if ((input[70]) >= (7.5)) {
				var17 = -0.0943773;
			}
			else {
				var17 = -0.0463849;
			}
		}
	}
	else {
		if ((input[14]) >= (0.225)) {
			if ((input[66]) >= (4213.5)) {
				var17 = -0.044509534;
			}
			else {
				var17 = 0.04078886;
			}
		}
		else {
			if ((input[49]) >= (2.625)) {
				var17 = -0.095753014;
			}
			else {
				var17 = 0.09187933;
			}
		}
	}
	double var18;
	if ((input[5]) >= (0.1)) {
		if ((input[49]) >= (2.41665)) {
			if ((input[49]) >= (6.125)) {
				var18 = -0.09995233;
			}
			else {
				var18 = -0.07105041;
			}
		}
		else {
			if ((input[66]) >= (1560.5)) {
				var18 = -0.046195995;
			}
			else {
				var18 = 0.023155805;
			}
		}
	}
	else {
		if ((input[3]) >= (0.225)) {
			if ((input[71]) >= (7764.5)) {
				var18 = -0.055590976;
			}
			else {
				var18 = 0.026058022;
			}
		}
		else {
			if ((input[49]) >= (1.875)) {
				var18 = -0.04224306;
			}
			else {
				var18 = 0.09189016;
			}
		}
	}
	double var19;
	if ((input[70]) >= (0.5)) {
		if ((input[26]) >= (2.225)) {
			if ((input[49]) >= (5.875)) {
				var19 = -0.097929;
			}
			else {
				var19 = -0.060076106;
			}
		}
		else {
			if ((input[1]) >= (2.4016)) {
				var19 = -0.054530706;
			}
			else {
				var19 = 0.015321731;
			}
		}
	}
	else {
		if ((input[66]) >= (3297.5)) {
			if ((input[64]) >= (48.5)) {
				var19 = 0.053351905;
			}
			else {
				var19 = -0.040533874;
			}
		}
		else {
			if ((input[64]) >= (45.5)) {
				var19 = 0.107768215;
			}
			else {
				var19 = 0.06327495;
			}
		}
	}
	double var20;
	if ((input[16]) >= (0.1)) {
		if ((input[70]) >= (6.5)) {
			if ((input[51]) >= (1.5)) {
				var20 = -0.061826188;
			}
			else {
				var20 = -0.096414514;
			}
		}
		else {
			if ((input[7]) >= (3.5)) {
				var20 = 0.024035484;
			}
			else {
				var20 = -0.03625733;
			}
		}
	}
	else {
		if ((input[12]) >= (0.13295001)) {
			if ((input[71]) >= (19281.0)) {
				var20 = -0.052428868;
			}
			else {
				var20 = 0.030143935;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var20 = -0.0065722386;
			}
			else {
				var20 = 0.09810836;
			}
		}
	}
	double var21;
	if ((input[70]) >= (0.5)) {
		if ((input[49]) >= (2.41665)) {
			if ((input[49]) >= (6.125)) {
				var21 = -0.09474746;
			}
			else {
				var21 = -0.0619465;
			}
		}
		else {
			if ((input[12]) >= (0.31775)) {
				var21 = -0.033726104;
			}
			else {
				var21 = 0.021453438;
			}
		}
	}
	else {
		if ((input[66]) >= (15069.5)) {
			if ((input[64]) >= (50.5)) {
				var21 = 0.011704186;
			}
			else {
				var21 = -0.07504765;
			}
		}
		else {
			if ((input[3]) >= (0.1)) {
				var21 = 0.02936016;
			}
			else {
				var21 = 0.09134687;
			}
		}
	}
	double var22;
	if ((input[18]) >= (3.5)) {
		if ((input[1]) >= (0.0138)) {
			if ((input[64]) >= (43.5)) {
				var22 = 0.03732271;
			}
			else {
				var22 = -0.031307604;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var22 = -0.0060340837;
			}
			else {
				var22 = 0.099763885;
			}
		}
	}
	else {
		if ((input[70]) >= (7.5)) {
			if ((input[51]) >= (1.5)) {
				var22 = -0.06081862;
			}
			else {
				var22 = -0.094620995;
			}
		}
		else {
			if ((input[66]) >= (114.5)) {
				var22 = -0.028287265;
			}
			else {
				var22 = 0.067556605;
			}
		}
	}
	double var23;
	if ((input[7]) >= (3.5)) {
		if ((input[1]) >= (0.064349994)) {
			if ((input[66]) >= (22247.0)) {
				var23 = -0.048516456;
			}
			else {
				var23 = 0.025301924;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var23 = -0.005941445;
			}
			else {
				var23 = 0.09486165;
			}
		}
	}
	else {
		if ((input[70]) >= (6.5)) {
			if ((input[57]) >= (1.8378501)) {
				var23 = -0.0584308;
			}
			else {
				var23 = -0.09374728;
			}
		}
		else {
			if ((input[71]) >= (3517.5)) {
				var23 = -0.051289625;
			}
			else {
				var23 = 0.0037026003;
			}
		}
	}
	double var24;
	if ((input[26]) >= (0.45)) {
		if ((input[51]) >= (1.5)) {
			if ((input[66]) >= (279.5)) {
				var24 = -0.037988342;
			}
			else {
				var24 = 0.045212965;
			}
		}
		else {
			if ((input[70]) >= (9.5)) {
				var24 = -0.0943095;
			}
			else {
				var24 = -0.06227216;
			}
		}
	}
	else {
		if ((input[12]) >= (0.0052500004)) {
			if ((input[64]) >= (45.5)) {
				var24 = 0.036989;
			}
			else {
				var24 = -0.027930303;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var24 = -0.0023870717;
			}
			else {
				var24 = 0.093800016;
			}
		}
	}
	double var25;
	if ((input[51]) >= (3.5)) {
		if ((input[14]) >= (0.1)) {
			if ((input[71]) >= (4972.5)) {
				var25 = -0.036721144;
			}
			else {
				var25 = 0.02668286;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var25 = 0.000292933;
			}
			else {
				var25 = 0.0880877;
			}
		}
	}
	else {
		if ((input[49]) >= (4.125)) {
			if ((input[49]) >= (6.125)) {
				var25 = -0.08891835;
			}
			else {
				var25 = -0.053732883;
			}
		}
		else {
			if ((input[1]) >= (0.95595)) {
				var25 = -0.039707743;
			}
			else {
				var25 = 0.014311977;
			}
		}
	}
	double var26;
	if ((input[16]) >= (0.1)) {
		if ((input[49]) >= (4.125)) {
			if ((input[70]) >= (9.5)) {
				var26 = -0.09010326;
			}
			else {
				var26 = -0.055545498;
			}
		}
		else {
			if ((input[66]) >= (305.5)) {
				var26 = -0.023665592;
			}
			else {
				var26 = 0.04934324;
			}
		}
	}
	else {
		if ((input[66]) >= (2897.5)) {
			if ((input[64]) >= (50.5)) {
				var26 = 0.0817663;
			}
			else {
				var26 = -0.014919574;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var26 = 0.0819263;
			}
			else {
				var26 = -0.0685947;
			}
		}
	}
	double var27;
	if ((input[10]) >= (3.5)) {
		if ((input[66]) >= (2486.5)) {
			if ((input[64]) >= (49.5)) {
				var27 = 0.069728464;
			}
			else {
				var27 = -0.012383201;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var27 = 0.076873235;
			}
			else {
				var27 = -0.05743742;
			}
		}
	}
	else {
		if ((input[70]) >= (7.5)) {
			if ((input[57]) >= (1.76075)) {
				var27 = -0.054026555;
			}
			else {
				var27 = -0.09050227;
			}
		}
		else {
			if ((input[66]) >= (1636.5)) {
				var27 = -0.03748835;
			}
			else {
				var27 = 0.0077662393;
			}
		}
	}
	double var28;
	if ((input[12]) >= (0.93130004)) {
		if ((input[51]) >= (1.5)) {
			if ((input[60]) >= (1.74035)) {
				var28 = -0.008643821;
			}
			else {
				var28 = -0.063158326;
			}
		}
		else {
			if ((input[56]) >= (7.6518)) {
				var28 = -0.059092987;
			}
			else {
				var28 = -0.097869724;
			}
		}
	}
	else {
		if ((input[70]) >= (0.5)) {
			if ((input[28]) >= (2.5)) {
				var28 = 0.012702832;
			}
			else {
				var28 = -0.03712415;
			}
		}
		else {
			if ((input[2]) >= (0.5)) {
				var28 = -0.01415461;
			}
			else {
				var28 = 0.08176597;
			}
		}
	}
	double var29;
	if ((input[26]) >= (0.45)) {
		if ((input[49]) >= (5.875)) {
			if ((input[70]) >= (10.5)) {
				var29 = -0.092339166;
			}
			else {
				var29 = -0.06500979;
			}
		}
		else {
			if ((input[66]) >= (112.5)) {
				var29 = -0.03147465;
			}
			else {
				var29 = 0.05956757;
			}
		}
	}
	else {
		if ((input[12]) >= (0.005)) {
			if ((input[64]) >= (43.5)) {
				var29 = 0.025642544;
			}
			else {
				var29 = -0.033499643;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var29 = -0.007400191;
			}
			else {
				var29 = 0.086464964;
			}
		}
	}
	double var30;
	if ((input[12]) >= (1.59045)) {
		if ((input[51]) >= (2.5)) {
			if ((input[64]) >= (43.5)) {
				var30 = 0.0021220294;
			}
			else {
				var30 = -0.04124199;
			}
		}
		else {
			if ((input[60]) >= (3.0936499)) {
				var30 = -0.045351144;
			}
			else {
				var30 = -0.08896931;
			}
		}
	}
	else {
		if ((input[14]) >= (0.1)) {
			if ((input[3]) >= (0.1)) {
				var30 = -0.0123265805;
			}
			else {
				var30 = 0.05123157;
			}
		}
		else {
			if ((input[70]) >= (2.5)) {
				var30 = -0.043449726;
			}
			else {
				var30 = 0.072687835;
			}
		}
	}
	double var31;
	if ((input[49]) >= (1.875)) {
		if ((input[57]) >= (1.06915)) {
			if ((input[70]) >= (7.5)) {
				var31 = -0.060075592;
			}
			else {
				var31 = 0.0022397782;
			}
		}
		else {
			if ((input[66]) >= (55.5)) {
				var31 = -0.08333283;
			}
			else {
				var31 = 0.015771646;
			}
		}
	}
	else {
		if ((input[3]) >= (0.1)) {
			if ((input[66]) >= (15035.0)) {
				var31 = -0.04328724;
			}
			else {
				var31 = 0.0071950005;
			}
		}
		else {
			if ((input[70]) >= (3.5)) {
				var31 = -0.041803416;
			}
			else {
				var31 = 0.06795016;
			}
		}
	}
	double var32;
	if ((input[18]) >= (3.5)) {
		if ((input[66]) >= (1571.5)) {
			if ((input[64]) >= (50.5)) {
				var32 = 0.07417382;
			}
			else {
				var32 = -0.0074053826;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var32 = 0.07669579;
			}
			else {
				var32 = -0.05288709;
			}
		}
	}
	else {
		if ((input[49]) >= (5.875)) {
			if ((input[60]) >= (3.5654001)) {
				var32 = -0.052926857;
			}
			else {
				var32 = -0.0888545;
			}
		}
		else {
			if ((input[66]) >= (692.5)) {
				var32 = -0.026312653;
			}
			else {
				var32 = 0.020840561;
			}
		}
	}
	double var33;
	if ((input[1]) >= (2.5576)) {
		if ((input[51]) >= (2.5)) {
			if ((input[66]) >= (22243.0)) {
				var33 = -0.048912697;
			}
			else {
				var33 = -0.0047908486;
			}
		}
		else {
			if ((input[57]) >= (0.9311)) {
				var33 = -0.04724946;
			}
			else {
				var33 = -0.09064448;
			}
		}
	}
	else {
		if ((input[64]) >= (43.5)) {
			if ((input[66]) >= (22273.5)) {
				var33 = -0.03323287;
			}
			else {
				var33 = 0.06329662;
			}
		}
		else {
			if ((input[66]) >= (530.5)) {
				var33 = -0.02874643;
			}
			else {
				var33 = 0.020905456;
			}
		}
	}
	double var34;
	if ((input[10]) >= (3.5)) {
		if ((input[0]) >= (6.0)) {
			if ((input[66]) >= (69.5)) {
				var34 = -0.04438641;
			}
			else {
				var34 = 0.06389782;
			}
		}
		else {
			if ((input[1]) >= (0.01355)) {
				var34 = 0.015893314;
			}
			else {
				var34 = 0.08457273;
			}
		}
	}
	else {
		if ((input[49]) >= (5.125)) {
			if ((input[56]) >= (7.65635)) {
				var34 = -0.0521124;
			}
			else {
				var34 = -0.08985553;
			}
		}
		else {
			if ((input[66]) >= (182.5)) {
				var34 = -0.021181399;
			}
			else {
				var34 = 0.045317423;
			}
		}
	}
	double var35;
	if ((input[28]) >= (2.5)) {
		if ((input[66]) >= (1639.5)) {
			if ((input[61]) >= (0.026250001)) {
				var35 = 0.005988708;
			}
			else {
				var35 = -0.06365371;
			}
		}
		else {
			if ((input[64]) >= (43.5)) {
				var35 = 0.086451456;
			}
			else {
				var35 = 0.012992679;
			}
		}
	}
	else {
		if ((input[56]) >= (7.31215)) {
			if ((input[66]) >= (1253.5)) {
				var35 = -0.037462648;
			}
			else {
				var35 = 0.037400577;
			}
		}
		else {
			if ((input[66]) >= (23.5)) {
				var35 = -0.06983887;
			}
			else {
				var35 = 0.058104675;
			}
		}
	}
	double var36;
	if ((input[12]) >= (1.9298)) {
		if ((input[57]) >= (1.1264)) {
			if ((input[16]) >= (8.1)) {
				var36 = -0.046979185;
			}
			else {
				var36 = -0.0019381641;
			}
		}
		else {
			if ((input[64]) >= (46.5)) {
				var36 = -0.024475854;
			}
			else {
				var36 = -0.08280986;
			}
		}
	}
	else {
		if ((input[64]) >= (43.5)) {
			if ((input[66]) >= (15316.0)) {
				var36 = -0.01931691;
			}
			else {
				var36 = 0.060935892;
			}
		}
		else {
			if ((input[66]) >= (511.5)) {
				var36 = -0.024129113;
			}
			else {
				var36 = 0.021876989;
			}
		}
	}
	double var37;
	if ((input[49]) >= (4.125)) {
		if ((input[62]) >= (0.34745002)) {
			if ((input[70]) >= (11.5)) {
				var37 = -0.07204601;
			}
			else {
				var37 = -0.013635037;
			}
		}
		else {
			if ((input[66]) >= (420.5)) {
				var37 = -0.10448637;
			}
			else {
				var37 = -0.06569325;
			}
		}
	}
	else {
		if ((input[64]) >= (46.5)) {
			if ((input[66]) >= (5503.5)) {
				var37 = 0.0013550032;
			}
			else {
				var37 = 0.09166422;
			}
		}
		else {
			if ((input[66]) >= (742.5)) {
				var37 = -0.019148298;
			}
			else {
				var37 = 0.030678136;
			}
		}
	}
	double var38;
	if ((input[1]) >= (3.09875)) {
		if ((input[57]) >= (1.0507)) {
			if ((input[16]) >= (8.1)) {
				var38 = -0.05256955;
			}
			else {
				var38 = -0.00551096;
			}
		}
		else {
			if ((input[48]) >= (-0.5)) {
				var38 = -0.069438964;
			}
			else {
				var38 = 0.22749059;
			}
		}
	}
	else {
		if ((input[64]) >= (45.5)) {
			if ((input[66]) >= (22306.5)) {
				var38 = -0.028491262;
			}
			else {
				var38 = 0.06547546;
			}
		}
		else {
			if ((input[61]) >= (0.024549998)) {
				var38 = 0.008828829;
			}
			else {
				var38 = -0.044952583;
			}
		}
	}
	double var39;
	if ((input[26]) >= (4.1)) {
		if ((input[70]) >= (10.5)) {
			if ((input[60]) >= (3.5971498)) {
				var39 = -0.051778805;
			}
			else {
				var39 = -0.088547714;
			}
		}
		else {
			if ((input[56]) >= (7.33635)) {
				var39 = 0.0021549396;
			}
			else {
				var39 = -0.049981024;
			}
		}
	}
	else {
		if ((input[66]) >= (15068.0)) {
			if ((input[61]) >= (0.1122)) {
				var39 = -0.0120526;
			}
			else {
				var39 = -0.07453371;
			}
		}
		else {
			if ((input[64]) >= (46.5)) {
				var39 = 0.07503648;
			}
			else {
				var39 = 0.008083807;
			}
		}
	}
	double var40;
	if ((input[12]) >= (0.0021000002)) {
		if ((input[56]) >= (2.1824)) {
			if ((input[66]) >= (3681.5)) {
				var40 = -0.017555192;
			}
			else {
				var40 = 0.028058747;
			}
		}
		else {
			if ((input[66]) >= (31.5)) {
				var40 = -0.071667776;
			}
			else {
				var40 = 0.054169953;
			}
		}
	}
	else {
		if ((input[0]) >= (6.0)) {
			if ((input[58]) >= (19.0704)) {
				var40 = 0.096397266;
			}
			else {
				var40 = -0.035995048;
			}
		}
		else {
			if ((input[1]) >= (0.06730001)) {
				var40 = -0.06892066;
			}
			else {
				var40 = 0.079169385;
			}
		}
	}
	double var41;
	if ((input[49]) >= (6.125)) {
		if ((input[57]) >= (1.0331)) {
			if ((input[26]) >= (6.45)) {
				var41 = -0.059259843;
			}
			else {
				var41 = 0.014338571;
			}
		}
		else {
			if ((input[55]) >= (1.8237)) {
				var41 = -0.09215588;
			}
			else {
				var41 = 0.10151525;
			}
		}
	}
	else {
		if ((input[66]) >= (1037.5)) {
			if ((input[57]) >= (2.00545)) {
				var41 = 0.010369047;
			}
			else {
				var41 = -0.033592064;
			}
		}
		else {
			if ((input[64]) >= (43.5)) {
				var41 = 0.08470977;
			}
			else {
				var41 = 0.0073844227;
			}
		}
	}
	double var42;
	if ((input[61]) >= (0.02395)) {
		if ((input[66]) >= (3681.5)) {
			if ((input[64]) >= (43.5)) {
				var42 = 0.004640742;
			}
			else {
				var42 = -0.036570508;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var42 = 0.05441279;
			}
			else {
				var42 = -0.0323828;
			}
		}
	}
	else {
		if ((input[66]) >= (283.5)) {
			if ((input[64]) >= (49.5)) {
				var42 = 0.05034707;
			}
			else {
				var42 = -0.059760757;
			}
		}
		else {
			if ((input[70]) >= (2.5)) {
				var42 = -0.06198417;
			}
			else {
				var42 = 0.05618092;
			}
		}
	}
	double var43;
	if ((input[26]) >= (4.1)) {
		if ((input[70]) >= (10.5)) {
			if ((input[57]) >= (1.7276001)) {
				var43 = -0.044851433;
			}
			else {
				var43 = -0.08307158;
			}
		}
		else {
			if ((input[58]) >= (7.60115)) {
				var43 = 0.0007682931;
			}
			else {
				var43 = -0.043805603;
			}
		}
	}
	else {
		if ((input[66]) >= (1571.5)) {
			if ((input[57]) >= (2.00965)) {
				var43 = 0.013421654;
			}
			else {
				var43 = -0.030314405;
			}
		}
		else {
			if ((input[64]) >= (43.5)) {
				var43 = 0.07577067;
			}
			else {
				var43 = 0.010673552;
			}
		}
	}
	double var44;
	if ((input[61]) >= (0.019949999)) {
		if ((input[66]) >= (1636.5)) {
			if ((input[64]) >= (43.5)) {
				var44 = 0.0110564055;
			}
			else {
				var44 = -0.028731132;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var44 = 0.063382365;
			}
			else {
				var44 = -0.029228166;
			}
		}
	}
	else {
		if ((input[66]) >= (55.5)) {
			if ((input[64]) >= (49.5)) {
				var44 = 0.051905543;
			}
			else {
				var44 = -0.050562996;
			}
		}
		else {
			if ((input[70]) >= (2.5)) {
				var44 = -0.06650937;
			}
			else {
				var44 = 0.077589415;
			}
		}
	}
	double var45;
	if ((input[1]) >= (3.09875)) {
		if ((input[49]) >= (3.58335)) {
			if ((input[62]) >= (17.810799)) {
				var45 = -0.036641255;
			}
			else {
				var45 = -0.08900445;
			}
		}
		else {
			if ((input[64]) >= (46.5)) {
				var45 = 0.006145516;
			}
			else {
				var45 = -0.026434174;
			}
		}
	}
	else {
		if ((input[64]) >= (45.5)) {
			if ((input[66]) >= (22306.5)) {
				var45 = -0.024479246;
			}
			else {
				var45 = 0.056280166;
			}
		}
		else {
			if ((input[62]) >= (4.35435)) {
				var45 = 0.009786255;
			}
			else {
				var45 = -0.031990558;
			}
		}
	}
	double var46;
	if ((input[12]) >= (0.0021000002)) {
		if ((input[58]) >= (6.22635)) {
			if ((input[66]) >= (9273.0)) {
				var46 = -0.021938706;
			}
			else {
				var46 = 0.020610074;
			}
		}
		else {
			if ((input[66]) >= (291.5)) {
				var46 = -0.05842482;
			}
			else {
				var46 = -0.0047336044;
			}
		}
	}
	else {
		if ((input[0]) >= (3.0)) {
			if ((input[66]) >= (22.5)) {
				var46 = -0.016154567;
			}
			else {
				var46 = 0.08676218;
			}
		}
		else {
			if ((input[1]) >= (0.0602)) {
				var46 = -0.05961391;
			}
			else {
				var46 = 0.08437719;
			}
		}
	}
	double var47;
	if ((input[57]) >= (2.00545)) {
		if ((input[66]) >= (5022.5)) {
			if ((input[64]) >= (43.5)) {
				var47 = 0.0096945595;
			}
			else {
				var47 = -0.02863248;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var47 = 0.060040303;
			}
			else {
				var47 = -0.02747855;
			}
		}
	}
	else {
		if ((input[70]) >= (4.5)) {
			if ((input[58]) >= (51.7762)) {
				var47 = -0.023572547;
			}
			else {
				var47 = -0.06850337;
			}
		}
		else {
			if ((input[71]) >= (934.5)) {
				var47 = -0.029236669;
			}
			else {
				var47 = 0.029544566;
			}
		}
	}
	double var48;
	if ((input[64]) >= (49.5)) {
		if ((input[66]) >= (28493.5)) {
			if ((input[61]) >= (0.1485)) {
				var48 = 0.02893418;
			}
			else {
				var48 = -0.12139451;
			}
		}
		else {
			if ((input[70]) >= (4.5)) {
				var48 = -0.099123396;
			}
			else {
				var48 = 0.10166027;
			}
		}
	}
	else {
		if ((input[57]) >= (2.00425)) {
			if ((input[66]) >= (5323.5)) {
				var48 = -0.009414767;
			}
			else {
				var48 = 0.032663587;
			}
		}
		else {
			if ((input[66]) >= (696.5)) {
				var48 = -0.03538121;
			}
			else {
				var48 = 0.010903354;
			}
		}
	}
	double var49;
	if ((input[64]) >= (46.5)) {
		if ((input[66]) >= (4806.5)) {
			if ((input[61]) >= (0.11245)) {
				var49 = 0.030721812;
			}
			else {
				var49 = -0.043728393;
			}
		}
		else {
			if ((input[70]) >= (1.5)) {
				var49 = -0.031029565;
			}
			else {
				var49 = 0.08681601;
			}
		}
	}
	else {
		if ((input[61]) >= (0.029150002)) {
			if ((input[66]) >= (1328.5)) {
				var49 = -0.011681157;
			}
			else {
				var49 = 0.032901112;
			}
		}
		else {
			if ((input[66]) >= (376.5)) {
				var49 = -0.05375108;
			}
			else {
				var49 = 0.005765881;
			}
		}
	}
	double var50;
	if ((input[26]) >= (4.675)) {
		if ((input[70]) >= (10.5)) {
			if ((input[49]) >= (8.125)) {
				var50 = -0.085441746;
			}
			else {
				var50 = -0.045195654;
			}
		}
		else {
			if ((input[71]) >= (3431.5)) {
				var50 = -0.041749846;
			}
			else {
				var50 = -0.003777016;
			}
		}
	}
	else {
		if ((input[0]) >= (6.0)) {
			if ((input[2]) >= (0.5)) {
				var50 = 0.014170821;
			}
			else {
				var50 = -0.044069495;
			}
		}
		else {
			if ((input[12]) >= (0.0021000002)) {
				var50 = 0.005983046;
			}
			else {
				var50 = 0.060870606;
			}
		}
	}
	double var51;
	if ((input[64]) >= (43.5)) {
		if ((input[66]) >= (1667.5)) {
			if ((input[56]) >= (26.5793)) {
				var51 = 0.018965904;
			}
			else {
				var51 = -0.038435202;
			}
		}
		else {
			if ((input[70]) >= (1.5)) {
				var51 = -0.0072318497;
			}
			else {
				var51 = 0.0716286;
			}
		}
	}
	else {
		if ((input[66]) >= (112.5)) {
			if ((input[58]) >= (2.00625)) {
				var51 = -0.010066327;
			}
			else {
				var51 = -0.066259734;
			}
		}
		else {
			if ((input[64]) >= (39.5)) {
				var51 = 0.070332006;
			}
			else {
				var51 = 0.0061652637;
			}
		}
	}
	double var52;
	if ((input[49]) >= (6.125)) {
		if ((input[57]) >= (1.0331)) {
			if ((input[70]) >= (12.5)) {
				var52 = -0.06375896;
			}
			else {
				var52 = -0.0016297993;
			}
		}
		else {
			if ((input[55]) >= (1.8237)) {
				var52 = -0.08456471;
			}
			else {
				var52 = 0.09107088;
			}
		}
	}
	else {
		if ((input[66]) >= (22243.0)) {
			if ((input[70]) >= (1.5)) {
				var52 = -0.0059653902;
			}
			else {
				var52 = -0.05084714;
			}
		}
		else {
			if ((input[64]) >= (45.5)) {
				var52 = 0.04309063;
			}
			else {
				var52 = -0.0043895184;
			}
		}
	}
	double var53;
	if ((input[64]) >= (49.5)) {
		if ((input[66]) >= (28493.5)) {
			if ((input[61]) >= (0.1485)) {
				var53 = 0.024294006;
			}
			else {
				var53 = -0.104335;
			}
		}
		else {
			if ((input[70]) >= (4.5)) {
				var53 = -0.095545225;
			}
			else {
				var53 = 0.09889086;
			}
		}
	}
	else {
		if ((input[60]) >= (2.8988)) {
			if ((input[66]) >= (9273.0)) {
				var53 = -0.018156027;
			}
			else {
				var53 = 0.025864968;
			}
		}
		else {
			if ((input[66]) >= (292.5)) {
				var53 = -0.034319516;
			}
			else {
				var53 = 0.020878045;
			}
		}
	}
	double var54;
	if ((input[26]) >= (4.675)) {
		if ((input[70]) >= (10.5)) {
			if ((input[49]) >= (8.125)) {
				var54 = -0.082165174;
			}
			else {
				var54 = -0.040817287;
			}
		}
		else {
			if ((input[58]) >= (7.60115)) {
				var54 = 0.00024005988;
			}
			else {
				var54 = -0.032696646;
			}
		}
	}
	else {
		if ((input[66]) >= (15068.0)) {
			if ((input[61]) >= (0.1122)) {
				var54 = -0.005816045;
			}
			else {
				var54 = -0.054623343;
			}
		}
		else {
			if ((input[64]) >= (46.5)) {
				var54 = 0.060711592;
			}
			else {
				var54 = 0.0045078974;
			}
		}
	}
	double var55;
	if ((input[57]) >= (2.34975)) {
		if ((input[48]) >= (0.29165)) {
			if ((input[53]) >= (0.5)) {
				var55 = 0.012909601;
			}
			else {
				var55 = -0.02188902;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var55 = 0.03899595;
			}
			else {
				var55 = -0.029256893;
			}
		}
	}
	else {
		if ((input[66]) >= (696.5)) {
			if ((input[58]) >= (15.682051)) {
				var55 = -0.0059743314;
			}
			else {
				var55 = -0.042498976;
			}
		}
		else {
			if ((input[70]) >= (3.5)) {
				var55 = -0.04154403;
			}
			else {
				var55 = 0.032984182;
			}
		}
	}
	double var56;
	if ((input[61]) >= (0.01955)) {
		if ((input[66]) >= (3681.5)) {
			if ((input[61]) >= (0.09945)) {
				var56 = 0.0013781836;
			}
			else {
				var56 = -0.031639505;
			}
		}
		else {
			if ((input[64]) >= (40.5)) {
				var56 = 0.044389647;
			}
			else {
				var56 = -0.007797211;
			}
		}
	}
	else {
		if ((input[53]) >= (0.5)) {
			if ((input[47]) >= (0.53034997)) {
				var56 = 0.016398324;
			}
			else {
				var56 = -0.06372399;
			}
		}
		else {
			if ((input[71]) >= (697.5)) {
				var56 = -0.03672287;
			}
			else {
				var56 = 0.015218692;
			}
		}
	}
	double var57;
	if ((input[64]) >= (50.5)) {
		if ((input[66]) >= (33502.0)) {
			if ((input[61]) >= (0.24149999)) {
				var57 = 0.044518277;
			}
			else {
				var57 = -0.073954225;
			}
		}
		else {
			if ((input[49]) >= (3.375)) {
				var57 = -0.0957882;
			}
			else {
				var57 = 0.09972825;
			}
		}
	}
	else {
		if ((input[56]) >= (2.1824)) {
			if ((input[66]) >= (3406.5)) {
				var57 = -0.010816816;
			}
			else {
				var57 = 0.022051817;
			}
		}
		else {
			if ((input[66]) >= (199.5)) {
				var57 = -0.04149137;
			}
			else {
				var57 = 0.020355579;
			}
		}
	}
	double var58;
	if ((input[64]) >= (43.5)) {
		if ((input[66]) >= (1632.5)) {
			if ((input[56]) >= (26.5793)) {
				var58 = 0.017256418;
			}
			else {
				var58 = -0.03369238;
			}
		}
		else {
			if ((input[56]) >= (18.8587)) {
				var58 = 0.09634675;
			}
			else {
				var58 = 0.03978557;
			}
		}
	}
	else {
		if ((input[66]) >= (112.5)) {
			if ((input[56]) >= (0.86915004)) {
				var58 = -0.009920744;
			}
			else {
				var58 = -0.06312554;
			}
		}
		else {
			if ((input[70]) >= (0.5)) {
				var58 = 0.010601562;
			}
			else {
				var58 = 0.07620812;
			}
		}
	}
	double var59;
	if ((input[12]) >= (1.59045)) {
		if ((input[60]) >= (4.6917496)) {
			if ((input[66]) >= (9278.0)) {
				var59 = -0.017056242;
			}
			else {
				var59 = 0.018701663;
			}
		}
		else {
			if ((input[48]) >= (-0.5)) {
				var59 = -0.037857324;
			}
			else {
				var59 = 0.18096286;
			}
		}
	}
	else {
		if ((input[0]) >= (6.0)) {
			if ((input[2]) >= (0.5)) {
				var59 = 0.017850056;
			}
			else {
				var59 = -0.041325636;
			}
		}
		else {
			if ((input[1]) >= (0.00135)) {
				var59 = 0.006519956;
			}
			else {
				var59 = 0.059690356;
			}
		}
	}
	double var60;
	if ((input[64]) >= (43.5)) {
		if ((input[66]) >= (22243.0)) {
			if ((input[57]) >= (4.1205997)) {
				var60 = 0.0357663;
			}
			else {
				var60 = -0.03625941;
			}
		}
		else {
			if ((input[56]) >= (26.5793)) {
				var60 = 0.054362345;
			}
			else {
				var60 = 0.0010383294;
			}
		}
	}
	else {
		if ((input[46]) >= (6.06185)) {
			if ((input[66]) >= (1719.5)) {
				var60 = -0.008471623;
			}
			else {
				var60 = 0.029133102;
			}
		}
		else {
			if ((input[66]) >= (112.5)) {
				var60 = -0.02934905;
			}
			else {
				var60 = 0.030539414;
			}
		}
	}
	double var61;
	if ((input[28]) >= (0.5)) {
		if ((input[0]) >= (6.0)) {
			if ((input[70]) >= (1.5)) {
				var61 = 0.003823001;
			}
			else {
				var61 = -0.04473469;
			}
		}
		else {
			if ((input[66]) >= (1223.5)) {
				var61 = 0.0022019935;
			}
			else {
				var61 = 0.041782517;
			}
		}
	}
	else {
		if ((input[70]) >= (8.5)) {
			if ((input[57]) >= (2.0634499)) {
				var61 = -0.03300983;
			}
			else {
				var61 = -0.07011228;
			}
		}
		else {
			if ((input[71]) >= (3379.5)) {
				var61 = -0.04239196;
			}
			else {
				var61 = -0.002168358;
			}
		}
	}
	double var62;
	if ((input[64]) >= (49.5)) {
		if ((input[66]) >= (28493.5)) {
			if ((input[61]) >= (0.1485)) {
				var62 = 0.020675063;
			}
			else {
				var62 = -0.08335832;
			}
		}
		else {
			if ((input[70]) >= (4.5)) {
				var62 = -0.09848072;
			}
			else {
				var62 = 0.095321834;
			}
		}
	}
	else {
		if ((input[57]) >= (2.16775)) {
			if ((input[64]) >= (38.5)) {
				var62 = 0.016520046;
			}
			else {
				var62 = -0.028879648;
			}
		}
		else {
			if ((input[66]) >= (1073.5)) {
				var62 = -0.026713064;
			}
			else {
				var62 = 0.0051660365;
			}
		}
	}
	double var63;
	if ((input[64]) >= (46.5)) {
		if ((input[66]) >= (4857.0)) {
			if ((input[61]) >= (0.14655)) {
				var63 = 0.029004714;
			}
			else {
				var63 = -0.032101065;
			}
		}
		else {
			if ((input[71]) >= (-2109.5)) {
				var63 = 0.0771198;
			}
			else {
				var63 = -0.051561266;
			}
		}
	}
	else {
		if ((input[58]) >= (8.61875)) {
			if ((input[66]) >= (2502.5)) {
				var63 = -0.009798916;
			}
			else {
				var63 = 0.026536826;
			}
		}
		else {
			if ((input[66]) >= (537.5)) {
				var63 = -0.046792;
			}
			else {
				var63 = 0.0022628333;
			}
		}
	}
	double var64;
	if ((input[20]) >= (1.5)) {
		if ((input[46]) >= (5.5651503)) {
			if ((input[26]) >= (5.9)) {
				var64 = -0.020205969;
			}
			else {
				var64 = 0.011279653;
			}
		}
		else {
			if ((input[51]) >= (3.5)) {
				var64 = 0.008620745;
			}
			else {
				var64 = -0.041683596;
			}
		}
	}
	else {
		if ((input[0]) >= (6.0)) {
			if ((input[61]) >= (0.08465)) {
				var64 = 0.008739732;
			}
			else {
				var64 = -0.039491285;
			}
		}
		else {
			if ((input[71]) >= (7459.5)) {
				var64 = -0.009865365;
			}
			else {
				var64 = 0.031646576;
			}
		}
	}
	double var65;
	if ((input[64]) >= (42.5)) {
		if ((input[66]) >= (14187.5)) {
			if ((input[57]) >= (2.3567)) {
				var65 = -0.002189054;
			}
			else {
				var65 = -0.047365755;
			}
		}
		else {
			if ((input[56]) >= (27.2838)) {
				var65 = 0.058356434;
			}
			else {
				var65 = 0.0002884309;
			}
		}
	}
	else {
		if ((input[46]) >= (6.06185)) {
			if ((input[66]) >= (1720.5)) {
				var65 = -0.0090050185;
			}
			else {
				var65 = 0.029578581;
			}
		}
		else {
			if ((input[66]) >= (112.5)) {
				var65 = -0.028032849;
			}
			else {
				var65 = 0.02372518;
			}
		}
	}
	double var66;
	if ((input[64]) >= (50.5)) {
		if ((input[66]) >= (33502.0)) {
			if ((input[57]) >= (1.96225)) {
				var66 = 0.005053747;
			}
			else {
				var66 = -0.118212774;
			}
		}
		else {
			if ((input[49]) >= (3.375)) {
				var66 = -0.092714295;
			}
			else {
				var66 = 0.09632325;
			}
		}
	}
	else {
		if ((input[57]) >= (1.27495)) {
			if ((input[66]) >= (7881.5)) {
				var66 = -0.012596145;
			}
			else {
				var66 = 0.0143280225;
			}
		}
		else {
			if ((input[70]) >= (4.5)) {
				var66 = -0.049095135;
			}
			else {
				var66 = -0.009140032;
			}
		}
	}
	double var67;
	if ((input[12]) >= (0.0021000002)) {
		if ((input[58]) >= (8.7411)) {
			if ((input[71]) >= (7430.5)) {
				var67 = -0.0117284935;
			}
			else {
				var67 = 0.016046373;
			}
		}
		else {
			if ((input[64]) >= (47.5)) {
				var67 = 0.00767171;
			}
			else {
				var67 = -0.037926886;
			}
		}
	}
	else {
		if ((input[0]) >= (3.0)) {
			if ((input[2]) >= (0.5)) {
				var67 = 0.03924251;
			}
			else {
				var67 = -0.024963979;
			}
		}
		else {
			if ((input[1]) >= (0.02465)) {
				var67 = -0.04140495;
			}
			else {
				var67 = 0.07189946;
			}
		}
	}
	double var68;
	if ((input[64]) >= (42.5)) {
		if ((input[66]) >= (22243.0)) {
			if ((input[57]) >= (4.1205997)) {
				var68 = 0.031410582;
			}
			else {
				var68 = -0.028956031;
			}
		}
		else {
			if ((input[56]) >= (18.4156)) {
				var68 = 0.0405416;
			}
			else {
				var68 = -0.00405075;
			}
		}
	}
	else {
		if ((input[70]) >= (1.5)) {
			if ((input[46]) >= (5.5844)) {
				var68 = 0.01615773;
			}
			else {
				var68 = -0.02454264;
			}
		}
		else {
			if ((input[71]) >= (487.5)) {
				var68 = -0.047242444;
			}
			else {
				var68 = 0.016870415;
			}
		}
	}
	double var69;
	if ((input[64]) >= (49.5)) {
		if ((input[66]) >= (34932.0)) {
			if ((input[62]) >= (740.9169)) {
				var69 = 0.031910803;
			}
			else {
				var69 = -0.06457242;
			}
		}
		else {
			if ((input[70]) >= (5.5)) {
				var69 = -0.076380014;
			}
			else {
				var69 = 0.08703464;
			}
		}
	}
	else {
		if ((input[66]) >= (1138.5)) {
			if ((input[58]) >= (16.21435)) {
				var69 = -0.0005739567;
			}
			else {
				var69 = -0.038642745;
			}
		}
		else {
			if ((input[64]) >= (40.5)) {
				var69 = 0.03522671;
			}
			else {
				var69 = -0.010774322;
			}
		}
	}
	double var70;
	if ((input[1]) >= (3.09875)) {
		if ((input[46]) >= (5.7939)) {
			if ((input[26]) >= (7.55)) {
				var70 = -0.032664042;
			}
			else {
				var70 = 0.0053457497;
			}
		}
		else {
			if ((input[48]) >= (-0.5)) {
				var70 = -0.030910451;
			}
			else {
				var70 = 0.21014264;
			}
		}
	}
	else {
		if ((input[64]) >= (43.5)) {
			if ((input[0]) >= (6.0)) {
				var70 = -0.035293892;
			}
			else {
				var70 = 0.028451636;
			}
		}
		else {
			if ((input[70]) >= (1.5)) {
				var70 = 0.0090466915;
			}
			else {
				var70 = -0.01837973;
			}
		}
	}
	double var71;
	if ((input[53]) >= (2.5)) {
		if ((input[63]) >= (0.99995)) {
			if ((input[3]) >= (0.225)) {
				var71 = -0.04377187;
			}
			else {
				var71 = -0.091699496;
			}
		}
		else {
			if ((input[12]) >= (0.25524998)) {
				var71 = -0.029868796;
			}
			else {
				var71 = 0.15971963;
			}
		}
	}
	else {
		if ((input[66]) >= (3406.5)) {
			if ((input[61]) >= (0.09425)) {
				var71 = 0.0025565277;
			}
			else {
				var71 = -0.030551855;
			}
		}
		else {
			if ((input[64]) >= (46.5)) {
				var71 = 0.07186277;
			}
			else {
				var71 = 0.0041721435;
			}
		}
	}
	double var72;
	if ((input[28]) >= (0.5)) {
		if ((input[0]) >= (6.0)) {
			if ((input[3]) >= (0.63335)) {
				var72 = 0.016449986;
			}
			else {
				var72 = -0.029392788;
			}
		}
		else {
			if ((input[12]) >= (0.0021000002)) {
				var72 = 0.003669301;
			}
			else {
				var72 = 0.0448955;
			}
		}
	}
	else {
		if ((input[70]) >= (8.5)) {
			if ((input[57]) >= (1.7276001)) {
				var72 = -0.030882347;
			}
			else {
				var72 = -0.06821119;
			}
		}
		else {
			if ((input[71]) >= (3355.5)) {
				var72 = -0.036133554;
			}
			else {
				var72 = -0.0012132934;
			}
		}
	}
	double var73;
	if ((input[56]) >= (2.1824)) {
		if ((input[66]) >= (2502.5)) {
			if ((input[56]) >= (37.57435)) {
				var73 = 0.0013951838;
			}
			else {
				var73 = -0.025848404;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var73 = 0.03269916;
			}
			else {
				var73 = -0.01772338;
			}
		}
	}
	else {
		if ((input[66]) >= (55.5)) {
			if ((input[64]) >= (45.5)) {
				var73 = 0.004509655;
			}
			else {
				var73 = -0.037580833;
			}
		}
		else {
			if ((input[70]) >= (2.5)) {
				var73 = -0.04854088;
			}
			else {
				var73 = 0.061858345;
			}
		}
	}
	double var74;
	if ((input[57]) >= (3.19115)) {
		if ((input[47]) >= (1.07575)) {
			if ((input[59]) >= (1.8102)) {
				var74 = 0.043270316;
			}
			else {
				var74 = -0.034020204;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var74 = 0.034509297;
			}
			else {
				var74 = -0.019358506;
			}
		}
	}
	else {
		if ((input[71]) >= (70446.0)) {
			if ((input[70]) >= (3.5)) {
				var74 = -0.018587692;
			}
			else {
				var74 = -0.107447855;
			}
		}
		else {
			if ((input[64]) >= (49.5)) {
				var74 = 0.060255606;
			}
			else {
				var74 = -0.0052555474;
			}
		}
	}
	double var75;
	if ((input[12]) >= (1.59045)) {
		if ((input[58]) >= (50.20865)) {
			if ((input[66]) >= (9260.5)) {
				var75 = -0.013026548;
			}
			else {
				var75 = 0.019927109;
			}
		}
		else {
			if ((input[49]) >= (6.125)) {
				var75 = -0.07303042;
			}
			else {
				var75 = -0.020619547;
			}
		}
	}
	else {
		if ((input[64]) >= (47.5)) {
			if ((input[0]) >= (6.0)) {
				var75 = -0.032301836;
			}
			else {
				var75 = 0.04529899;
			}
		}
		else {
			if ((input[66]) >= (1349.5)) {
				var75 = -0.008737802;
			}
			else {
				var75 = 0.01452551;
			}
		}
	}
	double var76;
	if ((input[57]) >= (2.34975)) {
		if ((input[47]) >= (0.86364996)) {
			if ((input[50]) >= (3.5)) {
				var76 = -0.039368868;
			}
			else {
				var76 = -0.006797734;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var76 = 0.021834036;
			}
			else {
				var76 = -0.022275925;
			}
		}
	}
	else {
		if ((input[66]) >= (4226.5)) {
			if ((input[58]) >= (74.8585)) {
				var76 = -0.010681379;
			}
			else {
				var76 = -0.047486987;
			}
		}
		else {
			if ((input[64]) >= (46.5)) {
				var76 = 0.05866136;
			}
			else {
				var76 = -0.0064180144;
			}
		}
	}
	double var77;
	if ((input[1]) >= (11.9911995)) {
		if ((input[57]) >= (2.8544002)) {
			if ((input[66]) >= (69986.0)) {
				var77 = -0.04925806;
			}
			else {
				var77 = 0.0388706;
			}
		}
		else {
			if ((input[3]) >= (0.55)) {
				var77 = -0.011789068;
			}
			else {
				var77 = -0.06092763;
			}
		}
	}
	else {
		if ((input[64]) >= (48.5)) {
			if ((input[0]) >= (1.5)) {
				var77 = -0.0061079822;
			}
			else {
				var77 = 0.05891188;
			}
		}
		else {
			if ((input[58]) >= (3.12105)) {
				var77 = 0.0037762213;
			}
			else {
				var77 = -0.020008245;
			}
		}
	}
	double var78;
	if ((input[12]) >= (0.0021000002)) {
		if ((input[58]) >= (8.973049)) {
			if ((input[71]) >= (7430.5)) {
				var78 = -0.011339958;
			}
			else {
				var78 = 0.013971251;
			}
		}
		else {
			if ((input[48]) >= (-0.5)) {
				var78 = -0.024852348;
			}
			else {
				var78 = 0.23305225;
			}
		}
	}
	else {
		if ((input[0]) >= (3.0)) {
			if ((input[2]) >= (0.5)) {
				var78 = 0.03840883;
			}
			else {
				var78 = -0.020111924;
			}
		}
		else {
			if ((input[70]) >= (2.5)) {
				var78 = -0.050068177;
			}
			else {
				var78 = 0.06561414;
			}
		}
	}
	double var79;
	if ((input[66]) >= (1037.5)) {
		if ((input[58]) >= (16.21435)) {
			if ((input[66]) >= (9260.5)) {
				var79 = -0.008977409;
			}
			else {
				var79 = 0.0134848;
			}
		}
		else {
			if ((input[64]) >= (49.5)) {
				var79 = 0.055522364;
			}
			else {
				var79 = -0.032211475;
			}
		}
	}
	else {
		if ((input[64]) >= (40.5)) {
			if ((input[70]) >= (4.5)) {
				var79 = -0.025193516;
			}
			else {
				var79 = 0.04175506;
			}
		}
		else {
			if ((input[58]) >= (15.875851)) {
				var79 = 0.04338684;
			}
			else {
				var79 = -0.016933518;
			}
		}
	}
	double var80;
	if ((input[64]) >= (42.5)) {
		if ((input[66]) >= (14187.5)) {
			if ((input[61]) >= (0.14975)) {
				var80 = 0.00088492985;
			}
			else {
				var80 = -0.038548645;
			}
		}
		else {
			if ((input[56]) >= (27.2838)) {
				var80 = 0.04549066;
			}
			else {
				var80 = 0.000058964037;
			}
		}
	}
	else {
		if ((input[70]) >= (1.5)) {
			if ((input[46]) >= (5.5844)) {
				var80 = 0.016302915;
			}
			else {
				var80 = -0.019200405;
			}
		}
		else {
			if ((input[71]) >= (487.5)) {
				var80 = -0.042791013;
			}
			else {
				var80 = 0.0143526895;
			}
		}
	}
	double var81;
	if ((input[12]) >= (2.25405)) {
		if ((input[46]) >= (6.16585)) {
			if ((input[61]) >= (0.6874)) {
				var81 = 0.109119914;
			}
			else {
				var81 = 0.0019985307;
			}
		}
		else {
			if ((input[64]) >= (45.5)) {
				var81 = 0.0018039698;
			}
			else {
				var81 = -0.036214974;
			}
		}
	}
	else {
		if ((input[64]) >= (50.5)) {
			if ((input[71]) >= (76882.0)) {
				var81 = -0.043455545;
			}
			else {
				var81 = 0.0777201;
			}
		}
		else {
			if ((input[66]) >= (3398.5)) {
				var81 = -0.008352993;
			}
			else {
				var81 = 0.01178414;
			}
		}
	}
	double var82;
	if ((input[53]) >= (2.5)) {
		if ((input[63]) >= (0.99995)) {
			if ((input[29]) >= (3.5)) {
				var82 = 0.098256074;
			}
			else {
				var82 = -0.07177515;
			}
		}
		else {
			if ((input[12]) >= (0.25524998)) {
				var82 = -0.02237848;
			}
			else {
				var82 = 0.13859837;
			}
		}
	}
	else {
		if ((input[66]) >= (1761.5)) {
			if ((input[61]) >= (0.11085)) {
				var82 = 0.005359326;
			}
			else {
				var82 = -0.017687835;
			}
		}
		else {
			if ((input[58]) >= (16.189949)) {
				var82 = 0.043195996;
			}
			else {
				var82 = 0.0022477123;
			}
		}
	}
	double var83;
	if ((input[64]) >= (45.5)) {
		if ((input[66]) >= (4935.5)) {
			if ((input[61]) >= (0.11075)) {
				var83 = 0.014038297;
			}
			else {
				var83 = -0.03245218;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var83 = -0.01788626;
			}
			else {
				var83 = 0.06430096;
			}
		}
	}
	else {
		if ((input[70]) >= (1.5)) {
			if ((input[26]) >= (6.675)) {
				var83 = -0.01868116;
			}
			else {
				var83 = 0.014195058;
			}
		}
		else {
			if ((input[71]) >= (495.5)) {
				var83 = -0.033081036;
			}
			else {
				var83 = 0.019140925;
			}
		}
	}
	double var84;
	if ((input[57]) >= (2.00425)) {
		if ((input[71]) >= (239681.5)) {
			if ((input[70]) >= (3.5)) {
				var84 = -0.0066145225;
			}
			else {
				var84 = -0.06388828;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var84 = 0.014168829;
			}
			else {
				var84 = -0.01726512;
			}
		}
	}
	else {
		if ((input[71]) >= (7268.5)) {
			if ((input[33]) >= (0.775)) {
				var84 = 0.011281633;
			}
			else {
				var84 = -0.045174472;
			}
		}
		else {
			if ((input[64]) >= (46.5)) {
				var84 = 0.036351413;
			}
			else {
				var84 = -0.009454112;
			}
		}
	}
	double var85;
	if ((input[61]) >= (0.72455)) {
		if ((input[47]) >= (1.65155)) {
			if ((input[20]) >= (1.5)) {
				var85 = 0.031294927;
			}
			else {
				var85 = -0.10102443;
			}
		}
		else {
			if ((input[22]) >= (21.0708)) {
				var85 = 0.1436327;
			}
			else {
				var85 = 0.044505123;
			}
		}
	}
	else {
		if ((input[49]) >= (8.625)) {
			if ((input[57]) >= (4.0938)) {
				var85 = 0.10755521;
			}
			else {
				var85 = -0.049303662;
			}
		}
		else {
			if ((input[71]) >= (76872.5)) {
				var85 = -0.026169116;
			}
			else {
				var85 = 0.0026892952;
			}
		}
	}
	double var86;
	if ((input[57]) >= (3.19115)) {
		if ((input[32]) >= (0.5)) {
			if ((input[70]) >= (2.5)) {
				var86 = 0.017974278;
			}
			else {
				var86 = -0.020576037;
			}
		}
		else {
			if ((input[71]) >= (2883.5)) {
				var86 = 0.019381009;
			}
			else {
				var86 = 0.0658186;
			}
		}
	}
	else {
		if ((input[71]) >= (71036.5)) {
			if ((input[70]) >= (3.5)) {
				var86 = -0.014570035;
			}
			else {
				var86 = -0.09626007;
			}
		}
		else {
			if ((input[64]) >= (49.5)) {
				var86 = 0.054149915;
			}
			else {
				var86 = -0.004173632;
			}
		}
	}
	double var87;
	if ((input[1]) >= (11.9911995)) {
		if ((input[55]) >= (60.3904)) {
			if ((input[66]) >= (1343.0)) {
				var87 = -0.03996506;
			}
			else {
				var87 = 0.03175659;
			}
		}
		else {
			if ((input[46]) >= (7.0284)) {
				var87 = 0.21796484;
			}
			else {
				var87 = -0.00082095125;
			}
		}
	}
	else {
		if ((input[64]) >= (43.5)) {
			if ((input[0]) >= (6.0)) {
				var87 = -0.022797896;
			}
			else {
				var87 = 0.017280623;
			}
		}
		else {
			if ((input[70]) >= (1.5)) {
				var87 = 0.0051374515;
			}
			else {
				var87 = -0.019666163;
			}
		}
	}
	double var88;
	if ((input[12]) >= (0.0021000002)) {
		if ((input[58]) >= (3.1220498)) {
			if ((input[70]) >= (0.5)) {
				var88 = 0.006169843;
			}
			else {
				var88 = -0.01376751;
			}
		}
		else {
			if ((input[48]) >= (-0.5)) {
				var88 = -0.030691242;
			}
			else {
				var88 = 0.22103451;
			}
		}
	}
	else {
		if ((input[0]) >= (3.0)) {
			if ((input[3]) >= (0.9)) {
				var88 = 0.046908878;
			}
			else {
				var88 = -0.015396482;
			}
		}
		else {
			if ((input[70]) >= (2.5)) {
				var88 = -0.04642375;
			}
			else {
				var88 = 0.06059866;
			}
		}
	}
	double var89;
	if ((input[71]) >= (-1756.0)) {
		if ((input[66]) >= (292.5)) {
			if ((input[58]) >= (16.21435)) {
				var89 = 0.004562281;
			}
			else {
				var89 = -0.015901184;
			}
		}
		else {
			if ((input[64]) >= (40.5)) {
				var89 = 0.0575384;
			}
			else {
				var89 = -0.004458166;
			}
		}
	}
	else {
		if ((input[32]) >= (0.5)) {
			if ((input[22]) >= (1.96155)) {
				var89 = -0.036059283;
			}
			else {
				var89 = 0.08404657;
			}
		}
		else {
			if ((input[1]) >= (0.081650004)) {
				var89 = -0.16302672;
			}
			else {
				var89 = 0.025193451;
			}
		}
	}
	double var90;
	if ((input[66]) >= (3406.5)) {
		if ((input[62]) >= (34.26435)) {
			if ((input[70]) >= (2.5)) {
				var90 = 0.011648482;
			}
			else {
				var90 = -0.013382207;
			}
		}
		else {
			if ((input[64]) >= (50.5)) {
				var90 = 0.051021006;
			}
			else {
				var90 = -0.03851671;
			}
		}
	}
	else {
		if ((input[64]) >= (45.5)) {
			if ((input[0]) >= (6.0)) {
				var90 = -0.0083412975;
			}
			else {
				var90 = 0.06385233;
			}
		}
		else {
			if ((input[58]) >= (16.21435)) {
				var90 = 0.0240647;
			}
			else {
				var90 = -0.0102320155;
			}
		}
	}
	double var91;
	if ((input[64]) >= (48.5)) {
		if ((input[0]) >= (1.5)) {
			if ((input[71]) >= (34878.0)) {
				var91 = -0.048393186;
			}
			else {
				var91 = 0.0052633304;
			}
		}
		else {
			if ((input[66]) >= (55081.0)) {
				var91 = -0.012760597;
			}
			else {
				var91 = 0.07997685;
			}
		}
	}
	else {
		if ((input[66]) >= (1223.5)) {
			if ((input[33]) >= (0.45)) {
				var91 = 0.0044298875;
			}
			else {
				var91 = -0.022552252;
			}
		}
		else {
			if ((input[58]) >= (15.55715)) {
				var91 = 0.04827482;
			}
			else {
				var91 = 0.00026823432;
			}
		}
	}
	double var92;
	if ((input[71]) >= (-755.5)) {
		if ((input[64]) >= (43.5)) {
			if ((input[66]) >= (1610.5)) {
				var92 = 0.0016020903;
			}
			else {
				var92 = 0.04553687;
			}
		}
		else {
			if ((input[22]) >= (10.977301)) {
				var92 = -0.02392038;
			}
			else {
				var92 = -0.0003395409;
			}
		}
	}
	else {
		if ((input[57]) >= (3.3031502)) {
			if ((input[70]) >= (7.5)) {
				var92 = -0.061969288;
			}
			else {
				var92 = 0.077976346;
			}
		}
		else {
			if ((input[70]) >= (13.5)) {
				var92 = 0.09963954;
			}
			else {
				var92 = -0.09001302;
			}
		}
	}
	double var93;
	if ((input[28]) >= (0.5)) {
		if ((input[55]) >= (45.5831)) {
			if ((input[46]) >= (4.7154503)) {
				var93 = 0.002236032;
			}
			else {
				var93 = -0.020962862;
			}
		}
		else {
			if ((input[24]) >= (1.1)) {
				var93 = 0.065747686;
			}
			else {
				var93 = 0.009403648;
			}
		}
	}
	else {
		if ((input[70]) >= (8.5)) {
			if ((input[51]) >= (1.5)) {
				var93 = -0.01572967;
			}
			else {
				var93 = -0.050099958;
			}
		}
		else {
			if ((input[57]) >= (3.7625499)) {
				var93 = 0.046018835;
			}
			else {
				var93 = -0.010204267;
			}
		}
	}
	double var94;
	if ((input[1]) >= (0.0138)) {
		if ((input[60]) >= (3.0084)) {
			if ((input[46]) >= (3.34635)) {
				var94 = 0.003824543;
			}
			else {
				var94 = -0.06206559;
			}
		}
		else {
			if ((input[48]) >= (-0.5)) {
				var94 = -0.01652193;
			}
			else {
				var94 = 0.19353162;
			}
		}
	}
	else {
		if ((input[0]) >= (3.0)) {
			if ((input[2]) >= (0.5)) {
				var94 = 0.028513575;
			}
			else {
				var94 = -0.02004611;
			}
		}
		else {
			if ((input[70]) >= (2.5)) {
				var94 = -0.031271633;
			}
			else {
				var94 = 0.056634665;
			}
		}
	}
	double var95;
	if ((input[66]) >= (9273.0)) {
		if ((input[70]) >= (2.5)) {
			if ((input[71]) >= (4915.5)) {
				var95 = 0.008988481;
			}
			else {
				var95 = -0.06309155;
			}
		}
		else {
			if ((input[64]) >= (48.5)) {
				var95 = -0.0004374899;
			}
			else {
				var95 = -0.03881703;
			}
		}
	}
	else {
		if ((input[64]) >= (43.5)) {
			if ((input[56]) >= (31.04915)) {
				var95 = 0.054552734;
			}
			else {
				var95 = 0.0030913036;
			}
		}
		else {
			if ((input[46]) >= (5.55655)) {
				var95 = 0.005426431;
			}
			else {
				var95 = -0.017203145;
			}
		}
	}
	double var96;
	if ((input[66]) >= (925.5)) {
		if ((input[58]) >= (39.0664)) {
			if ((input[66]) >= (6181.5)) {
				var96 = -0.0046340977;
			}
			else {
				var96 = 0.0215241;
			}
		}
		else {
			if ((input[64]) >= (50.5)) {
				var96 = 0.05414899;
			}
			else {
				var96 = -0.019702567;
			}
		}
	}
	else {
		if ((input[64]) >= (38.5)) {
			if ((input[56]) >= (7.2367496)) {
				var96 = 0.05294941;
			}
			else {
				var96 = 0.0048093386;
			}
		}
		else {
			if ((input[70]) >= (1.5)) {
				var96 = 0.005699728;
			}
			else {
				var96 = -0.041597616;
			}
		}
	}
	double var97;
	if ((input[71]) >= (-755.5)) {
		if ((input[66]) >= (292.5)) {
			if ((input[56]) >= (2.4657001)) {
				var97 = 0.001766112;
			}
			else {
				var97 = -0.021626031;
			}
		}
		else {
			if ((input[64]) >= (40.5)) {
				var97 = 0.057820678;
			}
			else {
				var97 = -0.00054095936;
			}
		}
	}
	else {
		if ((input[57]) >= (3.3031502)) {
			if ((input[71]) >= (-4103.0)) {
				var97 = 0.06912954;
			}
			else {
				var97 = -0.068313636;
			}
		}
		else {
			if ((input[66]) >= (-2105.5)) {
				var97 = -0.05603099;
			}
			else {
				var97 = -0.1498492;
			}
		}
	}
	double var98;
	if ((input[61]) >= (0.72455)) {
		if ((input[47]) >= (1.5)) {
			if ((input[12]) >= (4.5298004)) {
				var98 = 0.036621068;
			}
			else {
				var98 = -0.099924676;
			}
		}
		else {
			if ((input[14]) >= (1.775)) {
				var98 = 0.16446055;
			}
			else {
				var98 = 0.043770712;
			}
		}
	}
	else {
		if ((input[66]) >= (5021.5)) {
			if ((input[62]) >= (34.25085)) {
				var98 = -0.003990087;
			}
			else {
				var98 = -0.034845557;
			}
		}
		else {
			if ((input[64]) >= (46.5)) {
				var98 = 0.04509693;
			}
			else {
				var98 = 0.00055410346;
			}
		}
	}
	double var99;
	if ((input[12]) >= (1.59045)) {
		if ((input[46]) >= (5.5793)) {
			if ((input[71]) >= (241537.0)) {
				var99 = -0.030781686;
			}
			else {
				var99 = 0.003655176;
			}
		}
		else {
			if ((input[64]) >= (45.5)) {
				var99 = 0.00079881557;
			}
			else {
				var99 = -0.033707224;
			}
		}
	}
	else {
		if ((input[3]) >= (2.63335)) {
			if ((input[33]) >= (1.70835)) {
				var99 = 0.018848637;
			}
			else {
				var99 = 0.18810214;
			}
		}
		else {
			if ((input[71]) >= (2438.5)) {
				var99 = -0.005694629;
			}
			else {
				var99 = 0.011675465;
			}
		}
	}
	double var100;
	var100 = (1.0) / ((1.0) + (exp((0.0) - ((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((var0)+(var1)) + (var2)) + (var3)) + (var4)) + (var5)) + (var6)) + (var7)) + (var8)) + (var9)) + (var10)) + (var11)) + (var12)) + (var13)) + (var14)) + (var15)) + (var16)) + (var17)) + (var18)) + (var19)) + (var20)) + (var21)) + (var22)) + (var23)) + (var24)) + (var25)) + (var26)) + (var27)) + (var28)) + (var29)) + (var30)) + (var31)) + (var32)) + (var33)) + (var34)) + (var35)) + (var36)) + (var37)) + (var38)) + (var39)) + (var40)) + (var41)) + (var42)) + (var43)) + (var44)) + (var45)) + (var46)) + (var47)) + (var48)) + (var49)) + (var50)) + (var51)) + (var52)) + (var53)) + (var54)) + (var55)) + (var56)) + (var57)) + (var58)) + (var59)) + (var60)) + (var61)) + (var62)) + (var63)) + (var64)) + (var65)) + (var66)) + (var67)) + (var68)) + (var69)) + (var70)) + (var71)) + (var72)) + (var73)) + (var74)) + (var75)) + (var76)) + (var77)) + (var78)) + (var79)) + (var80)) + (var81)) + (var82)) + (var83)) + (var84)) + (var85)) + (var86)) + (var87)) + (var88)) + (var89)) + (var90)) + (var91)) + (var92)) + (var93)) + (var94)) + (var95)) + (var96)) + (var97)) + (var98)) + (var99)))));
	*output = var100 < 0.5 ? 0 : 1;
}

void PUclf6(double * input, double * output) {
	double var0;
	if ((input[70]) >= (5.5)) {
		if ((input[49]) >= (4.29165)) {
			if ((input[49]) >= (6.29165)) {
				var0 = -0.17240322;
			}
			else {
				var0 = -0.11686778;
			}
		}
		else {
			if ((input[26]) >= (4.45)) {
				var0 = -0.07700355;
			}
			else {
				var0 = -0.0015719974;
			}
		}
	}
	else {
		if ((input[49]) >= (4.41665)) {
			if ((input[70]) >= (1.5)) {
				var0 = -0.0722652;
			}
			else {
				var0 = 0.04845674;
			}
		}
		else {
			if ((input[71]) >= (2924.5)) {
				var0 = 0.04845486;
			}
			else {
				var0 = 0.13897122;
			}
		}
	}
	double var1;
	if ((input[70]) >= (1.5)) {
		if ((input[49]) >= (4.29165)) {
			if ((input[70]) >= (7.5)) {
				var1 = -0.14893846;
			}
			else {
				var1 = -0.07404138;
			}
		}
		else {
			if ((input[70]) >= (7.5)) {
				var1 = -0.047278326;
			}
			else {
				var1 = 0.039627653;
			}
		}
	}
	else {
		if ((input[71]) >= (1304.5)) {
			if ((input[26]) >= (5.775)) {
				var1 = -0.018137842;
			}
			else {
				var1 = 0.07810906;
			}
		}
		else {
			if ((input[64]) >= (37.5)) {
				var1 = 0.15302506;
			}
			else {
				var1 = 0.046723377;
			}
		}
	}
	double var2;
	if ((input[70]) >= (1.5)) {
		if ((input[49]) >= (4.29165)) {
			if ((input[70]) >= (7.5)) {
				var2 = -0.1364931;
			}
			else {
				var2 = -0.06697738;
			}
		}
		else {
			if ((input[67]) >= (0.5)) {
				var2 = 0.03570979;
			}
			else {
				var2 = -0.0438751;
			}
		}
	}
	else {
		if ((input[71]) >= (907.5)) {
			if ((input[49]) >= (2.625)) {
				var2 = -0.004268553;
			}
			else {
				var2 = 0.07777845;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var2 = 0.15029772;
			}
			else {
				var2 = 0.091424726;
			}
		}
	}
	double var3;
	if ((input[70]) >= (4.5)) {
		if ((input[26]) >= (6.45)) {
			if ((input[49]) >= (5.625)) {
				var3 = -0.1401171;
			}
			else {
				var3 = -0.082482874;
			}
		}
		else {
			if ((input[51]) >= (2.5)) {
				var3 = 0.019326737;
			}
			else {
				var3 = -0.05280252;
			}
		}
	}
	else {
		if ((input[49]) >= (2.875)) {
			if ((input[49]) >= (6.70835)) {
				var3 = -0.060607523;
			}
			else {
				var3 = 0.0192068;
			}
		}
		else {
			if ((input[71]) >= (2852.5)) {
				var3 = 0.03949004;
			}
			else {
				var3 = 0.114550434;
			}
		}
	}
	double var4;
	if ((input[70]) >= (1.5)) {
		if ((input[51]) >= (1.5)) {
			if ((input[26]) >= (2.1)) {
				var4 = -0.03163557;
			}
			else {
				var4 = 0.035592295;
			}
		}
		else {
			if ((input[26]) >= (5.325)) {
				var4 = -0.11477023;
			}
			else {
				var4 = -0.046417996;
			}
		}
	}
	else {
		if ((input[71]) >= (827.5)) {
			if ((input[26]) >= (5.775)) {
				var4 = -0.01954193;
			}
			else {
				var4 = 0.06517575;
			}
		}
		else {
			if ((input[64]) >= (37.5)) {
				var4 = 0.12649599;
			}
			else {
				var4 = 0.030009583;
			}
		}
	}
	double var5;
	if ((input[70]) >= (6.5)) {
		if ((input[49]) >= (4.29165)) {
			if ((input[16]) >= (8.9)) {
				var5 = -0.12150254;
			}
			else {
				var5 = -0.07219159;
			}
		}
		else {
			if ((input[27]) >= (1.5)) {
				var5 = -0.01014412;
			}
			else {
				var5 = -0.06445983;
			}
		}
	}
	else {
		if ((input[51]) >= (2.5)) {
			if ((input[71]) >= (1776.5)) {
				var5 = 0.039131824;
			}
			else {
				var5 = 0.104202084;
			}
		}
		else {
			if ((input[70]) >= (1.5)) {
				var5 = -0.031704143;
			}
			else {
				var5 = 0.042493884;
			}
		}
	}
	double var6;
	if ((input[70]) >= (2.5)) {
		if ((input[26]) >= (6.45)) {
			if ((input[49]) >= (6.58335)) {
				var6 = -0.11544591;
			}
			else {
				var6 = -0.061055362;
			}
		}
		else {
			if ((input[49]) >= (2.125)) {
				var6 = -0.031190416;
			}
			else {
				var6 = 0.028119737;
			}
		}
	}
	else {
		if ((input[71]) >= (189.5)) {
			if ((input[26]) >= (3.775)) {
				var6 = -0.015490117;
			}
			else {
				var6 = 0.06650252;
			}
		}
		else {
			if ((input[64]) >= (37.5)) {
				var6 = 0.12219514;
			}
			else {
				var6 = 0.03588005;
			}
		}
	}
	double var7;
	if ((input[70]) >= (7.5)) {
		if ((input[49]) >= (6.125)) {
			if ((input[57]) >= (2.07535)) {
				var7 = -0.08980086;
			}
			else {
				var7 = -0.12352015;
			}
		}
		else {
			if ((input[27]) >= (1.5)) {
				var7 = -0.02210852;
			}
			else {
				var7 = -0.06836099;
			}
		}
	}
	else {
		if ((input[49]) >= (2.125)) {
			if ((input[51]) >= (1.5)) {
				var7 = 0.022479866;
			}
			else {
				var7 = -0.042663038;
			}
		}
		else {
			if ((input[71]) >= (2718.5)) {
				var7 = 0.024077743;
			}
			else {
				var7 = 0.09409019;
			}
		}
	}
	double var8;
	if ((input[70]) >= (1.5)) {
		if ((input[26]) >= (6.45)) {
			if ((input[49]) >= (6.58335)) {
				var8 = -0.10010274;
			}
			else {
				var8 = -0.047847763;
			}
		}
		else {
			if ((input[70]) >= (10.5)) {
				var8 = -0.04045005;
			}
			else {
				var8 = 0.01884663;
			}
		}
	}
	else {
		if ((input[71]) >= (189.5)) {
			if ((input[28]) >= (1.5)) {
				var8 = 0.06144458;
			}
			else {
				var8 = -0.008668999;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var8 = 0.1256144;
			}
			else {
				var8 = 0.06798099;
			}
		}
	}
	double var9;
	if ((input[51]) >= (1.5)) {
		if ((input[70]) >= (1.5)) {
			if ((input[51]) >= (3.5)) {
				var9 = 0.03802436;
			}
			else {
				var9 = -0.013512417;
			}
		}
		else {
			if ((input[71]) >= (3056.5)) {
				var9 = 0.022240162;
			}
			else {
				var9 = 0.08810056;
			}
		}
	}
	else {
		if ((input[70]) >= (8.5)) {
			if ((input[57]) >= (2.19635)) {
				var9 = -0.0671302;
			}
			else {
				var9 = -0.11175276;
			}
		}
		else {
			if ((input[56]) >= (1.1870999)) {
				var9 = -0.011953951;
			}
			else {
				var9 = -0.08193082;
			}
		}
	}
	double var10;
	if ((input[70]) >= (5.5)) {
		if ((input[49]) >= (5.41665)) {
			if ((input[57]) >= (1.78355)) {
				var10 = -0.07141035;
			}
			else {
				var10 = -0.11019527;
			}
		}
		else {
			if ((input[16]) >= (6.675)) {
				var10 = -0.047881305;
			}
			else {
				var10 = -0.0018001177;
			}
		}
	}
	else {
		if ((input[26]) >= (2.1)) {
			if ((input[66]) >= (18.5)) {
				var10 = -0.008937961;
			}
			else {
				var10 = 0.108483076;
			}
		}
		else {
			if ((input[71]) >= (1814.5)) {
				var10 = 0.02768741;
			}
			else {
				var10 = 0.08627162;
			}
		}
	}
	double var11;
	if ((input[51]) >= (2.5)) {
		if ((input[70]) >= (3.5)) {
			if ((input[57]) >= (1.6171501)) {
				var11 = 0.018997192;
			}
			else {
				var11 = -0.028399909;
			}
		}
		else {
			if ((input[71]) >= (3965.0)) {
				var11 = 0.019793386;
			}
			else {
				var11 = 0.08055347;
			}
		}
	}
	else {
		if ((input[70]) >= (8.5)) {
			if ((input[57]) >= (1.8499501)) {
				var11 = -0.0542602;
			}
			else {
				var11 = -0.101839;
			}
		}
		else {
			if ((input[16]) >= (6.9)) {
				var11 = -0.03337139;
			}
			else {
				var11 = 0.017802319;
			}
		}
	}
	double var12;
	if ((input[49]) >= (4.29165)) {
		if ((input[70]) >= (8.5)) {
			if ((input[57]) >= (1.9491)) {
				var12 = -0.05671499;
			}
			else {
				var12 = -0.10072114;
			}
		}
		else {
			if ((input[58]) >= (0.59145)) {
				var12 = -0.0097966315;
			}
			else {
				var12 = -0.08677255;
			}
		}
	}
	else {
		if ((input[70]) >= (1.5)) {
			if ((input[51]) >= (3.5)) {
				var12 = 0.031480603;
			}
			else {
				var12 = -0.01115025;
			}
		}
		else {
			if ((input[71]) >= (1425.5)) {
				var12 = 0.026670752;
			}
			else {
				var12 = 0.08268222;
			}
		}
	}
	double var13;
	if ((input[26]) >= (4.55)) {
		if ((input[70]) >= (2.5)) {
			if ((input[49]) >= (6.58335)) {
				var13 = -0.08268797;
			}
			else {
				var13 = -0.034695048;
			}
		}
		else {
			if ((input[71]) >= (69.5)) {
				var13 = -0.017780552;
			}
			else {
				var13 = 0.08238481;
			}
		}
	}
	else {
		if ((input[70]) >= (3.5)) {
			if ((input[57]) >= (1.0109)) {
				var13 = 0.009578847;
			}
			else {
				var13 = -0.064341165;
			}
		}
		else {
			if ((input[71]) >= (896.5)) {
				var13 = 0.024551325;
			}
			else {
				var13 = 0.083156906;
			}
		}
	}
	double var14;
	if ((input[49]) >= (2.125)) {
		if ((input[70]) >= (7.5)) {
			if ((input[57]) >= (1.57955)) {
				var14 = -0.043379217;
			}
			else {
				var14 = -0.09179353;
			}
		}
		else {
			if ((input[66]) >= (18.5)) {
				var14 = -0.010837212;
			}
			else {
				var14 = 0.094380766;
			}
		}
	}
	else {
		if ((input[66]) >= (2721.5)) {
			if ((input[64]) >= (43.5)) {
				var14 = 0.027030021;
			}
			else {
				var14 = -0.016123489;
			}
		}
		else {
			if ((input[70]) >= (0.5)) {
				var14 = 0.046169233;
			}
			else {
				var14 = 0.09844463;
			}
		}
	}
	double var15;
	if ((input[28]) >= (1.5)) {
		if ((input[12]) >= (0.96140003)) {
			if ((input[57]) >= (0.36575)) {
				var15 = 0.009154371;
			}
			else {
				var15 = -0.092280515;
			}
		}
		else {
			if ((input[70]) >= (1.5)) {
				var15 = 0.032431196;
			}
			else {
				var15 = 0.07341119;
			}
		}
	}
	else {
		if ((input[51]) >= (0.5)) {
			if ((input[66]) >= (74.5)) {
				var15 = -0.029492786;
			}
			else {
				var15 = 0.05329029;
			}
		}
		else {
			if ((input[70]) >= (10.5)) {
				var15 = -0.09192316;
			}
			else {
				var15 = -0.054350007;
			}
		}
	}
	double var16;
	if ((input[51]) >= (1.5)) {
		if ((input[66]) >= (116.5)) {
			if ((input[26]) >= (2.1)) {
				var16 = -0.013149123;
			}
			else {
				var16 = 0.027187282;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var16 = 0.10337508;
			}
			else {
				var16 = 0.040261384;
			}
		}
	}
	else {
		if ((input[70]) >= (10.5)) {
			if ((input[57]) >= (2.41425)) {
				var16 = -0.042556714;
			}
			else {
				var16 = -0.08910244;
			}
		}
		else {
			if ((input[58]) >= (1.0345)) {
				var16 = -0.008562881;
			}
			else {
				var16 = -0.07495987;
			}
		}
	}
	double var17;
	if ((input[16]) >= (6.1)) {
		if ((input[70]) >= (3.5)) {
			if ((input[57]) >= (1.5813501)) {
				var17 = -0.029795527;
			}
			else {
				var17 = -0.07581689;
			}
		}
		else {
			if ((input[71]) >= (77.5)) {
				var17 = -0.010274734;
			}
			else {
				var17 = 0.056671828;
			}
		}
	}
	else {
		if ((input[66]) >= (1364.5)) {
			if ((input[64]) >= (43.5)) {
				var17 = 0.026758347;
			}
			else {
				var17 = -0.008468617;
			}
		}
		else {
			if ((input[70]) >= (4.5)) {
				var17 = 0.00906837;
			}
			else {
				var17 = 0.06676815;
			}
		}
	}
	double var18;
	if ((input[49]) >= (6.58335)) {
		if ((input[70]) >= (10.5)) {
			if ((input[57]) >= (1.95705)) {
				var18 = -0.056892194;
			}
			else {
				var18 = -0.09417592;
			}
		}
		else {
			if ((input[57]) >= (0.40545)) {
				var18 = -0.019196875;
			}
			else {
				var18 = -0.093222834;
			}
		}
	}
	else {
		if ((input[12]) >= (0.5581)) {
			if ((input[51]) >= (3.5)) {
				var18 = 0.029060563;
			}
			else {
				var18 = -0.01269412;
			}
		}
		else {
			if ((input[70]) >= (0.5)) {
				var18 = 0.034292333;
			}
			else {
				var18 = 0.08128218;
			}
		}
	}
	double var19;
	if ((input[28]) >= (0.5)) {
		if ((input[12]) >= (2.5483499)) {
			if ((input[57]) >= (0.55055)) {
				var19 = -0.0011992109;
			}
			else {
				var19 = -0.08430327;
			}
		}
		else {
			if ((input[70]) >= (1.5)) {
				var19 = 0.018238397;
			}
			else {
				var19 = 0.057323404;
			}
		}
	}
	else {
		if ((input[66]) >= (45.5)) {
			if ((input[50]) >= (0.5)) {
				var19 = -0.030673442;
			}
			else {
				var19 = -0.07694169;
			}
		}
		else {
			if ((input[56]) >= (0.38605)) {
				var19 = 0.08056635;
			}
			else {
				var19 = -0.008453577;
			}
		}
	}
	double var20;
	if ((input[70]) >= (10.5)) {
		if ((input[27]) >= (0.5)) {
			if ((input[57]) >= (1.6150501)) {
				var20 = -0.012664949;
			}
			else {
				var20 = -0.05594005;
			}
		}
		else {
			if ((input[57]) >= (1.3310001)) {
				var20 = -0.054938298;
			}
			else {
				var20 = -0.09453316;
			}
		}
	}
	else {
		if ((input[51]) >= (3.5)) {
			if ((input[71]) >= (2695.5)) {
				var20 = 0.012551804;
			}
			else {
				var20 = 0.060194463;
			}
		}
		else {
			if ((input[4]) >= (1.5)) {
				var20 = -0.013563714;
			}
			else {
				var20 = 0.022215532;
			}
		}
	}
	double var21;
	if ((input[70]) >= (3.5)) {
		if ((input[58]) >= (0.9671)) {
			if ((input[70]) >= (12.5)) {
				var21 = -0.037141774;
			}
			else {
				var21 = 0.0020399704;
			}
		}
		else {
			if ((input[66]) >= (24.5)) {
				var21 = -0.0946038;
			}
			else {
				var21 = 0.033489835;
			}
		}
	}
	else {
		if ((input[71]) >= (127.5)) {
			if ((input[57]) >= (0.49844998)) {
				var21 = 0.015241238;
			}
			else {
				var21 = -0.078541234;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var21 = 0.08824018;
			}
			else {
				var21 = 0.029530156;
			}
		}
	}
	double var22;
	if ((input[7]) >= (1.5)) {
		if ((input[66]) >= (3820.5)) {
			if ((input[64]) >= (43.5)) {
				var22 = 0.015653647;
			}
			else {
				var22 = -0.019026797;
			}
		}
		else {
			if ((input[70]) >= (0.5)) {
				var22 = 0.029207988;
			}
			else {
				var22 = 0.080297686;
			}
		}
	}
	else {
		if ((input[51]) >= (0.5)) {
			if ((input[66]) >= (91.5)) {
				var22 = -0.016103435;
			}
			else {
				var22 = 0.045759555;
			}
		}
		else {
			if ((input[57]) >= (1.57735)) {
				var22 = -0.03771644;
			}
			else {
				var22 = -0.07738654;
			}
		}
	}
	double var23;
	if ((input[49]) >= (2.125)) {
		if ((input[57]) >= (1.0121)) {
			if ((input[70]) >= (13.5)) {
				var23 = -0.038291916;
			}
			else {
				var23 = 0.0022284158;
			}
		}
		else {
			if ((input[66]) >= (18.5)) {
				var23 = -0.060274508;
			}
			else {
				var23 = 0.069696955;
			}
		}
	}
	else {
		if ((input[66]) >= (2721.5)) {
			if ((input[64]) >= (43.5)) {
				var23 = 0.017441278;
			}
			else {
				var23 = -0.015961444;
			}
		}
		else {
			if ((input[70]) >= (0.5)) {
				var23 = 0.029889166;
			}
			else {
				var23 = 0.07840472;
			}
		}
	}
	double var24;
	if ((input[5]) >= (8.225)) {
		if ((input[66]) >= (18.5)) {
			if ((input[50]) >= (0.5)) {
				var24 = -0.019132566;
			}
			else {
				var24 = -0.06365249;
			}
		}
		else {
			if ((input[22]) >= (34.153847)) {
				var24 = -0.037311994;
			}
			else {
				var24 = 0.07850032;
			}
		}
	}
	else {
		if ((input[1]) >= (2.9819)) {
			if ((input[57]) >= (1.00055)) {
				var24 = 0.0015307886;
			}
			else {
				var24 = -0.05563118;
			}
		}
		else {
			if ((input[66]) >= (4080.0)) {
				var24 = 0.005405584;
			}
			else {
				var24 = 0.039644953;
			}
		}
	}
	double var25;
	if ((input[18]) >= (1.5)) {
		if ((input[66]) >= (620.5)) {
			if ((input[58]) >= (9.81415)) {
				var25 = 0.012380899;
			}
			else {
				var25 = -0.044799596;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var25 = 0.06882351;
			}
			else {
				var25 = 0.0048931492;
			}
		}
	}
	else {
		if ((input[1]) >= (1.8657)) {
			if ((input[56]) >= (6.29445)) {
				var25 = -0.021413906;
			}
			else {
				var25 = -0.06568627;
			}
		}
		else {
			if ((input[62]) >= (3.95015)) {
				var25 = 0.022337388;
			}
			else {
				var25 = -0.018235339;
			}
		}
	}
	double var26;
	if ((input[70]) >= (4.5)) {
		if ((input[57]) >= (1.7665501)) {
			if ((input[12]) >= (7.08495)) {
				var26 = -0.02495454;
			}
			else {
				var26 = 0.009258502;
			}
		}
		else {
			if ((input[58]) >= (0.62855005)) {
				var26 = -0.03592616;
			}
			else {
				var26 = -0.09307758;
			}
		}
	}
	else {
		if ((input[71]) >= (70.5)) {
			if ((input[58]) >= (6.9007998)) {
				var26 = 0.01695515;
			}
			else {
				var26 = -0.021928275;
			}
		}
		else {
			if ((input[70]) >= (2.5)) {
				var26 = 0.011895549;
			}
			else {
				var26 = 0.07287168;
			}
		}
	}
	double var27;
	if ((input[70]) >= (0.5)) {
		if ((input[12]) >= (6.9966)) {
			if ((input[57]) >= (1.5066)) {
				var27 = -0.019472657;
			}
			else {
				var27 = -0.06202781;
			}
		}
		else {
			if ((input[56]) >= (4.84405)) {
				var27 = 0.013653859;
			}
			else {
				var27 = -0.020243745;
			}
		}
	}
	else {
		if ((input[66]) >= (4468.5)) {
			if ((input[64]) >= (41.5)) {
				var27 = 0.020058155;
			}
			else {
				var27 = -0.06299167;
			}
		}
		else {
			if ((input[64]) >= (43.5)) {
				var27 = 0.10059595;
			}
			else {
				var27 = 0.04839936;
			}
		}
	}
	double var28;
	if ((input[26]) >= (0.9)) {
		if ((input[49]) >= (6.875)) {
			if ((input[1]) >= (9.74755)) {
				var28 = -0.07117987;
			}
			else {
				var28 = -0.034438286;
			}
		}
		else {
			if ((input[66]) >= (91.5)) {
				var28 = -0.010035321;
			}
			else {
				var28 = 0.03926829;
			}
		}
	}
	else {
		if ((input[66]) >= (3036.5)) {
			if ((input[64]) >= (44.5)) {
				var28 = 0.02182244;
			}
			else {
				var28 = -0.014115204;
			}
		}
		else {
			if ((input[61]) >= (0.06025)) {
				var28 = 0.061989285;
			}
			else {
				var28 = 0.02269721;
			}
		}
	}
	double var29;
	if ((input[15]) >= (0.5)) {
		if ((input[51]) >= (0.5)) {
			if ((input[66]) >= (47.5)) {
				var29 = -0.008635415;
			}
			else {
				var29 = 0.047762413;
			}
		}
		else {
			if ((input[58]) >= (0.62855005)) {
				var29 = -0.03419861;
			}
			else {
				var29 = -0.085687265;
			}
		}
	}
	else {
		if ((input[66]) >= (104.5)) {
			if ((input[61]) >= (0.03085)) {
				var29 = 0.01912193;
			}
			else {
				var29 = -0.015699098;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var29 = 0.08758738;
			}
			else {
				var29 = 0.030056259;
			}
		}
	}
	double var30;
	if ((input[12]) >= (0.53470004)) {
		if ((input[56]) >= (6.2962)) {
			if ((input[66]) >= (1445.5)) {
				var30 = -0.009101414;
			}
			else {
				var30 = 0.026945507;
			}
		}
		else {
			if ((input[66]) >= (55.5)) {
				var30 = -0.04719387;
			}
			else {
				var30 = 0.027491817;
			}
		}
	}
	else {
		if ((input[70]) >= (0.5)) {
			if ((input[62]) >= (4.76475)) {
				var30 = 0.03252262;
			}
			else {
				var30 = 0.000974877;
			}
		}
		else {
			if ((input[66]) >= (690.5)) {
				var30 = 0.021484911;
			}
			else {
				var30 = 0.07717427;
			}
		}
	}
	double var31;
	if ((input[70]) >= (10.5)) {
		if ((input[0]) >= (0.5)) {
			if ((input[24]) >= (0.9)) {
				var31 = -0.0014340468;
			}
			else {
				var31 = -0.046419024;
			}
		}
		else {
			if ((input[49]) >= (5.375)) {
				var31 = -0.069078356;
			}
			else {
				var31 = -0.029976616;
			}
		}
	}
	else {
		if ((input[71]) >= (5728.5)) {
			if ((input[64]) >= (45.5)) {
				var31 = 0.0059641483;
			}
			else {
				var31 = -0.025498768;
			}
		}
		else {
			if ((input[56]) >= (7.67135)) {
				var31 = 0.032651912;
			}
			else {
				var31 = -0.0038595402;
			}
		}
	}
	double var32;
	if ((input[70]) >= (0.5)) {
		if ((input[12]) >= (3.7837)) {
			if ((input[57]) >= (1.34005)) {
				var32 = -0.011405492;
			}
			else {
				var32 = -0.05316774;
			}
		}
		else {
			if ((input[56]) >= (3.2044501)) {
				var32 = 0.014227924;
			}
			else {
				var32 = -0.015433149;
			}
		}
	}
	else {
		if ((input[66]) >= (5395.5)) {
			if ((input[64]) >= (41.5)) {
				var32 = 0.012892074;
			}
			else {
				var32 = -0.057932585;
			}
		}
		else {
			if ((input[64]) >= (43.5)) {
				var32 = 0.094399914;
			}
			else {
				var32 = 0.036161553;
			}
		}
	}
	double var33;
	if ((input[1]) >= (0.21435)) {
		if ((input[57]) >= (0.33315)) {
			if ((input[5]) >= (8.225)) {
				var33 = -0.018486569;
			}
			else {
				var33 = 0.0059749037;
			}
		}
		else {
			if ((input[66]) >= (19.5)) {
				var33 = -0.07011407;
			}
			else {
				var33 = 0.049268354;
			}
		}
	}
	else {
		if ((input[70]) >= (2.5)) {
			if ((input[56]) >= (0.35145)) {
				var33 = 0.021938782;
			}
			else {
				var33 = -0.074155465;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var33 = 0.06428462;
			}
			else {
				var33 = 0.0051680747;
			}
		}
	}
	double var34;
	if ((input[66]) >= (55.5)) {
		if ((input[27]) >= (0.5)) {
			if ((input[57]) >= (0.19929999)) {
				var34 = 0.006459601;
			}
			else {
				var34 = -0.051344223;
			}
		}
		else {
			if ((input[49]) >= (8.375)) {
				var34 = -0.051919457;
			}
			else {
				var34 = -0.016918505;
			}
		}
	}
	else {
		if ((input[64]) >= (38.5)) {
			if ((input[71]) >= (-19.5)) {
				var34 = 0.103512526;
			}
			else {
				var34 = 0.054421213;
			}
		}
		else {
			if ((input[56]) >= (0.27685)) {
				var34 = 0.054418396;
			}
			else {
				var34 = -0.01951028;
			}
		}
	}
	double var35;
	if ((input[1]) >= (3.8608499)) {
		if ((input[57]) >= (1.32355)) {
			if ((input[70]) >= (16.5)) {
				var35 = -0.030545404;
			}
			else {
				var35 = 0.0003324365;
			}
		}
		else {
			if ((input[51]) >= (1.5)) {
				var35 = -0.024449337;
			}
			else {
				var35 = -0.066097945;
			}
		}
	}
	else {
		if ((input[64]) >= (43.5)) {
			if ((input[66]) >= (10201.5)) {
				var35 = 0.0061675287;
			}
			else {
				var35 = 0.057457626;
			}
		}
		else {
			if ((input[12]) >= (0.0747)) {
				var35 = -0.0032723595;
			}
			else {
				var35 = 0.029673731;
			}
		}
	}
	double var36;
	if ((input[66]) >= (189.5)) {
		if ((input[61]) >= (0.03475)) {
			if ((input[66]) >= (1711.5)) {
				var36 = -0.005664455;
			}
			else {
				var36 = 0.024460914;
			}
		}
		else {
			if ((input[12]) >= (2.3472)) {
				var36 = -0.049104046;
			}
			else {
				var36 = -0.01694795;
			}
		}
	}
	else {
		if ((input[64]) >= (38.5)) {
			if ((input[70]) >= (1.5)) {
				var36 = 0.031161407;
			}
			else {
				var36 = 0.08428508;
			}
		}
		else {
			if ((input[56]) >= (2.6231499)) {
				var36 = 0.033711445;
			}
			else {
				var36 = -0.024736682;
			}
		}
	}
	double var37;
	if ((input[51]) >= (3.5)) {
		if ((input[71]) >= (659.5)) {
			if ((input[64]) >= (43.5)) {
				var37 = 0.026801798;
			}
			else {
				var37 = -0.008193343;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var37 = 0.06379239;
			}
			else {
				var37 = -0.010009026;
			}
		}
	}
	else {
		if ((input[12]) >= (8.634251)) {
			if ((input[46]) >= (4.6661997)) {
				var37 = -0.019625887;
			}
			else {
				var37 = -0.05828872;
			}
		}
		else {
			if ((input[56]) >= (3.20745)) {
				var37 = 0.007000282;
			}
			else {
				var37 = -0.019974642;
			}
		}
	}
	double var38;
	if ((input[66]) >= (47.5)) {
		if ((input[5]) >= (8.775)) {
			if ((input[58]) >= (0.59145)) {
				var38 = -0.013899493;
			}
			else {
				var38 = -0.058214992;
			}
		}
		else {
			if ((input[71]) >= (58922.0)) {
				var38 = -0.021549871;
			}
			else {
				var38 = 0.008815123;
			}
		}
	}
	else {
		if ((input[70]) >= (0.5)) {
			if ((input[60]) >= (0.36815)) {
				var38 = 0.050111353;
			}
			else {
				var38 = 0.0047669723;
			}
		}
		else {
			if ((input[46]) >= (7.08585)) {
				var38 = -0.09686163;
			}
			else {
				var38 = 0.08424299;
			}
		}
	}
	double var39;
	if ((input[66]) >= (218.5)) {
		if ((input[57]) >= (1.6477499)) {
			if ((input[47]) >= (2.6764998)) {
				var39 = -0.044986136;
			}
			else {
				var39 = 0.005581354;
			}
		}
		else {
			if ((input[64]) >= (43.5)) {
				var39 = 0.0018572084;
			}
			else {
				var39 = -0.03410663;
			}
		}
	}
	else {
		if ((input[64]) >= (38.5)) {
			if ((input[70]) >= (2.5)) {
				var39 = 0.012093718;
			}
			else {
				var39 = 0.0758473;
			}
		}
		else {
			if ((input[62]) >= (5.12945)) {
				var39 = 0.035320114;
			}
			else {
				var39 = -0.017871836;
			}
		}
	}
	double var40;
	if ((input[18]) >= (3.5)) {
		if ((input[66]) >= (10415.0)) {
			if ((input[57]) >= (2.4697)) {
				var40 = 0.005115039;
			}
			else {
				var40 = -0.029152278;
			}
		}
		else {
			if ((input[64]) >= (43.5)) {
				var40 = 0.063324474;
			}
			else {
				var40 = 0.015684864;
			}
		}
	}
	else {
		if ((input[1]) >= (8.89795)) {
			if ((input[22]) >= (8.8618)) {
				var40 = -0.030299043;
			}
			else {
				var40 = 0.002191376;
			}
		}
		else {
			if ((input[64]) >= (43.5)) {
				var40 = 0.020272095;
			}
			else {
				var40 = -0.00485553;
			}
		}
	}
	double var41;
	if ((input[66]) >= (101.5)) {
		if ((input[61]) >= (0.03475)) {
			if ((input[66]) >= (1711.5)) {
				var41 = -0.0048159803;
			}
			else {
				var41 = 0.023764096;
			}
		}
		else {
			if ((input[58]) >= (0.28320003)) {
				var41 = -0.01583162;
			}
			else {
				var41 = -0.061166674;
			}
		}
	}
	else {
		if ((input[64]) >= (38.5)) {
			if ((input[71]) >= (-85.5)) {
				var41 = 0.0826573;
			}
			else {
				var41 = 0.029727167;
			}
		}
		else {
			if ((input[56]) >= (2.5655499)) {
				var41 = 0.045299884;
			}
			else {
				var41 = -0.010707596;
			}
		}
	}
	double var42;
	if ((input[70]) >= (11.5)) {
		if ((input[71]) >= (431.5)) {
			if ((input[2]) >= (2.5)) {
				var42 = 0.0070680166;
			}
			else {
				var42 = -0.020516088;
			}
		}
		else {
			if ((input[57]) >= (1.15435)) {
				var42 = -0.037077416;
			}
			else {
				var42 = -0.07955764;
			}
		}
	}
	else {
		if ((input[71]) >= (886.5)) {
			if ((input[64]) >= (43.5)) {
				var42 = 0.011577538;
			}
			else {
				var42 = -0.01699937;
			}
		}
		else {
			if ((input[58]) >= (10.34115)) {
				var42 = 0.051452685;
			}
			else {
				var42 = 0.0029596786;
			}
		}
	}
	double var43;
	if ((input[1]) >= (0.76119995)) {
		if ((input[60]) >= (2.18505)) {
			if ((input[66]) >= (643.5)) {
				var43 = -0.0041453145;
			}
			else {
				var43 = 0.03031261;
			}
		}
		else {
			if ((input[66]) >= (284.5)) {
				var43 = -0.041926272;
			}
			else {
				var43 = -0.008136128;
			}
		}
	}
	else {
		if ((input[64]) >= (38.5)) {
			if ((input[66]) >= (467.5)) {
				var43 = 0.009638387;
			}
			else {
				var43 = 0.052033544;
			}
		}
		else {
			if ((input[66]) >= (18.5)) {
				var43 = -0.004582495;
			}
			else {
				var43 = 0.05265355;
			}
		}
	}
	double var44;
	if ((input[70]) >= (0.5)) {
		if ((input[57]) >= (2.07545)) {
			if ((input[66]) >= (2998.5)) {
				var44 = -0.004290708;
			}
			else {
				var44 = 0.022460094;
			}
		}
		else {
			if ((input[70]) >= (3.5)) {
				var44 = -0.022971727;
			}
			else {
				var44 = 0.002145042;
			}
		}
	}
	else {
		if ((input[66]) >= (5395.5)) {
			if ((input[64]) >= (44.5)) {
				var44 = 0.010696393;
			}
			else {
				var44 = -0.044818867;
			}
		}
		else {
			if ((input[64]) >= (43.5)) {
				var44 = 0.085517;
			}
			else {
				var44 = 0.021213174;
			}
		}
	}
	double var45;
	if ((input[0]) >= (0.5)) {
		if ((input[66]) >= (375.5)) {
			if ((input[57]) >= (1.64765)) {
				var45 = 0.006267605;
			}
			else {
				var45 = -0.021304062;
			}
		}
		else {
			if ((input[64]) >= (40.5)) {
				var45 = 0.066280834;
			}
			else {
				var45 = 0.010570362;
			}
		}
	}
	else {
		if ((input[70]) >= (12.5)) {
			if ((input[32]) >= (1.5)) {
				var45 = -0.02691879;
			}
			else {
				var45 = -0.07157116;
			}
		}
		else {
			if ((input[71]) >= (114.5)) {
				var45 = -0.017821584;
			}
			else {
				var45 = 0.012571774;
			}
		}
	}
	double var46;
	if ((input[5]) >= (0.1)) {
		if ((input[12]) >= (8.5347)) {
			if ((input[46]) >= (4.8261003)) {
				var46 = -0.0129857985;
			}
			else {
				var46 = -0.042870592;
			}
		}
		else {
			if ((input[64]) >= (43.5)) {
				var46 = 0.019532321;
			}
			else {
				var46 = -0.003497239;
			}
		}
	}
	else {
		if ((input[71]) >= (6718.5)) {
			if ((input[60]) >= (18.9131)) {
				var46 = 0.013109586;
			}
			else {
				var46 = -0.01383781;
			}
		}
		else {
			if ((input[64]) >= (43.5)) {
				var46 = 0.06522661;
			}
			else {
				var46 = 0.014801807;
			}
		}
	}
	double var47;
	if ((input[66]) >= (55.5)) {
		if ((input[58]) >= (0.97134995)) {
			if ((input[47]) >= (2.6764998)) {
				var47 = -0.037931696;
			}
			else {
				var47 = 0.002253068;
			}
		}
		else {
			if ((input[70]) >= (2.5)) {
				var47 = -0.057311136;
			}
			else {
				var47 = -0.0064088055;
			}
		}
	}
	else {
		if ((input[70]) >= (2.5)) {
			if ((input[58]) >= (0.2426)) {
				var47 = 0.03694992;
			}
			else {
				var47 = -0.072347365;
			}
		}
		else {
			if ((input[1]) >= (0.0033999998)) {
				var47 = 0.03373813;
			}
			else {
				var47 = 0.07055719;
			}
		}
	}
	double var48;
	if ((input[1]) >= (0.0991)) {
		if ((input[46]) >= (3.36055)) {
			if ((input[64]) >= (43.5)) {
				var48 = 0.011984747;
			}
			else {
				var48 = -0.0066701914;
			}
		}
		else {
			if ((input[53]) >= (0.5)) {
				var48 = -0.07113653;
			}
			else {
				var48 = -0.021015363;
			}
		}
	}
	else {
		if ((input[64]) >= (38.5)) {
			if ((input[70]) >= (2.5)) {
				var48 = 0.0032398615;
			}
			else {
				var48 = 0.056514204;
			}
		}
		else {
			if ((input[3]) >= (1.45)) {
				var48 = 0.03079519;
			}
			else {
				var48 = -0.013595114;
			}
		}
	}
	double var49;
	if ((input[1]) >= (2.7036)) {
		if ((input[58]) >= (30.639751)) {
			if ((input[66]) >= (1711.5)) {
				var49 = -0.007423149;
			}
			else {
				var49 = 0.026839033;
			}
		}
		else {
			if ((input[66]) >= (394.5)) {
				var49 = -0.048318144;
			}
			else {
				var49 = 0.00019137992;
			}
		}
	}
	else {
		if ((input[62]) >= (6.2564497)) {
			if ((input[71]) >= (1986.5)) {
				var49 = 0.0024450845;
			}
			else {
				var49 = 0.031056179;
			}
		}
		else {
			if ((input[66]) >= (18.5)) {
				var49 = -0.0089998795;
			}
			else {
				var49 = 0.041677188;
			}
		}
	}
	double var50;
	if ((input[0]) >= (0.5)) {
		if ((input[55]) >= (43.9966)) {
			if ((input[57]) >= (1.0026)) {
				var50 = 0.0027252722;
			}
			else {
				var50 = -0.023364915;
			}
		}
		else {
			if ((input[66]) >= (54.5)) {
				var50 = 0.015813353;
			}
			else {
				var50 = 0.058512896;
			}
		}
	}
	else {
		if ((input[70]) >= (13.5)) {
			if ((input[33]) >= (1.63335)) {
				var50 = -0.020401536;
			}
			else {
				var50 = -0.06305099;
			}
		}
		else {
			if ((input[71]) >= (156.5)) {
				var50 = -0.016291639;
			}
			else {
				var50 = 0.008648401;
			}
		}
	}
	double var51;
	if ((input[71]) >= (58764.5)) {
		if ((input[70]) >= (5.5)) {
			if ((input[70]) >= (404.5)) {
				var51 = -0.055252332;
			}
			else {
				var51 = 0.0014605167;
			}
		}
		else {
			if ((input[62]) >= (1287.0669)) {
				var51 = -0.022988794;
			}
			else {
				var51 = -0.049462344;
			}
		}
	}
	else {
		if ((input[57]) >= (2.57755)) {
			if ((input[70]) >= (17.5)) {
				var51 = -0.005928364;
			}
			else {
				var51 = 0.022752022;
			}
		}
		else {
			if ((input[70]) >= (4.5)) {
				var51 = -0.015490212;
			}
			else {
				var51 = 0.0047897794;
			}
		}
	}
	double var52;
	if ((input[64]) >= (43.5)) {
		if ((input[66]) >= (8239.5)) {
			if ((input[1]) >= (11.21385)) {
				var52 = -0.027169898;
			}
			else {
				var52 = 0.0038062935;
			}
		}
		else {
			if ((input[62]) >= (64.2185)) {
				var52 = 0.06507691;
			}
			else {
				var52 = 0.014897803;
			}
		}
	}
	else {
		if ((input[66]) >= (200.5)) {
			if ((input[58]) >= (0.39005)) {
				var52 = -0.0070161032;
			}
			else {
				var52 = -0.08124497;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var52 = 0.046983507;
			}
			else {
				var52 = -0.0010577779;
			}
		}
	}
	double var53;
	if ((input[1]) >= (8.9058)) {
		if ((input[22]) >= (8.8618)) {
			if ((input[57]) >= (1.8985)) {
				var53 = -0.01001061;
			}
			else {
				var53 = -0.04229177;
			}
		}
		else {
			if ((input[66]) >= (684.5)) {
				var53 = 0.0015831793;
			}
			else {
				var53 = 0.031030009;
			}
		}
	}
	else {
		if ((input[64]) >= (43.5)) {
			if ((input[66]) >= (9723.5)) {
				var53 = 0.0023696087;
			}
			else {
				var53 = 0.039732054;
			}
		}
		else {
			if ((input[12]) >= (0.0454)) {
				var53 = -0.00420305;
			}
			else {
				var53 = 0.018240165;
			}
		}
	}
	double var54;
	if ((input[57]) >= (0.3323)) {
		if ((input[66]) >= (386.5)) {
			if ((input[58]) >= (29.97945)) {
				var54 = 0.0023579334;
			}
			else {
				var54 = -0.019395858;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var54 = 0.04749313;
			}
			else {
				var54 = -0.0011085424;
			}
		}
	}
	else {
		if ((input[70]) >= (2.5)) {
			if ((input[22]) >= (3.4433498)) {
				var54 = -0.07205498;
			}
			else {
				var54 = -0.03206381;
			}
		}
		else {
			if ((input[71]) >= (98.5)) {
				var54 = -0.04847417;
			}
			else {
				var54 = 0.049883515;
			}
		}
	}
	double var55;
	if ((input[20]) >= (0.5)) {
		if ((input[46]) >= (3.3920002)) {
			if ((input[58]) >= (0.24419999)) {
				var55 = -0.0012595797;
			}
			else {
				var55 = -0.038360815;
			}
		}
		else {
			if ((input[70]) >= (1.5)) {
				var55 = -0.062850855;
			}
			else {
				var55 = 0.010337255;
			}
		}
	}
	else {
		if ((input[66]) >= (3889.0)) {
			if ((input[47]) >= (2.5147)) {
				var55 = -0.033238657;
			}
			else {
				var55 = 0.0027762258;
			}
		}
		else {
			if ((input[60]) >= (3.90185)) {
				var55 = 0.041927762;
			}
			else {
				var55 = 0.0037638966;
			}
		}
	}
	double var56;
	if ((input[12]) >= (3.2427)) {
		if ((input[46]) >= (3.44525)) {
			if ((input[0]) >= (0.5)) {
				var56 = -0.00077162933;
			}
			else {
				var56 = -0.024613872;
			}
		}
		else {
			if ((input[66]) >= (71.5)) {
				var56 = -0.057841867;
			}
			else {
				var56 = 0.05324861;
			}
		}
	}
	else {
		if ((input[58]) >= (6.94215)) {
			if ((input[66]) >= (620.5)) {
				var56 = 0.0056006047;
			}
			else {
				var56 = 0.043786112;
			}
		}
		else {
			if ((input[66]) >= (47.5)) {
				var56 = -0.01050263;
			}
			else {
				var56 = 0.02272236;
			}
		}
	}
	double var57;
	if ((input[64]) >= (40.5)) {
		if ((input[66]) >= (510.5)) {
			if ((input[56]) >= (28.3871)) {
				var57 = 0.0066428143;
			}
			else {
				var57 = -0.025293548;
			}
		}
		else {
			if ((input[70]) >= (2.5)) {
				var57 = 0.006609419;
			}
			else {
				var57 = 0.062458355;
			}
		}
	}
	else {
		if ((input[66]) >= (55.5)) {
			if ((input[58]) >= (0.29365)) {
				var57 = -0.0072938264;
			}
			else {
				var57 = -0.0751222;
			}
		}
		else {
			if ((input[60]) >= (0.31005)) {
				var57 = 0.033653572;
			}
			else {
				var57 = -0.0021171311;
			}
		}
	}
	double var58;
	if ((input[71]) >= (58764.5)) {
		if ((input[67]) >= (0.5)) {
			if ((input[47]) >= (2.2647)) {
				var58 = -0.052616384;
			}
			else {
				var58 = -0.022313835;
			}
		}
		else {
			if ((input[70]) >= (404.5)) {
				var58 = -0.050138563;
			}
			else {
				var58 = 0.01167391;
			}
		}
	}
	else {
		if ((input[57]) >= (2.57915)) {
			if ((input[66]) >= (4437.5)) {
				var58 = 0.0072440705;
			}
			else {
				var58 = 0.026170377;
			}
		}
		else {
			if ((input[64]) >= (46.5)) {
				var58 = 0.037009925;
			}
			else {
				var58 = -0.00598741;
			}
		}
	}
	double var59;
	if ((input[12]) >= (0.0757)) {
		if ((input[46]) >= (3.36055)) {
			if ((input[47]) >= (2.6764998)) {
				var59 = -0.031036338;
			}
			else {
				var59 = 0.00052926573;
			}
		}
		else {
			if ((input[53]) >= (1.5)) {
				var59 = -0.06970612;
			}
			else {
				var59 = -0.023578888;
			}
		}
	}
	else {
		if ((input[64]) >= (38.5)) {
			if ((input[70]) >= (1.5)) {
				var59 = 0.006848179;
			}
			else {
				var59 = 0.05581404;
			}
		}
		else {
			if ((input[3]) >= (1.45)) {
				var59 = 0.028568981;
			}
			else {
				var59 = -0.017071351;
			}
		}
	}
	double var60;
	if ((input[0]) >= (0.5)) {
		if ((input[55]) >= (43.9966)) {
			if ((input[0]) >= (6.0)) {
				var60 = -0.015268599;
			}
			else {
				var60 = 0.004011169;
			}
		}
		else {
			if ((input[12]) >= (0.1284)) {
				var60 = 0.009032229;
			}
			else {
				var60 = 0.028677016;
			}
		}
	}
	else {
		if ((input[70]) >= (16.5)) {
			if ((input[33]) >= (1.63335)) {
				var60 = -0.01762653;
			}
			else {
				var60 = -0.0599145;
			}
		}
		else {
			if ((input[58]) >= (0.5855)) {
				var60 = -0.0026110357;
			}
			else {
				var60 = -0.033695176;
			}
		}
	}
	double var61;
	if ((input[66]) >= (1315.5)) {
		if ((input[57]) >= (2.46645)) {
			if ((input[71]) >= (53495.5)) {
				var61 = -0.013434692;
			}
			else {
				var61 = 0.010698254;
			}
		}
		else {
			if ((input[2]) >= (2.5)) {
				var61 = 0.0080574;
			}
			else {
				var61 = -0.020300843;
			}
		}
	}
	else {
		if ((input[64]) >= (38.5)) {
			if ((input[56]) >= (26.06025)) {
				var61 = 0.059444737;
			}
			else {
				var61 = 0.0101043675;
			}
		}
		else {
			if ((input[66]) >= (18.5)) {
				var61 = -0.013000718;
			}
			else {
				var61 = 0.042846765;
			}
		}
	}
	double var62;
	if ((input[64]) >= (43.5)) {
		if ((input[66]) >= (8483.5)) {
			if ((input[60]) >= (18.38035)) {
				var62 = 0.0066909133;
			}
			else {
				var62 = -0.019632462;
			}
		}
		else {
			if ((input[60]) >= (10.18505)) {
				var62 = 0.068855666;
			}
			else {
				var62 = 0.018424725;
			}
		}
	}
	else {
		if ((input[66]) >= (218.5)) {
			if ((input[58]) >= (1.50665)) {
				var62 = -0.00491629;
			}
			else {
				var62 = -0.044816546;
			}
		}
		else {
			if ((input[58]) >= (10.260099)) {
				var62 = 0.065120876;
			}
			else {
				var62 = 0.004980089;
			}
		}
	}
	double var63;
	if ((input[64]) >= (46.5)) {
		if ((input[66]) >= (12651.0)) {
			if ((input[58]) >= (194.9013)) {
				var63 = 0.002514055;
			}
			else {
				var63 = -0.063143775;
			}
		}
		else {
			if ((input[70]) >= (3.5)) {
				var63 = 0.019603858;
			}
			else {
				var63 = 0.082237616;
			}
		}
	}
	else {
		if ((input[66]) >= (639.5)) {
			if ((input[58]) >= (32.184547)) {
				var63 = -0.0011747228;
			}
			else {
				var63 = -0.025496075;
			}
		}
		else {
			if ((input[58]) >= (10.309)) {
				var63 = 0.03812802;
			}
			else {
				var63 = -0.0010195117;
			}
		}
	}
	double var64;
	if ((input[1]) >= (10.51415)) {
		if ((input[51]) >= (0.5)) {
			if ((input[66]) >= (8829.0)) {
				var64 = -0.018153882;
			}
			else {
				var64 = 0.0034356534;
			}
		}
		else {
			if ((input[22]) >= (9.9848995)) {
				var64 = -0.045447275;
			}
			else {
				var64 = 0.00034208148;
			}
		}
	}
	else {
		if ((input[64]) >= (43.5)) {
			if ((input[66]) >= (1004.5)) {
				var64 = 0.0102290865;
			}
			else {
				var64 = 0.062319525;
			}
		}
		else {
			if ((input[1]) >= (0.21435)) {
				var64 = -0.0046032136;
			}
			else {
				var64 = 0.010769725;
			}
		}
	}
	double var65;
	if ((input[57]) >= (2.36825)) {
		if ((input[66]) >= (4458.5)) {
			if ((input[60]) >= (22.1968)) {
				var65 = 0.008236683;
			}
			else {
				var65 = -0.009016865;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var65 = 0.03207097;
			}
			else {
				var65 = 0.0010873821;
			}
		}
	}
	else {
		if ((input[66]) >= (478.5)) {
			if ((input[58]) >= (61.1257)) {
				var65 = -0.0032798667;
			}
			else {
				var65 = -0.02333127;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var65 = 0.026195986;
			}
			else {
				var65 = -0.010192915;
			}
		}
	}
	double var66;
	if ((input[57]) >= (0.19665)) {
		if ((input[66]) >= (1445.5)) {
			if ((input[61]) >= (0.31155002)) {
				var66 = 0.0072879614;
			}
			else {
				var66 = -0.00890288;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var66 = 0.022354139;
			}
			else {
				var66 = -0.0053151543;
			}
		}
	}
	else {
		if ((input[70]) >= (1.5)) {
			if ((input[47]) >= (2.4411998)) {
				var66 = 0.16584007;
			}
			else {
				var66 = -0.050396647;
			}
		}
		else {
			if ((input[71]) >= (71.5)) {
				var66 = -0.038782027;
			}
			else {
				var66 = 0.08171598;
			}
		}
	}
	double var67;
	if ((input[64]) >= (46.5)) {
		if ((input[66]) >= (23184.0)) {
			if ((input[57]) >= (2.6823502)) {
				var67 = 0.0008422739;
			}
			else {
				var67 = -0.037273727;
			}
		}
		else {
			if ((input[70]) >= (3.5)) {
				var67 = 0.013538294;
			}
			else {
				var67 = 0.07041224;
			}
		}
	}
	else {
		if ((input[14]) >= (0.55)) {
			if ((input[1]) >= (0.050300002)) {
				var67 = -0.0016534664;
			}
			else {
				var67 = 0.022523355;
			}
		}
		else {
			if ((input[66]) >= (98.5)) {
				var67 = -0.025226876;
			}
			else {
				var67 = 0.011360408;
			}
		}
	}
	double var68;
	if ((input[61]) >= (0.07305)) {
		if ((input[66]) >= (2722.5)) {
			if ((input[47]) >= (2.5147)) {
				var68 = -0.025845347;
			}
			else {
				var68 = 0.000015935035;
			}
		}
		else {
			if ((input[64]) >= (40.5)) {
				var68 = 0.04919566;
			}
			else {
				var68 = 0.008096871;
			}
		}
	}
	else {
		if ((input[66]) >= (285.5)) {
			if ((input[59]) >= (1.0165501)) {
				var68 = -0.0085803205;
			}
			else {
				var68 = -0.032799397;
			}
		}
		else {
			if ((input[64]) >= (39.5)) {
				var68 = 0.033926588;
			}
			else {
				var68 = -0.0036579717;
			}
		}
	}
	double var69;
	if ((input[46]) >= (3.4022)) {
		if ((input[71]) >= (58764.5)) {
			if ((input[70]) >= (5.5)) {
				var69 = 0.0011811311;
			}
			else {
				var69 = -0.02817589;
			}
		}
		else {
			if ((input[64]) >= (46.5)) {
				var69 = 0.031305145;
			}
			else {
				var69 = 0.0009870896;
			}
		}
	}
	else {
		if ((input[53]) >= (1.5)) {
			if ((input[57]) >= (1.3965499)) {
				var69 = -0.02499432;
			}
			else {
				var69 = -0.075437136;
			}
		}
		else {
			if ((input[12]) >= (0.12745)) {
				var69 = -0.023406113;
			}
			else {
				var69 = 0.053647626;
			}
		}
	}
	double var70;
	if ((input[57]) >= (2.41715)) {
		if ((input[71]) >= (53495.5)) {
			if ((input[70]) >= (9.5)) {
				var70 = 0.008888982;
			}
			else {
				var70 = -0.019346291;
			}
		}
		else {
			if ((input[64]) >= (45.5)) {
				var70 = 0.028076887;
			}
			else {
				var70 = 0.007874335;
			}
		}
	}
	else {
		if ((input[66]) >= (513.5)) {
			if ((input[46]) >= (5.37685)) {
				var70 = -0.0039731236;
			}
			else {
				var70 = -0.024649544;
			}
		}
		else {
			if ((input[64]) >= (40.5)) {
				var70 = 0.032717694;
			}
			else {
				var70 = -0.0036699164;
			}
		}
	}
	double var71;
	if ((input[0]) >= (0.5)) {
		if ((input[13]) >= (0.5)) {
			if ((input[66]) >= (2670.5)) {
				var71 = -0.00036645474;
			}
			else {
				var71 = 0.012681519;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var71 = -0.027757792;
			}
			else {
				var71 = -0.0019535997;
			}
		}
	}
	else {
		if ((input[1]) >= (2.0874)) {
			if ((input[66]) >= (72.5)) {
				var71 = -0.023683809;
			}
			else {
				var71 = 0.034332182;
			}
		}
		else {
			if ((input[68]) >= (0.5)) {
				var71 = 0.020145586;
			}
			else {
				var71 = -0.0074359984;
			}
		}
	}
	double var72;
	if ((input[64]) >= (43.5)) {
		if ((input[66]) >= (6364.5)) {
			if ((input[58]) >= (221.5432)) {
				var72 = 0.0020585502;
			}
			else {
				var72 = -0.037906267;
			}
		}
		else {
			if ((input[62]) >= (116.72975)) {
				var72 = 0.06599393;
			}
			else {
				var72 = 0.0151211275;
			}
		}
	}
	else {
		if ((input[57]) >= (0.19665)) {
			if ((input[66]) >= (192.5)) {
				var72 = -0.0046693054;
			}
			else {
				var72 = 0.013485933;
			}
		}
		else {
			if ((input[66]) >= (12.5)) {
				var72 = -0.04121686;
			}
			else {
				var72 = 0.092291795;
			}
		}
	}
	double var73;
	if ((input[61]) >= (0.04515)) {
		if ((input[66]) >= (1698.5)) {
			if ((input[72]) >= (23.5)) {
				var73 = -0.005640031;
			}
			else {
				var73 = 0.009208634;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var73 = 0.03415092;
			}
			else {
				var73 = 0.00093098276;
			}
		}
	}
	else {
		if ((input[66]) >= (101.5)) {
			if ((input[46]) >= (4.8955)) {
				var73 = -0.007323885;
			}
			else {
				var73 = -0.027517943;
			}
		}
		else {
			if ((input[71]) >= (-118.5)) {
				var73 = 0.01591287;
			}
			else {
				var73 = -0.037491396;
			}
		}
	}
	double var74;
	if ((input[61]) >= (0.30835)) {
		if ((input[66]) >= (2949.5)) {
			if ((input[72]) >= (32.5)) {
				var74 = -0.0015122549;
			}
			else {
				var74 = 0.016824527;
			}
		}
		else {
			if ((input[64]) >= (40.5)) {
				var74 = 0.070374034;
			}
			else {
				var74 = 0.020126572;
			}
		}
	}
	else {
		if ((input[66]) >= (1229.5)) {
			if ((input[46]) >= (5.3769503)) {
				var74 = -0.003938466;
			}
			else {
				var74 = -0.024794107;
			}
		}
		else {
			if ((input[64]) >= (43.5)) {
				var74 = 0.049246266;
			}
			else {
				var74 = 0.00047297517;
			}
		}
	}
	double var75;
	if ((input[56]) >= (11.5659)) {
		if ((input[66]) >= (1193.5)) {
			if ((input[12]) >= (14.1089)) {
				var75 = -0.01408721;
			}
			else {
				var75 = 0.002367648;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var75 = 0.041070618;
			}
			else {
				var75 = -0.00012208127;
			}
		}
	}
	else {
		if ((input[22]) >= (3.2612)) {
			if ((input[66]) >= (220.5)) {
				var75 = -0.033396434;
			}
			else {
				var75 = -0.00042981855;
			}
		}
		else {
			if ((input[71]) >= (53.5)) {
				var75 = -0.0054943818;
			}
			else {
				var75 = 0.014766584;
			}
		}
	}
	double var76;
	if ((input[64]) >= (40.5)) {
		if ((input[66]) >= (3033.5)) {
			if ((input[58]) >= (221.68785)) {
				var76 = 0.0021049145;
			}
			else {
				var76 = -0.020586744;
			}
		}
		else {
			if ((input[58]) >= (86.38015)) {
				var76 = 0.053837746;
			}
			else {
				var76 = 0.006186249;
			}
		}
	}
	else {
		if ((input[58]) >= (1.5215499)) {
			if ((input[70]) >= (1.5)) {
				var76 = 0.0025683453;
			}
			else {
				var76 = -0.02122295;
			}
		}
		else {
			if ((input[66]) >= (23.5)) {
				var76 = -0.031841222;
			}
			else {
				var76 = 0.032948013;
			}
		}
	}
	double var77;
	if ((input[64]) >= (43.5)) {
		if ((input[66]) >= (9355.5)) {
			if ((input[24]) >= (0.775)) {
				var77 = 0.0026235802;
			}
			else {
				var77 = -0.027486786;
			}
		}
		else {
			if ((input[70]) >= (1.5)) {
				var77 = 0.0055588577;
			}
			else {
				var77 = 0.045963358;
			}
		}
	}
	else {
		if ((input[2]) >= (2.5)) {
			if ((input[66]) >= (47753.0)) {
				var77 = -0.011285025;
			}
			else {
				var77 = 0.015315217;
			}
		}
		else {
			if ((input[66]) >= (317.5)) {
				var77 = -0.011738407;
			}
			else {
				var77 = 0.0054915915;
			}
		}
	}
	double var78;
	if ((input[57]) >= (2.68085)) {
		if ((input[71]) >= (97289.5)) {
			if ((input[70]) >= (5.5)) {
				var78 = 0.0010348106;
			}
			else {
				var78 = -0.03143237;
			}
		}
		else {
			if ((input[12]) >= (19.03955)) {
				var78 = -0.012721996;
			}
			else {
				var78 = 0.013992964;
			}
		}
	}
	else {
		if ((input[2]) >= (2.5)) {
			if ((input[0]) >= (0.5)) {
				var78 = 0.017187301;
			}
			else {
				var78 = -0.007568308;
			}
		}
		else {
			if ((input[66]) >= (1069.5)) {
				var78 = -0.01326518;
			}
			else {
				var78 = 0.00086030253;
			}
		}
	}
	double var79;
	if ((input[64]) >= (46.5)) {
		if ((input[66]) >= (23184.0)) {
			if ((input[24]) >= (2.225)) {
				var79 = 0.03886422;
			}
			else {
				var79 = -0.0099622;
			}
		}
		else {
			if ((input[66]) >= (5269.0)) {
				var79 = 0.03109551;
			}
			else {
				var79 = 0.08064755;
			}
		}
	}
	else {
		if ((input[14]) >= (1.1)) {
			if ((input[12]) >= (0.02)) {
				var79 = 0.00040607067;
			}
			else {
				var79 = 0.027007872;
			}
		}
		else {
			if ((input[70]) >= (10.5)) {
				var79 = -0.033287406;
			}
			else {
				var79 = -0.006068373;
			}
		}
	}
	double var80;
	if ((input[46]) >= (3.36075)) {
		if ((input[47]) >= (2.6764998)) {
			if ((input[71]) >= (526.5)) {
				var80 = -0.024797892;
			}
			else {
				var80 = 0.052575227;
			}
		}
		else {
			if ((input[61]) >= (0.12735)) {
				var80 = 0.007597481;
			}
			else {
				var80 = -0.0026127931;
			}
		}
	}
	else {
		if ((input[53]) >= (0.5)) {
			if ((input[48]) >= (1.875)) {
				var80 = 0.07149143;
			}
			else {
				var80 = -0.051707324;
			}
		}
		else {
			if ((input[1]) >= (0.117699996)) {
				var80 = -0.013119345;
			}
			else {
				var80 = 0.071804926;
			}
		}
	}
	double var81;
	if ((input[0]) >= (0.5)) {
		if ((input[13]) >= (0.5)) {
			if ((input[1]) >= (0.050300002)) {
				var81 = 0.0026253115;
			}
			else {
				var81 = 0.022549892;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var81 = -0.024809169;
			}
			else {
				var81 = -0.0013091163;
			}
		}
	}
	else {
		if ((input[70]) >= (64.5)) {
			if ((input[55]) >= (56.4121)) {
				var81 = -0.06300096;
			}
			else {
				var81 = -0.011666719;
			}
		}
		else {
			if ((input[58]) >= (0.59055)) {
				var81 = -0.0031683862;
			}
			else {
				var81 = -0.031467266;
			}
		}
	}
	double var82;
	if ((input[64]) >= (40.5)) {
		if ((input[66]) >= (746.5)) {
			if ((input[58]) >= (58.43605)) {
				var82 = 0.005365647;
			}
			else {
				var82 = -0.02201784;
			}
		}
		else {
			if ((input[70]) >= (2.5)) {
				var82 = -0.0007535092;
			}
			else {
				var82 = 0.04375894;
			}
		}
	}
	else {
		if ((input[56]) >= (0.0731)) {
			if ((input[70]) >= (1.5)) {
				var82 = 0.0009250763;
			}
			else {
				var82 = -0.019441074;
			}
		}
		else {
			if ((input[66]) >= (17.5)) {
				var82 = -0.05930556;
			}
			else {
				var82 = 0.06191904;
			}
		}
	}
	double var83;
	if ((input[72]) >= (23.5)) {
		if ((input[66]) >= (4573.5)) {
			if ((input[67]) >= (0.5)) {
				var83 = -0.01350721;
			}
			else {
				var83 = 0.0012106255;
			}
		}
		else {
			if ((input[64]) >= (46.5)) {
				var83 = 0.07546467;
			}
			else {
				var83 = 0.00062536064;
			}
		}
	}
	else {
		if ((input[46]) >= (5.61175)) {
			if ((input[71]) >= (151488.5)) {
				var83 = -0.018249502;
			}
			else {
				var83 = 0.016232535;
			}
		}
		else {
			if ((input[22]) >= (9.74855)) {
				var83 = -0.019139497;
			}
			else {
				var83 = 0.004123474;
			}
		}
	}
	double var84;
	if ((input[20]) >= (0.5)) {
		if ((input[14]) >= (1.1)) {
			if ((input[12]) >= (0.02)) {
				var84 = -0.00087421125;
			}
			else {
				var84 = 0.024209296;
			}
		}
		else {
			if ((input[22]) >= (5.5620003)) {
				var84 = -0.024430761;
			}
			else {
				var84 = -0.0026145112;
			}
		}
	}
	else {
		if ((input[0]) >= (6.0)) {
			if ((input[61]) >= (0.05975)) {
				var84 = 0.010382196;
			}
			else {
				var84 = -0.022062397;
			}
		}
		else {
			if ((input[12]) >= (0.01025)) {
				var84 = 0.0077050473;
			}
			else {
				var84 = 0.06019774;
			}
		}
	}
	double var85;
	if ((input[64]) >= (44.5)) {
		if ((input[66]) >= (9819.0)) {
			if ((input[58]) >= (845.4606)) {
				var85 = 0.0056184432;
			}
			else {
				var85 = -0.020313436;
			}
		}
		else {
			if ((input[58]) >= (132.41605)) {
				var85 = 0.04922326;
			}
			else {
				var85 = 0.007796122;
			}
		}
	}
	else {
		if ((input[66]) >= (144.5)) {
			if ((input[70]) >= (1.5)) {
				var85 = -0.00070190214;
			}
			else {
				var85 = -0.01793508;
			}
		}
		else {
			if ((input[62]) >= (5.8271503)) {
				var85 = 0.031919286;
			}
			else {
				var85 = 0.0019439545;
			}
		}
	}
	double var86;
	if ((input[0]) >= (0.5)) {
		if ((input[55]) >= (43.9966)) {
			if ((input[0]) >= (6.0)) {
				var86 = -0.012487819;
			}
			else {
				var86 = 0.0032209312;
			}
		}
		else {
			if ((input[16]) >= (4.45)) {
				var86 = 0.023016723;
			}
			else {
				var86 = 0.005774639;
			}
		}
	}
	else {
		if ((input[1]) >= (2.0625)) {
			if ((input[49]) >= (7.70835)) {
				var86 = -0.037770417;
			}
			else {
				var86 = -0.011993585;
			}
		}
		else {
			if ((input[68]) >= (0.5)) {
				var86 = 0.019025618;
			}
			else {
				var86 = -0.0069823563;
			}
		}
	}
	double var87;
	if ((input[64]) >= (40.5)) {
		if ((input[66]) >= (3033.5)) {
			if ((input[58]) >= (646.4655)) {
				var87 = 0.0047298656;
			}
			else {
				var87 = -0.011601097;
			}
		}
		else {
			if ((input[58]) >= (139.58685)) {
				var87 = 0.056268163;
			}
			else {
				var87 = 0.007858456;
			}
		}
	}
	else {
		if ((input[58]) >= (1.5215499)) {
			if ((input[71]) >= (55.5)) {
				var87 = -0.0041025667;
			}
			else {
				var87 = 0.02192585;
			}
		}
		else {
			if ((input[66]) >= (40.5)) {
				var87 = -0.031172846;
			}
			else {
				var87 = 0.015041013;
			}
		}
	}
	double var88;
	if ((input[64]) >= (46.5)) {
		if ((input[66]) >= (23184.0)) {
			if ((input[61]) >= (0.31085)) {
				var88 = 0.009815195;
			}
			else {
				var88 = -0.017262854;
			}
		}
		else {
			if ((input[51]) >= (2.5)) {
				var88 = 0.06145642;
			}
			else {
				var88 = 0.014670359;
			}
		}
	}
	else {
		if ((input[13]) >= (2.5)) {
			if ((input[48]) >= (1.875)) {
				var88 = -0.0043011475;
			}
			else {
				var88 = 0.015797405;
			}
		}
		else {
			if ((input[66]) >= (1074.5)) {
				var88 = -0.009833106;
			}
			else {
				var88 = 0.0031071806;
			}
		}
	}
	double var89;
	if ((input[46]) >= (3.237)) {
		if ((input[72]) >= (23.5)) {
			if ((input[66]) >= (4573.5)) {
				var89 = -0.0075437473;
			}
			else {
				var89 = 0.0023597032;
			}
		}
		else {
			if ((input[2]) >= (2.5)) {
				var89 = 0.021443386;
			}
			else {
				var89 = 0.003263029;
			}
		}
	}
	else {
		if ((input[53]) >= (0.5)) {
			if ((input[48]) >= (1.875)) {
				var89 = 0.080940835;
			}
			else {
				var89 = -0.049538042;
			}
		}
		else {
			if ((input[62]) >= (0.9231)) {
				var89 = -0.022042416;
			}
			else {
				var89 = 0.060197778;
			}
		}
	}
	double var90;
	if ((input[56]) >= (11.5659)) {
		if ((input[66]) >= (637.5)) {
			if ((input[55]) >= (43.99365)) {
				var90 = -0.0027632422;
			}
			else {
				var90 = 0.010829685;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var90 = 0.0508056;
			}
			else {
				var90 = 0.0104392385;
			}
		}
	}
	else {
		if ((input[22]) >= (3.2612)) {
			if ((input[66]) >= (220.5)) {
				var90 = -0.029130936;
			}
			else {
				var90 = 0.0005541309;
			}
		}
		else {
			if ((input[54]) >= (2.5)) {
				var90 = -0.0065627955;
			}
			else {
				var90 = 0.009081653;
			}
		}
	}
	double var91;
	if ((input[64]) >= (40.5)) {
		if ((input[66]) >= (4578.5)) {
			if ((input[58]) >= (640.8129)) {
				var91 = 0.0038811818;
			}
			else {
				var91 = -0.013575164;
			}
		}
		else {
			if ((input[58]) >= (118.2926)) {
				var91 = 0.037304986;
			}
			else {
				var91 = 0.004280483;
			}
		}
	}
	else {
		if ((input[56]) >= (0.072349995)) {
			if ((input[70]) >= (1.5)) {
				var91 = 0.00060257316;
			}
			else {
				var91 = -0.017128784;
			}
		}
		else {
			if ((input[66]) >= (12.5)) {
				var91 = -0.049578637;
			}
			else {
				var91 = 0.08913746;
			}
		}
	}
	double var92;
	if ((input[0]) >= (0.5)) {
		if ((input[24]) >= (0.225)) {
			if ((input[49]) >= (4.125)) {
				var92 = 0.01532733;
			}
			else {
				var92 = 0.0008010358;
			}
		}
		else {
			if ((input[70]) >= (8.5)) {
				var92 = -0.03778491;
			}
			else {
				var92 = -0.004341776;
			}
		}
	}
	else {
		if ((input[1]) >= (12.454599)) {
			if ((input[35]) >= (0.1)) {
				var92 = -0.021729315;
			}
			else {
				var92 = -0.060611278;
			}
		}
		else {
			if ((input[58]) >= (0.9675)) {
				var92 = -0.00064504554;
			}
			else {
				var92 = -0.02328019;
			}
		}
	}
	double var93;
	if ((input[64]) >= (43.5)) {
		if ((input[66]) >= (9355.5)) {
			if ((input[12]) >= (14.9795)) {
				var93 = -0.021750191;
			}
			else {
				var93 = 0.0026904126;
			}
		}
		else {
			if ((input[60]) >= (11.6714)) {
				var93 = 0.05388076;
			}
			else {
				var93 = 0.009680131;
			}
		}
	}
	else {
		if ((input[32]) >= (2.5)) {
			if ((input[67]) >= (0.5)) {
				var93 = -0.0038842726;
			}
			else {
				var93 = 0.026541138;
			}
		}
		else {
			if ((input[66]) >= (341.5)) {
				var93 = -0.008243484;
			}
			else {
				var93 = 0.0054328064;
			}
		}
	}
	double var94;
	if ((input[46]) >= (6.16965)) {
		if ((input[48]) >= (1.875)) {
			if ((input[22]) >= (66.54445)) {
				var94 = 0.06031228;
			}
			else {
				var94 = -0.011114966;
			}
		}
		else {
			if ((input[57]) >= (2.91645)) {
				var94 = 0.021017123;
			}
			else {
				var94 = 0.0041175927;
			}
		}
	}
	else {
		if ((input[1]) >= (2.0845)) {
			if ((input[33]) >= (0.225)) {
				var94 = -0.0055471;
			}
			else {
				var94 = -0.026506841;
			}
		}
		else {
			if ((input[58]) >= (6.89165)) {
				var94 = 0.009453834;
			}
			else {
				var94 = -0.0035891067;
			}
		}
	}
	double var95;
	if ((input[5]) >= (0.1)) {
		if ((input[3]) >= (1.55)) {
			if ((input[66]) >= (918.5)) {
				var95 = -0.0011710757;
			}
			else {
				var95 = 0.014670618;
			}
		}
		else {
			if ((input[70]) >= (15.5)) {
				var95 = -0.027842263;
			}
			else {
				var95 = -0.004042508;
			}
		}
	}
	else {
		if ((input[0]) >= (6.0)) {
			if ((input[61]) >= (0.04795)) {
				var95 = 0.010515011;
			}
			else {
				var95 = -0.025766853;
			}
		}
		else {
			if ((input[12]) >= (0.0122)) {
				var95 = 0.009998858;
			}
			else {
				var95 = 0.077051274;
			}
		}
	}
	double var96;
	if ((input[47]) >= (2.6764998)) {
		if ((input[71]) >= (526.5)) {
			if ((input[68]) >= (0.5)) {
				var96 = 0.0084488895;
			}
			else {
				var96 = -0.029148463;
			}
		}
		else {
			if ((input[59]) >= (1.7696)) {
				var96 = 0.00019659237;
			}
			else {
				var96 = 0.09249234;
			}
		}
	}
	else {
		if ((input[61]) >= (0.27034998)) {
			if ((input[66]) >= (1696.5)) {
				var96 = 0.006135193;
			}
			else {
				var96 = 0.040378224;
			}
		}
		else {
			if ((input[66]) >= (382.5)) {
				var96 = -0.00458495;
			}
			else {
				var96 = 0.0058324407;
			}
		}
	}
	double var97;
	if ((input[66]) >= (-236.5)) {
		if ((input[64]) >= (46.5)) {
			if ((input[66]) >= (30030.0)) {
				var97 = -0.0066943723;
			}
			else {
				var97 = 0.039359868;
			}
		}
		else {
			if ((input[14]) >= (0.55)) {
				var97 = 0.0005931968;
			}
			else {
				var97 = -0.012958869;
			}
		}
	}
	else {
		if ((input[1]) >= (4.31785)) {
			if ((input[24]) >= (1.1)) {
				var97 = -0.089384474;
			}
			else {
				var97 = 0.058379557;
			}
		}
		else {
			if ((input[72]) >= (13.5)) {
				var97 = -0.12248745;
			}
			else {
				var97 = 0.020588871;
			}
		}
	}
	double var98;
	if ((input[0]) >= (0.5)) {
		if ((input[0]) >= (6.0)) {
			if ((input[55]) >= (52.7666)) {
				var98 = -0.012776521;
			}
			else {
				var98 = 0.0053468104;
			}
		}
		else {
			if ((input[66]) >= (1079.5)) {
				var98 = 0.00082776276;
			}
			else {
				var98 = 0.021079002;
			}
		}
	}
	else {
		if ((input[70]) >= (70.5)) {
			if ((input[55]) >= (56.285652)) {
				var98 = -0.059856247;
			}
			else {
				var98 = -0.0057506436;
			}
		}
		else {
			if ((input[33]) >= (1.55)) {
				var98 = 0.001204324;
			}
			else {
				var98 = -0.01486007;
			}
		}
	}
	double var99;
	if ((input[64]) >= (40.5)) {
		if ((input[66]) >= (1059.5)) {
			if ((input[58]) >= (35.44505)) {
				var99 = 0.0033574668;
			}
			else {
				var99 = -0.025893554;
			}
		}
		else {
			if ((input[71]) >= (-335.5)) {
				var99 = 0.02491667;
			}
			else {
				var99 = -0.04929504;
			}
		}
	}
	else {
		if ((input[66]) >= (55.5)) {
			if ((input[58]) >= (0.3172)) {
				var99 = -0.0043328223;
			}
			else {
				var99 = -0.05333772;
			}
		}
		else {
			if ((input[71]) >= (-221.5)) {
				var99 = 0.01821935;
			}
			else {
				var99 = -0.06166634;
			}
		}
	}
	double var100;
	var100 = (1.0) / ((1.0) + (exp((0.0) - ((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((var0)+(var1)) + (var2)) + (var3)) + (var4)) + (var5)) + (var6)) + (var7)) + (var8)) + (var9)) + (var10)) + (var11)) + (var12)) + (var13)) + (var14)) + (var15)) + (var16)) + (var17)) + (var18)) + (var19)) + (var20)) + (var21)) + (var22)) + (var23)) + (var24)) + (var25)) + (var26)) + (var27)) + (var28)) + (var29)) + (var30)) + (var31)) + (var32)) + (var33)) + (var34)) + (var35)) + (var36)) + (var37)) + (var38)) + (var39)) + (var40)) + (var41)) + (var42)) + (var43)) + (var44)) + (var45)) + (var46)) + (var47)) + (var48)) + (var49)) + (var50)) + (var51)) + (var52)) + (var53)) + (var54)) + (var55)) + (var56)) + (var57)) + (var58)) + (var59)) + (var60)) + (var61)) + (var62)) + (var63)) + (var64)) + (var65)) + (var66)) + (var67)) + (var68)) + (var69)) + (var70)) + (var71)) + (var72)) + (var73)) + (var74)) + (var75)) + (var76)) + (var77)) + (var78)) + (var79)) + (var80)) + (var81)) + (var82)) + (var83)) + (var84)) + (var85)) + (var86)) + (var87)) + (var88)) + (var89)) + (var90)) + (var91)) + (var92)) + (var93)) + (var94)) + (var95)) + (var96)) + (var97)) + (var98)) + (var99)))));
	*output = var100 < 0.5 ? 0 : 1;
}

void PUclf7(double * input, double * output) {
	double var0;
	if ((input[70]) >= (2.5)) {
		if ((input[51]) >= (1.5)) {
			if ((input[70]) >= (7.5)) {
				var0 = -0.062210698;
			}
			else {
				var0 = 0.034372505;
			}
		}
		else {
			if ((input[70]) >= (7.5)) {
				var0 = -0.15440264;
			}
			else {
				var0 = -0.075124435;
			}
		}
	}
	else {
		if ((input[71]) >= (616.5)) {
			if ((input[26]) >= (5.55)) {
				var0 = -0.0011311055;
			}
			else {
				var0 = 0.074082844;
			}
		}
		else {
			if ((input[70]) >= (1.5)) {
				var0 = 0.07397005;
			}
			else {
				var0 = 0.16517623;
			}
		}
	}
	double var1;
	if ((input[70]) >= (4.5)) {
		if ((input[50]) >= (1.5)) {
			if ((input[70]) >= (8.5)) {
				var1 = -0.066295825;
			}
			else {
				var1 = -0.00070529594;
			}
		}
		else {
			if ((input[27]) >= (0.5)) {
				var1 = -0.091024145;
			}
			else {
				var1 = -0.15328443;
			}
		}
	}
	else {
		if ((input[70]) >= (1.5)) {
			if ((input[51]) >= (1.5)) {
				var1 = 0.059538282;
			}
			else {
				var1 = -0.032180615;
			}
		}
		else {
			if ((input[71]) >= (810.5)) {
				var1 = 0.06386935;
			}
			else {
				var1 = 0.14810823;
			}
		}
	}
	double var2;
	if ((input[70]) >= (3.5)) {
		if ((input[50]) >= (1.5)) {
			if ((input[70]) >= (6.5)) {
				var2 = -0.050422486;
			}
			else {
				var2 = 0.019677894;
			}
		}
		else {
			if ((input[26]) >= (7.1)) {
				var2 = -0.13592772;
			}
			else {
				var2 = -0.06956252;
			}
		}
	}
	else {
		if ((input[70]) >= (1.5)) {
			if ((input[49]) >= (4.375)) {
				var2 = -0.01053823;
			}
			else {
				var2 = 0.073148;
			}
		}
		else {
			if ((input[71]) >= (273.5)) {
				var2 = 0.06947481;
			}
			else {
				var2 = 0.14348613;
			}
		}
	}
	double var3;
	if ((input[70]) >= (4.5)) {
		if ((input[26]) >= (7.1)) {
			if ((input[49]) >= (6.58335)) {
				var3 = -0.13007745;
			}
			else {
				var3 = -0.07445033;
			}
		}
		else {
			if ((input[70]) >= (12.5)) {
				var3 = -0.06862899;
			}
			else {
				var3 = 0.0007028218;
			}
		}
	}
	else {
		if ((input[70]) >= (1.5)) {
			if ((input[27]) >= (0.5)) {
				var3 = 0.044466436;
			}
			else {
				var3 = -0.039350487;
			}
		}
		else {
			if ((input[71]) >= (978.5)) {
				var3 = 0.04858307;
			}
			else {
				var3 = 0.124933675;
			}
		}
	}
	double var4;
	if ((input[70]) >= (2.5)) {
		if ((input[51]) >= (1.5)) {
			if ((input[70]) >= (7.5)) {
				var4 = -0.040263794;
			}
			else {
				var4 = 0.025703719;
			}
		}
		else {
			if ((input[70]) >= (8.5)) {
				var4 = -0.11506416;
			}
			else {
				var4 = -0.054687645;
			}
		}
	}
	else {
		if ((input[71]) >= (145.5)) {
			if ((input[26]) >= (7.55)) {
				var4 = -0.025396535;
			}
			else {
				var4 = 0.062214024;
			}
		}
		else {
			if ((input[64]) >= (37.5)) {
				var4 = 0.13239223;
			}
			else {
				var4 = 0.045152426;
			}
		}
	}
	double var5;
	if ((input[70]) >= (5.5)) {
		if ((input[31]) >= (0.5)) {
			if ((input[54]) >= (2.5)) {
				var5 = 0.0030226018;
			}
			else {
				var5 = -0.054373797;
			}
		}
		else {
			if ((input[50]) >= (0.5)) {
				var5 = -0.083021276;
			}
			else {
				var5 = -0.12702636;
			}
		}
	}
	else {
		if ((input[70]) >= (1.5)) {
			if ((input[50]) >= (2.5)) {
				var5 = 0.052049268;
			}
			else {
				var5 = -0.020380728;
			}
		}
		else {
			if ((input[71]) >= (476.5)) {
				var5 = 0.046348542;
			}
			else {
				var5 = 0.11380839;
			}
		}
	}
	double var6;
	if ((input[70]) >= (3.5)) {
		if ((input[26]) >= (6.775)) {
			if ((input[49]) >= (4.29165)) {
				var6 = -0.099125996;
			}
			else {
				var6 = -0.033681717;
			}
		}
		else {
			if ((input[70]) >= (12.5)) {
				var6 = -0.052279394;
			}
			else {
				var6 = 0.009173131;
			}
		}
	}
	else {
		if ((input[51]) >= (2.5)) {
			if ((input[71]) >= (1483.5)) {
				var6 = 0.038208093;
			}
			else {
				var6 = 0.11118972;
			}
		}
		else {
			if ((input[71]) >= (23.5)) {
				var6 = 0.007450835;
			}
			else {
				var6 = 0.083430134;
			}
		}
	}
	double var7;
	if ((input[70]) >= (2.5)) {
		if ((input[49]) >= (6.58335)) {
			if ((input[16]) >= (8.9)) {
				var7 = -0.098762855;
			}
			else {
				var7 = -0.040414434;
			}
		}
		else {
			if ((input[70]) >= (9.5)) {
				var7 = -0.04264212;
			}
			else {
				var7 = 0.012370624;
			}
		}
	}
	else {
		if ((input[71]) >= (61.5)) {
			if ((input[27]) >= (0.5)) {
				var7 = 0.05557553;
			}
			else {
				var7 = -0.02572093;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var7 = 0.12903808;
			}
			else {
				var7 = 0.07690351;
			}
		}
	}
	double var8;
	if ((input[70]) >= (5.5)) {
		if ((input[16]) >= (8.45)) {
			if ((input[51]) >= (0.5)) {
				var8 = -0.05928042;
			}
			else {
				var8 = -0.105784915;
			}
		}
		else {
			if ((input[54]) >= (2.5)) {
				var8 = 0.007097227;
			}
			else {
				var8 = -0.03773845;
			}
		}
	}
	else {
		if ((input[51]) >= (2.5)) {
			if ((input[71]) >= (1080.5)) {
				var8 = 0.027917713;
			}
			else {
				var8 = 0.09735681;
			}
		}
		else {
			if ((input[16]) >= (6.775)) {
				var8 = -0.01900913;
			}
			else {
				var8 = 0.04011595;
			}
		}
	}
	double var9;
	if ((input[70]) >= (2.5)) {
		if ((input[27]) >= (0.5)) {
			if ((input[70]) >= (12.5)) {
				var9 = -0.043337695;
			}
			else {
				var9 = 0.011299671;
			}
		}
		else {
			if ((input[70]) >= (10.5)) {
				var9 = -0.09768473;
			}
			else {
				var9 = -0.050831992;
			}
		}
	}
	else {
		if ((input[71]) >= (37.5)) {
			if ((input[26]) >= (5.55)) {
				var9 = -0.013801805;
			}
			else {
				var9 = 0.05264324;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var9 = 0.12071963;
			}
			else {
				var9 = 0.079675876;
			}
		}
	}
	double var10;
	if ((input[70]) >= (1.5)) {
		if ((input[51]) >= (0.5)) {
			if ((input[49]) >= (2.125)) {
				var10 = -0.02156376;
			}
			else {
				var10 = 0.030833198;
			}
		}
		else {
			if ((input[70]) >= (8.5)) {
				var10 = -0.095379986;
			}
			else {
				var10 = -0.050741345;
			}
		}
	}
	else {
		if ((input[71]) >= (273.5)) {
			if ((input[71]) >= (13708.5)) {
				var10 = -0.013541986;
			}
			else {
				var10 = 0.043505885;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var10 = 0.11601456;
			}
			else {
				var10 = 0.049318198;
			}
		}
	}
	double var11;
	if ((input[70]) >= (6.5)) {
		if ((input[49]) >= (6.70835)) {
			if ((input[0]) >= (0.5)) {
				var11 = -0.05846989;
			}
			else {
				var11 = -0.096754745;
			}
		}
		else {
			if ((input[51]) >= (2.5)) {
				var11 = -0.00043375973;
			}
			else {
				var11 = -0.038107812;
			}
		}
	}
	else {
		if ((input[49]) >= (2.125)) {
			if ((input[66]) >= (18.5)) {
				var11 = -0.0025704242;
			}
			else {
				var11 = 0.094642535;
			}
		}
		else {
			if ((input[71]) >= (1637.5)) {
				var11 = 0.017633237;
			}
			else {
				var11 = 0.08764645;
			}
		}
	}
	double var12;
	if ((input[51]) >= (1.5)) {
		if ((input[70]) >= (3.5)) {
			if ((input[51]) >= (3.5)) {
				var12 = 0.029891377;
			}
			else {
				var12 = -0.011898647;
			}
		}
		else {
			if ((input[71]) >= (1481.5)) {
				var12 = 0.012005945;
			}
			else {
				var12 = 0.07436267;
			}
		}
	}
	else {
		if ((input[16]) >= (8.325)) {
			if ((input[66]) >= (18.5)) {
				var12 = -0.06884006;
			}
			else {
				var12 = 0.07365476;
			}
		}
		else {
			if ((input[12]) >= (3.32225)) {
				var12 = -0.025049388;
			}
			else {
				var12 = 0.023815623;
			}
		}
	}
	double var13;
	if ((input[70]) >= (1.5)) {
		if ((input[27]) >= (0.5)) {
			if ((input[1]) >= (4.02145)) {
				var13 = -0.017257841;
			}
			else {
				var13 = 0.023482954;
			}
		}
		else {
			if ((input[66]) >= (18.5)) {
				var13 = -0.059759963;
			}
			else {
				var13 = 0.06308748;
			}
		}
	}
	else {
		if ((input[71]) >= (105.5)) {
			if ((input[64]) >= (37.5)) {
				var13 = 0.03397681;
			}
			else {
				var13 = -0.10254703;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var13 = 0.11262983;
			}
			else {
				var13 = 0.05262136;
			}
		}
	}
	double var14;
	if ((input[70]) >= (7.5)) {
		if ((input[49]) >= (6.70835)) {
			if ((input[0]) >= (0.5)) {
				var14 = -0.052892175;
			}
			else {
				var14 = -0.090459034;
			}
		}
		else {
			if ((input[0]) >= (0.5)) {
				var14 = -0.013343886;
			}
			else {
				var14 = -0.056020904;
			}
		}
	}
	else {
		if ((input[51]) >= (2.5)) {
			if ((input[71]) >= (1078.5)) {
				var14 = 0.013349369;
			}
			else {
				var14 = 0.07147376;
			}
		}
		else {
			if ((input[66]) >= (17.5)) {
				var14 = -0.0066809054;
			}
			else {
				var14 = 0.07699036;
			}
		}
	}
	double var15;
	if ((input[51]) >= (0.5)) {
		if ((input[70]) >= (1.5)) {
			if ((input[15]) >= (3.5)) {
				var15 = -0.015923072;
			}
			else {
				var15 = 0.021024639;
			}
		}
		else {
			if ((input[71]) >= (49.5)) {
				var15 = 0.03069353;
			}
			else {
				var15 = 0.092855774;
			}
		}
	}
	else {
		if ((input[57]) >= (0.99145)) {
			if ((input[70]) >= (10.5)) {
				var15 = -0.0715188;
			}
			else {
				var15 = -0.020654386;
			}
		}
		else {
			if ((input[66]) >= (12.5)) {
				var15 = -0.097431414;
			}
			else {
				var15 = 0.05636343;
			}
		}
	}
	double var16;
	if ((input[26]) >= (4.775)) {
		if ((input[66]) >= (18.5)) {
			if ((input[50]) >= (0.5)) {
				var16 = -0.02160659;
			}
			else {
				var16 = -0.072056524;
			}
		}
		else {
			if ((input[53]) >= (0.5)) {
				var16 = 0.05366387;
			}
			else {
				var16 = 0.083914496;
			}
		}
	}
	else {
		if ((input[66]) >= (964.5)) {
			if ((input[64]) >= (43.5)) {
				var16 = 0.02381746;
			}
			else {
				var16 = -0.012847898;
			}
		}
		else {
			if ((input[70]) >= (4.5)) {
				var16 = 0.01661225;
			}
			else {
				var16 = 0.07371704;
			}
		}
	}
	double var17;
	if ((input[70]) >= (8.5)) {
		if ((input[0]) >= (0.5)) {
			if ((input[1]) >= (9.626949)) {
				var17 = -0.045488037;
			}
			else {
				var17 = -0.005062619;
			}
		}
		else {
			if ((input[50]) >= (0.5)) {
				var17 = -0.060601354;
			}
			else {
				var17 = -0.0924054;
			}
		}
	}
	else {
		if ((input[49]) >= (2.125)) {
			if ((input[66]) >= (17.5)) {
				var17 = -0.0049474654;
			}
			else {
				var17 = 0.07019771;
			}
		}
		else {
			if ((input[71]) >= (779.5)) {
				var17 = 0.01617856;
			}
			else {
				var17 = 0.070580795;
			}
		}
	}
	double var18;
	if ((input[26]) >= (5.55)) {
		if ((input[70]) >= (10.5)) {
			if ((input[0]) >= (0.5)) {
				var18 = -0.036517914;
			}
			else {
				var18 = -0.07998848;
			}
		}
		else {
			if ((input[66]) >= (17.5)) {
				var18 = -0.020362668;
			}
			else {
				var18 = 0.05932268;
			}
		}
	}
	else {
		if ((input[66]) >= (1231.5)) {
			if ((input[64]) >= (43.5)) {
				var18 = 0.018505758;
			}
			else {
				var18 = -0.014626537;
			}
		}
		else {
			if ((input[70]) >= (3.5)) {
				var18 = 0.013753006;
			}
			else {
				var18 = 0.06623321;
			}
		}
	}
	double var19;
	if ((input[51]) >= (1.5)) {
		if ((input[71]) >= (55.5)) {
			if ((input[4]) >= (3.5)) {
				var19 = -0.005661249;
			}
			else {
				var19 = 0.025900725;
			}
		}
		else {
			if ((input[70]) >= (1.5)) {
				var19 = 0.0286592;
			}
			else {
				var19 = 0.085665174;
			}
		}
	}
	else {
		if ((input[5]) >= (8.325)) {
			if ((input[70]) >= (10.5)) {
				var19 = -0.06911571;
			}
			else {
				var19 = -0.029444858;
			}
		}
		else {
			if ((input[61]) >= (0.00025)) {
				var19 = 0.0017521102;
			}
			else {
				var19 = -0.099211715;
			}
		}
	}
	double var20;
	if ((input[51]) >= (2.5)) {
		if ((input[71]) >= (325.5)) {
			if ((input[64]) >= (43.5)) {
				var20 = 0.029798228;
			}
			else {
				var20 = 0.0004195813;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var20 = 0.08230389;
			}
			else {
				var20 = 0.014925258;
			}
		}
	}
	else {
		if ((input[16]) >= (8.325)) {
			if ((input[70]) >= (10.5)) {
				var20 = -0.06124128;
			}
			else {
				var20 = -0.021709098;
			}
		}
		else {
			if ((input[12]) >= (7.0878496)) {
				var20 = -0.019156577;
			}
			else {
				var20 = 0.019343628;
			}
		}
	}
	double var21;
	if ((input[51]) >= (0.5)) {
		if ((input[12]) >= (0.49025)) {
			if ((input[64]) >= (38.5)) {
				var21 = 0.008467451;
			}
			else {
				var21 = -0.028114473;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var21 = 0.061610788;
			}
			else {
				var21 = 0.022467215;
			}
		}
	}
	else {
		if ((input[57]) >= (0.6077)) {
			if ((input[70]) >= (10.5)) {
				var21 = -0.05619384;
			}
			else {
				var21 = -0.014891753;
			}
		}
		else {
			if ((input[66]) >= (12.5)) {
				var21 = -0.09775507;
			}
			else {
				var21 = 0.049583003;
			}
		}
	}
	double var22;
	if ((input[5]) >= (6.775)) {
		if ((input[66]) >= (23.5)) {
			if ((input[16]) >= (6.1)) {
				var22 = -0.033368137;
			}
			else {
				var22 = 0.011546502;
			}
		}
		else {
			if ((input[53]) >= (0.5)) {
				var22 = 0.032421302;
			}
			else {
				var22 = 0.07712405;
			}
		}
	}
	else {
		if ((input[66]) >= (763.5)) {
			if ((input[1]) >= (13.82615)) {
				var22 = -0.017698547;
			}
			else {
				var22 = 0.011827288;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var22 = 0.015756113;
			}
			else {
				var22 = 0.0724131;
			}
		}
	}
	double var23;
	if ((input[70]) >= (1.5)) {
		if ((input[57]) >= (1.10555)) {
			if ((input[70]) >= (15.5)) {
				var23 = -0.032582052;
			}
			else {
				var23 = 0.0071034464;
			}
		}
		else {
			if ((input[53]) >= (0.5)) {
				var23 = -0.063064046;
			}
			else {
				var23 = -0.010199563;
			}
		}
	}
	else {
		if ((input[71]) >= (30.5)) {
			if ((input[64]) >= (38.5)) {
				var23 = 0.029062852;
			}
			else {
				var23 = -0.04229166;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var23 = 0.09650147;
			}
			else {
				var23 = 0.056867637;
			}
		}
	}
	double var24;
	if ((input[49]) >= (2.125)) {
		if ((input[50]) >= (0.5)) {
			if ((input[66]) >= (23.5)) {
				var24 = -0.007197797;
			}
			else {
				var24 = 0.053314168;
			}
		}
		else {
			if ((input[58]) >= (0.5272)) {
				var24 = -0.036688574;
			}
			else {
				var24 = -0.09224882;
			}
		}
	}
	else {
		if ((input[71]) >= (1712.5)) {
			if ((input[64]) >= (44.5)) {
				var24 = 0.021579295;
			}
			else {
				var24 = -0.010589701;
			}
		}
		else {
			if ((input[64]) >= (40.5)) {
				var24 = 0.0725461;
			}
			else {
				var24 = 0.029864302;
			}
		}
	}
	double var25;
	if ((input[15]) >= (3.5)) {
		if ((input[50]) >= (0.5)) {
			if ((input[66]) >= (30.5)) {
				var25 = -0.009488984;
			}
			else {
				var25 = 0.046605043;
			}
		}
		else {
			if ((input[58]) >= (0.2025)) {
				var25 = -0.038692288;
			}
			else {
				var25 = -0.098746754;
			}
		}
	}
	else {
		if ((input[12]) >= (7.01755)) {
			if ((input[66]) >= (285.5)) {
				var25 = -0.0070221126;
			}
			else {
				var25 = 0.033151645;
			}
		}
		else {
			if ((input[71]) >= (2559.5)) {
				var25 = 0.013569529;
			}
			else {
				var25 = 0.04467057;
			}
		}
	}
	double var26;
	if ((input[0]) >= (0.5)) {
		if ((input[1]) >= (4.02145)) {
			if ((input[57]) >= (0.44835)) {
				var26 = -0.0047667893;
			}
			else {
				var26 = -0.069081165;
			}
		}
		else {
			if ((input[70]) >= (3.5)) {
				var26 = 0.008086227;
			}
			else {
				var26 = 0.040747385;
			}
		}
	}
	else {
		if ((input[66]) >= (53.5)) {
			if ((input[57]) >= (1.6235499)) {
				var26 = -0.02494719;
			}
			else {
				var26 = -0.060985833;
			}
		}
		else {
			if ((input[56]) >= (1.48695)) {
				var26 = 0.053980302;
			}
			else {
				var26 = -0.013430418;
			}
		}
	}
	double var27;
	if ((input[70]) >= (9.5)) {
		if ((input[57]) >= (1.3975501)) {
			if ((input[12]) >= (21.099651)) {
				var27 = -0.04593945;
			}
			else {
				var27 = -0.0065271365;
			}
		}
		else {
			if ((input[58]) >= (28.44505)) {
				var27 = -0.040871322;
			}
			else {
				var27 = -0.08181729;
			}
		}
	}
	else {
		if ((input[71]) >= (13060.5)) {
			if ((input[64]) >= (45.5)) {
				var27 = -0.008831123;
			}
			else {
				var27 = -0.03425562;
			}
		}
		else {
			if ((input[26]) >= (5.29165)) {
				var27 = -0.0026124378;
			}
			else {
				var27 = 0.028102553;
			}
		}
	}
	double var28;
	if ((input[70]) >= (4.5)) {
		if ((input[57]) >= (1.11385)) {
			if ((input[70]) >= (15.5)) {
				var28 = -0.025439715;
			}
			else {
				var28 = 0.0032057378;
			}
		}
		else {
			if ((input[71]) >= (-19.5)) {
				var28 = -0.06362891;
			}
			else {
				var28 = 0.008447922;
			}
		}
	}
	else {
		if ((input[71]) >= (222.5)) {
			if ((input[64]) >= (43.5)) {
				var28 = 0.02346067;
			}
			else {
				var28 = -0.015089813;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var28 = 0.06815299;
			}
			else {
				var28 = 0.0053567323;
			}
		}
	}
	double var29;
	if ((input[49]) >= (2.125)) {
		if ((input[1]) >= (9.50975)) {
			if ((input[57]) >= (0.44494998)) {
				var29 = -0.023581004;
			}
			else {
				var29 = -0.08170462;
			}
		}
		else {
			if ((input[56]) >= (1.86265)) {
				var29 = 0.008888852;
			}
			else {
				var29 = -0.027395999;
			}
		}
	}
	else {
		if ((input[71]) >= (780.5)) {
			if ((input[64]) >= (43.5)) {
				var29 = 0.02260041;
			}
			else {
				var29 = -0.0063642263;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var29 = 0.060331788;
			}
			else {
				var29 = 0.009848752;
			}
		}
	}
	double var30;
	if ((input[4]) >= (3.5)) {
		if ((input[66]) >= (23.5)) {
			if ((input[58]) >= (0.5272)) {
				var30 = -0.009566697;
			}
			else {
				var30 = -0.064293854;
			}
		}
		else {
			if ((input[53]) >= (0.5)) {
				var30 = 0.026125643;
			}
			else {
				var30 = 0.0641553;
			}
		}
	}
	else {
		if ((input[66]) >= (327.5)) {
			if ((input[62]) >= (9.587549)) {
				var30 = 0.0142812;
			}
			else {
				var30 = -0.032721158;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var30 = 0.07159217;
			}
			else {
				var30 = 0.00812323;
			}
		}
	}
	double var31;
	if ((input[12]) >= (1.63865)) {
		if ((input[0]) >= (0.5)) {
			if ((input[64]) >= (40.5)) {
				var31 = 0.0068932907;
			}
			else {
				var31 = -0.01475654;
			}
		}
		else {
			if ((input[66]) >= (27.5)) {
				var31 = -0.041779563;
			}
			else {
				var31 = 0.038732678;
			}
		}
	}
	else {
		if ((input[64]) >= (37.5)) {
			if ((input[70]) >= (2.5)) {
				var31 = 0.011696725;
			}
			else {
				var31 = 0.0422752;
			}
		}
		else {
			if ((input[56]) >= (2.4615002)) {
				var31 = 0.017879702;
			}
			else {
				var31 = -0.025886208;
			}
		}
	}
	double var32;
	if ((input[1]) >= (0.75584996)) {
		if ((input[64]) >= (38.5)) {
			if ((input[66]) >= (183.5)) {
				var32 = -0.0030633924;
			}
			else {
				var32 = 0.05819684;
			}
		}
		else {
			if ((input[66]) >= (36.5)) {
				var32 = -0.036122497;
			}
			else {
				var32 = 0.026403109;
			}
		}
	}
	else {
		if ((input[70]) >= (1.5)) {
			if ((input[56]) >= (2.96285)) {
				var32 = 0.030106882;
			}
			else {
				var32 = -0.012034383;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var32 = 0.06108612;
			}
			else {
				var32 = 0.009279634;
			}
		}
	}
	double var33;
	if ((input[53]) >= (1.5)) {
		if ((input[57]) >= (0.6727)) {
			if ((input[70]) >= (8.5)) {
				var33 = -0.03518201;
			}
			else {
				var33 = -0.0029870707;
			}
		}
		else {
			if ((input[66]) >= (12.5)) {
				var33 = -0.077913426;
			}
			else {
				var33 = 0.0606058;
			}
		}
	}
	else {
		if ((input[66]) >= (42.5)) {
			if ((input[0]) >= (0.5)) {
				var33 = 0.007278026;
			}
			else {
				var33 = -0.022140125;
			}
		}
		else {
			if ((input[70]) >= (0.5)) {
				var33 = 0.038026;
			}
			else {
				var33 = 0.08737504;
			}
		}
	}
	double var34;
	if ((input[12]) >= (2.84965)) {
		if ((input[57]) >= (1.4409499)) {
			if ((input[71]) >= (17823.5)) {
				var34 = -0.023290012;
			}
			else {
				var34 = 0.0043521016;
			}
		}
		else {
			if ((input[66]) >= (45.5)) {
				var34 = -0.04368027;
			}
			else {
				var34 = 0.034603607;
			}
		}
	}
	else {
		if ((input[64]) >= (37.5)) {
			if ((input[66]) >= (420.5)) {
				var34 = 0.00841084;
			}
			else {
				var34 = 0.03452052;
			}
		}
		else {
			if ((input[62]) >= (1.5348)) {
				var34 = 0.010446429;
			}
			else {
				var34 = -0.031269755;
			}
		}
	}
	double var35;
	if ((input[70]) >= (0.5)) {
		if ((input[12]) >= (10.49025)) {
			if ((input[70]) >= (15.5)) {
				var35 = -0.038019344;
			}
			else {
				var35 = -0.011717888;
			}
		}
		else {
			if ((input[62]) >= (3.4399)) {
				var35 = 0.011217377;
			}
			else {
				var35 = -0.015301309;
			}
		}
	}
	else {
		if ((input[66]) >= (2216.5)) {
			if ((input[64]) >= (43.5)) {
				var35 = 0.023503214;
			}
			else {
				var35 = -0.052711632;
			}
		}
		else {
			if ((input[64]) >= (40.5)) {
				var35 = 0.086253636;
			}
			else {
				var35 = 0.03032121;
			}
		}
	}
	double var36;
	if ((input[53]) >= (0.5)) {
		if ((input[57]) >= (0.13525)) {
			if ((input[70]) >= (12.5)) {
				var36 = -0.02667885;
			}
			else {
				var36 = -0.0017870164;
			}
		}
		else {
			if ((input[66]) >= (18.5)) {
				var36 = -0.08436618;
			}
			else {
				var36 = 0.040133696;
			}
		}
	}
	else {
		if ((input[71]) >= (2101.5)) {
			if ((input[64]) >= (43.5)) {
				var36 = 0.0077157207;
			}
			else {
				var36 = -0.016676227;
			}
		}
		else {
			if ((input[64]) >= (39.5)) {
				var36 = 0.043213468;
			}
			else {
				var36 = 0.0065844553;
			}
		}
	}
	double var37;
	if ((input[66]) >= (46.5)) {
		if ((input[61]) >= (0.03505)) {
			if ((input[66]) >= (1339.5)) {
				var37 = -0.0053201467;
			}
			else {
				var37 = 0.023271367;
			}
		}
		else {
			if ((input[70]) >= (10.5)) {
				var37 = -0.061020363;
			}
			else {
				var37 = -0.01769552;
			}
		}
	}
	else {
		if ((input[64]) >= (38.5)) {
			if ((input[71]) >= (-66.5)) {
				var37 = 0.09317099;
			}
			else {
				var37 = 0.03448155;
			}
		}
		else {
			if ((input[56]) >= (0.00195)) {
				var37 = 0.029629407;
			}
			else {
				var37 = -0.024570681;
			}
		}
	}
	double var38;
	if ((input[15]) >= (3.5)) {
		if ((input[66]) >= (64.5)) {
			if ((input[57]) >= (1.57005)) {
				var38 = -0.0033809654;
			}
			else {
				var38 = -0.031365644;
			}
		}
		else {
			if ((input[56]) >= (2.46455)) {
				var38 = 0.04971688;
			}
			else {
				var38 = 0.006330854;
			}
		}
	}
	else {
		if ((input[0]) >= (6.0)) {
			if ((input[66]) >= (23.5)) {
				var38 = -0.010227698;
			}
			else {
				var38 = 0.04293306;
			}
		}
		else {
			if ((input[66]) >= (660.5)) {
				var38 = 0.010849483;
			}
			else {
				var38 = 0.056738596;
			}
		}
	}
	double var39;
	if ((input[1]) >= (0.19335)) {
		if ((input[64]) >= (38.5)) {
			if ((input[66]) >= (218.5)) {
				var39 = -0.0015324856;
			}
			else {
				var39 = 0.048213135;
			}
		}
		else {
			if ((input[66]) >= (67.5)) {
				var39 = -0.029110208;
			}
			else {
				var39 = 0.007293753;
			}
		}
	}
	else {
		if ((input[53]) >= (1.5)) {
			if ((input[57]) >= (0.0514)) {
				var39 = 0.0029538267;
			}
			else {
				var39 = -0.07237681;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var39 = 0.049981304;
			}
			else {
				var39 = 0.02025402;
			}
		}
	}
	double var40;
	if ((input[4]) >= (3.5)) {
		if ((input[12]) >= (2.8418)) {
			if ((input[57]) >= (1.10555)) {
				var40 = -0.010080847;
			}
			else {
				var40 = -0.043562975;
			}
		}
		else {
			if ((input[62]) >= (1.61985)) {
				var40 = 0.0117258085;
			}
			else {
				var40 = -0.012321717;
			}
		}
	}
	else {
		if ((input[0]) >= (6.0)) {
			if ((input[66]) >= (23.5)) {
				var40 = -0.008834104;
			}
			else {
				var40 = 0.038456343;
			}
		}
		else {
			if ((input[71]) >= (927.5)) {
				var40 = 0.007293626;
			}
			else {
				var40 = 0.045709487;
			}
		}
	}
	double var41;
	if ((input[51]) >= (3.5)) {
		if ((input[71]) >= (2730.5)) {
			if ((input[58]) >= (993.0941)) {
				var41 = 0.017507782;
			}
			else {
				var41 = -0.016453177;
			}
		}
		else {
			if ((input[64]) >= (40.5)) {
				var41 = 0.057977464;
			}
			else {
				var41 = 0.015102113;
			}
		}
	}
	else {
		if ((input[12]) >= (20.80665)) {
			if ((input[66]) >= (115.5)) {
				var41 = -0.028373925;
			}
			else {
				var41 = 0.02549685;
			}
		}
		else {
			if ((input[57]) >= (1.66025)) {
				var41 = 0.007653025;
			}
			else {
				var41 = -0.013255685;
			}
		}
	}
	double var42;
	if ((input[1]) >= (0.14645)) {
		if ((input[57]) >= (0.34085)) {
			if ((input[64]) >= (43.5)) {
				var42 = 0.013682181;
			}
			else {
				var42 = -0.005683319;
			}
		}
		else {
			if ((input[66]) >= (20.5)) {
				var42 = -0.057481945;
			}
			else {
				var42 = 0.04357441;
			}
		}
	}
	else {
		if ((input[58]) >= (7.2113)) {
			if ((input[66]) >= (754.5)) {
				var42 = 0.026382541;
			}
			else {
				var42 = 0.07106131;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var42 = 0.044336893;
			}
			else {
				var42 = -0.0021041965;
			}
		}
	}
	double var43;
	if ((input[1]) >= (7.85745)) {
		if ((input[66]) >= (87.5)) {
			if ((input[60]) >= (4.72075)) {
				var43 = -0.007380695;
			}
			else {
				var43 = -0.04250594;
			}
		}
		else {
			if ((input[58]) >= (782.6553)) {
				var43 = -0.05648696;
			}
			else {
				var43 = 0.038661182;
			}
		}
	}
	else {
		if ((input[56]) >= (9.724449)) {
			if ((input[71]) >= (822.5)) {
				var43 = 0.00411115;
			}
			else {
				var43 = 0.039938685;
			}
		}
		else {
			if ((input[66]) >= (112.5)) {
				var43 = -0.023156395;
			}
			else {
				var43 = 0.012615183;
			}
		}
	}
	double var44;
	if ((input[70]) >= (0.5)) {
		if ((input[0]) >= (0.5)) {
			if ((input[71]) >= (19752.5)) {
				var44 = -0.016006982;
			}
			else {
				var44 = 0.0059146443;
			}
		}
		else {
			if ((input[70]) >= (11.5)) {
				var44 = -0.04571582;
			}
			else {
				var44 = -0.009232096;
			}
		}
	}
	else {
		if ((input[66]) >= (2460.5)) {
			if ((input[64]) >= (43.5)) {
				var44 = 0.01786447;
			}
			else {
				var44 = -0.04559665;
			}
		}
		else {
			if ((input[64]) >= (43.5)) {
				var44 = 0.10773213;
			}
			else {
				var44 = 0.032248605;
			}
		}
	}
	double var45;
	if ((input[53]) >= (1.5)) {
		if ((input[57]) >= (0.6727)) {
			if ((input[70]) >= (13.5)) {
				var45 = -0.03002647;
			}
			else {
				var45 = -0.0029566146;
			}
		}
		else {
			if ((input[66]) >= (12.5)) {
				var45 = -0.06166042;
			}
			else {
				var45 = 0.051144805;
			}
		}
	}
	else {
		if ((input[66]) >= (232.5)) {
			if ((input[56]) >= (9.7696)) {
				var45 = 0.0031279065;
			}
			else {
				var45 = -0.028263444;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var45 = 0.059483893;
			}
			else {
				var45 = 0.0051194555;
			}
		}
	}
	double var46;
	if ((input[1]) >= (1.73245)) {
		if ((input[60]) >= (2.76755)) {
			if ((input[66]) >= (738.5)) {
				var46 = -0.0049750046;
			}
			else {
				var46 = 0.027044712;
			}
		}
		else {
			if ((input[66]) >= (71.5)) {
				var46 = -0.03711413;
			}
			else {
				var46 = 0.010033926;
			}
		}
	}
	else {
		if ((input[56]) >= (4.6581497)) {
			if ((input[66]) >= (837.5)) {
				var46 = 0.0074556344;
			}
			else {
				var46 = 0.03887755;
			}
		}
		else {
			if ((input[66]) >= (12.5)) {
				var46 = -0.006312366;
			}
			else {
				var46 = 0.047005296;
			}
		}
	}
	double var47;
	if ((input[15]) >= (3.5)) {
		if ((input[1]) >= (0.087850004)) {
			if ((input[58]) >= (0.5272)) {
				var47 = -0.005576026;
			}
			else {
				var47 = -0.04159057;
			}
		}
		else {
			if ((input[55]) >= (83.96484)) {
				var47 = -0.013103351;
			}
			else {
				var47 = 0.034272145;
			}
		}
	}
	else {
		if ((input[0]) >= (6.0)) {
			if ((input[66]) >= (28.5)) {
				var47 = -0.008991274;
			}
			else {
				var47 = 0.027911684;
			}
		}
		else {
			if ((input[66]) >= (288.5)) {
				var47 = 0.0106781;
			}
			else {
				var47 = 0.057505872;
			}
		}
	}
	double var48;
	if ((input[64]) >= (43.5)) {
		if ((input[66]) >= (5468.5)) {
			if ((input[60]) >= (3.76365)) {
				var48 = -0.0005695702;
			}
			else {
				var48 = -0.06387183;
			}
		}
		else {
			if ((input[70]) >= (1.5)) {
				var48 = 0.014444423;
			}
			else {
				var48 = 0.08040762;
			}
		}
	}
	else {
		if ((input[66]) >= (231.5)) {
			if ((input[56]) >= (9.77205)) {
				var48 = -0.004746162;
			}
			else {
				var48 = -0.038350206;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var48 = 0.04535659;
			}
			else {
				var48 = -0.0007378073;
			}
		}
	}
	double var49;
	if ((input[1]) >= (14.060551)) {
		if ((input[66]) >= (1330.5)) {
			if ((input[57]) >= (2.16415)) {
				var49 = -0.012828796;
			}
			else {
				var49 = -0.040836465;
			}
		}
		else {
			if ((input[70]) >= (10.5)) {
				var49 = -0.047457773;
			}
			else {
				var49 = 0.018942727;
			}
		}
	}
	else {
		if ((input[61]) >= (0.06805)) {
			if ((input[71]) >= (812.5)) {
				var49 = 0.0035940637;
			}
			else {
				var49 = 0.033267897;
			}
		}
		else {
			if ((input[66]) >= (53.5)) {
				var49 = -0.012142361;
			}
			else {
				var49 = 0.018090114;
			}
		}
	}
	double var50;
	if ((input[4]) >= (3.5)) {
		if ((input[66]) >= (112.5)) {
			if ((input[57]) >= (1.53405)) {
				var50 = -0.00301265;
			}
			else {
				var50 = -0.026776388;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var50 = 0.051568527;
			}
			else {
				var50 = 0.0030392797;
			}
		}
	}
	else {
		if ((input[0]) >= (6.0)) {
			if ((input[55]) >= (36.86135)) {
				var50 = -0.012155152;
			}
			else {
				var50 = 0.011838858;
			}
		}
		else {
			if ((input[71]) >= (923.5)) {
				var50 = 0.00635807;
			}
			else {
				var50 = 0.03633296;
			}
		}
	}
	double var51;
	if ((input[64]) >= (43.5)) {
		if ((input[66]) >= (6106.5)) {
			if ((input[71]) >= (24349.5)) {
				var51 = -0.015052457;
			}
			else {
				var51 = 0.0076559247;
			}
		}
		else {
			if ((input[70]) >= (1.5)) {
				var51 = 0.013070794;
			}
			else {
				var51 = 0.074082285;
			}
		}
	}
	else {
		if ((input[66]) >= (177.5)) {
			if ((input[58]) >= (9.375851)) {
				var51 = -0.003746966;
			}
			else {
				var51 = -0.03224239;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var51 = 0.046907306;
			}
			else {
				var51 = -0.00014554955;
			}
		}
	}
	double var52;
	if ((input[12]) >= (1.05275)) {
		if ((input[57]) >= (1.5218501)) {
			if ((input[71]) >= (21071.0)) {
				var52 = -0.016194358;
			}
			else {
				var52 = 0.0062489393;
			}
		}
		else {
			if ((input[66]) >= (28.5)) {
				var52 = -0.023413312;
			}
			else {
				var52 = 0.024954902;
			}
		}
	}
	else {
		if ((input[56]) >= (4.65615)) {
			if ((input[66]) >= (551.5)) {
				var52 = 0.010904825;
			}
			else {
				var52 = 0.040795665;
			}
		}
		else {
			if ((input[66]) >= (12.5)) {
				var52 = -0.0033492134;
			}
			else {
				var52 = 0.04092401;
			}
		}
	}
	double var53;
	if ((input[53]) >= (1.5)) {
		if ((input[46]) >= (4.6321497)) {
			if ((input[62]) >= (0.101349995)) {
				var53 = -0.002142062;
			}
			else {
				var53 = -0.051075656;
			}
		}
		else {
			if ((input[58]) >= (0.744)) {
				var53 = -0.031710796;
			}
			else {
				var53 = -0.07514583;
			}
		}
	}
	else {
		if ((input[71]) >= (2725.5)) {
			if ((input[58]) >= (827.4404)) {
				var53 = 0.0025601278;
			}
			else {
				var53 = -0.021826757;
			}
		}
		else {
			if ((input[64]) >= (43.5)) {
				var53 = 0.05340861;
			}
			else {
				var53 = 0.0059262966;
			}
		}
	}
	double var54;
	if ((input[0]) >= (0.5)) {
		if ((input[71]) >= (61974.5)) {
			if ((input[70]) >= (7.5)) {
				var54 = -0.00827771;
			}
			else {
				var54 = -0.04152512;
			}
		}
		else {
			if ((input[64]) >= (40.5)) {
				var54 = 0.013116725;
			}
			else {
				var54 = -0.0027165334;
			}
		}
	}
	else {
		if ((input[70]) >= (13.5)) {
			if ((input[51]) >= (0.5)) {
				var54 = -0.016700782;
			}
			else {
				var54 = -0.06153651;
			}
		}
		else {
			if ((input[71]) >= (1121.5)) {
				var54 = -0.034730803;
			}
			else {
				var54 = -0.0006348456;
			}
		}
	}
	double var55;
	if ((input[12]) >= (10.93165)) {
		if ((input[64]) >= (46.5)) {
			if ((input[66]) >= (7777.5)) {
				var55 = 0.002826101;
			}
			else {
				var55 = 0.073823504;
			}
		}
		else {
			if ((input[66]) >= (267.5)) {
				var55 = -0.016678704;
			}
			else {
				var55 = 0.012418674;
			}
		}
	}
	else {
		if ((input[61]) >= (0.06925)) {
			if ((input[71]) >= (798.5)) {
				var55 = 0.004349118;
			}
			else {
				var55 = 0.030403256;
			}
		}
		else {
			if ((input[66]) >= (30.5)) {
				var55 = -0.008305963;
			}
			else {
				var55 = 0.02114661;
			}
		}
	}
	double var56;
	if ((input[57]) >= (2.16475)) {
		if ((input[71]) >= (12818.5)) {
			if ((input[70]) >= (6.5)) {
				var56 = 0.004833219;
			}
			else {
				var56 = -0.020158965;
			}
		}
		else {
			if ((input[70]) >= (15.5)) {
				var56 = -0.0073696286;
			}
			else {
				var56 = 0.020855328;
			}
		}
	}
	else {
		if ((input[66]) >= (112.5)) {
			if ((input[64]) >= (43.5)) {
				var56 = 0.010830724;
			}
			else {
				var56 = -0.016439008;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var56 = 0.0536154;
			}
			else {
				var56 = -0.0018127932;
			}
		}
	}
	double var57;
	if ((input[61]) >= (0.00014999999)) {
		if ((input[66]) >= (483.5)) {
			if ((input[58]) >= (29.4789)) {
				var57 = 0.00022401827;
			}
			else {
				var57 = -0.02744922;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var57 = 0.031986766;
			}
			else {
				var57 = -0.00019857679;
			}
		}
	}
	else {
		if ((input[70]) >= (1.5)) {
			if ((input[71]) >= (8.5)) {
				var57 = -0.102982044;
			}
			else {
				var57 = -0.040077016;
			}
		}
		else {
			if ((input[71]) >= (14.5)) {
				var57 = -0.03255492;
			}
			else {
				var57 = 0.08684751;
			}
		}
	}
	double var58;
	if ((input[64]) >= (40.5)) {
		if ((input[66]) >= (2935.5)) {
			if ((input[58]) >= (827.29297)) {
				var58 = 0.006245687;
			}
			else {
				var58 = -0.020490939;
			}
		}
		else {
			if ((input[70]) >= (2.5)) {
				var58 = 0.0019211164;
			}
			else {
				var58 = 0.04219815;
			}
		}
	}
	else {
		if ((input[66]) >= (46.5)) {
			if ((input[56]) >= (0.27225)) {
				var58 = -0.006560148;
			}
			else {
				var58 = -0.06450589;
			}
		}
		else {
			if ((input[62]) >= (3.61205)) {
				var58 = 0.040122192;
			}
			else {
				var58 = 0.009459233;
			}
		}
	}
	double var59;
	if ((input[1]) >= (16.673851)) {
		if ((input[66]) >= (1806.5)) {
			if ((input[60]) >= (17.576149)) {
				var59 = -0.01054492;
			}
			else {
				var59 = -0.036552183;
			}
		}
		else {
			if ((input[70]) >= (14.5)) {
				var59 = -0.04697601;
			}
			else {
				var59 = 0.0143190175;
			}
		}
	}
	else {
		if ((input[58]) >= (9.36655)) {
			if ((input[71]) >= (317.5)) {
				var59 = 0.0034752998;
			}
			else {
				var59 = 0.030020773;
			}
		}
		else {
			if ((input[66]) >= (64.5)) {
				var59 = -0.015949627;
			}
			else {
				var59 = 0.009581593;
			}
		}
	}
	double var60;
	if ((input[12]) >= (0.087850004)) {
		if ((input[57]) >= (0.98875)) {
			if ((input[71]) >= (490.5)) {
				var60 = -0.0034682916;
			}
			else {
				var60 = 0.011946882;
			}
		}
		else {
			if ((input[66]) >= (14.5)) {
				var60 = -0.023315815;
			}
			else {
				var60 = 0.031899266;
			}
		}
	}
	else {
		if ((input[58]) >= (6.6591)) {
			if ((input[71]) >= (4439.0)) {
				var60 = 0.01020207;
			}
			else {
				var60 = 0.050072517;
			}
		}
		else {
			if ((input[55]) >= (53.87695)) {
				var60 = -0.019742075;
			}
			else {
				var60 = 0.02131225;
			}
		}
	}
	double var61;
	if ((input[64]) >= (43.5)) {
		if ((input[66]) >= (7572.5)) {
			if ((input[57]) >= (2.5247998)) {
				var61 = 0.004219027;
			}
			else {
				var61 = -0.01916994;
			}
		}
		else {
			if ((input[70]) >= (1.5)) {
				var61 = 0.0081853885;
			}
			else {
				var61 = 0.062208664;
			}
		}
	}
	else {
		if ((input[1]) >= (3.58395)) {
			if ((input[46]) >= (4.86135)) {
				var61 = -0.006120724;
			}
			else {
				var61 = -0.026785119;
			}
		}
		else {
			if ((input[58]) >= (6.92555)) {
				var61 = 0.010689543;
			}
			else {
				var61 = -0.0047313613;
			}
		}
	}
	double var62;
	if ((input[0]) >= (0.5)) {
		if ((input[71]) >= (71362.5)) {
			if ((input[70]) >= (7.5)) {
				var62 = -0.006919469;
			}
			else {
				var62 = -0.041467663;
			}
		}
		else {
			if ((input[57]) >= (2.36695)) {
				var62 = 0.012031357;
			}
			else {
				var62 = -0.0015013574;
			}
		}
	}
	else {
		if ((input[70]) >= (14.5)) {
			if ((input[51]) >= (0.5)) {
				var62 = -0.01260563;
			}
			else {
				var62 = -0.057433754;
			}
		}
		else {
			if ((input[71]) >= (4787.0)) {
				var62 = -0.0466659;
			}
			else {
				var62 = -0.0035793607;
			}
		}
	}
	double var63;
	if ((input[53]) >= (0.5)) {
		if ((input[46]) >= (3.79455)) {
			if ((input[61]) >= (0.00014999999)) {
				var63 = -0.0014015778;
			}
			else {
				var63 = -0.048546534;
			}
		}
		else {
			if ((input[57]) >= (1.87535)) {
				var63 = -0.0053494573;
			}
			else {
				var63 = -0.05225438;
			}
		}
	}
	else {
		if ((input[71]) >= (1749.5)) {
			if ((input[64]) >= (45.5)) {
				var63 = 0.014443256;
			}
			else {
				var63 = -0.00860277;
			}
		}
		else {
			if ((input[64]) >= (40.5)) {
				var63 = 0.0350691;
			}
			else {
				var63 = 0.00539357;
			}
		}
	}
	double var64;
	if ((input[12]) >= (0.087850004)) {
		if ((input[57]) >= (0.09735)) {
			if ((input[64]) >= (43.5)) {
				var64 = 0.009399611;
			}
			else {
				var64 = -0.0037587944;
			}
		}
		else {
			if ((input[66]) >= (17.5)) {
				var64 = -0.050955333;
			}
			else {
				var64 = 0.04031911;
			}
		}
	}
	else {
		if ((input[13]) >= (1.5)) {
			if ((input[53]) >= (0.5)) {
				var64 = 0.009908321;
			}
			else {
				var64 = 0.038984414;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var64 = 0.024768157;
			}
			else {
				var64 = -0.015650438;
			}
		}
	}
	double var65;
	if ((input[12]) >= (18.412151)) {
		if ((input[66]) >= (879.5)) {
			if ((input[61]) >= (0.23414999)) {
				var65 = -0.006647465;
			}
			else {
				var65 = -0.029556653;
			}
		}
		else {
			if ((input[70]) >= (14.5)) {
				var65 = -0.05178518;
			}
			else {
				var65 = 0.015413743;
			}
		}
	}
	else {
		if ((input[61]) >= (0.06805)) {
			if ((input[71]) >= (540.5)) {
				var65 = 0.0030447731;
			}
			else {
				var65 = 0.02611577;
			}
		}
		else {
			if ((input[66]) >= (109.5)) {
				var65 = -0.0111729745;
			}
			else {
				var65 = 0.0074457424;
			}
		}
	}
	double var66;
	if ((input[0]) >= (0.5)) {
		if ((input[71]) >= (19752.5)) {
			if ((input[70]) >= (6.5)) {
				var66 = 0.0067955772;
			}
			else {
				var66 = -0.024014203;
			}
		}
		else {
			if ((input[57]) >= (2.5718498)) {
				var66 = 0.01870562;
			}
			else {
				var66 = 0.000030075907;
			}
		}
	}
	else {
		if ((input[66]) >= (5626.5)) {
			if ((input[49]) >= (8.875)) {
				var66 = -0.061165195;
			}
			else {
				var66 = -0.024395153;
			}
		}
		else {
			if ((input[46]) >= (4.62285)) {
				var66 = -0.0011236737;
			}
			else {
				var66 = -0.025703697;
			}
		}
	}
	double var67;
	if ((input[64]) >= (40.5)) {
		if ((input[66]) >= (2935.5)) {
			if ((input[58]) >= (827.29297)) {
				var67 = 0.0059707365;
			}
			else {
				var67 = -0.018033182;
			}
		}
		else {
			if ((input[70]) >= (2.5)) {
				var67 = 0.0010280035;
			}
			else {
				var67 = 0.036754142;
			}
		}
	}
	else {
		if ((input[66]) >= (153.5)) {
			if ((input[70]) >= (1.5)) {
				var67 = -0.005839445;
			}
			else {
				var67 = -0.0374895;
			}
		}
		else {
			if ((input[58]) >= (6.4334497)) {
				var67 = 0.043671176;
			}
			else {
				var67 = -0.0021197728;
			}
		}
	}
	double var68;
	if ((input[70]) >= (433.5)) {
		if ((input[46]) >= (6.417)) {
			if ((input[1]) >= (14.4258)) {
				var68 = -0.040311422;
			}
			else {
				var68 = 0.23452397;
			}
		}
		else {
			if ((input[16]) >= (9.45)) {
				var68 = -0.08369305;
			}
			else {
				var68 = -0.043605722;
			}
		}
	}
	else {
		if ((input[57]) >= (1.87305)) {
			if ((input[71]) >= (17813.5)) {
				var68 = -0.00897704;
			}
			else {
				var68 = 0.009528518;
			}
		}
		else {
			if ((input[66]) >= (23.5)) {
				var68 = -0.008460189;
			}
			else {
				var68 = 0.0215132;
			}
		}
	}
	double var69;
	if ((input[64]) >= (43.5)) {
		if ((input[66]) >= (5468.5)) {
			if ((input[56]) >= (42.859703)) {
				var69 = 0.0012519917;
			}
			else {
				var69 = -0.0604895;
			}
		}
		else {
			if ((input[70]) >= (1.5)) {
				var69 = 0.0061816536;
			}
			else {
				var69 = 0.060698003;
			}
		}
	}
	else {
		if ((input[66]) >= (232.5)) {
			if ((input[58]) >= (29.4315)) {
				var69 = -0.002006335;
			}
			else {
				var69 = -0.023182867;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var69 = 0.03276785;
			}
			else {
				var69 = -0.00051381096;
			}
		}
	}
	double var70;
	if ((input[12]) >= (1.11135)) {
		if ((input[60]) >= (3.97075)) {
			if ((input[71]) >= (526.5)) {
				var70 = -0.0017447369;
			}
			else {
				var70 = 0.02181388;
			}
		}
		else {
			if ((input[66]) >= (258.5)) {
				var70 = -0.02679495;
			}
			else {
				var70 = -0.0020986313;
			}
		}
	}
	else {
		if ((input[58]) >= (20.091301)) {
			if ((input[66]) >= (2850.5)) {
				var70 = 0.0027266457;
			}
			else {
				var70 = 0.030513814;
			}
		}
		else {
			if ((input[71]) >= (41.5)) {
				var70 = -0.0040387865;
			}
			else {
				var70 = 0.012286576;
			}
		}
	}
	double var71;
	if ((input[46]) >= (4.9050503)) {
		if ((input[71]) >= (2188.5)) {
			if ((input[58]) >= (827.29297)) {
				var71 = 0.004045828;
			}
			else {
				var71 = -0.01621052;
			}
		}
		else {
			if ((input[58]) >= (29.3679)) {
				var71 = 0.020419674;
			}
			else {
				var71 = 0.0006305289;
			}
		}
	}
	else {
		if ((input[53]) >= (0.5)) {
			if ((input[57]) >= (2.90305)) {
				var71 = 0.017628256;
			}
			else {
				var71 = -0.025719443;
			}
		}
		else {
			if ((input[71]) >= (20.5)) {
				var71 = -0.0014942138;
			}
			else {
				var71 = 0.03197592;
			}
		}
	}
	double var72;
	if ((input[64]) >= (46.5)) {
		if ((input[66]) >= (11980.5)) {
			if ((input[61]) >= (0.24785)) {
				var72 = 0.013543698;
			}
			else {
				var72 = -0.015066764;
			}
		}
		else {
			if ((input[70]) >= (3.5)) {
				var72 = 0.0041547376;
			}
			else {
				var72 = 0.074540325;
			}
		}
	}
	else {
		if ((input[12]) >= (10.93165)) {
			if ((input[66]) >= (879.5)) {
				var72 = -0.01456271;
			}
			else {
				var72 = 0.0039694225;
			}
		}
		else {
			if ((input[3]) >= (1.775)) {
				var72 = 0.0057085035;
			}
			else {
				var72 = -0.008693646;
			}
		}
	}
	double var73;
	if ((input[66]) >= (-352.5)) {
		if ((input[64]) >= (40.5)) {
			if ((input[66]) >= (419.5)) {
				var73 = 0.0019420615;
			}
			else {
				var73 = 0.037225865;
			}
		}
		else {
			if ((input[66]) >= (72.5)) {
				var73 = -0.0076865987;
			}
			else {
				var73 = 0.00934043;
			}
		}
	}
	else {
		if ((input[3]) >= (2.325)) {
			if ((input[70]) >= (2.5)) {
				var73 = -0.094235785;
			}
			else {
				var73 = -0.37696916;
			}
		}
		else {
			if ((input[1]) >= (18.2207)) {
				var73 = 0.03539167;
			}
			else {
				var73 = -0.06452107;
			}
		}
	}
	double var74;
	if ((input[57]) >= (0.069800004)) {
		if ((input[71]) >= (71212.0)) {
			if ((input[70]) >= (7.5)) {
				var74 = -0.0068971626;
			}
			else {
				var74 = -0.036482867;
			}
		}
		else {
			if ((input[64]) >= (45.5)) {
				var74 = 0.016414708;
			}
			else {
				var74 = 0.000027144828;
			}
		}
	}
	else {
		if ((input[70]) >= (1.5)) {
			if ((input[71]) >= (11.5)) {
				var74 = -0.08625395;
			}
			else {
				var74 = -0.030704005;
			}
		}
		else {
			if ((input[71]) >= (11.5)) {
				var74 = -0.023373803;
			}
			else {
				var74 = 0.08483125;
			}
		}
	}
	double var75;
	if ((input[46]) >= (5.36445)) {
		if ((input[71]) >= (17823.5)) {
			if ((input[70]) >= (5.5)) {
				var75 = 0.0027795834;
			}
			else {
				var75 = -0.020457685;
			}
		}
		else {
			if ((input[58]) >= (830.6571)) {
				var75 = 0.02152396;
			}
			else {
				var75 = 0.003217565;
			}
		}
	}
	else {
		if ((input[53]) >= (1.5)) {
			if ((input[57]) >= (0.99)) {
				var75 = -0.01082733;
			}
			else {
				var75 = -0.0407503;
			}
		}
		else {
			if ((input[71]) >= (30.5)) {
				var75 = -0.004942917;
			}
			else {
				var75 = 0.01434713;
			}
		}
	}
	double var76;
	if ((input[70]) >= (433.5)) {
		if ((input[46]) >= (6.417)) {
			if ((input[33]) >= (2.325)) {
				var76 = -0.056442987;
			}
			else {
				var76 = 0.18662642;
			}
		}
		else {
			if ((input[16]) >= (9.45)) {
				var76 = -0.08047079;
			}
			else {
				var76 = -0.03992888;
			}
		}
	}
	else {
		if ((input[57]) >= (2.16475)) {
			if ((input[71]) >= (10656.5)) {
				var76 = -0.004319811;
			}
			else {
				var76 = 0.011895068;
			}
		}
		else {
			if ((input[1]) >= (1.65035)) {
				var76 = -0.010329596;
			}
			else {
				var76 = 0.003946993;
			}
		}
	}
	double var77;
	if ((input[0]) >= (0.5)) {
		if ((input[66]) >= (475.5)) {
			if ((input[58]) >= (12.31005)) {
				var77 = 0.0014069054;
			}
			else {
				var77 = -0.030587858;
			}
		}
		else {
			if ((input[61]) >= (0.059550002)) {
				var77 = 0.03426076;
			}
			else {
				var77 = 0.0031746272;
			}
		}
	}
	else {
		if ((input[1]) >= (3.16605)) {
			if ((input[50]) >= (0.5)) {
				var77 = -0.011431595;
			}
			else {
				var77 = -0.0365729;
			}
		}
		else {
			if ((input[58]) >= (0.9984)) {
				var77 = 0.0062474757;
			}
			else {
				var77 = -0.019075733;
			}
		}
	}
	double var78;
	if ((input[64]) >= (43.5)) {
		if ((input[66]) >= (2941.5)) {
			if ((input[58]) >= (111.931046)) {
				var78 = 0.0044938093;
			}
			else {
				var78 = -0.03841607;
			}
		}
		else {
			if ((input[70]) >= (1.5)) {
				var78 = 0.0065303952;
			}
			else {
				var78 = 0.064987384;
			}
		}
	}
	else {
		if ((input[66]) >= (258.5)) {
			if ((input[70]) >= (2.5)) {
				var78 = -0.0017675617;
			}
			else {
				var78 = -0.019637689;
			}
		}
		else {
			if ((input[58]) >= (16.9363)) {
				var78 = 0.039136704;
			}
			else {
				var78 = 0.0009349606;
			}
		}
	}
	double var79;
	if ((input[66]) >= (-352.5)) {
		if ((input[64]) >= (39.5)) {
			if ((input[66]) >= (698.5)) {
				var79 = -0.00058233866;
			}
			else {
				var79 = 0.024604723;
			}
		}
		else {
			if ((input[66]) >= (36.5)) {
				var79 = -0.008432022;
			}
			else {
				var79 = 0.012613343;
			}
		}
	}
	else {
		if ((input[56]) >= (4.01115)) {
			if ((input[70]) >= (4.5)) {
				var79 = -0.0662454;
			}
			else {
				var79 = -0.16211753;
			}
		}
		else {
			if ((input[66]) >= (-1406.5)) {
				var79 = -0.020742754;
			}
			else {
				var79 = 0.11789002;
			}
		}
	}
	double var80;
	if ((input[46]) >= (5.36445)) {
		if ((input[71]) >= (17823.5)) {
			if ((input[70]) >= (6.5)) {
				var80 = 0.0050060647;
			}
			else {
				var80 = -0.016603542;
			}
		}
		else {
			if ((input[56]) >= (2135.297)) {
				var80 = 0.037104275;
			}
			else {
				var80 = 0.0049399496;
			}
		}
	}
	else {
		if ((input[53]) >= (1.5)) {
			if ((input[46]) >= (3.36195)) {
				var80 = -0.012545796;
			}
			else {
				var80 = -0.05148297;
			}
		}
		else {
			if ((input[52]) >= (0.5)) {
				var80 = -0.009717817;
			}
			else {
				var80 = 0.0049489625;
			}
		}
	}
	double var81;
	if ((input[70]) >= (297.5)) {
		if ((input[70]) >= (501.5)) {
			if ((input[46]) >= (6.4169)) {
				var81 = 0.0867678;
			}
			else {
				var81 = -0.07704136;
			}
		}
		else {
			if ((input[16]) >= (7.1)) {
				var81 = -0.037549544;
			}
			else {
				var81 = 0.0049396744;
			}
		}
	}
	else {
		if ((input[57]) >= (2.57545)) {
			if ((input[71]) >= (42418.0)) {
				var81 = -0.013035265;
			}
			else {
				var81 = 0.011482792;
			}
		}
		else {
			if ((input[71]) >= (803.5)) {
				var81 = -0.009745472;
			}
			else {
				var81 = 0.0017701352;
			}
		}
	}
	double var82;
	if ((input[64]) >= (46.5)) {
		if ((input[66]) >= (11980.5)) {
			if ((input[61]) >= (0.24785)) {
				var82 = 0.013217151;
			}
			else {
				var82 = -0.012514946;
			}
		}
		else {
			if ((input[70]) >= (2.5)) {
				var82 = 0.008732547;
			}
			else {
				var82 = 0.06982686;
			}
		}
	}
	else {
		if ((input[3]) >= (1.775)) {
			if ((input[12]) >= (19.58395)) {
				var82 = -0.014042682;
			}
			else {
				var82 = 0.0042289454;
			}
		}
		else {
			if ((input[70]) >= (10.5)) {
				var82 = -0.02866206;
			}
			else {
				var82 = -0.005254666;
			}
		}
	}
	double var83;
	if ((input[9]) >= (1.5)) {
		if ((input[46]) >= (4.89125)) {
			if ((input[71]) >= (-288.5)) {
				var83 = -0.0012371063;
			}
			else {
				var83 = -0.07131303;
			}
		}
		else {
			if ((input[70]) >= (2.5)) {
				var83 = -0.024774684;
			}
			else {
				var83 = 0.0019839809;
			}
		}
	}
	else {
		if ((input[2]) >= (1.5)) {
			if ((input[71]) >= (490.5)) {
				var83 = 0.0025234923;
			}
			else {
				var83 = 0.019890677;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var83 = -0.016497783;
			}
			else {
				var83 = 0.000080841266;
			}
		}
	}
	double var84;
	if ((input[1]) >= (0.11525001)) {
		if ((input[56]) >= (0.88815)) {
			if ((input[66]) >= (475.5)) {
				var84 = -0.0024848997;
			}
			else {
				var84 = 0.008690813;
			}
		}
		else {
			if ((input[66]) >= (14.5)) {
				var84 = -0.024226176;
			}
			else {
				var84 = 0.029334515;
			}
		}
	}
	else {
		if ((input[55]) >= (84.03905)) {
			if ((input[58]) >= (4.573)) {
				var84 = 0.013908523;
			}
			else {
				var84 = -0.030043034;
			}
		}
		else {
			if ((input[14]) >= (1.325)) {
				var84 = 0.027445767;
			}
			else {
				var84 = 0.0007381421;
			}
		}
	}
	double var85;
	if ((input[61]) >= (0.09655)) {
		if ((input[66]) >= (1403.5)) {
			if ((input[58]) >= (708.07117)) {
				var85 = 0.005369397;
			}
			else {
				var85 = -0.011383576;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var85 = 0.03760736;
			}
			else {
				var85 = -0.0037710776;
			}
		}
	}
	else {
		if ((input[66]) >= (116.5)) {
			if ((input[46]) >= (5.36495)) {
				var85 = -0.0013561228;
			}
			else {
				var85 = -0.02115742;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var85 = 0.037640836;
			}
			else {
				var85 = -0.00014635379;
			}
		}
	}
	double var86;
	if ((input[66]) >= (-352.5)) {
		if ((input[64]) >= (43.5)) {
			if ((input[66]) >= (2941.5)) {
				var86 = 0.0017884945;
			}
			else {
				var86 = 0.037656736;
			}
		}
		else {
			if ((input[66]) >= (177.5)) {
				var86 = -0.005074057;
			}
			else {
				var86 = 0.00692285;
			}
		}
	}
	else {
		if ((input[56]) >= (16.762901)) {
			if ((input[70]) >= (2.5)) {
				var86 = -0.080928795;
			}
			else {
				var86 = -0.20053439;
			}
		}
		else {
			if ((input[62]) >= (0.49230003)) {
				var86 = -0.039595474;
			}
			else {
				var86 = 0.070279025;
			}
		}
	}
	double var87;
	if ((input[0]) >= (0.5)) {
		if ((input[0]) >= (1.5)) {
			if ((input[55]) >= (46.06835)) {
				var87 = -0.003982224;
			}
			else {
				var87 = 0.007270156;
			}
		}
		else {
			if ((input[71]) >= (2157.5)) {
				var87 = 0.0023855034;
			}
			else {
				var87 = 0.033152875;
			}
		}
	}
	else {
		if ((input[1]) >= (3.16605)) {
			if ((input[71]) >= (-3.5)) {
				var87 = -0.019857427;
			}
			else {
				var87 = 0.026267478;
			}
		}
		else {
			if ((input[68]) >= (0.5)) {
				var87 = 0.015201797;
			}
			else {
				var87 = -0.008489582;
			}
		}
	}
	double var88;
	if ((input[58]) >= (17.493649)) {
		if ((input[66]) >= (965.5)) {
			if ((input[58]) >= (760.83435)) {
				var88 = 0.0051464536;
			}
			else {
				var88 = -0.008198132;
			}
		}
		else {
			if ((input[70]) >= (12.5)) {
				var88 = -0.020202205;
			}
			else {
				var88 = 0.02505351;
			}
		}
	}
	else {
		if ((input[66]) >= (372.5)) {
			if ((input[22]) >= (3.11525)) {
				var88 = -0.035765283;
			}
			else {
				var88 = -0.0061057913;
			}
		}
		else {
			if ((input[64]) >= (39.5)) {
				var88 = 0.020973282;
			}
			else {
				var88 = -0.0051123537;
			}
		}
	}
	double var89;
	if ((input[46]) >= (2.85085)) {
		if ((input[70]) >= (251.5)) {
			if ((input[64]) >= (38.5)) {
				var89 = -0.016181933;
			}
			else {
				var89 = -0.05378752;
			}
		}
		else {
			if ((input[60]) >= (32.892548)) {
				var89 = 0.013585016;
			}
			else {
				var89 = -0.00032886275;
			}
		}
	}
	else {
		if ((input[49]) >= (0.125)) {
			if ((input[66]) >= (10.5)) {
				var89 = -0.049555466;
			}
			else {
				var89 = 0.07755273;
			}
		}
		else {
			if ((input[71]) >= (115.5)) {
				var89 = 0.0025304593;
			}
			else {
				var89 = 0.06732865;
			}
		}
	}
	double var90;
	if ((input[66]) >= (-143.5)) {
		if ((input[64]) >= (46.5)) {
			if ((input[66]) >= (6106.5)) {
				var90 = 0.0066873366;
			}
			else {
				var90 = 0.068786226;
			}
		}
		else {
			if ((input[1]) >= (21.0918)) {
				var90 = -0.011250588;
			}
			else {
				var90 = 0.00093227084;
			}
		}
	}
	else {
		if ((input[58]) >= (4.0028)) {
			if ((input[70]) >= (2.5)) {
				var90 = -0.06019345;
			}
			else {
				var90 = -0.13247119;
			}
		}
		else {
			if ((input[62]) >= (0.44005)) {
				var90 = -0.014215916;
			}
			else {
				var90 = 0.070367396;
			}
		}
	}
	double var91;
	if ((input[71]) >= (93754.0)) {
		if ((input[68]) >= (0.5)) {
			if ((input[47]) >= (2.05555)) {
				var91 = -0.011894665;
			}
			else {
				var91 = 0.03281426;
			}
		}
		else {
			if ((input[24]) >= (0.675)) {
				var91 = -0.027947828;
			}
			else {
				var91 = -0.07857425;
			}
		}
	}
	else {
		if ((input[57]) >= (2.36695)) {
			if ((input[71]) >= (10656.5)) {
				var91 = -0.0018562757;
			}
			else {
				var91 = 0.012103813;
			}
		}
		else {
			if ((input[46]) >= (5.27065)) {
				var91 = 0.001536933;
			}
			else {
				var91 = -0.009382282;
			}
		}
	}
	double var92;
	if ((input[62]) >= (0.10155)) {
		if ((input[66]) >= (475.5)) {
			if ((input[58]) >= (50.53305)) {
				var92 = 0.0009690661;
			}
			else {
				var92 = -0.016936699;
			}
		}
		else {
			if ((input[58]) >= (37.5028)) {
				var92 = 0.029387265;
			}
			else {
				var92 = 0.0026626934;
			}
		}
	}
	else {
		if ((input[66]) >= (17.5)) {
			if ((input[64]) >= (38.5)) {
				var92 = -0.0069554457;
			}
			else {
				var92 = -0.06839612;
			}
		}
		else {
			if ((input[70]) >= (1.5)) {
				var92 = 0.016244156;
			}
			else {
				var92 = 0.08118961;
			}
		}
	}
	double var93;
	if ((input[12]) >= (37.61915)) {
		if ((input[22]) >= (9.5175495)) {
			if ((input[55]) >= (114.1543)) {
				var93 = -0.04620532;
			}
			else {
				var93 = -0.016296595;
			}
		}
		else {
			if ((input[41]) >= (3.5)) {
				var93 = -0.0019393134;
			}
			else {
				var93 = 0.03458402;
			}
		}
	}
	else {
		if ((input[64]) >= (43.5)) {
			if ((input[66]) >= (6106.5)) {
				var93 = 0.00076886354;
			}
			else {
				var93 = 0.027333582;
			}
		}
		else {
			if ((input[3]) >= (1.775)) {
				var93 = 0.0015267775;
			}
			else {
				var93 = -0.009277506;
			}
		}
	}
	double var94;
	if ((input[71]) >= (-824.5)) {
		if ((input[64]) >= (46.5)) {
			if ((input[66]) >= (7556.5)) {
				var94 = 0.00541505;
			}
			else {
				var94 = 0.06189242;
			}
		}
		else {
			if ((input[66]) >= (153.5)) {
				var94 = -0.0026730557;
			}
			else {
				var94 = 0.00784517;
			}
		}
	}
	else {
		if ((input[55]) >= (109.40235)) {
			if ((input[46]) >= (6.2846003)) {
				var94 = 0.074860126;
			}
			else {
				var94 = -0.12439511;
			}
		}
		else {
			if ((input[70]) >= (2.5)) {
				var94 = -0.017468512;
			}
			else {
				var94 = -0.1065315;
			}
		}
	}
	double var95;
	if ((input[70]) >= (433.5)) {
		if ((input[22]) >= (100.318344)) {
			var95 = 0.09863646;
		}
		else {
			if ((input[16]) >= (9.45)) {
				var95 = -0.07232084;
			}
			else {
				var95 = -0.029285679;
			}
		}
	}
	else {
		if ((input[9]) >= (0.5)) {
			if ((input[66]) >= (-124.5)) {
				var95 = -0.0019853367;
			}
			else {
				var95 = -0.06542007;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var95 = -0.0051659015;
			}
			else {
				var95 = 0.010720353;
			}
		}
	}
	double var96;
	if ((input[1]) >= (0.041050002)) {
		if ((input[58]) >= (0.50084996)) {
			if ((input[66]) >= (197.5)) {
				var96 = -0.0016655888;
			}
			else {
				var96 = 0.00846942;
			}
		}
		else {
			if ((input[66]) >= (17.5)) {
				var96 = -0.03113589;
			}
			else {
				var96 = 0.029318063;
			}
		}
	}
	else {
		if ((input[14]) >= (1.775)) {
			if ((input[55]) >= (83.16015)) {
				var96 = -0.0040534674;
			}
			else {
				var96 = 0.03240179;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var96 = 0.023562042;
			}
			else {
				var96 = -0.012901259;
			}
		}
	}
	double var97;
	if ((input[46]) >= (5.40475)) {
		if ((input[71]) >= (5237.5)) {
			if ((input[67]) >= (0.5)) {
				var97 = -0.00805479;
			}
			else {
				var97 = 0.008282287;
			}
		}
		else {
			if ((input[58]) >= (703.14404)) {
				var97 = 0.030020813;
			}
			else {
				var97 = 0.0044606808;
			}
		}
	}
	else {
		if ((input[53]) >= (0.5)) {
			if ((input[48]) >= (1.41665)) {
				var97 = -0.0052050706;
			}
			else {
				var97 = -0.034000948;
			}
		}
		else {
			if ((input[71]) >= (24.5)) {
				var97 = -0.0019682192;
			}
			else {
				var97 = 0.021216786;
			}
		}
	}
	double var98;
	if ((input[60]) >= (32.892548)) {
		if ((input[71]) >= (12818.5)) {
			if ((input[4]) >= (8.0)) {
				var98 = -0.009266472;
			}
			else {
				var98 = 0.016268868;
			}
		}
		else {
			if ((input[42]) >= (4.5)) {
				var98 = 0.050534606;
			}
			else {
				var98 = 0.017593766;
			}
		}
	}
	else {
		if ((input[12]) >= (2.85745)) {
			if ((input[60]) >= (7.3769503)) {
				var98 = -0.00011093716;
			}
			else {
				var98 = -0.01327972;
			}
		}
		else {
			if ((input[58]) >= (6.92555)) {
				var98 = 0.008371453;
			}
			else {
				var98 = -0.0024080907;
			}
		}
	}
	double var99;
	if ((input[71]) >= (-196.5)) {
		if ((input[66]) >= (112.5)) {
			if ((input[58]) >= (37.091652)) {
				var99 = 0.0022982785;
			}
			else {
				var99 = -0.010871814;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var99 = 0.051667124;
			}
			else {
				var99 = 0.002151499;
			}
		}
	}
	else {
		if ((input[66]) >= (133.5)) {
			if ((input[1]) >= (3.32225)) {
				var99 = 0.019419333;
			}
			else {
				var99 = -0.032020103;
			}
		}
		else {
			if ((input[56]) >= (873.81946)) {
				var99 = -0.13776249;
			}
			else {
				var99 = -0.044692952;
			}
		}
	}
	double var100;
	var100 = (1.0) / ((1.0) + (exp((0.0) - ((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((var0)+(var1)) + (var2)) + (var3)) + (var4)) + (var5)) + (var6)) + (var7)) + (var8)) + (var9)) + (var10)) + (var11)) + (var12)) + (var13)) + (var14)) + (var15)) + (var16)) + (var17)) + (var18)) + (var19)) + (var20)) + (var21)) + (var22)) + (var23)) + (var24)) + (var25)) + (var26)) + (var27)) + (var28)) + (var29)) + (var30)) + (var31)) + (var32)) + (var33)) + (var34)) + (var35)) + (var36)) + (var37)) + (var38)) + (var39)) + (var40)) + (var41)) + (var42)) + (var43)) + (var44)) + (var45)) + (var46)) + (var47)) + (var48)) + (var49)) + (var50)) + (var51)) + (var52)) + (var53)) + (var54)) + (var55)) + (var56)) + (var57)) + (var58)) + (var59)) + (var60)) + (var61)) + (var62)) + (var63)) + (var64)) + (var65)) + (var66)) + (var67)) + (var68)) + (var69)) + (var70)) + (var71)) + (var72)) + (var73)) + (var74)) + (var75)) + (var76)) + (var77)) + (var78)) + (var79)) + (var80)) + (var81)) + (var82)) + (var83)) + (var84)) + (var85)) + (var86)) + (var87)) + (var88)) + (var89)) + (var90)) + (var91)) + (var92)) + (var93)) + (var94)) + (var95)) + (var96)) + (var97)) + (var98)) + (var99)))));
	*output = var100 < 0.5 ? 0 : 1;
}

void PUclf8(double * input, double * output) {
	double var0;
	if ((input[70]) >= (4.5)) {
		if ((input[70]) >= (9.5)) {
			if ((input[49]) >= (6.58335)) {
				var0 = -0.17194784;
			}
			else {
				var0 = -0.12116623;
			}
		}
		else {
			if ((input[0]) >= (0.5)) {
				var0 = -0.02388605;
			}
			else {
				var0 = -0.12068683;
			}
		}
	}
	else {
		if ((input[70]) >= (1.5)) {
			if ((input[50]) >= (1.5)) {
				var0 = 0.073635094;
			}
			else {
				var0 = 0.0021574346;
			}
		}
		else {
			if ((input[71]) >= (356.5)) {
				var0 = 0.07216859;
			}
			else {
				var0 = 0.17632963;
			}
		}
	}
	double var1;
	if ((input[70]) >= (3.5)) {
		if ((input[70]) >= (8.5)) {
			if ((input[50]) >= (1.5)) {
				var1 = -0.10225373;
			}
			else {
				var1 = -0.1528434;
			}
		}
		else {
			if ((input[0]) >= (0.5)) {
				var1 = -0.007082812;
			}
			else {
				var1 = -0.10403097;
			}
		}
	}
	else {
		if ((input[71]) >= (204.5)) {
			if ((input[0]) >= (0.5)) {
				var1 = 0.057961512;
			}
			else {
				var1 = -0.08277751;
			}
		}
		else {
			if ((input[70]) >= (2.5)) {
				var1 = 0.05313552;
			}
			else {
				var1 = 0.15329899;
			}
		}
	}
	double var2;
	if ((input[70]) >= (4.5)) {
		if ((input[70]) >= (10.5)) {
			if ((input[0]) >= (0.5)) {
				var2 = -0.11545127;
			}
			else {
				var2 = -0.16214955;
			}
		}
		else {
			if ((input[0]) >= (0.5)) {
				var2 = -0.021269472;
			}
			else {
				var2 = -0.10288656;
			}
		}
	}
	else {
		if ((input[71]) >= (109.5)) {
			if ((input[0]) >= (0.5)) {
				var2 = 0.049167346;
			}
			else {
				var2 = -0.07864951;
			}
		}
		else {
			if ((input[70]) >= (2.5)) {
				var2 = 0.036487833;
			}
			else {
				var2 = 0.14522377;
			}
		}
	}
	double var3;
	if ((input[70]) >= (3.5)) {
		if ((input[70]) >= (8.5)) {
			if ((input[26]) >= (7.1)) {
				var3 = -0.13279565;
			}
			else {
				var3 = -0.08621841;
			}
		}
		else {
			if ((input[50]) >= (1.5)) {
				var3 = 0.010095669;
			}
			else {
				var3 = -0.059918713;
			}
		}
	}
	else {
		if ((input[71]) >= (2532.5)) {
			if ((input[64]) >= (43.5)) {
				var3 = 0.03174169;
			}
			else {
				var3 = -0.029605275;
			}
		}
		else {
			if ((input[70]) >= (1.5)) {
				var3 = 0.06167726;
			}
			else {
				var3 = 0.12843218;
			}
		}
	}
	double var4;
	if ((input[70]) >= (5.5)) {
		if ((input[70]) >= (11.5)) {
			if ((input[0]) >= (0.5)) {
				var4 = -0.10106035;
			}
			else {
				var4 = -0.14455198;
			}
		}
		else {
			if ((input[71]) >= (-29.5)) {
				var4 = -0.056739397;
			}
			else {
				var4 = 0.06173403;
			}
		}
	}
	else {
		if ((input[70]) >= (2.5)) {
			if ((input[0]) >= (0.5)) {
				var4 = 0.019612553;
			}
			else {
				var4 = -0.07400791;
			}
		}
		else {
			if ((input[71]) >= (508.5)) {
				var4 = 0.034428604;
			}
			else {
				var4 = 0.117211044;
			}
		}
	}
	double var5;
	if ((input[70]) >= (3.5)) {
		if ((input[70]) >= (10.5)) {
			if ((input[0]) >= (0.5)) {
				var5 = -0.09051793;
			}
			else {
				var5 = -0.13523169;
			}
		}
		else {
			if ((input[0]) >= (0.5)) {
				var5 = -0.0062621557;
			}
			else {
				var5 = -0.08319245;
			}
		}
	}
	else {
		if ((input[71]) >= (71.5)) {
			if ((input[71]) >= (4474.5)) {
				var5 = -0.019311436;
			}
			else {
				var5 = 0.05516538;
			}
		}
		else {
			if ((input[51]) >= (0.5)) {
				var5 = 0.121704474;
			}
			else {
				var5 = 0.02114481;
			}
		}
	}
	double var6;
	if ((input[70]) >= (5.5)) {
		if ((input[70]) >= (13.5)) {
			if ((input[0]) >= (0.5)) {
				var6 = -0.0928432;
			}
			else {
				var6 = -0.13210689;
			}
		}
		else {
			if ((input[0]) >= (0.5)) {
				var6 = -0.02402833;
			}
			else {
				var6 = -0.08880577;
			}
		}
	}
	else {
		if ((input[70]) >= (1.5)) {
			if ((input[50]) >= (1.5)) {
				var6 = 0.039929587;
			}
			else {
				var6 = -0.019160818;
			}
		}
		else {
			if ((input[71]) >= (509.5)) {
				var6 = 0.039074767;
			}
			else {
				var6 = 0.115766004;
			}
		}
	}
	double var7;
	if ((input[70]) >= (3.5)) {
		if ((input[70]) >= (11.5)) {
			if ((input[71]) >= (-81.5)) {
				var7 = -0.098753616;
			}
			else {
				var7 = 0.026776532;
			}
		}
		else {
			if ((input[51]) >= (0.5)) {
				var7 = -0.0005569726;
			}
			else {
				var7 = -0.06417424;
			}
		}
	}
	else {
		if ((input[71]) >= (37.5)) {
			if ((input[0]) >= (0.5)) {
				var7 = 0.043801557;
			}
			else {
				var7 = -0.06659437;
			}
		}
		else {
			if ((input[70]) >= (1.5)) {
				var7 = 0.06781005;
			}
			else {
				var7 = 0.12537448;
			}
		}
	}
	double var8;
	if ((input[70]) >= (5.5)) {
		if ((input[70]) >= (13.5)) {
			if ((input[0]) >= (0.5)) {
				var8 = -0.08015804;
			}
			else {
				var8 = -0.1226972;
			}
		}
		else {
			if ((input[71]) >= (-24.5)) {
				var8 = -0.04566171;
			}
			else {
				var8 = 0.05891291;
			}
		}
	}
	else {
		if ((input[70]) >= (1.5)) {
			if ((input[71]) >= (-0.5)) {
				var8 = 0.0019463539;
			}
			else {
				var8 = 0.08762551;
			}
		}
		else {
			if ((input[71]) >= (1624.5)) {
				var8 = 0.016075375;
			}
			else {
				var8 = 0.09873568;
			}
		}
	}
	double var9;
	if ((input[70]) >= (2.5)) {
		if ((input[70]) >= (7.5)) {
			if ((input[49]) >= (6.70835)) {
				var9 = -0.093405;
			}
			else {
				var9 = -0.041569512;
			}
		}
		else {
			if ((input[50]) >= (1.5)) {
				var9 = 0.019955598;
			}
			else {
				var9 = -0.035836205;
			}
		}
	}
	else {
		if ((input[71]) >= (59.5)) {
			if ((input[64]) >= (38.5)) {
				var9 = 0.04334417;
			}
			else {
				var9 = -0.06405699;
			}
		}
		else {
			if ((input[64]) >= (37.5)) {
				var9 = 0.112555444;
			}
			else {
				var9 = 0.053532686;
			}
		}
	}
	double var10;
	if ((input[70]) >= (2.5)) {
		if ((input[70]) >= (10.5)) {
			if ((input[0]) >= (0.5)) {
				var10 = -0.061772563;
			}
			else {
				var10 = -0.11194382;
			}
		}
		else {
			if ((input[26]) >= (8.9)) {
				var10 = -0.05175608;
			}
			else {
				var10 = 0.007902;
			}
		}
	}
	else {
		if ((input[71]) >= (144.5)) {
			if ((input[64]) >= (38.5)) {
				var10 = 0.032038596;
			}
			else {
				var10 = -0.09290336;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var10 = 0.11391206;
			}
			else {
				var10 = 0.059093397;
			}
		}
	}
	double var11;
	if ((input[70]) >= (6.5)) {
		if ((input[71]) >= (-24.5)) {
			if ((input[49]) >= (6.58335)) {
				var11 = -0.08417117;
			}
			else {
				var11 = -0.037547465;
			}
		}
		else {
			if ((input[66]) >= (-6.5)) {
				var11 = 0.015274078;
			}
			else {
				var11 = 0.09706504;
			}
		}
	}
	else {
		if ((input[71]) >= (2780.5)) {
			if ((input[64]) >= (43.5)) {
				var11 = 0.00510189;
			}
			else {
				var11 = -0.045427956;
			}
		}
		else {
			if ((input[51]) >= (1.5)) {
				var11 = 0.07215229;
			}
			else {
				var11 = 0.002435452;
			}
		}
	}
	double var12;
	if ((input[70]) >= (2.5)) {
		if ((input[70]) >= (12.5)) {
			if ((input[0]) >= (0.5)) {
				var12 = -0.059387367;
			}
			else {
				var12 = -0.108879305;
			}
		}
		else {
			if ((input[0]) >= (0.5)) {
				var12 = 0.0029711842;
			}
			else {
				var12 = -0.06001085;
			}
		}
	}
	else {
		if ((input[71]) >= (27.5)) {
			if ((input[64]) >= (37.5)) {
				var12 = 0.03364353;
			}
			else {
				var12 = -0.105140306;
			}
		}
		else {
			if ((input[70]) >= (1.5)) {
				var12 = 0.069564514;
			}
			else {
				var12 = 0.10898651;
			}
		}
	}
	double var13;
	if ((input[70]) >= (2.5)) {
		if ((input[50]) >= (0.5)) {
			if ((input[71]) >= (-11.5)) {
				var13 = -0.016717164;
			}
			else {
				var13 = 0.0689022;
			}
		}
		else {
			if ((input[66]) >= (17.5)) {
				var13 = -0.07355407;
			}
			else {
				var13 = 0.074569725;
			}
		}
	}
	else {
		if ((input[71]) >= (207.5)) {
			if ((input[71]) >= (7055.5)) {
				var13 = -0.04272632;
			}
			else {
				var13 = 0.028339813;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var13 = 0.102857426;
			}
			else {
				var13 = 0.042721834;
			}
		}
	}
	double var14;
	if ((input[70]) >= (7.5)) {
		if ((input[50]) >= (2.5)) {
			if ((input[71]) >= (144.5)) {
				var14 = -0.02853539;
			}
			else {
				var14 = 0.050496634;
			}
		}
		else {
			if ((input[0]) >= (0.5)) {
				var14 = -0.051306225;
			}
			else {
				var14 = -0.092957534;
			}
		}
	}
	else {
		if ((input[66]) >= (18.5)) {
			if ((input[26]) >= (8.9)) {
				var14 = -0.038591243;
			}
			else {
				var14 = 0.024880227;
			}
		}
		else {
			if ((input[51]) >= (2.5)) {
				var14 = 0.107544675;
			}
			else {
				var14 = 0.07684121;
			}
		}
	}
	double var15;
	if ((input[70]) >= (4.5)) {
		if ((input[70]) >= (14.5)) {
			if ((input[0]) >= (0.5)) {
				var15 = -0.05578051;
			}
			else {
				var15 = -0.10336238;
			}
		}
		else {
			if ((input[50]) >= (2.5)) {
				var15 = 0.017724304;
			}
			else {
				var15 = -0.030171305;
			}
		}
	}
	else {
		if ((input[71]) >= (3477.5)) {
			if ((input[64]) >= (45.5)) {
				var15 = 0.012269373;
			}
			else {
				var15 = -0.043173473;
			}
		}
		else {
			if ((input[26]) >= (6.775)) {
				var15 = 0.0045047062;
			}
			else {
				var15 = 0.0625461;
			}
		}
	}
	double var16;
	if ((input[51]) >= (1.5)) {
		if ((input[71]) >= (234.5)) {
			if ((input[64]) >= (40.5)) {
				var16 = 0.017153142;
			}
			else {
				var16 = -0.03720021;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var16 = 0.090907775;
			}
			else {
				var16 = 0.03769536;
			}
		}
	}
	else {
		if ((input[70]) >= (8.5)) {
			if ((input[0]) >= (0.5)) {
				var16 = -0.04918657;
			}
			else {
				var16 = -0.092782214;
			}
		}
		else {
			if ((input[66]) >= (18.5)) {
				var16 = -0.017568478;
			}
			else {
				var16 = 0.06468724;
			}
		}
	}
	double var17;
	if ((input[70]) >= (1.5)) {
		if ((input[70]) >= (14.5)) {
			if ((input[64]) >= (38.5)) {
				var17 = -0.044333607;
			}
			else {
				var17 = -0.088715844;
			}
		}
		else {
			if ((input[0]) >= (0.5)) {
				var17 = 0.0062335613;
			}
			else {
				var17 = -0.04579733;
			}
		}
	}
	else {
		if ((input[71]) >= (35.5)) {
			if ((input[64]) >= (38.5)) {
				var17 = 0.036366068;
			}
			else {
				var17 = -0.05421984;
			}
		}
		else {
			if ((input[71]) >= (-675.5)) {
				var17 = 0.09780317;
			}
			else {
				var17 = 0.025531167;
			}
		}
	}
	double var18;
	if ((input[50]) >= (2.5)) {
		if ((input[71]) >= (842.5)) {
			if ((input[64]) >= (43.5)) {
				var18 = 0.027232042;
			}
			else {
				var18 = -0.023233375;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var18 = 0.08273547;
			}
			else {
				var18 = 0.030557439;
			}
		}
	}
	else {
		if ((input[70]) >= (8.5)) {
			if ((input[0]) >= (0.5)) {
				var18 = -0.0408295;
			}
			else {
				var18 = -0.08555921;
			}
		}
		else {
			if ((input[66]) >= (18.5)) {
				var18 = -0.010707343;
			}
			else {
				var18 = 0.06358015;
			}
		}
	}
	double var19;
	if ((input[51]) >= (0.5)) {
		if ((input[71]) >= (121.5)) {
			if ((input[64]) >= (38.5)) {
				var19 = 0.0039179246;
			}
			else {
				var19 = -0.071798205;
			}
		}
		else {
			if ((input[51]) >= (2.5)) {
				var19 = 0.08102296;
			}
			else {
				var19 = 0.030996187;
			}
		}
	}
	else {
		if ((input[26]) >= (6.775)) {
			if ((input[66]) >= (18.5)) {
				var19 = -0.06192258;
			}
			else {
				var19 = 0.047135707;
			}
		}
		else {
			if ((input[71]) >= (-20.5)) {
				var19 = -0.016469136;
			}
			else {
				var19 = 0.0566999;
			}
		}
	}
	double var20;
	if ((input[70]) >= (1.5)) {
		if ((input[70]) >= (15.5)) {
			if ((input[0]) >= (0.5)) {
				var20 = -0.044300925;
			}
			else {
				var20 = -0.09421349;
			}
		}
		else {
			if ((input[0]) >= (0.5)) {
				var20 = 0.0048556193;
			}
			else {
				var20 = -0.03968349;
			}
		}
	}
	else {
		if ((input[71]) >= (28.5)) {
			if ((input[71]) >= (5413.5)) {
				var20 = -0.028178591;
			}
			else {
				var20 = 0.03587414;
			}
		}
		else {
			if ((input[66]) >= (-589.5)) {
				var20 = 0.09350666;
			}
			else {
				var20 = 0.0082612075;
			}
		}
	}
	double var21;
	if ((input[70]) >= (3.5)) {
		if ((input[71]) >= (-13.5)) {
			if ((input[26]) >= (8.325)) {
				var21 = -0.05074602;
			}
			else {
				var21 = -0.011870801;
			}
		}
		else {
			if ((input[66]) >= (29.5)) {
				var21 = 0.019099725;
			}
			else {
				var21 = 0.06590576;
			}
		}
	}
	else {
		if ((input[71]) >= (1283.5)) {
			if ((input[64]) >= (43.5)) {
				var21 = 0.016384987;
			}
			else {
				var21 = -0.03121475;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var21 = 0.06454378;
			}
			else {
				var21 = 0.002008236;
			}
		}
	}
	double var22;
	if ((input[50]) >= (2.5)) {
		if ((input[71]) >= (355.5)) {
			if ((input[71]) >= (7489.0)) {
				var22 = -0.035794154;
			}
			else {
				var22 = 0.013913413;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var22 = 0.07904535;
			}
			else {
				var22 = 0.032954965;
			}
		}
	}
	else {
		if ((input[26]) >= (7.1)) {
			if ((input[66]) >= (18.5)) {
				var22 = -0.04184636;
			}
			else {
				var22 = 0.049340572;
			}
		}
		else {
			if ((input[71]) >= (5402.5)) {
				var22 = -0.03442084;
			}
			else {
				var22 = 0.01648399;
			}
		}
	}
	double var23;
	if ((input[66]) >= (23.5)) {
		if ((input[64]) >= (38.5)) {
			if ((input[71]) >= (4224.5)) {
				var23 = -0.03019995;
			}
			else {
				var23 = 0.024300985;
			}
		}
		else {
			if ((input[71]) >= (242.5)) {
				var23 = -0.08568688;
			}
			else {
				var23 = -0.020719815;
			}
		}
	}
	else {
		if ((input[70]) >= (1.5)) {
			if ((input[53]) >= (0.5)) {
				var23 = 0.029945318;
			}
			else {
				var23 = 0.065160744;
			}
		}
		else {
			if ((input[71]) >= (-335.5)) {
				var23 = 0.0899089;
			}
			else {
				var23 = 0.012688398;
			}
		}
	}
	double var24;
	if ((input[51]) >= (0.5)) {
		if ((input[71]) >= (142.5)) {
			if ((input[64]) >= (39.5)) {
				var24 = 0.0059755347;
			}
			else {
				var24 = -0.04605591;
			}
		}
		else {
			if ((input[53]) >= (0.5)) {
				var24 = 0.01603095;
			}
			else {
				var24 = 0.060933102;
			}
		}
	}
	else {
		if ((input[70]) >= (11.5)) {
			if ((input[0]) >= (0.5)) {
				var24 = -0.04310752;
			}
			else {
				var24 = -0.087691784;
			}
		}
		else {
			if ((input[58]) >= (0.0293)) {
				var24 = -0.014024401;
			}
			else {
				var24 = -0.09727788;
			}
		}
	}
	double var25;
	if ((input[49]) >= (2.125)) {
		if ((input[70]) >= (14.5)) {
			if ((input[0]) >= (0.5)) {
				var25 = -0.03873388;
			}
			else {
				var25 = -0.08678227;
			}
		}
		else {
			if ((input[66]) >= (18.5)) {
				var25 = -0.006741715;
			}
			else {
				var25 = 0.052177776;
			}
		}
	}
	else {
		if ((input[71]) >= (578.5)) {
			if ((input[58]) >= (815.584)) {
				var25 = 0.026227636;
			}
			else {
				var25 = -0.021481648;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var25 = 0.07722066;
			}
			else {
				var25 = 0.038790476;
			}
		}
	}
	double var26;
	if ((input[70]) >= (1.5)) {
		if ((input[6]) >= (0.5)) {
			if ((input[71]) >= (5428.5)) {
				var26 = -0.025831206;
			}
			else {
				var26 = 0.015029924;
			}
		}
		else {
			if ((input[50]) >= (0.5)) {
				var26 = -0.013044252;
			}
			else {
				var26 = -0.046989426;
			}
		}
	}
	else {
		if ((input[71]) >= (26.5)) {
			if ((input[64]) >= (37.5)) {
				var26 = 0.021964474;
			}
			else {
				var26 = -0.101875715;
			}
		}
		else {
			if ((input[71]) >= (-335.5)) {
				var26 = 0.08549533;
			}
			else {
				var26 = 0.004827008;
			}
		}
	}
	double var27;
	if ((input[26]) >= (5.775)) {
		if ((input[66]) >= (18.5)) {
			if ((input[64]) >= (38.5)) {
				var27 = -0.008838571;
			}
			else {
				var27 = -0.042672914;
			}
		}
		else {
			if ((input[54]) >= (2.5)) {
				var27 = 0.071949296;
			}
			else {
				var27 = 0.03690913;
			}
		}
	}
	else {
		if ((input[71]) >= (1753.5)) {
			if ((input[58]) >= (1058.1094)) {
				var27 = 0.0034852277;
			}
			else {
				var27 = -0.039802384;
			}
		}
		else {
			if ((input[70]) >= (3.5)) {
				var27 = 0.010632667;
			}
			else {
				var27 = 0.05113727;
			}
		}
	}
	double var28;
	if ((input[49]) >= (2.125)) {
		if ((input[70]) >= (15.5)) {
			if ((input[0]) >= (0.5)) {
				var28 = -0.035570197;
			}
			else {
				var28 = -0.08364473;
			}
		}
		else {
			if ((input[17]) >= (0.5)) {
				var28 = 0.010757427;
			}
			else {
				var28 = -0.015855955;
			}
		}
	}
	else {
		if ((input[71]) >= (490.5)) {
			if ((input[64]) >= (43.5)) {
				var28 = 0.037184432;
			}
			else {
				var28 = -0.010256427;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var28 = 0.07141356;
			}
			else {
				var28 = 0.034442212;
			}
		}
	}
	double var29;
	if ((input[71]) >= (9038.5)) {
		if ((input[64]) >= (43.5)) {
			if ((input[71]) >= (33224.5)) {
				var29 = -0.05936817;
			}
			else {
				var29 = 0.00027946554;
			}
		}
		else {
			if ((input[57]) >= (2.23715)) {
				var29 = -0.04824549;
			}
			else {
				var29 = -0.080880746;
			}
		}
	}
	else {
		if ((input[42]) >= (3.5)) {
			if ((input[1]) >= (8.77345)) {
				var29 = 0.011602842;
			}
			else {
				var29 = 0.04117009;
			}
		}
		else {
			if ((input[66]) >= (31.5)) {
				var29 = -0.01202347;
			}
			else {
				var29 = 0.04000632;
			}
		}
	}
	double var30;
	if ((input[51]) >= (2.5)) {
		if ((input[71]) >= (419.5)) {
			if ((input[64]) >= (40.5)) {
				var30 = 0.015325365;
			}
			else {
				var30 = -0.038290467;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var30 = 0.066629894;
			}
			else {
				var30 = 0.024468297;
			}
		}
	}
	else {
		if ((input[17]) >= (0.5)) {
			if ((input[71]) >= (5426.5)) {
				var30 = -0.02171348;
			}
			else {
				var30 = 0.013589854;
			}
		}
		else {
			if ((input[66]) >= (13.5)) {
				var30 = -0.025404332;
			}
			else {
				var30 = 0.04092929;
			}
		}
	}
	double var31;
	if ((input[1]) >= (2.7266002)) {
		if ((input[64]) >= (38.5)) {
			if ((input[71]) >= (3958.5)) {
				var31 = -0.022761025;
			}
			else {
				var31 = 0.012300181;
			}
		}
		else {
			if ((input[71]) >= (236.5)) {
				var31 = -0.08200933;
			}
			else {
				var31 = -0.02087157;
			}
		}
	}
	else {
		if ((input[64]) >= (37.5)) {
			if ((input[66]) >= (198.5)) {
				var31 = 0.012571241;
			}
			else {
				var31 = 0.04794562;
			}
		}
		else {
			if ((input[66]) >= (196.5)) {
				var31 = -0.085481204;
			}
			else {
				var31 = 0.001979051;
			}
		}
	}
	double var32;
	if ((input[70]) >= (1.5)) {
		if ((input[57]) >= (1.00085)) {
			if ((input[71]) >= (172.5)) {
				var32 = -0.009190919;
			}
			else {
				var32 = 0.028069496;
			}
		}
		else {
			if ((input[71]) >= (-2.5)) {
				var32 = -0.04658919;
			}
			else {
				var32 = 0.011300139;
			}
		}
	}
	else {
		if ((input[71]) >= (26.5)) {
			if ((input[64]) >= (43.5)) {
				var32 = 0.042880405;
			}
			else {
				var32 = -0.008044107;
			}
		}
		else {
			if ((input[71]) >= (-335.5)) {
				var32 = 0.07787581;
			}
			else {
				var32 = -0.013590276;
			}
		}
	}
	double var33;
	if ((input[71]) >= (9721.0)) {
		if ((input[64]) >= (43.5)) {
			if ((input[71]) >= (44806.5)) {
				var33 = -0.06681674;
			}
			else {
				var33 = -0.0029471314;
			}
		}
		else {
			if ((input[57]) >= (2.46155)) {
				var33 = -0.039000846;
			}
			else {
				var33 = -0.06817655;
			}
		}
	}
	else {
		if ((input[64]) >= (39.5)) {
			if ((input[66]) >= (203.5)) {
				var33 = 0.011396718;
			}
			else {
				var33 = 0.057059545;
			}
		}
		else {
			if ((input[66]) >= (129.5)) {
				var33 = -0.035304364;
			}
			else {
				var33 = 0.010081808;
			}
		}
	}
	double var34;
	if ((input[0]) >= (0.5)) {
		if ((input[71]) >= (2471.5)) {
			if ((input[64]) >= (43.5)) {
				var34 = 0.011855675;
			}
			else {
				var34 = -0.02585989;
			}
		}
		else {
			if ((input[70]) >= (3.5)) {
				var34 = 0.0012558138;
			}
			else {
				var34 = 0.03217232;
			}
		}
	}
	else {
		if ((input[66]) >= (91.5)) {
			if ((input[64]) >= (43.5)) {
				var34 = -0.009982535;
			}
			else {
				var34 = -0.059131797;
			}
		}
		else {
			if ((input[58]) >= (0.66795003)) {
				var34 = 0.026184246;
			}
			else {
				var34 = -0.030834807;
			}
		}
	}
	double var35;
	if ((input[49]) >= (0.41665)) {
		if ((input[1]) >= (9.9922)) {
			if ((input[57]) >= (1.95865)) {
				var35 = -0.0053824433;
			}
			else {
				var35 = -0.039476153;
			}
		}
		else {
			if ((input[0]) >= (0.5)) {
				var35 = 0.011883978;
			}
			else {
				var35 = -0.020462412;
			}
		}
	}
	else {
		if ((input[71]) >= (1380.5)) {
			if ((input[58]) >= (842.96094)) {
				var35 = 0.016694007;
			}
			else {
				var35 = -0.027127136;
			}
		}
		else {
			if ((input[64]) >= (40.5)) {
				var35 = 0.062002297;
			}
			else {
				var35 = 0.03125216;
			}
		}
	}
	double var36;
	if ((input[1]) >= (0.6172)) {
		if ((input[64]) >= (38.5)) {
			if ((input[71]) >= (6040.5)) {
				var36 = -0.022162184;
			}
			else {
				var36 = 0.011920459;
			}
		}
		else {
			if ((input[71]) >= (172.5)) {
				var36 = -0.06766465;
			}
			else {
				var36 = -0.010225509;
			}
		}
	}
	else {
		if ((input[53]) >= (0.5)) {
			if ((input[62]) >= (0.110750005)) {
				var36 = 0.01637124;
			}
			else {
				var36 = -0.034685574;
			}
		}
		else {
			if ((input[71]) >= (75.5)) {
				var36 = 0.020318238;
			}
			else {
				var36 = 0.062324274;
			}
		}
	}
	double var37;
	if ((input[50]) >= (0.5)) {
		if ((input[71]) >= (7344.5)) {
			if ((input[57]) >= (2.66615)) {
				var37 = -0.005421504;
			}
			else {
				var37 = -0.036419656;
			}
		}
		else {
			if ((input[64]) >= (43.5)) {
				var37 = 0.041785948;
			}
			else {
				var37 = 0.0048971605;
			}
		}
	}
	else {
		if ((input[57]) >= (0.81855)) {
			if ((input[70]) >= (16.5)) {
				var37 = -0.04753214;
			}
			else {
				var37 = -0.0044586356;
			}
		}
		else {
			if ((input[66]) >= (8.5)) {
				var37 = -0.06717759;
			}
			else {
				var37 = 0.060341697;
			}
		}
	}
	double var38;
	if ((input[1]) >= (4.9922)) {
		if ((input[58]) >= (709.084)) {
			if ((input[71]) >= (3958.5)) {
				var38 = -0.013521068;
			}
			else {
				var38 = 0.024439475;
			}
		}
		else {
			if ((input[71]) >= (460.5)) {
				var38 = -0.04843734;
			}
			else {
				var38 = -0.008719909;
			}
		}
	}
	else {
		if ((input[66]) >= (12.5)) {
			if ((input[58]) >= (6.8730497)) {
				var38 = 0.02112163;
			}
			else {
				var38 = -0.009869554;
			}
		}
		else {
			if ((input[70]) >= (0.5)) {
				var38 = 0.039417353;
			}
			else {
				var38 = 0.09355421;
			}
		}
	}
	double var39;
	if ((input[1]) >= (0.22659999)) {
		if ((input[57]) >= (1.07445)) {
			if ((input[71]) >= (14269.5)) {
				var39 = -0.03118429;
			}
			else {
				var39 = 0.006811674;
			}
		}
		else {
			if ((input[66]) >= (23.5)) {
				var39 = -0.036477435;
			}
			else {
				var39 = 0.03330842;
			}
		}
	}
	else {
		if ((input[53]) >= (1.5)) {
			if ((input[57]) >= (0.8385)) {
				var39 = 0.017405426;
			}
			else {
				var39 = -0.04153362;
			}
		}
		else {
			if ((input[71]) >= (355.5)) {
				var39 = 0.007032481;
			}
			else {
				var39 = 0.050311882;
			}
		}
	}
	double var40;
	if ((input[49]) >= (0.41665)) {
		if ((input[58]) >= (0.0293)) {
			if ((input[71]) >= (79.5)) {
				var40 = -0.009380066;
			}
			else {
				var40 = 0.017131835;
			}
		}
		else {
			if ((input[66]) >= (16.5)) {
				var40 = -0.07310644;
			}
			else {
				var40 = 0.04819205;
			}
		}
	}
	else {
		if ((input[71]) >= (1380.5)) {
			if ((input[64]) >= (43.5)) {
				var40 = 0.026098473;
			}
			else {
				var40 = -0.017063366;
			}
		}
		else {
			if ((input[70]) >= (3.5)) {
				var40 = 0.019562013;
			}
			else {
				var40 = 0.049523897;
			}
		}
	}
	double var41;
	if ((input[42]) >= (3.5)) {
		if ((input[71]) >= (4869.5)) {
			if ((input[71]) >= (33351.5)) {
				var41 = -0.04792794;
			}
			else {
				var41 = -0.0023640022;
			}
		}
		else {
			if ((input[56]) >= (1412.7986)) {
				var41 = 0.058105417;
			}
			else {
				var41 = 0.013047273;
			}
		}
	}
	else {
		if ((input[71]) >= (185.5)) {
			if ((input[64]) >= (43.5)) {
				var41 = 0.016922051;
			}
			else {
				var41 = -0.028077373;
			}
		}
		else {
			if ((input[58]) >= (9.24025)) {
				var41 = 0.03467574;
			}
			else {
				var41 = -0.0050335187;
			}
		}
	}
	double var42;
	if ((input[0]) >= (0.5)) {
		if ((input[12]) >= (6.8515997)) {
			if ((input[63]) >= (0.99455)) {
				var42 = -0.017682133;
			}
			else {
				var42 = 0.010617824;
			}
		}
		else {
			if ((input[71]) >= (1283.5)) {
				var42 = 0.0007480119;
			}
			else {
				var42 = 0.02360036;
			}
		}
	}
	else {
		if ((input[66]) >= (92.5)) {
			if ((input[1]) >= (0.0078)) {
				var42 = -0.046877112;
			}
			else {
				var42 = 0.034737762;
			}
		}
		else {
			if ((input[56]) >= (1.8094499)) {
				var42 = 0.03867547;
			}
			else {
				var42 = -0.009593933;
			}
		}
	}
	double var43;
	if ((input[64]) >= (40.5)) {
		if ((input[71]) >= (19882.5)) {
			if ((input[57]) >= (2.68345)) {
				var43 = -0.011817434;
			}
			else {
				var43 = -0.046908673;
			}
		}
		else {
			if ((input[70]) >= (1.5)) {
				var43 = 0.0068284543;
			}
			else {
				var43 = 0.035276916;
			}
		}
	}
	else {
		if ((input[71]) >= (272.5)) {
			if ((input[58]) >= (11.373051)) {
				var43 = -0.02101657;
			}
			else {
				var43 = -0.09956737;
			}
		}
		else {
			if ((input[61]) >= (0.05025)) {
				var43 = 0.030731792;
			}
			else {
				var43 = -0.00836674;
			}
		}
	}
	double var44;
	if ((input[34]) >= (8.0)) {
		if ((input[66]) >= (47.5)) {
			if ((input[64]) >= (45.5)) {
				var44 = 0.009332726;
			}
			else {
				var44 = -0.046044618;
			}
		}
		else {
			if ((input[46]) >= (3.8242)) {
				var44 = 0.035047602;
			}
			else {
				var44 = -0.02363352;
			}
		}
	}
	else {
		if ((input[63]) >= (0.98445)) {
			if ((input[71]) >= (1178.5)) {
				var44 = -0.021098798;
			}
			else {
				var44 = 0.0061278413;
			}
		}
		else {
			if ((input[71]) >= (14268.0)) {
				var44 = -0.012409324;
			}
			else {
				var44 = 0.020338612;
			}
		}
	}
	double var45;
	if ((input[64]) >= (38.5)) {
		if ((input[66]) >= (147.5)) {
			if ((input[58]) >= (12.0996)) {
				var45 = 0.0067878775;
			}
			else {
				var45 = -0.03931198;
			}
		}
		else {
			if ((input[71]) >= (-214.5)) {
				var45 = 0.06625944;
			}
			else {
				var45 = 0.00039955508;
			}
		}
	}
	else {
		if ((input[66]) >= (36.5)) {
			if ((input[57]) >= (0.63685)) {
				var45 = -0.019592946;
			}
			else {
				var45 = -0.08131274;
			}
		}
		else {
			if ((input[53]) >= (1.5)) {
				var45 = -0.008768458;
			}
			else {
				var45 = 0.033834573;
			}
		}
	}
	double var46;
	if ((input[1]) >= (21.992199)) {
		if ((input[64]) >= (43.5)) {
			if ((input[71]) >= (44778.0)) {
				var46 = -0.051473495;
			}
			else {
				var46 = 0.007998873;
			}
		}
		else {
			if ((input[70]) >= (16.5)) {
				var46 = -0.05201265;
			}
			else {
				var46 = -0.017973144;
			}
		}
	}
	else {
		if ((input[58]) >= (812.65625)) {
			if ((input[71]) >= (2038.5)) {
				var46 = 0.005392961;
			}
			else {
				var46 = 0.048496883;
			}
		}
		else {
			if ((input[71]) >= (442.5)) {
				var46 = -0.026837567;
			}
			else {
				var46 = 0.0095107285;
			}
		}
	}
	double var47;
	if ((input[64]) >= (43.5)) {
		if ((input[71]) >= (21620.5)) {
			if ((input[71]) >= (64312.5)) {
				var47 = -0.06727675;
			}
			else {
				var47 = -0.013770294;
			}
		}
		else {
			if ((input[70]) >= (1.5)) {
				var47 = 0.013855529;
			}
			else {
				var47 = 0.046141576;
			}
		}
	}
	else {
		if ((input[71]) >= (972.5)) {
			if ((input[58]) >= (1178.1309)) {
				var47 = 0.00007512445;
			}
			else {
				var47 = -0.04375411;
			}
		}
		else {
			if ((input[60]) >= (5.5078)) {
				var47 = 0.029236108;
			}
			else {
				var47 = -0.0063438574;
			}
		}
	}
	double var48;
	if ((input[57]) >= (1.8303499)) {
		if ((input[71]) >= (2742.5)) {
			if ((input[64]) >= (45.5)) {
				var48 = 0.020826174;
			}
			else {
				var48 = -0.011824078;
			}
		}
		else {
			if ((input[56]) >= (1373.573)) {
				var48 = 0.05015058;
			}
			else {
				var48 = 0.01579666;
			}
		}
	}
	else {
		if ((input[66]) >= (32.5)) {
			if ((input[64]) >= (38.5)) {
				var48 = -0.0057134144;
			}
			else {
				var48 = -0.035881847;
			}
		}
		else {
			if ((input[70]) >= (0.5)) {
				var48 = 0.018308591;
			}
			else {
				var48 = 0.08961229;
			}
		}
	}
	double var49;
	if ((input[1]) >= (16.523449)) {
		if ((input[63]) >= (0.93415)) {
			if ((input[64]) >= (43.5)) {
				var49 = -0.0014435215;
			}
			else {
				var49 = -0.030518813;
			}
		}
		else {
			if ((input[70]) >= (18.5)) {
				var49 = -0.028248036;
			}
			else {
				var49 = 0.014719473;
			}
		}
	}
	else {
		if ((input[64]) >= (38.5)) {
			if ((input[71]) >= (1754.5)) {
				var49 = -0.0017930347;
			}
			else {
				var49 = 0.02421952;
			}
		}
		else {
			if ((input[71]) >= (207.5)) {
				var49 = -0.044342976;
			}
			else {
				var49 = 0.0009343928;
			}
		}
	}
	double var50;
	if ((input[12]) >= (2.7266002)) {
		if ((input[58]) >= (60.26365)) {
			if ((input[71]) >= (1182.5)) {
				var50 = -0.005872641;
			}
			else {
				var50 = 0.01917933;
			}
		}
		else {
			if ((input[66]) >= (135.5)) {
				var50 = -0.047813732;
			}
			else {
				var50 = -0.00094434246;
			}
		}
	}
	else {
		if ((input[53]) >= (0.5)) {
			if ((input[56]) >= (1.8094499)) {
				var50 = 0.01227112;
			}
			else {
				var50 = -0.018792516;
			}
		}
		else {
			if ((input[71]) >= (75.5)) {
				var50 = 0.0110272765;
			}
			else {
				var50 = 0.040263847;
			}
		}
	}
	double var51;
	if ((input[57]) >= (2.2455502)) {
		if ((input[70]) >= (19.5)) {
			if ((input[64]) >= (40.5)) {
				var51 = -0.0052090525;
			}
			else {
				var51 = -0.036671117;
			}
		}
		else {
			if ((input[71]) >= (19889.5)) {
				var51 = -0.015788797;
			}
			else {
				var51 = 0.020352332;
			}
		}
	}
	else {
		if ((input[71]) >= (108.5)) {
			if ((input[58]) >= (29.87305)) {
				var51 = -0.0068868445;
			}
			else {
				var51 = -0.04936246;
			}
		}
		else {
			if ((input[53]) >= (0.5)) {
				var51 = -0.008324652;
			}
			else {
				var51 = 0.029909758;
			}
		}
	}
	double var52;
	if ((input[6]) >= (0.5)) {
		if ((input[66]) >= (338.5)) {
			if ((input[58]) >= (319.6172)) {
				var52 = 0.01130891;
			}
			else {
				var52 = -0.02244229;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var52 = 0.006141982;
			}
			else {
				var52 = 0.04127797;
			}
		}
	}
	else {
		if ((input[46]) >= (4.3821)) {
			if ((input[70]) >= (16.5)) {
				var52 = -0.030636316;
			}
			else {
				var52 = 0.0014820491;
			}
		}
		else {
			if ((input[53]) >= (0.5)) {
				var52 = -0.0384694;
			}
			else {
				var52 = -0.0024808075;
			}
		}
	}
	double var53;
	if ((input[63]) >= (0.99095)) {
		if ((input[71]) >= (1272.5)) {
			if ((input[64]) >= (43.5)) {
				var53 = 0.006402401;
			}
			else {
				var53 = -0.033922665;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var53 = 0.016269883;
			}
			else {
				var53 = -0.014511593;
			}
		}
	}
	else {
		if ((input[71]) >= (14268.0)) {
			if ((input[70]) >= (48.5)) {
				var53 = -0.0597818;
			}
			else {
				var53 = -0.009997906;
			}
		}
		else {
			if ((input[58]) >= (2220.496)) {
				var53 = 0.03628566;
			}
			else {
				var53 = 0.008194602;
			}
		}
	}
	double var54;
	if ((input[12]) >= (25.132801)) {
		if ((input[70]) >= (19.5)) {
			if ((input[58]) >= (1221.6309)) {
				var54 = -0.02975295;
			}
			else {
				var54 = -0.067604735;
			}
		}
		else {
			if ((input[55]) >= (126.6797)) {
				var54 = -0.036987003;
			}
			else {
				var54 = -0.0041500097;
			}
		}
	}
	else {
		if ((input[58]) >= (2382.1309)) {
			if ((input[66]) >= (4785.5)) {
				var54 = 0.006412831;
			}
			else {
				var54 = 0.049185954;
			}
		}
		else {
			if ((input[1]) >= (0.22659999)) {
				var54 = -0.0034373906;
			}
			else {
				var54 = 0.025958532;
			}
		}
	}
	double var55;
	if ((input[57]) >= (1.55895)) {
		if ((input[71]) >= (2478.5)) {
			if ((input[0]) >= (0.5)) {
				var55 = -0.0022725624;
			}
			else {
				var55 = -0.060232993;
			}
		}
		else {
			if ((input[56]) >= (1373.573)) {
				var55 = 0.04687841;
			}
			else {
				var55 = 0.011468693;
			}
		}
	}
	else {
		if ((input[66]) >= (23.5)) {
			if ((input[62]) >= (0.0807)) {
				var55 = -0.011654008;
			}
			else {
				var55 = -0.058720093;
			}
		}
		else {
			if ((input[70]) >= (1.5)) {
				var55 = 0.013730779;
			}
			else {
				var55 = 0.060013205;
			}
		}
	}
	double var56;
	if ((input[12]) >= (6.8515997)) {
		if ((input[58]) >= (596.2637)) {
			if ((input[71]) >= (2865.5)) {
				var56 = -0.0075956993;
			}
			else {
				var56 = 0.01820059;
			}
		}
		else {
			if ((input[71]) >= (563.5)) {
				var56 = -0.043050934;
			}
			else {
				var56 = -0.008468102;
			}
		}
	}
	else {
		if ((input[58]) >= (9.0996)) {
			if ((input[71]) >= (617.5)) {
				var56 = 0.005427318;
			}
			else {
				var56 = 0.03342771;
			}
		}
		else {
			if ((input[71]) >= (297.5)) {
				var56 = -0.066192314;
			}
			else {
				var56 = 0.0027477879;
			}
		}
	}
	double var57;
	if ((input[64]) >= (43.5)) {
		if ((input[71]) >= (33224.5)) {
			if ((input[57]) >= (2.68365)) {
				var57 = -0.01207613;
			}
			else {
				var57 = -0.050507583;
			}
		}
		else {
			if ((input[70]) >= (0.5)) {
				var57 = 0.015723286;
			}
			else {
				var57 = 0.059123557;
			}
		}
	}
	else {
		if ((input[66]) >= (180.5)) {
			if ((input[58]) >= (7.5996)) {
				var57 = -0.0044422755;
			}
			else {
				var57 = -0.052847087;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var57 = 0.03950269;
			}
			else {
				var57 = 0.0006767683;
			}
		}
	}
	double var58;
	if ((input[51]) >= (3.5)) {
		if ((input[71]) >= (580.5)) {
			if ((input[64]) >= (40.5)) {
				var58 = 0.017414792;
			}
			else {
				var58 = -0.030848125;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var58 = 0.053094592;
			}
			else {
				var58 = 0.016271636;
			}
		}
	}
	else {
		if ((input[57]) >= (1.00705)) {
			if ((input[71]) >= (323.5)) {
				var58 = -0.003860949;
			}
			else {
				var58 = 0.014145209;
			}
		}
		else {
			if ((input[71]) >= (8.5)) {
				var58 = -0.034631133;
			}
			else {
				var58 = 0.0040806346;
			}
		}
	}
	double var59;
	if ((input[64]) >= (40.5)) {
		if ((input[71]) >= (17395.5)) {
			if ((input[57]) >= (2.68345)) {
				var59 = -0.0028977825;
			}
			else {
				var59 = -0.032742523;
			}
		}
		else {
			if ((input[70]) >= (0.5)) {
				var59 = 0.00863951;
			}
			else {
				var59 = 0.049928565;
			}
		}
	}
	else {
		if ((input[66]) >= (129.5)) {
			if ((input[58]) >= (7.8730497)) {
				var59 = -0.009250942;
			}
			else {
				var59 = -0.04851931;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var59 = 0.047785755;
			}
			else {
				var59 = 0.001874341;
			}
		}
	}
	double var60;
	if ((input[46]) >= (4.6555)) {
		if ((input[71]) >= (4994.5)) {
			if ((input[64]) >= (46.5)) {
				var60 = 0.03703445;
			}
			else {
				var60 = -0.012236692;
			}
		}
		else {
			if ((input[56]) >= (1915.7684)) {
				var60 = 0.046474528;
			}
			else {
				var60 = 0.006064521;
			}
		}
	}
	else {
		if ((input[53]) >= (1.5)) {
			if ((input[62]) >= (0.1288)) {
				var60 = -0.024220781;
			}
			else {
				var60 = -0.08097142;
			}
		}
		else {
			if ((input[71]) >= (16.5)) {
				var60 = -0.008177297;
			}
			else {
				var60 = 0.020774899;
			}
		}
	}
	double var61;
	if ((input[57]) >= (2.3431501)) {
		if ((input[70]) >= (20.5)) {
			if ((input[59]) >= (1.3413501)) {
				var61 = -0.0027497113;
			}
			else {
				var61 = -0.033066507;
			}
		}
		else {
			if ((input[71]) >= (2267.5)) {
				var61 = 0.0062998096;
			}
			else {
				var61 = 0.0257242;
			}
		}
	}
	else {
		if ((input[12]) >= (2.7266002)) {
			if ((input[58]) >= (2271.4688)) {
				var61 = 0.0087852655;
			}
			else {
				var61 = -0.018088471;
			}
		}
		else {
			if ((input[42]) >= (4.5)) {
				var61 = 0.027213607;
			}
			else {
				var61 = 0.002004532;
			}
		}
	}
	double var62;
	if ((input[63]) >= (0.72445)) {
		if ((input[71]) >= (766.5)) {
			if ((input[64]) >= (43.5)) {
				var62 = 0.009072004;
			}
			else {
				var62 = -0.02182374;
			}
		}
		else {
			if ((input[62]) >= (29.1179)) {
				var62 = 0.021835001;
			}
			else {
				var62 = -0.0041183564;
			}
		}
	}
	else {
		if ((input[71]) >= (9945.5)) {
			if ((input[71]) >= (44672.0)) {
				var62 = -0.033779208;
			}
			else {
				var62 = 0.00090560265;
			}
		}
		else {
			if ((input[56]) >= (2698.3271)) {
				var62 = 0.05029757;
			}
			else {
				var62 = 0.016386595;
			}
		}
	}
	double var63;
	if ((input[1]) >= (14.0391)) {
		if ((input[57]) >= (2.53295)) {
			if ((input[70]) >= (19.5)) {
				var63 = -0.021975806;
			}
			else {
				var63 = 0.009333305;
			}
		}
		else {
			if ((input[66]) >= (97.5)) {
				var63 = -0.021797018;
			}
			else {
				var63 = 0.013525695;
			}
		}
	}
	else {
		if ((input[64]) >= (38.5)) {
			if ((input[71]) >= (1757.5)) {
				var63 = 0.000106027444;
			}
			else {
				var63 = 0.01945116;
			}
		}
		else {
			if ((input[71]) >= (330.5)) {
				var63 = -0.043096658;
			}
			else {
				var63 = -0.00068553403;
			}
		}
	}
	double var64;
	if ((input[34]) >= (8.0)) {
		if ((input[66]) >= (333.5)) {
			if ((input[64]) >= (42.5)) {
				var64 = -0.0163096;
			}
			else {
				var64 = -0.051387932;
			}
		}
		else {
			if ((input[58]) >= (3.04295)) {
				var64 = 0.0130977845;
			}
			else {
				var64 = -0.028350268;
			}
		}
	}
	else {
		if ((input[58]) >= (0.0293)) {
			if ((input[1]) >= (0.2422)) {
				var64 = 0.0013126847;
			}
			else {
				var64 = 0.025163425;
			}
		}
		else {
			if ((input[71]) >= (8.5)) {
				var64 = -0.0864142;
			}
			else {
				var64 = -0.003210755;
			}
		}
	}
	double var65;
	if ((input[64]) >= (46.5)) {
		if ((input[71]) >= (38495.5)) {
			if ((input[70]) >= (5.5)) {
				var65 = 0.037733223;
			}
			else {
				var65 = -0.029193258;
			}
		}
		else {
			if ((input[71]) >= (-687.5)) {
				var65 = 0.050236475;
			}
			else {
				var65 = -0.052000888;
			}
		}
	}
	else {
		if ((input[12]) >= (30.8516)) {
			if ((input[55]) >= (130.0547)) {
				var65 = -0.043893483;
			}
			else {
				var65 = -0.0137619;
			}
		}
		else {
			if ((input[58]) >= (1831.4336)) {
				var65 = 0.016206788;
			}
			else {
				var65 = -0.0023019984;
			}
		}
	}
	double var66;
	if ((input[1]) >= (0.6172)) {
		if ((input[58]) >= (90.82615)) {
			if ((input[71]) >= (1757.5)) {
				var66 = -0.0042102095;
			}
			else {
				var66 = 0.016508652;
			}
		}
		else {
			if ((input[71]) >= (299.5)) {
				var66 = -0.044956982;
			}
			else {
				var66 = -0.005097449;
			}
		}
	}
	else {
		if ((input[70]) >= (3.5)) {
			if ((input[57]) >= (1.4866)) {
				var66 = 0.023618434;
			}
			else {
				var66 = -0.014514351;
			}
		}
		else {
			if ((input[71]) >= (53.5)) {
				var66 = 0.010530574;
			}
			else {
				var66 = 0.040718477;
			}
		}
	}
	double var67;
	if ((input[46]) >= (4.6752996)) {
		if ((input[71]) >= (8996.5)) {
			if ((input[0]) >= (0.5)) {
				var67 = -0.008430508;
			}
			else {
				var67 = -0.06895821;
			}
		}
		else {
			if ((input[58]) >= (2220.4297)) {
				var67 = 0.027842093;
			}
			else {
				var67 = 0.0038671575;
			}
		}
	}
	else {
		if ((input[53]) >= (0.5)) {
			if ((input[57]) >= (1.4807501)) {
				var67 = -0.0070293583;
			}
			else {
				var67 = -0.03290257;
			}
		}
		else {
			if ((input[71]) >= (431.5)) {
				var67 = -0.010519083;
			}
			else {
				var67 = 0.018782407;
			}
		}
	}
	double var68;
	if ((input[64]) >= (45.5)) {
		if ((input[71]) >= (19803.5)) {
			if ((input[70]) >= (4.5)) {
				var68 = 0.01328923;
			}
			else {
				var68 = -0.025286434;
			}
		}
		else {
			if ((input[70]) >= (1.5)) {
				var68 = 0.01248522;
			}
			else {
				var68 = 0.05011188;
			}
		}
	}
	else {
		if ((input[71]) >= (455.5)) {
			if ((input[58]) >= (15.9746)) {
				var68 = -0.0054087196;
			}
			else {
				var68 = -0.072663836;
			}
		}
		else {
			if ((input[61]) >= (0.06565)) {
				var68 = 0.022479417;
			}
			else {
				var68 = -0.0030785815;
			}
		}
	}
	double var69;
	if ((input[63]) >= (0.96205)) {
		if ((input[71]) >= (1012.5)) {
			if ((input[64]) >= (43.5)) {
				var69 = 0.005719278;
			}
			else {
				var69 = -0.02448679;
			}
		}
		else {
			if ((input[53]) >= (1.5)) {
				var69 = -0.016333118;
			}
			else {
				var69 = 0.010948452;
			}
		}
	}
	else {
		if ((input[0]) >= (0.5)) {
			if ((input[66]) >= (6152.5)) {
				var69 = 0.0019770444;
			}
			else {
				var69 = 0.01959457;
			}
		}
		else {
			if ((input[66]) >= (209.5)) {
				var69 = -0.031534463;
			}
			else {
				var69 = 0.0062126894;
			}
		}
	}
	double var70;
	if ((input[64]) >= (40.5)) {
		if ((input[71]) >= (33249.5)) {
			if ((input[71]) >= (66461.0)) {
				var70 = -0.055357452;
			}
			else {
				var70 = -0.018082354;
			}
		}
		else {
			if ((input[1]) >= (33.4766)) {
				var70 = -0.0107350955;
			}
			else {
				var70 = 0.0121929245;
			}
		}
	}
	else {
		if ((input[66]) >= (174.5)) {
			if ((input[0]) >= (1.0)) {
				var70 = -0.01060799;
			}
			else {
				var70 = -0.06172527;
			}
		}
		else {
			if ((input[62]) >= (19.6373)) {
				var70 = 0.04343701;
			}
			else {
				var70 = 0.00010802121;
			}
		}
	}
	double var71;
	if ((input[57]) >= (2.55285)) {
		if ((input[66]) >= (-163.5)) {
			if ((input[70]) >= (50.5)) {
				var71 = -0.027123753;
			}
			else {
				var71 = 0.014438971;
			}
		}
		else {
			if ((input[70]) >= (2.5)) {
				var71 = -0.046776276;
			}
			else {
				var71 = -0.22834204;
			}
		}
	}
	else {
		if ((input[66]) >= (47.5)) {
			if ((input[58]) >= (0.91795003)) {
				var71 = -0.004184503;
			}
			else {
				var71 = -0.038416293;
			}
		}
		else {
			if ((input[70]) >= (0.5)) {
				var71 = 0.010394155;
			}
			else {
				var71 = 0.07642355;
			}
		}
	}
	double var72;
	if ((input[46]) >= (4.6555)) {
		if ((input[71]) >= (1733.5)) {
			if ((input[58]) >= (1853.4043)) {
				var72 = 0.008494479;
			}
			else {
				var72 = -0.01625489;
			}
		}
		else {
			if ((input[58]) >= (473.16992)) {
				var72 = 0.030189047;
			}
			else {
				var72 = 0.005712019;
			}
		}
	}
	else {
		if ((input[70]) >= (1.5)) {
			if ((input[62]) >= (0.1029)) {
				var72 = -0.010121705;
			}
			else {
				var72 = -0.05402009;
			}
		}
		else {
			if ((input[71]) >= (26.5)) {
				var72 = -0.0021958516;
			}
			else {
				var72 = 0.057356246;
			}
		}
	}
	double var73;
	if ((input[1]) >= (4.3828)) {
		if ((input[57]) >= (1.82325)) {
			if ((input[70]) >= (19.5)) {
				var73 = -0.019104086;
			}
			else {
				var73 = 0.0045789536;
			}
		}
		else {
			if ((input[66]) >= (40.5)) {
				var73 = -0.02046229;
			}
			else {
				var73 = 0.019244859;
			}
		}
	}
	else {
		if ((input[56]) >= (7.5008497)) {
			if ((input[71]) >= (897.5)) {
				var73 = 0.0054782717;
			}
			else {
				var73 = 0.025724215;
			}
		}
		else {
			if ((input[71]) >= (32.5)) {
				var73 = -0.018561525;
			}
			else {
				var73 = 0.011147749;
			}
		}
	}
	double var74;
	if ((input[64]) >= (46.5)) {
		if ((input[71]) >= (-687.5)) {
			if ((input[66]) >= (5345.5)) {
				var74 = 0.023492051;
			}
			else {
				var74 = 0.07355858;
			}
		}
		else {
			if ((input[45]) >= (3.5)) {
				var74 = -0.09957908;
			}
			else {
				var74 = -0.00923557;
			}
		}
	}
	else {
		if ((input[12]) >= (9.3516)) {
			if ((input[57]) >= (3.04135)) {
				var74 = 0.013105712;
			}
			else {
				var74 = -0.011995969;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var74 = 0.009597808;
			}
			else {
				var74 = -0.0050459336;
			}
		}
	}
	double var75;
	if ((input[63]) >= (0.76094997)) {
		if ((input[71]) >= (715.5)) {
			if ((input[58]) >= (23.013649)) {
				var75 = -0.0074178292;
			}
			else {
				var75 = -0.06428524;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var75 = 0.0150261065;
			}
			else {
				var75 = -0.006336752;
			}
		}
	}
	else {
		if ((input[71]) >= (7712.5)) {
			if ((input[57]) >= (2.85525)) {
				var75 = 0.008116785;
			}
			else {
				var75 = -0.012996807;
			}
		}
		else {
			if ((input[56]) >= (2691.7778)) {
				var75 = 0.04476851;
			}
			else {
				var75 = 0.012057552;
			}
		}
	}
	double var76;
	if ((input[12]) >= (0.64845)) {
		if ((input[58]) >= (488.36523)) {
			if ((input[71]) >= (2279.5)) {
				var76 = -0.002491703;
			}
			else {
				var76 = 0.018818526;
			}
		}
		else {
			if ((input[66]) >= (281.5)) {
				var76 = -0.020466767;
			}
			else {
				var76 = 0.0014956961;
			}
		}
	}
	else {
		if ((input[53]) >= (0.5)) {
			if ((input[62]) >= (1.24355)) {
				var76 = 0.0124827735;
			}
			else {
				var76 = -0.009367782;
			}
		}
		else {
			if ((input[70]) >= (6.5)) {
				var76 = 0.0031754721;
			}
			else {
				var76 = 0.028736377;
			}
		}
	}
	double var77;
	if ((input[57]) >= (2.0689502)) {
		if ((input[12]) >= (0.0078)) {
			if ((input[71]) >= (-209.5)) {
				var77 = 0.006423615;
			}
			else {
				var77 = -0.04330648;
			}
		}
		else {
			if ((input[61]) >= (0.2568)) {
				var77 = 0.122567594;
			}
			else {
				var77 = 0.047984254;
			}
		}
	}
	else {
		if ((input[53]) >= (0.5)) {
			if ((input[46]) >= (3.9024)) {
				var77 = -0.007998268;
			}
			else {
				var77 = -0.034527954;
			}
		}
		else {
			if ((input[71]) >= (442.5)) {
				var77 = -0.009364094;
			}
			else {
				var77 = 0.0157963;
			}
		}
	}
	double var78;
	if ((input[64]) >= (40.5)) {
		if ((input[71]) >= (23961.5)) {
			if ((input[71]) >= (66461.0)) {
				var78 = -0.051660653;
			}
			else {
				var78 = -0.012479866;
			}
		}
		else {
			if ((input[70]) >= (0.5)) {
				var78 = 0.0059967577;
			}
			else {
				var78 = 0.03902793;
			}
		}
	}
	else {
		if ((input[71]) >= (71.5)) {
			if ((input[58]) >= (6.9355497)) {
				var78 = -0.007856634;
			}
			else {
				var78 = -0.04051442;
			}
		}
		else {
			if ((input[62]) >= (19.65665)) {
				var78 = 0.038683724;
			}
			else {
				var78 = 0.00088627357;
			}
		}
	}
	double var79;
	if ((input[34]) >= (8.0)) {
		if ((input[66]) >= (359.5)) {
			if ((input[0]) >= (0.5)) {
				var79 = -0.019713638;
			}
			else {
				var79 = -0.053601723;
			}
		}
		else {
			if ((input[1]) >= (0.64845)) {
				var79 = -0.012831156;
			}
			else {
				var79 = 0.034533914;
			}
		}
	}
	else {
		if ((input[57]) >= (0.54465)) {
			if ((input[66]) >= (-117.5)) {
				var79 = 0.0037862973;
			}
			else {
				var79 = -0.047137838;
			}
		}
		else {
			if ((input[66]) >= (10.5)) {
				var79 = -0.025087569;
			}
			else {
				var79 = 0.05576117;
			}
		}
	}
	double var80;
	if ((input[1]) >= (33.8516)) {
		if ((input[55]) >= (108.27345)) {
			if ((input[71]) >= (670.5)) {
				var80 = -0.04400754;
			}
			else {
				var80 = -0.011978536;
			}
		}
		else {
			if ((input[70]) >= (17.5)) {
				var80 = -0.03432714;
			}
			else {
				var80 = 0.002535748;
			}
		}
	}
	else {
		if ((input[58]) >= (2398.6367)) {
			if ((input[66]) >= (4211.5)) {
				var80 = 0.003607435;
			}
			else {
				var80 = 0.036462173;
			}
		}
		else {
			if ((input[1]) >= (1.8359499)) {
				var80 = -0.0052110753;
			}
			else {
				var80 = 0.008552422;
			}
		}
	}
	double var81;
	if ((input[64]) >= (46.5)) {
		if ((input[71]) >= (-36.5)) {
			if ((input[66]) >= (5345.5)) {
				var81 = 0.022287166;
			}
			else {
				var81 = 0.07408106;
			}
		}
		else {
			if ((input[59]) >= (3.6066499)) {
				var81 = 0.12084263;
			}
			else {
				var81 = -0.04382762;
			}
		}
	}
	else {
		if ((input[1]) >= (9.4141)) {
			if ((input[63]) >= (0.99095)) {
				var81 = -0.014229955;
			}
			else {
				var81 = 0.0027684488;
			}
		}
		else {
			if ((input[3]) >= (2.45)) {
				var81 = 0.008631594;
			}
			else {
				var81 = -0.0046425904;
			}
		}
	}
	double var82;
	if ((input[46]) >= (4.6912003)) {
		if ((input[71]) >= (1733.5)) {
			if ((input[58]) >= (28.742199)) {
				var82 = -0.001303042;
			}
			else {
				var82 = -0.086763725;
			}
		}
		else {
			if ((input[56]) >= (1075.8949)) {
				var82 = 0.040798202;
			}
			else {
				var82 = 0.007590104;
			}
		}
	}
	else {
		if ((input[53]) >= (1.5)) {
			if ((input[46]) >= (3.68735)) {
				var82 = -0.010427308;
			}
			else {
				var82 = -0.04654683;
			}
		}
		else {
			if ((input[52]) >= (1.5)) {
				var82 = -0.022329248;
			}
			else {
				var82 = 0.0035799507;
			}
		}
	}
	double var83;
	if ((input[57]) >= (3.12495)) {
		if ((input[70]) >= (55.5)) {
			if ((input[71]) >= (12480.0)) {
				var83 = -0.055162203;
			}
			else {
				var83 = -0.011933651;
			}
		}
		else {
			if ((input[70]) >= (4.5)) {
				var83 = 0.03137962;
			}
			else {
				var83 = 0.008648111;
			}
		}
	}
	else {
		if ((input[71]) >= (763.5)) {
			if ((input[58]) >= (1176.0059)) {
				var83 = 0.0019305367;
			}
			else {
				var83 = -0.02101068;
			}
		}
		else {
			if ((input[70]) >= (0.5)) {
				var83 = 0.0018963388;
			}
			else {
				var83 = 0.05041459;
			}
		}
	}
	double var84;
	if ((input[64]) >= (43.5)) {
		if ((input[71]) >= (17388.0)) {
			if ((input[70]) >= (4.5)) {
				var84 = 0.01103486;
			}
			else {
				var84 = -0.02522992;
			}
		}
		else {
			if ((input[71]) >= (-317.0)) {
				var84 = 0.018637367;
			}
			else {
				var84 = -0.04076908;
			}
		}
	}
	else {
		if ((input[66]) >= (338.5)) {
			if ((input[56]) >= (7.95605)) {
				var84 = -0.00432986;
			}
			else {
				var84 = -0.05460168;
			}
		}
		else {
			if ((input[62]) >= (15.188601)) {
				var84 = 0.023964169;
			}
			else {
				var84 = -0.00022822211;
			}
		}
	}
	double var85;
	if ((input[64]) >= (39.5)) {
		if ((input[71]) >= (2065.5)) {
			if ((input[0]) >= (0.5)) {
				var85 = -0.0013726095;
			}
			else {
				var85 = -0.048632585;
			}
		}
		else {
			if ((input[70]) >= (1.5)) {
				var85 = 0.0041741203;
			}
			else {
				var85 = 0.035151463;
			}
		}
	}
	else {
		if ((input[66]) >= (43.5)) {
			if ((input[58]) >= (0.18555)) {
				var85 = -0.010210744;
			}
			else {
				var85 = -0.079896405;
			}
		}
		else {
			if ((input[71]) >= (13.5)) {
				var85 = -0.017139;
			}
			else {
				var85 = 0.018401185;
			}
		}
	}
	double var86;
	if ((input[64]) >= (46.5)) {
		if ((input[71]) >= (-36.5)) {
			if ((input[66]) >= (5345.5)) {
				var86 = 0.020601526;
			}
			else {
				var86 = 0.068444684;
			}
		}
		else {
			if ((input[61]) >= (0.0392)) {
				var86 = -0.047034252;
			}
			else {
				var86 = 0.039632384;
			}
		}
	}
	else {
		if ((input[12]) >= (35.21095)) {
			if ((input[55]) >= (130.0547)) {
				var86 = -0.0392607;
			}
			else {
				var86 = -0.010733483;
			}
		}
		else {
			if ((input[58]) >= (2783.6543)) {
				var86 = 0.015214293;
			}
			else {
				var86 = -0.0011191027;
			}
		}
	}
	double var87;
	if ((input[70]) >= (78.5)) {
		if ((input[63]) >= (0.71485)) {
			if ((input[70]) >= (116.5)) {
				var87 = -0.06528677;
			}
			else {
				var87 = -0.029822785;
			}
		}
		else {
			if ((input[71]) >= (817.5)) {
				var87 = -0.026524654;
			}
			else {
				var87 = 0.079604715;
			}
		}
	}
	else {
		if ((input[57]) >= (1.59165)) {
			if ((input[71]) >= (3477.5)) {
				var87 = -0.003194704;
			}
			else {
				var87 = 0.010134408;
			}
		}
		else {
			if ((input[71]) >= (16.5)) {
				var87 = -0.01349619;
			}
			else {
				var87 = 0.009068085;
			}
		}
	}
	double var88;
	if ((input[46]) >= (4.8725)) {
		if ((input[71]) >= (11587.5)) {
			if ((input[64]) >= (46.5)) {
				var88 = 0.018341875;
			}
			else {
				var88 = -0.015433746;
			}
		}
		else {
			if ((input[58]) >= (2530.787)) {
				var88 = 0.023429114;
			}
			else {
				var88 = 0.004954266;
			}
		}
	}
	else {
		if ((input[53]) >= (0.5)) {
			if ((input[62]) >= (0.110249996)) {
				var88 = -0.008690913;
			}
			else {
				var88 = -0.04184349;
			}
		}
		else {
			if ((input[52]) >= (0.5)) {
				var88 = -0.0066540665;
			}
			else {
				var88 = 0.011288497;
			}
		}
	}
	double var89;
	if ((input[1]) >= (0.22659999)) {
		if ((input[62]) >= (0.0282)) {
			if ((input[34]) >= (8.0)) {
				var89 = -0.020319754;
			}
			else {
				var89 = 0.0008163447;
			}
		}
		else {
			if ((input[71]) >= (6.5)) {
				var89 = -0.07526982;
			}
			else {
				var89 = -0.003493571;
			}
		}
	}
	else {
		if ((input[13]) >= (2.5)) {
			if ((input[12]) >= (0.0078)) {
				var89 = 0.014594137;
			}
			else {
				var89 = 0.048179932;
			}
		}
		else {
			if ((input[70]) >= (6.5)) {
				var89 = -0.027473966;
			}
			else {
				var89 = 0.009239572;
			}
		}
	}
	double var90;
	if ((input[63]) >= (0.48865)) {
		if ((input[71]) >= (1182.5)) {
			if ((input[0]) >= (0.5)) {
				var90 = -0.004829917;
			}
			else {
				var90 = -0.04843428;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var90 = 0.01130799;
			}
			else {
				var90 = -0.0060649547;
			}
		}
	}
	else {
		if ((input[71]) >= (5974.0)) {
			if ((input[70]) >= (4.5)) {
				var90 = 0.011670108;
			}
			else {
				var90 = -0.009181778;
			}
		}
		else {
			if ((input[56]) >= (2586.3044)) {
				var90 = 0.058204897;
			}
			else {
				var90 = 0.01749229;
			}
		}
	}
	double var91;
	if ((input[57]) >= (3.12495)) {
		if ((input[66]) >= (-573.5)) {
			if ((input[66]) >= (2335.5)) {
				var91 = 0.011602117;
			}
			else {
				var91 = 0.036451824;
			}
		}
		else {
			if ((input[70]) >= (1.5)) {
				var91 = -0.06345939;
			}
			else {
				var91 = -0.28727552;
			}
		}
	}
	else {
		if ((input[71]) >= (450.5)) {
			if ((input[58]) >= (14.79295)) {
				var91 = -0.003869788;
			}
			else {
				var91 = -0.050753217;
			}
		}
		else {
			if ((input[62]) >= (92.424805)) {
				var91 = 0.021852797;
			}
			else {
				var91 = -0.0002657947;
			}
		}
	}
	double var92;
	if ((input[64]) >= (45.5)) {
		if ((input[71]) >= (19803.5)) {
			if ((input[71]) >= (63067.0)) {
				var92 = -0.04490679;
			}
			else {
				var92 = -0.0012070866;
			}
		}
		else {
			if ((input[0]) >= (6.0)) {
				var92 = 0.00070521625;
			}
			else {
				var92 = 0.03415912;
			}
		}
	}
	else {
		if ((input[66]) >= (211.5)) {
			if ((input[64]) >= (37.5)) {
				var92 = -0.0030671852;
			}
			else {
				var92 = -0.05940787;
			}
		}
		else {
			if ((input[71]) >= (-241.5)) {
				var92 = 0.008615422;
			}
			else {
				var92 = -0.036671326;
			}
		}
	}
	double var93;
	if ((input[46]) >= (4.71035)) {
		if ((input[71]) >= (4262.5)) {
			if ((input[64]) >= (43.5)) {
				var93 = 0.0053561213;
			}
			else {
				var93 = -0.011939241;
			}
		}
		else {
			if ((input[56]) >= (2742.6084)) {
				var93 = 0.04707734;
			}
			else {
				var93 = 0.005211522;
			}
		}
	}
	else {
		if ((input[53]) >= (1.5)) {
			if ((input[46]) >= (3.6871)) {
				var93 = -0.008541912;
			}
			else {
				var93 = -0.04150369;
			}
		}
		else {
			if ((input[52]) >= (1.5)) {
				var93 = -0.019375874;
			}
			else {
				var93 = 0.00282487;
			}
		}
	}
	double var94;
	if ((input[64]) >= (40.5)) {
		if ((input[66]) >= (-117.5)) {
			if ((input[66]) >= (412.5)) {
				var94 = 0.0026177836;
			}
			else {
				var94 = 0.03210486;
			}
		}
		else {
			if ((input[70]) >= (5.5)) {
				var94 = -0.0014702035;
			}
			else {
				var94 = -0.08327534;
			}
		}
	}
	else {
		if ((input[66]) >= (128.5)) {
			if ((input[58]) >= (7.8730497)) {
				var94 = -0.005874216;
			}
			else {
				var94 = -0.034180503;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var94 = 0.036813136;
			}
			else {
				var94 = 0.0020281035;
			}
		}
	}
	double var95;
	if ((input[12]) >= (3.89845)) {
		if ((input[56]) >= (145.28079)) {
			if ((input[71]) >= (2741.5)) {
				var95 = -0.005845918;
			}
			else {
				var95 = 0.008984961;
			}
		}
		else {
			if ((input[66]) >= (68.5)) {
				var95 = -0.020874817;
			}
			else {
				var95 = 0.014444768;
			}
		}
	}
	else {
		if ((input[58]) >= (1.2246001)) {
			if ((input[71]) >= (-113.5)) {
				var95 = 0.010977887;
			}
			else {
				var95 = -0.038036104;
			}
		}
		else {
			if ((input[66]) >= (9.5)) {
				var95 = -0.015054646;
			}
			else {
				var95 = 0.043795146;
			}
		}
	}
	double var96;
	if ((input[57]) >= (2.55285)) {
		if ((input[71]) >= (-335.5)) {
			if ((input[66]) >= (621.5)) {
				var96 = 0.006441634;
			}
			else {
				var96 = 0.031304155;
			}
		}
		else {
			if ((input[70]) >= (3.5)) {
				var96 = -0.028544623;
			}
			else {
				var96 = -0.13236822;
			}
		}
	}
	else {
		if ((input[66]) >= (338.5)) {
			if ((input[58]) >= (1165.5859)) {
				var96 = 0.0036230048;
			}
			else {
				var96 = -0.015972665;
			}
		}
		else {
			if ((input[64]) >= (38.5)) {
				var96 = 0.019607922;
			}
			else {
				var96 = -0.0027902434;
			}
		}
	}
	double var97;
	if ((input[70]) >= (79.5)) {
		if ((input[61]) >= (0.28254998)) {
			if ((input[58]) >= (1127.9844)) {
				var97 = -0.03923891;
			}
			else {
				var97 = -0.094569966;
			}
		}
		else {
			if ((input[71]) >= (817.5)) {
				var97 = -0.023404164;
			}
			else {
				var97 = 0.031425368;
			}
		}
	}
	else {
		if ((input[58]) >= (2787.0762)) {
			if ((input[55]) >= (107.89845)) {
				var97 = -0.0057287044;
			}
			else {
				var97 = 0.024482524;
			}
		}
		else {
			if ((input[71]) >= (917.5)) {
				var97 = -0.007472133;
			}
			else {
				var97 = 0.0030226116;
			}
		}
	}
	double var98;
	if ((input[71]) >= (37085.5)) {
		if ((input[57]) >= (2.5622501)) {
			if ((input[64]) >= (46.5)) {
				var98 = 0.012432326;
			}
			else {
				var98 = -0.023754409;
			}
		}
		else {
			if ((input[55]) >= (89.6172)) {
				var98 = -0.060734004;
			}
			else {
				var98 = -0.025710642;
			}
		}
	}
	else {
		if ((input[64]) >= (43.5)) {
			if ((input[71]) >= (-43.5)) {
				var98 = 0.013812961;
			}
			else {
				var98 = -0.028786203;
			}
		}
		else {
			if ((input[63]) >= (0.52825)) {
				var98 = -0.0032099697;
			}
			else {
				var98 = 0.021839926;
			}
		}
	}
	double var99;
	if ((input[46]) >= (4.8725)) {
		if ((input[71]) >= (9024.5)) {
			if ((input[0]) >= (0.5)) {
				var99 = -0.006059475;
			}
			else {
				var99 = -0.056219406;
			}
		}
		else {
			if ((input[64]) >= (43.5)) {
				var99 = 0.022103341;
			}
			else {
				var99 = 0.0048478413;
			}
		}
	}
	else {
		if ((input[53]) >= (0.5)) {
			if ((input[57]) >= (1.4872501)) {
				var99 = -0.0031460691;
			}
			else {
				var99 = -0.019796511;
			}
		}
		else {
			if ((input[71]) >= (21.5)) {
				var99 = -0.0010258406;
			}
			else {
				var99 = 0.01835265;
			}
		}
	}
	double var100;
	var100 = (1.0) / ((1.0) + (exp((0.0) - ((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((var0)+(var1)) + (var2)) + (var3)) + (var4)) + (var5)) + (var6)) + (var7)) + (var8)) + (var9)) + (var10)) + (var11)) + (var12)) + (var13)) + (var14)) + (var15)) + (var16)) + (var17)) + (var18)) + (var19)) + (var20)) + (var21)) + (var22)) + (var23)) + (var24)) + (var25)) + (var26)) + (var27)) + (var28)) + (var29)) + (var30)) + (var31)) + (var32)) + (var33)) + (var34)) + (var35)) + (var36)) + (var37)) + (var38)) + (var39)) + (var40)) + (var41)) + (var42)) + (var43)) + (var44)) + (var45)) + (var46)) + (var47)) + (var48)) + (var49)) + (var50)) + (var51)) + (var52)) + (var53)) + (var54)) + (var55)) + (var56)) + (var57)) + (var58)) + (var59)) + (var60)) + (var61)) + (var62)) + (var63)) + (var64)) + (var65)) + (var66)) + (var67)) + (var68)) + (var69)) + (var70)) + (var71)) + (var72)) + (var73)) + (var74)) + (var75)) + (var76)) + (var77)) + (var78)) + (var79)) + (var80)) + (var81)) + (var82)) + (var83)) + (var84)) + (var85)) + (var86)) + (var87)) + (var88)) + (var89)) + (var90)) + (var91)) + (var92)) + (var93)) + (var94)) + (var95)) + (var96)) + (var97)) + (var98)) + (var99)))));
	*output = var100 < 0.5 ? 0 : 1;
}

