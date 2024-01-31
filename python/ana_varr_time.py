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
logDir = "logs/exp_raw/"

algs = ["adsRunner", "flowseedsRunner", "betalocalDensity","exactLocalDensity"]


for fNa, fFlag, kl, kr, fn in dataSets:
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

# print("seedsFiles:", len(fs))
    # print("]")
    # print(fn,"=[",end="")
    # print(fn,end=",")
    # continue
    for alg in algs:
        print(fn+"_"+alg, end="=")
        # continue

        dirOut = logDir + alg + "/" + fn + "/"

        rts = []
        for seedf in fs:
            fName = seedf.split('.')[0]
            
            outPath = dirOut + fName + ".txt"

            r = 0
            t = 0.0
            with open(outPath, "r") as f:
                for l in f:
                    if l.startswith("Rsize:") or l.startswith("Rsz:"):
                        i = l.index(":")
                        r = int(l[i+1:])
                    if l.startswith("time:"):
                        i = l.index(':')
                        j = l.index('ms')
                        t = int(l[i+1:j])
                    
                    # if l.startswith('realEdgeBasedDensity'):
                    #     i = l.index(':')
                    #     t = float(l[i+1:])
                    #     lb.append(v)
                    # elif l.startswith('size'):
                    #     i = l.index(':')
                    #     v = int(l[i+1:])
                    #     lc.append(v)
            rts.append((r,t))
        print(rts)


