cd %1
mkdir %2
cd %2
..\..\TAppEncoder.exe -c F:\zzx\myCfg\baseCfg_3view+depth_%1.cfg --QTL=0 --QtPredFlag=0 -c F:\zzx\myCfg\QP%2.cfg --FramesToBeEncoded=100 > out.txt
del *.yuv
del *.bit