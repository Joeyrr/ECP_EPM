sequences = ['balloons', 'kendo', 'newspaper', 'gtfly', 'PoznanHall2', 'PoznanStreet', 'undodancer', 'shark']
QPs = ['25_34', '30_39', '35_42', '40_45']

import xlrd
import xlwt
import xlutils
from xlutils.copy import copy
from calcET import calcET

# sequences = ['balloons']
# QPs = ['25_34']
# dir = str('F:\\CTC\\ECU\\')
dir = str('F:\\academic_prj\\Python\\DMIN\\ResultEval\\results\\DCC_EPM\\')
# dir_baseline = str('F:\\CTC\\baseline_fast_enable\\results\\')
dir_baseline = str('F:\\academic_prj\\Python\\DMIN\\ResultEval\\results\\baseline_ET_float\\')

# origin_excel =  xlrd.open_workbook(r'.\kendo_v2_test.xls', formatting_info=True)
origin_excel =  xlrd.open_workbook(r'.\3D_16.0_vs_15.2.xls', formatting_info=True)
result_excel = copy(origin_excel)
ws = result_excel.get_sheet('experimental data')

allET_baseline = calcET('F:/academic_prj/Python/DMIN/ResultEval/results/baseline_ET_float', 0)
allET_kendo_v2 = calcET('F:/academic_prj/Python/DMIN/ResultEval/results/DCC_EPM', 0)

outFileName = 'DCC_EPM_ET_Depth.xls'

iSeq = 0
for sequence in sequences:
    iQP = 0
    for QP in QPs:

        print(sequence + QP)
        rowNum = 0
        with open(dir_baseline + sequence + '\\' + QP + '\\' + 'enc.txt') as f:
            summary = -100000
            for line in f:
                rowNum = rowNum + 1
                if line.startswith('Layer   5   POC   49'):
                    summary = rowNum + 3

                if rowNum == (summary + 2):
                    video0_bitrate = float(line[15:28])
                    video0_Y = float(line[29:38])
                    video0_U = float(line[39:48])
                    video0_V = float(line[49:58])
                    print([video0_bitrate, video0_Y, video0_U, video0_V])
                    ws.write(2+iQP+iSeq, 4, video0_bitrate)
                    ws.write(2+iQP+iSeq, 5, video0_Y)
                    ws.write(2+iQP+iSeq, 6, video0_U)
                    ws.write(2+iQP+iSeq, 7, video0_V)

                if rowNum == (summary + 24):
                    Depth0_bitrate = float(line[15:28])
                    Depth0_Y = float(line[29:38])
                    print([Depth0_bitrate, Depth0_Y])
                    ws.write(14+iQP+iSeq, 4, Depth0_bitrate)
                    ws.write(14+iQP+iSeq, 5, Depth0_Y)


                if rowNum == (summary + 46):
                    video1_bitrate = float(line[15:28])
                    video1_Y = float(line[29:38])
                    video1_U = float(line[39:48])
                    video1_V = float(line[49:58])
                    print([video1_bitrate, video1_Y, video1_U, video1_V])
                    ws.write(6+iQP+iSeq, 4, video1_bitrate)
                    ws.write(6+iQP+iSeq, 5, video1_Y)
                    ws.write(6+iQP+iSeq, 6, video1_U)
                    ws.write(6+iQP+iSeq, 7, video1_V)
                if rowNum == (summary + 68):
                    Depth1_bitrate = float(line[15:28])
                    Depth1_Y = float(line[29:38])
                    print([Depth1_bitrate, Depth1_Y])
                    ws.write(18+iQP+iSeq, 4, Depth1_bitrate)
                    ws.write(18+iQP+iSeq, 5, Depth1_Y)

                if rowNum == (summary + 90):
                    video2_bitrate = float(line[15:28])
                    video2_Y = float(line[29:38])
                    video2_U = float(line[39:48])
                    video2_V = float(line[49:58])
                    print([video2_bitrate, video2_Y, video2_U, video2_V])
                    ws.write(10+iQP+iSeq, 4, video2_bitrate)
                    ws.write(10+iQP+iSeq, 5, video2_Y)
                    ws.write(10+iQP+iSeq, 6, video2_U)
                    ws.write(10+iQP+iSeq, 7, video2_V)
                if rowNum == (summary + 112):
                    Depth2_bitrate = float(line[15:28])
                    Depth2_Y = float(line[29:38])
                    print([Depth2_bitrate, Depth2_Y])
                    ws.write(22+iQP+iSeq, 4, Depth2_bitrate)
                    ws.write(22+iQP+iSeq, 5, Depth2_Y)

            ws.write(50+iQP+iSeq, 4, video0_bitrate + video1_bitrate + video2_bitrate + Depth0_bitrate + Depth1_bitrate + Depth2_bitrate)
            ws.write(50+iQP+iSeq, 8, allET_baseline[sequence+QP])


        with open(dir_baseline + sequence + '\\' + QP + '\\' + 'PSNR_02.txt') as f:
            for line in f:
                if line.startswith('total'):
                    syn_Y = float(line[5:13])
                    syn_U = float(line[14:21])
                    syn_V = float(line[22:29])
            print([syn_Y, syn_U, syn_V])
            ws.write(26 + iQP + iSeq, 5, syn_Y)
            ws.write(26 + iQP + iSeq, 6, syn_U)
            ws.write(26 + iQP + iSeq, 7, syn_V)

        with open(dir_baseline + sequence + '\\' + QP + '\\' + 'PSNR_03.txt') as f:
            for line in f:
                if line.startswith('total'):
                    syn_Y = float(line[6:13])
                    syn_U = float(line[14:21])
                    syn_V = float(line[22:29])
            print([syn_Y, syn_U, syn_V])
            ws.write(30 + iQP + iSeq, 5, syn_Y)
            ws.write(30 + iQP + iSeq, 6, syn_U)
            ws.write(30 + iQP + iSeq, 7, syn_V)

        with open(dir_baseline + sequence + '\\' + QP + '\\' + 'PSNR_04.txt') as f:
            for line in f:
                if line.startswith('total'):
                    syn_Y = float(line[6:13])
                    syn_U = float(line[14:21])
                    syn_V = float(line[22:29])
            print([syn_Y, syn_U, syn_V])
            ws.write(34 + iQP + iSeq, 5, syn_Y)
            ws.write(34 + iQP + iSeq, 6, syn_U)
            ws.write(34 + iQP + iSeq, 7, syn_V)

        with open(dir_baseline + sequence + '\\' + QP + '\\' + 'PSNR_06.txt') as f:
            for line in f:
                if line.startswith('total'):
                    syn_Y = float(line[6:13])
                    syn_U = float(line[14:21])
                    syn_V = float(line[22:29])
            print([syn_Y, syn_U, syn_V])
            ws.write(38 + iQP + iSeq, 5, syn_Y)
            ws.write(38 + iQP + iSeq, 6, syn_U)
            ws.write(38 + iQP + iSeq, 7, syn_V)

        with open(dir_baseline + sequence + '\\' + QP + '\\' + 'PSNR_07.txt') as f:
            for line in f:
                if line.startswith('total'):
                    syn_Y = float(line[6:13])
                    syn_U = float(line[14:21])
                    syn_V = float(line[22:29])
            print([syn_Y, syn_U, syn_V])
            ws.write(42 + iQP + iSeq, 5, syn_Y)
            ws.write(42 + iQP + iSeq, 6, syn_U)
            ws.write(42 + iQP + iSeq, 7, syn_V)

        with open(dir_baseline + sequence + '\\' + QP + '\\' + 'PSNR_08.txt') as f:
            for line in f:
                if line.startswith('total'):
                    syn_Y = float(line[6:13])
                    syn_U = float(line[14:21])
                    syn_V = float(line[22:29])
            print([syn_Y, syn_U, syn_V])
            ws.write(46 + iQP + iSeq, 5, syn_Y)
            ws.write(46 + iQP + iSeq, 6, syn_U)
            ws.write(46 + iQP + iSeq, 7, syn_V)

        iQP = iQP + 1
    iSeq = iSeq + 52

