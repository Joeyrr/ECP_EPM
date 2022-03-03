# ECU 是主体，EPU 是陪衬
# 4：6 设置训练集和测试集
# 舍去RDO剔除的数据，分序列构建数据集


# 新增了lovebird和Poznan_CarPark两条序列

import pandas as pd


# 74 个 raw 特征
raw_featureNames = [ 'deltaPOC_t0', 'SG_t0', 'Dt_t0', 'Dt_cc_t0', 'Pt_t0' ,'Pt_cc_t0',
                     'MergeFlag_cc_t0' ,'SkipFlag_cc_t0', 'SDCFlag_cc_t0', 'DISFlag_cc_t0', 'MSMFlag_cc_t0',
                     'deltaPOC_t1', 'SG_t1', 'Dt_t1', 'Dt_cc_t1', 'Pt_t1', 'Pt_cc_t1',
                     'MergeFlag_cc_t1', 'SkipFlag_cc_t1', 'SDCFlag_cc_t1', 'DISFlag_cc_t1', 'MSMFlag_cc_t1',
                     'VSG', 'Dv', 'Dv_cc', 'Pv', 'Pv_cc', 'MergeFlag_cc_v', 'SkipFlag_cc_v', 'SDCFlag_cc_v', 'DISFlag_cc_v', 'MSMFlag_cc_v',
                     'Dc', 'Dc_cc', 'Pc', 'Pc_cc', 'MergeFlag_c', 'SkipFlag_c', 'SDCFlag_c', 'DISFlag_c', 'MSMFlag_c',
                     'MergeFlag_cc_c', 'SkipFlag_cc_c', 'SDCFlag_cc_c', 'DISFlag_cc_c', 'MSMFlag_cc_c', 'Hc',
                     'Ds_GAMMA', 'Ds4mean', 'Ps4mean', 'MergeFlag_s', 'SkipFlag_s', 'SDCFlag_s', 'DISFlag_s', 'MSMFlag_s',
                     'curMean', 'curVar', 'Hcur', 'curSubmvar', 'Ratio_curSubmvar', 'curSubdmean', 'Norm_curSubdmean', 'curSubdvar', 'Norm_curSubdvar',
                     'QP', 'QP_config', 'RD_MSM', 'SkipFlag', 'SDCFlag', 'TotalBins', 'TotalBits', 'TotalDistortion', 'POC', 'isB_SLICE']


labels = ['split', 'isMSM']

# 64 个
featureNames_IND = [ 'deltaPOC_t0', 'SG_t0', 'Dt_t0', 'Dt_cc_t0', 'Pt_t0' ,'Pt_cc_t0',
                     'MergeFlag_cc_t0' ,'SkipFlag_cc_t0', 'SDCFlag_cc_t0', 'DISFlag_cc_t0', 'MSMFlag_cc_t0',
                     'deltaPOC_t1', 'SG_t1', 'Dt_t1', 'Dt_cc_t1', 'Pt_t1', 'Pt_cc_t1',
                     'MergeFlag_cc_t1', 'SkipFlag_cc_t1', 'SDCFlag_cc_t1', 'DISFlag_cc_t1', 'MSMFlag_cc_t1',
                     # 'VSG', 'Dv', 'Dv_cc', 'Pv', 'Pv_cc', 'MergeFlag_cc_v', 'SkipFlag_cc_v', 'SDCFlag_cc_v', 'DISFlag_cc_v', 'MSMFlag_cc_v',
                     'Dc', 'Dc_cc', 'Pc', 'Pc_cc', 'MergeFlag_c', 'SkipFlag_c', 'SDCFlag_c', 'DISFlag_c', 'MSMFlag_c',
                     'MergeFlag_cc_c', 'SkipFlag_cc_c', 'SDCFlag_cc_c', 'DISFlag_cc_c', 'MSMFlag_cc_c', 'Hc',
                     'Ds_GAMMA', 'Ds4mean', 'Ps4mean', 'MergeFlag_s', 'SkipFlag_s', 'SDCFlag_s', 'DISFlag_s', 'MSMFlag_s',
                     'curMean', 'curVar', 'Hcur', 'curSubmvar', 'Ratio_curSubmvar', 'curSubdmean', 'Norm_curSubdmean', 'curSubdvar', 'Norm_curSubdvar',
                     'QP', 'QP_config', 'RD_MSM', 'SkipFlag', 'SDCFlag', 'TotalBins', 'TotalBits', 'TotalDistortion', 'POC', 'isB_SLICE']
