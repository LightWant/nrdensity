#include "kclistpp.h"
#include "../tools/HLPP.hpp"
#include "../tools/dinic.hpp"

#include "../kcliqueEnumerator/kclist.h"
void kclistpp::runKclistPPFlow(int t) {
    T += t;

    auto f = [](const itr & bg, const itr & ed, ull stIdx, 
        std::vector<ui> * alpha,
        std::vector<ui> * rr) {
        itr st = bg;
        for(itr i = bg+1; i != ed; ++i) {
            if((*rr)[*i] < (*rr)[*st]) st = i;
        }
        (*rr)[*st]++;
        (*alpha)[stIdx + (st - bg)]++;
    };
    auto f2 = std::bind(f, 
        std::placeholders::_1, 
        std::placeholders::_2, 
        std::placeholders::_3, 
        std::placeholders::_4, 
        &r);

    cliqueE_kClist* kclEtr = static_cast<cliqueE_kClist*>(etr);
    for(int i = 0; i < t; i++) {
        // if(i > 0) kclEtr->shuffleAlpha();
        kclEtr->scanFlowAlpha(f2);
    }
}

bool kclistpp::kclistStableSubset(bool strong) {
    if(strong) {
        ui maxR = r[reordered[0]];
        ui minR = r[reordered[sizeDensestSubgraph-1]];
    printf("diff:%u\n", maxR /T - minR / T);
        if(maxR - minR > k * T) return false;
    }

    cliqueE_kClist* kclEtr = static_cast<cliqueE_kClist*>(etr); 
    ui tmp;
    return kclEtr->checkStableFlow(r, 
        sizeDensestSubgraph, reordered, level, T, tmp);
}

//optimzied memory
bool kclistpp::checkExactPreFlow() {
    auto f = [](const itr & bg, const itr & ed, 
        const std::vector<ui> & level, ui sz,
        ull * cntClique,
        std::vector<ull> * localCnt) {
        for(itr i = bg; i != ed; ++i) {
            if(level[*i] >= sz) return;
        }
        (*cntClique)++;
        for(itr i = bg; i != ed; ++i) {
            (*localCnt)[level[*i]]++;
        }
    };

    ull cntClique = 0;
    std::vector<ull> localCnt(sizeDensestSubgraph);
    using namespace std::placeholders;
    auto f2 = std::bind(f, _1, _2, level, 
        sizeDensestSubgraph, &cntClique, &localCnt);

    etr->scan(f2);

printf("runningflowCntInClique:%llu\n", cntClique);
// std::cout << "runningflowCntInClique:" << cntClique << std::endl;

    ull cntOfV = sizeDensestSubgraph + cntClique + 2;
    ull cntOfE = cntClique*(k+1) + sizeDensestSubgraph;
    ui s = 0, t = 1;

    HLPP_MemoryOptimized hlpp(cntOfV, cntOfE, s, t);

    hlpp.pEdge.resize(cntOfE*2);
    hlpp.pIdx.resize(cntOfV + 1);
    hlpp.pIdx[s+1] = cntClique;
    hlpp.pIdx[t+1] = sizeDensestSubgraph;
    for(ui i = 2; i < sizeDensestSubgraph+2; i++) {
        hlpp.pIdx[i+1] = localCnt[i-2]+1;
    }
    for(ull i = sizeDensestSubgraph + 2; i < cntOfV; i++) {
        hlpp.pIdx[i+1] = k + 1;
    }
    for(ull i = 1; i <= cntOfV; i++) {
        hlpp.pIdx[i] += hlpp.pIdx[i-1];
    }

    auto f3 = [](const itr & bg, const itr & ed, ui stIdx, 
        std::vector<ui> * alpha,
        const std::vector<ui> & level, ui sz, ui s, ui T, ull cntOfCliques,
        HLPP_MemoryOptimized * hlpp, ull * idClique, std::vector<long long> * totalAlpha)
    {
        for(itr i = bg; i != ed; ++i) {
            if(level[*i] >= sz) return;
        }

        long long total = 0;
        for(itr i = bg; i != ed; ++i) {
            long long a = (*alpha)[stIdx + (i - bg)] * sz / T;
            ui idV = level[*i]+2;

            if((*totalAlpha)[idV-2] + a > cntOfCliques
             || total + a > sz) {
                a = cntOfCliques-(*totalAlpha)[idV-2];
                a = std::min(a, sz - total);
            }

            (*hlpp).addEdgePreFlow(*idClique, idV, sz, a);
            (*totalAlpha)[idV-2] += a;
            total += a;
        }
        (*hlpp).addEdgePreFlow(s, *idClique, sz, total);  
        (*idClique)++;
    };
    ull id = sizeDensestSubgraph + 2;

    std::vector<long long> totalAlpha(sizeDensestSubgraph);
    auto f4 = std::bind(f3, _1, _2, _3, _4, level,
        sizeDensestSubgraph, s, T, cntClique, &hlpp, &id, &totalAlpha);
    // enumerator.scan(f4);
    cliqueE_kClist* kclEtr = static_cast<cliqueE_kClist*>(etr); 
    kclEtr->scanFlowAlpha(f4);

    ull preFlow = 0;
    for(ui i = 2; i < sizeDensestSubgraph+2; i++) {
// assert(totalAlpha[i-2] <= cntClique);
        hlpp.addEdgePreFlow(i, t, cntClique, totalAlpha[i-2]);
        preFlow += totalAlpha[i-2];
    }

    for(ull i = cntOfV; i > 0; i--) {
        hlpp.pIdx[i] = hlpp.pIdx[i - 1];
    }
    hlpp.pIdx[0] = 0;

    ull flow = hlpp.maxFlow();
printf("preFlow:%llu, need %llu, has %llu\n", 
    preFlow, cntClique*sizeDensestSubgraph-preFlow, flow);
// std::cout << "flow:" << flow << std::endl;
// std::cout << "n*m:" << cntClique*sizeDensestSubgraph << std::endl;
    return flow+preFlow >= cntClique*sizeDensestSubgraph;
}

