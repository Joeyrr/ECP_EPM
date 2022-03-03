/* The copyright in this software is being made available under the BSD
 * License, included below. This software may be subject to other third party
 * and contributor rights, including patent rights, and no such rights are
 * granted under this license.
 *
 * Copyright (c) 2010-2015, ITU/ISO/IEC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *  * Neither the name of the ITU/ISO/IEC nor the names of its contributors may
 *    be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

/** \file     TEncCu.cpp
    \brief    Coding Unit (CU) encoder class
*/

#include <stdio.h>
#include "TEncTop.h"
#include "TEncCu.h"
#include "TEncAnalyze.h"
#include "TLibCommon/Debug.h"

#include "TLibCommon/TComStatistics.h"

#include <cmath>
#include <algorithm>
using namespace std;


//! \ingroup TLibEncoder
//! \{

// ====================================================================================================================
// Constructor / destructor / create / destroy
// ====================================================================================================================

/**
 \param    uhTotalDepth  total number of allowable depth
 \param    uiMaxWidth    largest CU width
 \param    uiMaxHeight   largest CU height
 \param    chromaFormat  chroma format
 */
Void TEncCu::create(UChar uhTotalDepth, UInt uiMaxWidth, UInt uiMaxHeight, ChromaFormat chromaFormat)
{
  Int i;

  m_uhTotalDepth   = uhTotalDepth + 1;
  m_ppcBestCU      = new TComDataCU*[m_uhTotalDepth-1];
  m_ppcTempCU      = new TComDataCU*[m_uhTotalDepth-1];

#if NH_3D_ARP
  m_ppcWeightedTempCU = new TComDataCU*[m_uhTotalDepth-1];
#endif 

  m_ppcPredYuvBest = new TComYuv*[m_uhTotalDepth-1];
  m_ppcResiYuvBest = new TComYuv*[m_uhTotalDepth-1];
  m_ppcRecoYuvBest = new TComYuv*[m_uhTotalDepth-1];
  m_ppcPredYuvTemp = new TComYuv*[m_uhTotalDepth-1];
  m_ppcResiYuvTemp = new TComYuv*[m_uhTotalDepth-1];
  m_ppcRecoYuvTemp = new TComYuv*[m_uhTotalDepth-1];
  m_ppcOrigYuv     = new TComYuv*[m_uhTotalDepth-1];
#if NH_3D_DBBP
  m_ppcOrigYuvDBBP = new TComYuv*[m_uhTotalDepth-1];
#endif

  UInt uiNumPartitions;
  for( i=0 ; i<m_uhTotalDepth-1 ; i++)
  {
    uiNumPartitions = 1<<( ( m_uhTotalDepth - i - 1 )<<1 );
    UInt uiWidth  = uiMaxWidth  >> i;
    UInt uiHeight = uiMaxHeight >> i;

    m_ppcBestCU[i] = new TComDataCU; m_ppcBestCU[i]->create( chromaFormat, uiNumPartitions, uiWidth, uiHeight, false, uiMaxWidth >> (m_uhTotalDepth - 1) );
    m_ppcTempCU[i] = new TComDataCU; m_ppcTempCU[i]->create( chromaFormat, uiNumPartitions, uiWidth, uiHeight, false, uiMaxWidth >> (m_uhTotalDepth - 1) );
#if NH_3D_ARP
    m_ppcWeightedTempCU[i] = new TComDataCU; m_ppcWeightedTempCU[i]->create( chromaFormat, uiNumPartitions, uiWidth, uiHeight, false, uiMaxWidth >> (m_uhTotalDepth - 1) );
#endif  

    m_ppcPredYuvBest[i] = new TComYuv; m_ppcPredYuvBest[i]->create(uiWidth, uiHeight, chromaFormat);
    m_ppcResiYuvBest[i] = new TComYuv; m_ppcResiYuvBest[i]->create(uiWidth, uiHeight, chromaFormat);
    m_ppcRecoYuvBest[i] = new TComYuv; m_ppcRecoYuvBest[i]->create(uiWidth, uiHeight, chromaFormat);

    m_ppcPredYuvTemp[i] = new TComYuv; m_ppcPredYuvTemp[i]->create(uiWidth, uiHeight, chromaFormat);
    m_ppcResiYuvTemp[i] = new TComYuv; m_ppcResiYuvTemp[i]->create(uiWidth, uiHeight, chromaFormat);
    m_ppcRecoYuvTemp[i] = new TComYuv; m_ppcRecoYuvTemp[i]->create(uiWidth, uiHeight, chromaFormat);

    m_ppcOrigYuv    [i] = new TComYuv; m_ppcOrigYuv    [i]->create(uiWidth, uiHeight, chromaFormat);
#if NH_3D_DBBP
    m_ppcOrigYuvDBBP[i] = new TComYuv; m_ppcOrigYuvDBBP[i]->create(uiWidth, uiHeight, chromaFormat);
#endif

  }

  m_bEncodeDQP          = false;

#if KWU_RC_MADPRED_E0227
  m_LCUPredictionSAD = 0;
  m_addSADDepth      = 0;
  m_temporalSAD      = 0;
  m_spatialSAD       = 0;
#endif

  m_stillToCodeChromaQpOffsetFlag  = false;
  m_cuChromaQpOffsetIdxPlus1       = 0;
  m_bFastDeltaQP                   = false;

  // initialize partition order.
  UInt* piTmp = &g_auiZscanToRaster[0];
  initZscanToRaster( m_uhTotalDepth, 1, 0, piTmp);
  initRasterToZscan( uiMaxWidth, uiMaxHeight, m_uhTotalDepth );

  // initialize conversion matrix from partition index to pel
  initRasterToPelXY( uiMaxWidth, uiMaxHeight, m_uhTotalDepth );
}

Void TEncCu::destroy()
{
  Int i;

  for( i=0 ; i<m_uhTotalDepth-1 ; i++)
  {
    if(m_ppcBestCU[i])
    {
      m_ppcBestCU[i]->destroy();      delete m_ppcBestCU[i];      m_ppcBestCU[i] = NULL;
    }
    if(m_ppcTempCU[i])
    {
      m_ppcTempCU[i]->destroy();      delete m_ppcTempCU[i];      m_ppcTempCU[i] = NULL;
    }
#if NH_3D_ARP
    if(m_ppcWeightedTempCU[i])
    {
      m_ppcWeightedTempCU[i]->destroy(); delete m_ppcWeightedTempCU[i]; m_ppcWeightedTempCU[i] = NULL;
    }
#endif
    if(m_ppcPredYuvBest[i])
    {
      m_ppcPredYuvBest[i]->destroy(); delete m_ppcPredYuvBest[i]; m_ppcPredYuvBest[i] = NULL;
    }
    if(m_ppcResiYuvBest[i])
    {
      m_ppcResiYuvBest[i]->destroy(); delete m_ppcResiYuvBest[i]; m_ppcResiYuvBest[i] = NULL;
    }
    if(m_ppcRecoYuvBest[i])
    {
      m_ppcRecoYuvBest[i]->destroy(); delete m_ppcRecoYuvBest[i]; m_ppcRecoYuvBest[i] = NULL;
    }
    if(m_ppcPredYuvTemp[i])
    {
      m_ppcPredYuvTemp[i]->destroy(); delete m_ppcPredYuvTemp[i]; m_ppcPredYuvTemp[i] = NULL;
    }
    if(m_ppcResiYuvTemp[i])
    {
      m_ppcResiYuvTemp[i]->destroy(); delete m_ppcResiYuvTemp[i]; m_ppcResiYuvTemp[i] = NULL;
    }
    if(m_ppcRecoYuvTemp[i])
    {
      m_ppcRecoYuvTemp[i]->destroy(); delete m_ppcRecoYuvTemp[i]; m_ppcRecoYuvTemp[i] = NULL;
    }
    if(m_ppcOrigYuv[i])
    {
      m_ppcOrigYuv[i]->destroy();     delete m_ppcOrigYuv[i];     m_ppcOrigYuv[i] = NULL;
    }
#if NH_3D_DBBP
    if(m_ppcOrigYuvDBBP[i])
    {
      m_ppcOrigYuvDBBP[i]->destroy(); delete m_ppcOrigYuvDBBP[i]; m_ppcOrigYuvDBBP[i] = NULL;
    }
#endif
  }
  if(m_ppcBestCU)
  {
    delete [] m_ppcBestCU;
    m_ppcBestCU = NULL;
  }
  if(m_ppcTempCU)
  {
    delete [] m_ppcTempCU;
    m_ppcTempCU = NULL;
  }

#if NH_3D_ARP
  if(m_ppcWeightedTempCU)
  {
    delete [] m_ppcWeightedTempCU; 
    m_ppcWeightedTempCU = NULL; 
  }
#endif
  if(m_ppcPredYuvBest)
  {
    delete [] m_ppcPredYuvBest;
    m_ppcPredYuvBest = NULL;
  }
  if(m_ppcResiYuvBest)
  {
    delete [] m_ppcResiYuvBest;
    m_ppcResiYuvBest = NULL;
  }
  if(m_ppcRecoYuvBest)
  {
    delete [] m_ppcRecoYuvBest;
    m_ppcRecoYuvBest = NULL;
  }
  if(m_ppcPredYuvTemp)
  {
    delete [] m_ppcPredYuvTemp;
    m_ppcPredYuvTemp = NULL;
  }
  if(m_ppcResiYuvTemp)
  {
    delete [] m_ppcResiYuvTemp;
    m_ppcResiYuvTemp = NULL;
  }
  if(m_ppcRecoYuvTemp)
  {
    delete [] m_ppcRecoYuvTemp;
    m_ppcRecoYuvTemp = NULL;
  }
  if(m_ppcOrigYuv)
  {
    delete [] m_ppcOrigYuv;
    m_ppcOrigYuv = NULL;
  }
#if NH_3D_DBBP
  if(m_ppcOrigYuvDBBP)
  {
    delete [] m_ppcOrigYuvDBBP;
    m_ppcOrigYuvDBBP = NULL;
  }
#endif
}

/** \param    pcEncTop      pointer of encoder class
 */
Void TEncCu::init( TEncTop* pcEncTop )
{
  m_pcEncCfg           = pcEncTop;
  m_pcPredSearch       = pcEncTop->getPredSearch();
  m_pcTrQuant          = pcEncTop->getTrQuant();
  m_pcRdCost           = pcEncTop->getRdCost();

  m_pcEntropyCoder     = pcEncTop->getEntropyCoder();
  m_pcBinCABAC         = pcEncTop->getBinCABAC();

  m_pppcRDSbacCoder    = pcEncTop->getRDSbacCoder();
  m_pcRDGoOnSbacCoder  = pcEncTop->getRDGoOnSbacCoder();

  m_pcRateCtrl         = pcEncTop->getRateCtrl();
}

// ====================================================================================================================
// Public member functions
// ====================================================================================================================

/** 
 \param  pCtu pointer of CU data class
 */
Void TEncCu::compressCtu( TComDataCU* pCtu )
{
  // initialize CU data
  m_ppcBestCU[0]->initCtu( pCtu->getPic(), pCtu->getCtuRsAddr() );
  m_ppcTempCU[0]->initCtu( pCtu->getPic(), pCtu->getCtuRsAddr() );

#if NH_3D_ARP
  m_ppcWeightedTempCU[0]->initCtu( pCtu->getPic(), pCtu->getCtuRsAddr() );
#endif

#if KWU_RC_MADPRED_E0227
  m_LCUPredictionSAD = 0;
  m_addSADDepth      = 0;
  m_temporalSAD      = 0;
  m_spatialSAD       = 0;
#endif

  // analysis of CU
  DEBUG_STRING_NEW(sDebug)

  xCompressCU( m_ppcBestCU[0], m_ppcTempCU[0], 0 DEBUG_STRING_PASS_INTO(sDebug) );
  DEBUG_STRING_OUTPUT(std::cout, sDebug)

#if PRINT_LABEL
  //if (pCtu->getSlice()->getSliceType() == B_SLICE && pCtu->getSlice()->getIsDepth() && myFlag[0] && myFlag[1])
  if (pCtu->getSlice()->getSliceType() != I_SLICE && pCtu->getSlice()->getIsDepth() && myFlag[0] && myFlag[1]) // 增加P帧
  {
	  //TComDataCU* BestCU = m_ppcBestCU[0];
	  //(UInt)(BestCU->getDepth()[g_auiRasterToZscan[k]]);
	  //这里选择和RDO过程相同的顺序，方便特征和标签对应

    // 根据纹理图深度来确定提取的标签
	  labels[0] = ((UInt)m_ppcBestCU[0]->getDepth(0) > 0 ? 1 : 0); // 64x64
	  labels[1] = ((UInt)m_ppcBestCU[0]->getDepth(0) < 1 ? 2 : (UInt)m_ppcBestCU[0]->getDepth(0) > 1 ? 1 : 0); // 32x32
	  labels[2] = ((UInt)m_ppcBestCU[0]->getDepth(0) < 2 ? 2 : (UInt)m_ppcBestCU[0]->getDepth(0) > 2 ? 1 : 0); // 16x16
	  labels[3] = labels[2] == 1 ? 0 : 2; // 8x8
	  labels[4] = labels[2] == 1 ? 0 : 2; // 8x8
	  labels[5] = labels[2] == 1 ? 0 : 2; // 8x8
	  labels[6] = labels[2] == 1 ? 0 : 2; // 8x8
	  labels[7] = ((UInt)m_ppcBestCU[0]->getDepth(16) < 2 ? 2 : (UInt)m_ppcBestCU[0]->getDepth(16) > 2 ? 1 : 0); // 16x16
	  labels[8] = labels[7] == 1 ? 0 : 2; // 8x8
	  labels[9] = labels[7] == 1 ? 0 : 2; // 8x8
	  labels[10] = labels[7] == 1 ? 0 : 2; // 8x8
	  labels[11] = labels[7] == 1 ? 0 : 2; // 8x8
	  labels[12] = ((UInt)m_ppcBestCU[0]->getDepth(32) < 2 ? 2 : (UInt)m_ppcBestCU[0]->getDepth(32) > 2 ? 1 : 0); // 16x16
	  labels[13] = labels[12] == 1 ? 0 : 2; // 8x8
	  labels[14] = labels[12] == 1 ? 0 : 2; // 8x8
	  labels[15] = labels[12] == 1 ? 0 : 2; // 8x8
	  labels[16] = labels[12] == 1 ? 0 : 2; // 8x8
	  labels[17] = ((UInt)m_ppcBestCU[0]->getDepth(48) < 2 ? 2 : (UInt)m_ppcBestCU[0]->getDepth(48) > 2 ? 1 : 0); // 16x16
	  labels[18] = labels[17] == 1 ? 0 : 2; // 8x8
	  labels[19] = labels[17] == 1 ? 0 : 2; // 8x8
	  labels[20] = labels[17] == 1 ? 0 : 2; // 8x8
	  labels[21] = labels[17] == 1 ? 0 : 2; // 8x8
	  labels[22] = ((UInt)m_ppcBestCU[0]->getDepth(64) < 1 ? 2 : (UInt)m_ppcBestCU[0]->getDepth(64) > 1 ? 1 : 0); // 32x32
	  labels[23] = ((UInt)m_ppcBestCU[0]->getDepth(64) < 2 ? 2 : (UInt)m_ppcBestCU[0]->getDepth(64) > 2 ? 1 : 0); // 16x16
	  labels[24] = labels[23] == 1 ? 0 : 2; // 8x8
	  labels[25] = labels[23] == 1 ? 0 : 2; // 8x8
	  labels[26] = labels[23] == 1 ? 0 : 2; // 8x8
	  labels[27] = labels[23] == 1 ? 0 : 2; // 8x8
	  labels[28] = ((UInt)m_ppcBestCU[0]->getDepth(80) < 2 ? 2 : (UInt)m_ppcBestCU[0]->getDepth(80) > 2 ? 1 : 0); // 16x16
	  labels[29] = labels[28] == 1 ? 0 : 2; // 8x8
	  labels[30] = labels[28] == 1 ? 0 : 2; // 8x8
	  labels[31] = labels[28] == 1 ? 0 : 2; // 8x8
	  labels[32] = labels[28] == 1 ? 0 : 2; // 8x8
	  labels[33] = ((UInt)m_ppcBestCU[0]->getDepth(96) < 2 ? 2 : (UInt)m_ppcBestCU[0]->getDepth(96) > 2 ? 1 : 0); // 16x16
	  labels[34] = labels[33] == 1 ? 0 : 2; // 8x8
	  labels[35] = labels[33] == 1 ? 0 : 2; // 8x8
	  labels[36] = labels[33] == 1 ? 0 : 2; // 8x8
	  labels[37] = labels[33] == 1 ? 0 : 2; // 8x8
	  labels[38] = ((UInt)m_ppcBestCU[0]->getDepth(112) < 2 ? 2 : (UInt)m_ppcBestCU[0]->getDepth(112) > 2 ? 1 : 0); // 16x16
	  labels[39] = labels[38] == 1 ? 0 : 2; // 8x8
	  labels[40] = labels[38] == 1 ? 0 : 2; // 8x8
	  labels[41] = labels[38] == 1 ? 0 : 2; // 8x8
	  labels[42] = labels[38] == 1 ? 0 : 2; // 8x8
	  labels[43] = ((UInt)m_ppcBestCU[0]->getDepth(128) < 1 ? 2 : (UInt)m_ppcBestCU[0]->getDepth(128) > 1 ? 1 : 0); // 32x32
	  labels[44] = ((UInt)m_ppcBestCU[0]->getDepth(128) < 2 ? 2 : (UInt)m_ppcBestCU[0]->getDepth(128) > 2 ? 1 : 0); // 16x16
	  labels[45] = labels[44] == 1 ? 0 : 2; // 8x8
	  labels[46] = labels[44] == 1 ? 0 : 2; // 8x8
	  labels[47] = labels[44] == 1 ? 0 : 2; // 8x8
	  labels[48] = labels[44] == 1 ? 0 : 2; // 8x8
	  labels[49] = ((UInt)m_ppcBestCU[0]->getDepth(144) < 2 ? 2 : (UInt)m_ppcBestCU[0]->getDepth(144) > 2 ? 1 : 0); // 16x16
	  labels[50] = labels[49] == 1 ? 0 : 2; // 8x8
	  labels[51] = labels[49] == 1 ? 0 : 2; // 8x8
	  labels[52] = labels[49] == 1 ? 0 : 2; // 8x8
	  labels[53] = labels[49] == 1 ? 0 : 2; // 8x8
	  labels[54] = ((UInt)m_ppcBestCU[0]->getDepth(160) < 2 ? 2 : (UInt)m_ppcBestCU[0]->getDepth(160) > 2 ? 1 : 0); // 16x16
	  labels[55] = labels[54] == 1 ? 0 : 2; // 8x8
	  labels[56] = labels[54] == 1 ? 0 : 2; // 8x8
	  labels[57] = labels[54] == 1 ? 0 : 2; // 8x8
	  labels[58] = labels[54] == 1 ? 0 : 2; // 8x8
	  labels[59] = ((UInt)m_ppcBestCU[0]->getDepth(176) < 2 ? 2 : (UInt)m_ppcBestCU[0]->getDepth(176) > 2 ? 1 : 0); // 16x16
	  labels[60] = labels[59] == 1 ? 0 : 2; // 8x8
	  labels[61] = labels[59] == 1 ? 0 : 2; // 8x8
	  labels[62] = labels[59] == 1 ? 0 : 2; // 8x8
	  labels[63] = labels[59] == 1 ? 0 : 2; // 8x8
	  labels[64] = ((UInt)m_ppcBestCU[0]->getDepth(192) < 1 ? 2 : (UInt)m_ppcBestCU[0]->getDepth(192) > 1 ? 1 : 0); // 32x32
	  labels[65] = ((UInt)m_ppcBestCU[0]->getDepth(192) < 2 ? 2 : (UInt)m_ppcBestCU[0]->getDepth(192) > 2 ? 1 : 0); // 16x16
	  labels[66] = labels[65] == 1 ? 0 : 2; // 8x8
	  labels[67] = labels[65] == 1 ? 0 : 2; // 8x8
	  labels[68] = labels[65] == 1 ? 0 : 2; // 8x8
	  labels[69] = labels[65] == 1 ? 0 : 2; // 8x8
	  labels[70] = ((UInt)m_ppcBestCU[0]->getDepth(208) < 2 ? 2 : (UInt)m_ppcBestCU[0]->getDepth(208) > 2 ? 1 : 0); // 16x16
	  labels[71] = labels[70] == 1 ? 0 : 2; // 8x8
	  labels[72] = labels[70] == 1 ? 0 : 2; // 8x8
	  labels[73] = labels[70] == 1 ? 0 : 2; // 8x8
	  labels[74] = labels[70] == 1 ? 0 : 2; // 8x8
	  labels[75] = ((UInt)m_ppcBestCU[0]->getDepth(224) < 2 ? 2 : (UInt)m_ppcBestCU[0]->getDepth(224) > 2 ? 1 : 0); // 16x16
	  labels[76] = labels[75] == 1 ? 0 : 2; // 8x8
	  labels[77] = labels[75] == 1 ? 0 : 2; // 8x8
	  labels[78] = labels[75] == 1 ? 0 : 2; // 8x8
	  labels[79] = labels[75] == 1 ? 0 : 2; // 8x8
	  labels[80] = ((UInt)m_ppcBestCU[0]->getDepth(240) < 2 ? 2 : (UInt)m_ppcBestCU[0]->getDepth(240) > 2 ? 1 : 0); // 16x16
	  labels[81] = labels[80] == 1 ? 0 : 2; // 8x8
	  labels[82] = labels[80] == 1 ? 0 : 2; // 8x8
	  labels[83] = labels[80] == 1 ? 0 : 2; // 8x8
	  labels[84] = labels[80] == 1 ? 0 : 2; // 8x8

	// 增加模式选择信息
	PU_labels[0] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(0) && (UInt)m_ppcBestCU[0]->getMergeFlag(0)) ? 1 : 0; // 64x64
	PU_labels[1] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(0) && (UInt)m_ppcBestCU[0]->getMergeFlag(0)) ? 1 : 0; // 32x32
	PU_labels[2] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(0) && (UInt)m_ppcBestCU[0]->getMergeFlag(0)) ? 1 : 0; // 16x16
	PU_labels[3] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(0) && (UInt)m_ppcBestCU[0]->getMergeFlag(0)) ? 1 : 0; // 8x8
	PU_labels[4] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(4) && (UInt)m_ppcBestCU[0]->getMergeFlag(4)) ? 1 : 0; // 8x8
	PU_labels[5] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(8) && (UInt)m_ppcBestCU[0]->getMergeFlag(8)) ? 1 : 0; // 8x8
	PU_labels[6] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(12) && (UInt)m_ppcBestCU[0]->getMergeFlag(12)) ? 1 : 0; // 8x8
	PU_labels[7] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(16) && (UInt)m_ppcBestCU[0]->getMergeFlag(16)) ? 1 : 0; // 16x16
	PU_labels[8] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(16) && (UInt)m_ppcBestCU[0]->getMergeFlag(16)) ? 1 : 0; // 8x8
	PU_labels[9] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(20) && (UInt)m_ppcBestCU[0]->getMergeFlag(20)) ? 1 : 0; // 8x8
	PU_labels[10] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(24) && (UInt)m_ppcBestCU[0]->getMergeFlag(24)) ? 1 : 0; // 8x8
	PU_labels[11] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(28) && (UInt)m_ppcBestCU[0]->getMergeFlag(28)) ? 1 : 0; // 8x8
	PU_labels[12] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(32) && (UInt)m_ppcBestCU[0]->getMergeFlag(32)) ? 1 : 0; // 16x16
	PU_labels[13] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(32) && (UInt)m_ppcBestCU[0]->getMergeFlag(32)) ? 1 : 0; // 8x8
	PU_labels[14] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(36) && (UInt)m_ppcBestCU[0]->getMergeFlag(36)) ? 1 : 0; // 8x8
	PU_labels[15] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(40) && (UInt)m_ppcBestCU[0]->getMergeFlag(40)) ? 1 : 0; // 8x8
	PU_labels[16] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(44) && (UInt)m_ppcBestCU[0]->getMergeFlag(44)) ? 1 : 0; // 8x8
	PU_labels[17] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(48) && (UInt)m_ppcBestCU[0]->getMergeFlag(48)) ? 1 : 0; // 16x16
	PU_labels[18] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(48) && (UInt)m_ppcBestCU[0]->getMergeFlag(48)) ? 1 : 0; // 8x8
	PU_labels[19] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(52) && (UInt)m_ppcBestCU[0]->getMergeFlag(52)) ? 1 : 0; // 8x8
	PU_labels[20] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(56) && (UInt)m_ppcBestCU[0]->getMergeFlag(56)) ? 1 : 0; // 8x8
	PU_labels[21] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(60) && (UInt)m_ppcBestCU[0]->getMergeFlag(60)) ? 1 : 0; // 8x8
	PU_labels[22] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(64) && (UInt)m_ppcBestCU[0]->getMergeFlag(64)) ? 1 : 0; // 32x32
	PU_labels[23] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(64) && (UInt)m_ppcBestCU[0]->getMergeFlag(64)) ? 1 : 0; // 16x16
	PU_labels[24] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(64) && (UInt)m_ppcBestCU[0]->getMergeFlag(64)) ? 1 : 0; // 8x8
	PU_labels[25] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(68) && (UInt)m_ppcBestCU[0]->getMergeFlag(68)) ? 1 : 0; // 8x8
	PU_labels[26] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(72) && (UInt)m_ppcBestCU[0]->getMergeFlag(72)) ? 1 : 0; // 8x8
	PU_labels[27] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(76) && (UInt)m_ppcBestCU[0]->getMergeFlag(76)) ? 1 : 0; // 8x8
	PU_labels[28] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(80) && (UInt)m_ppcBestCU[0]->getMergeFlag(80)) ? 1 : 0; // 16x16
	PU_labels[29] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(80) && (UInt)m_ppcBestCU[0]->getMergeFlag(80)) ? 1 : 0; // 8x8
	PU_labels[30] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(84) && (UInt)m_ppcBestCU[0]->getMergeFlag(84)) ? 1 : 0; // 8x8
	PU_labels[31] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(88) && (UInt)m_ppcBestCU[0]->getMergeFlag(88)) ? 1 : 0; // 8x8
	PU_labels[32] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(92) && (UInt)m_ppcBestCU[0]->getMergeFlag(92)) ? 1 : 0; // 8x8
	PU_labels[33] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(96) && (UInt)m_ppcBestCU[0]->getMergeFlag(96)) ? 1 : 0; // 16x16
	PU_labels[34] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(96) && (UInt)m_ppcBestCU[0]->getMergeFlag(96)) ? 1 : 0; // 8x8
	PU_labels[35] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(100) && (UInt)m_ppcBestCU[0]->getMergeFlag(100)) ? 1 : 0; // 8x8
	PU_labels[36] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(104) && (UInt)m_ppcBestCU[0]->getMergeFlag(104)) ? 1 : 0; // 8x8
	PU_labels[37] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(108) && (UInt)m_ppcBestCU[0]->getMergeFlag(108)) ? 1 : 0; // 8x8
	PU_labels[38] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(112) && (UInt)m_ppcBestCU[0]->getMergeFlag(112)) ? 1 : 0; // 16x16
	PU_labels[39] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(112) && (UInt)m_ppcBestCU[0]->getMergeFlag(112)) ? 1 : 0; // 8x8
	PU_labels[40] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(116) && (UInt)m_ppcBestCU[0]->getMergeFlag(116)) ? 1 : 0; // 8x8
	PU_labels[41] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(120) && (UInt)m_ppcBestCU[0]->getMergeFlag(120)) ? 1 : 0; // 8x8
	PU_labels[42] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(124) && (UInt)m_ppcBestCU[0]->getMergeFlag(124)) ? 1 : 0; // 8x8
	PU_labels[43] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(128) && (UInt)m_ppcBestCU[0]->getMergeFlag(128)) ? 1 : 0; // 32x32
	PU_labels[44] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(128) && (UInt)m_ppcBestCU[0]->getMergeFlag(128)) ? 1 : 0; // 16x16
	PU_labels[45] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(128) && (UInt)m_ppcBestCU[0]->getMergeFlag(128)) ? 1 : 0; // 8x8
	PU_labels[46] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(132) && (UInt)m_ppcBestCU[0]->getMergeFlag(132)) ? 1 : 0; // 8x8
	PU_labels[47] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(136) && (UInt)m_ppcBestCU[0]->getMergeFlag(136)) ? 1 : 0; // 8x8
	PU_labels[48] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(140) && (UInt)m_ppcBestCU[0]->getMergeFlag(140)) ? 1 : 0; // 8x8
	PU_labels[49] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(144) && (UInt)m_ppcBestCU[0]->getMergeFlag(144)) ? 1 : 0; // 16x16
	PU_labels[50] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(144) && (UInt)m_ppcBestCU[0]->getMergeFlag(144)) ? 1 : 0; // 8x8
	PU_labels[51] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(148) && (UInt)m_ppcBestCU[0]->getMergeFlag(148)) ? 1 : 0; // 8x8
	PU_labels[52] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(152) && (UInt)m_ppcBestCU[0]->getMergeFlag(152)) ? 1 : 0; // 8x8
	PU_labels[53] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(156) && (UInt)m_ppcBestCU[0]->getMergeFlag(156)) ? 1 : 0; // 8x8
	PU_labels[54] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(160) && (UInt)m_ppcBestCU[0]->getMergeFlag(160)) ? 1 : 0; // 16x16
	PU_labels[55] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(160) && (UInt)m_ppcBestCU[0]->getMergeFlag(160)) ? 1 : 0; // 8x8
	PU_labels[56] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(164) && (UInt)m_ppcBestCU[0]->getMergeFlag(164)) ? 1 : 0; // 8x8
	PU_labels[57] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(168) && (UInt)m_ppcBestCU[0]->getMergeFlag(168)) ? 1 : 0; // 8x8
	PU_labels[58] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(172) && (UInt)m_ppcBestCU[0]->getMergeFlag(172)) ? 1 : 0; // 8x8
	PU_labels[59] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(176) && (UInt)m_ppcBestCU[0]->getMergeFlag(176)) ? 1 : 0; // 16x16
	PU_labels[60] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(176) && (UInt)m_ppcBestCU[0]->getMergeFlag(176)) ? 1 : 0; // 8x8
	PU_labels[61] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(180) && (UInt)m_ppcBestCU[0]->getMergeFlag(180)) ? 1 : 0; // 8x8
	PU_labels[62] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(184) && (UInt)m_ppcBestCU[0]->getMergeFlag(184)) ? 1 : 0; // 8x8
	PU_labels[63] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(188) && (UInt)m_ppcBestCU[0]->getMergeFlag(188)) ? 1 : 0; // 8x8
	PU_labels[64] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(192) && (UInt)m_ppcBestCU[0]->getMergeFlag(192)) ? 1 : 0; // 32x32
	PU_labels[65] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(192) && (UInt)m_ppcBestCU[0]->getMergeFlag(192)) ? 1 : 0; // 16x16
	PU_labels[66] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(192) && (UInt)m_ppcBestCU[0]->getMergeFlag(192)) ? 1 : 0; // 8x8
	PU_labels[67] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(196) && (UInt)m_ppcBestCU[0]->getMergeFlag(196)) ? 1 : 0; // 8x8
	PU_labels[68] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(200) && (UInt)m_ppcBestCU[0]->getMergeFlag(200)) ? 1 : 0; // 8x8
	PU_labels[69] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(204) && (UInt)m_ppcBestCU[0]->getMergeFlag(204)) ? 1 : 0; // 8x8
	PU_labels[70] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(208) && (UInt)m_ppcBestCU[0]->getMergeFlag(208)) ? 1 : 0; // 16x16
	PU_labels[71] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(208) && (UInt)m_ppcBestCU[0]->getMergeFlag(208)) ? 1 : 0; // 8x8
	PU_labels[72] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(212) && (UInt)m_ppcBestCU[0]->getMergeFlag(212)) ? 1 : 0; // 8x8
	PU_labels[73] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(216) && (UInt)m_ppcBestCU[0]->getMergeFlag(216)) ? 1 : 0; // 8x8
	PU_labels[74] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(220) && (UInt)m_ppcBestCU[0]->getMergeFlag(220)) ? 1 : 0; // 8x8
	PU_labels[75] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(224) && (UInt)m_ppcBestCU[0]->getMergeFlag(224)) ? 1 : 0; // 16x16
	PU_labels[76] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(224) && (UInt)m_ppcBestCU[0]->getMergeFlag(224)) ? 1 : 0; // 8x8
	PU_labels[77] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(228) && (UInt)m_ppcBestCU[0]->getMergeFlag(228)) ? 1 : 0; // 8x8
	PU_labels[78] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(232) && (UInt)m_ppcBestCU[0]->getMergeFlag(232)) ? 1 : 0; // 8x8
	PU_labels[79] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(236) && (UInt)m_ppcBestCU[0]->getMergeFlag(236)) ? 1 : 0; // 8x8
	PU_labels[80] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(240) && (UInt)m_ppcBestCU[0]->getMergeFlag(240)) ? 1 : 0; // 16x16
	PU_labels[81] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(240) && (UInt)m_ppcBestCU[0]->getMergeFlag(240)) ? 1 : 0; // 8x8
	PU_labels[82] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(244) && (UInt)m_ppcBestCU[0]->getMergeFlag(244)) ? 1 : 0; // 8x8
	PU_labels[83] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(248) && (UInt)m_ppcBestCU[0]->getMergeFlag(248)) ? 1 : 0; // 8x8
	PU_labels[84] = (!(UInt)m_ppcBestCU[0]->getPartitionSize(252) && (UInt)m_ppcBestCU[0]->getMergeFlag(252)) ? 1 : 0; // 8x8

	  //// 增加模式选择信息
	  //PU_labels[0] = (UInt)m_ppcBestCU[0]->getPredictionMode(0) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(0); // 64x64
	  //PU_labels[1] = (UInt)m_ppcBestCU[0]->getPredictionMode(0) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(0); // 32x32
	  //PU_labels[2] = (UInt)m_ppcBestCU[0]->getPredictionMode(0) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(0); // 16x16
   // PU_labels[3] = (UInt)m_ppcBestCU[0]->getPredictionMode(0) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(0); // 8x8
   // PU_labels[4] = (UInt)m_ppcBestCU[0]->getPredictionMode(4) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(4); // 8x8
   // PU_labels[5] = (UInt)m_ppcBestCU[0]->getPredictionMode(8) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(8); // 8x8
   // PU_labels[6] = (UInt)m_ppcBestCU[0]->getPredictionMode(12) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(12); // 8x8
	  //PU_labels[7] = (UInt)m_ppcBestCU[0]->getPredictionMode(16) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(16); // 16x16
   // PU_labels[8] = (UInt)m_ppcBestCU[0]->getPredictionMode(16) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(16); // 8x8
   // PU_labels[9] = (UInt)m_ppcBestCU[0]->getPredictionMode(20) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(20); // 8x8
   // PU_labels[10] = (UInt)m_ppcBestCU[0]->getPredictionMode(24) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(24); // 8x8
   // PU_labels[11] = (UInt)m_ppcBestCU[0]->getPredictionMode(28) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(28); // 8x8
	  //PU_labels[12] = (UInt)m_ppcBestCU[0]->getPredictionMode(32) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(32); // 16x16
   // PU_labels[13] = (UInt)m_ppcBestCU[0]->getPredictionMode(32) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(32); // 8x8
   // PU_labels[14] = (UInt)m_ppcBestCU[0]->getPredictionMode(36) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(36); // 8x8
   // PU_labels[15] = (UInt)m_ppcBestCU[0]->getPredictionMode(40) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(40); // 8x8
   // PU_labels[16] = (UInt)m_ppcBestCU[0]->getPredictionMode(44) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(44); // 8x8
	  //PU_labels[17] = (UInt)m_ppcBestCU[0]->getPredictionMode(48) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(48); // 16x16
   // PU_labels[18] = (UInt)m_ppcBestCU[0]->getPredictionMode(48) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(48); // 8x8
   // PU_labels[19] = (UInt)m_ppcBestCU[0]->getPredictionMode(52) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(52); // 8x8
   // PU_labels[20] = (UInt)m_ppcBestCU[0]->getPredictionMode(56) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(56); // 8x8
   // PU_labels[21] = (UInt)m_ppcBestCU[0]->getPredictionMode(60) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(60); // 8x8
	  //PU_labels[22] = (UInt)m_ppcBestCU[0]->getPredictionMode(64) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(64); // 32x32
	  //PU_labels[23] = (UInt)m_ppcBestCU[0]->getPredictionMode(64) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(64); // 16x16
   // PU_labels[24] = (UInt)m_ppcBestCU[0]->getPredictionMode(64) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(64); // 8x8
   // PU_labels[25] = (UInt)m_ppcBestCU[0]->getPredictionMode(68) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(68); // 8x8
   // PU_labels[26] = (UInt)m_ppcBestCU[0]->getPredictionMode(72) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(72); // 8x8
   // PU_labels[27] = (UInt)m_ppcBestCU[0]->getPredictionMode(76) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(76); // 8x8
	  //PU_labels[28] = (UInt)m_ppcBestCU[0]->getPredictionMode(80) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(80); // 16x16
   // PU_labels[29] = (UInt)m_ppcBestCU[0]->getPredictionMode(80) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(80); // 8x8
   // PU_labels[30] = (UInt)m_ppcBestCU[0]->getPredictionMode(84) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(84); // 8x8
   // PU_labels[31] = (UInt)m_ppcBestCU[0]->getPredictionMode(88) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(88); // 8x8
   // PU_labels[32] = (UInt)m_ppcBestCU[0]->getPredictionMode(92) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(92); // 8x8
	  //PU_labels[33] = (UInt)m_ppcBestCU[0]->getPredictionMode(96) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(96); // 16x16
   // PU_labels[34] = (UInt)m_ppcBestCU[0]->getPredictionMode(96) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(96); // 8x8
   // PU_labels[35] = (UInt)m_ppcBestCU[0]->getPredictionMode(100) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(100); // 8x8
   // PU_labels[36] = (UInt)m_ppcBestCU[0]->getPredictionMode(104) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(104); // 8x8
   // PU_labels[37] = (UInt)m_ppcBestCU[0]->getPredictionMode(108) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(108); // 8x8
	  //PU_labels[38] = (UInt)m_ppcBestCU[0]->getPredictionMode(112) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(112); // 16x16
   // PU_labels[39] = (UInt)m_ppcBestCU[0]->getPredictionMode(112) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(112); // 8x8
   // PU_labels[40] = (UInt)m_ppcBestCU[0]->getPredictionMode(116) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(116); // 8x8
   // PU_labels[41] = (UInt)m_ppcBestCU[0]->getPredictionMode(120) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(120); // 8x8
   // PU_labels[42] = (UInt)m_ppcBestCU[0]->getPredictionMode(124) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(124); // 8x8
	  //PU_labels[43] = (UInt)m_ppcBestCU[0]->getPredictionMode(128) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(128); // 32x32
	  //PU_labels[44] = (UInt)m_ppcBestCU[0]->getPredictionMode(128) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(128); // 16x16
   // PU_labels[45] = (UInt)m_ppcBestCU[0]->getPredictionMode(128) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(128); // 8x8
   // PU_labels[46] = (UInt)m_ppcBestCU[0]->getPredictionMode(132) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(132); // 8x8
   // PU_labels[47] = (UInt)m_ppcBestCU[0]->getPredictionMode(136) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(136); // 8x8
   // PU_labels[48] = (UInt)m_ppcBestCU[0]->getPredictionMode(140) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(140); // 8x8
	  //PU_labels[49] = (UInt)m_ppcBestCU[0]->getPredictionMode(144) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(144); // 16x16
   // PU_labels[50] = (UInt)m_ppcBestCU[0]->getPredictionMode(144) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(144); // 8x8
   // PU_labels[51] = (UInt)m_ppcBestCU[0]->getPredictionMode(148) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(148); // 8x8
   // PU_labels[52] = (UInt)m_ppcBestCU[0]->getPredictionMode(152) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(152); // 8x8
   // PU_labels[53] = (UInt)m_ppcBestCU[0]->getPredictionMode(156) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(156); // 8x8
	  //PU_labels[54] = (UInt)m_ppcBestCU[0]->getPredictionMode(160) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(160); // 16x16
   // PU_labels[55] = (UInt)m_ppcBestCU[0]->getPredictionMode(160) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(160); // 8x8
   // PU_labels[56] = (UInt)m_ppcBestCU[0]->getPredictionMode(164) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(164); // 8x8
   // PU_labels[57] = (UInt)m_ppcBestCU[0]->getPredictionMode(168) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(168); // 8x8
   // PU_labels[58] = (UInt)m_ppcBestCU[0]->getPredictionMode(172) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(172); // 8x8
	  //PU_labels[59] = (UInt)m_ppcBestCU[0]->getPredictionMode(176) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(176); // 16x16
   // PU_labels[60] = (UInt)m_ppcBestCU[0]->getPredictionMode(176) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(176); // 8x8
   // PU_labels[61] = (UInt)m_ppcBestCU[0]->getPredictionMode(180) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(180); // 8x8
   // PU_labels[62] = (UInt)m_ppcBestCU[0]->getPredictionMode(184) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(184); // 8x8
   // PU_labels[63] = (UInt)m_ppcBestCU[0]->getPredictionMode(188) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(188); // 8x8
	  //PU_labels[64] = (UInt)m_ppcBestCU[0]->getPredictionMode(192) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(192); // 32x32
	  //PU_labels[65] = (UInt)m_ppcBestCU[0]->getPredictionMode(192) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(192); // 16x16
   // PU_labels[66] = (UInt)m_ppcBestCU[0]->getPredictionMode(192) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(192); // 8x8
   // PU_labels[67] = (UInt)m_ppcBestCU[0]->getPredictionMode(196) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(196); // 8x8
   // PU_labels[68] = (UInt)m_ppcBestCU[0]->getPredictionMode(200) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(200); // 8x8
   // PU_labels[69] = (UInt)m_ppcBestCU[0]->getPredictionMode(204) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(204); // 8x8
	  //PU_labels[70] = (UInt)m_ppcBestCU[0]->getPredictionMode(208) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(208); // 16x16
   // PU_labels[71] = (UInt)m_ppcBestCU[0]->getPredictionMode(208) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(208); // 8x8
   // PU_labels[72] = (UInt)m_ppcBestCU[0]->getPredictionMode(212) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(212); // 8x8
   // PU_labels[73] = (UInt)m_ppcBestCU[0]->getPredictionMode(216) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(216); // 8x8
   // PU_labels[74] = (UInt)m_ppcBestCU[0]->getPredictionMode(220) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(220); // 8x8
	  //PU_labels[75] = (UInt)m_ppcBestCU[0]->getPredictionMode(224) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(224); // 16x16
   // PU_labels[76] = (UInt)m_ppcBestCU[0]->getPredictionMode(224) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(224); // 8x8
   // PU_labels[77] = (UInt)m_ppcBestCU[0]->getPredictionMode(228) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(228); // 8x8
   // PU_labels[78] = (UInt)m_ppcBestCU[0]->getPredictionMode(232) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(232); // 8x8
   // PU_labels[79] = (UInt)m_ppcBestCU[0]->getPredictionMode(236) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(236); // 8x8
	  //PU_labels[80] = (UInt)m_ppcBestCU[0]->getPredictionMode(240) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(240); // 16x16
   // PU_labels[81] = (UInt)m_ppcBestCU[0]->getPredictionMode(240) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(240); // 8x8
   // PU_labels[82] = (UInt)m_ppcBestCU[0]->getPredictionMode(244) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(244); // 8x8
   // PU_labels[83] = (UInt)m_ppcBestCU[0]->getPredictionMode(248) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(248); // 8x8
   // PU_labels[84] = (UInt)m_ppcBestCU[0]->getPredictionMode(252) * 9 + (UInt)m_ppcBestCU[0]->getPartitionSize(252); // 8x8


    Bool rapPic = (m_ppcBestCU[0]->getSlice()->getNalUnitType() == NAL_UNIT_CODED_SLICE_IDR_W_RADL || m_ppcBestCU[0]->getSlice()->getNalUnitType() == NAL_UNIT_CODED_SLICE_IDR_N_LP || m_ppcBestCU[0]->getSlice()->getNalUnitType() == NAL_UNIT_CODED_SLICE_CRA);

    // 除去每一帧的第一个CTU
	int fileNum = (int)(pCtu->getSlice()->getPOC() / 24);
    ofstream fLabel0, fLabel1, fLabel2, fLabel3;
    //fLabel.open(("Label_ViewId_" + to_string(pCtu->getSlice()->getViewId()) + "_" + to_string((int)time_tag_filename_suffix) + ".txt"), ios::app);
    fLabel0.open(("flieNum_" + to_string(fileNum) + "_Label_ViewId_" + to_string(pCtu->getSlice()->getViewId()) + "_Size_64_" + to_string((int)time_tag_filename_suffix) + ".txt"), ios::app);
    fLabel1.open(("flieNum_" + to_string(fileNum) + "_Label_ViewId_" + to_string(pCtu->getSlice()->getViewId()) + "_Size_32_" + to_string((int)time_tag_filename_suffix) + ".txt"), ios::app);
    fLabel2.open(("flieNum_" + to_string(fileNum) + "_Label_ViewId_" + to_string(pCtu->getSlice()->getViewId()) + "_Size_16_" + to_string((int)time_tag_filename_suffix) + ".txt"), ios::app);
    fLabel3.open(("flieNum_" + to_string(fileNum) + "_Label_ViewId_" + to_string(pCtu->getSlice()->getViewId()) + "_Size_8_" + to_string((int)time_tag_filename_suffix) + ".txt"), ios::app);
    
    for (UInt i = 0; i < 85; i++)
    {
		if (i == 0)
		{
			fLabel0 << labels[i] << " " << PU_labels[i] << endl;
		}
		else if (i % 21 == 1)
		{
			fLabel1 << labels[i] << " " << PU_labels[i] << endl;
		}
		else if (i == 2 || i == 7 || i == 12 || i == 17 ||
			i == 23 || i == 28 || i == 33 || i == 38 ||
			i == 44 || i == 49 || i == 54 || i == 59 ||
			i == 65 || i == 70 || i == 75 || i == 80)
		{
			fLabel2 << labels[i] << " " << PU_labels[i] << endl;
		}
		else
		{
			fLabel3 << labels[i] << " " << PU_labels[i] << endl;
		}
		//fLabel << labels[i] << " " << PU_labels[i] << " " << pCtu->getSlice()->getPOC() << " " << (UInt)pCtu->getSlice()->getSliceType() << " " << (UInt)rapPic << endl;
		labels[i] = -1;
		PU_labels[i] = -1;
    }
    fLabel0.close();
    fLabel1.close();
    fLabel2.close();
    fLabel3.close();

  }
