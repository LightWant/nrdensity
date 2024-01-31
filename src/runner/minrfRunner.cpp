// #define DEBUG

#include "../graph/graph.h"
#include "../tools/getArgs.hpp"
#include "../convex/minrf.h"

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
// g.print();
    int k = 4;
    if(ac.exist("-k")) k =  std::stoi(ac["-k"]);
    std::cout << "k:" << k << std::endl;

    int t = 4;
    if(ac.exist("-t")) t =  std::stoi(ac["-t"]);
    std::cout << "t:" << t << std::endl;

    auto t1 = std::chrono::steady_clock::now();

    minrf obj(g, k);
// printf("here\n");fflush(stdout);
    // obj.run();
    obj.runKCL(t);
// printf("here\n");fflush(stdout);
    auto t2 = std::chrono::steady_clock::now();
    auto durationOfEnumeration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    std::cout << "time:" << durationOfEnumeration.count() << "ms" << std::endl;

    double appRho = obj.appDensity();
    ui sz = obj.getSizeDensestSubgraph();

    std::cout << std::fixed << std::setprecision(8) << "rho:" << appRho << std::endl;
    std::cout << "size:" << sz << std::endl;

    return 0;
}