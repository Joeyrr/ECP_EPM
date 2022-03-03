# 统计 CU 和 PU 分布

from lightgbm import LGBMClassifier
import optuna
from sklearn import tree
import graphviz
import pydotplus
import numpy as np
import pandas as pd
import xgboost as xgb
from xgboost import XGBClassifier
from xgboost import plot_importance
from xgboost import plot_tree
from sklearn.model_selection import train_test_split,cross_val_score
from sklearn.metrics import classification_report
from sklearn.metrics import accuracy_score
from sklearn.metrics import mean_squared_error
from six import StringIO
import time
import matplotlib.pyplot as plt
import m2cgen as m2c
# 样本均衡（过采样）
from imblearn.over_sampling import SMOTE
from pylab import *
import xlwt
mpl.rcParams['font.sans-serif'] = ['SimSun']
import random

featureNames_IND = [ 'deltaPOC_t0', 'SG_t0', 'Dt_t0', 'Dt_cc_t0', 'Pt_t0','Pt_cc_t0',
                     'MergeFlag_cc_t0' ,'SkipFlag_cc_t0', 'SDCFlag_cc_t0', 'DISFlag_cc_t0', 'MSMFlag_cc_t0',
                     'deltaPOC_t1', 'SG_t1', 'Dt_t1', 'Dt_cc_t1', 'Pt_t1', 'Pt_cc_t1',
                     'MergeFlag_cc_t1', 'SkipFlag_cc_t1', 'SDCFlag_cc_t1', 'DISFlag_cc_t1', 'MSMFlag_cc_t1',
                     # 'VSG', 'Dv', 'Dv_cc', 'Pv', 'Pv_cc', 'MergeFlag_cc_v', 'SkipFlag_cc_v', 'SDCFlag_cc_v', 'DISFlag_cc_v', 'MSMFlag_cc_v',
                     'Dc', 'Dc_cc', 'Pc', 'Pc_cc', 'MergeFlag_c', 'SkipFlag_c', 'SDCFlag_c', 'DISFlag_c', 'MSMFlag_c',
                     'MergeFlag_cc_c', 'SkipFlag_cc_c', 'SDCFlag_cc_c', 'DISFlag_cc_c', 'MSMFlag_cc_c', 'Hc',
                     'Ds_GAMMA', 'Ds4mean', 'Ps4mean', 'MergeFlag_s', 'SkipFlag_s', 'SDCFlag_s', 'DISFlag_s', 'MSMFlag_s',
                     'curMean', 'curVar', 'Hcur', 'curSubmvar', 'Ratio_curSubmvar', 'curSubdmean', 'Norm_curSubdmean', 'curSubdvar', 'Norm_curSubdvar',
                     'QP', 'QP_config', 'RD_MSM', 'SkipFlag', 'SDCFlag', 'TotalBins', 'TotalBits', 'TotalDistortion', 'POC', 'isB_SLICE']
# 34 个
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


CUlabel = ['split']
PUlabel = ['isMSM']

path = 'D:/data_add_P_SLICE/'
outPath = 'D:/data_add_P_SLICE/raw_data/'

AllSeq = ['balloons', 'newspaper', 'kendo', 'lovebird',
          'PoznanStreet', 'Poznan_CarPark', 'PoznanHall2', 'shark', 'undodancer', 'gtfly']
# AllSeq = ['balloons', 'newspaper', 'kendo', 'lovebird',
#           'PoznanStreet', 'Poznan_CarPark', 'PoznanHall2', 'shark']
trainSeq = ['kendo', 'lovebird', 'PoznanStreet', 'Poznan_CarPark']
# trainSeq = ['balloons', 'lovebird', 'PoznanHall2', 'Poznan_CarPark']
# trainSeq = ['kendo', 'PoznanStreet']
testSeq = [seq for seq in AllSeq if seq not in trainSeq]
print('trainSeq: ')
print(trainSeq)
print('testSeq: ')
print(testSeq)

workspace = 'D:/data_add_P_SLICE/workspace/'
fileNames = ['/raw_data_IND_64.txt', '/raw_data_IND_32.txt', '/raw_data_IND_16.txt', '/raw_data_IND_8.txt',
             '/raw_data_DEP_64.txt', '/raw_data_DEP_32.txt', '/raw_data_DEP_16.txt', '/raw_data_DEP_8.txt']
