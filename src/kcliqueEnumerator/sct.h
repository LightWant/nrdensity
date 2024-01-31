#ifndef SCT_H
#define SCT_H

#include "enumerator.h"
#include "kclist.h"
#include "../tools/linearSet.hpp"
#include "../tools/comb.hpp"



class SCT : public enumerator {
private:
    struct treeNode {
        ui treeNodeId, v, fa, isP, depth, nxt;
    };
    std::vector<ui> head;
    ull cntOfTreeNode;
    std::vector<treeNode> nodes;
    std::vector<ui> P, H;
    comb cb;

    LinearSet C;

    ull cntSCTNode = 0;
    std::vector<ui> bufferSCT2V;
    std::vector<ui> phSCT, ppSCT;

    void addEdgeSCT(ui treeId, ui v);
    ui kr;
    void pivoter(ui sz);

    std::vector<ui> tmpClique;
    std::vector<ui> localCnt;

    void scanEnu(ui i, ui deep, ui j, std::function<
            void(const std::vector<ui>::iterator &, 
                const std::vector<ui>::iterator &)> f);

    double rho;
    ui sizeDensestSubgraph;

    std::vector<ui> shuffleNumbers;
    std::vector<ui> SCTNodesWeight;
    void initShuffle(ui cntClique);
    
public:
    SCT(Graph * g_, int k_) {
        g = g_;
        kr = k = k_;

        C.resize(g->n);

        H.resize(k); P.resize(k);
        H.clear(); P.clear();

        bufferSCT2V.resize(g->n * 10);
        bufferSCT2V.clear();
        phSCT.resize(g->n + 1); ppSCT.resize(g->n + 1);
        

        tmpClique.resize(g->coreNumber);
        tmpClique.clear();

        localCnt.resize(g->n);
    }

    void enumerate();
    ull getCntClique() { return cntClique; }
    void shuffle();
    
    virtual void scan(
        std::function<
            void(const std::vector<ui>::iterator &, 
                const std::vector<ui>::iterator &)> f
    );

    virtual void scanSCT(
        std::function<
            void(const std::vector<ui>::iterator &, 
            const std::vector<ui>::iterator &, 
            const std::vector<ui>::iterator &, 
                const std::vector<ui>::iterator &,
                ui, std::vector<ui> *,
                std::vector<ui> *)> f
    );

    virtual void scanSCTPava(std::vector<ui> & level, std::vector<ui> & rPava);
    
    virtual void sortSCTNodesByMinR(std::vector<ui> & r);

    std::vector<ui> alpha;
    virtual void scanSCTFlow( std::function<void(
        std::vector<ui> * sct_, ui pp,
        ui n, 
        ull upper, //total count of k-cliques
        ui *alpha, 
        std::vector<ui> *V_, 
        std::vector<ui> * localCnt_
                )> f) ;
    
    std::vector<bool> isP;
    void sctppFixedBugPupper(std::function<
            void(const std::vector<ui>::iterator &, 
            const std::vector<ui>::iterator &, 
            const std::vector<ui>::iterator &, 
                const std::vector<ui>::iterator &,
                ui, std::vector<ui> *,
                std::vector<bool> & ,
                comb &,
                std::vector<ui> *
        )> f);
    void sctppWk(std::vector<ui> & r);
    
    bool checkSummaryOfAlpha(std::vector<ui> & r, ui T);
    bool checkStableFlow(std::vector<ui> & r, ui sz, 
        std::vector<ui> & reordered, std::vector<ui> & level, 
        ui T, ui & nexSize);
    bool checkStableFlow2(std::vector<ui> & r, ui sz, 
        std::vector<ui> & reordered, std::vector<ui> & level, ui T);

    double appDensity();
    ui getSizeDensestSubgraph() { return sizeDensestSubgraph; }
    ui getAppRho() { return rho; }
    bool checkExact() { return false; };

    void print() {
        for(ull i = 0; i < cntSCTNode; i++) {
            for(ui j = ppSCT[i]; j < phSCT[i]; j++) {
                printf("%u ", bufferSCT2V[j]);
            }
            printf("/ ");
            for(ui j = phSCT[i]; j < ppSCT[i+1]; j++) {
                printf("%u ", bufferSCT2V[j]);
            }
            printf("\n");
        }
    }

    ui tmpFunction() { 
        if(alpha.size() > 0) return alpha[1];
        return 1000;
    }
public://flow alpha 
    void buildCliquesFromSCTStableSubset(
        cliqueE_kClist *, const std::vector<ui> & legvel, ui sz);

public://save the enumerated cliques
//sctSave.cpp
    void setKr(ui kr_) { 
        kr = kr_; H.resize(kr); P.resize(kr);
        H.clear(); P.clear();
    }
    void saveAsIndex(const std::string & path);
    void loadFromIndex(const std::string & path, ui k);

public://sampling
    void sampleFromSCT(cliqueE_kClist  &kcl, ui cntToSample);
    double realDensityInSg(std::vector<ui> & nodes);

public://shrink according to 
    bool shrink(const std::vector<ui> & level, ui sz);
};

#endif