#include "sct.h"
#include <cmath>
#include <random>


void listingAll2(ui l, ui j, std::vector<ui> & bufferSCT2V, ui pp, ui ph,
    ui pp2,    std::vector<ui> & bf,
    cliqueE_kClist * kcl) {
    if(l == 0) {
        for(ui i = pp; i < ph; i++) {
            kcl->addV(bufferSCT2V[i]);
        }
        for(ui u : bf) kcl->addV(u);
        kcl->addC();
        return;
    }

    for(ui i = j; i+l <= pp2; i++) {
        bf.push_back(bufferSCT2V[i]);
        listingAll2(l-1, i+1, bufferSCT2V, pp, ph, pp2, bf, kcl);
        bf.pop_back();
    }
}
// #define DEBUG
void SCT::sampleFromSCT(cliqueE_kClist & kcl, ui cntToSample) {
#ifdef DEBUG
ull tmp = 0;
ull tmp2 = 0;
#endif
    if(cntToSample >= cntClique) {
printf("load all\n");fflush(stdout);
        std::vector<ui> bf(k); 

        for(ui i = 0; i < cntSCTNode; i++) {
            ui h = phSCT[i] - ppSCT[i];
            ui p = ppSCT[i+1] - phSCT[i];

            if(p + h < k) continue;
            if(p + h == k) {
                for(ui j = ppSCT[i]; j < ppSCT[i+1]; j++) {
                    kcl.addV(bufferSCT2V[j]);
                }
                kcl.addC();
                continue;
            }

            bf.clear();
            listingAll2(k-h, ppSCT[i], 
                bufferSCT2V, ppSCT[i], phSCT[i], ppSCT[i+1], bf, &kcl);
        }
        return;
    }

    std::random_device rd;
    std::default_random_engine generator(rd());
    // std::uniform_real_distribution<double> uiDistribution(0, 1);

    kcl.reserve(cntToSample);

    std::vector<ull> weights(100);
    std::vector<ui> times(100);

    ull sumSec = 0;
    for(ull ii = 0, jj = 0; jj < cntSCTNode; jj++) {
// if(jj >= 27086000) {

// printf("%u ", jj);fflush(stdout);
// }
        ui h = phSCT[jj] - ppSCT[jj];
        ui p = ppSCT[jj+1]-phSCT[jj];
        ull cnt = cb(p, k-h);
        sumSec += cnt;
        if(sumSec * cntToSample < cntClique && jj+1 < cntSCTNode) {
            continue;
        }
        ull c = std::round(1.0 * sumSec* cntToSample / cntClique);
        if(c == 0) continue;
#ifdef DEBUG
tmp2 += c;
#endif
        //sasmpling sumSec cliques from [ii-jj]
// if(ii >= 230010344)
// printf("%u-%u:", ii, jj);fflush(stdout);
#ifdef DEBUG
ull tmpp = 0;
ull tmpp2 = 0;
#endif
        if(weights.size() < jj-ii+1) weights.resize(jj-ii+1);
        for(ui i = ii; i <= jj; i++) {
            ui h = phSCT[i] - ppSCT[i];
            ui p = ppSCT[i+1]-phSCT[i];
            ull cnt = cb(p, k-h);
            weights[i-ii] = cnt;
        }
        std::discrete_distribution<ui> uiDistribution(weights.begin(), weights.end());
        if(times.size() < jj-ii+1) times.resize(jj-ii+1);
        for(ull i = ii; i <= jj; i++) times[i-ii] = 0;
        for(ull a = 0; a < c; a++) times[uiDistribution(generator)]++;
        for(ull i = ii; i <= jj; i++) {
            ui h = phSCT[i] - ppSCT[i];
            ui p = ppSCT[i+1]-phSCT[i];
            ull cnt = cb(p, k-h);
            // ull c2 = std::round(1.0 * cnt * c / sumSec);
            
            ull c2 = times[i-ii];
            if(c2 == 0) continue;
#ifdef DEBUG
tmp += c2;
tmpp2 += c2;
#endif
            double gap = 1.0 * cnt / c2;//gap >= 1
            for(double cliqueId = 0; cliqueId+1e-5 < cnt; cliqueId += gap) {
                for(ui j = ppSCT[i]; j < phSCT[i]; j++) {
                    kcl.addV(bufferSCT2V[j]);
                }

                ull nowId = 0;
                ui choosed = 0;
                ull cId = std::round(cliqueId);
                if(cId == cnt) cId--;
// printf("%llu:", cId);
                for(ui j = 0; j < p; j++) {
                    //choose j: cb(p-j-1, k-h-choosed-1)
                    //not choose j: cb(p-j-1, k-h-choosed)
                    if(nowId + cb(p-j-1, k-h-choosed-1) > cId) {

                        choosed++;
                        kcl.addV(bufferSCT2V[phSCT[i]+j]);
                        if(choosed + h == k) break;
                    }
                    else {
                        nowId += cb(p-j-1, k-h-choosed-1);
                    }
// printf("%u-%u ", j, nowId);
                }
// printf("\n");
// if(choosed+h != k) {
//     printf("choosed:%u, h %u, p %u, cId %llu, %f, %llu\n", 
//         choosed, h, p, cId, cliqueId, cnt);
// }
// assert(choosed+h == k);
                kcl.addC();
#ifdef DEBUG
tmpp++;
#endif
                c2--;
                if(c2 == 0) break;
            }
        }

// #ifdef DEBUG
// if(tmpp < c) {
//    printf("%llu-%llu-%llu ", tmpp, tmpp2, c);fflush(stdout); 
// }
// #endif
        ii = jj+1;
        sumSec = 0;
    }
// printf("here\n");fflush(stdout);
#ifdef DEBUG
printf("tmp:%llu\n", tmp);
printf("tmp2:%llu\n", tmp2);
#endif
}

double SCT::realDensityInSg(std::vector<ui> & nodes) {
    std::vector<bool> isInG(g->n);
    for(ui u : nodes) isInG[u] = true;
    double cnt = 0;
    for(ui i = 0; i < cntSCTNode; i++) {
        bool f = true;
        for(ui j = ppSCT[i]; j < phSCT[i]; j++) {
            if(!isInG[bufferSCT2V[j]]) {
                f = false; break;
            }
        }
        if(!f) continue;

        ui h = phSCT[i] - ppSCT[i];
        ui p = 0;
        for(ui j = phSCT[i]; j < ppSCT[i+1]; j++) {
            if(isInG[bufferSCT2V[j]]) p++;
        }

        if(p >= k-h)
            cnt += cb(p, k-h);
    }
printf("cliquesInAppSg:%.0f\n", cnt);
    return cnt / nodes.size();
}