// bool kclistpp::checkExactPreFlow() {
//     auto f = [](const itr & bg, const itr & ed, 
//         const std::vector<ui> & level, ui sz,
//         ull * cntClique) {
//         for(itr i = bg; i != ed; ++i) {
//             if(level[*i] >= sz) return;
//         }
//         (*cntClique)++;
//     };

//     ull cntClique = 0;
//     using namespace std::placeholders;
//     auto f2 = std::bind(f, _1, _2, level, sizeDensestSubgraph, &cntClique);

//     etr->scan(f2);

// printf("runningflowCntInClique:%llu\n", cntClique);
// // std::cout << "runningflowCntInClique:" << cntClique << std::endl;

//     ui cntOfV = sizeDensestSubgraph + cntClique + 2;
//     ull cntOfE = cntClique*(k+1) + sizeDensestSubgraph;
//     ui s = 0, t = 1;
// //vtype, eType, fType
//     Dinic<ull, ull, ull> din(cntOfV, cntOfE, s, t);

//     auto f3 = [](const itr & bg, const itr & ed, ui stIdx, 
//         std::vector<ui> * alpha,
//         const std::vector<ui> & level, ui sz, ui s, ui T, ull cntOfCliques,
//         Dinic<ull, ull, ull> * din, ull * idClique, std::vector<long long> * totalAlpha)
//     {
//         for(itr i = bg; i != ed; ++i) {
//             if(level[*i] >= sz) return;
//         }

//         long long total = 0;
//         for(itr i = bg; i != ed; ++i) {
//             long long a = (*alpha)[stIdx + (i - bg)] * sz / T;
//             ui idV = level[*i]+2;

//             if((*totalAlpha)[idV-2] + a > cntOfCliques
//              || total + a > sz) {
//                 a = 0;
//             }

//             (*din).addEdge(*idClique, idV, sz, a);
//             (*totalAlpha)[idV-2] += a;
//             total += a;
//         }
//         (*din).addEdge(s, *idClique, sz, total);  
//         (*idClique)++;
//     };
//     ull id = sizeDensestSubgraph + 2;

//     std::vector<long long> totalAlpha(sizeDensestSubgraph);
//     auto f4 = std::bind(f3, _1, _2, _3, _4, level,
//         sizeDensestSubgraph, s, T, cntClique, &din, &id, &totalAlpha);
//     // enumerator.scan(f4);
//     cliqueE_kClist* kclEtr = static_cast<cliqueE_kClist*>(etr); 
//     kclEtr->scanFlowAlpha(f4);

//     ull preFlow = 0;
//     for(ui i = 2; i < sizeDensestSubgraph+2; i++) {
// assert(totalAlpha[i-2] <= cntClique);
//         din.addEdge(i, t, cntClique, totalAlpha[i-2]);
//         preFlow += totalAlpha[i-2];
//     }
// assert(preFlow <= cntClique*sizeDensestSubgraph);

