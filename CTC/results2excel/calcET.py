def calcET(dir, opt):
    # opt: 0 仅深度，1 纹理加深度总时间
    sequences = ['balloons', 'gtfly', 'kendo', 'newspaper', 'PoznanHall2', 'PoznanStreet', 'shark', 'undodancer']
    QPs = ['25_34', '30_39', '35_42', '40_45']
    allET = {}
    for sequence in sequences:
        for QP in QPs:
            rowNum = 0
            ET = 0
            with open(dir + '/' + sequence + '/' + QP + '/enc.txt') as f:
                for line in f:
                    rowNum = rowNum + 1
                    if line.startswith('Layer 5         Internal ChromaFormatIDC =   4:0:0'):
                        startRowNum = rowNum + 3
                        break
            rowNum = 0
            with open(dir + '/' + sequence + '/' + QP + '/enc.txt') as f:
                for line in f:
                    rowNum = rowNum + 1
                    if rowNum >= startRowNum:
                        if line == '\n':
                            break
                        else:
                            if line[30:31] != 'I':
                                # print(line[30:31])
                                if opt == 0:
                                    # 深度图总编码时间
                                    if int(line[6:9]) in [1, 3, 5]:
                                        ET = ET + float(line[125:131])
                                else:
                                    ET = ET + float(line[125:131])
                                    # print(line)
            allET[sequence+QP] = ET

    return allET