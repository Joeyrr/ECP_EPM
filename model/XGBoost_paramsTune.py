# XGBoost  optuna 调参

# MSM模式后直接判断是否需要划分

# 使用模式选择前可以获得的特征
# depth=3，lr=0.1，TreeNum=50 选前十特征
# 数据下采样处理
# 直接 1：1 分训练集和测试集

# 使用统计权重，beta取1，评价函数使用AUC，手动绘制AUC曲线说明AUC值偏高的原因，

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
mpl.rcParams['font.sans-serif'] = ['SimSun']
import random


# raw_featureNames = [ 'deltaPOC_t0', 'SG_t0', 'Dt_t0', 'Dt_cc_t0', 'Pt_t0' ,'Pt_cc_t0',
#                      'MergeFlag_cc_t0' ,'SkipFlag_cc_t0', 'SDCFlag_cc_t0', 'DISFlag_cc_t0', 'MSMFlag_cc_t0',
#                      'deltaPOC_t1', 'SG_t1', 'Dt_t1', 'Dt_cc_t1', 'Pt_t1', 'Pt_cc_t1',
#                      'MergeFlag_cc_t1', 'SkipFlag_cc_t1', 'SDCFlag_cc_t1', 'DISFlag_cc_t1', 'MSMFlag_cc_t1',
#                      'VSG', 'Dv', 'Dv_cc', 'Pv', 'Pv_cc', 'MergeFlag_cc_v', 'SkipFlag_cc_v', 'SDCFlag_cc_v', 'DISFlag_cc_v', 'MSMFlag_cc_v',
#                      'Dc', 'Dc_cc', 'Pc', 'Pc_cc', 'MergeFlag_c', 'SkipFlag_c', 'SDCFlag_c', 'DISFlag_c', 'MSMFlag_c',
#                      'MergeFlag_cc_c', 'SkipFlag_cc_c', 'SDCFlag_cc_c', 'DISFlag_cc_c', 'MSMFlag_cc_c', 'Hc',
#                      'Ds_GAMMA', 'Ds4mean', 'Ps4mean', 'MergeFlag_s', 'SkipFlag_s', 'SDCFlag_s', 'DISFlag_s', 'MSMFlag_s',
#                      'curMean', 'curVar', 'Hcur', 'curSubmvar', 'Ratio_curSubmvar', 'curSubdmean', 'Norm_curSubdmean', 'curSubdvar', 'Norm_curSubdvar',
#                      'QP', 'QP_config', 'RD_MSM', 'SkipFlag', 'SDCFlag', 'TotalBins', 'TotalBits', 'TotalDistortion', 'POC', 'isB_SLICE']

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



if not haveAllData:

    for i in range(1,9):
        print('==================== Tree-' + str(i) + ' ======================')
        if i < 5:
            features = featureNames_IND
        else:
            features = featureNames_DEP
        # features = simpleFeatures

        raw_data = pd.read_csv(path + trainSeq[0] + fileNames[i-1], sep=' ', index_col=False, encoding='utf-8')
        for seq in trainSeq[1:]:
            raw_data = pd.concat([raw_data, pd.read_csv(path + seq + fileNames[i-1], sep=' ', index_col=False, encoding='utf-8')], ignore_index=True)
        print(raw_data)

        raw_data.to_csv(outPath + 'train_data_' + str(i) + '.txt', sep=' ', index=False, encoding='utf-8')
        print('训练数据保存完成')
        print(raw_data.shape)
        if i % 4 != 0:
            ratio = float(len(raw_data[raw_data['split']==0]))/float(len(raw_data[raw_data['split']==1]))
            print('正负样本失衡比例： %f' % ratio)

        raw_data = pd.read_csv(path + testSeq[0] + fileNames[i-1], sep=' ', index_col=False, encoding='utf-8')
        for seq in testSeq[1:]:
            raw_data = pd.concat([raw_data, pd.read_csv(path + seq + fileNames[i-1], sep=' ', index_col=False, encoding='utf-8')], ignore_index=True)
        print(raw_data)

        raw_data.to_csv(outPath + 'test_data_' + str(i) + '.txt', sep=' ', index=False, encoding='utf-8')
        print('测试数据保存完成')
        print(raw_data.shape)
        if i % 4 != 0:
            ratio = float(len(raw_data[raw_data['split']==0]))/float(len(raw_data[raw_data['split']==1]))
            print('正负样本失衡比例： %f' % ratio)


        # alphas.append(1/(1+1/ratio))
        # print(alphas)

        # 后面做交叉验证，这里不需要划分训练集和测试集
        # # 划分训练集和测试集
        # # shuffle 数据
        # raw_data = raw_data.sample(frac=1, random_state=314)
        # print(raw_data)
        # raw_data.reset_index(drop=True, inplace=True)
        # print(raw_data)
        # data_train = raw_data[raw_data.index<=len(raw_data)/2]
        # data_test = raw_data[raw_data.index>len(raw_data)/2]
        # print(data_train)
        # print(data_test)
        #
        # # 保存训练集和测试集
        # data_train.to_csv(workspace + 'data_train_' + str(i) + '.txt', sep=' ', index=False, encoding='utf-8')
        # data_test.to_csv(workspace + 'data_test_' + str(i) + '.txt', sep=' ', index=False, encoding='utf-8')