#endif


#if ADAPTIVE_QP_SELECTION
  if( m_pcEncCfg->getUseAdaptQpSelect() )
  {
    if(pCtu->getSlice()->getSliceType()!=I_SLICE) //IIII
    {
      xCtuCollectARLStats( pCtu );
    }
  }
#endif
}
/** \param  pCtu  pointer of CU data class
 */
Void TEncCu::encodeCtu ( TComDataCU* pCtu )
{
  if ( pCtu->getSlice()->getPPS()->getUseDQP() )
  {
    setdQPFlag(true);
  }

  if ( pCtu->getSlice()->getUseChromaQpAdj() )
  {
    setCodeChromaQpAdjFlag(true);
  }

  // Encode CU data
  xEncodeCU( pCtu, 0, 0 );
}

// ====================================================================================================================
// Protected member functions
// ====================================================================================================================
//! Derive small set of test modes for AMP encoder speed-up
#if AMP_ENC_SPEEDUP
#if AMP_MRG
Void TEncCu::deriveTestModeAMP (TComDataCU *pcBestCU, PartSize eParentPartSize, Bool &bTestAMP_Hor, Bool &bTestAMP_Ver, Bool &bTestMergeAMP_Hor, Bool &bTestMergeAMP_Ver)
#else
Void TEncCu::deriveTestModeAMP (TComDataCU *pcBestCU, PartSize eParentPartSize, Bool &bTestAMP_Hor, Bool &bTestAMP_Ver)
#endif
{
  if ( pcBestCU->getPartitionSize(0) == SIZE_2NxN )
  {
    bTestAMP_Hor = true;
  }
  else if ( pcBestCU->getPartitionSize(0) == SIZE_Nx2N )
  {
    bTestAMP_Ver = true;
  }
  else if ( pcBestCU->getPartitionSize(0) == SIZE_2Nx2N && pcBestCU->getMergeFlag(0) == false && pcBestCU->isSkipped(0) == false )
  {
    bTestAMP_Hor = true;
    bTestAMP_Ver = true;
  }

#if AMP_MRG
  //! Utilizing the partition size of parent PU
  if ( eParentPartSize >= SIZE_2NxnU && eParentPartSize <= SIZE_nRx2N )
  {
    bTestMergeAMP_Hor = true;
    bTestMergeAMP_Ver = true;
  }

  if ( eParentPartSize == NUMBER_OF_PART_SIZES ) //! if parent is intra
  {
    if ( pcBestCU->getPartitionSize(0) == SIZE_2NxN )
    {
      bTestMergeAMP_Hor = true;
    }
    else if ( pcBestCU->getPartitionSize(0) == SIZE_Nx2N )
    {
      bTestMergeAMP_Ver = true;
    }
  }

  if ( pcBestCU->getPartitionSize(0) == SIZE_2Nx2N && pcBestCU->isSkipped(0) == false )
  {
    bTestMergeAMP_Hor = true;
    bTestMergeAMP_Ver = true;
  }

  if ( pcBestCU->getWidth(0) == 64 )
  {
    bTestAMP_Hor = false;
    bTestAMP_Ver = false;
  }
#else
  //! Utilizing the partition size of parent PU
  if ( eParentPartSize >= SIZE_2NxnU && eParentPartSize <= SIZE_nRx2N )
  {
    bTestAMP_Hor = true;
    bTestAMP_Ver = true;
  }

  if ( eParentPartSize == SIZE_2Nx2N )
  {
    bTestAMP_Hor = false;
    bTestAMP_Ver = false;
  }
#endif
}
#endif


