# DOWNLOAD DATASET



# GET DATASET BY YOURSELF

### 1. GET RAW FEATURES

You can build the HTM from the source code by replace the `source` file folder with `HTM-16.0_feature_extract\source` to get the `TAppEncoder.exe`

or

you can use the `TAppEncoder.exe` directly.

Run the `all.bat` to get raw features `.txt` files.

Cause there are total 10 sequences, it will take a long time to run them all. So you can handle two sequences one time. The `all.bat` gives a example to handle `Kendo` and `balloons`. The other 8 sequences (newspaper lovebird gtfly shark undodancer PoznanHall2 PoznanStreet Poznan_CarPark) can be tackled by change the input parameters in the `all.bat`.

### 2. FEATURES PREPROCESS

Change the sequence info (timeTags, videoInfos, path, and outPath) in `preprocess.py` to do data preprocessing.

