#include "../graph/graph.h"
#include "../tools/getArgs.hpp"
#include "../kcliqueEnumerator/sct.h"
#include "../kcliqueEnumerator/kclist.h"
#include "../kcliqueEnumerator/enumerator.h"
#include "../convex/kclistpp.h"
// #include "../convex/kclistpp.h"
#include <iostream>
#include <chrono>
#include <iomanip>

void printUsage() {
    std::cout << "-f graph file directory(edge.bin & idx.bin)" << std::endl;
    std::cout << "-f_txt graph file text file, each edge exists one time" << std::endl;
    std::cout << "-f_txtD graph file text file, each edge exists two times" << std::endl;
    std::cout << "-k clique size" << std::endl;
    std::cout << "-w fix watering weight" << std::endl;
    std::cout << "-p xxx  previous loops, 0 in default" << std::endl;
    std::cout << "-flow, false in default" << std::endl;
    std::cout << "-s, shrink graph, true in default" << std::endl;
}

int main(int argc, char * argv[]) {
    argsController ac(argc, argv);

    if(!ac.exist("-f_txt") && !ac.exist("-f") && !ac.exist("-f_txtD")) {
        printUsage();
        return 0;
    }

    bool noVUM = false;
    if(ac.exist("noUVM")) noVUM = true;
    Graph g;
    if(ac.exist("-f_txt")) {
        g.readFromText(ac["-f_txt"], noVUM);
    }
    else if(ac.exist("-f_txtD")) g.readFromTextDoubleEdges(ac["-f_txtD"]);
    else g.readFromBin(ac["-f"]);

    std::cout << "load graph: n " << g.n << " m " << g.m << " maxD " << g.maxD << std::endl;

    auto s1 = std::chrono::steady_clock::now();
    g.changeToCoreOrder();
    auto s2 = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(s2 - s1);
    std::cout << "changeToCoreOrder:" << duration.count() << "ms" << std::endl;
    std::cout << "coreNumber:" << g.coreNumber << std::endl;

    g.initHash();

    int k = 4;
    if(ac.exist("-k")) k =  std::stoi(ac["-k"]);
    std::cout << "k:" << k << std::endl;

    // bool shuffle = !ac.exist("-no_shuffle");
    // std::cout << "shuffle:" << shuffle << std::endl;

    bool isStrong = ac.exist("-s");
    std::cout << "strong:" << isStrong << std::endl;
    
    auto t1 = std::chrono::steady_clock::now();

    SCT * sct = new SCT(&g, k);
    if(ac.exist("-load")) {
        sct->loadFromIndex(ac["-load"], k);
std::cout << "loaded" << std::endl;
    }
    else {
        sct->enumerate();
    }

    auto t2 = std::chrono::steady_clock::now();
    auto durationOfEnumeration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    std::cout << "enumerateTime:" << durationOfEnumeration.count() << "ms" << std::endl;
    std::cout << "cntClique:" << sct->getCntClique() << std::endl;
    
    kclistpp sctpp(g, sct, k);

// g.print();
// sct->print();
    ui szStable = 0;
    double appRhoSCT;
    for(ui T = 1; ; T *= 2) {
// printf("T%u %u\n", T,sct->tmpFunction());
        sctpp.runSCTPPFlow(T - T/2);
// printf("TT%u %u\n", T,sct->tmpFunction());
        appRhoSCT = sctpp.appDensitySCT(false);//run
// printf("TTT%u %u\n", T,sct->tmpFunction());
std::cout << std::fixed << std::setprecision(8) << "rho"<<T<<":" << appRhoSCT << std::endl;
//         szStable = sctpp.getSizeDensestSubgraph();
// std::cout << "size"<<T<<":" << szStable << std::endl;
        break;

        // bool isStable = false;
        // if(isStrong) isStable = sctpp.strongStableSubset();
        // else isStable = sctpp.stableSubset();

// auto tlp = std::chrono::steady_clock::now();
// auto durationLP = std::chrono::duration_cast<std::chrono::milliseconds>(tlp - t2);
// std::cout << "SCTTime:" << durationLP.count() << "ms" << std::endl;

        // if(isStable) {
        //     break;
        // }
        // if(T > 10) break;
    }
    
auto t3 = std::chrono::steady_clock::now();
auto duration2 = std::chrono::duration_cast<std::chrono::milliseconds>(t3 - t2);
std::cout << "sctTime:" << duration2.count() << "ms" << std::endl;

    szStable = sctpp.shrinkNodesByAppRho(appRhoSCT);
std::cout << "shirnkedSize:" << szStable << std::endl;
     //get kcliques in stable subsets
    cliqueE_kClist kcl;
    kcl.setK(k);
    sct->buildCliquesFromSCTStableSubset(&kcl, sctpp.level, szStable);
// kcl.printTmp();
// printf("kcl:%llu\n", kcl.getCntClique());fflush(stdout);
    kcl.shuffle();

    kclistpp kpp(g, &kcl, k);
    ull shrinkedCliqueCnt = kcl.getCntClique();
printf("kcl:%llu\n", shrinkedCliqueCnt);

    if(shrinkedCliqueCnt*k > UINT32_MAX) {
        printf("too many cliques\n");
        delete sct;
        return 0;
    }

    for(ui T = 1; ; T *= 2) {
        kpp.runKclistPPFlow(T - T/2);//alpha

        double appRho = kpp.appDensity(false);
std::cout << std::fixed << std::setprecision(8) << "rho"<<T<<":" << appRho << std::endl;
std::cout << "size"<<T<<":" << kpp.getSizeDensestSubgraph() << std::endl;
        bool isStable = kpp.kclistStableSubset(ac.exist("-s"));
std::cout << "isStable:"<< isStable << std::endl;
// break;
        if(isStable) {
auto flowT0 = std::chrono::steady_clock::now();

            bool isDensest = true;
            if(ac.exist("-preflow")) {
                isDensest = kpp.checkExactPreFlow();
            }
            else isDensest = kpp.checkExact();
auto flowT1 = std::chrono::steady_clock::now();
auto durationFlow = 
    std::chrono::duration_cast<std::chrono::milliseconds>(flowT1 - flowT0);
std::cout << "flowRunningTime:" << durationFlow.count() << "ms" << std::endl;
            // bool isDensest = kpp.checkExact();
            if(isDensest) {
                printf("densest!\n");
                break;
            }
            else {
                printf("not densest!\n");
            }
        }
    }
auto t4 = std::chrono::steady_clock::now();
auto duration3 = std::chrono::duration_cast<std::chrono::milliseconds>(t4 - t2);
std::cout << "totalTime:" << duration3.count() << "ms" << std::endl;

    delete sct;

    return 0;
}