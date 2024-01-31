import subprocess
import sys
import os

# da = "lj"
dataSets = [
    ("../maximalKPlex/data/amazon/", "-f", 0.7, 10, "ama"), 
  # # ("../maximalKPlex/data/caida/", "-f", 0.4, 10, "cai"),
    ("../maximalKPlex/data/dblp/dblp.txt", "noUVM -f_txt", 0.8, 10,  "dbl"),
  ##  ("../maximalKPlex/data/eronEmail/Email-EuAll.txt", "noUVM -f_txt", 0.3, 10, "ema"), 
    ("../maximalKPlex/data/epin/soc-Epinions.txt", "noUVM -f_txt", 0.4, 10, "epi"),
    ("../maximalKPlex/data/wik/Wiki-Vote.txt", "noUVM -f_txt", 0.3, 10, "wik"),
    ("../maximalKPlex/data/pokec/data.txt", "noUVM -f_txt", 0.8, 10, "pok"), 
    ("../densestSubgraph/data/youtube/you.txt", "noUVM -f_txt", 0.5, 10, "you"),
    ("../maximalKPlex/data/gow/data.txt","noUVM -f_txt", 0.6, 10, "gow"),
    ("../maximalKPlex/data/berkstan/", "-f", 0.8, 10, "ber"),
    ("../maximalKPlex/data/skitter/", "-f", 0.8, 10, "ski"),
    ("../maximalKPlex/data/comlj/", "-f", 0.8, 10, "lj"),
    ("../densestSubgraph/data/orkut/", "-f", 0.8, 10, "ork"),
  ("../densestSubgraph/data/friender/", "-f", 0.8, 10, "fri"),
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
# print(ds)

# seedsNa = "wk3_seeds"
# logDir = "logs/exp_raw/"+seedsNa+"/"

seedsNa = "seeds"
logDir = "logs/exp_raw/"

algs = ["adsRunner", "flowseedsRunner", "betalocalDensity","exactLocalDensity"]


for fNa, fFlag, kl, kr, fn in dataSets:
    # print("\""+fn+"\"", end=",")
    # continue
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
    seedDir = outDir + seedsNa+"/"

    # get seeds
    fs = []
    for file in os.listdir(seedDir):
        if file.endswith(".R"):
            fs.append(file)

    fs.sort(key=lambda x : int(x.split('_')[0]) )

    # print("seedsFiles:", len(fs))
    
    for alg in algs:
        # print(alg)

        dirOut = logDir + alg + "/" + fn + "/"
        la = []
        lb = []
        lc = []
        ld = []

        for seedf in fs:
            fName = seedf.split('.')[0]
            Af = seedDir+fName+".A"
            
            outPath = dirOut + fName + ".txt"

            if os.path.exists(outPath) == False:
                continue

            den = []
            with open(outPath, "r") as f:
                for l in f:
                    if l.startswith("time:"):
                        i = l.index(':')
                        j = l.index('ms')
                        v = int(l[i+1:j])
                        la.append(v)
                    elif l.startswith('realEdgeBasedDensity'):
                        i = l.index(':')
                        v = float(l[i+1:])
                        lb.append(v)
                    elif l.startswith('size'):
                        i = l.index(':')
                        v = int(l[i+1:])
                        lc.append(v)

        # print(sum(la), len(la), sum(la) / len(la))
        # print(sum(lb), len(lb), sum(lb) / len(lb))
        # print(sum(lc), len(lc), sum(lc) / len(lc))
        # print(sum(la) / len(la), sum(lb) / len(lb), sum(lc) / len(lc), end="")
        print(sum(la) / len(la), end=" ")
        # print(sum(lb) / len(lb), end=" ")
        # print(sum(lc) / len(lc), end=" ")
    print()

        


