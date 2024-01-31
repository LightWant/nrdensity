#ifndef TRIPATH
#define TRIPATH

#include "../graph/graph.h"
#include "../tools/types.hpp"
#include <vector>

class tripath {
private:
    Graph * g;
    int k;
    ui T;

private:
     std::vector<bool> vis;
    std::vector<ui> nodes;
    
    double appThroughTripath(bool biased);

    std::vector<ui> colors;
    std::vector<ui> sortByColor;
    ui cntOfColors = 0;
    void coloring();
    std::vector<ui> reId;
    std::vector<ui> pIdx, pEdge;
    void reIdNodesByColors();

    std::vector<std::vector<ull>> dp;
    ull cntTriPath;
    void DP();

    std::vector<ui> sampledCCPaths;//T*k
    std::vector<bool> isClique;
    std::vector<ui> sampledCliques;
    std::vector<ui> r;
    std::vector<bool> isg;
    double sample();
    ull DPg();

    std::vector<ui> appDensestSg;

public:
    tripath(Graph * g, int k, ui T) : g(g), k(k), T(T) {
        vis.resize(g->n, false);
        colors.resize(g->n);
        reId.resize(g->n);
        dp.resize(k+1);

        sampledCCPaths.resize(unsigned(T*1.1*k));
        isClique.resize(unsigned(T*1.1*k));
    }
    
    double appDensity();
    double realDensityInAppSg();

    void printAppSg();
    double realDensityInCore();
    double realEdgeDensityInCore();
};

#endif