// ====================================================================================================================
// Protected member functions
// ====================================================================================================================
/** Compress a CU block recursively with enabling sub-CTU-level delta QP
 *  - for loop of QP value to compress the current CU with all possible QP
*/
#if AMP_ENC_SPEEDUP
Void TEncCu::xCompressCU( TComDataCU*& rpcBestCU, TComDataCU*& rpcTempCU, const UInt uiDepth DEBUG_STRING_FN_DECLARE(sDebug_), PartSize eParentPartSize )
#else
Void TEncCu::xCompressCU( TComDataCU*& rpcBestCU, TComDataCU*& rpcTempCU, const UInt uiDepth )
#endif
{
//#if PRINT_FEATURE
//	myMSMFlag = false;
//#endif

  TComPic* pcPic = rpcBestCU->getPic();
  DEBUG_STRING_NEW(sDebug)
  const TComPPS &pps=*(rpcTempCU->getSlice()->getPPS());
  const TComSPS &sps=*(rpcTempCU->getSlice()->getSPS());
  
  // These are only used if getFastDeltaQp() is true
  const UInt fastDeltaQPCuMaxSize    = Clip3(sps.getMaxCUHeight()>>sps.getLog2DiffMaxMinCodingBlockSize(), sps.getMaxCUHeight(), 32u);

#if NH_3D_QTL
#if NH_3D_QTLPC
  Bool  bLimQtPredFalg    = pcPic->getSlice(0)->getQtPredFlag(); 
#else
  Bool  bLimQtPredFalg    = false;
#endif
  TComPic *pcTexture      = rpcBestCU->getSlice()->getTexturePic();

  Bool  depthMapDetect    = (pcTexture != NULL);
  Bool  bIntraSliceDetect = (rpcBestCU->getSlice()->getSliceType() == I_SLICE);

  Bool rapPic             = (rpcBestCU->getSlice()->getNalUnitType() == NAL_UNIT_CODED_SLICE_IDR_W_RADL || rpcBestCU->getSlice()->getNalUnitType() == NAL_UNIT_CODED_SLICE_IDR_N_LP || rpcBestCU->getSlice()->getNalUnitType() == NAL_UNIT_CODED_SLICE_CRA);

  Bool bTry2NxN           = true;
  Bool bTryNx2N           = true;
#endif

  // get Original YUV data from picture
  m_ppcOrigYuv[uiDepth]->copyFromPicYuv( pcPic->getPicYuvOrg(), rpcBestCU->getCtuRsAddr(), rpcBestCU->getZorderIdxInCtu() );

#if NH_3D_QTL  
  Bool    bTrySplit     = true;
  Bool    bTrySplitDQP  = true;
#endif
  // variable for Cbf fast mode PU decision
  Bool    doNotBlockPu = true;
  Bool    earlyDetectionSkipMode = false;

#if NH_3D_NBDV
  DisInfo DvInfo; 
  DvInfo.m_acNBDV.setZero();
  DvInfo.m_aVIdxCan = 0;
#if NH_3D_NBDV_REF
  DvInfo.m_acDoNBDV.setZero();
#endif
#endif
  const UInt uiLPelX   = rpcBestCU->getCUPelX();
  const UInt uiRPelX   = uiLPelX + rpcBestCU->getWidth(0)  - 1;
  const UInt uiTPelY   = rpcBestCU->getCUPelY();
  const UInt uiBPelY   = uiTPelY + rpcBestCU->getHeight(0) - 1;
  const UInt uiWidth   = rpcBestCU->getWidth(0);

#if NH_MV_ENC_DEC_TRAC
#if ENC_DEC_TRACE
    stopAtPos  ( rpcBestCU->getSlice()->getPOC(), 
                 rpcBestCU->getSlice()->getLayerId(), 
                 rpcBestCU->getCUPelX(),
                 rpcBestCU->getCUPelY(),
                 rpcBestCU->getWidth(0), 
                 rpcBestCU->getHeight(0) );
#endif
#endif

  Int iBaseQP = xComputeQP( rpcBestCU, uiDepth );
  Int iMinQP;
  Int iMaxQP;
  Bool isAddLowestQP = false;

  const UInt numberValidComponents = rpcBestCU->getPic()->getNumberValidComponents();

  if( uiDepth <= pps.getMaxCuDQPDepth() )
  {
    Int idQP = m_pcEncCfg->getMaxDeltaQP();
    iMinQP = Clip3( -sps.getQpBDOffset(CHANNEL_TYPE_LUMA), MAX_QP, iBaseQP-idQP );
    iMaxQP = Clip3( -sps.getQpBDOffset(CHANNEL_TYPE_LUMA), MAX_QP, iBaseQP+idQP );
  }
  else
  {
    iMinQP = rpcTempCU->getQP(0);
    iMaxQP = rpcTempCU->getQP(0);
  }

  if ( m_pcEncCfg->getUseRateCtrl() )
  {
    iMinQP = m_pcRateCtrl->getRCQP();
    iMaxQP = m_pcRateCtrl->getRCQP();
  }

  // transquant-bypass (TQB) processing loop variable initialisation ---

  const Int lowestQP = iMinQP; // For TQB, use this QP which is the lowest non TQB QP tested (rather than QP'=0) - that way delta QPs are smaller, and TQB can be tested at all CU levels.

  if ( (pps.getTransquantBypassEnableFlag()) )
  {
    isAddLowestQP = true; // mark that the first iteration is to cost TQB mode.
    iMinQP = iMinQP - 1;  // increase loop variable range by 1, to allow testing of TQB mode along with other QPs
    if ( m_pcEncCfg->getCUTransquantBypassFlagForceValue() )
    {
      iMaxQP = iMinQP;
    }
  }

#if NH_3D_IC
  Bool bICEnabled = rpcTempCU->getSlice()->getViewIndex() && ( rpcTempCU->getSlice()->getSliceType() == P_SLICE || rpcTempCU->getSlice()->getSliceType() == B_SLICE ) && !rpcTempCU->getSlice()->getIsDepth();
  bICEnabled = bICEnabled && rpcTempCU->getSlice()->getApplyIC();
#endif

  TComSlice * pcSlice = rpcTempCU->getPic()->getSlice(rpcTempCU->getPic()->getCurrSliceIdx());

  const Bool bBoundary = !( uiRPelX < sps.getPicWidthInLumaSamples() && uiBPelY < sps.getPicHeightInLumaSamples() );
#if  NH_3D_FAST_TEXTURE_ENCODING
    Bool bIVFMerge = false;
    Int  iIVFMaxD = 0;
    Bool bFMD = false;
    Bool bSubBranch = true;
#endif
  if ( !bBoundary )
  {
    for (Int iQP=iMinQP; iQP<=iMaxQP; iQP++)
    {
      const Bool bIsLosslessMode = isAddLowestQP && (iQP == iMinQP);

      if (bIsLosslessMode)
      {
        iQP = lowestQP;
      }

#if NH_3D_QTL
      bTrySplit    = true;
#endif

      m_cuChromaQpOffsetIdxPlus1 = 0;
      if (pcSlice->getUseChromaQpAdj())
      {
        /* Pre-estimation of chroma QP based on input block activity may be performed
         * here, using for example m_ppcOrigYuv[uiDepth] */
        /* To exercise the current code, the index used for adjustment is based on
         * block position
         */
        Int lgMinCuSize = sps.getLog2MinCodingBlockSize() +
                          std::max<Int>(0, sps.getLog2DiffMaxMinCodingBlockSize()-Int(pps.getPpsRangeExtension().getDiffCuChromaQpOffsetDepth()));
        m_cuChromaQpOffsetIdxPlus1 = ((uiLPelX >> lgMinCuSize) + (uiTPelY >> lgMinCuSize)) % (pps.getPpsRangeExtension().getChromaQpOffsetListLen() + 1);
      }

      rpcTempCU->initEstData( uiDepth, iQP, bIsLosslessMode );
#if NH_3D_QTL
      //logic for setting bTrySplit using the partition information that is stored of the texture colocated CU
#if H_3D_FCO
      if(depthMapDetect && !bIntraSliceDetect && !rapPic && ( m_pcEncCfg->getUseQTL() || bLimQtPredFalg ) && pcTexture->getReconMark())
#else
      if(depthMapDetect && !bIntraSliceDetect && !rapPic && ( m_pcEncCfg->getUseQTL() || bLimQtPredFalg ))
#endif
      {
        TComDataCU* pcTextureCU = pcTexture->getCtu( rpcBestCU->getCtuRsAddr() ); //Corresponding texture LCU
        UInt uiCUIdx            = rpcBestCU->getZorderIdxInCtu();
        assert(pcTextureCU->getDepth(uiCUIdx) >= uiDepth); //Depth cannot be more partitioned than the texture.
        if (pcTextureCU->getDepth(uiCUIdx) > uiDepth || pcTextureCU->getPartitionSize(uiCUIdx) == SIZE_NxN) //Texture was split.
        {
          bTrySplit = true;
          bTryNx2N  = true;
          bTry2NxN  = true;
        }
        else
        {
          bTrySplit = false;
          bTryNx2N  = false;
          bTry2NxN  = false;
          if( pcTextureCU->getDepth(uiCUIdx) == uiDepth && pcTextureCU->getPartitionSize(uiCUIdx) != SIZE_2Nx2N)
          {
            if(pcTextureCU->getPartitionSize(uiCUIdx)==SIZE_2NxN || pcTextureCU->getPartitionSize(uiCUIdx)==SIZE_2NxnU|| pcTextureCU->getPartitionSize(uiCUIdx)==SIZE_2NxnD)
            {
              bTry2NxN  = true;
            }
            else
            {
              bTryNx2N  = true;
            }
          }
        }
      }
#endif

#if NH_3D_NBDV
      if( rpcTempCU->getSlice()->getSliceType() != I_SLICE )
      {
#if NH_3D_ARP && NH_3D_IV_MERGE && NH_3D_VSP
        if( rpcTempCU->getSlice()->getIvResPredFlag() || rpcTempCU->getSlice()->getIvMvPredFlag() || rpcTempCU->getSlice()->getViewSynthesisPredFlag() )
#else 
#if NH_3D_IV_MERGE && NH_3D_VSP
        if( rpcTempCU->getSlice()->getIvMvPredFlag() || rpcTempCU->getSlice()->getViewSynthesisPredFlag() )
#else
#if NH_3D_ARP && NH_3D_VSP
        if( rpcTempCU->getSlice()->getIvResPredFlag() || rpcTempCU->getSlice()->getViewSynthesisPredFlag() )
#else
#if NH_3D_VSP
        if( rpcTempCU->getSlice()->getViewSynthesisPredFlag() )
#else
#if NH_3D_ARP
        if( rpcTempCU->getSlice()->getIvResPredFlag() )
#else
#if H_3D_IV_MERGE
        if( rpcTempCU->getSlice()->getVPS()->getIvMvPredFlag(rpcTempCU->getSlice()->getLayerId()) )
#else
#if NH_3D_DBBP
        if( rpcTempCU->getSlice()->getDepthBasedBlkPartFlag() )
#else
        if (0)
#endif
#endif
#endif
#endif
#endif
#endif
#endif
        {
          PartSize ePartTemp = rpcTempCU->getPartitionSize(0);
          rpcTempCU->setPartSizeSubParts(SIZE_2Nx2N, 0, uiDepth);
#if NH_3D_IV_MERGE
          if (rpcTempCU->getSlice()->getIsDepth() )
          {
            rpcTempCU->getDispforDepth(0, 0, &DvInfo);
          }
          else
          {
#endif 
#if NH_3D_NBDV_REF
            if( rpcTempCU->getSlice()->getDepthRefinementFlag() )
            {
              rpcTempCU->getDisMvpCandNBDV(&DvInfo, true);
            }
            else
#endif 
            {
              rpcTempCU->getDisMvpCandNBDV(&DvInfo);
            }
#if NH_3D_IV_MERGE
          }
#endif
          rpcTempCU->setDvInfoSubParts(DvInfo, 0, uiDepth);
          rpcBestCU->setDvInfoSubParts(DvInfo, 0, uiDepth);
          rpcTempCU->setPartSizeSubParts( ePartTemp, 0, uiDepth );
        }
      }

	  // 尝试获取视差对应块

#if  NH_3D_FAST_TEXTURE_ENCODING
      if(rpcTempCU->getSlice()->getViewIndex() && !rpcTempCU->getSlice()->getIsDepth() && rpcTempCU->getSlice()->getDefaultRefViewIdxAvailableFlag() )
      {
        PartSize ePartTemp = rpcTempCU->getPartitionSize(0);
        rpcTempCU->setPartSizeSubParts( SIZE_2Nx2N, 0, uiDepth ); 
        rpcTempCU->getIVNStatus( 0, &DvInfo,  bIVFMerge, iIVFMaxD);
        rpcTempCU->setPartSizeSubParts( ePartTemp, 0, uiDepth );
      }
#endif
#endif
      // do inter modes, SKIP and 2Nx2N
      if( rpcBestCU->getSlice()->getSliceType() != I_SLICE )
      {
#if NH_3D_IC
        for( UInt uiICId = 0; uiICId < ( bICEnabled ? 2 : 1 ); uiICId++ )
        {
          Bool bICFlag = uiICId ? true : false;
#endif
        // 2Nx2N
        if(m_pcEncCfg->getUseEarlySkipDetection())
        {
#if NH_3D_IC
            rpcTempCU->setICFlagSubParts(bICFlag, 0, 0, uiDepth);
#endif
#if  NH_3D_FAST_TEXTURE_ENCODING
          xCheckRDCostInter( rpcBestCU, rpcTempCU, SIZE_2Nx2N DEBUG_STRING_PASS_INTO(sDebug), bFMD );  rpcTempCU->initEstData( uiDepth, iQP, bIsLosslessMode  );//by Competition for inter_2Nx2N
#else
          xCheckRDCostInter( rpcBestCU, rpcTempCU, SIZE_2Nx2N DEBUG_STRING_PASS_INTO(sDebug) );
          rpcTempCU->initEstData( uiDepth, iQP, bIsLosslessMode );//by Competition for inter_2Nx2N
#endif
#if NH_3D_VSP  || NH_3D_DBBP
          rpcTempCU->setDvInfoSubParts(DvInfo, 0, uiDepth);
#endif
        }
        // SKIP
#if NH_3D_IC
          rpcTempCU->setICFlagSubParts(bICFlag, 0, 0, uiDepth);
#endif
        xCheckRDCostMerge2Nx2N( rpcBestCU, rpcTempCU DEBUG_STRING_PASS_INTO(sDebug), &earlyDetectionSkipMode );//by Merge for inter_2Nx2N

#if PRINT_FEATURE
		
		//if (rpcBestCU->getSlice()->getSliceType() == B_SLICE && uiDepth < 4 && rpcBestCU->getSlice()->getIsDepth() && myFlag[0] && myFlag[1])
		if (rpcBestCU->getSlice()->getSliceType() != I_SLICE && uiDepth < 4 && rpcBestCU->getSlice()->getIsDepth() && myFlag[0] && myFlag[1])
		{

#if PRINT_TIME
      timeTag = clock();
#endif

	  // ==================================================== feature extraction begin ! =========================================

	  // 时域相关性
	  deltaPOC_t0 = -1.0;
	  SG_t0 = -1.0;
	  Dt_t0 = -1.0;
	  Dt_cc_t0 = -1.0;
	  Pt_t0 = -1.0;
	  Pt_cc_t0 = -1.0;
	  MergeFlag_cc_t0 = -1.0;
	  SkipFlag_cc_t0 = -1.0;
	  SDCFlag_cc_t0 = -1.0;
	  DISFlag_cc_t0 = -1.0;
	  MSMFlag_cc_t0 = -1.0;
	  deltaPOC_t1 = -1.0;
	  SG_t1 = -1.0;
	  Dt_t1 = -1.0;
	  Dt_cc_t1 = -1.0;
	  Pt_t1 = -1.0;
	  Pt_cc_t1 = -1.0;
	  MergeFlag_cc_t1 = -1.0;
	  SkipFlag_cc_t1 = -1.0;
	  SDCFlag_cc_t1 = -1.0;
	  DISFlag_cc_t1 = -1.0;
	  MSMFlag_cc_t1 = -1.0;

	  // 视点间相关性
	  VSG = -1.0;
	  Dv = -1.0;
	  Dv_cc = -1.0;
	  Pv = -1.0;
	  Pv_cc = -1.0;
	  MergeFlag_cc_v = -1.0;
	  SkipFlag_cc_v = -1.0;
	  SDCFlag_cc_v = -1.0;
	  DISFlag_cc_v = -1.0;
	  MSMFlag_cc_v = -1.0;

	  // 组件间相关性(尝试同位块，或者crisscross)
	  Dc = -1.0;
	  Dc_cc = -1.0;
	  Pc = -1.0;
	  Pc_cc = -1.0;
	  MergeFlag_c = -1.0;
	  SkipFlag_c = -1.0;
	  SDCFlag_c = -1.0;
	  DISFlag_c = -1.0;
	  MSMFlag_c = -1.0;
	  MergeFlag_cc_c = -1.0;
	  SkipFlag_cc_c = -1.0;
	  SDCFlag_cc_c = -1.0;
	  DISFlag_cc_c = -1.0;
	  MSMFlag_cc_c = -1.0;
	  Hc = -1.0;

	  // 空域相关性
	  Ds_GAMMA = -1.0;
	  Ds4mean = -1.0;
	  Ps4mean = -1.0;
	  MergeFlag_s = -1.0;
	  SkipFlag_s = -1.0;
	  SDCFlag_s = -1.0;
	  DISFlag_s = -1.0;
	  MSMFlag_s = -1.0;

	  // 当前块与子块
	  curMean = -1.0;
	  curVar = -1.0;
	  Hcur = -1.0;
	  curSubmvar = -1.0;
	  Ratio_curSubmvar = -1.0;
	  curSubdmean = -1.0;
	  Norm_curSubdmean = -1.0;
	  curSubdvar = -1.0;
	  Norm_curSubdvar = -1.0;

	  // 当前块已编码信息
	  QP = -1.0;
	  RD_MSM = -1.0;
	  SkipFlag = -1.0;
	  SDCFlag = -1.0;
	  TotalBins = -1.0;
	  TotalBits = -1.0;
	  TotalDistortion = -1.0;
	  POC = -1.0;
	  isB_SLICE = -1.0;

	  // ============================== InterFrame InterComponent InterView correlation (T2CV) ======================================
	  TComPic * T0 = rpcBestCU->getSlice()->getRefPic(RefPicList(0), 0);
	  TComPic * T1 = NULL;
	  if (rpcBestCU->getSlice()->getSliceType() == B_SLICE)
	  {
		  T1 = rpcBestCU->getSlice()->getRefPic(RefPicList(1), 0);
	  }
	  else
	  {
		  T1 = rpcBestCU->getSlice()->getRefPic(RefPicList(0), 0);
	  }

	  TComPic * TexturePic = pcTexture;
	  TComPic * BvPic = NULL;
	  bool DependentView = rpcBestCU->getSlice()->getViewId() != 1;
	  if (DependentView)
	  {
		  BvPic = rpcBestCU->getSlice()->getIvPic(1, 0);
	  }

	  //TComDataCU *colCTU_T0 = T0->getCtu(rpcBestCU->getCtuRsAddr);
	  //TComDataCU *colCTU_T1 = T1->getCtu(rpcBestCU->getCtuRsAddr);

	  assert(rpcBestCU->getWidth(0) == rpcBestCU->getHeight(0));
	  UInt const CU_SIZE = rpcBestCU->getWidth(0);
	  UInt centerOffset = CU_SIZE / 4 / 2 * 16 + CU_SIZE / 4 / 2;
	  UInt centerZorder = g_auiRasterToZscan[g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()] + centerOffset];
	  UInt frameWidthInCtus = pcPic->getFrameWidthInCtus();
	  UInt frameHeightInCtus = pcPic->getFrameHeightInCtus();
	  UInt tmpZorder = 0;

	  // 同位块（取中心 4x4 小块）
	  Dt_t0 = T0->getCtu(rpcBestCU->getCtuRsAddr())->getDepth(centerZorder);
	  Dt_t1 = T1->getCtu(rpcBestCU->getCtuRsAddr())->getDepth(centerZorder);
	  Dc = TexturePic->getCtu(rpcBestCU->getCtuRsAddr())->getDepth(centerZorder);

	  Pt_t0 = (UInt)T0->getCtu(rpcBestCU->getCtuRsAddr())->getPredictionMode(centerZorder) * 9 + (UInt)T0->getCtu(rpcBestCU->getCtuRsAddr())->getPartitionSize(centerZorder);
	  Pt_t1 = (UInt)T1->getCtu(rpcBestCU->getCtuRsAddr())->getPredictionMode(centerZorder) * 9 + (UInt)T1->getCtu(rpcBestCU->getCtuRsAddr())->getPartitionSize(centerZorder);
	  Pc = (UInt)TexturePic->getCtu(rpcBestCU->getCtuRsAddr())->getPredictionMode(centerZorder) * 9 + (UInt)TexturePic->getCtu(rpcBestCU->getCtuRsAddr())->getPartitionSize(centerZorder);
	  MergeFlag_c = TexturePic->getCtu(rpcBestCU->getCtuRsAddr())->getMergeFlag(centerZorder);
	  SkipFlag_c = TexturePic->getCtu(rpcBestCU->getCtuRsAddr())->getSkipFlag(centerZorder);
	  SDCFlag_c = TexturePic->getCtu(rpcBestCU->getCtuRsAddr())->getSDCFlag(centerZorder);
	  DISFlag_c = TexturePic->getCtu(rpcBestCU->getCtuRsAddr())->getDISFlag(centerZorder);
	  //CbfFlag_c = TexturePic->getCtu(rpcBestCU->getCtuRsAddr())->getQtRootCbf(centerZorder);
	  MSMFlag_c = (TexturePic->getCtu(rpcBestCU->getCtuRsAddr())->getMergeFlag(centerZorder) && !TexturePic->getCtu(rpcBestCU->getCtuRsAddr())->getPartitionSize(centerZorder)) ? 1 : 0;

	  if (DependentView)
	  {
		  Dv = BvPic->getCtu(rpcBestCU->getCtuRsAddr())->getDepth(centerZorder);
		  Pv = (UInt)BvPic->getCtu(rpcBestCU->getCtuRsAddr())->getPredictionMode(centerZorder) * 9 + (UInt)BvPic->getCtu(rpcBestCU->getCtuRsAddr())->getPartitionSize(centerZorder);
	  }

	  // 获取十字交叉均值或者十字交叉Flag和
	  UInt num_cc = 0;
	  
	  Dt_cc_t0 = 0;
	  Pt_cc_t0 = 0;
	  MergeFlag_cc_t0 = 0;
	  SkipFlag_cc_t0 = 0;
	  SDCFlag_cc_t0 = 0;
	  DISFlag_cc_t0 = 0;
	  //CbfFlag_cc_t0 = 0;
	  MSMFlag_cc_t0 = 0;

	  Dt_cc_t1 = 0;
	  Pt_cc_t1 = 0;
	  MergeFlag_cc_t1 = 0;
	  SkipFlag_cc_t1 = 0;
	  SDCFlag_cc_t1 = 0;
	  DISFlag_cc_t1 = 0;
	  //CbfFlag_cc_t1 = 0;
	  MSMFlag_cc_t1 = 0;

	  Dc_cc = 0;
	  Pc_cc = 0;
	  MergeFlag_cc_c = 0;
	  SkipFlag_cc_c = 0;
	  SDCFlag_cc_c = 0;
	  DISFlag_cc_c = 0;
	  //CbfFlag_cc_t1 = 0;
	  MSMFlag_cc_c = 0;
	  if (DependentView)
	  {
		  Dv_cc = 0;
		  Pv_cc = 0;
		  MergeFlag_cc_v = 0;
		  SkipFlag_cc_v = 0;
		  SDCFlag_cc_v = 0;
		  DISFlag_cc_v = 0;
		  //CbfFlag_cc_v += BvPic->getCtu(rpcBestCU->getCtuRsAddr())->getQtRootCbf(centerZorder);
		  MSMFlag_cc_v = 0;
	  }

	  // center
	  Dt_cc_t0 += Dt_t0;
	  Pt_cc_t0 += Pt_t0;
	  MergeFlag_cc_t0 += T0->getCtu(rpcBestCU->getCtuRsAddr())->getMergeFlag(centerZorder);
	  SkipFlag_cc_t0 += T0->getCtu(rpcBestCU->getCtuRsAddr())->getSkipFlag(centerZorder);
	  SDCFlag_cc_t0 += T0->getCtu(rpcBestCU->getCtuRsAddr())->getSDCFlag(centerZorder);
	  DISFlag_cc_t0 += T0->getCtu(rpcBestCU->getCtuRsAddr())->getDISFlag(centerZorder);
	  //CbfFlag_cc_t0 += T0->getCtu(rpcBestCU->getCtuRsAddr())->getQtRootCbf(centerZorder);
	  MSMFlag_cc_t0 += (T0->getCtu(rpcBestCU->getCtuRsAddr())->getMergeFlag(centerZorder) && !T0->getCtu(rpcBestCU->getCtuRsAddr())->getPartitionSize(centerZorder)) ? 1 : 0;

	  Dt_cc_t1 += Dt_t1; 
	  Pt_cc_t1 += Pt_t1; 
	  MergeFlag_cc_t1 += T1->getCtu(rpcBestCU->getCtuRsAddr())->getMergeFlag(centerZorder);
	  SkipFlag_cc_t1 += T1->getCtu(rpcBestCU->getCtuRsAddr())->getSkipFlag(centerZorder);
	  SDCFlag_cc_t1 += T1->getCtu(rpcBestCU->getCtuRsAddr())->getSDCFlag(centerZorder);
	  DISFlag_cc_t1 += T1->getCtu(rpcBestCU->getCtuRsAddr())->getDISFlag(centerZorder);
	  //CbfFlag_cc_t1 += T1->getCtu(rpcBestCU->getCtuRsAddr())->getQtRootCbf(centerZorder);
	  MSMFlag_cc_t1 += (T1->getCtu(rpcBestCU->getCtuRsAddr())->getMergeFlag(centerZorder) && !T1->getCtu(rpcBestCU->getCtuRsAddr())->getPartitionSize(centerZorder)) ? 1 : 0;

	  Dc_cc += Dc;
	  Pc_cc += Pc;
	  MergeFlag_cc_c += MergeFlag_c;
	  SkipFlag_cc_c += SkipFlag_c;
	  SDCFlag_cc_c += SDCFlag_c;
	  DISFlag_cc_c += DISFlag_c;
	  //CbfFlag_cc_c += CbfFlag_c;
	  MSMFlag_cc_c += MSMFlag_c;
	  
	  if (DependentView)
	  {
		  Dv_cc += Dv;
		  Pv_cc += Pv;
		  MergeFlag_cc_v += BvPic->getCtu(rpcBestCU->getCtuRsAddr())->getMergeFlag(centerZorder);
		  SkipFlag_cc_v += BvPic->getCtu(rpcBestCU->getCtuRsAddr())->getSkipFlag(centerZorder);
		  SDCFlag_cc_v += BvPic->getCtu(rpcBestCU->getCtuRsAddr())->getSDCFlag(centerZorder);
		  DISFlag_cc_v += BvPic->getCtu(rpcBestCU->getCtuRsAddr())->getDISFlag(centerZorder);
		  //CbfFlag_cc_v += BvPic->getCtu(rpcBestCU->getCtuRsAddr())->getQtRootCbf(centerZorder);
		  MSMFlag_cc_v += (BvPic->getCtu(rpcBestCU->getCtuRsAddr())->getMergeFlag(centerZorder) && !BvPic->getCtu(rpcBestCU->getCtuRsAddr())->getPartitionSize(centerZorder)) ? 1 : 0;
	  }
	  num_cc++;
	  // above
	  TComDataCU* CtuA_t0 = NULL;
	  TComDataCU* CtuA_t1 = NULL;
	  TComDataCU* CtuA_c = NULL;
	  TComDataCU* CtuA_v = NULL;
	  if (g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()] < 16)
	  {
		  CtuA_t0 = T0->getCtu(rpcBestCU->getCtuRsAddr())->getCtuAbove();
		  CtuA_t1 = T1->getCtu(rpcBestCU->getCtuRsAddr())->getCtuAbove();
		  CtuA_c = TexturePic->getCtu(rpcBestCU->getCtuRsAddr())->getCtuAbove();
		  if (DependentView)
		  {
			  CtuA_v = BvPic->getCtu(rpcBestCU->getCtuRsAddr())->getCtuAbove();
		  }
		  tmpZorder = g_auiRasterToZscan[g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()] + 256 - 16 * (rpcBestCU->getWidth(0) / 4) + centerOffset];
	  }
	  else
	  {
		  CtuA_t0 = T0->getCtu(rpcBestCU->getCtuRsAddr());
		  CtuA_t1 = T1->getCtu(rpcBestCU->getCtuRsAddr());
		  CtuA_c = TexturePic->getCtu(rpcBestCU->getCtuRsAddr());
		  if (DependentView)
		  {
			  CtuA_v = BvPic->getCtu(rpcBestCU->getCtuRsAddr());
		  }
		  tmpZorder = g_auiRasterToZscan[g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()] - 16 * (rpcBestCU->getWidth(0) / 4) + centerOffset];
	  }
	  if (CtuA_t0 && CtuA_t1 && CtuA_c)
	  {
		  num_cc++;
		  Dt_cc_t0 += CtuA_t0->getDepth(tmpZorder);
		  Dt_cc_t1 += CtuA_t1->getDepth(tmpZorder);
		  Dc_cc += CtuA_c->getDepth(tmpZorder);
		  Pt_cc_t0 += ((UInt)CtuA_t0->getPredictionMode(tmpZorder) * 9 + (UInt)CtuA_t0->getPartitionSize(tmpZorder));
		  Pt_cc_t1 += ((UInt)CtuA_t1->getPredictionMode(tmpZorder) * 9 + (UInt)CtuA_t1->getPartitionSize(tmpZorder));
		  Pc_cc += ((UInt)CtuA_c->getPredictionMode(tmpZorder) * 9 + (UInt)CtuA_c->getPartitionSize(tmpZorder));

		  MergeFlag_cc_t0 += CtuA_t0->getMergeFlag(tmpZorder);
		  SkipFlag_cc_t0 += CtuA_t0->getSkipFlag(tmpZorder);
		  SDCFlag_cc_t0 += CtuA_t0->getSDCFlag(tmpZorder);
		  DISFlag_cc_t0 += CtuA_t0->getDISFlag(tmpZorder);
		  //CbfFlag_cc_t0 += CtuA_t0->getQtRootCbf(tmpZorder);
		  MSMFlag_cc_t0 += ((CtuA_t0->getMergeFlag(tmpZorder) && !CtuA_t0->getPartitionSize(tmpZorder)) ? 1 : 0);
		  MergeFlag_cc_t1 += CtuA_t1->getMergeFlag(tmpZorder);
		  SkipFlag_cc_t1 += CtuA_t1->getSkipFlag(tmpZorder);
		  SDCFlag_cc_t1 += CtuA_t1->getSDCFlag(tmpZorder);
		  DISFlag_cc_t1 += CtuA_t1->getDISFlag(tmpZorder);
		  //CbfFlag_cc_t1 += CtuA_t1->getQtRootCbf(tmpZorder);
		  MSMFlag_cc_t1 += ((CtuA_t1->getMergeFlag(tmpZorder) && !CtuA_t1->getPartitionSize(tmpZorder)) ? 1 : 0);
		  MergeFlag_cc_c += CtuA_c->getMergeFlag(tmpZorder);
		  SkipFlag_cc_c += CtuA_c->getSkipFlag(tmpZorder);
		  SDCFlag_cc_c += CtuA_c->getSDCFlag(tmpZorder);
		  DISFlag_cc_c += CtuA_c->getDISFlag(tmpZorder);
		  //CbfFlag_cc_c += CtuA_c->getQtRootCbf(tmpZorder);
		  MSMFlag_cc_c += ((CtuA_c->getMergeFlag(tmpZorder) && !CtuA_c->getPartitionSize(tmpZorder)) ? 1 : 0);
		  if (CtuA_v)
		  {
			  Dv_cc += CtuA_v->getDepth(tmpZorder);
			  Pv_cc += ((UInt)CtuA_v->getPredictionMode(tmpZorder) * 9 + (UInt)CtuA_v->getPartitionSize(tmpZorder));
			  MergeFlag_cc_v += CtuA_v->getMergeFlag(tmpZorder);
			  SkipFlag_cc_v += CtuA_v->getSkipFlag(tmpZorder);
			  SDCFlag_cc_v += CtuA_v->getSDCFlag(tmpZorder);
			  DISFlag_cc_v += CtuA_v->getDISFlag(tmpZorder);
			  //CbfFlag_cc_v += CtuA_v->getQtRootCbf(tmpZorder);
			  MSMFlag_cc_v += ((CtuA_v->getMergeFlag(tmpZorder) && !CtuA_v->getPartitionSize(tmpZorder)) ? 1 : 0);
		  }
	  }

	  TComDataCU* CtuB_t0 = NULL;
	  TComDataCU* CtuB_t1 = NULL;
	  TComDataCU* CtuB_c = NULL;
	  TComDataCU* CtuB_v = NULL;
	  // below
	  if (g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()] + 16 * rpcBestCU->getWidth(0) / 4 > 255)
	  {
		  if (rpcBestCU->getCtuRsAddr() < frameWidthInCtus * (frameHeightInCtus - 1))
		  {
			  CtuB_t0 = T0->getCtu(rpcBestCU->getCtuRsAddr() + frameWidthInCtus);
			  CtuB_t1 = T1->getCtu(rpcBestCU->getCtuRsAddr() + frameWidthInCtus);
			  CtuB_c = TexturePic->getCtu(rpcBestCU->getCtuRsAddr() + frameWidthInCtus);
			  tmpZorder = g_auiRasterToZscan[g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()] - 256 + 16 * (rpcBestCU->getWidth(0) / 4) + centerOffset];

			  if (DependentView)
			  {
				  CtuB_v = BvPic->getCtu(rpcBestCU->getCtuRsAddr() + frameWidthInCtus);
			  }
		  }
	  }
	  else
	  {
		  CtuB_t0 = T0->getCtu(rpcBestCU->getCtuRsAddr());
		  CtuB_t1 = T1->getCtu(rpcBestCU->getCtuRsAddr());
		  CtuB_c = TexturePic->getCtu(rpcBestCU->getCtuRsAddr());
		  tmpZorder = g_auiRasterToZscan[g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()] + 16 * (rpcBestCU->getWidth(0) / 4) + centerOffset];

		  if (DependentView)
		  {
			  CtuB_v = BvPic->getCtu(rpcBestCU->getCtuRsAddr());
		  }
	  }
	  if (CtuB_t0 && CtuB_t1 && CtuB_c)
	  {
		  num_cc++;
		  Dt_cc_t0 += CtuB_t0->getDepth(tmpZorder);
		  Dt_cc_t1 += CtuB_t1->getDepth(tmpZorder);
		  Dc_cc += CtuB_c->getDepth(tmpZorder);
		  Pt_cc_t0 += ((UInt)CtuB_t0->getPredictionMode(tmpZorder) * 9 + (UInt)CtuB_t0->getPartitionSize(tmpZorder));
		  Pt_cc_t1 += ((UInt)CtuB_t1->getPredictionMode(tmpZorder) * 9 + (UInt)CtuB_t1->getPartitionSize(tmpZorder));
		  Pc_cc += ((UInt)CtuB_c->getPredictionMode(tmpZorder) * 9 + (UInt)CtuB_c->getPartitionSize(tmpZorder));

		  MergeFlag_cc_t0 += CtuB_t0->getMergeFlag(tmpZorder);
		  SkipFlag_cc_t0 += CtuB_t0->getSkipFlag(tmpZorder);
		  SDCFlag_cc_t0 += CtuB_t0->getSDCFlag(tmpZorder);
		  DISFlag_cc_t0 += CtuB_t0->getDISFlag(tmpZorder);
		  //CbfFlag_cc_t0 += CtuB_t0->getQtRootCbf(tmpZorder);
		  MSMFlag_cc_t0 += ((CtuB_t0->getMergeFlag(tmpZorder) && !CtuB_t0->getPartitionSize(tmpZorder)) ? 1 : 0);
		  MergeFlag_cc_t1 += CtuB_t1->getMergeFlag(tmpZorder);
		  SkipFlag_cc_t1 += CtuB_t1->getSkipFlag(tmpZorder);
		  SDCFlag_cc_t1 += CtuB_t1->getSDCFlag(tmpZorder);
		  DISFlag_cc_t1 += CtuB_t1->getDISFlag(tmpZorder);
		  //CbfFlag_cc_t1 += CtuB_t1->getQtRootCbf(tmpZorder);
		  MSMFlag_cc_t1 += ((CtuB_t1->getMergeFlag(tmpZorder) && !CtuB_t1->getPartitionSize(tmpZorder)) ? 1 : 0);
		  MergeFlag_cc_c += CtuB_c->getMergeFlag(tmpZorder);
		  SkipFlag_cc_c += CtuB_c->getSkipFlag(tmpZorder);
		  SDCFlag_cc_c += CtuB_c->getSDCFlag(tmpZorder);
		  DISFlag_cc_c += CtuB_c->getDISFlag(tmpZorder);
		  //CbfFlag_cc_c += CtuB_c->getQtRootCbf(tmpZorder);
		  MSMFlag_cc_c += ((CtuB_c->getMergeFlag(tmpZorder) && !CtuB_c->getPartitionSize(tmpZorder)) ? 1 : 0);
		  if (CtuB_v)
		  {
			  Dv_cc += CtuB_v->getDepth(tmpZorder);
			  Pv_cc += ((UInt)CtuB_v->getPredictionMode(tmpZorder) * 9 + (UInt)CtuB_v->getPartitionSize(tmpZorder));
			  MergeFlag_cc_v += CtuB_v->getMergeFlag(tmpZorder);
			  SkipFlag_cc_v += CtuB_v->getSkipFlag(tmpZorder);
			  SDCFlag_cc_v += CtuB_v->getSDCFlag(tmpZorder);
			  DISFlag_cc_v += CtuB_v->getDISFlag(tmpZorder);
			  //CbfFlag_cc_v += CtuB_v->getQtRootCbf(tmpZorder);
			  MSMFlag_cc_v += ((CtuB_v->getMergeFlag(tmpZorder) && !CtuB_v->getPartitionSize(tmpZorder)) ? 1 : 0);
		  }
	  }

	  TComDataCU* CtuL_t0 = NULL;
	  TComDataCU* CtuL_t1 = NULL;
	  TComDataCU* CtuL_c = NULL;
	  TComDataCU* CtuL_v = NULL;
	  // left
	  if (g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()] % 16 == 0)
	  {
		  CtuL_t0 = T0->getCtu(rpcBestCU->getCtuRsAddr())->getCtuLeft();
		  CtuL_t1 = T1->getCtu(rpcBestCU->getCtuRsAddr())->getCtuLeft();
		  CtuL_c = TexturePic->getCtu(rpcBestCU->getCtuRsAddr())->getCtuLeft();
		  tmpZorder = g_auiRasterToZscan[g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()] + 16 - rpcBestCU->getWidth(0) / 4 + centerOffset];

		  if (DependentView)
		  {
			  CtuL_v = BvPic->getCtu(rpcBestCU->getCtuRsAddr())->getCtuLeft();
		  }
	  }  
	  else
	  {
		  CtuL_t0 = T0->getCtu(rpcBestCU->getCtuRsAddr());
		  CtuL_t1 = T1->getCtu(rpcBestCU->getCtuRsAddr());
		  CtuL_c = TexturePic->getCtu(rpcBestCU->getCtuRsAddr());
		  tmpZorder = g_auiRasterToZscan[g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()] - rpcBestCU->getWidth(0) / 4 + centerOffset];

		  if (DependentView)
		  {
			  CtuL_v = BvPic->getCtu(rpcBestCU->getCtuRsAddr())->getCtuLeft();
		  }
	  }
	  if (CtuL_t0 && CtuL_t1 && CtuL_c)
	  {
		  num_cc++;
		  Dt_cc_t0 += CtuL_t0->getDepth(tmpZorder);
		  Dt_cc_t1 += CtuL_t1->getDepth(tmpZorder);
		  Dc_cc += CtuL_c->getDepth(tmpZorder);
		  Pt_cc_t0 += ((UInt)CtuL_t0->getPredictionMode(tmpZorder) * 9 + (UInt)CtuL_t0->getPartitionSize(tmpZorder));
		  Pt_cc_t1 += ((UInt)CtuL_t1->getPredictionMode(tmpZorder) * 9 + (UInt)CtuL_t1->getPartitionSize(tmpZorder));
		  Pc_cc += ((UInt)CtuL_c->getPredictionMode(tmpZorder) * 9 + (UInt)CtuL_c->getPartitionSize(tmpZorder));

		  MergeFlag_cc_t0 += CtuL_t0->getMergeFlag(tmpZorder);
		  SkipFlag_cc_t0 += CtuL_t0->getSkipFlag(tmpZorder);
		  SDCFlag_cc_t0 += CtuL_t0->getSDCFlag(tmpZorder);
		  DISFlag_cc_t0 += CtuL_t0->getDISFlag(tmpZorder);
		  //CbfFlag_cc_t0 += CtuL_t0->getQtRootCbf(tmpZorder);
		  MSMFlag_cc_t0 += ((CtuL_t0->getMergeFlag(tmpZorder) && !CtuL_t0->getPartitionSize(tmpZorder)) ? 1 : 0);
		  MergeFlag_cc_t1 += CtuL_t1->getMergeFlag(tmpZorder);
		  SkipFlag_cc_t1 += CtuL_t1->getSkipFlag(tmpZorder);
		  SDCFlag_cc_t1 += CtuL_t1->getSDCFlag(tmpZorder);
		  DISFlag_cc_t1 += CtuL_t1->getDISFlag(tmpZorder);
		  //CbfFlag_cc_t1 += CtuL_t1->getQtRootCbf(tmpZorder);
		  MSMFlag_cc_t1 += ((CtuL_t1->getMergeFlag(tmpZorder) && !CtuL_t1->getPartitionSize(tmpZorder)) ? 1 : 0);
		  MergeFlag_cc_c += CtuL_c->getMergeFlag(tmpZorder);
		  SkipFlag_cc_c += CtuL_c->getSkipFlag(tmpZorder);
		  SDCFlag_cc_c += CtuL_c->getSDCFlag(tmpZorder);
		  DISFlag_cc_c += CtuL_c->getDISFlag(tmpZorder);
		  //CbfFlag_cc_c += CtuL_c->getQtRootCbf(tmpZorder);
		  MSMFlag_cc_c += ((CtuL_c->getMergeFlag(tmpZorder) && !CtuL_c->getPartitionSize(tmpZorder)) ? 1 : 0);
		  if (CtuL_v)
		  {
			  Dv_cc += CtuL_v->getDepth(tmpZorder);
			  Pv_cc += ((UInt)CtuL_v->getPredictionMode(tmpZorder) * 9 + (UInt)CtuL_v->getPartitionSize(tmpZorder));
			  MergeFlag_cc_v += CtuL_v->getMergeFlag(tmpZorder);
			  SkipFlag_cc_v += CtuL_v->getSkipFlag(tmpZorder);
			  SDCFlag_cc_v += CtuL_v->getSDCFlag(tmpZorder);
			  DISFlag_cc_v += CtuL_v->getDISFlag(tmpZorder);
			  //CbfFlag_cc_v += CtuL_v->getQtRootCbf(tmpZorder);
			  MSMFlag_cc_v += ((CtuL_v->getMergeFlag(tmpZorder) && !CtuL_v->getPartitionSize(tmpZorder)) ? 1 : 0);
		  }
	  }

	  TComDataCU* CtuR_t0 = NULL;
	  TComDataCU* CtuR_t1 = NULL;
	  TComDataCU* CtuR_c = NULL;
	  TComDataCU* CtuR_v = NULL;
	  // right
	  if ((g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()] + rpcBestCU->getWidth(0) / 4 ) % 16 == 0)
	  {
		  if (rpcBestCU->getCtuRsAddr() % frameWidthInCtus < (frameWidthInCtus - 1))
		  {
			  CtuR_t0 = T0->getCtu(rpcBestCU->getCtuRsAddr() + 1);
			  CtuR_t1 = T1->getCtu(rpcBestCU->getCtuRsAddr() + 1);
			  CtuR_c = TexturePic->getCtu(rpcBestCU->getCtuRsAddr() + 1);
			  tmpZorder = g_auiRasterToZscan[g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()] - 16 + rpcBestCU->getWidth(0) / 4 + centerOffset];
			  
			  if (DependentView)
			  {
				  CtuR_v = BvPic->getCtu(rpcBestCU->getCtuRsAddr() + 1);
			  }
		  }
	  }
	  else
	  {
		  CtuR_t0 = T0->getCtu(rpcBestCU->getCtuRsAddr());
		  CtuR_t1 = T1->getCtu(rpcBestCU->getCtuRsAddr());
		  CtuR_c = TexturePic->getCtu(rpcBestCU->getCtuRsAddr());
		  tmpZorder = g_auiRasterToZscan[g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()] + rpcBestCU->getWidth(0) / 4 + centerOffset];

		  if (DependentView)
		  {
			  CtuR_v = BvPic->getCtu(rpcBestCU->getCtuRsAddr());
		  }
	  }
	  if (CtuR_t0 && CtuR_t1 && CtuR_c)
	  {
		  num_cc++;
		  Dt_cc_t0 += CtuR_t0->getDepth(tmpZorder);
		  Dt_cc_t1 += CtuR_t1->getDepth(tmpZorder);
		  Dc_cc += CtuR_c->getDepth(tmpZorder);
		  Pt_cc_t0 += ((UInt)CtuR_t0->getPredictionMode(tmpZorder) * 9 + (UInt)CtuR_t0->getPartitionSize(tmpZorder));
		  Pt_cc_t1 += ((UInt)CtuR_t1->getPredictionMode(tmpZorder) * 9 + (UInt)CtuR_t1->getPartitionSize(tmpZorder));
		  Pc_cc += ((UInt)CtuR_c->getPredictionMode(tmpZorder) * 9 + (UInt)CtuR_c->getPartitionSize(tmpZorder));

		  MergeFlag_cc_t0 += CtuR_t0->getMergeFlag(tmpZorder);
		  SkipFlag_cc_t0 += CtuR_t0->getSkipFlag(tmpZorder);
		  SDCFlag_cc_t0 += CtuR_t0->getSDCFlag(tmpZorder);
		  DISFlag_cc_t0 += CtuR_t0->getDISFlag(tmpZorder);
		  //CbfFlag_cc_t0 += CtuR_t0->getQtRootCbf(tmpZorder);
		  MSMFlag_cc_t0 += ((CtuR_t0->getMergeFlag(tmpZorder) && !CtuR_t0->getPartitionSize(tmpZorder)) ? 1 : 0);
		  MergeFlag_cc_t1 += CtuR_t1->getMergeFlag(tmpZorder);
		  SkipFlag_cc_t1 += CtuR_t1->getSkipFlag(tmpZorder);
		  SDCFlag_cc_t1 += CtuR_t1->getSDCFlag(tmpZorder);
		  DISFlag_cc_t1 += CtuR_t1->getDISFlag(tmpZorder);
		  //CbfFlag_cc_t1 += CtuR_t1->getQtRootCbf(tmpZorder);
		  MSMFlag_cc_t1 += ((CtuR_t1->getMergeFlag(tmpZorder) && !CtuR_t1->getPartitionSize(tmpZorder)) ? 1 : 0);
		  MergeFlag_cc_c += CtuR_c->getMergeFlag(tmpZorder);
		  SkipFlag_cc_c += CtuR_c->getSkipFlag(tmpZorder);
		  SDCFlag_cc_c += CtuR_c->getSDCFlag(tmpZorder);
		  DISFlag_cc_c += CtuR_c->getDISFlag(tmpZorder);
		  //CbfFlag_cc_c += CtuR_c->getQtRootCbf(tmpZorder);
		  MSMFlag_cc_c += ((CtuR_c->getMergeFlag(tmpZorder) && !CtuR_c->getPartitionSize(tmpZorder)) ? 1 : 0);
		  if (CtuR_v)
		  {
			  Dv_cc += CtuR_v->getDepth(tmpZorder);
			  Pv_cc += ((UInt)CtuR_v->getPredictionMode(tmpZorder) * 9 + (UInt)CtuR_v->getPartitionSize(tmpZorder));
			  MergeFlag_cc_v += CtuR_v->getMergeFlag(tmpZorder);
			  SkipFlag_cc_v += CtuR_v->getSkipFlag(tmpZorder);
			  SDCFlag_cc_v += CtuR_v->getSDCFlag(tmpZorder);
			  DISFlag_cc_v += CtuR_v->getDISFlag(tmpZorder);
			  //CbfFlag_cc_v += CtuR_v->getQtRootCbf(tmpZorder);
			  MSMFlag_cc_v += ((CtuR_v->getMergeFlag(tmpZorder) && !CtuR_v->getPartitionSize(tmpZorder)) ? 1 : 0);
		  }
	  }

	  deltaPOC_t0 = T0->getPOC() - rpcBestCU->getSlice()->getPOC() > 0 ? T0->getPOC() - rpcBestCU->getSlice()->getPOC() : rpcBestCU->getSlice()->getPOC() - T0->getPOC();
	  deltaPOC_t1 = T1->getPOC() - rpcBestCU->getSlice()->getPOC() > 0 ? T1->getPOC() - rpcBestCU->getSlice()->getPOC() : rpcBestCU->getSlice()->getPOC() - T1->getPOC();
	  // 至少存在同位块，故num_cc必然不为零
	  assert(num_cc);
	  Dt_cc_t0 /= num_cc;
	  Dt_cc_t1 /= num_cc;
	  Dc_cc /= num_cc;
	  Pt_cc_t0 /= num_cc;
	  Pt_cc_t1 /= num_cc;
	  Pc_cc /= num_cc;
	  //MergeFlag_cc_t0 /= num_cc;
	  //SkipFlag_cc_t0 /= num_cc;
	  //SDCFlag_cc_t0 /= num_cc;
	  //DISFlag_cc_t0 /= num_cc;
	  //CbfFlag_cc_t0 /= num_cc;
	  //MSMFlag_cc_t0 /= num_cc;
	  //MergeFlag_cc_t1 /= num_cc;
	  //SkipFlag_cc_t1 /= num_cc;
	  //SDCFlag_cc_t1 /= num_cc;
	  //DISFlag_cc_t1 /= num_cc;
	  //CbfFlag_cc_t1 /= num_cc;
	  //MSMFlag_cc_t1 /= num_cc;
	  //MergeFlag_cc_c /= num_cc;
	  //SkipFlag_cc_c /= num_cc;
	  //SDCFlag_cc_c /= num_cc;
	  //DISFlag_cc_c /= num_cc;
	  //CbfFlag_cc_c /= num_cc;
	  //MSMFlag_cc_c /= num_cc;

	  if (DependentView)
	  {
		  Dv_cc /= num_cc;
		  Pv_cc /= num_cc;
		  //MergeFlag_cc_v /= num_cc;
		  //SkipFlag_cc_v /= num_cc;
		  //SDCFlag_cc_v /= num_cc;
		  //DISFlag_cc_v /= num_cc;
		  ////CbfFlag_cc_v /= num_cc;
		  //MSMFlag_cc_v /= num_cc;
	  }

	  // ============================== Spatial correlation (S) ======================================

	  // pcCU->getPartIndexAndSize( iPartIdx, uiPartAddr, iRoiWidth, iRoiHeight ); 可以获得CU块坐标值

	  //// 空域相邻块采用Merge模式所选取的5个块 (A0 块有可能尚未编码，merge 候选列表的构建仍需进一步理解)
	  //// A0 A1 B0 B1 B2

	  // get A1 分两种情况考虑
	  UInt A1=256;
	  TComDataCU* CU_A1=NULL;
	  if (g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()] % 16 > 0)
	  {
		  // 非第一列
		  A1 = g_auiRasterToZscan[g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()] + 16 * (rpcBestCU->getWidth(0) / 4 - 1) - 1];
		  CU_A1 = rpcBestCU->getSlice()->getPic()->getCtu(rpcBestCU->getCtuRsAddr());
	  }
	  else
	  {
		  TComDataCU* CtuL = rpcBestCU->getCtuLeft();
		  if (CtuL)
		  {
			  A1 = g_auiRasterToZscan[g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()] + 16 * (rpcBestCU->getWidth(0) / 4 - 1) + 15];
			  CU_A1 = CtuL;
		  }
	  }

	  // get B0 分四种情况考虑
	  UInt B0=256;
	  TComDataCU* CU_B0=NULL;
	  if ((g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()] + rpcBestCU->getWidth(0) / 4) % 16 > 0 && g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()] > 15)
	  {
		  // CTU 内部，不沾边
		  B0 = g_auiRasterToZscan[g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()] - 16 + rpcBestCU->getWidth(0) / 4];
		  CU_B0 = rpcBestCU->getSlice()->getPic()->getCtu(rpcBestCU->getCtuRsAddr());
	  }
	  if ((g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()] + rpcBestCU->getWidth(0) / 4) % 16 == 0 && g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()] > 15)
	  {
		  // 仅沾右边
		  if ((rpcBestCU->getCtuRsAddr() + 1) % frameWidthInCtus > 0)
		  {
			  CU_B0 = rpcBestCU->getSlice()->getPic()->getCtu(rpcBestCU->getCtuRsAddr() + 1);
			  B0 = g_auiRasterToZscan[g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()] + rpcBestCU->getWidth(0) / 4 - 16 * 2];
		  }
	  }
	  if ((g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()] + rpcBestCU->getWidth(0) / 4) % 16 > 0 && g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()] < 16)
	  {
		  // 仅沾上边
		  if (rpcBestCU->getCtuAbove())
		  {
			  CU_B0 = rpcBestCU->getCtuAbove();
			  B0 = g_auiRasterToZscan[g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()] + 256 - 16 + rpcBestCU->getWidth(0) / 4];
		  }
	  }
	  if ((g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()] + rpcBestCU->getWidth(0) / 4) % 16 == 0 && g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()] < 16)
	  {
		  // 右上角
		  if (rpcBestCU->getCtuAboveRight())
		  {
			  CU_B0 = rpcBestCU->getCtuAboveRight();
			  B0 = 170;
		  }
	  }

	  // get B1 分两种情况考虑
	  UInt B1 = 256;
	  TComDataCU* CU_B1 = NULL;
	  if (g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()] > 15)
	  {
		  // 非第一行
		  B1 = g_auiRasterToZscan[g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()] - 16 + (rpcBestCU->getWidth(0) / 4 - 1)];
		  CU_B1 = rpcBestCU->getSlice()->getPic()->getCtu(rpcBestCU->getCtuRsAddr());
	  }
	  else
	  {
		  if (rpcBestCU->getCtuAbove())
		  {
			  B1 = g_auiRasterToZscan[g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()] + rpcBestCU->getWidth(0) / 4 - 1 + 16 * 15];
			  CU_B1 = rpcBestCU->getCtuAbove();
		  }
	  }

	  // get B2 分四种情况考虑
	  UInt B2 = 256;
	  TComDataCU* CU_B2 = NULL;
	  if ((g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()]) % 16 > 0 && g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()] > 15)
	  {
		  // CTU 内部，不沾边
		  B2 = g_auiRasterToZscan[g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()] - 17];
		  CU_B2 = rpcBestCU->getSlice()->getPic()->getCtu(rpcBestCU->getCtuRsAddr());
	  }
	  if ((g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()]) % 16 == 0 && g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()] > 15)
	  {
		  // 仅沾左边
		  if (rpcBestCU->getCtuLeft())
		  {
			  B2 = g_auiRasterToZscan[g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()] - 1];
			  CU_B2 = rpcBestCU->getCtuLeft();
		  }
	  }
	  if ((g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()]) % 16 > 0 && g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()] < 16)
	  {
		  // 仅沾上边
		  if (rpcBestCU->getCtuAbove())
		  {
			  B2 = g_auiRasterToZscan[g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()] + 256 - 17];
			  CU_B2 = rpcBestCU->getCtuAbove();
		  }
	  }
	  if ((g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()]) % 16 == 0 && g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()] < 16)
	  {
		  // 左上角
		  if (rpcBestCU->getCtuAboveLeft())
		  {
			  B2 = 255;
			  CU_B2 = rpcBestCU->getCtuAboveLeft();
		  }
	  }

	  UInt numSpatial4 = 0;
	  Ds4mean = 0;
	  Ps4mean = 0;
	  MergeFlag_s = 0;
	  SkipFlag_s = 0;
	  SDCFlag_s = 0;
	  DISFlag_s = 0;
	  MSMFlag_s = 0;
	  if (CU_A1) {
		  Ds4mean += CU_A1->getDepth(A1); 
		  Ps4mean += ((UInt)CU_A1->getPredictionMode(A1) * 9 + (UInt)CU_A1->getPartitionSize(A1));
		  numSpatial4++;
		  MergeFlag_s += CU_A1->getMergeFlag(A1);
		  SkipFlag_s += CU_A1->getSkipFlag(A1);
		  SDCFlag_s += CU_A1->getSDCFlag(A1);
		  DISFlag_s += CU_A1->getDISFlag(A1);
		  //CbfFlag_s += CU_A1->getQtRootCbf(A1);
		  MSMFlag_s += ((CU_A1->getMergeFlag(A1) && !CU_A1->getPartitionSize(A1)) ? 1 : 0);
	  }
	  if (CU_B0) {
		  Ds4mean += CU_B0->getDepth(B0); 
		  Ps4mean += ((UInt)CU_B0->getPredictionMode(B0) * 9 + (UInt)CU_B0->getPartitionSize(B0));
		  numSpatial4++;
		  MergeFlag_s += CU_B0->getMergeFlag(B0);
		  SkipFlag_s += CU_B0->getSkipFlag(B0);
		  SDCFlag_s += CU_B0->getSDCFlag(B0);
		  DISFlag_s += CU_B0->getDISFlag(B0);
		  //CbfFlag_s += CU_B0->getQtRootCbf(B0);
		  MSMFlag_s += ((CU_B0->getMergeFlag(B0) && !CU_B0->getPartitionSize(B0)) ? 1 : 0);
	  }
	  if (CU_B1) {
		  Ds4mean += CU_B1->getDepth(B1); 
		  Ps4mean += ((UInt)CU_B1->getPredictionMode(B1) * 9 + (UInt)CU_B1->getPartitionSize(B1));
		  numSpatial4++;
		  MergeFlag_s += CU_B1->getMergeFlag(B1);
		  SkipFlag_s += CU_B1->getSkipFlag(B1);
		  SDCFlag_s += CU_B1->getSDCFlag(B1);
		  DISFlag_s += CU_B1->getDISFlag(B1);
		  //CbfFlag_s += CU_B1->getQtRootCbf(B1);
		  MSMFlag_s += ((CU_B1->getMergeFlag(B1) && !CU_B1->getPartitionSize(B1)) ? 1 : 0);
	  }
	  if (CU_B2) {
		  Ds4mean += CU_B2->getDepth(B2); 
		  Ps4mean += ((UInt)CU_B2->getPredictionMode(B2) * 9 + (UInt)CU_B2->getPartitionSize(B2));
		  numSpatial4++;
		  MergeFlag_s += CU_B2->getMergeFlag(B2);
		  SkipFlag_s += CU_B2->getSkipFlag(B2);
		  SDCFlag_s += CU_B2->getSDCFlag(B2);
		  DISFlag_s += CU_B2->getDISFlag(B2);
		  //CbfFlag_s += CU_B2->getQtRootCbf(B2);
		  MSMFlag_s += ((CU_B2->getMergeFlag(B2) && !CU_B2->getPartitionSize(B2)) ? 1 : 0);
	  }
	  //Ds4mean = -1.0;
	  if (numSpatial4)
	  {
		  Ds4mean /= numSpatial4;
		  Ps4mean /= numSpatial4;
	  }
	  else
	  {
		  Ds4mean = -1.0;
		  Ps4mean = -1.0;
		  MergeFlag_s = -1.0;
		  SkipFlag_s = -1.0;
		  SDCFlag_s = -1.0;
		  DISFlag_s = -1.0;
		  MSMFlag_s = -1.0;
	  }

	  // Ds_GAMMA
	  Ds_GAMMA = 0;
	  int tmp = 0;
	  TComDataCU* Ctu_cur = rpcBestCU->getSlice()->getPic()->getCtu(rpcBestCU->getCtuRsAddr());
	  if ((g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()]) % 16 > 0 && g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()] > 15)
	  {
		  // CTU 内部，不沾边
		  for (UInt i = 0; i < rpcBestCU->getWidth(0) / 4; i++)
		  {
			  Ds_GAMMA += Ctu_cur->getDepth(g_auiRasterToZscan[g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()] - 16 + i]);
			  tmp++;
		  }
		  for (UInt i = 0; i < rpcBestCU->getWidth(0) / 4; i++)
		  {
			  Ds_GAMMA += Ctu_cur->getDepth(g_auiRasterToZscan[g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()] - 1 + i * 16]);
			  tmp++;
		  }
		  Ds_GAMMA += Ctu_cur->getDepth(g_auiRasterToZscan[g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()] - 1 - 16]);
		  tmp++;
	  }
	  if ((g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()]) % 16 == 0 && g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()] > 15)
	  {
		  // 仅沾左边
		  for (UInt i = 0; i < rpcBestCU->getWidth(0) / 4; i++)
		  {
			  Ds_GAMMA += Ctu_cur->getDepth(g_auiRasterToZscan[g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()] - 16 + i]);
			  tmp++;
		  }
		  TComDataCU* CtuL = rpcBestCU->getCtuLeft();
		  if (CtuL)
		  {
			  for (UInt i = 0; i < rpcBestCU->getWidth(0) / 4; i++)
			  {
				  Ds_GAMMA += CtuL->getDepth(g_auiRasterToZscan[g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()] + 15 + i * 16]);
				  tmp++;
			  }
			  Ds_GAMMA += CtuL->getDepth(g_auiRasterToZscan[g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()] - 1]);
			  tmp++;
		  }
	  }
	  if ((g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()]) % 16 > 0 && g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()] < 16)
	  {
		  // 仅沾上边
		  TComDataCU* CtuA = rpcBestCU->getCtuAbove();
		  if (CtuA)
		  {
			  for (UInt i = 0; i < rpcBestCU->getWidth(0) / 4; i++)
			  {
				  Ds_GAMMA += CtuA->getDepth(g_auiRasterToZscan[g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()] + 256 - 16 + i]);
				  tmp++;
			  }

			  Ds_GAMMA += CtuA->getDepth(g_auiRasterToZscan[g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()] + 256 - 17]);
			  tmp++;
		  }
		  for (UInt i = 0; i < rpcBestCU->getWidth(0) / 4; i++)
		  {
			  Ds_GAMMA += Ctu_cur->getDepth(g_auiRasterToZscan[g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()] - 1 + 16 + i]);
			  tmp++;
		  }
	  }
	  if ((g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()]) % 16 == 0 && g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()] < 16)
	  {
		  // 左上角
		  TComDataCU* CtuAL = Ctu_cur->getCtuAboveLeft();
		  TComDataCU* CtuA = Ctu_cur->getCtuAbove();
		  TComDataCU* CtuL = Ctu_cur->getCtuLeft();
		  if (CtuAL && CtuA && CtuL)
		  {
			  for (UInt i = 0; i < rpcBestCU->getWidth(0) / 4; i++)
			  {
				  Ds_GAMMA += CtuA->getDepth(g_auiRasterToZscan[g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()] + 256 - 16 + i]);
				  tmp++;
			  }

			  Ds_GAMMA += CtuAL->getDepth(255);
			  tmp++;

			  for (UInt i = 0; i < rpcBestCU->getWidth(0) / 4; i++)
			  {
				  Ds_GAMMA += CtuL->getDepth(g_auiRasterToZscan[g_auiZscanToRaster[rpcBestCU->getZorderIdxInCtu()] + 15 + 16 * i]);
				  tmp++;
			  }
		  }
	  }
	  if (tmp)
	  {
		  Ds_GAMMA /= tmp;
	  }
	  else
	  {
		  Ds_GAMMA = -1.0;
	  }
	  //std::cout << Ds_GAMMA << std::endl;

	  // ============================== Current Pixel and codec features ======================================

	  //  当前块纹理特征
	  Pel *curPel, *tcolPel0, *tcolPel1, *ccolPel, *vcolPel;
	  TComPicYuv * curYuv = rpcBestCU->getSlice()->getPic()->getPicYuvOrg();
	  TComPicYuv * colYuv_T0 = T0->getPicYuvOrg();
	  TComPicYuv * colYuv_T1 = T1->getPicYuvOrg();
	  TComPicYuv * colYuv_C = TexturePic->getPicYuvOrg();
	  TComPicYuv * colYuv_V = NULL;

	  TComDataCU * colCU_V = NULL;
	  if (rpcBestCU->getSlice()->getViewId() != 1)
	  {
		  TComPic* pColPic_V = rpcBestCU->getSlice()->getIvPic(1, 0); // 1: depthFlag, 0: baseViewIndex
		  colYuv_V = pColPic_V->getPicYuvOrg();
		  colCU_V = pColPic_V->getCtu(rpcBestCU->getCtuRsAddr());
	  }

	  Pel* buffer = new Pel[CU_SIZE*CU_SIZE]();   // 初始化为0
	  Pel** bufferSub = new Pel*[4];
	  for (int i = 0; i < 4; i++)
	  {
		  bufferSub[i] = new Pel[CU_SIZE*CU_SIZE / 4]();
	  }
	  double sub[4] = { 0.0, 0.0, 0.0, 0.0 };  // 四子块均值方差
	  int cntSub0 = 0, cntSub1 = 0, cntSub2 = 0, cntSub3 = 0;
	  int PixelRange = 256;
	  double* Histcur = new double[PixelRange](); // 当前编码块直方图
	  // double* Histcolt0 = new double[PixelRange](); // 时域同位块直方图
	  // double* Histcolt1 = new double[PixelRange](); // 时域同位块直方图
	  double* Histcolc = new double[PixelRange](); // 纹理同位块直方图
	  // double* Histcolv = new double[PixelRange](); // 基视点同位块直方图
	  ::memset(Histcur, 0, sizeof(double)*PixelRange);
	  // memset(Histcolt0, 0, sizeof(double)*PixelRange);
	  // memset(Histcolt1, 0, sizeof(double)*PixelRange);
	  ::memset(Histcolc, 0, sizeof(double)*PixelRange);
	  // memset(Histcolv, 0, sizeof(double)*PixelRange);
	  double max = 0.0, min = 0.0;


	  for (Int comp = 0; comp < curYuv->getNumberValidComponents(); comp++)
	  {
		  //cout << "时域同位块curYuv->getNumberValidComponents(): " << curYuv->getNumberValidComponents() << endl;
#if PRINT_DEBUG 
		  cout << "============================================ 当前 CU 信息 ====================================" << endl;
		  cout << "视作普通块获得的信息" << endl;
		  cout << "rpcBestCU->getSlice()->getPOC():                                  " << rpcBestCU->getSlice()->getPOC() << endl;
		  cout << "当前是非 I 帧:                                                     " << (rpcBestCU->getSlice()->getSliceType() != I_SLICE) << endl;
		  cout << "当前是 B 帧:                                                       " << (rpcBestCU->getSlice()->getSliceType() == B_SLICE) << endl;
		  cout << "uiDepth:                                                          " << uiDepth << endl;
		  cout << "rpcBestCU->getDepth                                               " << (int)rpcBestCU->getDepth(0) << endl;
		  cout << "rpcBestCU->getWidth                                               " << (int)rpcBestCU->getWidth(0) << endl;
		  cout << "rpcBestCU->getPredictionMode			                                 " << rpcBestCU->getPredictionMode(0) << endl;
		  cout << "rpcBestCU->getPartitionSize			                                 " << rpcBestCU->getPartitionSize(0) << endl;
		  cout << "rpcBestCU->getCtuRsAddr                                           " << rpcBestCU->getCtuRsAddr() << endl;
		  cout << "rpcBestCU->getCUPelX                                              " << rpcBestCU->getCUPelX() << endl;
		  cout << "rpcBestCU->getCUPelY                                              " << rpcBestCU->getCUPelY() << endl;
		  cout << "rpcBestCU->getZorderIdxInCtu                                      " << rpcBestCU->getZorderIdxInCtu() << endl;
		  cout << "rpcBestCU->isSkipped                                              " << rpcBestCU->isSkipped(0) << endl;
		  cout << "rpcBestCU->getMergeFlag                                           " << rpcBestCU->getMergeFlag(0) << endl;
		  cout << "深度块信息" << endl;
		  cout << "rpcBestCU->getSlice()->getIsDepth():                              " << rpcBestCU->getSlice()->getIsDepth() << endl;
		  cout << "rpcBestCU->getSlice()->getPOC():                                  " << rpcBestCU->getSlice()->getPOC() << endl;
		  cout << "rpcBestCU->getSlice()->getViewId():                               " << rpcBestCU->getSlice()->getViewId() << endl;
		  cout << "rpcBestCU->getSlice()->getViewIndex():                            " << rpcBestCU->getSlice()->getViewIndex() << endl;
#endif	  
		  ComponentID compID = (ComponentID)comp;
		  Int iWidth = rpcBestCU->getWidth(0) >> getComponentScaleX(compID, curYuv->getChromaFormat());
		  Int iHeight = rpcBestCU->getHeight(0) >> getComponentScaleY(compID, curYuv->getChromaFormat());

		  curPel = curYuv->getAddr(compID, rpcBestCU->getCtuRsAddr(), rpcBestCU->getZorderIdxInCtu());
		  tcolPel0 = colYuv_T0->getAddr(compID, rpcBestCU->getCtuRsAddr(), rpcBestCU->getZorderIdxInCtu());
		  tcolPel1 = colYuv_T1->getAddr(compID, rpcBestCU->getCtuRsAddr(), rpcBestCU->getZorderIdxInCtu());
		  ccolPel = colYuv_C->getAddr(compID, rpcBestCU->getCtuRsAddr(), rpcBestCU->getZorderIdxInCtu());
		  if (rpcBestCU->getSlice()->getViewId() != 1)
		  {
			  vcolPel = colYuv_V->getAddr(compID, rpcBestCU->getCtuRsAddr(), rpcBestCU->getZorderIdxInCtu());
		  }

		  UInt  iDstStride = curYuv->getStride(compID);
		  Int k = 0;

		  //CU_64_feature << "当前块像素" << endl;

		  // F0: 时域同位块灰度相似度, F3: 基视点同位块灰度相似度VSG
		  double Gcur = 0.0;
		  double Gtcol0 = 0.0;
		  double Gtcol1 = 0.0;
		  double Gvcol = 0.0;
		  for (Int i = 0; i < iHeight; i++)
		  {
			  for (Int j = 0; j < iWidth; j++)
			  {
				  Gcur += curPel[k];
				  Gtcol0 += tcolPel0[k];
				  Gtcol1 += tcolPel1[k];
				  if (rpcBestCU->getSlice()->getViewId() != 1)
				  {
					  Gvcol += vcolPel[k];
				  }

				  // 缓存当前块像素值
				  buffer[i*iHeight + j] = curPel[k];

				  // 分别缓存四个子块像素值
				  if (i < iHeight / 2)
				  {
					  if (j < iWidth / 2)
					  {
						  bufferSub[0][cntSub0++] = curPel[k];
						  sub[0] += curPel[k];
					  }
					  else
					  {
						  bufferSub[1][cntSub1++] = curPel[k];
						  sub[1] += curPel[k];
					  }
				  }
				  else
				  {
					  if (j < iWidth / 2)
					  {
						  bufferSub[2][cntSub2++] = curPel[k];
						  sub[2] += curPel[k];
					  }
					  else
					  {
						  bufferSub[3][cntSub3++] = curPel[k];
						  sub[3] += curPel[k];
					  }
				  }


				  Histcur[curPel[k]]++;  // 当前块直方图
				  // Histcolt0[tcolPel0[k]]++;  // 时域同位块直方图
				  // Histcolt1[tcolPel1[k]]++;  // 时域同位块直方图
				  Histcolc[ccolPel[k]]++;  // 纹理同位块直方图
				  // if (rpcBestCU->getSlice()->getViewId() != 1)
				  // {
				  // 	  Histcolv[vcolPel[k]]++;  // 基视点同位块直方图
				  // }
				  k++;
			  }
			  k = 0;
			  curPel += iDstStride;
			  tcolPel0 += iDstStride;
			  tcolPel1 += iDstStride;
			  ccolPel += iDstStride;
			  if (rpcBestCU->getSlice()->getViewId() != 1)
			  {
				  vcolPel += iDstStride;
			  }
		  }
		  Gcur /= (iWidth * iHeight);
		  Gtcol0 /= (iWidth * iHeight);
		  Gtcol1 /= (iWidth * iHeight);
		  if (rpcBestCU->getSlice()->getViewId() != 1)
		  {
			  Gvcol /= (iWidth * iHeight);
			  VSG = Gcur - Gvcol < 0 ? Gvcol - Gcur : Gcur - Gvcol; // F3:基视点灰度相似度VSG
		  }
		  else
		  {
			  VSG = -1.0; // F3:基视点灰度相似度VSG
		  }
		  SG_t0 = Gcur - Gtcol0 < 0 ? Gtcol0 - Gcur : Gcur - Gtcol0; // F0:时域灰度相似度SG
		  SG_t1 = Gcur - Gtcol1 < 0 ? Gtcol1 - Gcur : Gcur - Gtcol1; // F0:时域灰度相似度SG
		  curMean = Gcur; // F8:当前块均值curMean
	  }

		// 熵 Hc Hcur
	  Hc = 0.0;
	  Hcur = 0.0;
	  for (UInt i = 0; i < PixelRange; i++)
	  {
		  Hc += (Histcolc[i] > 0 ? Histcolc[i] / (CU_SIZE*CU_SIZE) * log((CU_SIZE*CU_SIZE) / Histcolc[i]) / log(2.0) : 0);
		  Hcur += (Histcur[i] > 0 ? Histcur[i] / (CU_SIZE*CU_SIZE) * log((CU_SIZE*CU_SIZE) / Histcur[i]) / log(2.0) : 0);
	  }


	  // 当前块方差
	  curVar = 0.0;
	  for (Int i = 0; i < CU_SIZE * CU_SIZE; i++)
	  {
		  curVar += (buffer[i] - curMean) * (buffer[i] - curMean);
	  }
	  curVar /= (CU_SIZE * CU_SIZE); // F9:当前块方差curVar


	  // 当前块子块最大方差F11:curSubmvar；子块均值和父块最大差值F13:curSubdmean，子块方差和父块最大差值F14:curSubdvar
	  // F13
	  curSubdmean = curMean - sub[0] / (CU_SIZE*CU_SIZE / 4) < 0 ? sub[0] / (CU_SIZE*CU_SIZE / 4) - curMean : curMean - sub[0] / (CU_SIZE*CU_SIZE / 4);
	  for (int i = 0; i < 4; i++)
	  {
		  double mean = sub[i] / (CU_SIZE*CU_SIZE / 4); // 子块均值
		  if (curSubdmean < (curMean - mean < 0 ? mean - curMean : curMean - mean))
		  {
			  curSubdmean = curMean - mean < 0 ? mean - curMean : curMean - mean;
		  }

		  sub[i] = 0.0;
		  for (int j = 0; j < CU_SIZE*CU_SIZE / 4; j++)
		  {
			  sub[i] += (bufferSub[i][j] - mean) * (bufferSub[i][j] - mean);
		  }
		  // 子块方差
		  sub[i] /= (CU_SIZE*CU_SIZE / 4);

		  if (i == 0)
		  {
			  // F11
			  curSubmvar = sub[i];
			  // F14
			  curSubdvar = curVar - sub[i] < 0 ? sub[i] - curVar : curVar - sub[i];
		  }
		  else
		  {
			  if (curSubmvar < sub[i])
			  {
				  curSubmvar = sub[i]; // 子块最大方差
			  }
			  if (curSubdvar < (curVar - sub[i] < 0 ? sub[i] - curVar : curVar - sub[i]))
			  {
				  curSubdvar = curVar - sub[i] < 0 ? sub[i] - curVar : curVar - sub[i]; // 子块最大方差和父块差值
			  }
		  }
	  }

	  // F35: 归一化子块均值和父块最大差值 Norm_curSubdmean
	  Norm_curSubdmean = curMean > 0 ? curSubdmean / curMean : 1;
	  // F36: 子块方差与父块最大差值比率 Norm_curSubdvar
	  Norm_curSubdvar = curVar > 0 ? curSubdvar / curVar : 1;
	  // F12: 子块最大方差和父块方差比率 Ratio_curSubmvar
	  Ratio_curSubmvar = curVar > 0 ? curSubmvar / curVar : 1;

	  delete[] buffer;
	  for (int i = 0; i < 4; i++)
	  {
		  delete[] bufferSub[i];
	  }
	  delete[] Histcur;
	  delete[] Histcolc;



	  QP = rpcBestCU->getQP(0);
	  // QP_config
	  RD_MSM = rpcBestCU->getTotalCost();
	  SkipFlag = rpcBestCU->getSkipFlag(0);
	  //CbfFlag = rpcBestCU->getQtRootCbf(0);
	  SDCFlag = rpcBestCU->getSDCFlag(0);
	  TotalBits = rpcBestCU->getTotalBits();
	  TotalBins = rpcBestCU->getTotalBins();
	  TotalDistortion = rpcBestCU->getTotalDistortion();
	  POC = rpcBestCU->getSlice()->getPOC();
	  isB_SLICE = rpcBestCU->getSlice()->getSliceType() == B_SLICE ? 1 : 0;

