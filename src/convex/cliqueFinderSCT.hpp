#ifndef CLIQUEFINDERSCT_HPP
#define CLIQUEFINDERSCT_HPP

#include <vector>
#include "../tools/types.hpp"
#include "../graph/graph.h"
#include "../tools/linearSet.hpp"
#include "../tools/comb.hpp"
#include <cassert>
#include <random>

class cfSCT {
private:
    Graph & g;
    ui k;
    
    ull cntClique = 0;
    ui cntSCTNode = 0;

    std::vector<ui> bufferSCT2V;
    std::vector<ui> phSCT, ppSCT;

    std::vector<ui> bufferV2SCT;
    std::vector<ui> pv; 

    LinearSet C;
    std::vector<ui> P, H;
    comb cb;

    std::vector<ui> c;//local count V in SCT

public:
    cfSCT(Graph & g, ui k): g(g), k(k) {
        build();
    }

    void build() {
        H.resize(k); P.resize(k);
        H.clear(); P.clear();
        bufferSCT2V.resize(g.n * 10);
        bufferSCT2V.clear();
        phSCT.resize(g.n + 1); ppSCT.resize(g.n + 1);
        phSCT.clear(); ppSCT.clear();
        ppSCT.push_back(0);
        C.resize(g.n);

printf("here00\n");fflush(stdout);
        for(ui u = 0; u < g.n; u++) {
            ui d = g.pIdx[u+1] - g.pIdx2[u];
            ui sz = 0;
            for(ui i = 0; i < d; i++) {
                ui v = g.pEdge[g.pIdx2[u] + i];
                C.changeTo(v, sz++);
            }

            H.push_back(u);
            pivoter(sz);
            H.pop_back();
        }

        cntSCTNode = phSCT.size();
printf("here11\n");fflush(stdout);
        //build v2sct
        bufferV2SCT.resize(ppSCT[cntSCTNode]);
        pv.resize(g.n + 1); 
        for(ui i = 0; i < cntSCTNode; i++) {
            for(ui j = ppSCT[i]; j < ppSCT[i+1]; j++) {
                pv[bufferSCT2V[j] + 1]++;
            }
        }
printf("here22\n");fflush(stdout);
        for(ui i = 1; i <= g.n; i++) pv[i] += pv[i-1];
        for(ui i = 0; i < cntSCTNode; i++) {
            for(ui j = ppSCT[i]; j < ppSCT[i+1]; j++) {
                bufferV2SCT[pv[bufferSCT2V[j]]++] = i;
            }
        }
        for(ui i = g.n; i >= 1; i--) pv[i] = pv[i-1];
        pv[0] = 0;

        //build c
        c.resize(bufferSCT2V.size());
        for(ui i = 0; i < cntSCTNode; i++) {
            ui h = phSCT[i] - ppSCT[i];
            ui p = ppSCT[i+1] - phSCT[i];
            for(ui j = ppSCT[i]; j < phSCT[i]; j++) {
                c[j] = cb(p, k-h);
            }
            for(ui j = phSCT[i]; j < ppSCT[i+1]; j++) {
                c[j] = cb(p-1, k-h-1);
            }
        }

printf("here33\n");fflush(stdout);
        leftCliqueCnt = cntClique;

printf("bufferSCT2v\n");
for(ui i = 0; i < cntSCTNode; i++) {
    printf("%u:", i);
    for(ui j = ppSCT[i]; j < phSCT[i]; j++) 
        printf("%u ", bufferSCT2V[j]);
    printf("|");
    for(ui j = phSCT[i]; j < ppSCT[i+1]; j++) 
        printf("%u ", bufferSCT2V[j]);
    printf("\n");
}
printf("bufferv2SCT\n");
for(ui i = 0; i < g.n; i++) {
    printf("%u:", i);
    for(ui j = pv[i]; j < pv[i+1]; j++) {
        printf("%u ", bufferV2SCT[j]);
    }printf("\n");
}

    }