# else:
#     for i in range(1, 9):
#
#         print('==================== Tree-' + str(i) + ' ======================')
#         if i < 5:
#             features = featureNames_IND
#         else:
#             features = featureNames_DEP
#         # features = simpleFeatures


else:
    # # 首先将过分相似的数据剔除
    # th = 1
    # train_data_similar = train_data[train_data['SG']<th]
    # train_data_similar = train_data_similar.sample(frac=0.1, random_state=314)
    # train_data_notSimilar = train_data[train_data['SG']>=th]
    # train_data = pd.concat([train_data_similar, train_data_notSimilar], ignore_index=True)
    # acc_test = []
    # falseRatio = []
    blockSize = ['64x64', '32x32', '16x16', '8x8', '64x64', '32x32', '16x16', '8x8']
    for i in range(1, 9):
        train_data = pd.read_csv(outPath + 'train_data_' + str(i) + '.txt', sep=' ', index_col=False, encoding='utf-8')
        test_data = pd.read_csv(outPath + 'test_data_' + str(i) + '.txt', sep=' ', index_col=False, encoding='utf-8')
        if i < 5:
            features = featureNames_IND
            stringTag = 'IND'
        else:
            features = featureNames_DEP
            stringTag = 'DEP'
        print('==================== ' + stringTag + ' blockSize: ' + blockSize[i-1] + ' ======================')

        trainDataECU = train_data
        testDataECU = test_data
        trainDataEPU = train_data.copy(deep=True)
        testDataEPU = test_data.copy(deep=True)
        testDataECU.drop(['isMSM'], axis=1, inplace=True)
        testDataEPU.drop(['split'], axis=1, inplace=True)
        trainDatas = [trainDataECU, trainDataEPU]
        tmps = [0, 1]
        for trainData, tmp in zip(trainDatas, tmps):
            # testData = testDataECU
            if tmp == 0 and blockSize[i-1] == '8x8':
                continue
            if tmp == 0:
                print('============================= ECU ===================================')
                dropTag = 'isMSM'
                tag = 'split'
                testData = testDataECU
                fileTag = 'ECU'
            if tmp == 1:
                print('============================= EPU ===================================')
                dropTag = 'split'
                tag = 'isMSM'
                testData = testDataEPU
                fileTag = 'EPU'

            trainData.drop([dropTag], axis=1, inplace=True)
            # 首先下采样
            # if i % 4 != 0:
            negativeSample = trainData[trainData[tag] == 0]
            positiveSample = trainData[trainData[tag] == 1]
            print('数据总数： %d ' % len(trainData))
            print('正样本个数： %d ' % len(positiveSample))
            print('负样本个数： %d ' % len(negativeSample))
            if len(negativeSample) > len(positiveSample):
                downSample = negativeSample.sample(n=len(positiveSample), random_state=314)
                downSampleData = pd.concat([downSample, positiveSample], ignore_index=True)
            else:
                downSample = positiveSample.sample(n=len(negativeSample), random_state=314)
                downSampleData = pd.concat([downSample, negativeSample], ignore_index=True)
            # shuffle 数据
            trainData = downSampleData.sample(frac=1, random_state=314)
            trainData.reset_index(drop=True, inplace=True)
            # print(trainData)

            negativeSample = testData[testData[tag] == 0]
            positiveSample = testData[testData[tag] == 1]
            print('数据总数： %d ' % len(testData))
            print('正样本个数： %d ' % len(positiveSample))
            print('负样本个数： %d ' % len(negativeSample))
            if len(negativeSample) > len(positiveSample):
                downSample = negativeSample.sample(n=len(positiveSample), random_state=314)
                downSampleData = pd.concat([downSample, positiveSample], ignore_index=True)
            else:
                downSample = positiveSample.sample(n=len(negativeSample), random_state=314)
                downSampleData = pd.concat([downSample, negativeSample], ignore_index=True)
            # shuffle 数据
            testData = downSampleData.sample(frac=1, random_state=314)
            testData.reset_index(drop=True, inplace=True)

            # 训练数据初始化
            print('训练数据初始化')
            data_train = np.array(trainData)
            print('训练数据个数 : %d' % len(trainData))
            print('正样本个数： %d' % len(trainData[trainData[tag]==1]))
            print('负样本个数： %d' % len(trainData[trainData[tag]==0]))
            x_train = data_train[:, 0:-1]
            y_train = data_train[:, -1]
            data_test = np.array(testData)
            # data_test = np.array(train_data)
            x_test = data_test[:, 0:-1]
            y_test = data_test[:, -1]

            print(trainData)

            # # 调优
            # def objective(trial):
            #     # X_train, X_test, y_train, y_test = train_test_split(data, target, train_size=0.3)  # 数据集划分
            #     # param = {
            #     #     'objective': 'binary',
            #     #     'random_state': 314,
            #     #     'learning_rate': trial.suggest_categorical('learning_rate',
            #     #                                                [0.001, 0.01, 0.05, 0.1, 0.12, 0.14, 0.2]),
            #     #     'feature_fraction': trial.suggest_float('feature_fraction', 0.7, 1, step=0.01),
            #     #     'bagging_fraction': trial.suggest_float('bagging_fraction', 0.7, 1, step=0.01),
            #     #     'bagging_freq': trial.suggest_int('bagging_freq', 1, 10),
            #     #     # 'n_estimators': 20000,
            #     #     # 'reg_alpha': trial.suggest_loguniform('reg_alpha', 1e-3, 10.0),
            #     #     # 'reg_lambda': trial.suggest_loguniform('reg_lambda', 1e-3, 10.0),
            #     #     # 'colsample_bytree': trial.suggest_categorical('colsample_bytree',
            #     #     #                                               [0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0]),
            #     #     # 'subsample': trial.suggest_categorical('subsample', [0.4, 0.5, 0.6, 0.7, 0.8, 1.0]),
            #     #
            #     #     # 'max_depth': trial.suggest_categorical('max_depth', [5, 7, 9, 11, 13, 15, 17, 20, 50]),
            #     #     'max_depth': trial.suggest_int('max_depth', 3, 50),
            #     #     'num_leaves': trial.suggest_int('num_leaves', 10, 1000),
            #     #     'num_iterations': trial.suggest_int('num_iterations', 50, 150)
            #     #     # 'min_child_samples': trial.suggest_int('min_child_samples', 1, 100),
            #     #     # 'cat_smooth': trial.suggest_int('cat_smooth', 1, 100)
            #     # }
            #
            #     params = {
            #         'random_state': 314,
            #         'learning_rate': trial.suggest_loguniform('learning_rate', 0.005, 0.5),
            #         'n_estimators': trial.suggest_int('n_estimators', 10, 200),
            #         'max_depth': trial.suggest_int('max_depth', 2, 8),
            #         'alpha': trial.suggest_int('alpha', 1, 5),
            #         'lambda': trial.suggest_int('lambda', 1, 5),
            #         'min_child_weight': trial.suggest_int('min_child_weight', 1, 5),
            #         'gamma': trial.suggest_float('gamma', 0, 1, step=0.01),
            #         #
            #         # 'colsample_bytree': trial.suggest_discrete_uniform('colsample_bytree', 0.1, 1, 0.01),
            #         'nthread': -1
            #     }
            #
            #     xgb = XGBClassifier(**params)
            #     xgb.fit(x_train, y_train, eval_set=[(x_test, y_test)])
            #     y_pred = xgb.predict(x_test)
            #     accuracy = accuracy_score(y_test, y_pred)
            #     # rmse = mean_squared_error(y_test, pred_lgb, squared=False)
            #     return accuracy
            #
            # study = optuna.create_study(direction='minimize')
            # n_trials = 100  # try50次
            # study.optimize(objective, n_trials=n_trials)
            # print('Number of finished trials:', len(study.trials))
            # print("------------------------------------------------")
            # print('Best trial:', study.best_trial.params)
            # print("------------------------------------------------")
            # print(study.trials_dataframe())
            # print("------------------------------------------------")
            # params = study.best_params
            # # params['metric'] = 'rmse'
            # print(params)


            # params = {
            #     'random_state': 314,
            #     # # 'learning_rate': trial.suggest_loguniform('learning_rate', 0.005, 0.5),
            #     # # 'n_estimators': trial.suggest_int('n_estimators', 10, 200),
            #     # 'n_estimators': 100,
            #     # 'max_depth': 3,
            #     # 'verbosity': 0,
            #     # 'silent': 1,
            #     #
            #     # 'colsample_bytree': trial.suggest_discrete_uniform('colsample_bytree', 0.1, 1, 0.01),
            #     'nthread': -1
            # }
            # model = XGBClassifier(**params)
            # clf = LGBMClassifier(
            #     # max_depth=3,
            #     # learning_rate=0.1,
            #     # n_estimators=200,  # 使用多少个弱分类器
            #     # objective='multiclass',
            #     # num_class=2,
            #     # booster='gbdt',
            #     # min_child_weight=2,
            #     # subsample=0.8,
            #     # colsample_bytree=0.8,
            #     # reg_alpha=0,
            #     # reg_lambda=1,
            #     seed=0  # 随机数种子
            # )
            # model.fit(x_train, y_train, eval_set=[(x_train, y_train), (x_test, y_test)])

            # acc_train = []
            # acc_train = []

            # falseP_train = []
            # falseN_train = []
            acc_test = []
            falseP_test = []
            falseN_test = []
            for ntrees in TreeNums:
                model = XGBClassifier(n_estimators=ntrees, max_depth=3, random_state=314, learning_rate=0.1, verbosity=0, silent=0)
                model.fit(x_train, y_train, eval_metric='auc')
                #           verbose=100, early_stopping_rounds=50)
                # 对测试集进行预测
                y_pred = model.predict(x_test)
                # clf.predict_proba
                # 计算准确率
                accuracy = accuracy_score(y_test, y_pred)
                print('测试集准确率')
                print('accuracy:%3.f%%' % (accuracy * 100))
                acc_test.append(accuracy)
                #
                # # # 显示重要特征
                # # plot_importance(clf)
                # # plt.show()
                #
                # # clf = XGBClassifier(n_estimators=ntrees, max_depth=3, random_state=314, learning_rate=0.1, verbosity=0, silent=0)
                # # # clf = XGBClassifier(n_estimators=TreeNums[i-1], max_depth=Depths[i-1], random_state=314, scale_pos_weight=alpha, learning_rate=lr, verbosity=1, silent=0)
                # # # eval_set=['', ''],
                # # clf = clf.fit(x_train, y_train, eval_metric='auc')

                # acc_train.append(accuracy_score(y_train, y_hat))

                totalNum = int(len(x_test))
                positiveTotalNum = int(len(testData[testData[tag]==1]))
                negativeTotalNum = int(len(testData[testData[tag]==0]))
                print('测试总数： %d' % totalNum)
                # # print('划分个数： %d' % )
                # # Whether to output the raw untransformed margin value.
                # predictions = model.predict(x_test, output_margin=True)
                # y_hat_softmax = [1/(1+np.exp(-y)) for y in predictions]
                # y_hat_softmax = y_pred
                # # 单阈值处理
                # # Threshold[i-1]
                y_hat = [0 if y < 0.5 else 1 for y in y_pred]
                # # print(y_test)
                # # print(y_hat)
                hit = 0
                positiveFalse = 0
                negativeFalse = 0
                for label, pred in zip(y_test, y_hat):
                    if label == pred:
                        hit = hit + 1
                    else:
                        if label == 1:
                            positiveFalse = positiveFalse + 1
                        else:
                            negativeFalse = negativeFalse + 1

                print('hit: %.4f %%' % (100.0*hit/totalNum))
                print('positiveFalse/totalNum: %.4f %%' % (100.0*positiveFalse/totalNum))
                print('negativeFalse/totalNum: %.4f %%' % (100.0*negativeFalse/totalNum))
                #
                # # print('positiveFalse/positiveTotalNum: %.4f %%' % (100.0*positiveFalse/positiveTotalNum))
                # # print('negativeFalse/negativeTotalNum: %.4f %%' % (100.0*negativeFalse/negativeTotalNum))
                #
                print(classification_report(y_test, y_hat))
                print("AC", accuracy_score(y_test, y_hat))
                falseP_test.append((100.0*positiveFalse/totalNum))
                falseN_test.append((100.0*negativeFalse/totalNum))

                # feature_importance = dict(zip(features, model.feature_importances_))
                # d_order = sorted(feature_importance.items(), key=lambda x: x[1], reverse=False)
                # remove_features[i] = [d_order[i][0] for i in range(0, len(d_order)) if d_order[i][1] == 0]
                # print('去除重要性为零的特征')


                # # 训练集准确率
                # y_pred = model.predict(x_train)
                # print('训练集准确率')
                # print(classification_report(y_train, y_pred))
                # print("AC", accuracy_score(y_train, y_pred))
                # acc_train.append(accuracy_score(y_train, y_pred))
                #
                # totalNum = int(len(x_train))
                # positiveTotalNum = int(len(trainData[trainData[tag]==1]))
                # negativeTotalNum = int(len(trainData[trainData[tag]==0]))
                # print('训练总数： %d' % totalNum)
                # # # print('划分个数： %d' % )
                # # # Whether to output the raw untransformed margin value.
                # # predictions = model.predict(x_test, output_margin=True)
                # # y_hat_softmax = [1/(1+np.exp(-y)) for y in predictions]
                # # y_hat_softmax = y_pred
                # # # 单阈值处理
                # # # Threshold[i-1]
                # y_hat = [0 if y < 0.5 else 1 for y in y_pred]
                # # # print(y_test)
                # # # print(y_hat)
                # hit = 0
                # positiveFalse = 0
                # negativeFalse = 0
                # for label, pred in zip(y_train, y_hat):
                #     if label == pred:
                #         hit = hit + 1
                #     else:
                #         if label == 1:
                #             positiveFalse = positiveFalse + 1
                #         else:
                #             negativeFalse = negativeFalse + 1
                #
                # print('hit: %.4f %%' % (100.0*hit/totalNum))
                # print('positiveFalse/totalNum: %.4f %%' % (100.0*positiveFalse/totalNum))
                # print('negativeFalse/totalNum: %.4f %%' % (100.0*negativeFalse/totalNum))
                # #
                # # # print('positiveFalse/positiveTotalNum: %.4f %%' % (100.0*positiveFalse/positiveTotalNum))
                # # # print('negativeFalse/negativeTotalNum: %.4f %%' % (100.0*negativeFalse/negativeTotalNum))
                # #
                # print(classification_report(y_train, y_hat))
                # print("AC", accuracy_score(y_train, y_hat))
                # falseP_train.append((100.0*positiveFalse/totalNum))
                # falseN_train.append((100.0*negativeFalse/totalNum))

                # 将模型转化为cpp文件
                code = m2c.export_to_c(model)
                fileName = workspace + 'code/' + fileTag + '_' + str(i) + '.cpp'
                file = open(fileName, 'w')
                file.write(code)
                file.close()

            print(acc_test)
            print(falseP_test)
            print(falseN_test)
            # print(acc_train)
            # print(falseP_train)
            # print(falseN_train)
















            # # acc_train = []
            # # acc_test = []
            # for ntrees in TreeNums:
            #     clf = XGBClassifier(n_estimators=ntrees, max_depth=3, random_state=314, learning_rate=0.1, verbosity=0, silent=0)
            #     # clf = XGBClassifier(n_estimators=TreeNums[i-1], max_depth=Depths[i-1], random_state=314, scale_pos_weight=alpha, learning_rate=lr, verbosity=1, silent=0)
            #     # eval_set=['', ''],
            #     clf = clf.fit(x_train, y_train, eval_metric='auc')
            #     # 训练集准确率
            #     y_hat = clf.predict(x_train)
            #     print('训练集准确率')
            #     print(classification_report(y_train, y_hat))
            #     print("AC", accuracy_score(y_train, y_hat))
            #
            #     # acc_train.append(accuracy_score(y_train, y_hat))
            #
            #     totalNum = int(len(x_test))
            #     positiveTotalNum = int(len(testData[testData[tag]==1]))
            #     negativeTotalNum = int(len(testData[testData[tag]==0]))
            #     print('测试总数： %d' % totalNum)
            #     # print('划分个数： %d' % )
            #     # Whether to output the raw untransformed margin value.
            #     predictions = clf.predict(x_test, output_margin=True)
            #     y_hat_softmax = [1/(1+np.exp(-y)) for y in predictions]
            #     # 单阈值处理
            #     # Threshold[i-1]
            #     y_hat = [0 if y < 0.5 else 1 for y in y_hat_softmax]
            #     # print(y_test)
            #     # print(y_hat)
            #     hit = 0
            #     positiveFalse = 0
            #     negativeFalse = 0
            #     for label, pred in zip(y_test, y_hat):
            #         if label == pred:
            #             hit = hit + 1
            #         else:
            #             if label == 1:
            #                 positiveFalse = positiveFalse + 1
            #             else:
            #                 negativeFalse = negativeFalse + 1
            #
            #     print('hit: %.4f %%' % (100.0*hit/totalNum))
            #     print('positiveFalse/totalNum: %.4f %%' % (100.0*positiveFalse/totalNum))
            #     print('negativeFalse/totalNum: %.4f %%' % (100.0*negativeFalse/totalNum))
            #
            #     # print('positiveFalse/positiveTotalNum: %.4f %%' % (100.0*positiveFalse/positiveTotalNum))
            #     # print('negativeFalse/negativeTotalNum: %.4f %%' % (100.0*negativeFalse/negativeTotalNum))
            #
            #     print(classification_report(y_test, y_hat))
            #     print("AC", accuracy_score(y_test, y_hat))
            #
            #     # acc_test.append(accuracy_score(y_test, y_hat))
            # #     acc_real1.append((100.0*SplitFalse/totalNum))
            # #     acc_real2.append((100.0*notSplitFalse/totalNum))
            # #     acc_real3.append((100.0*SplitFalse/SplitTotalNum))
            # #     acc_real4.append((100.0*notSplitFalse/notSplitTotalNum))
            # #
            # # TuningData[i] = [acc_train, acc_test, acc_real1, acc_real2, acc_real3, acc_real4]
            #
            #     # print(acc_test)
            #
            #     # 绘制重要性图
            #     print('绘制特征重要性直方图')
            #     # ax1 = plt.subplot(2, 3, i)
            #     feat_imp = pd.Series(clf.feature_importances_, features).sort_values(ascending=False)
            #     # print(feat_imp)
            #     plt.figure(dpi=250)
            #     feat_imp.plot(kind='bar', title='特征重要性')
            #     plt.ylabel('特征重要性得分')
            #     plt.gcf().subplots_adjust(bottom=0.32)
            #     # plt.xlabel('(' + str(i) + ')')
            #     # plt.tight_layout()
            #
            #     plt.savefig(workspace + 'ParamsTuningPics/noFilter/Feature_Importances_50_3_' + fileTag + '_' + str(i) + '.png')
            #     # plt.show()
            #     plt.close()
            #
            #     # 将模型转化为cpp文件
            #     code = m2c.export_to_c(clf)
            #     fileName = workspace + 'code/' + fileTag + '_' + str(i) + '.cpp'
            #     file = open(fileName, 'w')
            #     file.write(code)
            #     file.close()

    # 将最终文件整合
    with open(workspace + 'code/clfs.cpp', 'w') as modelFile:
        modelFile.write('#include <math.h>\n')
        modelFile.write('#include <string.h>\n')

        for num in range(1, 9):
            if num % 4 != 0:
                with open(workspace + 'code/ECU_' + str(num) + '.cpp', 'r') as f:
                    fileRowNum = 0
                    for line in f:
                        fileRowNum = fileRowNum + 1
                        if fileRowNum < 3:
                            continue
                        if line.startswith('    memcpy(output, (double[])'):
                            # print(code_output[num-1])
                            # modelFile.write(code_output[num-1])
                            modelFile.write('    *output = var100 < 0.5 ? 0 : 1;\n')
                        elif line.startswith('void score'):
                            modelFile.write('void CUclf' + str(num) + '(double * input, double * output) {\n')
                        else:
                            modelFile.write(line)
                            # print(line)
                modelFile.write('\n')
    with open(workspace + 'code/clfs.cpp', 'a') as modelFile:
        # modelFile.write('#include <math.h>\n')
        # modelFile.write('#include <string.h>\n')

        for num in range(1, 9):
            with open(workspace + 'code/EPU_' + str(num) + '.cpp', 'r') as f:
                fileRowNum = 0
                for line in f:
                    fileRowNum = fileRowNum + 1
                    if fileRowNum < 3:
                        continue
                    if line.startswith('    memcpy(output, (double[])'):
                        # print(code_output[num-1])
                        # modelFile.write(code_output[num-1])
                        modelFile.write('    *output = var100 < 0.5 ? 0 : 1;\n')
                    elif line.startswith('void score'):
                        modelFile.write('void PUclf' + str(num) + '(double * input, double * output) {\n')
                    else:
                        modelFile.write(line)
                        # print(line)
            modelFile.write('\n')

