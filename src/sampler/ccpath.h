#ifndef CCPATH_H
#define CCPATH_H

#include "../graph/graph.h"
#include "../tools/types.hpp"
#include "../tools/comb.hpp"
#include <vector>

class ccpath {
private:
    Graph * g;
    int k;
    ui T;
    comb cb;

private:
    std::vector<bool> vis;
    std::vector<ui> nodes;
    
    double appThroughCCpath(bool biased);

    std::vector<ui> colors;
    std::vector<ui> sortByColor;
    ui cntOfColors = 0;
    void coloring();

    std::vector<ui> reId;
    std::vector<ui> pIdx, pEdge;
    void reIdNodesByColors();

    std::vector<std::vector<ull>> dp;
    ull cntCCPath;
    void DP();

    std::vector<ui> sampledCCPaths;//T*k
    std::vector<bool> isClique;
    std::vector<ui> sampledCliques;
    std::vector<ui> r;
    std::vector<bool> isg;
    double sample();
    double sampleV2();//only store sampled cliques
    ull DPg();

    std::vector<ui> appDensestSg;

//biased 
    std::vector<std::vector<ull>> dpReverse;
    std::vector<ull> cntCCPathPerNode;
    void localPathCnt();
    double biasedSample();

//ccpath2. local ccpath instead of global
    std::vector<ui> ppIdx, ppEdge, reIdForLocal;
    ull DPLocalSg(ui u, ui n);

public:
    ccpath(Graph * g, int k, ui T) : g(g), k(k), T(T) {
        vis.resize(g->n, false);
        colors.resize(g->n);
        reId.resize(g->n);
        dp.resize(k+1);

        sampledCCPaths.resize(unsigned(T*1.1*k));
        isClique.resize(unsigned(T*1.1*k));
    }
    
    double appDensity(bool biased=false);
    double realDensityInAppSg();

    void printAppSg();
    double realDensityInCore();

private://shrink graph
    double realEdgeDensityInCore();
    double realDensityInMaximumClique(ui & tmpU);
};

#endif