    void pivoter(ui sz) {
        if(sz + H.size() + P.size() < k) return;
        if(H.size() == k - 1) {
// printf("P:"); for(auto v : P) printf("%u ", v); 
// for(ui i = 0; i < sz; i++) printf("%u ", C[i]);
// printf(" H:"); for(auto v : H) printf("%u ", v); printf("\n");
            cntClique += P.size() + sz;
            // for(auto v : H) c[v] += P.size() + sz;
            // for(ui i = 0; i < P.size(); i++) {
            //     c[P[i]]++;
            // }
            // for(ui i = 0; i < sz; i++) {
            //     c[C[i]]++;
            // }
            
            for(auto v : H) bufferSCT2V.push_back(v);
            phSCT.push_back(bufferSCT2V.size());
            
            for(ui i = 0; i < P.size(); i++) {
                bufferSCT2V.push_back(P[i]);
            }
            for(ui i = 0; i < sz; i++) {
                bufferSCT2V.push_back(C[i]);
            }
            ppSCT.push_back(bufferSCT2V.size());
// printf("phsct:"); for(auto v : phSCT) printf("%u ", v); printf("\n");
// printf("ppsct:"); for(auto v : ppSCT) printf("%u ", v); printf("\n");
            return;
        }
        if(sz == 0) {
// printf("P:"); for(auto v : P) printf("%u ", v); 
// printf(" H:"); for(auto v : H) printf("%u ", v); printf("\n");
            cntClique += cb(P.size(), k - H.size());
            // for(auto v : H) c[v] += cb(P.size(), k - H.size());
            // if(k > H.size())
            // for(ui i = 0; i < P.size(); i++) {
            //     c[P[i]] += cb(P.size()-1, k-H.size()-1);
            // }

            for(auto v : H) bufferSCT2V.push_back(v);
            phSCT.push_back(bufferSCT2V.size());

            for(ui i = 0; i < P.size(); i++) {
                bufferSCT2V.push_back(P[i]);
            }
            ppSCT.push_back(bufferSCT2V.size());
// printf("phsct:"); for(auto v : phSCT) printf("%u ", v); printf("\n");
// printf("ppsct:"); for(auto v : ppSCT) printf("%u ", v); printf("\n");
            return;
        }

        ui pivot = C[0], pivotDeg = 0, num = 0;
        for(ui i = 0; i < sz; i++) {
            ui d = 0;
            for(ui j = 0; j < sz; j++) {
                if(g.connect(C[i], C[j])) d++;
            }
            if(d > pivotDeg) {
                pivot = C[i]; pivotDeg = d; num = 1;
            }
            else if(d == pivotDeg) num++;
        }

        C.changeTo(pivot, --sz);

        for(ui i = 0, j = 0; i < sz; i++) {
            if(g.connect(pivot, C[i])) C.changeToByPos(i, j++);
        }

        ui candSize = sz - pivotDeg;
        std::vector<ui> cands(candSize);
        for(ui i = pivotDeg; i < sz; i++) cands[i-pivotDeg] = C[i];

        P.push_back(pivot);
        pivoter(pivotDeg);
        P.pop_back();

        for(ui i = 0; i < candSize; i++) {
            ui u = cands[i];
            C.changeTo(u, --sz);
            ui newSz = 0;
            for(ui j = 0; j < sz; j++) {
                if(g.connect(u, C[j])) C.changeToByPos(j, newSz++);
            }

            H.push_back(u);
            pivoter(newSz);
            H.pop_back();
        }
    }

    ull leftCliqueCnt;

    bool find(ui v, std::uniform_int_distribution<ui> & D, std::mt19937 & gg) {
        typedef typename std::uniform_int_distribution<ui>::param_type param_t;

        while(pv[v] < pv[v+1]) {
            ui idx = D(gg, param_t(pv[v], pv[v+1]-1));
            std::swap(bufferV2SCT[pv[v]], bufferV2SCT[idx]);
            ui sctNodeId = bufferV2SCT[pv[v]];

            if(c[ppSCT[sctNodeId]] == 0) {
                pv[v]++; continue;
            }
printf("sctNode %u\n", sctNodeId);fflush(stdout);
            leftCliqueCnt--;
            
            bool isHNode = false;
            for(ui i = ppSCT[sctNodeId]; i < phSCT[sctNodeId]; i++) {
                ui u = bufferSCT2V[i];
                if(u == v) {
                    isHNode = true;
                    break;
                }
            }
printf("here\n");
assert(isHNode);//?
            ui tmp = phSCT[sctNodeId] - ppSCT[sctNodeId];
            if(!isHNode) {
                bool con = false;
                for(ui i = phSCT[sctNodeId]; i < ppSCT[sctNodeId+1]; i++) {
                    ui u = bufferSCT2V[i];
                    if(u == v) {
                        if(c[i] == 0) {
                            con = true;
                            break;
                        }
                        c[i]--;

// printf("minp %u c %u\n", u, c[i]);fflush(stdout);
                        tmp++;
                        break;
                    }
                }
                if(con) {
                    pv[v] += 1; continue;
                }
            }

            if(isHNode) {
                for(ui i = ppSCT[sctNodeId]; i < phSCT[sctNodeId]; i++) {
                    c[i]--;
                }
            }

            if(!isHNode && tmp < k) {//?
printf("aaaaaaaaaaaaaa\n");
                for(ui i = phSCT[sctNodeId]; i < ppSCT[sctNodeId+1]; i++) {
                    ui u = bufferSCT2V[i];
                    if(u != v && c[i] > 0) {
                        c[i]--;
// printf("min %u c %u\n", u, c[i]);fflush(stdout);
                        tmp++;
                        if(tmp == k) break;
                    }
                }
            }
            else {
                for(ui i = phSCT[sctNodeId]; i < ppSCT[sctNodeId+1]; i++) {
                    ui u = bufferSCT2V[i];
                    if(c[i] > 0) {
                        c[i]--;
printf("min %u c %u\n", u, c[i]);fflush(stdout);
                        tmp++;
                        if(tmp == k) break;
                    }
                }
            }

if(tmp < k) {
    printf("v %u\n", v);
    printf("sctNodeId %u\n", sctNodeId);
    for(ui i = ppSCT[sctNodeId]; i < phSCT[sctNodeId]; i++) {
        printf("%u ", c[bufferSCT2V[i]]);
    }printf("\n");
    for(ui i = phSCT[sctNodeId]; i < ppSCT[sctNodeId+1]; i++) {
        printf("%u ", c[bufferSCT2V[i]]);
    }printf("\n");
    fflush(stdout);
}
            assert(tmp == k);

            return true;
        }

        return false;
    }
};

#endif