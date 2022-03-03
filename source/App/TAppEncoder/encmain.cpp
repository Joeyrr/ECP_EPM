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

/** \file     encmain.cpp
    \brief    Encoder application main
*/

#include <time.h>
#include <iostream>
#include "TAppEncTop.h"
#include "TAppCommon/program_options_lite.h"

//! \ingroup TAppEncoder
//! \{

#include "../Lib/TLibCommon/Debug.h"
#include "TLibCommon/TComStatistics.h"

// ====================================================================================================================
// Main function
// ====================================================================================================================

int main(int argc, char* argv[])
{
  TAppEncTop  cTAppEncTop;

  // print information
  fprintf( stdout, "\n" );
#if NH_MV
  fprintf( stdout, "3D-HTM Software: Encoder Version [%s] based on HM Version [%s]", NV_VERSION, HM_VERSION );  
#else
  fprintf( stdout, "HM software: Encoder Version [%s] (including RExt)", NV_VERSION );
#endif
  fprintf( stdout, NVM_ONOS );
  fprintf( stdout, NVM_COMPILEDBY );
  fprintf( stdout, NVM_BITS );
  fprintf( stdout, "\n\n" );

  // create application encoder class
  cTAppEncTop.create();

  // parse configuration
  try
  {
    if(!cTAppEncTop.parseCfg( argc, argv ))
    {
      cTAppEncTop.destroy();
#if ENVIRONMENT_VARIABLE_DEBUG_AND_TEST
      EnvVar::printEnvVar();
#endif
      return 1;
    }
  }
  catch (df::program_options_lite::ParseFailure &e)
  {
    std::cerr << "Error parsing option \""<< e.arg <<"\" with argument \""<< e.val <<"\"." << std::endl;
    return 1;
  }

#if PRINT_MACRO_VALUES
  printMacroSettings();
#endif

#if ENVIRONMENT_VARIABLE_DEBUG_AND_TEST
  EnvVar::printEnvVarInUse();
#endif

  // starting time
  Double dResult;
  clock_t lBefore = clock();

#if PRINT_FEATURE && PRINT_LABEL
  // 给输出文件添加后缀名
  time_t timep = time(0); // 当前时间戳 seconds since 01-Jan-1970
  time_tag_filename_suffix = timep / 60; // min since 01-Jan-1970
  string time_tag = to_string((int)time_tag_filename_suffix);

  // feature0.open(("Label_ViewId_0_" + time_tag + ".txt"), ios::app);
  // feature1.open(("Label_ViewId_1_" + time_tag + ".txt"), ios::app);
  // feature2.open(("Label_ViewId_2_" + time_tag + ".txt"), ios::app);
  // feature_Ds0.open(("Feature_Ds_ViewId_0_" + time_tag + ".txt"), ios::app);
  // feature_Ds1.open(("Feature_Ds_ViewId_1_" + time_tag + ".txt"), ios::app);
  // feature_Ds2.open(("Feature_Ds_ViewId_2_" + time_tag + ".txt"), ios::app);
  // fLabel0.open(("Label_ViewId_0_" + time_tag + ".txt"), ios::app);;
  // fLabel1.open(("Label_ViewId_1_" + time_tag + ".txt"), ios::app);;
  // fLabel2.open(("Label_ViewId_2_" + time_tag + ".txt"), ios::app);;
  // fPUModeLabel0.open(("PUMode_Label_ViewId_0_" + time_tag + ".txt"), ios::app);;
  // fPUModeLabel1.open(("PUMode_Label_ViewId_1_" + time_tag + ".txt"), ios::app);;
  // fPUModeLabel2.open(("PUMode_Label_ViewId_2_" + time_tag + ".txt"), ios::app);;
  // fPUSizeLabel0.open(("PUSize_Label_ViewId_0_" + time_tag + ".txt"), ios::app);
  // fPUSizeLabel1.open(("PUSize_Label_ViewId_1_" + time_tag + ".txt"), ios::app);
  // fPUSizeLabel2.open(("PUSize_Label_ViewId_2_" + time_tag + ".txt"), ios::app);

  // feature0.flags(ios::fixed); feature0.precision(4); //设置输出精度
  // feature1.flags(ios::fixed); feature1.precision(4); //设置输出精度
  // feature2.flags(ios::fixed); feature2.precision(4); //设置输出精度
  // feature_Ds0.flags(ios::fixed); feature_Ds0.precision(4); //设置输出精度
  // feature_Ds1.flags(ios::fixed); feature_Ds1.precision(4); //设置输出精度
  // feature_Ds2.flags(ios::fixed); feature_Ds2.precision(4); //设置输出精度

#endif

  // call encoding function
  cTAppEncTop.encode();

// #if PRINT_FEATURE && PRINT_LABEL && PRINT_PU_LABEL
//   feature0.flush(); feature0.close();
//   feature1.flush(); feature1.close();
//   feature2.flush(); feature2.close();
//   feature_Ds0.flush(); feature_Ds0.close();
//   feature_Ds1.flush(); feature_Ds1.close();
//   feature_Ds2.flush(); feature_Ds2.close();
//   fLabel0.flush(); fLabel0.close();
//   fLabel1.flush(); fLabel1.close();
//   fLabel2.flush(); fLabel2.close();
//   fPUModeLabel0.flush(); fPUModeLabel0.close();
//   fPUModeLabel1.flush(); fPUModeLabel1.close();
//   fPUModeLabel2.flush(); fPUModeLabel2.close();
//   fPUSizeLabel0.flush(); fPUSizeLabel0.close();
//   fPUSizeLabel1.flush(); fPUSizeLabel1.close();
//   fPUSizeLabel2.flush(); fPUSizeLabel2.close();
// #endif

  // ending time
  dResult = (Double)(clock()-lBefore) / CLOCKS_PER_SEC;
  printf("\n Total Time: %12.3f sec.\n", dResult);

  // destroy application encoder class
  cTAppEncTop.destroy();

  return 0;
}

//! \}