#if PRINT_FEATURE_VALUE
	  // ========================================== feature extraction finish ! ================================================
	  std::cout
		  << "|deltaPOC_t0:" << deltaPOC_t0 << " "
		  << "|SG_t0:" << SG_t0 << " "
		  << "|Dt_t0:" << Dt_t0 << " "
		  << "|Dt_cc_t0:" << Dt_cc_t0 << " "
		  << "|Pt_t0:" << Pt_t0 << " "
		  << "|Pt_cc_t0:" << Pt_cc_t0 << " "
		  << "|MergeFlag_cc_t0:" << MergeFlag_cc_t0 << " "
		  << "|SkipFlag_cc_t0:" << SkipFlag_cc_t0 << " "
		  << "|SDCFlag_cc_t0:" << SDCFlag_cc_t0 << " "
		  << "|DISFlag_cc_t0:" << DISFlag_cc_t0 << " "
		  << "|MSMFlag_cc_t0:" << MSMFlag_cc_t0 << " "
		  << "|deltaPOC_t1:" << deltaPOC_t1 << " "
		  << "|SG_t1:" << SG_t1 << " "
		  << "|Dt_t1:" << Dt_t1 << " "
		  << "|Dt_cc_t1:" << Dt_cc_t1 << " "
		  << "|Pt_t1:" << Pt_t1 << " "
		  << "|Pt_cc_t1:" << Pt_cc_t1 << " "
		  << "|MergeFlag_cc_t1:" << MergeFlag_cc_t1 << " "
		  << "|SkipFlag_cc_t1:" << SkipFlag_cc_t1 << " "
		  << "|SDCFlag_cc_t1:" << SDCFlag_cc_t1 << " "
		  << "|DISFlag_cc_t1:" << DISFlag_cc_t1 << " "
		  << "|MSMFlag_cc_t1:" << MSMFlag_cc_t1 << " "

		  << "|VSG:" << VSG << " "
		  << "|Dv:" << Dv << " "
		  << "|Dv_cc:" << Dv_cc << " "
		  << "|Pv:" << Pv << " "
		  << "|Pv_cc:" << Pv_cc << " "
		  << "|MergeFlag_cc_v:" << MergeFlag_cc_v << " "
		  << "|SkipFlag_cc_v:" << SkipFlag_cc_v << " "
		  << "|SDCFlag_cc_v:" << SDCFlag_cc_v << " "
		  << "|DISFlag_cc_v:" << DISFlag_cc_v << " "
		  << "|MSMFlag_cc_v:" << MSMFlag_cc_v << " "

		  << "|Dc:" << Dc << " "
		  << "|Dc_cc:" << Dc_cc << " "
		  << "|Pc:" << Pc << " "
		  << "|Pc_cc:" << Pc_cc << " "
		  << "|MergeFlag_c:" << MergeFlag_c << " "
		  << "|SkipFlag_c:" << SkipFlag_c << " "
		  << "|SDCFlag_c:" << SDCFlag_c << " "
		  << "|DISFlag_c:" << DISFlag_c << " "
		  << "|MSMFlag_c:" << MSMFlag_c << " "
		  << "|MergeFlag_cc_c:" << MergeFlag_cc_c << " "
		  << "|SkipFlag_cc_c:" << SkipFlag_cc_c << " "
		  << "|SDCFlag_cc_c:" << SDCFlag_cc_c << " "
		  << "|DISFlag_cc_c:" << DISFlag_cc_c << " "
		  << "|MSMFlag_cc_c:" << MSMFlag_cc_c << " "
		  << "|Hc:" << Hc << " "

		  << "|Ds_GAMMA:" << Ds_GAMMA << " "
		  << "|Ds4mean:" << Ds4mean << " "
		  << "|Ps4mean:" << Ps4mean << " "
		  << "|MergeFlag_s:" << MergeFlag_s << " "
		  << "|SkipFlag_s:" << SkipFlag_s << " "
		  << "|SDCFlag_s:" << SDCFlag_s << " "
		  << "|DISFlag_s:" << DISFlag_s << " "
		  << "|MSMFlag_s:" << MSMFlag_s << " "

		  << "|curMean:" << curMean << " "
		  << "|curVar:" << curVar << " "
		  << "|Hcur:" << Hcur << " "
		  << "|curSubmvar:" << curSubmvar << " "
		  << "|Ratio_curSubmvar:" << Ratio_curSubmvar << " "
		  << "|curSubdmean:" << curSubdmean << " "
		  << "|Norm_curSubdmean:" << Norm_curSubdmean << " "
		  << "|curSubdvar:" << curSubdvar << " "
		  << "|Norm_curSubdvar:" << Norm_curSubdvar << " "

		  << "|QP:" << QP << " "
		  << "|QP_config:" << QP_config << " "
		  << "|RD_MSM:" << RD_MSM << " "
		  << "|SkipFlag:" << SkipFlag << " "
		  << "|SDCFlag:" << SDCFlag << " "
		  << "|TotalBins:" << TotalBins << " "
		  << "|TotalBits:" << TotalBits << " "
		  << "|TotalDistortion:" << TotalDistortion << " "
		  << "|POC:" << POC << " "
		  << "|isB_SLICE:" << isB_SLICE << std::endl;
	  // print features
