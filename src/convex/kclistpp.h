#ifndef KCLISTPP_H
#define KCLISTPP_H

// #include "../kcliqueEnumerator/kclist.h"
// #include "../kcliqueEnumerator/bufferKclist.h"
#include "../kcliqueEnumerator/enumerator.h"
#include "../graph/graph.h"
#include "../tools/types.hpp"
#include <vector>
#include <functional>

class kclistpp {
    typedef typename std::vector<ui>::iterator itr;
private:
    // cliqueE_kClist & enumerator;
    enumerator * etr;
    Graph & g;
    ui k;
private:
    std::vector<ui> r;
    std::vector<ui> rPava;
    std::vector<ui> nodes;
    // std::vector<ui> level; put it public
    std::vector<ui> reordered;
    double rho;
    ui sizeDensestSubgraph;
    int T = 0;
public:
    // kclistpp(Graph & g, cliqueE_kClist & e, ui k);
    kclistpp(Graph & g, enumerator * e, ui k);

    void runKclistPP(int);
    void runSCTPP(int);
    void runSCTPPWK(int);
    void runWatering(int t, int weight);
    double appDensity(bool printVs = false);
    double appDensitySCT(bool printVs = false);
    ui getSizeDensestSubgraph() { return sizeDensestSubgraph; }
    ui getAppRho() { return rho; }
    bool checkExact();

    // void printAppSg();
public://convex to flow
    std::vector<ui> level;
    void runSCTPPFlow(ui t);
    bool strongStableSubset();
    bool stableSubset();
    bool runConvexToFlow();

public://kcplistppflow.cpp
    void runKclistPPFlow(int t);
    ui shrinkNodesByAppRho(double appRho);

    bool kclistStableSubset(bool);
    bool checkExactPreFlow();

public:
    std::vector<ui> theDensestSg() {
        std::vector<ui> res;
        for(ui i = 0; i < sizeDensestSubgraph; i++) {
            res.push_back(reordered[i]);
        }
        return res;
    }

public://exactWK.cpp
    bool updateEnumeratorAfterShrink() ;
    bool checkExactSCT();
};

#endif