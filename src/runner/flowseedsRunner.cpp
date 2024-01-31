#include "../graph/graph.h"
#include "../tools/getArgs.hpp"
#include "../flowseeds/flowseeds.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <set>
#include <algorithm>

void printUsage() {
    std::cout << "-f graph file directory(edge.bin & idx.bin)" << std::endl;
    std::cout << "-f_txt graph file text file, each edge exists one time" << std::endl;
    std::cout << "-f_txtD graph file text file, each edge exists two times" << std::endl;
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

    std::cout << "load graph: n " << g.n << " m/2 " << g.m/2 << " maxD " << g.maxD << std::endl;

    // std::set<ui> A, R;
    std::vector<ui> A, R;
    if(ac.exist("-A")) {
        std::fstream finA(ac["-A"], std::ios::in);
        ui u;
        while(finA >> u) {
            // A.insert(u);
            A.push_back(u);
        }
        finA.close();
    }

    std::fstream finR(ac["-R"], std::ios::in);
    std::cout << "R_Path:" << ac["-R"] << std::endl;
    ui u;
    while(finR >> u) {
        // R.insert(u);
        R.push_back(u);
    }
    finR.close();

    std::sort(R.begin(), R.end());

printf("Rsize:%lu\n", R.size());
printf("Asize:%lu\n", A.size());
fflush(stdout);

auto t1 = std::chrono::steady_clock::now();

    flowseeds::community den = flowseeds::run(&g, R, A);
    
    //one order of magenitude faster than julia

auto t2 = std::chrono::steady_clock::now();
auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
std::cout << "time:" << duration.count() << "ms" << std::endl;

    printf("condunctance:%f\n", den.alphaStar);
    printf("size:%lu\n", den.sourceNodes.size());
    printf("den:");
    for(ui u : den.sourceNodes) printf("%u ", u);
    printf("\n");  

//real density

ull sumD = 0;
// ui maxV = 0;
// for(ui u : den.sourceNodes) maxV = std::max(maxV, u);
std::vector<bool> inVs(g.n);
for(ui i = 0; i < den.sourceNodes.size(); i++) {
    inVs[den.sourceNodes[i]] = true;
}
for(ui u : den.sourceNodes) {
    for(ui i = g.pIdx2[u]; i < g.pIdx[u+1]; i++) {
        ui v = g.pEdge[i];
        if(inVs[v]) sumD++;
    }
}
printf("realEdgeBasedDensity:%f\n", 1.0 * sumD / (den.sourceNodes.size()));

    return 0;
}