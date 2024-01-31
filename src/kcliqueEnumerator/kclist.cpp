#include "kclist.h"
#include <random>
#include <cassert>
#include <algorithm>

void cliqueE_kClist::enumerate() {
    cntClique = 0;
    cliques.clear();

    if(k == 2) {
        for(ui u = 0; u < g->n; u++) {
            ui d = g->pIdx[u+1] - g->pIdx2[u];
            for(ui i = 0; i < d; i++) {
                ui v = g->pEdge[g->pIdx2[u] + i];
                    
                cliques.push_back(u);
                cliques.push_back(v);
            }
        }
        cntClique = cliques.size() / 2;

        return;
    }

    for(ui u = 0; u < g->n; u++) {
        ui d = g->pIdx[u+1] - g->pIdx2[u];
        for(ui i = 0; i < d; i++) g2sg[g->pEdge[g->pIdx2[u] + i]] = i;
        for(ui i = 0; i < d; i++) sg2g[i] = g->pEdge[g->pIdx2[u] + i];
        for(ui i = 0; i < d; i++) cand[0][i] = i;
        for(ui i = 0; i < d; i++) label[i] = 0;
        for(ui i = 0; i < d; i++) {
            deg[0][i] = 0;
            ui v = g->pEdge[g->pIdx2[u] + i];
            ui dv = g->pIdx[v+1] - g->pIdx2[v];
            for(ui j = 0; j < dv; j++) {
                ui w = g->pEdge[g->pIdx2[v] + j];
                if(g2sg[w] < d) sg[i][deg[0][i]++] = g2sg[w]; 
            }
        }

        stk.clear();
        stk.push_back(u);
        kclist(d, 0);

        for(ui i = 0; i < d; i++) g2sg[g->pEdge[g->pIdx2[u] + i]] = UINT32_MAX;
    }

    // assert(cliques.size() == cntClique*k);
}

void cliqueE_kClist::kclist(ui sz, ui deep) {
    std::vector<ui> & d = deg[deep];
    std::vector<ui> & cd = cand[deep];

    if(deep + 3 == k) {
        for(ui i = 0; i < sz; i++) {
            ui sgu = cd[i];
            for(ui j = 0; j < d[sgu]; j++) {
                ui sgv = sg[sgu][j];
                cntClique++;
                for(auto v : stk) cliques.push_back(v);
                cliques.push_back(sg2g[sgu]);
                cliques.push_back(sg2g[sgv]);
            }
        }
        return;
    }

    for(ui i = 0; i < sz; i++) {
        ui sgu = cd[i];
        ui dsgu = d[sgu];

        if(deep+1+dsgu+1 < k) continue;

        ui newSz = 0;
        for(ui j = 0; j < dsgu; j++) {
            ui sgv = sg[sgu][j];
            // if(label[sgv] == deep) {
                label[sgv] = deep + 1;
                cand[deep+1][newSz++] = sgv;
            // }
        }
        
        for(ui j = 0; j < newSz; j++) {
            ui sgv = cand[deep+1][j];
            ui & newD = deg[deep+1][sgv];
            newD = d[sgv];
            for(ui l = 0; l < newD; ) {
                if(label[sg[sgv][l]] == deep+1) l++;
                else std::swap(sg[sgv][l], sg[sgv][--newD]);
            } 
        }

        stk.push_back(sg2g[sgu]);
        kclist(newSz, deep+1);
        stk.pop_back();

        for(ui j = 0; j < newSz; j++) {
            label[cand[deep+1][j]] = deep;
        }
    }
}

void cliqueE_kClist::shuffle() {
    std::random_device rd;
    std::mt19937 gg(rd());
    // std::mt19937 gg(31);
    
    typedef std::uniform_int_distribution<ui> distr_t;
    typedef typename distr_t::param_type param_t;
assert(cliques.size() == cntClique*k);
// printf("%llu %llu %d %u\n", cntClique*k, cntClique, k, cliques.size());
// assert(cntClique*k == cliques.size());
    distr_t D;
    if(cntClique>1)
    for(ui i = cntClique-1; i > 0; i--) {
        ui j = D(gg, param_t(0, i));
        for(ui l = 0; l < k; l++) {
            std::swap(cliques[i*k+l], cliques[j*k+l]);
        }
    }
}

