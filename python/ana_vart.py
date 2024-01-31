import subprocess
import sys
import os

# da = "lj"
dataSets = [
    ("../maximalKPlex/data/amazon/", "-f", 0.7, 10, "ama"), 
    ("../maximalKPlex/data/dblp/dblp.txt", "noUVM -f_txt", 0.8, 10,  "dbl"),
    ("../maximalKPlex/data/epin/soc-Epinions.txt", "noUVM -f_txt", 0.4, 10, "epi"),
    ("../maximalKPlex/data/wik/Wiki-Vote.txt", "noUVM -f_txt", 0.3, 10, "wik"),
    ("../maximalKPlex/data/pokec/data.txt", "noUVM -f_txt", 0.8, 10, "pok"), 
    ("../densestSubgraph/data/youtube/you.txt", "noUVM -f_txt", 0.5, 10, "you"),
    ("../maximalKPlex/data/gow/data.txt","noUVM -f_txt", 0.6, 10, "gow"),
    ("../maximalKPlex/data/berkstan/", "-f", 0.8, 10, "ber"),
    ("../maximalKPlex/data/skitter/", "-f", 0.8, 10, "ski"),
    ("../maximalKPlex/data/comlj/", "-f", 0.8, 10, "lj"),
    ("../densestSubgraph/data/orkut/", "-f", 0.8, 10, "ork"),
 #  ("../densestSubgraph/data/friender/", "-f", 0.8, 10, "fri"),
]
ns = []
ms = []
rd = []
for fNa, fFlag, kl, type, fn in dataSets:
    outDir = ""
    if fFlag == "-f":
        outDir = fNa
    else:
        i = 0
        j = 0
        while j < len(fNa):
            if fNa[j] == '/':
                i = j
            j += 1
        outDir = fNa[:i+1]
    seedDir = outDir + "seeds/"

    # get seeds
    fs = []
    for file in os.listdir(seedDir):
        if file.endswith(".R"):
            fs.append(file)
            break

    dirOut = "logs/exp_raw/adsRunner/" + fn + "/"

    n = 0
    m = 0

    sumR = 0.
    c = 0
    for seedF in fs:
        fName = seedF.split('.')[0]
        outPath = dirOut + fName + ".txt"

        with open(outPath, "r") as f:
            for l in f:
                if l.startswith("load"):
                    ls = l.split()
                    n = int(ls[3])
                    m = int(ls[5])
                if l.startswith("inducedRdensity"):
                    li = l.index(':')
                    sumR += float(l[li+1:])
                    c += 1
    ns.append(n)
    ms.append(m)
    rd.append(sumR/c)
zip_a_b = zip(ns, dataSets)
# zip_a_b = zip([a/b for a,b in zip(ns, ms)], dataSets)
# zip_a_b = zip(rd, dataSets)
sorted_zip = sorted(zip_a_b, key=lambda x:x[0])
ds, dataSets = zip(*sorted_zip)
dataSets = [dataSets[i] for i in range(0, len(dataSets), 2)]

logDir = "logs/exp_varT/"

algs = [ "adsRunner", "flowseedsRunner","betalocalDensity", "exactLocalDensity"]

for fNa, fFlag, kl, kr, fn in dataSets:
    print(fn, end=" ")
    outDir = ""
    if fFlag == "-f":
        outDir = fNa
    else:
        i = 0
        j = 0
        while j < len(fNa):
            if fNa[j] == '/':
                i = j
            j += 1
        outDir = fNa[:i+1]
    seedDir = outDir +"seeds/"

    # get seeds
    fs = []
    for file in os.listdir(seedDir):
        if file.endswith(".R"):
            fs.append(file)

    fs.sort(key=lambda x : int(x.split('_')[0]) )

    # print("seedsFiles:", len(fs))
    
    dirOut = "logs/exp_raw/exactLocalDensity/" + fn + "/"
    la = []

    for seedf in fs:
        fName = seedf.split('.')[0]
        outPath = dirOut + fName + ".txt"

        v = 0
        with open(outPath, "r") as f:
            for l in f:
                if l.startswith("rhoPava:"):
                    i = l.index(':')
                    v = max(v, float(l[i+1:]))
        la.append(v)

    # print(sum(la), len(la), sum(la) / len(la))
    # print(sum(lb), len(lb), sum(lb) / len(lb))
    # print(sum(lc), len(lc), sum(lc) / len(lc))
    # print(sum(la) / len(la), sum(lb) / len(lb), sum(lc) / len(lc), end="")
    print(sum(la) / len(la))
    # print(sum(lb) / len(lb), end=" ")
    # print(sum(lc) / len(lc), end=" ")
print()

        



alg = "betalocalDensity"


for fNa, fFlag, kl, kr, fn in dataSets:
    print(fn, end=" ")

    outDir = ""
    if fFlag == "-f":
        outDir = fNa
    else:
        i = 0
        j = 0
        while j < len(fNa):
            if fNa[j] == '/':
                i = j
            j += 1
        outDir = fNa[:i+1]
    seedDir = outDir + "seeds/"

    # get seeds
    fs = []
    for file in os.listdir(seedDir):
        if file.endswith(".R"):
            fs.append(file)

    fs.sort(key=lambda x : int(x.split('_')[0]) )

    dirOut = "logs/exp_raw/exactLocalDensity/" + fn + "/"
    la = []

    for seedf in fs:
        fName = seedf.split('.')[0]
        outPath = dirOut + fName + ".txt"

        v = 0
        with open(outPath, "r") as f:
            for l in f:
                if l.startswith("rhoPava:"):
                    i = l.index(':')
                    v = float(l[i+1:])
        la.append(v)
    
    assert(len(la) == len(fs))
    # print("seedsFiles:", len(fs))
    
    for T in [1,2,4,8,16]:
    # for T in range(1, 6):
    # if alg!="exactLocalDensity":
    #     continue
        # print(alg)

        dirOut = logDir + alg + "/" + fn + "/"

        sumT = 0
        cntT = 0

        idseedsf = 0
        for seedf in fs:
            fName = seedf.split('.')[0]

            outPath = dirOut + fName +"_"+str(T) + ".txt"

            with open(outPath, "r") as f:
                for l in f:
                    # if l.startswith("time:"):
                    #     i = l.index(':')
                    #     j = l.index('ms')
                    #     v = int(l[i+1:j])
                    #     sumT += v
                    #     cntT += 1

                    # if l.startswith("rhoPava:"):
                    #     i = l.index(':')
                    #     j = i
                    #     while(l[j] != ' '):
                    #         j += 1
                    #     v = float(l[i+1:j])
                    #     if fn =="dbl" and v > la[idseedsf]:
                    #         print(seedf, v, la[idseedsf])
                    #     sumT += v
                    #     cntT += 1

                    if l.startswith("upperOfError"):
                        i = l.index(':')
                        v = float(l[i+1:])
                        sumT += v
                        cntT += 1
                    
            idseedsf += 1
        print("&", "{:.2f}".format(sumT/cntT), end=" ")
    print("\\\\")
                        


