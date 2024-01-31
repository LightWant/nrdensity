import subprocess
import sys
import os

# da = "lj"
dataSets = [
    ("../maximalKPlex/data/amazon/", "-f", 0.7, "Co-purchase", "ama"), 
  # # ("../maximalKPlex/data/caida/", "-f", 0.4, 10, "cai"),
    ("../maximalKPlex/data/dblp/dblp.txt", "noUVM -f_txt", 0.8, "Collaboration",  "dbl"),
  ##  ("../maximalKPlex/data/eronEmail/Email-EuAll.txt", "noUVM -f_txt", 0.3, 10, "ema"), 
    ("../maximalKPlex/data/epin/soc-Epinions.txt", "noUVM -f_txt", 0.4, "social", "epi"),
    ("../maximalKPlex/data/wik/Wiki-Vote.txt", "noUVM -f_txt", 0.3, "social", "wik"),
    ("../maximalKPlex/data/pokec/data.txt", "noUVM -f_txt", 0.8, "social", "pok"), 
    ("../densestSubgraph/data/youtube/you.txt", "noUVM -f_txt", 0.5, "social", "you"),
    ("../maximalKPlex/data/gow/data.txt","noUVM -f_txt", 0.6, "social", "gow"),
    ("../maximalKPlex/data/berkstan/", "-f", 0.8, "Web", "ber"),
    ("../maximalKPlex/data/skitter/", "-f", 0.8, "Internet topology", "ski"),
    ("../maximalKPlex/data/comlj/", "-f", 0.8, "social", "lj"),
    ("../densestSubgraph/data/orkut/", "-f", 0.8, "social", "ork"),
    ("../densestSubgraph/data/friender/", "-f", 0.8, "social", "fri"),
]
logDir = "logs/exp_raw/"
alg = "adsRunner"

ns = []
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
    fName = fs[0].split('.')[0]
    outPath = dirOut + fName + ".txt"

    with open(outPath, "r") as f:
        for l in f:
            if l.startswith("load"):
                ls = l.split()
                n = int(ls[3])
                m = int(ls[5])
    ns.append(n)

zip_a_b = zip(ns, dataSets)
sorted_zip = sorted(zip_a_b, key=lambda x:x[0])
ns, dataSets = zip(*sorted_zip)

r = 0
c = 0
for fNa, fFlag, kl, type, fn in dataSets:
    # print("\""+fn+"\"", end=",")
    # continue

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

    dirOut = logDir + alg + "/" + fn + "/"

    n = 0
    m = 0

    for seedf in fs:
        fName = seedf.split('.')[0]
        outPath = dirOut + fName + ".txt"

        if os.path.exists(outPath)==False:
            continue

        with open(outPath, "r") as f:
            for l in f:
                if l.startswith("Rsize:"):
                    i = l.index(':')
                    r += int(l[i+1:])
                    c += 1
                if l.startswith("load"):
                    ls = l.split()
                    n = int(ls[3])
                    m = int(ls[5])
    if type == "social":
        type = "Social network"
    print("\\{} & {:,} & {:,} & {}\\\\".format(fn, n, m, type))
                    
print(r/c)
        


