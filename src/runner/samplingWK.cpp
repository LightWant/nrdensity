#include "../graph/graph.h"
#include "../tools/getArgs.hpp"
#include "../kcliqueEnumerator/sct.h"
#include "../kcliqueEnumerator/kclist.h"
#include "../convex/kclistpp.h"
#include <iostream>
#include <chrono>
#include <iomanip>

void printUsage() {
    std::cout << "-f graph file directory(edge.bin & idx.bin)" << std::endl;
    std::cout << "-f_txt graph file text file, each edge exists one time" << std::endl;
    std::cout << "-f_txtD graph file text file, each edge exists two times" << std::endl;
    std::cout << "-k clique size" << std::endl;
    std::cout << "-w fix watering weight" << std::endl;
    // std::cout << "-p xxx  previous loops, 0 in default" << std::endl;
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

    int k = 4;
    if(ac.exist("-k")) k =  std::stoi(ac["-k"]);
    std::cout << "k:" << k << std::endl;

    int t = 10;
    if(ac.exist("-t")) t =  std::stoi(ac["-t"]);
    std::cout << "t:" << t << std::endl;

    ui cntToSample = 10000000;
    if(ac.exist("-s")) cntToSample = std::stoi(ac["-s"]);
    std::cout << "s:" << cntToSample << std::endl;
    
auto t0 = std::chrono::steady_clock::now();

    SCT * sct = new SCT(&g, k);
    if(ac.exist("-load")) {
        sct->loadFromIndex(ac["-load"], k);
std::cout << "loaded" << std::endl;
    }
    else {
        sct->enumerate();
        printf("totalCliqueCnt:%llu\n", sct->getCntClique());
    }

auto t1 = std::chrono::steady_clock::now();
auto enumerateDu = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);
std::cout << "enumerate:" << enumerateDu.count() << "ms" << std::endl;
    
    cliqueE_kClist kcl;
    kcl.setK(k);
    sct->sampleFromSCT(kcl, cntToSample);
// kcl.printTmp();
printf("sampledClique:%llu\n", kcl.getCntClique());
    kcl.shuffle();
printf("shuffled\n");
    auto t2 = std::chrono::steady_clock::now();
    auto durationOfEnumeration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    std::cout << "sampling+shuffle:" << durationOfEnumeration.count() << "ms" << std::endl;

    
    kclistpp kpp(g, &kcl, k);
    kpp.runKclistPP(t);

    double appRho = kpp.appDensity(ac.exist("-p"));
    // double rho = sct->sampleFromSCT(&kcl, cntToSample);

    ui sz = kpp.getSizeDensestSubgraph();
    std::cout << std::fixed << std::setprecision(8) << "rho:" << appRho << std::endl;
    std::cout << "size:" << sz << std::endl;

    auto tlp = std::chrono::steady_clock::now();
    auto durationLP = std::chrono::duration_cast<std::chrono::milliseconds>(tlp - t2);
    std::cout << "kclistppTime:" << durationLP.count() << "ms" << std::endl;

auto totalDu = std::chrono::duration_cast<std::chrono::milliseconds>(tlp - t0);
std::cout << "totalTime:" << totalDu.count() << "ms" << std::endl;
    if(ac.exist("-pr")) {
        std::vector<ui> nodes = kpp.theDensestSg();
assert(nodes.size() == sz);
        double rho = sct->realDensityInSg(nodes);
        std::cout << "realDensityInAppSg:" << rho << std::endl;
    }

    return 0;
}