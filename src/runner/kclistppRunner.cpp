#include "../graph/graph.h"
#include "../tools/getArgs.hpp"
#include "../kcliqueEnumerator/kclist.h"
#include "../kcliqueEnumerator/bufferKclist.h"
#include "../kcliqueEnumerator/enumerator.h"
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

    int w = k;//watering weight
    if(ac.exist("-w")) w =  std::stoi(ac["-w"]);
    std::cout << "w:" << w << std::endl;

    // int p = 0;
    // if(ac.exist("-p")) p =  std::stoi(ac["-p"]);
    // std::cout << "p:" << p << std::endl;

    int t = 10;
    if(ac.exist("-t")) t =  std::stoi(ac["-t"]);
    std::cout << "t:" << t << std::endl;

    bool flow = ac.exist("-flow");
    std::cout << "flow:" << flow << std::endl;

    bool shuffle = !ac.exist("-no_shuffle");
    std::cout << "shuffle:" << shuffle << std::endl;
    
    auto t1 = std::chrono::steady_clock::now();

    enumerator * etr = nullptr;
    bufferKclist * bkcl = nullptr;
    cliqueE_kClist * kcl = nullptr;

    if(ac.exist("-buffer")) {
        etr = bkcl = new bufferKclist(&g, k);
        std::cout << "buffered" << std::endl;
    }
    else {
        kcl = new cliqueE_kClist(&g, k);
        etr = kcl;

        if(ac.exist("-load")) {
            std::cout << "load" << std::endl;
            kcl->loadFromIndex(ac["-load"], k);
        }
        else kcl->enumerate();

        auto t2 = std::chrono::steady_clock::now();
        auto durationOfEnumeration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
        std::cout << "enumerateTime:" << durationOfEnumeration.count() << "ms" << std::endl;
        std::cout << "cntClique:" << kcl->getCntClique() << std::endl;

        if(shuffle) {
            kcl->shuffle();
            auto t3 = std::chrono::steady_clock::now();
            auto durationOfShuffle = std::chrono::duration_cast<std::chrono::milliseconds>(t3 - t2);
            std::cout << "shuffleTime:" << durationOfShuffle.count() << "ms" << std::endl;
        }
        // etr = &kcl;
    }
    
    auto t2 = std::chrono::steady_clock::now();
    auto durationOfEnumeration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    std::cout << "enumerateTime+shuffle:" << durationOfEnumeration.count() << "ms" << std::endl;

    kclistpp kpp(g, etr, k);
    if(ac.exist("-water")) kpp.runWatering(t, k);
    else if(ac.exist("-dcre")) {
        ui sec = t / k;
        for(ui i = 0; i < t; i++) {
            ui weight = k - i/sec;
            kpp.runWatering(1, weight);
        }
    }
    else kpp.runKclistPP(t);

    double appRho = kpp.appDensity(ac.exist("-p"));

    ui sz = kpp.getSizeDensestSubgraph();
    std::cout << std::fixed << std::setprecision(8) << "rho:" << appRho << std::endl;
    std::cout << "size:" << sz << std::endl;

    auto tlp = std::chrono::steady_clock::now();
    auto durationLP = std::chrono::duration_cast<std::chrono::milliseconds>(tlp - t2);
    std::cout << "kclistppTime:" << durationLP.count() << "ms" << std::endl;

    if(flow) {
        bool isDensest = kpp.checkExact();
        std::cout << "isDensest:" << isDensest << std::endl;

        auto t3 = std::chrono::steady_clock::now();
        auto durationFlow = std::chrono::duration_cast<std::chrono::milliseconds>(t3 - tlp);
        std::cout << "flowTime:" << durationFlow.count() << "ms" << std::endl;
    }

    auto t4 = std::chrono::steady_clock::now();
    auto durationTotal = std::chrono::duration_cast<std::chrono::milliseconds>(t4 - t1);
    std::cout << "totalTime:" << durationTotal.count() << "ms" << std::endl;

    if(ac.exist("-buffer")) delete bkcl;
    else delete kcl;

    return 0;
}