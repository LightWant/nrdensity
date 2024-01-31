#include "sct.h"
#include <cassert>
#include <algorithm>
#include <random>
// #define DEBUG
// #define DDEBUG

void listingAll(ui l, ui j, std::vector<ui> & bufferSCT2V, ui pp, ui ph,
    std::vector<ui> & bf, std::vector<ui> & bf2,
    cliqueE_kClist * kcl) {
    if(l == 0) {
        for(ui i = 0; i < bf2.size(); i++) {
            kcl->addV(bf2[i]);
        }
        for(ui i = pp; i < ph; i++) kcl->addV(bufferSCT2V[i]);
        kcl->addC();
        return;
    }

    for(ui i = j; i+l <= bf.size(); i++) {
        
        bf2.push_back(bf[i]);
        listingAll(l-1, i+1, bufferSCT2V, pp, ph, bf, bf2, kcl);
        bf2.pop_back();
    }
}

void SCT::buildCliquesFromSCTStableSubset(
    cliqueE_kClist * kcl, const std::vector<ui> & level, ui sz) {
// ull totalCntCliqueStable = 0;
    std::vector<ui> bf; 
    std::vector<ui> bf2(k); 

    for(ull i = 0; i < cntSCTNode; i++) {
        bool f = true;
        for(ui j = ppSCT[i]; j < phSCT[i]; j++) {
            if(level[bufferSCT2V[j]] >= sz) {
                f = false; break;
            }
        }
        if(!f) continue;

        ui h = phSCT[i] - ppSCT[i];
        ui p = 0;
        for(ui j = phSCT[i]; j < ppSCT[i+1]; j++) {
            if(level[bufferSCT2V[j]] < sz) {
                p++;
            }
        }

// totalCntCliqueStable += cb(p, k-h);
        if(p + h < k) continue;
        if(p + h == k) {
            for(ui j = ppSCT[i]; j < ppSCT[i+1]; j++) {
                if(level[bufferSCT2V[j]] < sz) {
                    kcl->addV(bufferSCT2V[j]);
                }
            }
            kcl->addC();
            continue;
        }
        bf.clear();
        for(ui j = phSCT[i]; j < ppSCT[i+1]; j++) {
            if(level[bufferSCT2V[j]] < sz) {
                bf.push_back(bufferSCT2V[j]);
            }
        }

        bf2.clear();
        listingAll(k-h, 0, bufferSCT2V, ppSCT[i], phSCT[i], bf, bf2, kcl);
    }

    std::vector<ui>().swap(bf); 
    std::vector<ui>().swap(bf2); 
// printf("%llu\n", totalCntCliqueStable);
}

bool SCT::checkSummaryOfAlpha(std::vector<ui> & r, ui T) {
    ull total = 0;
    std::vector<ui> tmp(g->n);

    for(ull ii = 0; ii < cntSCTNode; ii++) {
        ull lo = 0;
        for(ui j = ppSCT[ii]; j < ppSCT[ii+1]; j++) {
            lo += alpha[j];
            tmp[bufferSCT2V[j]] += alpha[j];
        }
        ui h = phSCT[ii] - ppSCT[ii];
        ui p = ppSCT[ii+1] - phSCT[ii];

        if(lo != cb(p, k-h)*T) {
// printf("ex1 %llu %llu\n", lo, (ull)(cb(p, k-h)*T));
// printf("%u\n", ii);
            return false;
        }
        total += lo;
    }

    if(total != cntClique*T) {
        printf("ex2\n");
        return false;
    }

    total = 0;
    for(ui i = 0; i < g->n; i++) {
        total += r[i];
    }

    if(total != cntClique*T) {
        printf("ex3\n");
        return false;
    }

    for(ui i = 0; i < g->n; i++) {
        if(tmp[i] != r[i]) {
            printf("ex4\n");
            // printf("%u %u %u\n", i, tmp[i], r[i]);
            return false;
        }
    }

    return true;
}

// #define DDEBUG


