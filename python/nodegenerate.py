import subprocess
import sys
import os

# das = [ "act", "ama", "ema", "ski", "you"]
# ths = ["0.8", "0.45", "0.5", "0.7", "0.45"]
das = [ "ski", ]
ths = ["0.6",]
dataDir = "data/"

for da, th in zip(das, ths):
    dataPath = dataDir+da+"/"+"data.txt"
    seedDir = dataDir+da+"/"+"seeds/"
    
    run = "bin/notDegenerate"
    run += " -f_txt " + dataPath
    run += " -th  " + th
    outPath = seedDir + "notDegenerateNodes.txt"
    run += " > " + outPath

    # print(run)
    subprocess.run(run, shell=True)