void cliqueE_kClist::shuffleAlpha() {
    std::random_device rd;
    std::mt19937 gg(rd());
    
    typedef std::uniform_int_distribution<ui> distr_t;
    typedef typename distr_t::param_type param_t;

// printf("%llu %llu %d %u\n", cntClique*k, cntClique, k, cliques.size());
// assert(cntClique*k == cliques.size());
    distr_t D;
    if(cntClique>1)
    for(ui i = cntClique-1; i > 0; i--) {
        ui j = D(gg, param_t(0, i));
        for(ui l = 0; l < k; l++) {
            std::swap(cliques[i*k+l], cliques[j*k+l]);
            std::swap(alpha[i*k+l], alpha[j*k+l]);
        }
    }
}

void cliqueE_kClist::scan(
    std::function<
        void(const std::vector<ui>::iterator &, 
            const std::vector<ui>::iterator &)> f
    ) 
{
    typedef std::vector<ui>::iterator itr;
    for(ull i = 0; i < cntClique; i++) {
// if(i > 26890000) {

// printf("%u ", i);fflush(stdout);
// }
// assert(i*k+k <= cliques.size());
        f(itr(cliques.begin()+i*k), itr(cliques.begin()+i*k+k));
    }
// printf("down\n", i);fflush(stdout);
}


void cliqueE_kClist::applyToAClique(ui i, std::function<
            void(const std::vector<ui>::iterator &, 
                const std::vector<ui>::iterator &, ui)> f) {
    f(cliques.begin()+i*k, cliques.begin()+(i+1)*k, i);
}

void cliqueE_kClist::printClique(ui i) {
    for(ui j = i*k; j < i*k+k; j++) {
        printf("%u ", cliques[j]);
    }
    printf("\n");
}

void cliqueE_kClist::scanFlowAlpha(std::function<void(
        const std::vector<ui>::iterator &, 
        const std::vector<ui>::iterator &,
        ui,
        std::vector<ui> *
                )> f) {
    if(alpha.size() == 0) {
        alpha.resize(cntClique * k);
    }

    typedef std::vector<ui>::iterator itr;
    for(ull i = 0; i < cntClique; i++) {
        f(itr(cliques.begin()+i*k), itr(cliques.begin()+i*k+k), i*k, 
        &alpha);
    }
}