//re assign on average
bool SCT::checkStableFlow2(std::vector<ui> & r, 
    ui sz, std::vector<ui> & reordered, std::vector<ui> & level, ui T) {
    std::vector<ui> & V = tmpClique;
    std::vector<ui> tmpR(g->n);
    memcpy(tmpR.data(), r.data(), sizeof(ui)*g->n);
    // std::vector<ui> & tmpR = r;
#ifdef DDEBUG
ui xxx;
ui xx = 591645;
#endif
    auto f = [](std::vector<ui> * sct_,
        ui pp, ui n, ui sz, ui T, 
        ui upper0, ui upper1, std::vector<ui> & level,
        std::vector<ui> *V_, 
        std::vector<ui> * localCnt_,
        std::vector<ui> * r_ ) {
        
        std::vector<ui> & sct = (*sct_);
        std::vector<ui> & r = (*r_);
        std::vector<ui> & V = (*V_);

        V.clear();
        std::vector<ui> & localCnt = (*localCnt_);
 //sort V 
        for(ui i = 0; i < n; i++) V.push_back(pp+i);
        std::sort(V.begin(), V.end(), [&](ui a, ui b) {
            // if(r[sct[a]] == (r[sct[b]]))
            //     return localCnt[sct[a]] < localCnt[sct[b]];
            return r[sct[a]] < r[sct[b]];
        });

//the while loop
        ui st = 0;
        ui i = 1;
        while(i < n && r[sct[V[i]]] == r[sct[V[i-1]]]) {
            i++;
        }
        ui totalCnt = 0;//the count of cliques assigned
        ui totalCnt0 = 0;
        ui totalCnt1 = 0;
        ui gap = i<n ? r[sct[V[i]]]-r[sct[V[i-1]]] : UINT32_MAX;

#ifdef DDEBUG
if(xxx == xx) {
printf("st %u, i %u, gap %u, n %u\n", st, i, gap, n);
fflush(stdout);
}
#endif
        while(totalCnt < upper0+upper1) {
            //uodate 0
            do {
                ui minV = std::min(gap, upper0 - totalCnt0);
                ui cntNodes = 0;
                for(ui j = st; j < i; j++) {
                    if(level[sct[V[j]]] >= sz) continue;
                    minV = std::min(minV, localCnt[V[j]-pp]);
                    cntNodes++;
                }
                if(cntNodes == 0) break;
#ifdef DDEBUG
if(xxx == xx) {
printf("totalCnt0 %u upper0 %u minV %u cnt %u\n", 
totalCnt0, upper0, minV, cntNodes);
}
#endif
                if(minV*cntNodes <= upper0 - totalCnt0) {
                    for(ui j = st; j < i; j++) {
                        if(level[sct[V[j]]] >= sz) continue;

                        r[sct[V[j]]] += minV*T;
                        localCnt[V[j]-pp] -= minV;
                        totalCnt0 += minV;
                        totalCnt += minV;
#ifdef DDEBUG
if(xxx == xx) {
printf("add01 %u %u\n", V[j], minV);
}
#endif
                        if(localCnt[V[j]-pp] == 0) {
                            std::swap(V[st], V[j]);
                            st++;
                        }
                    }
// assert(totalCnt <= upper);
                }
                else {
                    ui v = (upper0 - totalCnt0) / cntNodes;//v <= minV
                    ui vmod = (upper0 - totalCnt0) % cntNodes;

                    for(ui j = st; j < i; j++) {
                        if(level[sct[V[j]]] >= sz) continue;

                        if(vmod>0) {
                            vmod--;
                            r[sct[V[j]]] += (v+1)*T;
                            localCnt[V[j]-pp] -= (v+1);
                            totalCnt0 += (v+1);
                            totalCnt += (v+1);
#ifdef DDEBUG
if(xxx == xx) {
printf("add02 %u %u, vmod %u, j%u, st + vmod%u\n", V[j], v+1, vmod, j, st + vmod);
}
#endif
                        }
                        else {
                            r[sct[V[j]]] += v*T;
                            localCnt[V[j]-pp] -= v;
                            totalCnt0 += v;
                            totalCnt += v;
#ifdef DDEBUG
if(xxx == xx) {
printf("add03 %u %u, j%u\n", V[j], v,  j);
}
#endif
                        }

                        if(localCnt[V[j]-pp] == 0) {
                            std::swap(V[st], V[j]);
                            ++st;
                        }
                    }
// assert(totalCnt <= upper);
                }

            } while(totalCnt0 < upper0 && r[sct[V[i-1]]] < r[sct[V[i]]] && st < i);

            do {
                ui minV = std::min(gap, upper1 - totalCnt1);
                ui cntNodes = 0;
                for(ui j = st; j < i; j++) {
                    if(level[sct[V[j]]] < sz) continue;
                    minV = std::min(minV, localCnt[V[j]-pp]);
                    cntNodes++;
                }
                if(cntNodes == 0) break;
#ifdef DDEBUG
if(xxx == xx) {
printf("totalCnt1 %u upper1 %u minV %u cnt %u\n", 
totalCnt1, upper1, minV, cntNodes);
}
#endif
                if(minV*cntNodes <= upper1 - totalCnt1) {
                    for(ui j = st; j < i; j++) {
                        if(level[sct[V[j]]] < sz) continue;

                        r[sct[V[j]]] += minV*T;
                        localCnt[V[j]-pp] -= minV;
                        totalCnt1 += minV;
                        totalCnt += minV;
#ifdef DDEBUG
if(xxx == xx) {
printf("add11 %u %u\n", V[j], minV);
}
#endif
                        if(localCnt[V[j]-pp] == 0) {
                            std::swap(V[st], V[j]);
                            st++;
                        }
                    }
// assert(totalCnt <= upper);
                }
                else {
                    ui v = (upper1 - totalCnt1) / cntNodes;//v <= minV
                    ui vmod = (upper1 - totalCnt1) % cntNodes;

                    for(ui j = st; j < i; j++) {
                        if(level[sct[V[j]]] < sz) continue;

                        if(vmod>0) {
                            vmod--;
                            r[sct[V[j]]] += (v+1)*T;
                            localCnt[V[j]-pp] -= (v+1);
                            totalCnt1 += (v+1);
                            totalCnt += (v+1);
#ifdef DDEBUG
if(xxx == xx) {
printf("add12 %u %u, vmod %u, j%u, st + vmod%u\n", V[j], v+1, vmod, j, st + vmod);
}
#endif
                        }
                        else {
                            r[sct[V[j]]] += v*T;
                            localCnt[V[j]-pp] -= v;
                            totalCnt1 += v;
                            totalCnt += v;
#ifdef DDEBUG
if(xxx == xx) {
printf("add13 %u %u\n", V[j], v+1);
}
#endif
                        }

                        if(localCnt[V[j]-pp] == 0) {
                            std::swap(V[st], V[j]);
                            ++st;
                        }
                    }
// assert(totalCnt <= upper);
                }
            } while(totalCnt1 < upper1 && r[sct[V[i-1]]] < r[sct[V[i]]] && st < i);


            ui iNew = std::min(i+1, n);
            while(iNew < n && r[sct[V[iNew]]] == r[sct[V[iNew-1]]]) {
                iNew++;
            }
            ui gapNew = iNew<n ? r[sct[V[iNew]]]-r[sct[V[iNew-1]]] : UINT32_MAX;
// #ifdef DDEBUG
// printf("totalCnt3 %u ", totalCnt);fflush(stdout);
// #endif
            ui stNew = st;
            while(stNew < iNew && localCnt[V[stNew]-pp] == 0) {
                stNew++;
            }

            i= iNew;
            st = stNew;
            gap = gapNew;
        }
assert(totalCnt0 == upper0);
assert(totalCnt1 == upper1);
assert(totalCnt == upper0+upper1);
    };

    for(ull ii = 0; ii < cntSCTNode; ii++) {
        ui h0 = 0, p0 = 0, h1 = 0, p1 = 0;
        for(ui j = ppSCT[ii]; j < phSCT[ii]; j++) {
            if(level[bufferSCT2V[j]] < sz) h0++;
            else h1++;
        }
        for(ui j = phSCT[ii]; j < ppSCT[ii+1]; j++) {
            if(level[bufferSCT2V[j]] < sz) p0++;
            else p1++;
        }

        if(p1 + h1 == 0 || p0 + h0 == 0) continue;

        ui* sct = bufferSCT2V.data() + ppSCT[ii];
        ui n = ppSCT[ii+1] - ppSCT[ii];
        ui st = ppSCT[ii];

        ui h = phSCT[ii] - ppSCT[ii];
        ui p = ppSCT[ii+1] - phSCT[ii];

        ull cliquesIn0 = h1==0?cb(p0, k-h0):0;
        ull cliquesIn1 = h0==0?cb(p1, k-h1):0;
        ull cliquesInter = cb(p, k-h)-cliquesIn0-cliquesIn1;

        for(ui j = ppSCT[ii]; j < ppSCT[ii+1]; j++) {
            tmpR[bufferSCT2V[j]] -= alpha[j];
        }

        for(ui j = ppSCT[ii]; j < phSCT[ii]; j++) {
            localCnt[j-ppSCT[ii]] = 0;
            if(level[bufferSCT2V[j]] < sz) {
                localCnt[j-ppSCT[ii]] = (h1>0&&p0>=k-h0)?0:cb(p0, k-h0);
            }
            else {
                ui pSmaller = 0;
                for(ui j2 = phSCT[ii]; j2 < ppSCT[ii+1]; j2++) {
                    if(level[bufferSCT2V[j2]] < level[bufferSCT2V[j]]) {
                        pSmaller++;
                    }
                }
                if(pSmaller >= k-h)
                    localCnt[j-ppSCT[ii]] = cb(pSmaller, k-h);
            }
        }

        for(ui j = phSCT[ii]; j < ppSCT[ii+1]; j++) {
            localCnt[j-ppSCT[ii]] = 0;
            if(level[bufferSCT2V[j]] < sz) {
                localCnt[j-ppSCT[ii]] = (h1>0&&p0>=k-h0)?0:cb(p0-1, k-h0-1);
            }
            else {
                ui pSmaller = 0;
                for(ui j2 = phSCT[ii]; j2 < ppSCT[ii+1]; j2++) {
                    if(level[bufferSCT2V[j2]] < level[bufferSCT2V[j]]) {
                        pSmaller++;
                    }
                }
                if(k>h && pSmaller >= k-h-1)
                    localCnt[j-ppSCT[ii]] = cb(pSmaller, k-h-1);
            }
        }

#ifdef DDEBUG
xxx=ii;
if(xxx == xx) {
printf("%u\n", ii);fflush(stdout);
}
#endif
        f(&bufferSCT2V, ppSCT[ii], n, sz, T,  cliquesIn0, cliquesInter+cliquesIn1, 
           level, &V, &localCnt, &tmpR);
    }

    ui maxR0 = 0;
    ui minR = tmpR[reordered[0]];
    for(ui i = 1; i < sz; i++) {
        minR = std::min(minR, tmpR[reordered[i]]);
        maxR0 = std::max(maxR0, tmpR[reordered[i]]);
    }

    ui maxR = tmpR[reordered[sz]];
    ui minR0 = tmpR[reordered[sz]];
    for(ui i = sz; i < g->n; i++) {
        maxR = std::max(maxR, tmpR[reordered[i]]);
        minR0 = std::min(minR0, tmpR[reordered[i]]);
    }
printf("1min-max:%u-%u/%u-%u\n", maxR0/T, minR/T, maxR/T, minR0/T);

    if(minR > maxR) return true;
    return false;
}