# plt.plot(TreeNums, TuningData[i][0], 'g')
# plt.plot(TreeNums, TuningData[i][1], 'b')
# plt.plot(TreeNums, TuningData[i][2], 'r')
# plt.plot(TreeNums, TuningData[i][3], 'r-')
# plt.plot(TreeNums, TuningData[i][4], 'r*')
# plt.plot(TreeNums, TuningData[i][5], 'r+')
# plt.show()

# feature_importance = dict(zip(features, clf.feature_importances_))
#
# d_order = sorted(feature_importance.items(), key=lambda x: x[1], reverse=False)
# # print(feature_importance)
# # print(d_order)
# #
# # # 临时生成可视化树
# # # plt.figure(dpi=250)
# # plot_tree(clf, num_trees=5)
# # plt.show()
# # plt.close()

# # 绘制重要性图
# print('绘制特征重要性直方图')
# # ax1 = plt.subplot(2, 3, i)
# feat_imp = pd.Series(clf.feature_importances_, features).sort_values(ascending=False)
# # print(feat_imp)
# plt.figure(dpi=250)
# feat_imp.plot(kind='bar', title='特征重要性')
# plt.ylabel('特征重要性得分')
# plt.gcf().subplots_adjust(bottom=0.32)
# # plt.xlabel('(' + str(i) + ')')
# # plt.tight_layout()
#
# plt.savefig(workspace + 'fig/CU_' + str(i) + '.png')
# plt.show()
# plt.close()