bool cliqueE_kClist::checkStableFlow(std::vector<ui> & r, 
    ui sz, std::vector<ui> & reordered, std::vector<ui> & level, 
    ui T, ui & nexSize) {
    ui n = r.size();
    std::vector<ui> tmpR(n);
    memcpy(tmpR.data(), r.data(), sizeof(ui)*n);
    // std::vector<ui> & tmpR = r;
    std::vector<ui> V(k);
    
    for(ull i = 0; i < cntClique; i++) {
        ui smallerThanSz = 0;
        for (unsigned j = 0; j < k; ++j) {
            if (level[cliques[i * k + j]] < sz) {
                smallerThanSz++;
            } 
        }
        
        if(0 < smallerThanSz && smallerThanSz < k) {
            ui sumR = 0;
            for (unsigned j = 0; j < k; ++j) {
                if (level[cliques[i * k + j]] < sz) {
                    sumR += alpha[i * k + j];
assert(tmpR[cliques[i * k + j]] >= alpha[i * k + j]);
                    tmpR[cliques[i * k + j]] -= alpha[i * k + j];
                    // alpha[i * k + j] = 0;
                }
            }

            
            V.clear();
            for (unsigned j = 0; j < k; ++j) {
                if (level[cliques[i * k + j]] >= sz) {
                    V.push_back(cliques[i * k + j]);
                }
            }

            std::sort(V.begin(), V.end(), [&](ui a, ui b) {
                return tmpR[a] < tmpR[b];
            });

            for(unsigned j = 1; j < V.size() && sumR > 0; j++) {
                unsigned diff = tmpR[V[j]]-tmpR[V[j-1]];
                if(diff == 0) continue;
                if(diff * j <= sumR) {
                    sumR -= diff * j;
                    for(unsigned l = 0; l < j; l++) {
                        tmpR[V[l]] += diff;
                        // alpha[i*k+remap[l]] += diff;
                    }
                }
                else {
                    unsigned v = sumR / j;
                    sumR = sumR % j;
                    
                    for(unsigned l = 0; l < sumR; l++) {
                        tmpR[V[l]] += 1 + v;
                        // alpha[i*k+remap[l]] += 1;
                    }
                    if(v)
                    for(unsigned l = sumR; l < j; l++) {
                        tmpR[V[l]] += v;
                        // alpha[i*k+remap[l]] += v;
                    }
                    sumR = 0;
                }
            }
            if(sumR > 0) {
                unsigned v = sumR / V.size();
                sumR = sumR % V.size();
                for(unsigned l = 0; l < sumR; l++) {
                    tmpR[V[l]] += 1 + v;
                    // alpha[i*k+remap[l]] += 1;
                }
                for(unsigned l = sumR; l < V.size(); l++) {
                    tmpR[V[l]] += v;
                    // alpha[i*k+remap[l]] += v;
                }
            }
        }
        else {
            ui sumR = 0;
            for (unsigned j = 0; j < k; ++j) {
                // if (level[cliques[i * k + j]] < sz) {
                    sumR += alpha[i * k + j];
// if(tmpR[cliques[i * k + j]] < alpha[i * k + j]) {
// printf("%u-%u-%llu\n", tmpR[cliques[i * k + j]], 
// alpha[i * k + j], i * k + j);
// fflush(stdout);
// }
// assert(tmpR[cliques[i * k + j]] >= alpha[i * k + j]);
                    if(tmpR[cliques[i*k + j]] < alpha[i*k + j]) 
                        tmpR[cliques[i*k + j]] = 0;
                    else tmpR[cliques[i * k + j]] -= alpha[i * k + j];
                    // alpha[i * k + j] = 0;
                // }
            }

            
            V.clear();
            for (unsigned j = 0; j < k; ++j) {
                // if (level[cliques[i * k + j]] >= sz) {
                    V.push_back(cliques[i * k + j]);
                // }
            }

            std::sort(V.begin(), V.end(), [&](ui a, ui b) {
                return tmpR[a] < tmpR[b];
            });

            for(unsigned j = 1; j < V.size() && sumR > 0; j++) {
                unsigned diff = tmpR[V[j]]-tmpR[V[j-1]];
                if(diff == 0) continue;
                if(diff * j <= sumR) {
                    sumR -= diff * j;
                    for(unsigned l = 0; l < j; l++) {
                        tmpR[V[l]] += diff;
                        // alpha[i*k+remap[l]] += diff;
                    }
                }
                else {
                    unsigned v = sumR / j;
                    sumR = sumR % j;
                    
                    for(unsigned l = 0; l < sumR; l++) {
                        tmpR[V[l]] += 1 + v;
                        // alpha[i*k+remap[l]] += 1;
                    }
                    if(v)
                    for(unsigned l = sumR; l < j; l++) {
                        tmpR[V[l]] += v;
                        // alpha[i*k+remap[l]] += v;
                    }
                    sumR = 0;
                }
            }
            if(sumR > 0) {
                unsigned v = sumR / V.size();
                sumR = sumR % V.size();
                for(unsigned l = 0; l < sumR; l++) {
                    tmpR[V[l]] += 1 + v;
                    // alpha[i*k+remap[l]] += 1;
                }
                for(unsigned l = sumR; l < V.size(); l++) {
                    tmpR[V[l]] += v;
                    // alpha[i*k+remap[l]] += v;
                }
            }
        }
        
    }

    ui prefix_min_rho = tmpR[reordered[0]];
    ui suffix_max_rho = 0;
    for (unsigned i = 1; i < sz; ++i)
        if (prefix_min_rho > tmpR[reordered[i]])
            prefix_min_rho = tmpR[reordered[i]];
    for (unsigned i = sz; i < n; i++)
        if (suffix_max_rho < tmpR[reordered[i]])
            suffix_max_rho = tmpR[reordered[i]];
    printf("min-max:%u-%u\n", prefix_min_rho, suffix_max_rho);
    return prefix_min_rho > suffix_max_rho;
}

// bool cliqueE_kClist::checkStableFlow(std::vector<ui> & r, 
//     ui sz, std::vector<ui> & reordered, std::vector<ui> & level, 
//     ui T, ui & nexSize) {
//     ui n = r.size();
//     std::vector<ui> tmpR(n);
//     memcpy(tmpR.data(), r.data(), sizeof(ui)*n);
//     // std::vector<ui> & tmpR = r;
    