bool SCT::checkStableFlow(std::vector<ui> & r, 
    ui sz, std::vector<ui> & reordered, std::vector<ui> & level, 
    ui T
    , ui & nexSize) {
    std::vector<ui> & V = tmpClique;
    std::vector<ui> tmpR(g->n);
    memcpy(tmpR.data(), r.data(), sizeof(ui)*g->n);
    // std::vector<ui> & tmpR = r;

    for(ull ii = 0; ii < cntSCTNode; ii++) {
        ui h0 = 0, p0 = 0, h1 = 0, p1 = 0;
        for(ui j = ppSCT[ii]; j < phSCT[ii]; j++) {
            if(level[bufferSCT2V[j]] < sz) h0++;
            else h1++;
        }
        for(ui j = phSCT[ii]; j < ppSCT[ii+1]; j++) {
            if(level[bufferSCT2V[j]] < sz) p0++;
            else p1++;
        }

        if(p1 + h1 == 0 || p0 + h0 == 0) continue;

        ui* sct = bufferSCT2V.data() + ppSCT[ii];
        ui n = ppSCT[ii+1] - ppSCT[ii];
        ui st = ppSCT[ii];

        ui h = phSCT[ii] - ppSCT[ii];
        ui p = ppSCT[ii+1] - phSCT[ii];

        V.clear();
 //sort V
        for(ui i = 0; i < n; i++) V.push_back(i);
        std::sort(V.begin(), V.end(), [&](ui a, ui b) {
            return tmpR[sct[a]] < tmpR[sct[b]];
        });

        ull cliquesIn0 = h1==0?cb(p0, k-h0):0;
        ull cliquesIn1 = h0==0?cb(p1, k-h1):0;
        ull cliquesInter = cb(p, k-h)-cliquesIn0-cliquesIn1;
        ui upper0 = cliquesIn0;
        ui upper1 = cliquesInter+cliquesIn1;
// printf("stup1 %u, up0 %u, p %u h %u, p0 %u, p1 %u\n", 
//     upper1, upper0, p, h, p0, p1);
//the while loop
// ull s1 = 0;   
//the weight of cliquesInter that assigns to stable
        for(ui j = ppSCT[ii]; j < ppSCT[ii+1]; j++) {
            tmpR[bufferSCT2V[j]] -= alpha[j];
// s1 += alpha[j];
            // alpha[j] = 0;
        }
// assert(s1 == cb(p, k-h)*T);
// assert(upper0+upper1 == cb(p, k-h));
// ull s2 = 0;

// for(ui i = 0; i < n; i++) printf("%u ", level[sct[V[i]]]);
// printf("\n");
// for(ui i = 0; i < n; i++) printf("%u ", r[sct[V[i]]]);
// printf("\n");

        for(ui i = 0; i < n; i++) {
            if(level[sct[V[i]]] < sz) {

                if(V[i] < h) {
// printf("add001 up0 %u, pkh %u %u %u, %u, level %u\n", 
//     upper0, p, k, h, (ui)cb(p, k-h), level[sct[V[i]]]);
                    ui val = std::min(upper0, (ui)cb(p0, k-h));
                    tmpR[sct[V[i]]] += val*T;
                    upper0 -= val;
// s2 += val*T;
                }
                else {
// printf("add011 up0 %u, pkh %u %u %u, %u, level %u\n", 
    // upper0, p, k, h, (ui)cb(p, k-h), level[sct[V[i]]]);
                    ui val = std::min(upper0, (ui)cb(p0-1, k-h-1));
                    tmpR[sct[V[i]]] += val*T;
                    upper0 -= val;
                    p--;
                    p0--;
// s2 += val*T;
                }
                continue;
            }
            if(V[i] < h) {
// printf("add1 up1 %u, pkh %u %u %u, %u, level %u\n", 
    // upper1, p, k, h, (ui)cb(p, k-h), level[sct[V[i]]]);
                ui val = std::min(upper1, (ui)cb(p, k-h));
                tmpR[sct[V[i]]] += val*T;
                upper1 -= val;
// s2 += val*T;
                // alpha[V[i]+st] += cb(p, k-h)*T;
                break;
            }
            else {
// printf("add2 up1 %u, pkh %u %u %u, %u, level %u\n", 
    // upper1, p, k, h, (ui)cb(p-1, k-h-1), level[sct[V[i]]]);
                ui val = std::min(upper1, (ui)cb(p-1, k-h-1));
                tmpR[sct[V[i]]] += val*T;
                upper1 -= val;
                p--;
// s2 += val*T;
                // alpha[V[i]+st] += cb(p-1, k-h-1)*T;
            }
        }
// assert(upper0 == 0);
// assert(upper1 == 0);
// assert(s1 == s2);
    }

    ui maxR0 = 0;
    ui minR = tmpR[reordered[0]];
    for(ui i = 1; i < sz; i++) {
        minR = std::min(minR, tmpR[reordered[i]]);
        maxR0 = std::max(maxR0, tmpR[reordered[i]]);
    }
    ui maxR = tmpR[reordered[sz]];
    ui minR0 = tmpR[reordered[sz]];
    // ui maxRIdx = sz;
    ui maxRIdx = sz;
    for(ui i = sz; i < g->n; i++) {
        if(tmpR[reordered[i]] > maxR) {
            maxRIdx = i;
            maxR = tmpR[reordered[i]];
        }
        // maxR = std::max(maxR, tmpR[reordered[i]]);
        minR0 = std::min(minR0, tmpR[reordered[i]]);
    }
    nexSize = maxRIdx + 1;
printf("1min-max:%u-%u/%u-%u\n", maxR0, minR, maxR, minR0);
printf("nexSIze:%u sz %u\n", nexSize, sz);

    if(minR > maxR) return true;
    
// average
    for(ull ii = 0; ii < cntSCTNode; ii++) {
        ui stableSize = 0;
        for(ui j = ppSCT[ii]; j < ppSCT[ii+1]; j++) {
            if(level[bufferSCT2V[j]] < sz) stableSize++;
        }
        
        if(stableSize != 0 && stableSize != ppSCT[ii+1]-ppSCT[ii]) {
            continue;
        }

        ui* sct = bufferSCT2V.data() + ppSCT[ii];
        ui n = ppSCT[ii+1] - ppSCT[ii];
        ui st = ppSCT[ii];

        ui h = phSCT[ii] - ppSCT[ii];
        ui p = ppSCT[ii+1] - phSCT[ii];
// ull s1 = 0;   
        for(ui j = ppSCT[ii]; j < ppSCT[ii+1]; j++) {
// assert(tmpR[bufferSCT2V[j]] >= alpha[j]);
            tmpR[bufferSCT2V[j]] -= alpha[j];
// s1 += alpha[j];
            // alpha[j] = 0;
        }
// assert(s1 == cb(p, k-h)*T);
        V.clear();
 //sort V
        for(ui i = 0; i < n; i++) V.push_back(i);
      
        std::sort(V.begin(), V.end(), [&](ui a, ui b) {
            return tmpR[sct[a]] < tmpR[sct[b]];
        });

//the while loop
// ull s2 = 0;
        for(ui i = 0; i < n; i++) {
            if(V[i] < h) {
                tmpR[sct[V[i]]] += cb(p, k-h)*T;
                // alpha[V[i]+st] += cb(p, k-h)*T;
// s2 += cb(p, k-h)*T;
                break;
            }
            else {
                tmpR[sct[V[i]]] += cb(p-1, k-h-1)*T;
// s2 += cb(p-1, k-h-1)*T;
                // alpha[V[i]+st] += cb(p-1, k-h-1)*T;
                p--;
            }
        }
// assert(s1 == s2);
    }

    minR = tmpR[reordered[0]];
    for(ui i = 1; i < sz; i++) {
        minR = std::min(minR, tmpR[reordered[i]]);
    }
    maxR = tmpR[reordered[sz]];
    ui idx = 0;
    for(ui i = sz; i < g->n; i++) {
        maxR = std::max(maxR, tmpR[reordered[i]]);
    }
printf("2min-max:%u-%u\n", minR, maxR);

    if(minR > maxR) return true;
    return false;
}