# 74 个
featureNames_DEP = [ 'deltaPOC_t0', 'SG_t0', 'Dt_t0', 'Dt_cc_t0', 'Pt_t0' ,'Pt_cc_t0',
                     'MergeFlag_cc_t0' ,'SkipFlag_cc_t0', 'SDCFlag_cc_t0', 'DISFlag_cc_t0', 'MSMFlag_cc_t0',
                     'deltaPOC_t1', 'SG_t1', 'Dt_t1', 'Dt_cc_t1', 'Pt_t1', 'Pt_cc_t1',
                     'MergeFlag_cc_t1', 'SkipFlag_cc_t1', 'SDCFlag_cc_t1', 'DISFlag_cc_t1', 'MSMFlag_cc_t1',
                     'VSG', 'Dv', 'Dv_cc', 'Pv', 'Pv_cc', 'MergeFlag_cc_v', 'SkipFlag_cc_v', 'SDCFlag_cc_v', 'DISFlag_cc_v', 'MSMFlag_cc_v',
                     'Dc', 'Dc_cc', 'Pc', 'Pc_cc', 'MergeFlag_c', 'SkipFlag_c', 'SDCFlag_c', 'DISFlag_c', 'MSMFlag_c',
                     'MergeFlag_cc_c', 'SkipFlag_cc_c', 'SDCFlag_cc_c', 'DISFlag_cc_c', 'MSMFlag_cc_c', 'Hc',
                     'Ds_GAMMA', 'Ds4mean', 'Ps4mean', 'MergeFlag_s', 'SkipFlag_s', 'SDCFlag_s', 'DISFlag_s', 'MSMFlag_s',
                     'curMean', 'curVar', 'Hcur', 'curSubmvar', 'Ratio_curSubmvar', 'curSubdmean', 'Norm_curSubdmean', 'curSubdvar', 'Norm_curSubdvar',
                     'QP', 'QP_config', 'RD_MSM', 'SkipFlag', 'SDCFlag', 'TotalBins', 'TotalBits', 'TotalDistortion', 'POC', 'isB_SLICE']


# balloons / Kendo
timeTags = ['27205346', '27205346', '27205346', '27205346']
videoInfos = [{'EncodedFrames': 100, 'width': 1024, 'height': 768, 'QP_config': 34, 'QP_dir': '25_34/'}
             ,{'EncodedFrames': 100, 'width': 1024, 'height': 768, 'QP_config': 39, 'QP_dir': '30_39/'}
             ,{'EncodedFrames': 100, 'width': 1024, 'height': 768, 'QP_config': 42, 'QP_dir': '35_42/'}
             ,{'EncodedFrames': 100, 'width': 1024, 'height': 768, 'QP_config': 45, 'QP_dir': '40_45/'}]
path = 'F:/CTC/DCC/balloons/'
outPath = 'F:/CTC/DCC/raw_data/balloons/'



haveRawData = False

# 去除RDO剔除的数据
# 分割独立视点和依赖视点
# fileNum_4_Feature_ViewId_0_Size_32_27205346
CUSize = [64, 32, 16, 8]
# CUSize = [8]
ViewId = [0, 1, 2]
if not haveRawData:

    for size in CUSize:
        raw_data_IND = pd.DataFrame(columns=(featureNames_IND + labels))
        raw_data_DEP = pd.DataFrame(columns=(featureNames_DEP + labels))
        for timeTag, videoInfo in zip(timeTags, videoInfos):
            for fileID in range(4):
                for view in ViewId:
                    featureFileName = path + videoInfo['QP_dir'] + 'fileNum_' + str(fileID) + '_Feature_ViewId_' + str(view) + '_Size_' + str(size) + '_' + timeTag + '.txt'
                    labelFileName = path + videoInfo['QP_dir'] + 'flieNum_' + str(fileID) + '_Label_ViewId_' + str(view) + '_Size_' + str(size) + '_' + timeTag + '.txt'

                    raw_features = pd.read_csv(featureFileName, names=raw_featureNames, sep=' ', index_col=False, encoding='utf-8')
                    label = pd.read_csv(labelFileName, names=labels, sep=' ', index_col=False, encoding='utf-8')
                    buffer = pd.concat([raw_features, label], axis=1, ignore_index=False )
                    # 剔除RDO数据
                    buffer.drop(buffer[buffer['split'] == 2].index.tolist(), axis=0, inplace=True)
                    if size == 8:
                        assert(len(buffer[buffer['split'] == 1]) == 0)
                    # 独立视点剔除视点间相关性特征
                    if view == 1:
                        buffer.drop(['VSG', 'Dv', 'Dv_cc', 'Pv', 'Pv_cc', 'MergeFlag_cc_v', 'SkipFlag_cc_v', 'SDCFlag_cc_v', 'DISFlag_cc_v', 'MSMFlag_cc_v'],
                                    axis=1, inplace=True)
                        raw_data_IND = pd.concat([raw_data_IND, buffer], ignore_index=True)
                    else:
                        raw_data_DEP = pd.concat([raw_data_DEP, buffer], ignore_index=True)
        print('========================== CU_' + str(size) + ' ==========================')
        print('独立视点')
        print(raw_data_IND)
        print(raw_data_IND.shape)
        raw_data_IND.to_csv(outPath + 'raw_data_IND_' + str(size) + '.txt', sep=' ', index=False, encoding='utf-8')
        print('依赖视点')
        print(raw_data_DEP)
        print(raw_data_DEP.shape)
        raw_data_DEP.to_csv(outPath + 'raw_data_DEP_' + str(size) + '.txt', sep=' ', index=False, encoding='utf-8')
