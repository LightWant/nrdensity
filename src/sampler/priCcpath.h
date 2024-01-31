#ifndef PRICCPATH_H
#define PRICCPATH_H

#include "../graph/graph.h"
#include "../tools/types.hpp"
#include <vector>

class priCcpath {
private:
    Graph * g;
    int k;
    ui T;

private:
    std::vector<bool> vis;
    std::vector<ui> nodes;
    
    double appThroughCCpath();

    std::vector<ui> colors;
    std::vector<ui> sortByColor;
    ui cntOfColors = 0;
    void coloring();

    std::vector<ui> reId;
    std::vector<ui> pIdx, pEdge;
    void reIdNodesByColors();

    std::vector<ui> weight;
    void triangleCntWeightBuilder();

    std::vector<std::vector<ull>> dp;
    ull cntCCPathWeight;
    ull cntCCPath;
    void DP();

    std::vector<ui> sampledCCPaths;//T*k
    std::vector<bool> isClique;
    std::vector<ui> sampledCliques;
    std::vector<ui> r;
    std::vector<bool> isg;
    double sample();
    ull DPg();
    void DPForTotalPathCnt();

    std::vector<ui> appDensestSg;

    double realEdgeDensityInCore();
public:
    priCcpath(Graph * g, int k, ui T) : g(g), k(k), T(T) {
        vis.resize(g->n, false);
        colors.resize(g->n);
        reId.resize(g->n);
        dp.resize(k+1);

        sampledCCPaths.resize(unsigned(T*1.1*k));
        isClique.resize(unsigned(T*1.1*k));
    }
    
    double appDensity();
    double realDensityInAppSg();
    double realDensityInCore();
    void printAppSg();
};

#endif