void SCT::scanSCTFlow( std::function<void(
        std::vector<ui> * sct_, ui pp,
        ui n,  
        ull upper, //total count of k-cliques
        ui *alpha, 
        std::vector<ui> *V_, 
        std::vector<ui> * localCnt_
                )> f) {
    
    tmpClique.clear();
    std::vector<ui> * V = &tmpClique;
    std::vector<ui> * localCnt_ = &localCnt;

    using itr = std::vector<ui>::iterator;

    initShuffle(cntSCTNode);
    if(alpha.size() < ppSCT[cntSCTNode]) {
        alpha.resize(ppSCT[cntSCTNode]);
    }

    for(ull ii = 0; ii < cntSCTNode; ii++) {
        ull i = shuffleNumbers[ii];
#ifdef DEBUG
printf("i %llu\n", i);fflush(stdout);
#endif
        
        ui n = ppSCT[i+1] - ppSCT[i];

        ui h = phSCT[i] - ppSCT[i];
        ui p = ppSCT[i+1] - phSCT[i];
        ull upper = cb(p, k-h);
#ifdef DEBUG
printf("h:");
#endif
        for(ui j = ppSCT[i]; j < phSCT[i]; j++) {
#ifdef DEBUG
printf("%u ", bufferSCT2V[j]);
#endif
            localCnt[j - ppSCT[i]] = upper;
        }
#ifdef DEBUG
printf(" p:");
#endif
        ui pupper = (p>=1 && k>=h+1) ? cb(p-1, k-h-1) : 0;
        for(ui j = phSCT[i]; j < ppSCT[i+1]; j++) {
#ifdef DEBUG
printf("%u ", bufferSCT2V[j]);
#endif
            localCnt[j - ppSCT[i]] = pupper;
        }
#ifdef DEBUG
printf("\n");
printf("pupper %llu %u %u %u\n\n", pupper, p, k, h);
#endif
        f(&bufferSCT2V, ppSCT[i], n, upper, alpha.data(), V, localCnt_);
#ifdef DEBUG
printf("after3\n");
#endif
    }
}