#endif

	  int fileNum = (int)(rpcBestCU->getSlice()->getPOC() / 24);
	  string filename = "fileNum_" + to_string(fileNum) + "_Feature_ViewId_" + to_string(rpcBestCU->getSlice()->getViewId()) + "_Size_" + to_string((int)CU_SIZE) + "_" + to_string((int)time_tag_filename_suffix) + ".txt";
	  ofstream feature;
	  // ofstream feature_Ds;

	  feature.open(filename, ios::app);
	  feature.flags(ios::fixed);
	  feature.precision(4); //设置输出精度
	  feature
		  << deltaPOC_t0 << " "
		  << SG_t0 << " "
		  << Dt_t0 << " "
		  << Dt_cc_t0 << " "
		  << Pt_t0 << " "
		  << Pt_cc_t0 << " "
		  << MergeFlag_cc_t0 << " "
		  << SkipFlag_cc_t0 << " "
		  << SDCFlag_cc_t0 << " "
		  << DISFlag_cc_t0 << " "
		  << MSMFlag_cc_t0 << " "
		  << deltaPOC_t1 << " "
		  << SG_t1 << " "
		  << Dt_t1 << " "
		  << Dt_cc_t1 << " "
		  << Pt_t1 << " "
		  << Pt_cc_t1 << " "
		  << MergeFlag_cc_t1 << " "
		  << SkipFlag_cc_t1 << " "
		  << SDCFlag_cc_t1 << " "
		  << DISFlag_cc_t1 << " "
		  << MSMFlag_cc_t1 << " "

		  << VSG << " "
		  << Dv << " "
		  << Dv_cc << " "
		  << Pv << " "
		  << Pv_cc << " "
		  << MergeFlag_cc_v << " "
		  << SkipFlag_cc_v << " "
		  << SDCFlag_cc_v << " "
		  << DISFlag_cc_v << " "
		  << MSMFlag_cc_v << " "

		  << Dc << " "
		  << Dc_cc << " "
		  << Pc << " "
		  << Pc_cc << " "
		  << MergeFlag_c << " "
		  << SkipFlag_c << " "
		  << SDCFlag_c << " "
		  << DISFlag_c << " "
		  << MSMFlag_c << " "
		  << MergeFlag_cc_c << " "
		  << SkipFlag_cc_c << " "
		  << SDCFlag_cc_c << " "
		  << DISFlag_cc_c << " "
		  << MSMFlag_cc_c << " "
		  << Hc << " "

		  << Ds_GAMMA << " "
		  << Ds4mean << " "
		  << Ps4mean << " "
		  << MergeFlag_s << " "
		  << SkipFlag_s << " "
		  << SDCFlag_s << " "
		  << DISFlag_s << " "
		  << MSMFlag_s << " "

		  << curMean << " "
		  << curVar << " "
		  << Hcur << " "
		  << curSubmvar << " "
		  << Ratio_curSubmvar << " "
		  << curSubdmean << " "
		  << Norm_curSubdmean << " "
		  << curSubdvar << " "
		  << Norm_curSubdvar << " "

		  << QP << " "
		  << QP_config << " "
		  << RD_MSM << " "
		  << SkipFlag << " "
		  << SDCFlag << " "
		  << TotalBins << " "
		  << TotalBits << " "
		  << TotalDistortion << " "
		  << POC << " "
		  << isB_SLICE << " ";
	  
	  feature << endl; // 打印QP_config和Ds
	  feature.close();

      
#if PRINT_TIME
      timeCost += (Double)(clock()-timeTag) / CLOCKS_PER_SEC;
#endif
		}
#endif

#if  NH_3D_FAST_TEXTURE_ENCODING
          bFMD = bIVFMerge && rpcBestCU->isSkipped(0);
#endif

        rpcTempCU->initEstData( uiDepth, iQP, bIsLosslessMode );
#if NH_3D_VSP  || NH_3D_DBBP
        rpcTempCU->setDvInfoSubParts(DvInfo, 0, uiDepth);
#endif

        //if(!m_pcEncCfg->getUseEarlySkipDetection() && !myMSMFlag)
        if(!m_pcEncCfg->getUseEarlySkipDetection())
        {
          // 2Nx2N, NxN
#if NH_3D_IC
            rpcTempCU->setICFlagSubParts(bICFlag, 0, 0, uiDepth);
#endif
#if  NH_3D_FAST_TEXTURE_ENCODING
            xCheckRDCostInter( rpcBestCU, rpcTempCU, SIZE_2Nx2N DEBUG_STRING_PASS_INTO(sDebug), bFMD );  rpcTempCU->initEstData( uiDepth, iQP, bIsLosslessMode );
#else

// #if PRINT_FEATURE
//           if (rpcBestCU->getSlice()->getSliceType() != I_SLICE && uiDepth < 4 && rpcBestCU->getSlice()->getIsDepth() && myFlag[0] && myFlag[1])
//           {
//             // 获取 Inter2Nx2N 模式的RDcost F17:RD_Inter2Nx2N
//             myFlag[3] = true;
//           }
// #endif
          xCheckRDCostInter( rpcBestCU, rpcTempCU, SIZE_2Nx2N DEBUG_STRING_PASS_INTO(sDebug) );
          // myFlag[3] = false;
          rpcTempCU->initEstData( uiDepth, iQP, bIsLosslessMode );
#endif
#if NH_3D_VSP  || NH_3D_DBBP
          rpcTempCU->setDvInfoSubParts(DvInfo, 0, uiDepth);
#endif
#if NH_3D_DBBP
            if( rpcTempCU->getSlice()->getDepthBasedBlkPartFlag() && rpcTempCU->getSlice()->getDefaultRefViewIdxAvailableFlag() )
            {
              xCheckRDCostInterDBBP( rpcBestCU, rpcTempCU DEBUG_STRING_PASS_INTO(sDebug), false );
              rpcTempCU->initEstData( uiDepth, iQP, bIsLosslessMode  );
#if NH_3D_VSP  || NH_3D_DBBP
              rpcTempCU->setDvInfoSubParts(DvInfo, 0, uiDepth);
#endif
            }
#endif

          if(m_pcEncCfg->getUseCbfFastMode())
          {
            doNotBlockPu = rpcBestCU->getQtRootCbf( 0 ) != 0;
          }
        }
#if NH_3D_IC
        }
#endif
      }
#if NH_3D_QTL
      if(depthMapDetect && !bIntraSliceDetect && !rapPic && ( m_pcEncCfg->getUseQTL() || bLimQtPredFalg ))
      {
        bTrySplitDQP = bTrySplit;
      }
#endif

      if (bIsLosslessMode) // Restore loop variable if lossless mode was searched.
      {
        iQP = iMinQP;
      }
    }

#if KWU_RC_MADPRED_E0227
    if ( uiDepth <= m_addSADDepth )
    {
      m_LCUPredictionSAD += m_temporalSAD;
      m_addSADDepth = uiDepth;
    }
#endif
#if NH_3D_ENC_DEPTH
    if( rpcBestCU->getSlice()->getIsDepth() && rpcBestCU->getSlice()->isIRAP() )
    {
      earlyDetectionSkipMode = false;
    }
#endif
#if NH_3D_DIS
    rpcTempCU->initEstData( uiDepth, iMinQP, isAddLowestQP  );
    //if( rpcBestCU->getSlice()->getDepthIntraSkipFlag() && !myMSMFlag)
    if( rpcBestCU->getSlice()->getDepthIntraSkipFlag())
    {
// #if PRINT_FEATURE
//       if (rpcBestCU->getSlice()->getSliceType() != I_SLICE && uiDepth < 4 && rpcBestCU->getSlice()->getIsDepth() && myFlag[0] && myFlag[1])
//       {
//         // 获取 DIS 模式的RDcost F15:RD_DIS
//         myFlag[2] = true;
//       }
// #endif

      xCheckRDCostDIS( rpcBestCU, rpcTempCU, SIZE_2Nx2N DEBUG_STRING_PASS_INTO(sDebug) );
	    // myFlag[2] = false; // 使用完立即设置为false

      rpcTempCU->initEstData( uiDepth, iMinQP, isAddLowestQP  );
    }
#endif

    //if(!earlyDetectionSkipMode && !myMSMFlag)
    if(!earlyDetectionSkipMode)
    {
      for (Int iQP=iMinQP; iQP<=iMaxQP; iQP++)
      {
        const Bool bIsLosslessMode = isAddLowestQP && (iQP == iMinQP); // If lossless, then iQP is irrelevant for subsequent modules.

        if (bIsLosslessMode)
        {
          iQP = lowestQP;
        }

        rpcTempCU->initEstData( uiDepth, iQP, bIsLosslessMode );

        // do inter modes, NxN, 2NxN, and Nx2N
        if( rpcBestCU->getSlice()->getSliceType() != I_SLICE )
        {
          // 2Nx2N, NxN

          if(!( (rpcBestCU->getWidth(0)==8) && (rpcBestCU->getHeight(0)==8) ))
          {
            if( uiDepth == sps.getLog2DiffMaxMinCodingBlockSize() && doNotBlockPu
#if NH_3D_QTL
                && bTrySplit
#endif
)
            {
#if  NH_3D_FAST_TEXTURE_ENCODING
              xCheckRDCostInter( rpcBestCU, rpcTempCU, SIZE_NxN DEBUG_STRING_PASS_INTO(sDebug), bFMD  );
#else

              xCheckRDCostInter( rpcBestCU, rpcTempCU, SIZE_NxN DEBUG_STRING_PASS_INTO(sDebug)   );
#endif
              rpcTempCU->initEstData( uiDepth, iQP, bIsLosslessMode );
#if NH_3D_VSP || NH_3D_DBBP
              rpcTempCU->setDvInfoSubParts(DvInfo, 0, uiDepth);
#endif

            }
          }

          if(doNotBlockPu
#if NH_3D_QTL
            && bTryNx2N
#endif
)
          {
#if  NH_3D_FAST_TEXTURE_ENCODING
            xCheckRDCostInter( rpcBestCU, rpcTempCU, SIZE_Nx2N DEBUG_STRING_PASS_INTO(sDebug), bFMD  );
#else
            xCheckRDCostInter( rpcBestCU, rpcTempCU, SIZE_Nx2N DEBUG_STRING_PASS_INTO(sDebug)  );
#endif
            rpcTempCU->initEstData( uiDepth, iQP, bIsLosslessMode );
#if NH_3D_VSP || NH_3D_DBBP
            rpcTempCU->setDvInfoSubParts(DvInfo, 0, uiDepth);
#endif

            if(m_pcEncCfg->getUseCbfFastMode() && rpcBestCU->getPartitionSize(0) == SIZE_Nx2N )
            {
              doNotBlockPu = rpcBestCU->getQtRootCbf( 0 ) != 0;
            }
          }
          if(doNotBlockPu
#if NH_3D_QTL
            && bTry2NxN
#endif
)
          {
#if  NH_3D_FAST_TEXTURE_ENCODING
            xCheckRDCostInter      ( rpcBestCU, rpcTempCU, SIZE_2NxN DEBUG_STRING_PASS_INTO(sDebug), bFMD  );
#else

            xCheckRDCostInter      ( rpcBestCU, rpcTempCU, SIZE_2NxN DEBUG_STRING_PASS_INTO(sDebug)  );
#endif

            rpcTempCU->initEstData( uiDepth, iQP, bIsLosslessMode );
#if NH_3D_VSP || NH_3D_DBBP
            rpcTempCU->setDvInfoSubParts(DvInfo, 0, uiDepth);
#endif

            if(m_pcEncCfg->getUseCbfFastMode() && rpcBestCU->getPartitionSize(0) == SIZE_2NxN)
            {
              doNotBlockPu = rpcBestCU->getQtRootCbf( 0 ) != 0;
            }
          }

          //! Try AMP (SIZE_2NxnU, SIZE_2NxnD, SIZE_nLx2N, SIZE_nRx2N)
          if(sps.getUseAMP() && uiDepth < sps.getLog2DiffMaxMinCodingBlockSize() )
          {
#if AMP_ENC_SPEEDUP
            Bool bTestAMP_Hor = false, bTestAMP_Ver = false;

#if AMP_MRG
            Bool bTestMergeAMP_Hor = false, bTestMergeAMP_Ver = false;

            deriveTestModeAMP (rpcBestCU, eParentPartSize, bTestAMP_Hor, bTestAMP_Ver, bTestMergeAMP_Hor, bTestMergeAMP_Ver);
#else
            deriveTestModeAMP (rpcBestCU, eParentPartSize, bTestAMP_Hor, bTestAMP_Ver);
#endif

            //! Do horizontal AMP
            if ( bTestAMP_Hor )
            {
              if(doNotBlockPu
#if NH_3D_QTL
                && bTry2NxN
#endif
)
              {
#if  NH_3D_FAST_TEXTURE_ENCODING
                xCheckRDCostInter( rpcBestCU, rpcTempCU, SIZE_2NxnU DEBUG_STRING_PASS_INTO(sDebug), bFMD );
#else
                xCheckRDCostInter( rpcBestCU, rpcTempCU, SIZE_2NxnU DEBUG_STRING_PASS_INTO(sDebug) );
#endif
                rpcTempCU->initEstData( uiDepth, iQP, bIsLosslessMode );
#if NH_3D_VSP || NH_3D_DBBP
                rpcTempCU->setDvInfoSubParts(DvInfo, 0, uiDepth);
#endif
                if(m_pcEncCfg->getUseCbfFastMode() && rpcBestCU->getPartitionSize(0) == SIZE_2NxnU )
                {
                  doNotBlockPu = rpcBestCU->getQtRootCbf( 0 ) != 0;
                }
              }
              if(doNotBlockPu
#if NH_3D_QTL
                && bTry2NxN
#endif
)
              {
#if  NH_3D_FAST_TEXTURE_ENCODING
                xCheckRDCostInter( rpcBestCU, rpcTempCU, SIZE_2NxnD DEBUG_STRING_PASS_INTO(sDebug), bFMD );
#else
                xCheckRDCostInter( rpcBestCU, rpcTempCU, SIZE_2NxnD DEBUG_STRING_PASS_INTO(sDebug) );
#endif

                rpcTempCU->initEstData( uiDepth, iQP, bIsLosslessMode );
#if NH_3D_VSP || NH_3D_DBBP
                rpcTempCU->setDvInfoSubParts(DvInfo, 0, uiDepth);
#endif

                if(m_pcEncCfg->getUseCbfFastMode() && rpcBestCU->getPartitionSize(0) == SIZE_2NxnD )
                {
                  doNotBlockPu = rpcBestCU->getQtRootCbf( 0 ) != 0;
                }
              }
            }
#if AMP_MRG
            else if ( bTestMergeAMP_Hor )
            {
              if(doNotBlockPu
#if NH_3D_QTL
                && bTry2NxN
#endif
)
              {
#if  NH_3D_FAST_TEXTURE_ENCODING
                xCheckRDCostInter( rpcBestCU, rpcTempCU, SIZE_2NxnU DEBUG_STRING_PASS_INTO(sDebug), bFMD, true );
#else

                xCheckRDCostInter( rpcBestCU, rpcTempCU, SIZE_2NxnU DEBUG_STRING_PASS_INTO(sDebug), true );
#endif

                rpcTempCU->initEstData( uiDepth, iQP, bIsLosslessMode );
#if NH_3D_VSP || NH_3D_DBBP
                rpcTempCU->setDvInfoSubParts(DvInfo, 0, uiDepth);
#endif
                if(m_pcEncCfg->getUseCbfFastMode() && rpcBestCU->getPartitionSize(0) == SIZE_2NxnU )
                {
                  doNotBlockPu = rpcBestCU->getQtRootCbf( 0 ) != 0;
                }
              }
              if(doNotBlockPu
#if NH_3D_QTL
                && bTry2NxN
#endif
)
              {
#if  NH_3D_FAST_TEXTURE_ENCODING
                xCheckRDCostInter( rpcBestCU, rpcTempCU, SIZE_2NxnD DEBUG_STRING_PASS_INTO(sDebug), bFMD, true );
#else
                xCheckRDCostInter( rpcBestCU, rpcTempCU, SIZE_2NxnD DEBUG_STRING_PASS_INTO(sDebug), true );
#endif
                rpcTempCU->initEstData( uiDepth, iQP, bIsLosslessMode );
#if NH_3D_VSP || NH_3D_DBBP
                rpcTempCU->setDvInfoSubParts(DvInfo, 0, uiDepth);
#endif

                if(m_pcEncCfg->getUseCbfFastMode() && rpcBestCU->getPartitionSize(0) == SIZE_2NxnD )
                {
                  doNotBlockPu = rpcBestCU->getQtRootCbf( 0 ) != 0;
                }
              }
            }
#endif

            //! Do horizontal AMP
            if ( bTestAMP_Ver )
            {
              if(doNotBlockPu
#if NH_3D_QTL
                && bTryNx2N
#endif
)
              {
#if  NH_3D_FAST_TEXTURE_ENCODING
                xCheckRDCostInter( rpcBestCU, rpcTempCU, SIZE_nLx2N DEBUG_STRING_PASS_INTO(sDebug), bFMD );
#else
                xCheckRDCostInter( rpcBestCU, rpcTempCU, SIZE_nLx2N DEBUG_STRING_PASS_INTO(sDebug) );
#endif

                rpcTempCU->initEstData( uiDepth, iQP, bIsLosslessMode );
#if NH_3D_VSP || NH_3D_DBBP
                rpcTempCU->setDvInfoSubParts(DvInfo, 0, uiDepth);
#endif
                if(m_pcEncCfg->getUseCbfFastMode() && rpcBestCU->getPartitionSize(0) == SIZE_nLx2N )
                {
                  doNotBlockPu = rpcBestCU->getQtRootCbf( 0 ) != 0;
                }
              }
              if(doNotBlockPu
#if NH_3D_QTL
                && bTryNx2N
#endif
)
              {
#if  NH_3D_FAST_TEXTURE_ENCODING
                xCheckRDCostInter( rpcBestCU, rpcTempCU, SIZE_nRx2N DEBUG_STRING_PASS_INTO(sDebug), bFMD );
#else
                xCheckRDCostInter( rpcBestCU, rpcTempCU, SIZE_nRx2N DEBUG_STRING_PASS_INTO(sDebug) );
#endif
                rpcTempCU->initEstData( uiDepth, iQP, bIsLosslessMode );
#if NH_3D_VSP || NH_3D_DBBP
                rpcTempCU->setDvInfoSubParts(DvInfo, 0, uiDepth);
#endif
              }
            }
#if AMP_MRG
            else if ( bTestMergeAMP_Ver )
            {
              if(doNotBlockPu
#if NH_3D_QTL
                && bTryNx2N
#endif
)
              {
#if  NH_3D_FAST_TEXTURE_ENCODING
                xCheckRDCostInter( rpcBestCU, rpcTempCU, SIZE_nLx2N DEBUG_STRING_PASS_INTO(sDebug), bFMD, true );
#else
                xCheckRDCostInter( rpcBestCU, rpcTempCU, SIZE_nLx2N DEBUG_STRING_PASS_INTO(sDebug), true );
#endif
                rpcTempCU->initEstData( uiDepth, iQP, bIsLosslessMode );
#if NH_3D_VSP || NH_3D_DBBP
                rpcTempCU->setDvInfoSubParts(DvInfo, 0, uiDepth);
#endif
                if(m_pcEncCfg->getUseCbfFastMode() && rpcBestCU->getPartitionSize(0) == SIZE_nLx2N )
                {
                  doNotBlockPu = rpcBestCU->getQtRootCbf( 0 ) != 0;
                }
              }
              if(doNotBlockPu
#if NH_3D_QTL
                && bTryNx2N
#endif
)
              {
#if  NH_3D_FAST_TEXTURE_ENCODING
                xCheckRDCostInter( rpcBestCU, rpcTempCU, SIZE_nRx2N DEBUG_STRING_PASS_INTO(sDebug), bFMD, true );
#else

                xCheckRDCostInter( rpcBestCU, rpcTempCU, SIZE_nRx2N DEBUG_STRING_PASS_INTO(sDebug), true );
#endif
                rpcTempCU->initEstData( uiDepth, iQP, bIsLosslessMode );
#if NH_3D_VSP || NH_3D_DBBP
                rpcTempCU->setDvInfoSubParts(DvInfo, 0, uiDepth);
#endif

              }
            }
#endif

#else
#if NH_3D_QTL
            if (bTry2NxN)
            {
#endif

            xCheckRDCostInter( rpcBestCU, rpcTempCU, SIZE_2NxnU );
            rpcTempCU->initEstData( uiDepth, iQP, bIsLosslessMode );
#if NH_3D_VSP || NH_3D_DBBP
            rpcTempCU->setDvInfoSubParts(DvInfo, 0, uiDepth);
#endif
            xCheckRDCostInter( rpcBestCU, rpcTempCU, SIZE_2NxnD );
            rpcTempCU->initEstData( uiDepth, iQP, bIsLosslessMode );
#if NH_3D_VSP || NH_3D_DBBP
            rpcTempCU->setDvInfoSubParts(DvInfo, 0, uiDepth);
#endif
#if NH_3D_QTL
            }
            if (bTryNx2N)
            {
#endif
            xCheckRDCostInter( rpcBestCU, rpcTempCU, SIZE_nLx2N );
            rpcTempCU->initEstData( uiDepth, iQP, bIsLosslessMode );
#if NH_3D_VSP || NH_3D_DBBP
            rpcTempCU->setDvInfoSubParts(DvInfo, 0, uiDepth);
#endif
            xCheckRDCostInter( rpcBestCU, rpcTempCU, SIZE_nRx2N );
            rpcTempCU->initEstData( uiDepth, iQP, bIsLosslessMode );
#if NH_3D_VSP || NH_3D_DBBP
            rpcTempCU->setDvInfoSubParts(DvInfo, 0, uiDepth);
#endif
#if NH_3D_QTL
            }
#endif


#endif
          }
        }
#if  NH_3D_FAST_TEXTURE_ENCODING
        if(!bFMD)
        {
#endif

        // do normal intra modes
        // speedup for inter frames
        Double intraCost = 0.0;

        if((rpcBestCU->getSlice()->getSliceType() == I_SLICE)                                     ||
           ((!m_pcEncCfg->getDisableIntraPUsInInterSlices()) && (
           (rpcBestCU->getCbf( 0, COMPONENT_Y  ) != 0)                                            ||
          ((rpcBestCU->getCbf( 0, COMPONENT_Cb ) != 0) && (numberValidComponents > COMPONENT_Cb)) ||
          ((rpcBestCU->getCbf( 0, COMPONENT_Cr ) != 0) && (numberValidComponents > COMPONENT_Cr))   // avoid very complex intra if it is unlikely
 #if NH_3D_ENC_DEPTH
            || rpcBestCU->getSlice()->getIsDepth()
#endif
            )))
        {
#if NH_3D_ENC_DEPTH
            Bool bOnlyIVP = false;
            Bool bUseIVP = true;
            if( (rpcBestCU->getSlice()->getSliceType() != I_SLICE) && 
                !( (rpcBestCU->getCbf( 0, COMPONENT_Y  ) != 0)                                            ||
                  ((rpcBestCU->getCbf( 0, COMPONENT_Cb ) != 0) && (numberValidComponents > COMPONENT_Cb)) ||
                  ((rpcBestCU->getCbf( 0, COMPONENT_Cr ) != 0) && (numberValidComponents > COMPONENT_Cr))   ) &&
                  (rpcBestCU->getSlice()->getIsDepth() && !(rpcBestCU->getSlice()->isIRAP())) )
            { 
              bOnlyIVP = true;
              bUseIVP = rpcBestCU->getSlice()->getIntraContourFlag();
            }
            if( bUseIVP )
            {
              xCheckRDCostIntra( rpcBestCU, rpcTempCU, intraCost, SIZE_2Nx2N DEBUG_STRING_PASS_INTO(sDebug), bOnlyIVP );
#else
          xCheckRDCostIntra( rpcBestCU, rpcTempCU, intraCost, SIZE_2Nx2N DEBUG_STRING_PASS_INTO(sDebug) );
#endif
#if KWU_RC_MADPRED_E0227
            if ( uiDepth <= m_addSADDepth )
            {
              m_LCUPredictionSAD += m_spatialSAD;
              m_addSADDepth = uiDepth;
            }
#endif

          rpcTempCU->initEstData( uiDepth, iQP, bIsLosslessMode );
          if( uiDepth == sps.getLog2DiffMaxMinCodingBlockSize() )
          {
#if NH_3D_QTL //Try IntraNxN
              if(bTrySplit)
              {
#endif
            if( rpcTempCU->getWidth(0) > ( 1 << sps.getQuadtreeTULog2MinSize() ) )
            {
              Double tmpIntraCost;
#if NH_3D_ENC_DEPTH
              xCheckRDCostIntra( rpcBestCU, rpcTempCU, tmpIntraCost, SIZE_NxN DEBUG_STRING_PASS_INTO(sDebug), bOnlyIVP );
#else
              xCheckRDCostIntra( rpcBestCU, rpcTempCU, tmpIntraCost, SIZE_NxN DEBUG_STRING_PASS_INTO(sDebug)   );
#endif

              intraCost = std::min(intraCost, tmpIntraCost);
              rpcTempCU->initEstData( uiDepth, iQP, bIsLosslessMode );
            }
#if NH_3D_QTL
              }
#endif
          }

// #if PRINT_FEATURE
//           if (rpcBestCU->getSlice()->getSliceType() != I_SLICE && uiDepth < 4 && rpcBestCU->getSlice()->getIsDepth() && myFlag[0] && myFlag[1])
//           {
//             // 帧内预测的RDcost F18:RD_Intra
//             features[18] = intraCost;
            
//           }
// #endif

#if NH_3D_ENC_DEPTH
          }
#endif
        }

        // test PCM
        if(sps.getUsePCM()
          && rpcTempCU->getWidth(0) <= (1<<sps.getPCMLog2MaxSize())
          && rpcTempCU->getWidth(0) >= (1<<sps.getPCMLog2MinSize()) )
        {
          UInt uiRawBits = getTotalBits(rpcBestCU->getWidth(0), rpcBestCU->getHeight(0), rpcBestCU->getPic()->getChromaFormat(), sps.getBitDepths().recon);
          UInt uiBestBits = rpcBestCU->getTotalBits();
#if NH_3D_VSO // M7
          Double dRDCostTemp = m_pcRdCost->getUseLambdaScaleVSO() ? m_pcRdCost->calcRdCostVSO(uiRawBits, 0) : m_pcRdCost->calcRdCost(uiRawBits, 0);
          if((uiBestBits > uiRawBits) || (rpcBestCU->getTotalCost() > dRDCostTemp ))
#else
          if((uiBestBits > uiRawBits) || (rpcBestCU->getTotalCost() > m_pcRdCost->calcRdCost(uiRawBits, 0)))
#endif

          {
            xCheckIntraPCM (rpcBestCU, rpcTempCU);
            rpcTempCU->initEstData( uiDepth, iQP, bIsLosslessMode );
          }
        }
#if  NH_3D_FAST_TEXTURE_ENCODING
        }
#endif
        if (bIsLosslessMode) // Restore loop variable if lossless mode was searched.
        {
          iQP = iMinQP;
        }
      }
    }

    if( rpcBestCU->getTotalCost()!=MAX_DOUBLE )
    {
      m_pcRDGoOnSbacCoder->load(m_pppcRDSbacCoder[uiDepth][CI_NEXT_BEST]);
      m_pcEntropyCoder->resetBits();
      m_pcEntropyCoder->encodeSplitFlag( rpcBestCU, 0, uiDepth, true );
      rpcBestCU->getTotalBits() += m_pcEntropyCoder->getNumberOfWrittenBits(); // split bits
      rpcBestCU->getTotalBins() += ((TEncBinCABAC *)((TEncSbac*)m_pcEntropyCoder->m_pcEntropyCoderIf)->getEncBinIf())->getBinsCoded();
#if NH_3D_VSO // M8
    if ( m_pcRdCost->getUseVSO() )    
    {
      rpcBestCU->getTotalCost()  = m_pcRdCost->calcRdCostVSO( rpcBestCU->getTotalBits(), rpcBestCU->getTotalDistortion() );    
    }
    else
#endif
      rpcBestCU->getTotalCost()  = m_pcRdCost->calcRdCost( rpcBestCU->getTotalBits(), rpcBestCU->getTotalDistortion() );
      m_pcRDGoOnSbacCoder->store(m_pppcRDSbacCoder[uiDepth][CI_NEXT_BEST]);
    }
#if  NH_3D_FAST_TEXTURE_ENCODING
    if(rpcBestCU->getSlice()->getViewIndex() && !rpcBestCU->getSlice()->getIsDepth() && (uiDepth >=iIVFMaxD) && rpcBestCU->isSkipped(0))
    {
      bSubBranch = false;
    }
#endif
  }

  // copy original YUV samples to PCM buffer
  if( rpcBestCU->getTotalCost()!=MAX_DOUBLE && rpcBestCU->isLosslessCoded(0) && (rpcBestCU->getIPCMFlag(0) == false))
  {
    xFillPCMBuffer(rpcBestCU, m_ppcOrigYuv[uiDepth]);
  }

  if( uiDepth == pps.getMaxCuDQPDepth() )
  {
    Int idQP = m_pcEncCfg->getMaxDeltaQP();
    iMinQP = Clip3( -sps.getQpBDOffset(CHANNEL_TYPE_LUMA), MAX_QP, iBaseQP-idQP );
    iMaxQP = Clip3( -sps.getQpBDOffset(CHANNEL_TYPE_LUMA), MAX_QP, iBaseQP+idQP );
  }
  else if( uiDepth < pps.getMaxCuDQPDepth() )
  {
    iMinQP = iBaseQP;
    iMaxQP = iBaseQP;
  }
  else
  {
    const Int iStartQP = rpcTempCU->getQP(0);
    iMinQP = iStartQP;
    iMaxQP = iStartQP;
  }

  if ( m_pcEncCfg->getUseRateCtrl() )
  {
    iMinQP = m_pcRateCtrl->getRCQP();
    iMaxQP = m_pcRateCtrl->getRCQP();
  }

  if ( m_pcEncCfg->getCUTransquantBypassFlagForceValue() )
  {
    iMaxQP = iMinQP; // If all TUs are forced into using transquant bypass, do not loop here.
  }
#if  NH_3D_FAST_TEXTURE_ENCODING
  bSubBranch = bSubBranch && (bBoundary || !( m_pcEncCfg->getUseEarlyCU() && rpcBestCU->getTotalCost()!=MAX_DOUBLE && rpcBestCU->isSkipped(0) ));
#else
  const Bool bSubBranch = bBoundary || !( m_pcEncCfg->getUseEarlyCU() && rpcBestCU->getTotalCost()!=MAX_DOUBLE && rpcBestCU->isSkipped(0) );
#endif


#if NH_3D_QTL
  if( bSubBranch && uiDepth < sps.getLog2DiffMaxMinCodingBlockSize() && (!getFastDeltaQp() || uiWidth > fastDeltaQPCuMaxSize || bBoundary) && bTrySplitDQP )
#else
  if( bSubBranch && uiDepth < sps.getLog2DiffMaxMinCodingBlockSize() && (!getFastDeltaQp() || uiWidth > fastDeltaQPCuMaxSize || bBoundary))
