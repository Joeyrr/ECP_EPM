# Learning-Based Fast Depth Inter Coding for 3D-HEVC via XGBoost (DCC 2022)

This repository mainly contains five parts, which is modified based on the original HTM-16.0 project.
## 1. makeDataset
The `makeDataset` file folder contains the source code to build the dataset, you can construct the dataset on your own or download the dataset directly. More details can be find in `/makeDataset/README.md`
## 2. model
The `model` file folder contains the python source code which mainly accomplish the following four things.

`XGBoost_paramsTune.py`:
+ Split the dataset into trainset and testset.
+ Build 14 XGBoost models.
+ Convert the XGBoost models into c++ source code which can be implemented into codec directly.
(The converted models can be found in the `model/workspace/code` file folder.)

The `result_statistic.py`:
+ Conduct statistical analysis of the coding result which is produced by `TAppEncoder_statistic.exe`. 
(The TAppEncoder_statistic.exe is modified using ECP+EPM and can export the coding information files.)

## 3. CTC
The `CTC` file folder contains the batch files including `test_50_NeedChangePATH.bat`, `balloons.bat`, etc.
You can run the `test_50_NeedChangePATH.bat` to test the codec preformance using the 8 standard sequences. The `CTC/result` file folder contains the coding results of our ECP+EPM method on our computer. Remember change the file PATH in these files before you run them.

The `CTC/results2excel` file folder contains the python scripts which can import the coding result from the `.txt` file to the excel file and calculate the BDBR and TS conveniently. Just change the file path in the code and run the `results2excel.py`.
The `CTC/results2excel` is the baseline coding results recoded on our computer. 
The `ECP_EPM_ET_All.xls` contains the coding performance of ECP+EPM as shown in our paper, the other `.xls` files show the coding performance of difference configurations.

## 4. myCfg
The `myCfg` file folder contains the configurations of the sequences and codec.

## 5. source
The `source` file folder contains the source code of our codec, you can build this project and get the `TAppEncoder.exe` to run our method. Lots of custom macro definitions in the source code to control the codec and some of them are deprecated which make the source code hard to read. We are sorry about that and we will clean up the redundant codes soon.

In order to facilitate the verification of the coding performance. The executable codecs are provided directly.
+ The `TAppEncoder_ECP_EPM.exe` is the encoder with our ECP+EPM algorithm.
+ The `TAppEncoder_ECP.exe` is the encoder with our ECP only algorithm.
+ The `TAppEncoder_EPM.exe` is the encoder with our EPM only algorithm.
+ The `TAppEncoder_statistic.exe` is the encoder with our ECP+EPM algorithm which can also export the coding results to analysis.

The coding result evaluated on our computer can be found in `CTC/results2excel` file folder.