# fileNames = fileNames_CU
# haveRemoveFeatures = False
remove_features = {}

# 使用XGBoost
# TreeNums = list(range(10, 151, 10))
TreeNums = [100]
# TreeNums.append()
# TreeNums = [50]
TuningData = {}
# Depths = [5, 3, 3, 5, 4, 3]
# Threshold = [0.3, 0.2, 0.1, 0.3, 0.2, 0.1]
# lr = 0.1
haveAllData =  True
# haveSampleData = False
# alphas = [0.8897504451116652, 0.6028593336466398, 0.5515386558031316, 0.8913030561458449, 0.6192668621231824, 0.6365181490485372]
# alphas = []

# # Groud Truth Results:
# if not haveAllData:
#
#     for i in range(1,9):
#         print('==================== Tree-' + str(i) + ' ======================')
#         if i < 5:
#             features = featureNames_IND
#         else:
#             features = featureNames_DEP
#         # features = simpleFeatures
#
#         raw_data = pd.read_csv(path + trainSeq[0] + fileNames[i-1], sep=' ', index_col=False, encoding='utf-8')
#         for seq in trainSeq[1:]:
#             raw_data = pd.concat([raw_data, pd.read_csv(path + seq + fileNames[i-1], sep=' ', index_col=False, encoding='utf-8')], ignore_index=True)
#         print(raw_data)
#
#         raw_data.to_csv(outPath + 'train_data_' + str(i) + '.txt', sep=' ', index=False, encoding='utf-8')
#         print('训练数据保存完成')
#         print(raw_data.shape)
#         if i % 4 != 0:
#             ratio = float(len(raw_data[raw_data['split']==0]))/float(len(raw_data[raw_data['split']==1]))
#             print('正负样本失衡比例： %f' % ratio)
#
#         raw_data = pd.read_csv(path + testSeq[0] + fileNames[i-1], sep=' ', index_col=False, encoding='utf-8')
#         for seq in testSeq[1:]:
#             raw_data = pd.concat([raw_data, pd.read_csv(path + seq + fileNames[i-1], sep=' ', index_col=False, encoding='utf-8')], ignore_index=True)
#         print(raw_data)
#
#         raw_data.to_csv(outPath + 'test_data_' + str(i) + '.txt', sep=' ', index=False, encoding='utf-8')
#         print('测试数据保存完成')
#         print(raw_data.shape)
#         if i % 4 != 0:
#             ratio = float(len(raw_data[raw_data['split']==0]))/float(len(raw_data[raw_data['split']==1]))
#             print('正负样本失衡比例： %f' % ratio)
#
#
# else:
#     # # 首先将过分相似的数据剔除
#     # th = 1
#     # train_data_similar = train_data[train_data['SG']<th]
#     # train_data_similar = train_data_similar.sample(frac=0.1, random_state=314)
#     # train_data_notSimilar = train_data[train_data['SG']>=th]
#     # train_data = pd.concat([train_data_similar, train_data_notSimilar], ignore_index=True)
#     # acc_test = []
#     # falseRatio = []
#     blockSize = ['64x64', '32x32', '16x16', '8x8', '64x64', '32x32', '16x16', '8x8']
#     columnName = ['split', 'isMSM', 'POC', 'SliceType']
#     results = {}
#     QPs = [34, 39, 42, 45]
#     for qp in QPs:
#         # totalNum = [0, 0, 0]
#         # unSplitNum = [0, 0, 0]
#         totalNum = [0, 0, 0, 0]
#         unSplitNum = [0, 0, 0, 0]
#
#         raw_data = pd.read_csv(outPath + 'test_data_4.txt', names=(featureNames_IND + CUlabel + PUlabel), sep=' ', index_col=False, encoding='utf-8')
#         raw_data = raw_data[raw_data['QP_config'] == qp]
#         totalNum[3] = totalNum[3] + len(raw_data)
#         # unSplitNum[3] = unSplitNum[3] + len(raw_data[raw_data['split']==0])
#         unSplitNum[3] = unSplitNum[3] + len(raw_data[raw_data['isMSM']==1])
#         raw_data = pd.read_csv(outPath + 'test_data_8.txt', names=(featureNames_DEP + CUlabel + PUlabel), sep=' ', index_col=False, encoding='utf-8')
#         raw_data = raw_data[raw_data['QP_config'] == qp]
#         totalNum[3] = totalNum[3] + len(raw_data)
#         # unSplitNum[3] = unSplitNum[3] + len(raw_data[raw_data['split']==0])
#         unSplitNum[3] = unSplitNum[3] + len(raw_data[raw_data['isMSM']==1])
#
#         raw_data = pd.read_csv(outPath + 'test_data_3.txt', names=(featureNames_IND + CUlabel + PUlabel), sep=' ', index_col=False, encoding='utf-8')
#         raw_data = raw_data[raw_data['QP_config'] == qp]
#         totalNum[2] = totalNum[2] + len(raw_data)
#         # unSplitNum[2] = unSplitNum[2] + len(raw_data[raw_data['split']==0])
#         unSplitNum[2] = unSplitNum[2] + len(raw_data[raw_data['isMSM']==1])
#         raw_data = pd.read_csv(outPath + 'test_data_7.txt', names=(featureNames_DEP + CUlabel + PUlabel), sep=' ', index_col=False, encoding='utf-8')
#         raw_data = raw_data[raw_data['QP_config'] == qp]
#         totalNum[2] = totalNum[2] + len(raw_data)
#         # unSplitNum[2] = unSplitNum[2] + len(raw_data[raw_data['split']==0])
#         unSplitNum[2] = unSplitNum[2] + len(raw_data[raw_data['isMSM']==1])
#
#         raw_data = pd.read_csv(outPath + 'test_data_2.txt', names=(featureNames_IND + CUlabel + PUlabel), sep=' ', index_col=False, encoding='utf-8')
#         raw_data = raw_data[raw_data['QP_config'] == qp]
#         totalNum[1] = totalNum[1] + len(raw_data)
#         # unSplitNum[1] = unSplitNum[1] + len(raw_data[raw_data['split']==0])
#         unSplitNum[1] = unSplitNum[1] + len(raw_data[raw_data['isMSM']==1])
#         raw_data = pd.read_csv(outPath + 'test_data_6.txt', names=(featureNames_DEP + CUlabel + PUlabel), sep=' ', index_col=False, encoding='utf-8')
#         raw_data = raw_data[raw_data['QP_config'] == qp]
#         totalNum[1] = totalNum[1] + len(raw_data)
#         # unSplitNum[1] = unSplitNum[1] + len(raw_data[raw_data['split']==0])
#         unSplitNum[1] = unSplitNum[1] + len(raw_data[raw_data['isMSM']==1])
#
#         raw_data = pd.read_csv(outPath + 'test_data_1.txt', names=(featureNames_IND + CUlabel + PUlabel), sep=' ', index_col=False, encoding='utf-8')
#         raw_data = raw_data[raw_data['QP_config'] == qp]
#         totalNum[0] = totalNum[0] + len(raw_data)
#         # unSplitNum[0] = unSplitNum[0] + len(raw_data[raw_data['split']==0])
#         unSplitNum[0] = unSplitNum[0] + len(raw_data[raw_data['isMSM']==1])
#         raw_data = pd.read_csv(outPath + 'test_data_5.txt', names=(featureNames_DEP + CUlabel + PUlabel), sep=' ', index_col=False, encoding='utf-8')
#         raw_data = raw_data[raw_data['QP_config'] == qp]
#         totalNum[0] = totalNum[0] + len(raw_data)
#         # unSplitNum[0] = unSplitNum[0] + len(raw_data[raw_data['split']==0])
#         unSplitNum[0] = unSplitNum[0] + len(raw_data[raw_data['isMSM']==1])
#
#         results[qp] = [a/b for a, b in zip(unSplitNum, totalNum)]
#     print(results)
# # 顺序：16 32 64
# # {34: [0.7340056795256559, 0.6886726876903694, 0.8444349541838657], 39: [0.8016386314845512, 0.7696895731630993, 0.9419754915632408], 42: [0.8462398466733595, 0.8212327873131893, 0.9777416562078743], 45: [0.9021937842778793, 0.869463535700649, 0.9923912285594757]}
# # 顺序：64 32 16 8
# {34: [0.8604862253993102, 0.7178719172825263, 0.639551863138034, 0.5452349123875343], 39: [0.9341328499047088, 0.7087808236324947, 0.6208320878284735, 0.5332471402150832], 42: [0.9641219843815367, 0.6928147229654767, 0.6040887104134344, 0.5318667458079834], 45: [0.9835767907776694, 0.6961149293623521, 0.6242230347349177, 0.5857476635514018]}