#endif
  {
    // further split
    for (Int iQP=iMinQP; iQP<=iMaxQP; iQP++)
    {
      const Bool bIsLosslessMode = false; // False at this level. Next level down may set it to true.

      rpcTempCU->initEstData( uiDepth, iQP, bIsLosslessMode );

#if NH_3D_VSO // M9
      // reset Model
      if( m_pcRdCost->getUseRenModel() )
      {
        UInt  uiWidthOy     = m_ppcOrigYuv[uiDepth]->getWidth ( COMPONENT_Y );
        UInt  uiHeightOy    = m_ppcOrigYuv[uiDepth]->getHeight( COMPONENT_Y );
        Pel*  piSrc         = m_ppcOrigYuv[uiDepth]->getAddr  ( COMPONENT_Y, 0 );
        UInt  uiSrcStride   = m_ppcOrigYuv[uiDepth]->getStride( COMPONENT_Y  );
        m_pcRdCost->setRenModelData( m_ppcBestCU[uiDepth], 0, piSrc, uiSrcStride, uiWidthOy, uiHeightOy );
      }
#endif
      UChar       uhNextDepth         = uiDepth+1;
      TComDataCU* pcSubBestPartCU     = m_ppcBestCU[uhNextDepth];
      TComDataCU* pcSubTempPartCU     = m_ppcTempCU[uhNextDepth];
      DEBUG_STRING_NEW(sTempDebug)

#if NH_3D_ARP
      m_ppcWeightedTempCU[uhNextDepth]->setSlice( m_ppcWeightedTempCU[ uiDepth]->getSlice()); 
      m_ppcWeightedTempCU[uhNextDepth]->setPic  ( m_ppcWeightedTempCU[ uiDepth] ); 
#endif
      for ( UInt uiPartUnitIdx = 0; uiPartUnitIdx < 4; uiPartUnitIdx++ )
      {
        pcSubBestPartCU->initSubCU( rpcTempCU, uiPartUnitIdx, uhNextDepth, iQP );           // clear sub partition datas or init.
        pcSubTempPartCU->initSubCU( rpcTempCU, uiPartUnitIdx, uhNextDepth, iQP );           // clear sub partition datas or init.

        if( ( pcSubBestPartCU->getCUPelX() < sps.getPicWidthInLumaSamples() ) && ( pcSubBestPartCU->getCUPelY() < sps.getPicHeightInLumaSamples() ) )
        {
          if ( 0 == uiPartUnitIdx) //initialize RD with previous depth buffer
          {
            m_pppcRDSbacCoder[uhNextDepth][CI_CURR_BEST]->load(m_pppcRDSbacCoder[uiDepth][CI_CURR_BEST]);
          }
          else
          {
            m_pppcRDSbacCoder[uhNextDepth][CI_CURR_BEST]->load(m_pppcRDSbacCoder[uhNextDepth][CI_NEXT_BEST]);
          }

#if AMP_ENC_SPEEDUP
          DEBUG_STRING_NEW(sChild)
          if ( !(rpcBestCU->getTotalCost()!=MAX_DOUBLE && rpcBestCU->isInter(0)) )
          {
            xCompressCU( pcSubBestPartCU, pcSubTempPartCU, uhNextDepth DEBUG_STRING_PASS_INTO(sChild), NUMBER_OF_PART_SIZES );
          }
          else
          {

            xCompressCU( pcSubBestPartCU, pcSubTempPartCU, uhNextDepth DEBUG_STRING_PASS_INTO(sChild), rpcBestCU->getPartitionSize(0) );
          }
          DEBUG_STRING_APPEND(sTempDebug, sChild)
#else
          xCompressCU( pcSubBestPartCU, pcSubTempPartCU, uhNextDepth );
#endif

          rpcTempCU->copyPartFrom( pcSubBestPartCU, uiPartUnitIdx, uhNextDepth );         // Keep best part data to current temporary data.
          xCopyYuv2Tmp( pcSubBestPartCU->getTotalNumPart()*uiPartUnitIdx, uhNextDepth );
        }
        else
        {
          pcSubBestPartCU->copyToPic( uhNextDepth );
          rpcTempCU->copyPartFrom( pcSubBestPartCU, uiPartUnitIdx, uhNextDepth );
        }
      }

      m_pcRDGoOnSbacCoder->load(m_pppcRDSbacCoder[uhNextDepth][CI_NEXT_BEST]);
      if( !bBoundary )
      {
        m_pcEntropyCoder->resetBits();
        m_pcEntropyCoder->encodeSplitFlag( rpcTempCU, 0, uiDepth, true );

        rpcTempCU->getTotalBits() += m_pcEntropyCoder->getNumberOfWrittenBits(); // split bits
        rpcTempCU->getTotalBins() += ((TEncBinCABAC *)((TEncSbac*)m_pcEntropyCoder->m_pcEntropyCoderIf)->getEncBinIf())->getBinsCoded();
      }
#if NH_3D_VSO // M10
      if ( m_pcRdCost->getUseVSO() )
      {
        rpcTempCU->getTotalCost()  = m_pcRdCost->calcRdCostVSO( rpcTempCU->getTotalBits(), rpcTempCU->getTotalDistortion() );
      }
      else
#endif
        rpcTempCU->getTotalCost()  = m_pcRdCost->calcRdCost( rpcTempCU->getTotalBits(), rpcTempCU->getTotalDistortion() );

      if( uiDepth == pps.getMaxCuDQPDepth() && pps.getUseDQP())
      {
        Bool hasResidual = false;
        for( UInt uiBlkIdx = 0; uiBlkIdx < rpcTempCU->getTotalNumPart(); uiBlkIdx ++)
        {
          if( (     rpcTempCU->getCbf(uiBlkIdx, COMPONENT_Y)
                || (rpcTempCU->getCbf(uiBlkIdx, COMPONENT_Cb) && (numberValidComponents > COMPONENT_Cb))
                || (rpcTempCU->getCbf(uiBlkIdx, COMPONENT_Cr) && (numberValidComponents > COMPONENT_Cr)) ) )
          {
            hasResidual = true;
            break;
          }
        }

        if ( hasResidual )
        {
          m_pcEntropyCoder->resetBits();
          m_pcEntropyCoder->encodeQP( rpcTempCU, 0, false );
          rpcTempCU->getTotalBits() += m_pcEntropyCoder->getNumberOfWrittenBits(); // dQP bits
          rpcTempCU->getTotalBins() += ((TEncBinCABAC *)((TEncSbac*)m_pcEntropyCoder->m_pcEntropyCoderIf)->getEncBinIf())->getBinsCoded();
#if NH_3D_VSO // M11
          if ( m_pcRdCost->getUseLambdaScaleVSO())          
          {
            rpcTempCU->getTotalCost()  = m_pcRdCost->calcRdCostVSO( rpcTempCU->getTotalBits(), rpcTempCU->getTotalDistortion() );          
          }
          else
#endif
            rpcTempCU->getTotalCost()  = m_pcRdCost->calcRdCost( rpcTempCU->getTotalBits(), rpcTempCU->getTotalDistortion() );

          Bool foundNonZeroCbf = false;
          rpcTempCU->setQPSubCUs( rpcTempCU->getRefQP( 0 ), 0, uiDepth, foundNonZeroCbf );
          assert( foundNonZeroCbf );
        }
        else
        {
          rpcTempCU->setQPSubParts( rpcTempCU->getRefQP( 0 ), 0, uiDepth ); // set QP to default QP
        }
      }

      m_pcRDGoOnSbacCoder->store(m_pppcRDSbacCoder[uiDepth][CI_TEMP_BEST]);

      // If the configuration being tested exceeds the maximum number of bytes for a slice / slice-segment, then
      // a proper RD evaluation cannot be performed. Therefore, termination of the
      // slice/slice-segment must be made prior to this CTU.
      // This can be achieved by forcing the decision to be that of the rpcTempCU.
      // The exception is each slice / slice-segment must have at least one CTU.
      if (rpcBestCU->getTotalCost()!=MAX_DOUBLE)
      {
        const Bool isEndOfSlice        =    pcSlice->getSliceMode()==FIXED_NUMBER_OF_BYTES
                                         && ((pcSlice->getSliceBits()+rpcBestCU->getTotalBits())>pcSlice->getSliceArgument()<<3)
                                         && rpcBestCU->getCtuRsAddr() != pcPic->getPicSym()->getCtuTsToRsAddrMap(pcSlice->getSliceCurStartCtuTsAddr())
                                         && rpcBestCU->getCtuRsAddr() != pcPic->getPicSym()->getCtuTsToRsAddrMap(pcSlice->getSliceSegmentCurStartCtuTsAddr());
        const Bool isEndOfSliceSegment =    pcSlice->getSliceSegmentMode()==FIXED_NUMBER_OF_BYTES
                                         && ((pcSlice->getSliceSegmentBits()+rpcBestCU->getTotalBits()) > pcSlice->getSliceSegmentArgument()<<3)
                                         && rpcBestCU->getCtuRsAddr() != pcPic->getPicSym()->getCtuTsToRsAddrMap(pcSlice->getSliceSegmentCurStartCtuTsAddr());
                                             // Do not need to check slice condition for slice-segment since a slice-segment is a subset of a slice.
        if(isEndOfSlice||isEndOfSliceSegment)
        {
          rpcBestCU->getTotalCost()=MAX_DOUBLE;
        }
      }

      xCheckBestMode( rpcBestCU, rpcTempCU, uiDepth DEBUG_STRING_PASS_INTO(sDebug) DEBUG_STRING_PASS_INTO(sTempDebug) DEBUG_STRING_PASS_INTO(false) ); // RD compare current larger prediction
                                                                                                                                                       // with sub partitioned prediction.
    }
  }
#if NH_3D_VSO // M12 
  if( m_pcRdCost->getUseRenModel() )
  {
    UInt  uiWidthRy     = m_ppcRecoYuvBest[uiDepth]->getWidth   ( COMPONENT_Y );
    UInt  uiHeightRy    = m_ppcRecoYuvBest[uiDepth]->getHeight  ( COMPONENT_Y );
    Pel*  piSrc       = m_ppcRecoYuvBest[uiDepth]->getAddr    ( COMPONENT_Y,  0 );
    UInt  uiSrcStride = m_ppcRecoYuvBest[uiDepth]->getStride  ( COMPONENT_Y );
    m_pcRdCost->setRenModelData( rpcBestCU, 0, piSrc, uiSrcStride, uiWidthRy, uiHeightRy );
  }
#endif

  DEBUG_STRING_APPEND(sDebug_, sDebug);

  rpcBestCU->copyToPic(uiDepth);                                                     // Copy Best data to Picture for next partition prediction.

  xCopyYuv2Pic( rpcBestCU->getPic(), rpcBestCU->getCtuRsAddr(), rpcBestCU->getZorderIdxInCtu(), uiDepth, uiDepth );   // Copy Yuv data to picture Yuv
  if (bBoundary)
  {
    return;
  }

  // Assert if Best prediction mode is NONE
  // Selected mode's RD-cost must be not MAX_DOUBLE.
  assert( rpcBestCU->getPartitionSize ( 0 ) != NUMBER_OF_PART_SIZES       );
  assert( rpcBestCU->getPredictionMode( 0 ) != NUMBER_OF_PREDICTION_MODES );
  assert( rpcBestCU->getTotalCost     (   ) != MAX_DOUBLE                 );
}

/** finish encoding a cu and handle end-of-slice conditions
 * \param pcCU
 * \param uiAbsPartIdx
 * \param uiDepth
 * \returns Void
 */
Void TEncCu::finishCU( TComDataCU* pcCU, UInt uiAbsPartIdx )
{
  TComPic* pcPic = pcCU->getPic();
  TComSlice * pcSlice = pcCU->getPic()->getSlice(pcCU->getPic()->getCurrSliceIdx());

  //Calculate end address
  const Int  currentCTUTsAddr = pcPic->getPicSym()->getCtuRsToTsAddrMap(pcCU->getCtuRsAddr());
  const Bool isLastSubCUOfCtu = pcCU->isLastSubCUOfCtu(uiAbsPartIdx);
  if ( isLastSubCUOfCtu )
  {
    // The 1-terminating bit is added to all streams, so don't add it here when it's 1.
    // i.e. when the slice segment CurEnd CTU address is the current CTU address+1.
    if (pcSlice->getSliceSegmentCurEndCtuTsAddr() != currentCTUTsAddr+1)
    {
      m_pcEntropyCoder->encodeTerminatingBit( 0 );
    }
  }
}

/** Compute QP for each CU
 * \param pcCU Target CU
 * \param uiDepth CU depth
 * \returns quantization parameter
 */
Int TEncCu::xComputeQP( TComDataCU* pcCU, UInt uiDepth )
{
  Int iBaseQp = pcCU->getSlice()->getSliceQp();
  Int iQpOffset = 0;
  if ( m_pcEncCfg->getUseAdaptiveQP() )
  {
    TEncPic* pcEPic = dynamic_cast<TEncPic*>( pcCU->getPic() );
    UInt uiAQDepth = min( uiDepth, pcEPic->getMaxAQDepth()-1 );
    TEncPicQPAdaptationLayer* pcAQLayer = pcEPic->getAQLayer( uiAQDepth );
    UInt uiAQUPosX = pcCU->getCUPelX() / pcAQLayer->getAQPartWidth();
    UInt uiAQUPosY = pcCU->getCUPelY() / pcAQLayer->getAQPartHeight();
    UInt uiAQUStride = pcAQLayer->getAQPartStride();
    TEncQPAdaptationUnit* acAQU = pcAQLayer->getQPAdaptationUnit();

    Double dMaxQScale = pow(2.0, m_pcEncCfg->getQPAdaptationRange()/6.0);
    Double dAvgAct = pcAQLayer->getAvgActivity();
    Double dCUAct = acAQU[uiAQUPosY * uiAQUStride + uiAQUPosX].getActivity();
    Double dNormAct = (dMaxQScale*dCUAct + dAvgAct) / (dCUAct + dMaxQScale*dAvgAct);
    Double dQpOffset = log(dNormAct) / log(2.0) * 6.0;
    iQpOffset = Int(floor( dQpOffset + 0.49999 ));
  }

  return Clip3(-pcCU->getSlice()->getSPS()->getQpBDOffset(CHANNEL_TYPE_LUMA), MAX_QP, iBaseQp+iQpOffset );
}

/** encode a CU block recursively
 * \param pcCU
 * \param uiAbsPartIdx
 * \param uiDepth
 * \returns Void
 */
Void TEncCu::xEncodeCU( TComDataCU* pcCU, UInt uiAbsPartIdx, UInt uiDepth )
{
        TComPic   *const pcPic   = pcCU->getPic();
        TComSlice *const pcSlice = pcCU->getSlice();
  const TComSPS   &sps =*(pcSlice->getSPS());
  const TComPPS   &pps =*(pcSlice->getPPS());

  const UInt maxCUWidth  = sps.getMaxCUWidth();
  const UInt maxCUHeight = sps.getMaxCUHeight();

        Bool bBoundary = false;
        UInt uiLPelX   = pcCU->getCUPelX() + g_auiRasterToPelX[ g_auiZscanToRaster[uiAbsPartIdx] ];
  const UInt uiRPelX   = uiLPelX + (maxCUWidth>>uiDepth)  - 1;
        UInt uiTPelY   = pcCU->getCUPelY() + g_auiRasterToPelY[ g_auiZscanToRaster[uiAbsPartIdx] ];
  const UInt uiBPelY   = uiTPelY + (maxCUHeight>>uiDepth) - 1;

#if NH_MV_ENC_DEC_TRAC
  DTRACE_CU_S("=========== coding_quadtree ===========\n")
  DTRACE_CU("x0", uiLPelX)
  DTRACE_CU("x1", uiTPelY)
  DTRACE_CU("log2CbSize", maxCUWidth>>uiDepth )
  DTRACE_CU("cqtDepth"  , uiDepth)
#endif

  if( ( uiRPelX < sps.getPicWidthInLumaSamples() ) && ( uiBPelY < sps.getPicHeightInLumaSamples() ) )
  {
    m_pcEntropyCoder->encodeSplitFlag( pcCU, uiAbsPartIdx, uiDepth );
  }
  else
  {
    bBoundary = true;
  }

  if( ( ( uiDepth < pcCU->getDepth( uiAbsPartIdx ) ) && ( uiDepth < sps.getLog2DiffMaxMinCodingBlockSize() ) ) || bBoundary )
  {
    UInt uiQNumParts = ( pcPic->getNumPartitionsInCtu() >> (uiDepth<<1) )>>2;
    if( uiDepth == pps.getMaxCuDQPDepth() && pps.getUseDQP())
    {
      setdQPFlag(true);
    }

    if( uiDepth == pps.getPpsRangeExtension().getDiffCuChromaQpOffsetDepth() && pcSlice->getUseChromaQpAdj())
    {
      setCodeChromaQpAdjFlag(true);
    }

    for ( UInt uiPartUnitIdx = 0; uiPartUnitIdx < 4; uiPartUnitIdx++, uiAbsPartIdx+=uiQNumParts )
    {
      uiLPelX   = pcCU->getCUPelX() + g_auiRasterToPelX[ g_auiZscanToRaster[uiAbsPartIdx] ];
      uiTPelY   = pcCU->getCUPelY() + g_auiRasterToPelY[ g_auiZscanToRaster[uiAbsPartIdx] ];
      if( ( uiLPelX < sps.getPicWidthInLumaSamples() ) && ( uiTPelY < sps.getPicHeightInLumaSamples() ) )
      {
        xEncodeCU( pcCU, uiAbsPartIdx, uiDepth+1 );
      }
    }
    return;
  }

#if NH_MV_ENC_DEC_TRAC
  DTRACE_CU_S("=========== coding_unit ===========\n")
#endif


  if( uiDepth <= pps.getMaxCuDQPDepth() && pps.getUseDQP())
  {
    setdQPFlag(true);
  }

  if( uiDepth <= pps.getPpsRangeExtension().getDiffCuChromaQpOffsetDepth() && pcSlice->getUseChromaQpAdj())
  {
    setCodeChromaQpAdjFlag(true);
  }

  if (pps.getTransquantBypassEnableFlag())
  {
    m_pcEntropyCoder->encodeCUTransquantBypassFlag( pcCU, uiAbsPartIdx );
  }

  if( !pcSlice->isIntra() )
  {
    m_pcEntropyCoder->encodeSkipFlag( pcCU, uiAbsPartIdx );
  }

  if( pcCU->isSkipped( uiAbsPartIdx ) )
  {
#if NH_MV_ENC_DEC_TRAC
    DTRACE_PU_S("=========== prediction_unit ===========\n")
    DTRACE_PU("x0", uiLPelX)
    DTRACE_PU("x1", uiTPelY)
#endif

    m_pcEntropyCoder->encodeMergeIndex( pcCU, uiAbsPartIdx );
#if NH_3D_ARP
    m_pcEntropyCoder->encodeARPW( pcCU , uiAbsPartIdx );
#endif
#if NH_3D_IC
    m_pcEntropyCoder->encodeICFlag  ( pcCU, uiAbsPartIdx );
#endif

    finishCU(pcCU,uiAbsPartIdx);
    return;
  }

#if NH_3D_DIS
  m_pcEntropyCoder->encodeDIS( pcCU, uiAbsPartIdx );
  if(!pcCU->getDISFlag(uiAbsPartIdx))
  {
#endif
  m_pcEntropyCoder->encodePredMode( pcCU, uiAbsPartIdx );
  m_pcEntropyCoder->encodePartSize( pcCU, uiAbsPartIdx, uiDepth );

  if (pcCU->isIntra( uiAbsPartIdx ) && pcCU->getPartitionSize( uiAbsPartIdx ) == SIZE_2Nx2N )
  {
    m_pcEntropyCoder->encodeIPCMInfo( pcCU, uiAbsPartIdx );

    if(pcCU->getIPCMFlag(uiAbsPartIdx))
    {
#if NH_3D_SDC_INTRA
      m_pcEntropyCoder->encodeSDCFlag( pcCU, uiAbsPartIdx );
#endif  

      // Encode slice finish
      finishCU(pcCU,uiAbsPartIdx);
      return;
    }
  }

  // prediction Info ( Intra : direction mode, Inter : Mv, reference idx )
  m_pcEntropyCoder->encodePredInfo( pcCU, uiAbsPartIdx );
#if NH_3D_DBBP
  m_pcEntropyCoder->encodeDBBPFlag( pcCU, uiAbsPartIdx );
#endif
#if NH_3D_SDC_INTRA
  m_pcEntropyCoder->encodeSDCFlag( pcCU, uiAbsPartIdx );
#endif  
#if NH_3D_ARP
  m_pcEntropyCoder->encodeARPW( pcCU , uiAbsPartIdx );
#endif
#if NH_3D_IC
  m_pcEntropyCoder->encodeICFlag  ( pcCU, uiAbsPartIdx );
#endif

  // Encode Coefficients
  Bool bCodeDQP = getdQPFlag();
  Bool codeChromaQpAdj = getCodeChromaQpAdjFlag();
  m_pcEntropyCoder->encodeCoeff( pcCU, uiAbsPartIdx, uiDepth, bCodeDQP, codeChromaQpAdj );
  setCodeChromaQpAdjFlag( codeChromaQpAdj );
  setdQPFlag( bCodeDQP );
#if NH_3D_DIS
  }
#endif


  // --- write terminating bit ---
  finishCU(pcCU,uiAbsPartIdx);
}

Int xCalcHADs8x8_ISlice(Pel *piOrg, Int iStrideOrg)
{
  Int k, i, j, jj;
  Int diff[64], m1[8][8], m2[8][8], m3[8][8], iSumHad = 0;

  for( k = 0; k < 64; k += 8 )
  {
    diff[k+0] = piOrg[0] ;
    diff[k+1] = piOrg[1] ;
    diff[k+2] = piOrg[2] ;
    diff[k+3] = piOrg[3] ;
    diff[k+4] = piOrg[4] ;
    diff[k+5] = piOrg[5] ;
    diff[k+6] = piOrg[6] ;
    diff[k+7] = piOrg[7] ;

    piOrg += iStrideOrg;
  }

  //horizontal
  for (j=0; j < 8; j++)
  {
    jj = j << 3;
    m2[j][0] = diff[jj  ] + diff[jj+4];
    m2[j][1] = diff[jj+1] + diff[jj+5];
    m2[j][2] = diff[jj+2] + diff[jj+6];
    m2[j][3] = diff[jj+3] + diff[jj+7];
    m2[j][4] = diff[jj  ] - diff[jj+4];
    m2[j][5] = diff[jj+1] - diff[jj+5];
    m2[j][6] = diff[jj+2] - diff[jj+6];
    m2[j][7] = diff[jj+3] - diff[jj+7];

    m1[j][0] = m2[j][0] + m2[j][2];
    m1[j][1] = m2[j][1] + m2[j][3];
    m1[j][2] = m2[j][0] - m2[j][2];
    m1[j][3] = m2[j][1] - m2[j][3];
    m1[j][4] = m2[j][4] + m2[j][6];
    m1[j][5] = m2[j][5] + m2[j][7];
    m1[j][6] = m2[j][4] - m2[j][6];
    m1[j][7] = m2[j][5] - m2[j][7];

    m2[j][0] = m1[j][0] + m1[j][1];
    m2[j][1] = m1[j][0] - m1[j][1];
    m2[j][2] = m1[j][2] + m1[j][3];
    m2[j][3] = m1[j][2] - m1[j][3];
    m2[j][4] = m1[j][4] + m1[j][5];
    m2[j][5] = m1[j][4] - m1[j][5];
    m2[j][6] = m1[j][6] + m1[j][7];
    m2[j][7] = m1[j][6] - m1[j][7];
  }

  //vertical
  for (i=0; i < 8; i++)
  {
    m3[0][i] = m2[0][i] + m2[4][i];
    m3[1][i] = m2[1][i] + m2[5][i];
    m3[2][i] = m2[2][i] + m2[6][i];
    m3[3][i] = m2[3][i] + m2[7][i];
    m3[4][i] = m2[0][i] - m2[4][i];
    m3[5][i] = m2[1][i] - m2[5][i];
    m3[6][i] = m2[2][i] - m2[6][i];
    m3[7][i] = m2[3][i] - m2[7][i];

    m1[0][i] = m3[0][i] + m3[2][i];
    m1[1][i] = m3[1][i] + m3[3][i];
    m1[2][i] = m3[0][i] - m3[2][i];
    m1[3][i] = m3[1][i] - m3[3][i];
    m1[4][i] = m3[4][i] + m3[6][i];
    m1[5][i] = m3[5][i] + m3[7][i];
    m1[6][i] = m3[4][i] - m3[6][i];
    m1[7][i] = m3[5][i] - m3[7][i];

    m2[0][i] = m1[0][i] + m1[1][i];
    m2[1][i] = m1[0][i] - m1[1][i];
    m2[2][i] = m1[2][i] + m1[3][i];
    m2[3][i] = m1[2][i] - m1[3][i];
    m2[4][i] = m1[4][i] + m1[5][i];
    m2[5][i] = m1[4][i] - m1[5][i];
    m2[6][i] = m1[6][i] + m1[7][i];
    m2[7][i] = m1[6][i] - m1[7][i];
  }

  for (i = 0; i < 8; i++)
  {
    for (j = 0; j < 8; j++)
    {
      iSumHad += abs(m2[i][j]);
    }
  }
  iSumHad -= abs(m2[0][0]);
  iSumHad =(iSumHad+2)>>2;
  return(iSumHad);
}

Int  TEncCu::updateCtuDataISlice(TComDataCU* pCtu, Int width, Int height)
{
  Int  xBl, yBl;
  const Int iBlkSize = 8;

  Pel* pOrgInit   = pCtu->getPic()->getPicYuvOrg()->getAddr(COMPONENT_Y, pCtu->getCtuRsAddr(), 0);
  Int  iStrideOrig = pCtu->getPic()->getPicYuvOrg()->getStride(COMPONENT_Y);
  Pel  *pOrg;

  Int iSumHad = 0;
  for ( yBl=0; (yBl+iBlkSize)<=height; yBl+= iBlkSize)
  {
    for ( xBl=0; (xBl+iBlkSize)<=width; xBl+= iBlkSize)
    {
      pOrg = pOrgInit + iStrideOrig*yBl + xBl;
      iSumHad += xCalcHADs8x8_ISlice(pOrg, iStrideOrig);
    }
  }
  return(iSumHad);
}

/** check RD costs for a CU block encoded with merge
 * \param rpcBestCU
 * \param rpcTempCU
 * \param earlyDetectionSkipMode
 */
