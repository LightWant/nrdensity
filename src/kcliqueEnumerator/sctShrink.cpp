#include "sct.h"
#include <vector>

bool SCT::shrink(const std::vector<ui> & level, ui sz) {
    ui l = 0;
    std::vector<ui> bf;
    ui preSCTNode = cntSCTNode;

//     ull minimumDegree = -1;
//     ull maxDegree = 0;
//     std::vector<ull> lc(g->n);
//     for(ui i = 0; i < cntSCTNode; i++) {
//         ui pp = ppSCT[i+1]-phSCT[i];
//         ui hh = phSCT[i]-ppSCT[i];
        
//         for(ui j = ppSCT[i]; j < phSCT[i]; j++) {
//             ui u = bufferSCT2V[j];
//             lc[u] += cb(pp, k-hh);
//         }

//         if(pp>=1 && k>=hh+1) 
//         for(ui j = phSCT[i]; j < ppSCT[i+1]; j++) {
//             ui u = bufferSCT2V[j];
//             lc[u] += cb(pp-1, k-hh-1);
//         }
//     }
//     for(ui i = 0; i < g->n; i++) {
//         if(level[i] < sz) {
//             minimumDegree = std::min(minimumDegree, lc[i]);
//         }
//         maxDegree = std::max(maxDegree, lc[i]);
//     }
// printf("minimumDegree1:%llu\n", minimumDegree);
// printf("maxDegree:%llu\n", maxDegree);
// ull cntClique3 = 0;
    for(ui i = 0; i < cntSCTNode; i++) {
        bool f = true;
        for(ui j = ppSCT[i]; j < phSCT[i]; j++) {
            ui u = bufferSCT2V[j];
            if(level[u] >= sz) {
                f = false; break;
            }
        }
        if(!f) continue;

        ui cnt = 0;
        for(ui j = phSCT[i]; j < ppSCT[i+1]; j++) {
            ui u = bufferSCT2V[j];
            if(level[u] < sz) {
                cnt++;
            }
        }

        ui h = phSCT[i]-ppSCT[i];
        if(h+cnt < k) continue;
// cntClique3 += cb(cnt, k-h);

        bf.clear();
        for(ui j = ppSCT[i]; j < phSCT[i]; j++) {
            ui u = bufferSCT2V[j];
            bf.push_back(u);
        }
        if(h < k)
        for(ui j = phSCT[i]; j < ppSCT[i+1]; j++) {
            ui u = bufferSCT2V[j];
            if(level[u] < sz) {
                bf.push_back(u);
            }
        }

        ui preIdx = 0;
        if(l > 0) preIdx = ppSCT[l-1];
        ppSCT[l] = preIdx + bf.size();
        phSCT[l] = preIdx + h;
// assert(preIdx < ppSCT[l]);
// assert(preIdx < phSCT[l]);
// if(phSCT[l] > ppSCT[l]) {
//     printf("%u %u %u %u %u\n", phSCT[l], ppSCT[l], 
//     phSCT[i] - ppSCT[i], phSCT[i], ppSCT[i]);
//     fflush(stdout);
// }
// assert(phSCT[l] <= ppSCT[l]);
        for(ui j = preIdx; j < ppSCT[l]; j++) {
            bufferSCT2V[j] = bf[j - preIdx];
        }
        
        l++;
    }

    for(ui i = l; i > 0; i--) ppSCT[i] = ppSCT[i-1];
    ppSCT[0] = 0;

    cntSCTNode = l;

    ull cntClique2 = 0;
    for(ui i = 0; i < cntSCTNode; i++) {
        ui pp = ppSCT[i+1]-phSCT[i];
        ui hh = phSCT[i]-ppSCT[i];
        cntClique2 += cb(pp, k-hh);
    }
    cntClique = cntClique2;
printf("cntSCTNodeNow:%u\n", cntSCTNode);
printf("cntCliqueNow:%llu\n", cntClique);

//     minimumDegree = -1;
//     maxDegree = 0;
//     // std::vector<ull> lc(g->n);
//     for(ui i = 0; i < g->n; i++) lc[i] = 0;
//     for(ui i = 0; i < cntSCTNode; i++) {
//         ui pp = ppSCT[i+1]-phSCT[i];
//         ui hh = phSCT[i]-ppSCT[i];
        
//         for(ui j = ppSCT[i]; j < phSCT[i]; j++) {
//             ui u = bufferSCT2V[j];
// assert(level[u] < sz);
//             lc[u] += cb(pp, k-hh);
//         }

//         if(pp>=1 && k>=hh+1) 
//         for(ui j = phSCT[i]; j < ppSCT[i+1]; j++) {
//             ui u = bufferSCT2V[j];
// assert(level[u] < sz);
//             lc[u] += cb(pp-1, k-hh-1);
//         }
//     }
//     ui mIdx = 0;
//     for(ui i = 0; i < g->n; i++) {
//         if(level[i] < sz) {
//             minimumDegree = std::min(minimumDegree, lc[i]);
//         }
//         if(lc[i] > maxDegree) {
//             maxDegree = lc[i];
//             mIdx = i;
//         }
//     }
// printf("minimumDegree:%llu\n", minimumDegree);
// printf("maxDegree:%llu %u\n", maxDegree, mIdx);
    return preSCTNode > cntSCTNode;
}