# path = 'D:/DCC_statistical/'
# # My Results:
# QPs = ['/25_34', '/30_39', '/35_42', '/40_45']
# timeTag = {'balloons': ['27246791', '27246832', '27246873', '27246897'],
#            'gtfly': ['27246791', '27246911', '27246963', '27247008'],
#            'newspaper': ['27246791', '27246829', '27246867', '27246890'],
#            'PoznanHall2': ['27246791', '27246910', '27246960', '27247004'],
#            'shark': ['27246791', '27246934', '27247001', '27247057'],
#            'undodancer': ['27246791', '27246917', '27250800', '27247005']}
#
# results = {}
# columnName = ['split', 'isMSM', 'POC', 'SliceType']
# cnt = 0
# for qp in QPs:
#     # Size = ['16', '32', '64']
#     Size = ['64', '32', '16', '8']
#     result = []
#     for size in Size:
#         FileNum = ['0', '1', '2']
#         totalNum = 0
#         unSplit = 0
#         # raw_data = pd.DataFrame(columns=columnName)
#         for fileNum in FileNum:
#             ViewId = ['1', '0', '2']
#             for view in ViewId:
#                 for seq in testSeq:
#                     tmpData = pd.read_csv(
#                             path + seq + qp + '/flieNum_' + fileNum + '_Label_ViewId_' + view + '_Size_' + size + '_' + timeTag[seq][
#                                 cnt] + '.txt', names=columnName, sep=' ', index_col=False, encoding='utf-8')
#                     data = tmpData[tmpData['split'] != 2]
#                     totalNum = totalNum + len(data)
#                     # unSplit = unSplit + len(data[data['split'] == 0])
#                     unSplit = unSplit + len(data[data['isMSM'] == 1])
#                     # raw_data = pd.concat([raw_data, tmpData], ignore_index=True)
#         result.append(unSplit/totalNum)
#     results[cnt] = result
#     cnt = cnt + 1
#
# print(results)
#
# # 顺序： 16 32 64
# # {0: [0.7513221340799502, 0.7134452096008558, 0.8740738869805348], 1: [0.8335245525470399, 0.8166689102779835, 0.9583052693024404], 2: [0.8879803761242846, 0.8834127740705434, 0.9852804157966817], 3: [0.9265913757700205, 0.9329939460649422, 0.9949007655867628]}
# # 顺序：64 32 16 8
# # {0: [0.8974315798926832, 0.7881092465066524, 0.7024760849276714, 0.5776778733385457], 1: [0.9498411575851463, 0.7511274146866797, 0.6428407526388251, 0.5133011716057891], 2: [0.9747255337778676, 0.7061010486177312, 0.5742027800490597, 0.4500456204379562], 3: [0.9886144226667565, 0.7143643368189323, 0.601129363449692, 0.5297202797202797]}




PU_origianl =  {34: [0.8604862253993102, 0.7178719172825263, 0.639551863138034, 0.5452349123875343], 39: [0.9341328499047088, 0.7087808236324947, 0.6208320878284735, 0.5332471402150832], 42: [0.9641219843815367, 0.6928147229654767, 0.6040887104134344, 0.5318667458079834], 45: [0.9835767907776694, 0.6961149293623521, 0.6242230347349177, 0.5857476635514018]}
PU_mine = {0: [0.8974315798926832, 0.7881092465066524, 0.7024760849276714, 0.5776778733385457], 1: [0.9498411575851463, 0.7511274146866797, 0.6428407526388251, 0.5133011716057891], 2: [0.9747255337778676, 0.7061010486177312, 0.5742027800490597, 0.4500456204379562], 3: [0.9886144226667565, 0.7143643368189323, 0.601129363449692, 0.5297202797202797]}


f = xlwt.Workbook()
sheet1 = f.add_sheet(r'sheet1', cell_overwrite_ok=True)

row = 0
for key, value in PU_mine.items():
    col = 0
    sheet1.write(row, col, key)
    for v in value:
        col = col + 1
        sheet1.write(row, col, v)
    row = row + 1
f.save('CU.xls')
