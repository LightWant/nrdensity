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

# seeds set generated by walk length 3
# seedsNa="wk3_seeds"
# logDir = "logs/exp_raw/wk3_seeds/"

# seeds set generated by walk length in [3,10] randomly
seedsNa="seeds"
logDir = "logs/exp_raw/"
if os.path.exists(logDir+"conductance/") == False:
    os.mkdir(logDir+"conductance/")

algs = ["adsRunner", "flowseedsRunner", "betalocalDensity","exactLocalDensity"]

F1 = []
Con = []
recall = []
for fNa, fFlag, kl, kr, fn in dataSets:
    # print(fn, end=" ")

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

    run = "bin/testLocalCondunctance " 
    run += fFlag + " "
    run += fNa + " "
    run += " -seed " + seedDir
    run += " -da " + fn
    run += " -log " + logDir
    
    outPath = logDir+"conductance/" + fn + ".txt"

    run += " > " + outPath

    # print(run)
    if os.path.exists(outPath) == False:
        subprocess.run(run, shell=True)
    with open(outPath) as f:
        for l in f:
            ls = l.split(' ')
            if ls[0] in algs:
                # print(ls[2], end=" ")#F1
                # print(ls[3][:-1], end=" ")#conductance
                F1.append(ls[2])
                Con.append(ls[3])
                recall.append(ls[4][:-1])
i = 0
for fNa, fFlag, kl, kr, fn in dataSets:
    print(fn, end=" ")
    for j in range(len(algs)):
        print(F1[i*len(algs)+j], end=" ")
    i += 1
    print()
print()

i = 0
for fNa, fFlag, kl, kr, fn in dataSets:
    print(fn, end=" ")
    for j in range(len(algs)):
        print(Con[i*len(algs)+j], end=" ")
    i += 1
    print()
print()

i = 0
for fNa, fFlag, kl, kr, fn in dataSets:
    print(fn, end=" ")
    for j in range(len(algs)):
        print(recall[i*len(algs)+j], end=" ")
    i += 1
    print()