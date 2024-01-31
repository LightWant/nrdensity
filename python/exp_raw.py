import subprocess
import sys
import os

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

# seedsNa = "wk3_seeds"
# logDir = "logs/exp_raw/"+seedsNa+"/"

seedsNa = "seeds"
logDir = "logs/exp_raw/"
if not os.path.exists(logDir):
    os.mkdir(logDir)


algs = ["betalocalDensity", "adsRunner", "flowseedsRunner", "exactLocalDensity"]
# algs = ["exactLocalDensity",]

for alg in algs:
    dirOut = logDir + alg + "/"
    if not os.path.exists(dirOut):
        os.mkdir(dirOut)

for fNa, fFlag, kl, kr, fn in dataSets:
    print(fn)

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
        # if alg!="exactLocalDensity":
        #     continue
        print(alg)

        dirOut = logDir + alg + "/" + fn + "/"
        if os.path.exists(dirOut) == False:
            os.mkdir(dirOut)
        ii = 0
        for seedf in fs:
            ii = ii + 1
            if fn== "fri" and ii > 100:
                break
            if ii % 10 == 1:
                print(ii, seedf)

            run = "bin/" + alg + " "
            run += fFlag + " "
            run += fNa + " "
            # run += " -T 100 "
            run += " -R " + seedDir+seedf

            fName = seedf.split('.')[0]
            Af = seedDir+fName+".A"
            if os.path.exists(Af):
                run += " -A " + Af
            
            outPath = dirOut + fName + ".txt"
            run += " > " + outPath

            if os.path.exists(outPath):
                continue

            # print(run)
            subprocess.run(run, shell=True)
        print()