void SCT::sortSCTNodesByMinR( std::vector<ui> & r) {
    if(SCTNodesWeight.size() < cntSCTNode) {
        SCTNodesWeight.resize(cntSCTNode);
    }
    for(ull i = 0; i < cntSCTNode; i++) {
        ui minR = r[bufferSCT2V[ppSCT[i]]];
        for(ui j = ppSCT[i]+1; j < ppSCT[i+1]; j++) {
            minR = std::min(minR, r[bufferSCT2V[j]]);
        }
        SCTNodesWeight[i] = minR;
    }

    std::sort(shuffleNumbers.begin(), shuffleNumbers.end(), 
        [&](ui a, ui b) {
        return SCTNodesWeight[a] < SCTNodesWeight[b];
    });
}


void SCT::initShuffle(ui sz) {
    // if(shuffleNumbers.size() >= sz) return;
    if(shuffleNumbers.size() < sz) shuffleNumbers.resize(sz);
    else {
        bool ok = true;
        for(ui i = 0; i < sz; i++) {
            if(shuffleNumbers[i] >= sz) {
                ok = false; break;
            }
        }
        if(!ok) {//for exactWk
            for(ui i = 0; i < sz; i++) shuffleNumbers[i] = i;
        }
        return;
    }
    for(ui i = 0; i < sz; i++) shuffleNumbers[i] = i;

    std::random_device rd;
    std::mt19937 gg(31);
    
    typedef std::uniform_int_distribution<ui> distr_t;
    typedef typename distr_t::param_type param_t;
    
    distr_t D;
    if(sz>1)
    for(ui i = sz-1; i > 0; i--) {
        ui j = D(gg, param_t(0, i));
        std::swap(shuffleNumbers[i], shuffleNumbers[j]);
    }
}

void SCT::sctppFixedBugPupper(std::function<
            void(const std::vector<ui>::iterator &, 
            const std::vector<ui>::iterator &, 
            const std::vector<ui>::iterator &, 
                const std::vector<ui>::iterator &,
                ui, std::vector<ui> *,
                std::vector<bool> & ,
                comb &,
                std::vector<ui> *
        )> f) {
    if(isP.size() < g->n) isP.resize(g->n);
    tmpClique.clear();
    std::vector<ui> * V = &tmpClique;
    std::vector<ui> * localCnt_ = &localCnt;

    using itr = std::vector<ui>::iterator;

    initShuffle(cntSCTNode);

    for(ull ii = 0; ii < cntSCTNode; ii++) {
        ull i = shuffleNumbers[ii];

// #define DEBUG
#ifdef DEBUG
printf("i %llu, ii %u\n", i, ii);fflush(stdout);
#endif
        itr hbg = bufferSCT2V.begin() + ppSCT[i];
        itr hed = bufferSCT2V.begin() + phSCT[i];
        itr pbg = bufferSCT2V.begin() + phSCT[i];
        itr ped = bufferSCT2V.begin() + ppSCT[i+1];

        ui h = phSCT[i] - ppSCT[i];
        ui p = ppSCT[i+1] - phSCT[i];
        ui upper = cb(p, k-h);
#ifdef DEBUG
printf("h:");
#endif
        for(ui j = ppSCT[i]; j < phSCT[i]; j++) {
#ifdef DEBUG
printf("%u ", bufferSCT2V[j]);
#endif
            localCnt[bufferSCT2V[j]] = upper;
        }
#ifdef DEBUG
printf(" p:");
#endif
        ui pupper = (p>=1 && k>=h+1) ? cb(p-1, k-h-1) : 0;
        for(ui j = phSCT[i]; j < ppSCT[i+1]; j++) {
#ifdef DEBUG
printf("%u ", bufferSCT2V[j]);
#endif
            localCnt[bufferSCT2V[j]] = pupper;
        }
#ifdef DEBUG
printf("\n");
printf("pupper %u %u %u %u\n\n", pupper, p, k, h);
#endif
        f(hbg, hed, pbg, ped, upper, V, isP, cb, localCnt_);
#ifdef DEBUG
printf("after3\n");
#endif
// #undef DEBUG
    }


}

void SCT::scanSCT(
        std::function<
            void(const std::vector<ui>::iterator &, 
            const std::vector<ui>::iterator &, 
            const std::vector<ui>::iterator &, 
                const std::vector<ui>::iterator &,
                ui, 
                std::vector<ui> *,
                std::vector<ui> *)> f
    )
{   
    tmpClique.clear();
    std::vector<ui> * V = &tmpClique;
    std::vector<ui> * localCnt_ = &localCnt;

    using itr = std::vector<ui>::iterator;

    initShuffle(cntSCTNode);

    for(ull ii = 0; ii < cntSCTNode; ii++) {
        ull i = shuffleNumbers[ii];
// printf("i %llu ", i);fflush(stdout);
#ifdef DEBUG
printf("i %llu\n", i);fflush(stdout);
#endif
        itr hbg = bufferSCT2V.begin() + ppSCT[i];
        itr hed = bufferSCT2V.begin() + phSCT[i];
        itr pbg = bufferSCT2V.begin() + phSCT[i];
        itr ped = bufferSCT2V.begin() + ppSCT[i+1];

        ui h = phSCT[i] - ppSCT[i];
        ui p = ppSCT[i+1] - phSCT[i];
        ui upper = cb(p, k-h);

#ifdef DEBUG
printf("h:");
#endif
        for(ui j = ppSCT[i]; j < phSCT[i]; j++) {
#ifdef DEBUG
printf("%u ", bufferSCT2V[j]);
#endif
            localCnt[bufferSCT2V[j]] = upper;
        }
#ifdef DEBUG
printf(" p:");
#endif
        ui pupper = (p>=1 && k>=h+1) ? cb(p-1, k-h-1) : 0;
        for(ui j = phSCT[i]; j < ppSCT[i+1]; j++) {
#ifdef DEBUG
printf("%u ", bufferSCT2V[j]);
#endif
            localCnt[bufferSCT2V[j]] = pupper;
        }
#ifdef DEBUG
printf("\n");
printf("pupper %u %u %u %u\n\n", pupper, p, k, h);
#endif
        f(hbg, hed, pbg, ped, upper, V, localCnt_);
#ifdef DEBUG
printf("after3\n");
#endif
    }
}