//     ull flow = din.maxFlow(0x3f3f3f3f3f3f3f3f);
// printf("preFlow:%llu, need %llu, has %llu\n", 
//     preFlow, cntClique*sizeDensestSubgraph-preFlow, flow);
// // std::cout << "flow:" << flow << std::endl;
// // std::cout << "n*m:" << cntClique*sizeDensestSubgraph << std::endl;
//     return flow >= cntClique*sizeDensestSubgraph;
// }

// bool kclistpp::checkExactPreFlow() {
//     auto f = [](const itr & bg, const itr & ed, 
//         const std::vector<ui> & level, ui sz,
//         ull * cntClique) {
//         for(itr i = bg; i != ed; ++i) {
//             if(level[*i] >= sz) return;
//         }
//         (*cntClique)++;
//     };

//     ull cntClique = 0;
//     using namespace std::placeholders;
//     auto f2 = std::bind(f, _1, _2, level, sizeDensestSubgraph, &cntClique);

//     etr->scan(f2);

// printf("runningflowCntInClique:%llu\n", cntClique);
// // std::cout << "runningflowCntInClique:" << cntClique << std::endl;

//     ull cntOfV = sizeDensestSubgraph + cntClique + 2;
//     ull cntOfE = cntClique*(k+1) + sizeDensestSubgraph;
//     ui s = 0, t = 1;

//     HLPP hlpp(cntOfV, cntOfE, s, t);

//     auto f3 = [](const itr & bg, const itr & ed, ui stIdx, 
//         std::vector<ui> * alpha,
//         const std::vector<ui> & level, ui sz, ui s, ui T, ull cntOfCliques,
//         HLPP * hlpp, ull * idClique, std::vector<long long> * totalAlpha)
//     {
//         for(itr i = bg; i != ed; ++i) {
//             if(level[*i] >= sz) return;
//         }

//         long long total = 0;
//         for(itr i = bg; i != ed; ++i) {
//             long long a = (*alpha)[stIdx + (i - bg)] * sz / T;
//             ui idV = level[*i]+2;

//             if((*totalAlpha)[idV-2] + a > cntOfCliques
//              || total + a > sz) {
//                 a = cntOfCliques-(*totalAlpha)[idV-2];
//                 a = std::min(a, sz - total);
//             }

//             (*hlpp).addEdgePreFlow(*idClique, idV, sz, a);
//             (*totalAlpha)[idV-2] += a;
//             total += a;
//         }
//         (*hlpp).addEdgePreFlow(s, *idClique, sz, total);  
//         (*idClique)++;
//     };
//     ull id = sizeDensestSubgraph + 2;

//     std::vector<long long> totalAlpha(sizeDensestSubgraph);
//     auto f4 = std::bind(f3, _1, _2, _3, _4, level,
//         sizeDensestSubgraph, s, T, cntClique, &hlpp, &id, &totalAlpha);
//     // enumerator.scan(f4);
//     cliqueE_kClist* kclEtr = static_cast<cliqueE_kClist*>(etr); 
//     kclEtr->scanFlowAlpha(f4);

//     ull preFlow = 0;
//     for(ui i = 2; i < sizeDensestSubgraph+2; i++) {
// // assert(totalAlpha[i-2] <= cntClique);
//         hlpp.addEdgePreFlow(i, t, cntClique, totalAlpha[i-2]);
//         preFlow += totalAlpha[i-2];
//     }

//     ull flow = hlpp.maxFlow();
// printf("preFlow:%llu, need %llu, has %llu\n", 
//     preFlow, cntClique*sizeDensestSubgraph-preFlow, flow);
// // std::cout << "flow:" << flow << std::endl;
// // std::cout << "n*m:" << cntClique*sizeDensestSubgraph << std::endl;
//     return flow+preFlow >= cntClique*sizeDensestSubgraph;
// }


// bool kclistpp::checkExactPreFlow() {
//     auto f = [](const itr & bg, const itr & ed, 
//         const std::vector<ui> & level, ui sz,
//         ull * cntClique) {
//         for(itr i = bg; i != ed; ++i) {
//             if(level[*i] >= sz) return;
//         }
//         (*cntClique)++;
//     };

//     ull cntClique = 0;
//     using namespace std::placeholders;
//     auto f2 = std::bind(f, _1, _2, level, sizeDensestSubgraph, &cntClique);

