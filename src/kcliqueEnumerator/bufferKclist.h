#ifndef BUFFERKCLIST_H
#define BUFFERKCLIST_H

// #include <vector>
#include <cstring>
// #include <functional>
// #include "../tools/types.hpp"
// #include "../graph/graph.h"
#include "enumerator.h"

class bufferKclist : public enumerator {
// private:
//     Graph * g = nullptr;
//     int k;
//     std::vector<ui> cliques;//buffer
    ui bufferSize;
//     ull cntClique = 0;

// private:
//     std::vector<std::vector<ui>> sg, deg, cand;
//     std::vector<ui> label;
//     std::vector<ui> g2sg, sg2g;
//     std::vector<ui> stk;

    void kclist(ui sz, ui deep, std::function<
        void(const std::vector<ui>::iterator &, 
            const std::vector<ui>::iterator &)> & f);

    void shuffle();
    void shuffle_fflush(std::function<
        void(const std::vector<ui>::iterator &, 
            const std::vector<ui>::iterator &)> & f);
public:
    bufferKclist() {};

    bufferKclist(Graph * g_, int k_, ui bufferSize_=1e5) {
        // g(g), k(k), bufferSize(bufferSize)
        g = g_;
        k = k_;
        bufferSize = bufferSize_;
        //g should be degeneracy ordered
        cliques.resize(bufferSize);
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
    };

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
};

#endif