Void TEncCu::xCheckRDCostMerge2Nx2N( TComDataCU*& rpcBestCU, TComDataCU*& rpcTempCU DEBUG_STRING_FN_DECLARE(sDebug), Bool *earlyDetectionSkipMode )
{
  assert( rpcTempCU->getSlice()->getSliceType() != I_SLICE );
  if(getFastDeltaQp())
  {
    return;   // never check merge in fast deltaqp mode
  }

#if NH_MV
  D_PRINT_INC_INDENT( g_traceModeCheck, "xCheckRDCostMerge2Nx2N" );
#endif

#if NH_3D_MLC
  TComMvField  cMvFieldNeighbours[MRG_MAX_NUM_CANDS_MEM << 1]; // double length for mv of both lists
  UChar uhInterDirNeighbours[MRG_MAX_NUM_CANDS_MEM];
#else
  TComMvField  cMvFieldNeighbours[2 * MRG_MAX_NUM_CANDS]; // double length for mv of both lists
  UChar uhInterDirNeighbours[MRG_MAX_NUM_CANDS];
#endif
  Int numValidMergeCand = 0;
  const Bool bTransquantBypassFlag = rpcTempCU->getCUTransquantBypass(0);

  for( UInt ui = 0; ui < rpcTempCU->getSlice()->getMaxNumMergeCand(); ++ui )
  {
    uhInterDirNeighbours[ui] = 0;
  }
  UChar uhDepth = rpcTempCU->getDepth( 0 );
#if NH_3D_IC
  Bool bICFlag = rpcTempCU->getICFlag( 0 );
#endif
#if NH_3D_VSO // M1  //necessary here?
  if( m_pcRdCost->getUseRenModel() )
  {
    UInt  uiWidth     = m_ppcOrigYuv[uhDepth]->getWidth ( COMPONENT_Y );
    UInt  uiHeight    = m_ppcOrigYuv[uhDepth]->getHeight( COMPONENT_Y );
    Pel*  piSrc       = m_ppcOrigYuv[uhDepth]->getAddr  ( COMPONENT_Y );
    UInt  uiSrcStride = m_ppcOrigYuv[uhDepth]->getStride( COMPONENT_Y );
    m_pcRdCost->setRenModelData( rpcTempCU, 0, piSrc, uiSrcStride, uiWidth, uiHeight );
  }
#endif

#if NH_3D_ARP
  DisInfo cOrigDisInfo = rpcTempCU->getDvInfo(0);
#else
#endif

  rpcTempCU->setPartSizeSubParts( SIZE_2Nx2N, 0, uhDepth ); // interprets depth relative to CTU level

#if NH_3D_SPIVMP
  Bool bSPIVMPFlag[MRG_MAX_NUM_CANDS_MEM];
  memset(bSPIVMPFlag, false, sizeof(Bool)*MRG_MAX_NUM_CANDS_MEM);
  TComMvField*  pcMvFieldSP;
  UChar* puhInterDirSP;
  pcMvFieldSP = new TComMvField[rpcTempCU->getPic()->getPicSym()->getNumPartitionsInCtu()*2]; 
  puhInterDirSP = new UChar[rpcTempCU->getPic()->getPicSym()->getNumPartitionsInCtu()]; 
#endif

#if NH_3D_VSP
#if !NH_3D_ARP
  Int vspFlag[MRG_MAX_NUM_CANDS_MEM];
  memset(vspFlag, 0, sizeof(Int)*MRG_MAX_NUM_CANDS_MEM);
#if NH_3D_MLC
  rpcTempCU->initAvailableFlags();
#endif
  rpcTempCU->getInterMergeCandidates( 0, 0, cMvFieldNeighbours, uhInterDirNeighbours, numValidMergeCand );
#if NH_3D_MLC
  rpcTempCU->xGetInterMergeCandidates( 0, 0, cMvFieldNeighbours,uhInterDirNeighbours
#if NH_3D_SPIVMP
    , pcMvFieldSP, puhInterDirSP
#endif
    , numValidMergeCand 
    );

  rpcTempCU->buildMCL( cMvFieldNeighbours,uhInterDirNeighbours, vspFlag
#if NH_3D_SPIVMP
    , bSPIVMPFlag
#endif
    , numValidMergeCand 
    );
#endif
#endif
#else
#if NH_3D_MLC
  rpcTempCU->initAvailableFlags();
#endif
  rpcTempCU->getInterMergeCandidates( 0, 0, cMvFieldNeighbours,uhInterDirNeighbours, numValidMergeCand );
#if NH_3D_MLC
  rpcTempCU->xGetInterMergeCandidates( 0, 0, cMvFieldNeighbours,uhInterDirNeighbours
#if H_3D_SPIVMP
    , pcMvFieldSP, puhInterDirSP
#endif
    , numValidMergeCand 
    );
#if NH_3D_MLC
  rpcTempCU->buildMCL( cMvFieldNeighbours,uhInterDirNeighbours
#if H_3D_SPIVMP
    , bSPIVMPFlag
#endif
    , numValidMergeCand 
    );
#endif
#endif
#endif

#if NH_3D_MLC
  Int mergeCandBuffer[MRG_MAX_NUM_CANDS_MEM];
#else
  Int mergeCandBuffer[MRG_MAX_NUM_CANDS];
#endif
#if NH_3D_MLC
  for( UInt ui = 0; ui < rpcTempCU->getSlice()->getMaxNumMergeCand(); ++ui )
#else
  for( UInt ui = 0; ui < numValidMergeCand; ++ui )
#endif
  {
    mergeCandBuffer[ui] = 0;
  }

  Bool bestIsSkip = false;

  UInt iteration;
  if ( rpcTempCU->isLosslessCoded(0))
  {
    iteration = 1;
  }
  else
  {
    iteration = 2;
  }
  DEBUG_STRING_NEW(bestStr)

#if NH_3D_ARP
  Int nARPWMax = rpcTempCU->getSlice()->getARPStepNum() - 1;
#if NH_3D_IC
  if( nARPWMax < 0 || bICFlag )
#else
  if( nARPWMax < 0 )
#endif
  {
    nARPWMax = 0;
  }
  for( Int nARPW=nARPWMax; nARPW >= 0 ; nARPW-- )
  {
#if NH_3D
#if DEBUG_STRING
    bestStr.clear(); 
#endif
#endif
#if NH_3D_IV_MERGE
    memset( mergeCandBuffer, 0, MRG_MAX_NUM_CANDS_MEM*sizeof(Int) );
#else
    memset( mergeCandBuffer, 0, MRG_MAX_NUM_CANDS * sizeof(Int) );
#endif
    rpcTempCU->setPartSizeSubParts( SIZE_2Nx2N, 0, uhDepth ); // interprets depth relative to LCU level
    rpcTempCU->setARPWSubParts( (UChar)nARPW , 0 , uhDepth );
#if NH_3D_IC
    rpcTempCU->setICFlagSubParts( bICFlag, 0, 0, uhDepth );
#endif
    rpcTempCU->getDvInfo(0) = cOrigDisInfo;
    rpcTempCU->setDvInfoSubParts(cOrigDisInfo, 0, uhDepth );
#if NH_3D_VSP
    Int vspFlag[MRG_MAX_NUM_CANDS_MEM];
    memset(vspFlag, 0, sizeof(Int)*MRG_MAX_NUM_CANDS_MEM);
#endif
#if NH_3D
#if NH_3D_MLC
    rpcTempCU->initAvailableFlags();
#endif
    rpcTempCU->getInterMergeCandidates( 0, 0, cMvFieldNeighbours, uhInterDirNeighbours, numValidMergeCand );
    rpcTempCU->xGetInterMergeCandidates( 0, 0, cMvFieldNeighbours,uhInterDirNeighbours
#if NH_3D_SPIVMP
      , pcMvFieldSP, puhInterDirSP
#endif
      , numValidMergeCand 
      );

    rpcTempCU->buildMCL( cMvFieldNeighbours,uhInterDirNeighbours
#if NH_3D_VSP
      , vspFlag
#endif
#if NH_3D_SPIVMP
      , bSPIVMPFlag
#endif
      , numValidMergeCand 
      );

#else
    rpcTempCU->getInterMergeCandidates( 0, 0, cMvFieldNeighbours,uhInterDirNeighbours, numValidMergeCand );
#endif


#endif

  for( UInt uiNoResidual = 0; uiNoResidual < iteration; ++uiNoResidual )
  {
#if NH_MV
    D_PRINT_INC_INDENT ( g_traceModeCheck, "uiNoResidual: " + n2s( uiNoResidual) );
#endif

    for( UInt uiMergeCand = 0; uiMergeCand < numValidMergeCand; ++uiMergeCand )
    {
#if NH_3D_IC
      if( rpcTempCU->getSlice()->getApplyIC() && rpcTempCU->getSlice()->getIcSkipParseFlag() )
      {
        if( bICFlag && uiMergeCand == 0 ) 
        {
          continue;
        }
      }
#endif
#if NH_MV
      D_PRINT_INC_INDENT ( g_traceModeCheck, "uiMergeCand: "+  n2s(uiMergeCand) );
#endif

      if(!(uiNoResidual==1 && mergeCandBuffer[uiMergeCand]==1))
      {
        if( !(bestIsSkip && uiNoResidual == 0) )
        {
          DEBUG_STRING_NEW(tmpStr)
          // set MC parameters
          rpcTempCU->setPredModeSubParts( MODE_INTER, 0, uhDepth ); // interprets depth relative to CTU level
#if NH_3D_IC
          rpcTempCU->setICFlagSubParts( bICFlag, 0, 0, uhDepth );
#endif
          rpcTempCU->setCUTransquantBypassSubParts( bTransquantBypassFlag, 0, uhDepth );
          rpcTempCU->setChromaQpAdjSubParts( bTransquantBypassFlag ? 0 : m_cuChromaQpOffsetIdxPlus1, 0, uhDepth );
          rpcTempCU->setPartSizeSubParts( SIZE_2Nx2N, 0, uhDepth ); // interprets depth relative to CTU level
          rpcTempCU->setMergeFlagSubParts( true, 0, 0, uhDepth ); // interprets depth relative to CTU level
          rpcTempCU->setMergeIndexSubParts( uiMergeCand, 0, 0, uhDepth ); // interprets depth relative to CTU level
#if NH_3D_ARP
          rpcTempCU->setARPWSubParts( (UChar)nARPW , 0 , uhDepth );
#endif

#if NH_3D_VSP
          rpcTempCU->setVSPFlagSubParts( vspFlag[uiMergeCand], 0, 0, uhDepth );
#endif
#if NH_3D_SPIVMP
          rpcTempCU->setSPIVMPFlagSubParts(bSPIVMPFlag[uiMergeCand], 0, 0, uhDepth);
          if (bSPIVMPFlag[uiMergeCand])
          {
            UInt uiSPAddr;
            Int iWidth = rpcTempCU->getWidth(0);
            Int iHeight = rpcTempCU->getHeight(0);
            Int iNumSPInOneLine, iNumSP, iSPWidth, iSPHeight;
            rpcTempCU->getSPPara(iWidth, iHeight, iNumSP, iNumSPInOneLine, iSPWidth, iSPHeight);
            for (Int iPartitionIdx = 0; iPartitionIdx < iNumSP; iPartitionIdx++)
            {
              rpcTempCU->getSPAbsPartIdx(0, iSPWidth, iSPHeight, iPartitionIdx, iNumSPInOneLine, uiSPAddr);
              rpcTempCU->setInterDirSP(puhInterDirSP[iPartitionIdx], uiSPAddr, iSPWidth, iSPHeight);
              rpcTempCU->getCUMvField( REF_PIC_LIST_0 )->setMvFieldSP(rpcTempCU, uiSPAddr, pcMvFieldSP[2*iPartitionIdx], iSPWidth, iSPHeight);
              rpcTempCU->getCUMvField( REF_PIC_LIST_1 )->setMvFieldSP(rpcTempCU, uiSPAddr, pcMvFieldSP[2*iPartitionIdx + 1], iSPWidth, iSPHeight);
            }
          }
          else
#endif
          {
#if NH_3D_VSP
            if ( vspFlag[uiMergeCand] )
            {
              UInt partAddr;
              Int vspSize;
              Int width, height;
              rpcTempCU->getPartIndexAndSize( 0, partAddr, width, height );
              if( uhInterDirNeighbours[ uiMergeCand ] & 0x01 )
              {
                rpcTempCU->setMvFieldPUForVSP( rpcTempCU, partAddr, width, height, REF_PIC_LIST_0, cMvFieldNeighbours[ 2*uiMergeCand + 0 ].getRefIdx(), vspSize );
                rpcTempCU->setVSPFlag( partAddr, vspSize );
              }
              else
              {
                rpcTempCU->getCUMvField( REF_PIC_LIST_0 )->setAllMvField( cMvFieldNeighbours[0 + 2*uiMergeCand], SIZE_2Nx2N, 0, 0 ); // interprets depth relative to rpcTempCU level
              }
              if( uhInterDirNeighbours[ uiMergeCand ] & 0x02 )
              {
                rpcTempCU->setMvFieldPUForVSP( rpcTempCU, partAddr, width, height, REF_PIC_LIST_1 , cMvFieldNeighbours[ 2*uiMergeCand + 1 ].getRefIdx(), vspSize );
                rpcTempCU->setVSPFlag( partAddr, vspSize );
              }
              else
              {
                rpcTempCU->getCUMvField( REF_PIC_LIST_1 )->setAllMvField( cMvFieldNeighbours[1 + 2*uiMergeCand], SIZE_2Nx2N, 0, 0 ); // interprets depth relative to rpcTempCU level
              }
              rpcTempCU->setInterDirSubParts( uhInterDirNeighbours[uiMergeCand], 0, 0, uhDepth ); // interprets depth relative to LCU level
            }
            else
            {
#endif
            rpcTempCU->setInterDirSubParts( uhInterDirNeighbours[uiMergeCand], 0, 0, uhDepth ); // interprets depth relative to CTU level
            rpcTempCU->getCUMvField( REF_PIC_LIST_0 )->setAllMvField( cMvFieldNeighbours[0 + 2*uiMergeCand], SIZE_2Nx2N, 0, 0 ); // interprets depth relative to rpcTempCU level
            rpcTempCU->getCUMvField( REF_PIC_LIST_1 )->setAllMvField( cMvFieldNeighbours[1 + 2*uiMergeCand], SIZE_2Nx2N, 0, 0 ); // interprets depth relative to rpcTempCU level
#if NH_3D_VSP
            }
#endif
          }
          // do MC
          m_pcPredSearch->motionCompensation ( rpcTempCU, m_ppcPredYuvTemp[uhDepth] );
          // estimate residual and encode everything
#if NH_3D_VSO //M2
          if( m_pcRdCost->getUseRenModel() )
          { //Reset
            UInt  uiWidth     = m_ppcOrigYuv[uhDepth]->getWidth    ( COMPONENT_Y );
            UInt  uiHeight    = m_ppcOrigYuv[uhDepth]->getHeight   ( COMPONENT_Y );
            Pel*  piSrc       = m_ppcOrigYuv[uhDepth]->getAddr     ( COMPONENT_Y );
            UInt  uiSrcStride = m_ppcOrigYuv[uhDepth]->getStride   ( COMPONENT_Y );
            m_pcRdCost->setRenModelData( rpcTempCU, 0, piSrc, uiSrcStride, uiWidth, uiHeight );
          }
#endif
          m_pcPredSearch->encodeResAndCalcRdInterCU( rpcTempCU,
                                                     m_ppcOrigYuv    [uhDepth],
                                                     m_ppcPredYuvTemp[uhDepth],
                                                     m_ppcResiYuvTemp[uhDepth],
                                                     m_ppcResiYuvBest[uhDepth],
                                                     m_ppcRecoYuvTemp[uhDepth],
                                                     (uiNoResidual != 0) DEBUG_STRING_PASS_INTO(tmpStr) );

#if DEBUG_STRING
          DebugInterPredResiReco(tmpStr, *(m_ppcPredYuvTemp[uhDepth]), *(m_ppcResiYuvBest[uhDepth]), *(m_ppcRecoYuvTemp[uhDepth]), DebugStringGetPredModeMask(rpcTempCU->getPredictionMode(0)));
#endif

          if ((uiNoResidual == 0) && (rpcTempCU->getQtRootCbf(0) == 0))
          {
            // If no residual when allowing for one, then set mark to not try case where residual is forced to 0
            mergeCandBuffer[uiMergeCand] = 1;
          }
#if NH_3D_DIS
          rpcTempCU->setDISFlagSubParts( false, 0, uhDepth );
#endif
#if NH_3D_VSP
          if( rpcTempCU->getSkipFlag(0) )
          {
            rpcTempCU->setTrIdxSubParts(0, 0, uhDepth);
          }
#endif
#if NH_3D_SDC_INTER
          TComDataCU *rpcTempCUPre = rpcTempCU;
#endif
          Int orgQP = rpcTempCU->getQP( 0 );
          xCheckDQP( rpcTempCU );
          xCheckBestMode(rpcBestCU, rpcTempCU, uhDepth DEBUG_STRING_PASS_INTO(bestStr) DEBUG_STRING_PASS_INTO(tmpStr));
#if NH_3D_SDC_INTER
          if( rpcTempCU->getSlice()->getInterSdcFlag() && !uiNoResidual )
          {
            Double dOffsetCost[3] = {MAX_DOUBLE,MAX_DOUBLE,MAX_DOUBLE};
            for( Int uiOffest = 1 ; uiOffest <= 5 ; uiOffest++ )
            {
              if( uiOffest > 3)
              {
                if ( dOffsetCost[0] < (0.9*dOffsetCost[1]) && dOffsetCost[0] < (0.9*dOffsetCost[2]) )
                {
                  continue;
                }
                if ( dOffsetCost[1] < dOffsetCost[0] && dOffsetCost[0] < dOffsetCost[2] &&  uiOffest == 5)
                {
                  continue;
                }
                if ( dOffsetCost[0] < dOffsetCost[1] && dOffsetCost[2] < dOffsetCost[0] &&  uiOffest == 4)
                {
                  continue;
                }
              }
              if( rpcTempCU != rpcTempCUPre )
              {
                rpcTempCU->initEstData( uhDepth, orgQP, bTransquantBypassFlag  );
                rpcTempCU->copyPartFrom( rpcBestCU, 0, uhDepth );
              }
              rpcTempCU->setSkipFlagSubParts( false, 0, uhDepth );
#if NH_3D_DIS
              rpcTempCU->setDISFlagSubParts( false, 0, uhDepth );
#endif
              rpcTempCU->setTrIdxSubParts( 0, 0, uhDepth );
              rpcTempCU->setCbfSubParts( 1, COMPONENT_Y, 0, uhDepth );
#if NH_3D_VSO //M2
              if( m_pcRdCost->getUseRenModel() )
              { //Reset
                UInt  uiWidth     = m_ppcOrigYuv[uhDepth]->getWidth    ( COMPONENT_Y );
                UInt  uiHeight    = m_ppcOrigYuv[uhDepth]->getHeight   ( COMPONENT_Y );
                Pel*  piSrc       = m_ppcOrigYuv[uhDepth]->getAddr     ( COMPONENT_Y );
                UInt  uiSrcStride = m_ppcOrigYuv[uhDepth]->getStride   ( COMPONENT_Y );
                m_pcRdCost->setRenModelData( rpcTempCU, 0, piSrc, uiSrcStride, uiWidth, uiHeight );
              }
#endif
              Int iSdcOffset = 0;
              if(uiOffest % 2 == 0)
              {
                iSdcOffset = uiOffest >> 1;
              }
              else
              {
                iSdcOffset = -1 * (uiOffest >> 1);
              }
              m_pcPredSearch->encodeResAndCalcRdInterSDCCU( rpcTempCU, 
                m_ppcOrigYuv[uhDepth], 
                ( rpcTempCU != rpcTempCUPre ) ? m_ppcPredYuvBest[uhDepth] : m_ppcPredYuvTemp[uhDepth], 
                m_ppcResiYuvTemp[uhDepth], 
                m_ppcRecoYuvTemp[uhDepth],
                iSdcOffset,
                uhDepth );
              if (uiOffest <= 3 )
              {
                dOffsetCost [uiOffest -1] = rpcTempCU->getTotalCost();
              }

              xCheckDQP( rpcTempCU );
              xCheckBestMode( rpcBestCU, rpcTempCU, uhDepth DEBUG_STRING_PASS_INTO(bestStr) DEBUG_STRING_PASS_INTO(tmpStr) );
            }
          }
#endif

          rpcTempCU->initEstData( uhDepth, orgQP, bTransquantBypassFlag );

          if( m_pcEncCfg->getUseFastDecisionForMerge() && !bestIsSkip )
          {
#if NH_3D_SDC_INTER
            if( rpcTempCU->getSlice()->getInterSdcFlag() )
            {
              bestIsSkip = !rpcBestCU->getSDCFlag( 0 ) && ( rpcBestCU->getQtRootCbf(0) == 0 );
            }
            else
            {
#endif
            bestIsSkip = rpcBestCU->getQtRootCbf(0) == 0;
#if NH_3D_SDC_INTER
            }
#endif
          }
        }
      }
#if NH_MV
      D_DEC_INDENT( g_traceModeCheck ); 
#endif
    }

    if(uiNoResidual == 0 && m_pcEncCfg->getUseEarlySkipDetection())
    {
      if(rpcBestCU->getQtRootCbf( 0 ) == 0)
      {
        if( rpcBestCU->getMergeFlag( 0 ))
        {
          *earlyDetectionSkipMode = true;
        }
        else if(m_pcEncCfg->getMotionEstimationSearchMethod() != MESEARCH_SELECTIVE)
        {
          Int absoulte_MV=0;
          for ( UInt uiRefListIdx = 0; uiRefListIdx < 2; uiRefListIdx++ )
          {
            if ( rpcBestCU->getSlice()->getNumRefIdx( RefPicList( uiRefListIdx ) ) > 0 )
            {
              TComCUMvField* pcCUMvField = rpcBestCU->getCUMvField(RefPicList( uiRefListIdx ));
              Int iHor = pcCUMvField->getMvd( 0 ).getAbsHor();
              Int iVer = pcCUMvField->getMvd( 0 ).getAbsVer();
              absoulte_MV+=iHor+iVer;
            }
          }

          if(absoulte_MV == 0)
          {
            *earlyDetectionSkipMode = true;
          }
        }
      }
    }
#if NH_MV
    D_DEC_INDENT( g_traceModeCheck ); 
#endif
  }
  DEBUG_STRING_APPEND(sDebug, bestStr)
#if NH_3D_ARP
 }
#endif
#if NH_3D_SPIVMP
 delete[] pcMvFieldSP;
 delete[] puhInterDirSP;
#endif
#if NH_MV
 D_DEC_INDENT( g_traceModeCheck );
#endif
}


#if AMP_MRG
#if  NH_3D_FAST_TEXTURE_ENCODING
Void TEncCu::xCheckRDCostInter( TComDataCU*& rpcBestCU, TComDataCU*& rpcTempCU, PartSize ePartSize DEBUG_STRING_FN_DECLARE(sDebug), Bool bFMD, Bool bUseMRG)
#else
Void TEncCu::xCheckRDCostInter( TComDataCU*& rpcBestCU, TComDataCU*& rpcTempCU, PartSize ePartSize DEBUG_STRING_FN_DECLARE(sDebug), Bool bUseMRG)
#endif
#else
Void TEncCu::xCheckRDCostInter( TComDataCU*& rpcBestCU, TComDataCU*& rpcTempCU, PartSize ePartSize )
#endif
{
  DEBUG_STRING_NEW(sTest)

  if(getFastDeltaQp())
  {
    const TComSPS &sps=*(rpcTempCU->getSlice()->getSPS());
    const UInt fastDeltaQPCuMaxSize = Clip3(sps.getMaxCUHeight()>>(sps.getLog2DiffMaxMinCodingBlockSize()), sps.getMaxCUHeight(), 32u);
    if(ePartSize != SIZE_2Nx2N || rpcTempCU->getWidth( 0 ) > fastDeltaQPCuMaxSize)
    {
      return; // only check necessary 2Nx2N Inter in fast deltaqp mode
    }
  }

#if NH_MV
  D_PRINT_INC_INDENT(g_traceModeCheck,   "xCheckRDCostInter; ePartSize:" + n2s( ePartSize) );
#endif


  // prior to this, rpcTempCU will have just been reset using rpcTempCU->initEstData( uiDepth, iQP, bIsLosslessMode );
#if NH_3D_ARP
  const Bool bTransquantBypassFlag = rpcTempCU->getCUTransquantBypass(0);
#endif
#if  NH_3D_FAST_TEXTURE_ENCODING
  if(!(bFMD && (ePartSize == SIZE_2Nx2N)))  //have  motion estimation or merge check
  {
#endif
  UChar uhDepth = rpcTempCU->getDepth( 0 );
#if NH_3D_ARP
    Bool bFirstTime = true;
    Int nARPWMax    = rpcTempCU->getSlice()->getARPStepNum() - 1;
#if NH_3D_IC
    if( nARPWMax < 0 || ePartSize != SIZE_2Nx2N || rpcTempCU->getICFlag(0) )
#else
    if( nARPWMax < 0 || ePartSize != SIZE_2Nx2N )
#endif
    {
      nARPWMax = 0;
    }

    for( Int nARPW = 0; nARPW <= nARPWMax; nARPW++ )
    {
#if DEBUG_STRING && NH_MV_ENC_DEC_TRAC
      sTest.clear(); 
#endif

      if( !bFirstTime && rpcTempCU->getSlice()->getIvResPredFlag() )
      {
        rpcTempCU->initEstData( rpcTempCU->getDepth(0), rpcTempCU->getQP(0),bTransquantBypassFlag );      
      }
#endif
#if NH_3D_VSO // M3
      if( m_pcRdCost->getUseRenModel() )
      {
        UInt  uiWidth     = m_ppcOrigYuv[uhDepth]->getWidth ( COMPONENT_Y );
        UInt  uiHeight    = m_ppcOrigYuv[uhDepth]->getHeight( COMPONENT_Y );
        Pel*  piSrc       = m_ppcOrigYuv[uhDepth]->getAddr  ( COMPONENT_Y );
        UInt  uiSrcStride = m_ppcOrigYuv[uhDepth]->getStride( COMPONENT_Y );
        m_pcRdCost->setRenModelData( rpcTempCU, 0, piSrc, uiSrcStride, uiWidth, uiHeight );
      }
#endif
#if NH_3D_DIS
      rpcTempCU->setDISFlagSubParts( false, 0, uhDepth );
#endif
  rpcTempCU->setPartSizeSubParts  ( ePartSize,  0, uhDepth );
  rpcTempCU->setPredModeSubParts  ( MODE_INTER, 0, uhDepth );
  rpcTempCU->setChromaQpAdjSubParts( rpcTempCU->getCUTransquantBypass(0) ? 0 : m_cuChromaQpOffsetIdxPlus1, 0, uhDepth );
#if NH_3D_ARP
      rpcTempCU->setARPWSubParts( (UChar)nARPW , 0 , uhDepth );
#endif
#if NH_3D_ARP
      if( bFirstTime == false && nARPWMax )
      {
        rpcTempCU->copyPartFrom( m_ppcWeightedTempCU[uhDepth] , 0 , uhDepth );
        rpcTempCU->setARPWSubParts( (UChar)nARPW , 0 , uhDepth );

        m_pcPredSearch->motionCompensation( rpcTempCU , m_ppcPredYuvTemp[uhDepth] );
      }
      else
      {
        bFirstTime = false;
#endif
#if AMP_MRG
  rpcTempCU->setMergeAMP (true);
#if  NH_3D_FAST_TEXTURE_ENCODING
        m_pcPredSearch->predInterSearch ( rpcTempCU, m_ppcOrigYuv[uhDepth], m_ppcPredYuvTemp[uhDepth], m_ppcResiYuvTemp[uhDepth], m_ppcRecoYuvTemp[uhDepth] DEBUG_STRING_PASS_INTO(sTest), bFMD, false, bUseMRG );
#else
  m_pcPredSearch->predInterSearch ( rpcTempCU, m_ppcOrigYuv[uhDepth], m_ppcPredYuvTemp[uhDepth], m_ppcResiYuvTemp[uhDepth], m_ppcRecoYuvTemp[uhDepth] DEBUG_STRING_PASS_INTO(sTest), false, bUseMRG );
#endif

#else
  m_pcPredSearch->predInterSearch ( rpcTempCU, m_ppcOrigYuv[uhDepth], m_ppcPredYuvTemp[uhDepth], m_ppcResiYuvTemp[uhDepth], m_ppcRecoYuvTemp[uhDepth] );
#endif
#if NH_3D_ARP
        if( nARPWMax )
        {
          m_ppcWeightedTempCU[uhDepth]->copyPartFrom( rpcTempCU , 0 , uhDepth );
        }
      }
#endif

#if AMP_MRG
  if ( !rpcTempCU->getMergeAMP() )
  {
#if NH_3D_ARP
        if( nARPWMax )
        {
          continue;
        }
        else
#endif
    {
#if NH_MV
        D_DEC_INDENT( g_traceModeCheck ); 
#endif
    return;
  }
  }
#endif
#if KWU_RC_MADPRED_E0227
      if ( m_pcEncCfg->getUseRateCtrl() && m_pcEncCfg->getLCULevelRC() && ePartSize == SIZE_2Nx2N && uhDepth <= m_addSADDepth )
      {
        UInt SAD = m_pcRdCost->getSADPart( g_bitDepthY, m_ppcPredYuvTemp[uhDepth]->getLumaAddr(), m_ppcPredYuvTemp[uhDepth]->getStride(),
          m_ppcOrigYuv[uhDepth]->getLumaAddr(), m_ppcOrigYuv[uhDepth]->getStride(),
          rpcTempCU->getWidth(0), rpcTempCU->getHeight(0) );
        m_temporalSAD = (Int)SAD;
      }
#endif

  m_pcPredSearch->encodeResAndCalcRdInterCU( rpcTempCU, m_ppcOrigYuv[uhDepth], m_ppcPredYuvTemp[uhDepth], m_ppcResiYuvTemp[uhDepth], m_ppcResiYuvBest[uhDepth], m_ppcRecoYuvTemp[uhDepth], false DEBUG_STRING_PASS_INTO(sTest) );
#if NH_3D_VSP
  if( rpcTempCU->getQtRootCbf(0)==0 )
  {
    rpcTempCU->setTrIdxSubParts(0, 0, uhDepth);
  }
#endif
#if NH_3D_VSO // M4
  if( m_pcRdCost->getUseLambdaScaleVSO() )
  {
    rpcTempCU->getTotalCost()  = m_pcRdCost->calcRdCostVSO( rpcTempCU->getTotalBits(), rpcTempCU->getTotalDistortion() );
  }
  else            
#endif
    rpcTempCU->getTotalCost()  = m_pcRdCost->calcRdCost( rpcTempCU->getTotalBits(), rpcTempCU->getTotalDistortion() );

#if DEBUG_STRING
  DebugInterPredResiReco(sTest, *(m_ppcPredYuvTemp[uhDepth]), *(m_ppcResiYuvBest[uhDepth]), *(m_ppcRecoYuvTemp[uhDepth]), DebugStringGetPredModeMask(rpcTempCU->getPredictionMode(0)));
#endif
#if NH_3D_SDC_INTER
      TComDataCU *rpcTempCUPre = rpcTempCU;
#endif

  xCheckDQP( rpcTempCU );

// #if PRINT_FEATURE
//   // 提取单纯的Inter_2Nx2N 率失真代价(第一次提取位置不对)
//   if (myFlag[3])
//   {
// 	  // 获得Inter2Nx2N 模式率失真代价
// 	  features[21] = rpcTempCU->getTotalCost();



// 		features[28] = rpcTempCU->getCUMvField(RefPicList(0))->getMvd(0).getHor();
// 		features[29] = rpcTempCU->getCUMvField(RefPicList(0))->getMvd(0).getVer();


//   }
// #endif

  xCheckBestMode(rpcBestCU, rpcTempCU, uhDepth DEBUG_STRING_PASS_INTO(sDebug) DEBUG_STRING_PASS_INTO(sTest));
#if NH_3D_SDC_INTER
      if( rpcTempCU->getSlice()->getInterSdcFlag() && ePartSize == SIZE_2Nx2N)
      {
        Double dOffsetCost[3] = {MAX_DOUBLE,MAX_DOUBLE,MAX_DOUBLE};
        for( Int uiOffest = 1 ; uiOffest <= 5 ; uiOffest++ )
        {
          if( uiOffest > 3)
          {
            if ( dOffsetCost[0] < (0.9*dOffsetCost[1]) && dOffsetCost[0] < (0.9*dOffsetCost[2]) )
            {
              continue;
            }
            if ( dOffsetCost[1] < dOffsetCost[0] && dOffsetCost[0] < dOffsetCost[2] &&  uiOffest == 5)
            {
              continue;
            }
            if ( dOffsetCost[0] < dOffsetCost[1] && dOffsetCost[2] < dOffsetCost[0] &&  uiOffest == 4)
            {
              continue;
            }
          }

          if( rpcTempCU != rpcTempCUPre )
          {
            Int orgQP = rpcBestCU->getQP( 0 );
            rpcTempCU->initEstData( uhDepth, orgQP ,bTransquantBypassFlag );      
            rpcTempCU->copyPartFrom( rpcBestCU, 0, uhDepth );
          }
          rpcTempCU->setSkipFlagSubParts( false, 0, uhDepth );
#if NH_3D_DIS
          rpcTempCU->setDISFlagSubParts( false, 0, uhDepth );
#endif
          rpcTempCU->setTrIdxSubParts( 0, 0, uhDepth );
          rpcTempCU->setCbfSubParts( 1, COMPONENT_Y, 0, uhDepth );
#if NH_3D_VSO // M3
          if( m_pcRdCost->getUseRenModel() )
          {
            UInt  uiWidth     = m_ppcOrigYuv[uhDepth]->getWidth ( COMPONENT_Y );
            UInt  uiHeight    = m_ppcOrigYuv[uhDepth]->getHeight( COMPONENT_Y  );
            Pel*  piSrc       = m_ppcOrigYuv[uhDepth]->getAddr  ( COMPONENT_Y  );
            UInt  uiSrcStride = m_ppcOrigYuv[uhDepth]->getStride( COMPONENT_Y  );
            m_pcRdCost->setRenModelData( rpcTempCU, 0, piSrc, uiSrcStride, uiWidth, uiHeight );
          }
#endif

          Int iSdcOffset = 0;
          if(uiOffest % 2 == 0)
          {
            iSdcOffset = uiOffest >> 1;
          }
          else
          {
            iSdcOffset = -1 * (uiOffest >> 1);
          }
          m_pcPredSearch->encodeResAndCalcRdInterSDCCU( rpcTempCU, 
            m_ppcOrigYuv[uhDepth],
            ( rpcTempCU != rpcTempCUPre ) ? m_ppcPredYuvBest[uhDepth] : m_ppcPredYuvTemp[uhDepth],
            m_ppcResiYuvTemp[uhDepth],
            m_ppcRecoYuvTemp[uhDepth],
            iSdcOffset,
            uhDepth );
          if (uiOffest <= 3 )
          {
            dOffsetCost [uiOffest -1] = rpcTempCU->getTotalCost();
          }

          xCheckDQP( rpcTempCU );
          xCheckBestMode(rpcBestCU, rpcTempCU, uhDepth DEBUG_STRING_PASS_INTO(sDebug) DEBUG_STRING_PASS_INTO(sTest));
        }

      }
#endif
#if NH_3D_ARP
    }
#endif
#if  NH_3D_FAST_TEXTURE_ENCODING
  }
#endif
#if NH_MV
  D_DEC_INDENT( g_traceModeCheck ); 
#endif
}

#if NH_3D_DBBP
Void TEncCu::xInvalidateOriginalSegments( TComYuv* pOrigYuv, TComYuv* pOrigYuvTemp, Bool* pMask, UInt uiValidSegment )
{
  UInt  uiWidth     = pOrigYuv->getWidth (COMPONENT_Y);
  UInt  uiHeight    = pOrigYuv->getHeight(COMPONENT_Y);
  Pel*  piSrc       = pOrigYuv->getAddr(COMPONENT_Y);
  UInt  uiSrcStride = pOrigYuv->getStride(COMPONENT_Y);
  Pel*  piDst       = pOrigYuvTemp->getAddr(COMPONENT_Y);
  UInt  uiDstStride = pOrigYuvTemp->getStride(COMPONENT_Y);
  
  UInt  uiMaskStride= MAX_CU_SIZE;
  
  AOF( uiWidth == uiHeight );
  
  // backup pointer
  Bool* pMaskStart = pMask;
  
  for (Int y=0; y<uiHeight; y++)
  {
    for (Int x=0; x<uiWidth; x++)
    {
      UChar ucSegment = (UChar)pMask[x];
      AOF( ucSegment < 2 );
      
      piDst[x] = (ucSegment==uiValidSegment)?piSrc[x]:DBBP_INVALID_SHORT;
    }
    
    piSrc  += uiSrcStride;
    piDst  += uiDstStride;
    pMask  += uiMaskStride;
  }
  
  // now invalidate chroma
  Pel*  piSrcU       = pOrigYuv->getAddr(COMPONENT_Cb);
  Pel*  piSrcV       = pOrigYuv->getAddr(COMPONENT_Cr);
  UInt  uiSrcStrideC = pOrigYuv->getStride(COMPONENT_Cb);
  Pel*  piDstU       = pOrigYuvTemp->getAddr(COMPONENT_Cb);
  Pel*  piDstV       = pOrigYuvTemp->getAddr(COMPONENT_Cr);
  UInt  uiDstStrideC = pOrigYuvTemp->getStride(COMPONENT_Cb);
  pMask = pMaskStart;
  
  for (Int y=0; y<uiHeight/2; y++)
  {
    for (Int x=0; x<uiWidth/2; x++)
    {
      UChar ucSegment = (UChar)pMask[x*2];
      AOF( ucSegment < 2 );
      
      piDstU[x] = (ucSegment==uiValidSegment)?piSrcU[x]:DBBP_INVALID_SHORT;
      piDstV[x] = (ucSegment==uiValidSegment)?piSrcV[x]:DBBP_INVALID_SHORT;
    }
    
    piSrcU  += uiSrcStrideC;
    piSrcV  += uiSrcStrideC;
    piDstU  += uiDstStrideC;
    piDstV  += uiDstStrideC;
    pMask   += 2*uiMaskStride;
  }
}
#endif

