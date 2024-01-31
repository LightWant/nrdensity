#ifndef MINRF_H
#define MINRF_H

#include "../graph/graph.h"
#include "../tools/types.hpp"
#include "cliqueFinderKCL.hpp"
#include <vector>
#include <functional>

class minrf {//minimum value of R be updated at first.
private:
    Graph & g;
    ui k;
    //the app densest subgraph
    ull cntCliqueSG;
    ui szSG;

    std::vector<ui> r;
    std::vector<ui> level;
    std::vector<ui> sortByR; 
    
    std::vector<ui> appDensestSg;
    void pava();
    void pava2(cfKCL & cf);
public:
    minrf(Graph & g, ui k);
    
    void run();

    void runKCL(ui t);

    double appDensity() { return 1.0 * cntCliqueSG / szSG; }
    ui getSizeDensestSubgraph() { return szSG; }
};

#endif