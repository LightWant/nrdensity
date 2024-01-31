#ifndef CLIQUEFINDERKCL_HPP
#define CLIQUEFINDERKCL_HPP

#include <vector>
#include "../tools/types.hpp"
#include "../graph/graph.h"
#include "../tools/linearSet.hpp"
// #include "../tools/comb.hpp"
#include "../kcliqueEnumerator/kclist.h"
#include <cassert>
#include <vector>
#include <functional>
#include <random>

class cfKCL {
public:
    Graph & g;
    ui k;
    cliqueE_kClist * kcl;
    ull cntClique;
    
    std::vector<bool> isFinded;
    std::vector<ui> v2clique;
    std::vector<ui> pv, pv2;

public:
    cfKCL(Graph & g, ui k) : g(g), k(k) {
        kcl = new cliqueE_kClist(&g, k);
        kcl->enumerate();

        leftCliqueCnt = cntClique = kcl->getCntClique();
        isFinded.resize(cntClique, false);
        v2clique.resize(cntClique * k);

        pv.resize(g.n + 1);

        typedef typename std::vector<ui>::iterator itr;
        auto f = [](const itr & bg, const itr & ed, 
            std::vector<ui> * pv, ui cid) {
            for(itr i = bg; i != ed; ++i) {
                (*pv)[(*i) + 1]++;
            }
        };
        auto f2 = std::bind(f, 
        std::placeholders::_1, std::placeholders::_2, &pv, std::placeholders::_3);
        for(ull i = 0; i < cntClique; i++) {
            kcl->applyToAClique(i, f2);
        }
        for(ui i = 1; i <= g.n; i++) pv[i] += pv[i-1];

        auto f3 = [](const itr & bg, const itr & ed, 
            std::vector<ui> * pv, std::vector<ui> * v2clique, ui cid) {
            for(itr i = bg; i != ed; ++i) {
                (*v2clique)[(*pv)[(*i)]++] = cid;
            }
        };
        auto f4 = std::bind(f3, std::placeholders::_1, 
            std::placeholders::_2, &pv, &v2clique, std::placeholders::_3);
        for(ull i = 0; i < cntClique; i++) {
            kcl->applyToAClique(i, f4);
        }

        for(ui i = g.n; i >= 1; i--) pv[i] = pv[i-1];
        pv[0] = 0;
        pv2.resize(pv.size());
        for(ui i = 0; i < g.n; i++) pv2[i] = pv[i];
// auto f5 = [](const itr & bg, const itr & ed, 
//     std::vector<ui> * pv, std::vector<ui> * v2clique, ui cid) {
//     for(itr i = bg; i != ed; ++i) {
//         ui v = *i;
//         ui c = 0;
//         for(ui j = (*pv)[v]; j < (*pv)[v+1]; j++) {
//             if((*v2clique)[j] == cid) c++;
//         }
//         if(c != 1) {
//             printf("not 1\n");
//             assert(false);
//         }
//     }
// };
// auto f6 = std::bind(f5, std::placeholders::_1, 
//     std::placeholders::_2, &pv, &v2clique, std::placeholders::_3);
// for(ull i = 0; i < cntClique; i++) {
//     kcl->applyToAClique(i, f6);
// }
    }
    ~cfKCL() {
        delete kcl;
    }

    ull leftCliqueCnt;

    bool find(ui v, std::uniform_int_distribution<ui> & D, std::mt19937 & gg) {
        
        
        // typedef typename std::uniform_int_distribution<ui>::param_type param_t;
        
        // do {
        //     while(pv[v] < pv[v+1] && isFinded[v2clique[pv[v]]]) {
        //         pv[v]++;
        //     }
        //     if(pv[v] == pv[v+1]) return false;

        //     ui idx = D(gg, param_t(pv[v], pv[v+1]-1));
        //     std::swap(v2clique[pv[v]], v2clique[idx]);
        // } while(isFinded[v2clique[pv[v]]]);
        
        while(pv[v] < pv[v+1] && isFinded[v2clique[pv[v]]]) {
            pv[v]++;
        }
        if(pv[v] == pv[v+1]) return false;

#ifdef DEBUG
printf("find clique %u, v%u\n", v2clique[pv[v]], v);
kcl->printClique(v2clique[pv[v]]);
#endif
        isFinded[v2clique[pv[v]]] = true;
        pv[v]++;
        leftCliqueCnt--;

        return true;
    }

    void reset() {
        leftCliqueCnt = cntClique;
        std::fill(isFinded.begin(), isFinded.end(), false);
        for(ui i = 0; i < g.n; i++) pv[i] = pv2[i];
    }

    void localCnt(std::vector<ui> & r) {
        typedef typename std::vector<ui>::iterator itr;
        auto f = [](const itr & bg, const itr & ed, 
            std::vector<ui> * r) {
            for(itr i = bg; i != ed; ++i) {
                (*r)[(*i)]++;
            }
        };
        auto f2 = std::bind(f, 
        std::placeholders::_1, std::placeholders::_2, &r);
//         for(ull i = 0; i < cntClique; i++) {
// printf("%u\n");fflush(stdout);
//             kcl->applyToAClique(i, f2);
//         }
        kcl->scan(f2);
    }
};

#endif