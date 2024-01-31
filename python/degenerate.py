import os
import commands

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
alg = "bin/notDegenerate"


for fNa, fFlag, kl, kr, fn in dataSets:
    run = alg + " "
    run += fFlag + " "
    run += fNa + " "
    run += " -th  " + str(kl) + " "

    if kl==0.8:
        continue

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
    outDir = outDir + "seeds/"
    if not os.path.exists(outDir):
        os.mkdir(outDir)
    
    outP = outDir + "notDegenerateNodes.txt"
    run += " > " + outP


    # print(run)
    commands.getstatusoutput(run)