# remove_features[i] = [d_order[i][0] for i in range(0, len(d_order)-N_left)]
# print(remove_features[i])
#
# # 确定了需要去除的特征
# # 去除特征后保存数据
# train_data.drop(remove_features[i], axis=1, inplace=True)
# test_data.drop(remove_features[i], axis=1, inplace=True)
# train_data.to_csv(workspace + 'features_reduction_downSampleData_train_' + str(i) + '.txt', sep=' ', index=False, encoding='utf-8')
# test_data.to_csv(workspace + 'features_reduction_downSampleData_test_' + str(i) + '.txt', sep=' ', index=False, encoding='utf-8')

#         # 将模型转化为cpp文件
#         code = m2c.export_to_c(clf)
#         fileName = workspace + 'code/ECU_' + str(i) + '.cpp'
#         file = open(fileName, 'w')
#         file.write(code)
#         file.close()
#
# # 将最终文件整合
# with open(workspace + 'code/ECU.cpp', 'w') as modelFile:
#     modelFile.write('#include <math.h>\n')
#     modelFile.write('#include <string.h>\n')
#
#     for num in range(1, 7):
#         with open(workspace + 'code/ECU_' + str(num) + '.cpp', 'r') as f:
#             fileRowNum = 0
#             for line in f:
#                 fileRowNum = fileRowNum + 1
#                 if fileRowNum < 3:
#                     continue
#                 if line.startswith('    memcpy(output, (double[])'):
#                     # print(code_output[num-1])
#                     # modelFile.write(code_output[num-1])
#                     modelFile.write('    *output = var50 < 0.5 ? 0 : 1;\n')
#                 elif line.startswith('void score'):
#                     modelFile.write('void CUscore' + str(num) + '(double * input, double * output) {\n')
#                 else:
#                     modelFile.write(line)
#                     # print(line)
#         modelFile.write('\n')