#ifndef KCLIST_H
#define KCLIST_H

// #include <vector>
#include <cstring>
// #include <functional>
// #include "../tools/types.hpp"
// #include "../graph/graph.h"
#include "enumerator.h"

class cliqueE_kClist: public enumerator {
// private:
//     Graph * g = nullptr;
//     int k;
//     std::vector<ui> cliques;
//     ull cntClique = 0;

// private:
//     std::vector<std::vector<ui>> sg, deg, cand;
//     std::vector<ui> label;
//     std::vector<ui> g2sg, sg2g;
//     std::vector<ui> stk;

    void kclist(ui, ui);

public:
    cliqueE_kClist() {}
    cliqueE_kClist(Graph * g_, int k_)  {
        //g should be degeneracy ordered
        g = g_;
        k = k_;
        cliques.resize(g->n);
        cliques.clear();

        sg.resize(g->coreNumber);
        for(ui i = 0; i < g->coreNumber; i++) {
            sg[i].resize(g->coreNumber);
        }
        deg.resize(g->coreNumber);
        for(ui i = 0; i < g->coreNumber; i++) {
            deg[i].resize(g->coreNumber);
        }
        label.resize(g->coreNumber);
        cand.resize(g->coreNumber);
        for(ui i = 0; i < g->coreNumber; i++) {
            cand[i].resize(g->coreNumber);
        }
        g2sg.resize(g->n, UINT32_MAX);
        sg2g.resize(g->coreNumber);
        stk.resize(k);
    }
    void setK(ui k_) { k = k_; }

    void enumerate();
    void shuffle();
    void shuffleAlpha();
    ull getCntClique() { return cntClique; }

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
                ui upper, std::vector<ui> *,
                std::vector<ui> *)> f
    ) {};

    virtual void scanSCTPava(
                std::vector<ui> &,//level
                std::vector<ui> & //rPava
    ) {};

    virtual void sortSCTNodesByMinR(std::vector<ui> &) {};
    virtual void scanSCTFlow( std::function<void(
        std::vector<ui> * sct_,ui pp,
        ui n, 
        ull upper, //total count of k-cliques
        ui *alpha, 
        std::vector<ui> *V_, 
        std::vector<ui> * localCnt_
                )> f) {};
    
    std::vector<ui> alpha;
    void scanFlowAlpha(std::function<void(
        const std::vector<ui>::iterator &, 
        const std::vector<ui>::iterator &,
        ui,
        std::vector<ui> *
                )> f);

    void applyToAClique(ui, std::function<
            void(const std::vector<ui>::iterator &, 
                const std::vector<ui>::iterator &, ui)> f);
    void printClique(ui i);

    void addV(ui u) {
        cliques.push_back(u);
// printf("cliques:%u\n", cliques.size());
    }
    void addC() {
        cntClique++;
    }
    void reserve(ui cntToSample) {
        cliques.resize(cntToSample);
        cliques.clear();
    }
    void printTmp() {
        printf("cliques:%lu\n", cliques.size());
    }

public:
    bool checkStableFlow(std::vector<ui> & r, 
    ui sz, std::vector<ui> & reordered, std::vector<ui> & level, 
    ui T
    , ui & nexSize);

    void scanBuildResnetPreflow(std::function<
            void(const std::vector<ui>::iterator &, 
                const std::vector<ui>::iterator &, ui)> f);

    // void scanBuildResnetPreflow(
    //     std::function<void(
    //     const std::vector<ui>::iterator &, 
    //     const std::vector<ui>::iterator &,
    //     ui,
    //     std::vector<ui> *
    //             )> f);
public://load cntClique and cliques from SCT index
    void loadFromIndex(const std::string & path, ui k) ;
    void samplingFromIndex(const std::string & path, ui k, ui cntSampling) ;
};

#endif