//     etr->scan(f2);

// printf("runningflowCntInClique:%llu\n", cntClique);
// // std::cout << "runningflowCntInClique:" << cntClique << std::endl;

//     ui cntOfV = sizeDensestSubgraph + cntClique + 2;
//     ull cntOfE = cntClique*(k+1) + sizeDensestSubgraph;
//     ui s = 0, t = 1;

//     HLPP hlpp(cntOfV, cntOfE, s, t);

//     auto f3 = [](const itr & bg, const itr & ed, ui stIdx, 
//         std::vector<ui> * alpha,
//         const std::vector<ui> & level, ui sz, ui s, ui T, ull cntOfCliques,
//         HLPP * hlpp, ull * idClique, std::vector<long long> * totalAlpha)
//     {
//         for(itr i = bg; i != ed; ++i) {
//             if(level[*i] >= sz) return;
//         }

//         long long total = 0;
//         for(itr i = bg; i != ed; ++i) {
//             long long a = (*alpha)[stIdx + (i - bg)] * sz / T;
//             ui idV = level[*i]+2;

//             if((*totalAlpha)[idV-2] + a > cntOfCliques
//              || total + a > sz) {
//                 a = 0;
//             }

//             // (*hlpp).addEdgePreFlow(*idClique, idV, sz, a);
//             (*totalAlpha)[idV-2] += a;
//             total += a;
//         }
//         (*hlpp).addEdgePreFlow(s, *idClique, sz, total);  
//         (*idClique)++;
//     };
//     ull id = sizeDensestSubgraph + 2;

//     std::vector<long long> totalAlpha(sizeDensestSubgraph);
//     auto f4 = std::bind(f3, _1, _2, _3, _4, level,
//         sizeDensestSubgraph, s, T, cntClique, &hlpp, &id, &totalAlpha);
//     // enumerator.scan(f4);
//     cliqueE_kClist* kclEtr = static_cast<cliqueE_kClist*>(etr); 
//     kclEtr->scanFlowAlpha(f4);

// //put t the first priority
//     ull preFlow = 0;
//     for(ui i = 2; i < sizeDensestSubgraph+2; i++) {
// // assert(totalAlpha[i-2] <= cntClique);
//         hlpp.addEdgePreFlow(i, t, cntClique, totalAlpha[i-2]);
//         preFlow += totalAlpha[i-2];
//     }

// assert(preFlow <= cntClique*sizeDensestSubgraph);
//     auto f5 = [](const itr & bg, const itr & ed, ui stIdx, 
//         std::vector<ui> * alpha,
//         const std::vector<ui> & level, ui sz, ui s, ui T, ull cntOfCliques,
//         HLPP * hlpp, ull * idClique, std::vector<long long> * totalAlpha)
//     {
//         for(itr i = bg; i != ed; ++i) {
//             if(level[*i] >= sz) return;
//         }

//         long long total = 0;
//         for(itr i = bg; i != ed; ++i) {
//             long long a = (*alpha)[stIdx + (i - bg)] * sz / T;
//             ui idV = level[*i]+2;

//             if((*totalAlpha)[idV-2] + a > cntOfCliques
//              || total + a > sz) {
//                 a = 0;
//             }

//             (*hlpp).addEdgePreFlow(*idClique, idV, sz, a);
            
//             // (*totalAlpha)[idV-2] += a;
//             // total += a;
//         }
//         // (*hlpp).addEdgePreFlow(s, *idClique, sz, total);  
//         (*idClique)++;
//     };

//     id = sizeDensestSubgraph + 2;
//     for(ui i = 0; i < sizeDensestSubgraph; i++) {
//         totalAlpha[i] = 0;
//     }
//     auto f6 = std::bind(f5, _1, _2, _3, _4, level,
//         sizeDensestSubgraph, s, T, cntClique, &hlpp, &id, &totalAlpha);
//     kclEtr->scanFlowAlpha(f6);

//     ull flow = hlpp.maxFlow();
// printf("preFlow:%llu, need %llu, has %llu\n", 
//     preFlow, cntClique*sizeDensestSubgraph-preFlow, flow);
// // std::cout << "flow:" << flow << std::endl;
// // std::cout << "n*m:" << cntClique*sizeDensestSubgraph << std::endl;
//     return flow+preFlow >= cntClique*sizeDensestSubgraph;
// }