void SCT::scanSCTPava(std::vector<ui> & level, std::vector<ui> & rPava) {
    using itr = std::vector<ui>::iterator;

    std::vector<ui> tmp;

    for(ull i = 0; i < cntSCTNode; i++) {
        itr hbg = bufferSCT2V.begin() + ppSCT[i];
        itr hed = bufferSCT2V.begin() + phSCT[i];
        itr pbg = bufferSCT2V.begin() + phSCT[i];
        itr ped = bufferSCT2V.begin() + ppSCT[i+1];
        ui h = phSCT[i] - ppSCT[i];
        ui p = ppSCT[i+1] - phSCT[i];

        ui maxJ = level[bufferSCT2V[ppSCT[i]]];
        for(ui j = ppSCT[i]; j < phSCT[i]; j++) {
            if(level[bufferSCT2V[j]] > maxJ) {
                maxJ = level[bufferSCT2V[j]];
            }
        }

        ui cntLargerThanMaxj = 0;
        for(itr j = pbg; j < ped; j++) {
            if(level[*j] > maxJ) cntLargerThanMaxj++;
        }
        rPava[maxJ] += cb(p-cntLargerThanMaxj, k-h);

        tmp.clear();
        for(ui i = 0; i < p; i++) {
            tmp.push_back(i);
        }
        std::sort(tmp.begin(), tmp.end(), [&](ui a, ui b) {
            return level[bufferSCT2V[a+phSCT[i]]] > level[bufferSCT2V[b+phSCT[i]]];
        });

        ui t = 0;
        for(ui j = 0; j < p; j++) {
            ui v = bufferSCT2V[tmp[j] + phSCT[i]];
            if(level[v] > maxJ) {
                rPava[level[v]] += cb(p-t-1, k-h-1);
                t++;
                if(p < t+1 || p+h < k+t) break;
            }
        }
        // for(itr j = pbg; j < ped; j++) {
        //     if(level[*j] > maxJ) {
        //         rPava[level[*j]] += cb(p-t-1, k-h-1);
        //         t++;
        //         if(p < t+1 || k < h+t+1) break;
        //     }
        // }
    }
}

void SCT::scan(
        std::function<
            void(const std::vector<ui>::iterator &, 
                const std::vector<ui>::iterator &)> f
    )
{
    for(ull i = 0; i < cntSCTNode; i++) {
// printf("s %llu\n", i);fflush(stdout);
tmpClique.clear();
        for(ui j = ppSCT[i]; j < phSCT[i]; j++) {
            tmpClique.push_back(bufferSCT2V[j]);
        }
// printf("s %llu\n", i);fflush(stdout);   
        if(phSCT[i] < ppSCT[i+1])
            scanEnu(ppSCT[i+1], phSCT[i]-ppSCT[i], phSCT[i], f);
        else f(tmpClique.begin(), tmpClique.end());

        for(ui j = ppSCT[i]; j < phSCT[i]; j++) {
            tmpClique.pop_back();
        }
    }
}

ull totalAssigned = 0;
ull totalLimed = 0;

