#include "../graph/graph.h"
#include "../tools/getArgs.hpp"
#include "../sampler/priCcpath.h"
#include <iostream>
#include <chrono>
#include <iomanip>

void printUsage() {
    std::cout << "-f graph file directory(edge.bin & idx.bin)" << std::endl;
    std::cout << "-f_txt graph file text file, each edge exists one time" << std::endl;
    std::cout << "-f_txtD graph file text file, each edge exists two times" << std::endl;
    std::cout << "-k clique size" << std::endl;
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

    ui T = 1000000;
    if(ac.exist("-t")) T =  std::stoi(ac["-t"]);
    std::cout << "t:" << T << std::endl;

    auto t1 = std::chrono::steady_clock::now();

    priCcpath cp(&g, k, T);
    double rho = cp.appDensity();

    auto t2 = std::chrono::steady_clock::now();
    auto durationApp = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    std::cout << "duration:" << durationApp.count() << "ms" << std::endl;

    if(ac.exist("-pr")) {
        double rho = cp.realDensityInAppSg();
        std::cout << "realDensityInAppSg:" << rho << std::endl;
    }

    if(ac.exist("-p")) {
        cp.printAppSg();
    }

    if(ac.exist("-pcore")) {
        double rho = cp.realDensityInCore();
        std::cout << "realDensityInCore:" << rho << std::endl;
    }

    return 0;
}