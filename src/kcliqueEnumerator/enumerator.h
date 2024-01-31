#ifndef ENUMERATOR_H
#define ENUMERATOR_H

#include <functional>
#include <vector>
#include "../tools/types.hpp"
#include "../graph/graph.h"

class enumerator {
protected:
    Graph * g = nullptr;
    int k;
    std::vector<ui> cliques;
    ull cntClique = 0;

    std::vector<std::vector<ui>> sg, deg, cand;
    std::vector<ui> label;
    std::vector<ui> g2sg, sg2g;
    std::vector<ui> stk;

public:
    virtual void scan(
        std::function<
            void(const std::vector<ui>::iterator &, 
                const std::vector<ui>::iterator &)> f
    ) = 0;

    virtual void scanSCT(
        std::function<
            void(const std::vector<ui>::iterator &, 
            const std::vector<ui>::iterator &, 
            const std::vector<ui>::iterator &, 
                const std::vector<ui>::iterator &,
                ui , //upper
                std::vector<ui> *,//V
                std::vector<ui> * //local
                )> f
    ) = 0;

    virtual void scanSCTPava(
                std::vector<ui> &,//level
                std::vector<ui> & //rPava
    ) = 0;

    virtual void sortSCTNodesByMinR(std::vector<ui> &) = 0;
    virtual void scanSCTFlow( std::function<void(
        std::vector<ui> * sct_, ui pp,
        ui n, 
        ull upper, //total count of k-cliques
        ui *alpha, 
        std::vector<ui> *V_, 
        std::vector<ui> * localCnt_
                )> f) = 0;
};

#endif