// #define DEBUGWK
#ifdef DEBUGWK
ui xx = 507531;
ui xxx;
#endif
void batchupdate(
    std::vector<ui> &Vh, 
    std::vector<ui> &Vp, 
    ull lim, 
    std::vector<ui> &r, comb & cb, ui k, ui deep) {
#ifdef DEBUGWK
if(xxx ==xx) {
printf(" deep %u\n", deep);
printf("Vh:");for(ui v : Vh) printf("%u ", v); printf("\n");
printf("Vp:");for(ui v : Vp) printf("%u ", v); printf("\n");
printf("lim:%llu\n", lim);
printf("totalAssigned:%llu\n", totalAssigned);
}
#endif
    if(lim == 0) return;
    if(totalAssigned == totalLimed) return;
    if(Vh.size() == k) {
        ui vminh = Vh[0];
        for(ui i = 1; i < Vh.size(); i++) {
            if(r[Vh[r[i]]] < r[vminh]) {
                vminh = Vh[r[i]];
            }
        }
        r[vminh] += 1;
totalAssigned+=1;
#ifdef DEBUGWK
if(xxx ==xx) {
printf("0 %u+%u\n", vminh, 1);
}
#endif
        // if(lim > 1) batchupdate(Vh, Vp, lim-1, r, cb, k, deep+1);
        return;
    }
    if(Vp.size()+Vh.size() == k) {
        ui vwmin = Vh[0];
        for(ui i = 1; i < Vh.size(); i++) {
            if(r[Vh[r[i]]] < r[vwmin]) {
                vwmin = Vh[r[i]];
            }
        }
        for(ui i = 0; i < Vp.size(); i++) {
            if(r[Vp[r[i]]] < r[vwmin]) {
                vwmin = Vp[r[i]];
            }
        }
        r[vwmin] += 1;
totalAssigned+=1;
#ifdef DEBUGWK
if(xxx ==xx) {
printf("1 %u+%u\n", vwmin, 1);
}
#endif
        // if(lim > 1) batchupdate(Vh, Vp, lim-1, r, cb, k, deep+1);
        return;
    }
    if(Vp.size()+Vh.size() < k) return;

    ui minp, minh, secondp, secondh, wmin, wsecond;
    ui vminp, vminh, vsecondp, vsecondh, vwmin, vwsecond;
    ui ivminp, ivsecondp;

    if(r[Vp[1]] < r[Vp[0]]) {
        minp = r[Vp[1]]; vminp = Vp[1]; ivminp = 1;
        secondp = r[Vp[0]]; vsecondp = Vp[0]; ivsecondp = 0;
    }
    else {
        minp = r[Vp[0]]; vminp = Vp[0]; ivminp = 0;
        secondp = r[Vp[1]]; vsecondp = Vp[1]; ivsecondp = 1;
    }
    for(ui i = 2; i < Vp.size(); i++) {
        if(r[Vp[i]] < minp) {
            secondp = minp; vsecondp = vminp; ivsecondp = ivminp;
            minp = r[Vp[i]]; vminp = Vp[i]; ivminp = i;
        }
        else if(r[Vp[i]] != minp && r[Vp[i]] < secondp) {
            secondp = r[Vp[i]]; vsecondp = Vp[i]; ivsecondp = i;
        }
    }
    
    if(Vh.size() == 1) {
        minh = r[Vh[0]]; vminh = Vh[0];
        secondh = -1;
    }
    else {
        if(r[Vh[1]] < r[Vh[0]]) {
            minh = r[Vh[1]]; vminh = Vh[1];
            secondh = r[Vh[0]]; vsecondh = Vh[0];
        }
        else {
            minh = r[Vh[0]]; vminh = Vh[0];
            secondh = r[Vh[1]]; vsecondh = Vh[1];
        }
        for(ui i = 2; i < Vh.size(); i++) {
            if(r[Vh[i]] < minh) {
                secondh = minh; vsecondh = vminh;
                minh = r[Vh[i]]; vminh = Vh[i];
            }
            else if(r[Vh[i]] != minh && r[Vh[i]] < secondh) {
                secondh = r[Vh[i]]; vsecondh = Vh[i];
            }
        }
    }

    if(minp < minh) {
        wmin = minp; vwmin = vminp;
        if(minh < secondp) {
            wsecond = minh; vwsecond = vminh;
        }
        else {
            wsecond = secondp; vwsecond = vsecondp;
        }
    }
    else {
        wmin = minh; vwmin = vminh;
        if(minp < secondh) {
            wsecond = minp; vwsecond = vsecondp;
        }
        else {
            wsecond = secondh; vwsecond = vsecondh;
        }
    }
assert(minp <= secondp);
assert(minh <= secondh);
assert(wmin <= wsecond);

    ull gap = wsecond - wmin;
    if(gap == 0) {
        // ui cntWmin = 0;
        // for(ui i = 0; i < Vh.size(); i++) {
        //     if(r[Vh[i]] == wmin) {
        //         cntWmin++;
        //     }
        // }
        // for(ui i = 0; i < Vp.size(); i++) {
        //     if(r[Vp[i]] == wmin) {
        //         cntWmin++;
        //     }
        // }

        // ull w1 = cb(Vp.size(), k - Vh.size());
        // ull w2 = totalLimed-totalAssigned;
        // ull w3 = lim;
        // gap = std::min(w1, w2);
        // gap = std::min(gap, w3);
        // // gap /= cntWmin;
        // if(gap == 0) gap = 1;

        ull realWsecond = -1;
        for(ui i = 0; i < Vh.size(); i++) {
            if(r[Vh[i]] == wmin) continue;
            if(r[Vh[i]] < realWsecond) {
                realWsecond = r[Vh[i]];
            }
        }
        for(ui i = 0; i < Vp.size(); i++) {
            if(r[Vp[i]] == wmin) continue;
            if(r[Vp[i]] < realWsecond) {
                realWsecond = r[Vp[i]];
            }
        }
        gap = realWsecond - wmin;
        if(gap == 0) gap = 1;
    }

    ull ckmin;
    if(minh < minp) {
        ckmin = cb(Vp.size(), k - Vh.size());
    }
    else {
        ckmin = cb(Vp.size()-1, k - Vh.size() - 1);
    }

    if(lim == cb(Vp.size(), k - Vh.size())) {
        ull val = std::min(ckmin, gap);
        val = std::min(val, lim);
val = std::min(val, totalLimed-totalAssigned);
        if(minh < minp) {
            r[vminh] += val;
#ifdef DEBUGWK
if(xxx ==xx) {
printf("2 %u+%u\n", vminh, val);
}
#endif
totalAssigned+=val;
if(totalAssigned == totalLimed) return;
            batchupdate(Vh, Vp, lim - val, r, cb, k, deep+1);
        }
        else {
            r[vminp] += val;
#ifdef DEBUGWK
if(xxx ==xx) {
printf("3 %u+%u\n", vminp, val);
}
#endif
totalAssigned+=val;
if(totalAssigned == totalLimed) return;
            if(ckmin > gap) {
                std::swap(Vp[ivminp], Vp[Vp.size()-1]);
                Vp.pop_back();
                Vh.push_back(vminp);
                
                batchupdate(Vh, Vp, ckmin-gap, r, cb, k, deep+1);
                
                Vh.pop_back();

                batchupdate(Vh, Vp, lim-ckmin, r, cb, k, deep+1);
                Vp.push_back(vminp);
                std::swap(Vp[ivminp], Vp[Vp.size()-1]);
            }
            else if(Vp.size()+Vh.size() >= k+1) {
                std::swap(Vp[ivminp], Vp[Vp.size()-1]);
                Vp.pop_back();
                
                batchupdate(Vh, Vp, 
                    std::min(lim  - ckmin, 
                    (ull)cb((ui)Vp.size(), (ui)(k-Vh.size()))), 
                        r, cb, k, deep+1);

                Vp.push_back(vminp);
                std::swap(Vp[ivminp], Vp[Vp.size()-1]);
            }
        }
    }
    else if(lim < cb(Vp.size(), k - Vh.size())) {
        ull mn = std::min(std::min(ckmin, gap), lim);
mn = std::min(mn, totalLimed-totalAssigned);
        if(minh < minp) {
            r[vminh] += mn;
#ifdef DEBUGWK
if(xxx ==xx) {
printf("4 %u+%u\n", vminh, mn);
}
#endif
totalAssigned += mn;
if(totalAssigned == totalLimed) return;
            batchupdate(Vh, Vp, std::min(ckmin,lim) - mn, r, cb, k, deep+1);
        }
        else {
            r[vminp] += mn;
#ifdef DEBUGWK
if(xxx ==xx){
printf("5 %u+%u\n", vminh, mn);
}
#endif
totalAssigned += mn;
if(totalAssigned == totalLimed) return;
            if(gap == mn) {
                std::swap(Vp[ivminp], Vp[Vp.size()-1]);
                Vp.pop_back();
                Vh.push_back(vminp);
                
                batchupdate(Vh, Vp, std::min(ckmin, lim)-gap, r, cb, k, deep+1);
                
                Vh.pop_back();
                
                if(Vp.size()+Vh.size() >= k+1) {
                    batchupdate(Vh, Vp, 
                        std::min((ull)cb(Vp.size(), (ull)(k-Vh.size())), 
                        lim - ckmin), r, cb, k, deep+1);
                }

                Vp.push_back(vminp);
                std::swap(Vp[ivminp], Vp[Vp.size()-1]);
            }
            else if(Vp.size()+Vh.size() >= k+1) {
                std::swap(Vp[ivminp], Vp[Vp.size()-1]);
                Vp.pop_back();
                
                batchupdate(Vh, Vp, std::min((ull)cb(Vp.size(), (ull)(k-Vh.size())), 
                        lim - ckmin), r, cb, k, deep+1);

                Vp.push_back(vminp);
                std::swap(Vp[ivminp], Vp[Vp.size()-1]);
            }
        }
    }
    // else {
    //     printf("error\n");
    // }
}