iSeq = 0
for sequence in sequences:
    iQP = 0
    for QP in QPs:
        print(sequence + QP)
        rowNum = 0
        with open(dir + sequence + '\\' + QP + '\\' + 'enc.txt') as f:
            summary = -100000
            for line in f:
                rowNum = rowNum + 1
                if line.startswith('Layer   5   POC   49'):
                    summary = rowNum + 3

                if rowNum == (summary + 2):
                    video0_bitrate = float(line[15:28])
                    video0_Y = float(line[29:38])
                    video0_U = float(line[39:48])
                    video0_V = float(line[49:58])
                    print([video0_bitrate, video0_Y, video0_U, video0_V])
                    ws.write(2+iQP+iSeq, 12, video0_bitrate)
                    ws.write(2+iQP+iSeq, 13, video0_Y)
                    ws.write(2+iQP+iSeq, 14, video0_U)
                    ws.write(2+iQP+iSeq, 15, video0_V)

                if rowNum == (summary + 24):
                    Depth0_bitrate = float(line[15:28])
                    Depth0_Y = float(line[29:38])
                    print([Depth0_bitrate, Depth0_Y])
                    ws.write(14+iQP+iSeq, 12, Depth0_bitrate)
                    ws.write(14+iQP+iSeq, 13, Depth0_Y)


                if rowNum == (summary + 46):
                    video1_bitrate = float(line[15:28])
                    video1_Y = float(line[29:38])
                    video1_U = float(line[39:48])
                    video1_V = float(line[49:58])
                    print([video1_bitrate, video1_Y, video1_U, video1_V])
                    ws.write(6+iQP+iSeq, 12, video1_bitrate)
                    ws.write(6+iQP+iSeq, 13, video1_Y)
                    ws.write(6+iQP+iSeq, 14, video1_U)
                    ws.write(6+iQP+iSeq, 15, video1_V)
                if rowNum == (summary + 68):
                    Depth1_bitrate = float(line[15:28])
                    Depth1_Y = float(line[29:38])
                    print([Depth1_bitrate, Depth1_Y])
                    ws.write(18+iQP+iSeq, 12, Depth1_bitrate)
                    ws.write(18+iQP+iSeq, 13, Depth1_Y)

                if rowNum == (summary + 90):
                    video2_bitrate = float(line[15:28])
                    video2_Y = float(line[29:38])
                    video2_U = float(line[39:48])
                    video2_V = float(line[49:58])
                    print([video2_bitrate, video2_Y, video2_U, video2_V])
                    ws.write(10+iQP+iSeq, 12, video2_bitrate)
                    ws.write(10+iQP+iSeq, 13, video2_Y)
                    ws.write(10+iQP+iSeq, 14, video2_U)
                    ws.write(10+iQP+iSeq, 15, video2_V)
                if rowNum == (summary + 112):
                    Depth2_bitrate = float(line[15:28])
                    Depth2_Y = float(line[29:38])
                    print([Depth2_bitrate, Depth2_Y])
                    ws.write(22+iQP+iSeq, 12, Depth2_bitrate)
                    ws.write(22+iQP+iSeq, 13, Depth2_Y)
            ws.write(50 + iQP + iSeq, 12, video0_bitrate + video1_bitrate + video2_bitrate + Depth0_bitrate + Depth1_bitrate + Depth2_bitrate)
            ws.write(50 + iQP + iSeq, 16, allET_kendo_v2[sequence+QP])

        with open(dir + sequence + '\\' + QP + '\\' + 'PSNR_02.txt') as f:
            for line in f:
                if line.startswith('total'):
                    syn_Y = float(line[5:13])
                    syn_U = float(line[14:21])
                    syn_V = float(line[22:29])
            print([syn_Y, syn_U, syn_V])
            ws.write(26 + iQP + iSeq, 13, syn_Y)
            ws.write(26 + iQP + iSeq, 14, syn_U)
            ws.write(26 + iQP + iSeq, 15, syn_V)

        with open(dir + sequence + '\\' + QP + '\\' + 'PSNR_03.txt') as f:
            for line in f:
                if line.startswith('total'):
                    syn_Y = float(line[6:13])
                    syn_U = float(line[14:21])
                    syn_V = float(line[22:29])
            print([syn_Y, syn_U, syn_V])
            ws.write(30 + iQP + iSeq, 13, syn_Y)
            ws.write(30 + iQP + iSeq, 14, syn_U)
            ws.write(30 + iQP + iSeq, 15, syn_V)

        with open(dir + sequence + '\\' + QP + '\\' + 'PSNR_04.txt') as f:
            for line in f:
                if line.startswith('total'):
                    syn_Y = float(line[6:13])
                    syn_U = float(line[14:21])
                    syn_V = float(line[22:29])
            print([syn_Y, syn_U, syn_V])
            ws.write(34 + iQP + iSeq, 13, syn_Y)
            ws.write(34 + iQP + iSeq, 14, syn_U)
            ws.write(34 + iQP + iSeq, 15, syn_V)

        with open(dir + sequence + '\\' + QP + '\\' + 'PSNR_06.txt') as f:
            for line in f:
                if line.startswith('total'):
                    syn_Y = float(line[6:13])
                    syn_U = float(line[14:21])
                    syn_V = float(line[22:29])
            print([syn_Y, syn_U, syn_V])
            ws.write(38 + iQP + iSeq, 13, syn_Y)
            ws.write(38 + iQP + iSeq, 14, syn_U)
            ws.write(38 + iQP + iSeq, 15, syn_V)

        with open(dir + sequence + '\\' + QP + '\\' + 'PSNR_07.txt') as f:
            for line in f:
                if line.startswith('total'):
                    syn_Y = float(line[6:13])
                    syn_U = float(line[14:21])
                    syn_V = float(line[22:29])
            print([syn_Y, syn_U, syn_V])
            ws.write(42 + iQP + iSeq, 13, syn_Y)
            ws.write(42 + iQP + iSeq, 14, syn_U)
            ws.write(42 + iQP + iSeq, 15, syn_V)

        with open(dir + sequence + '\\' + QP + '\\' + 'PSNR_08.txt') as f:
            for line in f:
                if line.startswith('total'):
                    syn_Y = float(line[6:13])
                    syn_U = float(line[14:21])
                    syn_V = float(line[22:29])
            print([syn_Y, syn_U, syn_V])
            ws.write(46 + iQP + iSeq, 13, syn_Y)
            ws.write(46 + iQP + iSeq, 14, syn_U)
            ws.write(46 + iQP + iSeq, 15, syn_V)

        iQP = iQP + 1
    iSeq = iSeq + 52


result_excel.save(outFileName)