
cd %1\gtfly
cd 25_34
..\..\TAppEncoder.exe -c F:\zzx\myCfg\baseCfg_3view+depth_gtfly.cfg -c F:\zzx\myCfg\seqCfg_GhostTownFly.cfg -c F:\zzx\myCfg\QP25_34.cfg --QTL=0 --QtPredFlag=0 --FramesToBeEncoded=%2 > ..\..\result\gtfly\25_34\enc.txt
..\..\TAppRenderer.exe -c F:\zzx\myCfg\rendering_3view_orgData_GhostTownFly.cfg > ..\..\result\gtfly\25_34\render_orgData.txt
..\..\TAppDecoder.exe -b stream.bit -o dec.yuv > ..\..\result\gtfly\25_34\dec.txt
..\..\TAppRenderer.exe -c rendering_3view_decData_GhostTownFly.cfg > ..\..\result\gtfly\25_34\render_decData.txt


..\..\PSNRStatic 1920 1088 org_synth_02.yuv dec_synth_02.yuv > ..\..\result\gtfly\25_34\PSNR_02.txt
..\..\PSNRStatic 1920 1088 org_synth_03.yuv dec_synth_03.yuv > ..\..\result\gtfly\25_34\PSNR_03.txt
..\..\PSNRStatic 1920 1088 org_synth_04.yuv dec_synth_04.yuv > ..\..\result\gtfly\25_34\PSNR_04.txt
..\..\PSNRStatic 1920 1088 org_synth_06.yuv dec_synth_06.yuv > ..\..\result\gtfly\25_34\PSNR_06.txt
..\..\PSNRStatic 1920 1088 org_synth_07.yuv dec_synth_07.yuv > ..\..\result\gtfly\25_34\PSNR_07.txt
..\..\PSNRStatic 1920 1088 org_synth_08.yuv dec_synth_08.yuv > ..\..\result\gtfly\25_34\PSNR_08.txt

del *.yuv

cd ..\30_39
..\..\TAppEncoder.exe -c F:\zzx\myCfg\baseCfg_3view+depth_gtfly.cfg -c F:\zzx\myCfg\seqCfg_GhostTownFly.cfg -c F:\zzx\myCfg\QP30_39.cfg --QTL=0 --QtPredFlag=0 --FramesToBeEncoded=%2 > ..\..\result\gtfly\30_39\enc.txt
..\..\TAppRenderer.exe -c F:\zzx\myCfg\rendering_3view_orgData_GhostTownFly.cfg > ..\..\result\gtfly\30_39\render_orgData.txt
..\..\TAppDecoder.exe -b stream.bit -o dec.yuv > ..\..\result\gtfly\30_39\dec.txt
..\..\TAppRenderer.exe -c rendering_3view_decData_GhostTownFly.cfg > ..\..\result\gtfly\30_39\render_decData.txt


..\..\PSNRStatic 1920 1088 org_synth_02.yuv dec_synth_02.yuv > ..\..\result\gtfly\30_39\PSNR_02.txt
..\..\PSNRStatic 1920 1088 org_synth_03.yuv dec_synth_03.yuv > ..\..\result\gtfly\30_39\PSNR_03.txt
..\..\PSNRStatic 1920 1088 org_synth_04.yuv dec_synth_04.yuv > ..\..\result\gtfly\30_39\PSNR_04.txt
..\..\PSNRStatic 1920 1088 org_synth_06.yuv dec_synth_06.yuv > ..\..\result\gtfly\30_39\PSNR_06.txt
..\..\PSNRStatic 1920 1088 org_synth_07.yuv dec_synth_07.yuv > ..\..\result\gtfly\30_39\PSNR_07.txt
..\..\PSNRStatic 1920 1088 org_synth_08.yuv dec_synth_08.yuv > ..\..\result\gtfly\30_39\PSNR_08.txt


del *.yuv

cd ..\35_42
..\..\TAppEncoder.exe -c F:\zzx\myCfg\baseCfg_3view+depth_gtfly.cfg -c F:\zzx\myCfg\seqCfg_GhostTownFly.cfg -c F:\zzx\myCfg\QP35_42.cfg --QTL=0 --QtPredFlag=0 --FramesToBeEncoded=%2 > ..\..\result\gtfly\35_42\enc.txt
..\..\TAppRenderer.exe -c F:\zzx\myCfg\rendering_3view_orgData_GhostTownFly.cfg > ..\..\result\gtfly\35_42\render_orgData.txt
..\..\TAppDecoder.exe -b stream.bit -o dec.yuv > ..\..\result\gtfly\35_42\dec.txt
..\..\TAppRenderer.exe -c rendering_3view_decData_GhostTownFly.cfg > ..\..\result\gtfly\35_42\render_decData.txt


..\..\PSNRStatic 1920 1088 org_synth_02.yuv dec_synth_02.yuv > ..\..\result\gtfly\35_42\PSNR_02.txt
..\..\PSNRStatic 1920 1088 org_synth_03.yuv dec_synth_03.yuv > ..\..\result\gtfly\35_42\PSNR_03.txt
..\..\PSNRStatic 1920 1088 org_synth_04.yuv dec_synth_04.yuv > ..\..\result\gtfly\35_42\PSNR_04.txt
..\..\PSNRStatic 1920 1088 org_synth_06.yuv dec_synth_06.yuv > ..\..\result\gtfly\35_42\PSNR_06.txt
..\..\PSNRStatic 1920 1088 org_synth_07.yuv dec_synth_07.yuv > ..\..\result\gtfly\35_42\PSNR_07.txt
..\..\PSNRStatic 1920 1088 org_synth_08.yuv dec_synth_08.yuv > ..\..\result\gtfly\35_42\PSNR_08.txt

del *.yuv

cd ..\40_45
..\..\TAppEncoder.exe -c F:\zzx\myCfg\baseCfg_3view+depth_gtfly.cfg -c F:\zzx\myCfg\seqCfg_GhostTownFly.cfg -c F:\zzx\myCfg\QP40_45.cfg --QTL=0 --QtPredFlag=0 --FramesToBeEncoded=%2 > ..\..\result\gtfly\40_45\enc.txt
..\..\TAppRenderer.exe -c F:\zzx\myCfg\rendering_3view_orgData_GhostTownFly.cfg > ..\..\result\gtfly\40_45\render_orgData.txt
..\..\TAppDecoder.exe -b stream.bit -o dec.yuv > ..\..\result\gtfly\40_45\dec.txt
..\..\TAppRenderer.exe -c rendering_3view_decData_GhostTownFly.cfg > ..\..\result\gtfly\40_45\render_decData.txt


..\..\PSNRStatic 1920 1088 org_synth_02.yuv dec_synth_02.yuv > ..\..\result\gtfly\40_45\PSNR_02.txt
..\..\PSNRStatic 1920 1088 org_synth_03.yuv dec_synth_03.yuv > ..\..\result\gtfly\40_45\PSNR_03.txt
..\..\PSNRStatic 1920 1088 org_synth_04.yuv dec_synth_04.yuv > ..\..\result\gtfly\40_45\PSNR_04.txt
..\..\PSNRStatic 1920 1088 org_synth_06.yuv dec_synth_06.yuv > ..\..\result\gtfly\40_45\PSNR_06.txt
..\..\PSNRStatic 1920 1088 org_synth_07.yuv dec_synth_07.yuv > ..\..\result\gtfly\40_45\PSNR_07.txt
..\..\PSNRStatic 1920 1088 org_synth_08.yuv dec_synth_08.yuv > ..\..\result\gtfly\40_45\PSNR_08.txt

del *.yuv