void SCT::sctppWk(std::vector<ui> & r) {
    std::vector<ui> Vh, Vp;
    ull lim;

    initShuffle(cntSCTNode);
    
    for(ull ii = 0; ii < cntSCTNode; ii++) {
        ull i = shuffleNumbers[ii];
// printf("        st %u\n", i);fflush(stdout);
        Vh.clear();
        for(ui j = ppSCT[i]; j < phSCT[i]; j++) {
            Vh.push_back(bufferSCT2V[j]);
        }
        Vp.clear();
        for(ui j = phSCT[i]; j < ppSCT[i+1]; j++) {
            Vp.push_back(bufferSCT2V[j]);
        }
        
        ui h = phSCT[i] - ppSCT[i];
        ui p = ppSCT[i+1] - phSCT[i];
        lim = cb(p, k-h);
totalAssigned = 0;
totalLimed = lim;
#ifdef DEBUGWK
xxx = i;
if(xxx ==xx)
printf("        st %u\n", i);fflush(stdout);
#endif

        batchupdate(Vh, Vp, lim, r, cb, k, 0);
if(totalAssigned != lim) {
printf("%llu %llu\n", totalAssigned, lim);
}
assert(totalAssigned == lim);
    }
}

void SCT::scanEnu(ui ed, ui d, ui st, std::function<
            void(const std::vector<ui>::iterator &, 
                const std::vector<ui>::iterator &)> f) {

// printf("d %u %u %u ", d, st, ed);fflush(stdout);
    if(d + 1 == k) {
        for(ui l = st; l < ed; l++) {
// if(d==2 && st == 2 && ed == 3) {
//     printf("l %u ", l);fflush(stdout);
// }
            tmpClique.push_back(bufferSCT2V[l]);
// if(d==2 && st == 2 && ed == 3) {
//     printf("bl %u ", bufferSCT2V[l]);fflush(stdout);
//     for(ui i = 0; i < k; i++) {
//         printf("%u ", tmpClique[i]);
//     }fflush(stdout);
// }
            f(tmpClique.begin(), tmpClique.end());
            tmpClique.pop_back();
        }
        return;
    }

    for(ui l = st; l < ed; l++) {
        if(ed - l + d < k) return;

        tmpClique.push_back(bufferSCT2V[l]);
        scanEnu(ed, d+1, l+1, f);
        tmpClique.pop_back();
    }
}

void SCT::enumerate() {
    phSCT.clear(); ppSCT.clear();
    ppSCT.push_back(0);
    
    cntClique = 0;
    P.clear();
    H.clear();

    for(ui u = 0; u < g->n; u++) {
        ui d = g->pIdx[u+1] - g->pIdx2[u];
        ui sz = 0;
        for(ui i = 0; i < d; i++) {
            ui v = g->pEdge[g->pIdx2[u] + i];
            C.changeTo(v, sz++);
        }

        H.push_back(u);
        pivoter(sz);
        H.pop_back();
    }

    cntSCTNode = phSCT.size();
printf("cntSCTNode:%llu\n", cntSCTNode);
printf("kr:%u\n", kr);
}

void SCT::pivoter(ui sz) {
    if(sz + H.size() + P.size() < k) return;
    if(H.size() == kr - 1) {
        if(kr == k)
            cntClique += P.size() + sz;
        
        for(auto v : H) bufferSCT2V.push_back(v);
        phSCT.push_back(bufferSCT2V.size());
        
        for(ui i = 0; i < P.size(); i++) {
            bufferSCT2V.push_back(P[i]);
        }
        for(ui i = 0; i < sz; i++) {
            bufferSCT2V.push_back(C[i]);
        }
        ppSCT.push_back(bufferSCT2V.size());
        return;
    }
    if(sz == 0) {
        if(kr == k)
            cntClique += cb(P.size(), k - H.size());

        for(auto v : H) bufferSCT2V.push_back(v);
        phSCT.push_back(bufferSCT2V.size());

        for(ui i = 0; i < P.size(); i++) {
            bufferSCT2V.push_back(P[i]);
        }
        ppSCT.push_back(bufferSCT2V.size());
        return;
    }

    ui pivot = C[0], pivotDeg = 0, num = 0;
    for(ui i = 0; i < sz; i++) {
        ui d = 0;
        for(ui j = 0; j < sz; j++) {
            if(g->connect(C[i], C[j])) d++;
        }
        if(d > pivotDeg) {
            pivot = C[i]; pivotDeg = d; num = 1;
        }
        else if(d == pivotDeg) num++;
    }

    C.changeTo(pivot, --sz);

    for(ui i = 0, j = 0; i < sz; i++) {
        if(g->connect(pivot, C[i])) C.changeToByPos(i, j++);
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
            if(g->connect(u, C[j])) C.changeToByPos(j, newSz++);
        }

        H.push_back(u);
        pivoter(newSz);
        H.pop_back();
    }
}