#if NH_3D_DBBP
Void TEncCu::xCheckRDCostInterDBBP( TComDataCU*& rpcBestCU, TComDataCU*& rpcTempCU  DEBUG_STRING_FN_DECLARE(sDebug), Bool bUseMRG )
{
  DEBUG_STRING_NEW(sTest)
  AOF( !rpcTempCU->getSlice()->getIsDepth() );
  
  UChar uhDepth = rpcTempCU->getDepth( 0 );
  
#if NH_3D_VSO
  if( m_pcRdCost->getUseRenModel() )
  {
    UInt  uiWidth     = m_ppcOrigYuv[uhDepth]->getWidth ( COMPONENT_Y );
    UInt  uiHeight    = m_ppcOrigYuv[uhDepth]->getHeight( COMPONENT_Y );
    Pel*  piSrc       = m_ppcOrigYuv[uhDepth]->getAddr  ( COMPONENT_Y );
    UInt  uiSrcStride = m_ppcOrigYuv[uhDepth]->getStride( COMPONENT_Y );
    m_pcRdCost->setRenModelData( rpcTempCU, 0, piSrc, uiSrcStride, uiWidth, uiHeight );
  }
#endif
  
  UInt uiWidth  = rpcTempCU->getWidth(0);
  UInt uiHeight = rpcTempCU->getHeight(0);
  AOF( uiWidth == uiHeight );
  
#if NH_3D_DBBP
  if(uiWidth <= 8)
  {
    return;
  }
#endif
  
  rpcTempCU->setPartSizeSubParts( SIZE_2Nx2N,  0, uhDepth );
  
  // fetch virtual depth block
  UInt uiDepthStride = 0;
#if H_3D_FCO
  Pel* pDepthPels = rpcTempCU->getVirtualDepthBlock(rpcTempCU->getZorderIdxInCU(), uiWidth, uiHeight, uiDepthStride);
#else
  Pel* pDepthPels = rpcTempCU->getVirtualDepthBlock(0, uiWidth, uiHeight, uiDepthStride);
#endif
  AOF( pDepthPels != NULL );
  AOF( uiDepthStride != 0 );
  
  PartSize eVirtualPartSize = m_pcPredSearch->getPartitionSizeFromDepth(pDepthPels, uiDepthStride, uiWidth, rpcTempCU);

  // derive partitioning from depth
  Bool pMask[MAX_CU_SIZE*MAX_CU_SIZE];
  Bool bValidMask = m_pcPredSearch->getSegmentMaskFromDepth(pDepthPels, uiDepthStride, uiWidth, uiHeight, pMask, rpcTempCU);
  
  if( !bValidMask )
  {
    return;
  }
  
  // find optimal motion/disparity vector for each segment
  DisInfo originalDvInfo = rpcTempCU->getDvInfo(0);
  DbbpTmpData* pDBBPTmpData = rpcTempCU->getDBBPTmpData();
  TComYuv* apPredYuv[2] = { m_ppcRecoYuvTemp[uhDepth], m_ppcPredYuvTemp[uhDepth] };
  
  // find optimal motion vector fields for both segments (as 2Nx2N)
  rpcTempCU->setDepthSubParts( uhDepth, 0 );
  rpcTempCU->setPartSizeSubParts( SIZE_2Nx2N,  0, uhDepth );
  rpcTempCU->setPredModeSubParts( MODE_INTER, 0, uhDepth );
  for( UInt uiSegment = 0; uiSegment < 2; uiSegment++ )
  {
    rpcTempCU->setDBBPFlagSubParts(true, 0, 0, uhDepth);
    rpcTempCU->setDvInfoSubParts(originalDvInfo, 0, uhDepth);
    
    // invalidate all other segments in original YUV
    xInvalidateOriginalSegments(m_ppcOrigYuv[uhDepth], m_ppcOrigYuvDBBP[uhDepth], pMask, uiSegment);
    
    // do motion estimation for this segment
    m_pcRdCost->setUseMask(true);
    rpcTempCU->getDBBPTmpData()->eVirtualPartSize = eVirtualPartSize;
    rpcTempCU->getDBBPTmpData()->uiVirtualPartIndex = uiSegment;
    m_pcPredSearch->predInterSearch( rpcTempCU, m_ppcOrigYuvDBBP[uhDepth], apPredYuv[uiSegment], m_ppcResiYuvTemp[uhDepth], m_ppcResiYuvTemp[uhDepth] DEBUG_STRING_PASS_INTO(sTest), false, bUseMRG );
    m_pcRdCost->setUseMask(false);
    
    // extract motion parameters of full block for this segment
    pDBBPTmpData->auhInterDir[uiSegment] = rpcTempCU->getInterDir(0);
    
    pDBBPTmpData->abMergeFlag[uiSegment] = rpcTempCU->getMergeFlag(0);
    pDBBPTmpData->auhMergeIndex[uiSegment] = rpcTempCU->getMergeIndex(0);
    
#if NH_3D_VSP
    AOF( rpcTempCU->getSPIVMPFlag(0) == false );
    AOF( rpcTempCU->getVSPFlag(0) == 0 );
#endif
    
    for ( UInt uiRefListIdx = 0; uiRefListIdx < 2; uiRefListIdx++ )
    {
      RefPicList eRefList = (RefPicList)uiRefListIdx;
      
      pDBBPTmpData->acMvd[uiSegment][eRefList] = rpcTempCU->getCUMvField(eRefList)->getMvd(0);
      pDBBPTmpData->aiMvpNum[uiSegment][eRefList] = rpcTempCU->getMVPNum(eRefList, 0);
      pDBBPTmpData->aiMvpIdx[uiSegment][eRefList] = rpcTempCU->getMVPIdx(eRefList, 0);
      
      rpcTempCU->getMvField(rpcTempCU, 0, eRefList, pDBBPTmpData->acMvField[uiSegment][eRefList]);
    }
  }
  
  // store final motion/disparity information in each PU using derived partitioning
  rpcTempCU->setDepthSubParts( uhDepth, 0 );
  rpcTempCU->setPartSizeSubParts  ( eVirtualPartSize,  0, uhDepth );
  rpcTempCU->setPredModeSubParts  ( MODE_INTER, 0, uhDepth );
  
  UInt uiPUOffset = ( g_auiPUOffset[UInt( eVirtualPartSize )] << ( ( rpcTempCU->getSlice()->getSPS()->getMaxTotalCUDepth() - uhDepth ) << 1 ) ) >> 4;
  for( UInt uiSegment = 0; uiSegment < 2; uiSegment++ )
  {
    UInt uiPartAddr = uiSegment*uiPUOffset;
    
    rpcTempCU->setDBBPFlagSubParts(true, uiPartAddr, uiSegment, uhDepth);
    
    // now set stored information from 2Nx2N motion search to each partition
    rpcTempCU->setInterDirSubParts(pDBBPTmpData->auhInterDir[uiSegment], uiPartAddr, uiSegment, uhDepth); // interprets depth relative to LCU level
    
    rpcTempCU->setMergeFlagSubParts(pDBBPTmpData->abMergeFlag[uiSegment], uiPartAddr, uiSegment, uhDepth);
    rpcTempCU->setMergeIndexSubParts(pDBBPTmpData->auhMergeIndex[uiSegment], uiPartAddr, uiSegment, uhDepth);
        
    for ( UInt uiRefListIdx = 0; uiRefListIdx < 2; uiRefListIdx++ )
    {
      RefPicList eRefList = (RefPicList)uiRefListIdx;
      
      rpcTempCU->getCUMvField( eRefList )->setAllMvd(pDBBPTmpData->acMvd[uiSegment][eRefList], eVirtualPartSize, uiPartAddr, 0, uiSegment);
      rpcTempCU->setMVPNum(eRefList, uiPartAddr, pDBBPTmpData->aiMvpNum[uiSegment][eRefList]);
      rpcTempCU->setMVPIdx(eRefList, uiPartAddr, pDBBPTmpData->aiMvpIdx[uiSegment][eRefList]);
      
      rpcTempCU->getCUMvField( eRefList )->setAllMvField( pDBBPTmpData->acMvField[uiSegment][eRefList], eVirtualPartSize, uiPartAddr, 0, uiSegment ); // interprets depth relative to rpcTempCU level
    }
  }
  
  // reconstruct final prediction signal by combining both segments
  Int bitDepthY = rpcTempCU->getSlice()->getSPS()->getBitDepth(CHANNEL_TYPE_LUMA);
  m_pcPredSearch->combineSegmentsWithMask(apPredYuv, m_ppcPredYuvTemp[uhDepth], pMask, uiWidth, uiHeight, 0, eVirtualPartSize, bitDepthY);
  m_pcPredSearch->encodeResAndCalcRdInterCU( rpcTempCU, m_ppcOrigYuv[uhDepth], m_ppcPredYuvTemp[uhDepth], m_ppcResiYuvTemp[uhDepth], m_ppcResiYuvBest[uhDepth], m_ppcRecoYuvTemp[uhDepth], false DEBUG_STRING_PASS_INTO(sTest) );
  
  xCheckDQP( rpcTempCU );
  xCheckBestMode(rpcBestCU, rpcTempCU, uhDepth  DEBUG_STRING_PASS_INTO(sDebug) DEBUG_STRING_PASS_INTO(sTest) );
}
#endif
#if NH_3D_DIS
Void TEncCu::xCheckRDCostDIS( TComDataCU*& rpcBestCU, TComDataCU*& rpcTempCU, PartSize eSize DEBUG_STRING_FN_DECLARE(sDebug) )
{
  DEBUG_STRING_NEW(sTest)
  UInt uiDepth = rpcTempCU->getDepth( 0 );
  if( !rpcBestCU->getSlice()->getIsDepth() || (eSize != SIZE_2Nx2N))
  {
    return;
  }

#if NH_MV
  D_PRINT_INC_INDENT(g_traceModeCheck, "xCheckRDCostDIS" );
#endif

#if NH_3D_VSO // M5
  if( m_pcRdCost->getUseRenModel() )
  {
    UInt  uiWidth     = m_ppcOrigYuv[uiDepth]->getWidth   ( COMPONENT_Y );
    UInt  uiHeight    = m_ppcOrigYuv[uiDepth]->getHeight  ( COMPONENT_Y );
    Pel*  piSrc       = m_ppcOrigYuv[uiDepth]->getAddr    ( COMPONENT_Y );
    UInt  uiSrcStride = m_ppcOrigYuv[uiDepth]->getStride  ( COMPONENT_Y );
    m_pcRdCost->setRenModelData( rpcTempCU, 0, piSrc, uiSrcStride, uiWidth, uiHeight );
  }
#endif

  rpcTempCU->setSkipFlagSubParts( false, 0, uiDepth );
  rpcTempCU->setPartSizeSubParts( SIZE_2Nx2N, 0, uiDepth );
  rpcTempCU->setPredModeSubParts( MODE_INTRA, 0, uiDepth );
  rpcTempCU->setCUTransquantBypassSubParts( rpcTempCU->getCUTransquantBypass(0), 0, uiDepth );

  rpcTempCU->setTrIdxSubParts(0, 0, uiDepth);
  rpcTempCU->setCbfSubParts(0, COMPONENT_Y, 0, uiDepth);
  rpcTempCU->setDISFlagSubParts(true, 0, uiDepth);
  rpcTempCU->setIntraDirSubParts(CHANNEL_TYPE_LUMA, DC_IDX, 0, uiDepth);
#if NH_3D_SDC_INTRA
  rpcTempCU->setSDCFlagSubParts( false, 0, uiDepth);
#endif

  UInt uiPreCalcDistC;
  m_pcPredSearch  ->estIntraPredDIS      ( rpcTempCU, m_ppcOrigYuv[uiDepth], m_ppcPredYuvTemp[uiDepth], m_ppcResiYuvTemp[uiDepth], m_ppcRecoYuvTemp[uiDepth], uiPreCalcDistC, false );

#if ENC_DEC_TRACE && NH_MV_ENC_DEC_TRAC
  Int oldTraceCopyBack = g_traceCopyBack; 
  g_traceCopyBack = false;  
#endif
  m_ppcRecoYuvTemp[uiDepth]->copyToPicComponent(COMPONENT_Y, rpcTempCU->getPic()->getPicYuvRec(), rpcTempCU->getCtuRsAddr(), rpcTempCU->getZorderIdxInCtu() );
#if ENC_DEC_TRACE && NH_MV_ENC_DEC_TRAC  
  g_traceCopyBack = oldTraceCopyBack; 
#endif

  m_pcEntropyCoder->resetBits();
  if ( rpcTempCU->getSlice()->getPPS()->getTransquantBypassEnableFlag())
  {
    m_pcEntropyCoder->encodeCUTransquantBypassFlag( rpcTempCU, 0,          true );
  }
  m_pcEntropyCoder->encodeSkipFlag ( rpcTempCU, 0,          true );
  m_pcEntropyCoder->encodeDIS( rpcTempCU, 0,          true );

  m_pcRDGoOnSbacCoder->store(m_pppcRDSbacCoder[uiDepth][CI_TEMP_BEST]);

  rpcTempCU->getTotalBits() = m_pcEntropyCoder->getNumberOfWrittenBits();
  rpcTempCU->getTotalBins() = ((TEncBinCABAC *)((TEncSbac*)m_pcEntropyCoder->m_pcEntropyCoderIf)->getEncBinIf())->getBinsCoded();

#if NH_3D_VSO // M6
  if( m_pcRdCost->getUseLambdaScaleVSO())
  {
    rpcTempCU->getTotalCost() = m_pcRdCost->calcRdCostVSO( rpcTempCU->getTotalBits(), rpcTempCU->getTotalDistortion() );  
  }
  else
#endif
  rpcTempCU->getTotalCost() = m_pcRdCost->calcRdCost( rpcTempCU->getTotalBits(), rpcTempCU->getTotalDistortion() );

  xCheckDQP( rpcTempCU );

  // // 获得DIS模式的RDcost
  // if (myFlag[2]) {
	//   features[20] = rpcTempCU->getTotalCost();
  // }

  xCheckBestMode(rpcBestCU, rpcTempCU, uiDepth  DEBUG_STRING_PASS_INTO(sDebug) DEBUG_STRING_PASS_INTO(sTest) );
#if NH_MV
  D_DEC_INDENT( g_traceModeCheck );  
#endif
}
#endif
Void TEncCu::xCheckRDCostIntra( TComDataCU *&rpcBestCU,
                                TComDataCU *&rpcTempCU,
                                Double      &cost,
                                PartSize     eSize
                                DEBUG_STRING_FN_DECLARE(sDebug)
#if NH_3D_ENC_DEPTH
                              , Bool bOnlyIVP
#endif
                              )
{
  DEBUG_STRING_NEW(sTest)

  if(getFastDeltaQp())
  {
    const TComSPS &sps=*(rpcTempCU->getSlice()->getSPS());
    const UInt fastDeltaQPCuMaxSize = Clip3(sps.getMaxCUHeight()>>(sps.getLog2DiffMaxMinCodingBlockSize()), sps.getMaxCUHeight(), 32u);
    if(rpcTempCU->getWidth( 0 ) > fastDeltaQPCuMaxSize)
    {
      return; // only check necessary 2Nx2N Intra in fast deltaqp mode
    }
  }
#if NH_MV
  D_PRINT_INC_INDENT (g_traceModeCheck, "xCheckRDCostIntra; eSize: " + n2s(eSize) );
#endif

  UInt uiDepth = rpcTempCU->getDepth( 0 );
#if NH_3D_VSO // M5
  if( m_pcRdCost->getUseRenModel() )
  {
    UInt  uiWidth     = m_ppcOrigYuv[uiDepth]->getWidth   ( COMPONENT_Y );
    UInt  uiHeight    = m_ppcOrigYuv[uiDepth]->getHeight  ( COMPONENT_Y );
    Pel*  piSrc       = m_ppcOrigYuv[uiDepth]->getAddr    ( COMPONENT_Y );
    UInt  uiSrcStride = m_ppcOrigYuv[uiDepth]->getStride  ( COMPONENT_Y );
    m_pcRdCost->setRenModelData( rpcTempCU, 0, piSrc, uiSrcStride, uiWidth, uiHeight );
  }
#endif

  rpcTempCU->setSkipFlagSubParts( false, 0, uiDepth );
#if NH_3D_DIS
  rpcTempCU->setDISFlagSubParts( false, 0, uiDepth );
#endif


  rpcTempCU->setPartSizeSubParts( eSize, 0, uiDepth );
  rpcTempCU->setPredModeSubParts( MODE_INTRA, 0, uiDepth );
  rpcTempCU->setChromaQpAdjSubParts( rpcTempCU->getCUTransquantBypass(0) ? 0 : m_cuChromaQpOffsetIdxPlus1, 0, uiDepth );

  Pel resiLuma[NUMBER_OF_STORED_RESIDUAL_TYPES][MAX_CU_SIZE * MAX_CU_SIZE];

  m_pcPredSearch->estIntraPredLumaQT( rpcTempCU, m_ppcOrigYuv[uiDepth], m_ppcPredYuvTemp[uiDepth], m_ppcResiYuvTemp[uiDepth], m_ppcRecoYuvTemp[uiDepth], resiLuma DEBUG_STRING_PASS_INTO(sTest) 
#if NH_3D_ENC_DEPTH
                                    , bOnlyIVP
#endif
                                    );

  m_ppcRecoYuvTemp[uiDepth]->copyToPicComponent(COMPONENT_Y, rpcTempCU->getPic()->getPicYuvRec(), rpcTempCU->getCtuRsAddr(), rpcTempCU->getZorderIdxInCtu() );

  if (rpcBestCU->getPic()->getChromaFormat()!=CHROMA_400)
  {
    m_pcPredSearch->estIntraPredChromaQT( rpcTempCU, m_ppcOrigYuv[uiDepth], m_ppcPredYuvTemp[uiDepth], m_ppcResiYuvTemp[uiDepth], m_ppcRecoYuvTemp[uiDepth], resiLuma DEBUG_STRING_PASS_INTO(sTest) );
  }
  
#if NH_3D_SDC_INTRA
  if( rpcTempCU->getSDCFlag( 0 ) )
  {
    assert( rpcTempCU->getTransformIdx(0) == 0 );
    assert( rpcTempCU->getCbf(0, COMPONENT_Y) == 1 );
  }
#endif

  m_pcEntropyCoder->resetBits();

  if ( rpcTempCU->getSlice()->getPPS()->getTransquantBypassEnableFlag())
  {
    m_pcEntropyCoder->encodeCUTransquantBypassFlag( rpcTempCU, 0,          true );
  }
  m_pcEntropyCoder->encodeSkipFlag ( rpcTempCU, 0,          true );
#if NH_3D_DIS
  m_pcEntropyCoder->encodeDIS( rpcTempCU, 0,          true );
  if(!rpcTempCU->getDISFlag(0))
  {
#endif
    m_pcEntropyCoder->encodePredMode( rpcTempCU, 0,          true );
    m_pcEntropyCoder->encodePartSize( rpcTempCU, 0, uiDepth, true );
    m_pcEntropyCoder->encodePredInfo( rpcTempCU, 0 );
    m_pcEntropyCoder->encodeIPCMInfo(rpcTempCU, 0, true );
#if NH_3D_SDC_INTRA
    m_pcEntropyCoder->encodeSDCFlag( rpcTempCU, 0, true );
#endif

    // Encode Coefficients
    Bool bCodeDQP = getdQPFlag();
    Bool codeChromaQpAdjFlag = getCodeChromaQpAdjFlag();
    m_pcEntropyCoder->encodeCoeff( rpcTempCU, 0, uiDepth, bCodeDQP, codeChromaQpAdjFlag );
    setCodeChromaQpAdjFlag( codeChromaQpAdjFlag );
    setdQPFlag( bCodeDQP );
#if NH_3D_DIS
  }
#endif
  m_pcRDGoOnSbacCoder->store(m_pppcRDSbacCoder[uiDepth][CI_TEMP_BEST]);

  rpcTempCU->getTotalBits() = m_pcEntropyCoder->getNumberOfWrittenBits();
  rpcTempCU->getTotalBins() = ((TEncBinCABAC *)((TEncSbac*)m_pcEntropyCoder->m_pcEntropyCoderIf)->getEncBinIf())->getBinsCoded();
#if NH_3D_VSO // M6
  if( m_pcRdCost->getUseLambdaScaleVSO())  
  {
    rpcTempCU->getTotalCost() = m_pcRdCost->calcRdCostVSO( rpcTempCU->getTotalBits(), rpcTempCU->getTotalDistortion() );  
  }
  else
#endif
    rpcTempCU->getTotalCost() = m_pcRdCost->calcRdCost( rpcTempCU->getTotalBits(), rpcTempCU->getTotalDistortion() );

  xCheckDQP( rpcTempCU );

  cost = rpcTempCU->getTotalCost();

  xCheckBestMode(rpcBestCU, rpcTempCU, uiDepth DEBUG_STRING_PASS_INTO(sDebug) DEBUG_STRING_PASS_INTO(sTest));

#if NH_MV
  D_DEC_INDENT( g_traceModeCheck ); 
#endif
}


/** Check R-D costs for a CU with PCM mode.
 * \param rpcBestCU pointer to best mode CU data structure
 * \param rpcTempCU pointer to testing mode CU data structure
 * \returns Void
 *
 * \note Current PCM implementation encodes sample values in a lossless way. The distortion of PCM mode CUs are zero. PCM mode is selected if the best mode yields bits greater than that of PCM mode.
 */
Void TEncCu::xCheckIntraPCM( TComDataCU*& rpcBestCU, TComDataCU*& rpcTempCU )
{
  if(getFastDeltaQp())
  {
    const TComSPS &sps=*(rpcTempCU->getSlice()->getSPS());
    const UInt fastDeltaQPCuMaxPCMSize = Clip3((UInt)1<<sps.getPCMLog2MinSize(), (UInt)1<<sps.getPCMLog2MaxSize(), 32u);
    if (rpcTempCU->getWidth( 0 ) > fastDeltaQPCuMaxPCMSize)
    {
      return;   // only check necessary PCM in fast deltaqp mode
    }
  }
  
  UInt uiDepth = rpcTempCU->getDepth( 0 );

#if NH_3D_VSO // VERY NEW
  if( m_pcRdCost->getUseRenModel() )
  {
    UInt  uiWidth     = m_ppcOrigYuv[uiDepth]->getWidth   ( COMPONENT_Y );
    UInt  uiHeight    = m_ppcOrigYuv[uiDepth]->getHeight  ( COMPONENT_Y );
    Pel*  piSrc       = m_ppcOrigYuv[uiDepth]->getAddr    ( COMPONENT_Y );
    UInt  uiSrcStride = m_ppcOrigYuv[uiDepth]->getStride  ( COMPONENT_Y );
    m_pcRdCost->setRenModelData( rpcTempCU, 0, piSrc, uiSrcStride, uiWidth, uiHeight );
  }
#endif
  rpcTempCU->setSkipFlagSubParts( false, 0, uiDepth );
#if NH_3D_DIS
  rpcTempCU->setDISFlagSubParts( false, 0, uiDepth );
#endif
  rpcTempCU->setIPCMFlag(0, true);
  rpcTempCU->setIPCMFlagSubParts (true, 0, rpcTempCU->getDepth(0));
  rpcTempCU->setPartSizeSubParts( SIZE_2Nx2N, 0, uiDepth );
  rpcTempCU->setPredModeSubParts( MODE_INTRA, 0, uiDepth );
  rpcTempCU->setTrIdxSubParts ( 0, 0, uiDepth );
  rpcTempCU->setChromaQpAdjSubParts( rpcTempCU->getCUTransquantBypass(0) ? 0 : m_cuChromaQpOffsetIdxPlus1, 0, uiDepth );

  m_pcPredSearch->IPCMSearch( rpcTempCU, m_ppcOrigYuv[uiDepth], m_ppcPredYuvTemp[uiDepth], m_ppcResiYuvTemp[uiDepth], m_ppcRecoYuvTemp[uiDepth]);

  m_pcRDGoOnSbacCoder->load(m_pppcRDSbacCoder[uiDepth][CI_CURR_BEST]);

  m_pcEntropyCoder->resetBits();

  if ( rpcTempCU->getSlice()->getPPS()->getTransquantBypassEnableFlag())
  {
    m_pcEntropyCoder->encodeCUTransquantBypassFlag( rpcTempCU, 0,          true );
  }

  m_pcEntropyCoder->encodeSkipFlag ( rpcTempCU, 0,          true );
#if NH_3D_DIS
  m_pcEntropyCoder->encodeDIS( rpcTempCU, 0,          true );
#endif
  m_pcEntropyCoder->encodePredMode ( rpcTempCU, 0,          true );
  m_pcEntropyCoder->encodePartSize ( rpcTempCU, 0, uiDepth, true );
  m_pcEntropyCoder->encodeIPCMInfo ( rpcTempCU, 0, true );
#if NH_3D_SDC_INTRA
  m_pcEntropyCoder->encodeSDCFlag( rpcTempCU, 0, true );
#endif
  m_pcRDGoOnSbacCoder->store(m_pppcRDSbacCoder[uiDepth][CI_TEMP_BEST]);

  rpcTempCU->getTotalBits() = m_pcEntropyCoder->getNumberOfWrittenBits();
  rpcTempCU->getTotalBins() = ((TEncBinCABAC *)((TEncSbac*)m_pcEntropyCoder->m_pcEntropyCoderIf)->getEncBinIf())->getBinsCoded();
#if NH_3D_VSO // M44
  if ( m_pcRdCost->getUseVSO() )
  {
    rpcTempCU->getTotalCost() = m_pcRdCost->calcRdCostVSO( rpcTempCU->getTotalBits(), rpcTempCU->getTotalDistortion() );
  }
  else
#endif
    rpcTempCU->getTotalCost() = m_pcRdCost->calcRdCost( rpcTempCU->getTotalBits(), rpcTempCU->getTotalDistortion() );

  xCheckDQP( rpcTempCU );
  DEBUG_STRING_NEW(a)
  DEBUG_STRING_NEW(b)
  xCheckBestMode(rpcBestCU, rpcTempCU, uiDepth DEBUG_STRING_PASS_INTO(a) DEBUG_STRING_PASS_INTO(b));
}

/** check whether current try is the best with identifying the depth of current try
 * \param rpcBestCU
 * \param rpcTempCU
 * \param uiDepth
 */
Void TEncCu::xCheckBestMode( TComDataCU*& rpcBestCU, TComDataCU*& rpcTempCU, UInt uiDepth DEBUG_STRING_FN_DECLARE(sParent) DEBUG_STRING_FN_DECLARE(sTest) DEBUG_STRING_PASS_INTO(Bool bAddSizeInfo) )
{
  if( rpcTempCU->getTotalCost() < rpcBestCU->getTotalCost() )
  {
    TComYuv* pcYuv;
    // Change Information data
    TComDataCU* pcCU = rpcBestCU;
    rpcBestCU = rpcTempCU;
    rpcTempCU = pcCU;

    // Change Prediction data
    pcYuv = m_ppcPredYuvBest[uiDepth];
    m_ppcPredYuvBest[uiDepth] = m_ppcPredYuvTemp[uiDepth];
    m_ppcPredYuvTemp[uiDepth] = pcYuv;

    // Change Reconstruction data
    pcYuv = m_ppcRecoYuvBest[uiDepth];
    m_ppcRecoYuvBest[uiDepth] = m_ppcRecoYuvTemp[uiDepth];
    m_ppcRecoYuvTemp[uiDepth] = pcYuv;

    pcYuv = NULL;
    pcCU  = NULL;

    // store temp best CI for next CU coding
    m_pppcRDSbacCoder[uiDepth][CI_TEMP_BEST]->store(m_pppcRDSbacCoder[uiDepth][CI_NEXT_BEST]);


#if DEBUG_STRING
    DEBUG_STRING_SWAP(sParent, sTest)
    const PredMode predMode=rpcBestCU->getPredictionMode(0);
    if ((DebugOptionList::DebugString_Structure.getInt()&DebugStringGetPredModeMask(predMode)) && bAddSizeInfo)
    {
      std::stringstream ss(stringstream::out);
      ss <<"###: " << (predMode==MODE_INTRA?"Intra   ":"Inter   ") << partSizeToString[rpcBestCU->getPartitionSize(0)] << " CU at " << rpcBestCU->getCUPelX() << ", " << rpcBestCU->getCUPelY() << " width=" << UInt(rpcBestCU->getWidth(0)) << std::endl;
      sParent+=ss.str();
    }
#endif
  }
}

Void TEncCu::xCheckDQP( TComDataCU* pcCU )
{
  UInt uiDepth = pcCU->getDepth( 0 );

  const TComPPS &pps = *(pcCU->getSlice()->getPPS());
  if ( pps.getUseDQP() && uiDepth <= pps.getMaxCuDQPDepth() )
  {
    if ( pcCU->getQtRootCbf( 0) )
    {
      m_pcEntropyCoder->resetBits();
      m_pcEntropyCoder->encodeQP( pcCU, 0, false );
      pcCU->getTotalBits() += m_pcEntropyCoder->getNumberOfWrittenBits(); // dQP bits
      pcCU->getTotalBins() += ((TEncBinCABAC *)((TEncSbac*)m_pcEntropyCoder->m_pcEntropyCoderIf)->getEncBinIf())->getBinsCoded();
#if NH_3D_VSO // M45
      if ( m_pcRdCost->getUseVSO() )      
      {
        pcCU->getTotalCost() = m_pcRdCost->calcRdCostVSO( pcCU->getTotalBits(), pcCU->getTotalDistortion() );      
      }
      else
#endif
        pcCU->getTotalCost() = m_pcRdCost->calcRdCost( pcCU->getTotalBits(), pcCU->getTotalDistortion() );
    }
    else
    {
      pcCU->setQPSubParts( pcCU->getRefQP( 0 ), 0, uiDepth ); // set QP to default QP
    }
  }
}

Void TEncCu::xCopyAMVPInfo (AMVPInfo* pSrc, AMVPInfo* pDst)
{
  pDst->iN = pSrc->iN;
  for (Int i = 0; i < pSrc->iN; i++)
  {
    pDst->m_acMvCand[i] = pSrc->m_acMvCand[i];
  }
}
Void TEncCu::xCopyYuv2Pic(TComPic* rpcPic, UInt uiCUAddr, UInt uiAbsPartIdx, UInt uiDepth, UInt uiSrcDepth )
{
  UInt uiAbsPartIdxInRaster = g_auiZscanToRaster[uiAbsPartIdx];
  UInt uiSrcBlkWidth = rpcPic->getNumPartInCtuWidth() >> (uiSrcDepth);
  UInt uiBlkWidth    = rpcPic->getNumPartInCtuWidth() >> (uiDepth);
  UInt uiPartIdxX = ( ( uiAbsPartIdxInRaster % rpcPic->getNumPartInCtuWidth() ) % uiSrcBlkWidth) / uiBlkWidth;
  UInt uiPartIdxY = ( ( uiAbsPartIdxInRaster / rpcPic->getNumPartInCtuWidth() ) % uiSrcBlkWidth) / uiBlkWidth;
  UInt uiPartIdx = uiPartIdxY * ( uiSrcBlkWidth / uiBlkWidth ) + uiPartIdxX;
  m_ppcRecoYuvBest[uiSrcDepth]->copyToPicYuv( rpcPic->getPicYuvRec (), uiCUAddr, uiAbsPartIdx, uiDepth - uiSrcDepth, uiPartIdx);

#if ENC_DEC_TRACE && NH_MV_ENC_DEC_TRAC
  Bool oldtraceCopyBack = g_traceCopyBack;
  g_traceCopyBack = false; 
#endif
  m_ppcPredYuvBest[uiSrcDepth]->copyToPicYuv( rpcPic->getPicYuvPred (), uiCUAddr, uiAbsPartIdx, uiDepth - uiSrcDepth, uiPartIdx);

#if ENC_DEC_TRACE && NH_MV_ENC_DEC_TRAC
  g_traceCopyBack = oldtraceCopyBack; 
#endif
}

Void TEncCu::xCopyYuv2Tmp( UInt uiPartUnitIdx, UInt uiNextDepth )
{
  UInt uiCurrDepth = uiNextDepth - 1;
  m_ppcRecoYuvBest[uiNextDepth]->copyToPartYuv( m_ppcRecoYuvTemp[uiCurrDepth], uiPartUnitIdx );
  m_ppcPredYuvBest[uiNextDepth]->copyToPartYuv( m_ppcPredYuvBest[uiCurrDepth], uiPartUnitIdx);
}

/** Function for filling the PCM buffer of a CU using its original sample array
 * \param pCU pointer to current CU
 * \param pOrgYuv pointer to original sample array
 */
Void TEncCu::xFillPCMBuffer     ( TComDataCU* pCU, TComYuv* pOrgYuv )
{
  const ChromaFormat format = pCU->getPic()->getChromaFormat();
  const UInt numberValidComponents = getNumberValidComponents(format);
  for (UInt componentIndex = 0; componentIndex < numberValidComponents; componentIndex++)
  {
    const ComponentID component = ComponentID(componentIndex);

    const UInt width  = pCU->getWidth(0)  >> getComponentScaleX(component, format);
    const UInt height = pCU->getHeight(0) >> getComponentScaleY(component, format);

    Pel *source      = pOrgYuv->getAddr(component, 0, width);
    Pel *destination = pCU->getPCMSample(component);

    const UInt sourceStride = pOrgYuv->getStride(component);

    for (Int line = 0; line < height; line++)
    {
      for (Int column = 0; column < width; column++)
      {
        destination[column] = source[column];
      }

      source      += sourceStride;
      destination += width;
    }
  }
}

#if ADAPTIVE_QP_SELECTION
/** Collect ARL statistics from one block
  */
Int TEncCu::xTuCollectARLStats(TCoeff* rpcCoeff, TCoeff* rpcArlCoeff, Int NumCoeffInCU, Double* cSum, UInt* numSamples )
{
  for( Int n = 0; n < NumCoeffInCU; n++ )
  {
    TCoeff u = abs( rpcCoeff[ n ] );
    TCoeff absc = rpcArlCoeff[ n ];

    if( u != 0 )
    {
      if( u < LEVEL_RANGE )
      {
        cSum[ u ] += ( Double )absc;
        numSamples[ u ]++;
      }
      else
      {
        cSum[ LEVEL_RANGE ] += ( Double )absc - ( Double )( u << ARL_C_PRECISION );
        numSamples[ LEVEL_RANGE ]++;
      }
    }
  }

  return 0;
}

//! Collect ARL statistics from one CTU
Void TEncCu::xCtuCollectARLStats(TComDataCU* pCtu )
{
  Double cSum[ LEVEL_RANGE + 1 ];     //: the sum of DCT coefficients corresponding to data type and quantization output
  UInt numSamples[ LEVEL_RANGE + 1 ]; //: the number of coefficients corresponding to data type and quantization output

  TCoeff* pCoeffY = pCtu->getCoeff(COMPONENT_Y);
  TCoeff* pArlCoeffY = pCtu->getArlCoeff(COMPONENT_Y);
  const TComSPS &sps = *(pCtu->getSlice()->getSPS());

  const UInt uiMinCUWidth = sps.getMaxCUWidth() >> sps.getMaxTotalCUDepth(); // NOTE: ed - this is not the minimum CU width. It is the square-root of the number of coefficients per part.
  const UInt uiMinNumCoeffInCU = 1 << uiMinCUWidth;                          // NOTE: ed - what is this?

  memset( cSum, 0, sizeof( Double )*(LEVEL_RANGE+1) );
  memset( numSamples, 0, sizeof( UInt )*(LEVEL_RANGE+1) );

  // Collect stats to cSum[][] and numSamples[][]
  for(Int i = 0; i < pCtu->getTotalNumPart(); i ++ )
  {
    UInt uiTrIdx = pCtu->getTransformIdx(i);

    if(pCtu->isInter(i) && pCtu->getCbf( i, COMPONENT_Y, uiTrIdx ) )
    {
      xTuCollectARLStats(pCoeffY, pArlCoeffY, uiMinNumCoeffInCU, cSum, numSamples);
    }//Note that only InterY is processed. QP rounding is based on InterY data only.

    pCoeffY  += uiMinNumCoeffInCU;
    pArlCoeffY  += uiMinNumCoeffInCU;
  }

  for(Int u=1; u<LEVEL_RANGE;u++)
  {
    m_pcTrQuant->getSliceSumC()[u] += cSum[ u ] ;
    m_pcTrQuant->getSliceNSamples()[u] += numSamples[ u ] ;
  }
  m_pcTrQuant->getSliceSumC()[LEVEL_RANGE] += cSum[ LEVEL_RANGE ] ;
  m_pcTrQuant->getSliceNSamples()[LEVEL_RANGE] += numSamples[ LEVEL_RANGE ] ;
}
#endif

//! \}
