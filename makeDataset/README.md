# CONSTRUCT DATASET FROM SOURCE CODE

### 1. GET RAW FEATURES

You can build the HTM from the source code by replace the `source` file folder with `HTM-16.0_feature_extract\source` to get the `TAppEncoder.exe`

or

you can use the `TAppEncoder.exe` directly.

This `TAppEncoder.exe` is modified from original 3D-HEVC encoder by adding the feature extraction codes to export the `.txt`feature file.

Run the `all.bat` to get raw features `.txt` files.

Cause there are total 10 sequences, it will take a long time to run them all. So we handle two sequences one time. The `all.bat` gives a example to handle `Kendo` and `balloons`. The other 8 sequences (newspaper lovebird gtfly shark undodancer PoznanHall2 PoznanStreet Poznan_CarPark) can be tackled by change the input parameters in the `all.bat`.

### 2. FEATURES PREPROCESS

Change the sequence info (timeTags, videoInfos, path, and outPath) in `preprocess.py` to do data preprocessing for all sequences.

# DOWNLOAD DATASET

[Here](https://pan.baidu.com/s/1N6iNNsCWniF2HMGUFlvX6w) is the dataset already constructed by the steps mentioned above, and the code is `2022`.

 