//     for(ull i = 0; i < cntClique; i++) {
//         // ui smallerThanSz = 0;
//         // for (unsigned j = 0; j < k; ++j) {
//         //     if (level[cliques[i * k + j]] < sz) {
//         //         smallerThanSz++;
//         //     } 
//         // }
//         unsigned max_level = 0, max_level_cnt = 0;
//         for (unsigned j = 0; j < k; ++j) {
//             if (level[cliques[i * k + j]] > max_level) {
//                 max_level = level[cliques[i * k + j]];
//                 max_level_cnt = 1;
//             } else if (level[cliques[i * k + j]] == max_level) {
//                 ++max_level_cnt;
//             }
//         }
//         ui sum = 0;
//         for (unsigned j = 0; j < k; ++j) {
//             if (level[cliques[i * k + j]] < max_level) {
//                 sum += alpha[i * k + j];
//                 tmpR[cliques[i * k + j]] -= alpha[i * k + j];
//                 // alpha[i * k + j] = 0;
//             }
//         }
//         ui v = sum / max_level_cnt;
//         for (unsigned j = 0; j < k; ++j) {
//             if (level[cliques[i * k + j]] == max_level) {
//                 tmpR[cliques[i * k + j]] += v;
//                 // alpha[i * k + j] += v;
//             }
//         }
//         sum = sum % max_level_cnt;
//         if(sum > 0) {
//             for (unsigned j = 0; sum > 0; ++j) {
//                 if (level[cliques[i * k + j]] == max_level) {
//                     tmpR[cliques[i * k + j]] += 1;
//                     // alpha[i * k + j]++;
//                     --sum;
//                 }
//             }
//         }
//     }

//     ui prefix_min_rho = tmpR[reordered[0]];
//     ui suffix_max_rho = 0;
//     for (unsigned i = 1; i < sz; ++i)
//         if (prefix_min_rho > tmpR[reordered[i]])
//             prefix_min_rho = tmpR[reordered[i]];
//     for (unsigned i = sz; i < n; i++)
//         if (suffix_max_rho < tmpR[reordered[i]])
//             suffix_max_rho = tmpR[reordered[i]];
//     printf("min-max:%u-%u\n", prefix_min_rho, suffix_max_rho);
//     return prefix_min_rho > suffix_max_rho;
// }


// void scanBuildResnetPreflow(
//         std::function<void(
//         const std::vector<ui>::iterator &, 
//         const std::vector<ui>::iterator &,
//         ui,
//         std::vector<ui> *
//                 )> f) {
    
// }

#include "../tools/comb.hpp"
void listingAll(ui l, ui j, std::vector<ui> & buffer, ui hh, ui pp, 
    std::vector<ui> & cliques, std::vector<ui> & bf) {
    if(l == 0) {
        for(ui i = 0; i < hh; i++) {
            cliques.push_back(buffer[i]);
        }
        for(ui i = 0; i < bf.size(); i++) cliques.push_back(bf[i]);
        return;
    }

    for(ui i = j; i+l <= pp+hh; i++) {
        bf.push_back(buffer[i]);
        listingAll(l-1, i+1, buffer, hh, pp, cliques, bf);
        bf.pop_back();
    }
}
void cliqueE_kClist::loadFromIndex(const std::string & path, ui k) {
    std::string fS = path + "sctS.txt";
    FILE *fpS = fopen(fS.c_str(), "r");
    ui cntSCTNode = 0;
    fscanf(fpS, "%u", &cntSCTNode);
    fclose(fpS);

    std::string fIph = path + "sctIdxph.idx";
    FILE *fpIdxph = fopen(fIph.c_str(), "rb");
    // phSCT.resize(cntSCTNode);
    std::vector<ui> phSCT(cntSCTNode);
    fread(phSCT.data(), 4, cntSCTNode, fpIdxph);
    fclose(fpIdxph);

    std::string fIpp = path + "sctIdxpp.idx";
    FILE *fpIdxpp = fopen(fIpp.c_str(), "rb");
    std::vector<ui> ppSCT(cntSCTNode+1);
    fread(ppSCT.data(), 4, cntSCTNode+1, fpIdxpp);
    fclose(fpIdxpp);

    comb cb;
    ui l = 0; 
    cntClique = 0;
    ui nxtPP = ppSCT[0];
    for(ui i = 0; i < cntSCTNode; i++) {
        ui pp = ppSCT[i+1]-phSCT[i];
        ui hh = phSCT[i]-ppSCT[i];
        if(pp+hh < k || hh > k) {
            continue;
        }
        
        cntClique += cb(pp, k-hh);
    }
// printf("%llu\n", cntClique);fflush(stdout);
    if(cntClique*k*4 >= 50*1024ull*1024*1024) {//let 50G be the upper bound of memory
        printf("too many cliques, need %f G\n", 
            1.0*(cntClique*k)*4/1024/1024/1024);
        fflush(stdout);
        exit(0);
        // return ;
    }

    std::string fE = path + "sctEdge.idx";
    FILE *fpEdge = fopen(fE.c_str(), "rb");
    cliques.resize(cntClique);
    cliques.clear();
    std::vector<ui> buffer(g->coreNumber+1);
    std::vector<ui> bf(k);
    bf.clear();
    for(ui i = 0; i < cntSCTNode; i++) {

        ui pp = ppSCT[i+1]-phSCT[i];
        ui hh = phSCT[i]-ppSCT[i];

        fread(buffer.data(), 4, pp + hh, fpEdge);

        if(pp+hh < k || hh > k) {
            continue;
        }
        if(hh == k) {
            for(ui j = 0; j < hh; j++) cliques.push_back(buffer[j]);
            continue;
        }

        listingAll(k-hh, hh, buffer, hh, pp, cliques, bf);

    }
assert(cliques.size() == cntClique * k);
    // fread(bufferSCT2V.data(), 4, ppSCT[cntSCTNode], fpEdge);
    fclose(fpEdge);
    // printf("cntSCTNode:%u\n", cntSCTNode);
}

void cliqueE_kClist::samplingFromIndex(const std::string & path, ui k, ui cntSampling) {
    std::string fS = path + "sctS.txt";
    FILE *fpS = fopen(fS.c_str(), "r");
    ui cntSCTNode = 0;
    fscanf(fpS, "%u", &cntSCTNode);
    fclose(fpS);

    std::string fIph = path + "sctIdxph.idx";
    FILE *fpIdxph = fopen(fIph.c_str(), "rb");
    // phSCT.resize(cntSCTNode);
    std::vector<ui> phSCT(cntSCTNode);
    fread(phSCT.data(), 4, cntSCTNode, fpIdxph);
    fclose(fpIdxph);

    std::string fIpp = path + "sctIdxpp.idx";
    FILE *fpIdxpp = fopen(fIpp.c_str(), "rb");
    std::vector<ui> ppSCT(cntSCTNode+1);
    fread(ppSCT.data(), 4, cntSCTNode+1, fpIdxpp);
    fclose(fpIdxpp);

    comb cb;
    ui l = 0; 
    cntClique = 0;
    ui nxtPP = ppSCT[0];
    for(ui i = 0; i < cntSCTNode; i++) {
        ui pp = ppSCT[i+1]-phSCT[i];
        ui hh = phSCT[i]-ppSCT[i];
        if(pp+hh < k || hh > k) {
            continue;
        }
        
        cntClique += cb(pp, k-hh);
    }
// printf("%llu\n", cntClique);fflush(stdout);
    if(cntClique*k*4 >= 50*1024ull*1024*1024) {//let 50G be the upper bound of memory
        printf("too many cliques, need %f G\n", 
            1.0*(cntClique*k)*4/1024/1024/1024);
        fflush(stdout);
        exit(0);
        // return ;
    }

    std::string fE = path + "sctEdge.idx";
    FILE *fpEdge = fopen(fE.c_str(), "rb");
    cliques.resize(cntClique);
    cliques.clear();
    std::vector<ui> buffer(g->coreNumber+1);
    std::vector<ui> bf(k);
    bf.clear();
    for(ui i = 0; i < cntSCTNode; i++) {

        ui pp = ppSCT[i+1]-phSCT[i];
        ui hh = phSCT[i]-ppSCT[i];

        fread(buffer.data(), 4, pp + hh, fpEdge);

        if(pp+hh < k || hh > k) {
            continue;
        }
        if(hh == k) {
            for(ui j = 0; j < hh; j++) cliques.push_back(buffer[j]);
            continue;
        }

        listingAll(k-hh, hh, buffer, hh, pp, cliques, bf);

    }
assert(cliques.size() == cntClique * k);
    // fread(bufferSCT2V.data(), 4, ppSCT[cntSCTNode], fpEdge);
    fclose(fpEdge);
    // printf("cntSCTNode:%u\n", cntSCTNode);
}