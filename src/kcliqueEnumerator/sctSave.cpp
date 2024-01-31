#include "sct.h"
#include <cassert>

void SCT::saveAsIndex(const std::string & path) {
    // ui cntSCTNode = 0;
    // std::vector<ui> bufferSCT2V;
    // std::vector<ui> phSCT, ppSCT;
    std::string fE = path + "sctEdge.idx";
    FILE *fpEdge = fopen(fE.c_str(), "wb");
assert(ppSCT[cntSCTNode] == bufferSCT2V.size());
    fwrite(bufferSCT2V.data(), 4, ppSCT[cntSCTNode], fpEdge);
    fclose(fpEdge);

    std::string fIpp = path + "sctIdxpp.idx";
    FILE *fpIdxpp = fopen(fIpp.c_str(), "wb");
    fwrite(ppSCT.data(), 4, cntSCTNode+1, fpIdxpp);
    fclose(fpIdxpp);

    std::string fIph = path + "sctIdxph.idx";
    FILE *fpIdxph = fopen(fIph.c_str(), "wb");
    fwrite(phSCT.data(), 4, cntSCTNode, fpIdxph);
    fclose(fpIdxph);

    std::string fS = path + "sctS.txt";
    FILE *fpS = fopen(fS.c_str(), "w");
    fprintf(fpS, "%u\n", cntSCTNode);
    fclose(fpS);
}

// void SCT::loadFromIndex(const std::string & path, ui k) {
//     std::string fS = path + "sctS.txt";
//     FILE *fpS = fopen(fS.c_str(), "r");
//     fscanf(fpS, "%u", &cntSCTNode);
//     fclose(fpS);

//     std::string fIph = path + "sctIdxph.idx";
//     FILE *fpIdxph = fopen(fIph.c_str(), "rb");
//     phSCT.resize(cntSCTNode);
//     fread(phSCT.data(), 4, phSCT.size(), fpIdxph);
//     fclose(fpIdxph);

//     std::string fIpp = path + "sctIdxpp.idx";
//     FILE *fpIdxpp = fopen(fIpp.c_str(), "rb");
//     ppSCT.resize(cntSCTNode+1);
//     fread(ppSCT.data(), 4, ppSCT.size(), fpIdxpp);
//     fclose(fpIdxpp);

//     std::string fE = path + "sctEdge.idx";
//     FILE *fpEdge = fopen(fE.c_str(), "rb");
//     bufferSCT2V.resize(ppSCT[cntSCTNode]);
//     fread(bufferSCT2V.data(), 4, ppSCT[cntSCTNode], fpEdge);
//     fclose(fpEdge);

//     ui l = 0; 
//     cntClique = 0;
//     ui nxtPP = ppSCT[0];
//     for(ui i = 0; i < cntSCTNode; i++) {
//         ui pp = ppSCT[i+1]-phSCT[i];
//         ui hh = phSCT[i]-nxtPP;
// // printf("%u %u\n", pp, hh);fflush(stdout);
// // if(l>0 && ppSCT[68055+1]- phSCT[68055] >= 30000 ) {
// //     printf("%u aa %u %u\n", i, ppSCT[68055+1], phSCT[68055]);fflush(stdout);
// // }
//         if(pp+hh < k || hh > k) {
//             nxtPP = ppSCT[i+1];
//             continue;
//         }
//         else if(hh == k) {
//             cntClique += 1;
// //             if(l == i) {
// //                 nxtPP = ppSCT[i+1];
// //                 ppSCT[l+1] = ppSCT[l]+k;
// //                 phSCT[l] = ppSCT[l] + k;
// // if( ppSCT[l+1]- phSCT[l] >= 30000 || phSCT[l]-ppSCT[l] > k) {
// //     printf("%u a %u %u %u\n", i, ppSCT[l], phSCT[l], ppSCT[l+1]);fflush(stdout);
// // }
// //                 l++;
// //                 continue;
// //             }
//             ui nxt = ppSCT[l];
//             for(ui j = nxtPP; j < phSCT[i]; j++) {
//                 bufferSCT2V[nxt++] = bufferSCT2V[j];
//             }
//             phSCT[l] = ppSCT[l] + k;
//             nxtPP = ppSCT[i+1];
//             ppSCT[l+1] = nxt;
// // assert(nxt == ppSCT[l] + k);
// // if( ppSCT[l+1]- phSCT[l] >= 30000 || phSCT[l]-ppSCT[l] > k) {
// //     printf("%u x %u %u %u\n", i, ppSCT[l], phSCT[l], ppSCT[l+1]);fflush(stdout);
// // }
//             l++;
//         }
//         else {
//             cntClique += cb(pp, k-hh);
            
//             ui nxt = ppSCT[l];
//             for(ui j = nxtPP; j < ppSCT[i+1]; j++) {
//                 bufferSCT2V[nxt++] = bufferSCT2V[j];
//             }
//             phSCT[l] = ppSCT[l] + hh;
//             nxtPP = ppSCT[i+1];
//             ppSCT[l+1] = nxt;
// // if( ppSCT[l+1]- phSCT[l] >= 30000 || phSCT[l]-ppSCT[l] > k) {
// //     printf("%u s %u %u %u\n", i, ppSCT[l], phSCT[l], ppSCT[l+1]);fflush(stdout);
// // }
//             l++;
//         }
//     }
//     cntSCTNode = l;
// printf("cntSCTNode:%u\n", cntSCTNode);
// // ull cntClique2 = 0;
// // for(ui i = 0; i < cntSCTNode; i++) {
// //     ui pp = ppSCT[i+1]-phSCT[i];
// //     ui hh = phSCT[i]-ppSCT[i];
// // if(pp >= 30000 || k-hh >= 30) {
// //     printf("%u %u %u\n", pp, hh, i);fflush(stdout);
// // }
// //     cntClique2 += cb(pp, k-hh);
// // }
// // assert(cntClique2 == cntClique);

// }

void SCT::loadFromIndex(const std::string & path, ui k) {
    std::string fS = path + "sctS.txt";
    FILE *fpS = fopen(fS.c_str(), "r");
    fscanf(fpS, "%llu", &cntSCTNode);
    fclose(fpS);

    std::string fIph = path + "sctIdxph.idx";
    FILE *fpIdxph = fopen(fIph.c_str(), "rb");
    // phSCT.resize(cntSCTNode);
    // fread(phSCT.data(), 4, phSCT.size(), fpIdxph);
    // fclose(fpIdxph);
    std::vector<ui> tmpphSCT(cntSCTNode);
    fread(tmpphSCT.data(), 4, tmpphSCT.size(), fpIdxph);
    fclose(fpIdxph);

    std::string fIpp = path + "sctIdxpp.idx";
    FILE *fpIdxpp = fopen(fIpp.c_str(), "rb");
    // ppSCT.resize(cntSCTNode+1);
    // fread(ppSCT.data(), 4, ppSCT.size(), fpIdxpp);
    // fclose(fpIdxpp);
    std::vector<ui> tmpppSCT(cntSCTNode+1);
    fread(tmpppSCT.data(), 4, tmpppSCT.size(), fpIdxpp);
    fclose(fpIdxpp);

    std::string fE = path + "sctEdge.idx";
    FILE *fpEdge = fopen(fE.c_str(), "rb");
    bufferSCT2V.resize(5000000);
    bufferSCT2V.clear();
    // fread(bufferSCT2V.data(), 4, ppSCT[cntSCTNode], fpEdge);
    // fclose(fpEdge);
    std::vector<ui> bf(g->coreNumber + 5);

    ppSCT.push_back(0);
    cntClique = 0;
    ui st = 0;
    for(ull i = 0; i < cntSCTNode; i++) {
        ui pp = tmpppSCT[i+1]-tmpphSCT[i];
        ui hh = tmpphSCT[i]-tmpppSCT[i];

        fread(bf.data(), 4, pp+hh, fpEdge);

        if(pp+hh < k || hh > k) {
            continue;
        }
        
        if(hh == k) {
            cntClique += 1;

            for(ui j = 0; j < hh; j++) {
                bufferSCT2V.push_back(bf[j]);
            }
            phSCT.push_back(st + hh);
            ppSCT.push_back(st + hh);
            st += hh;
        }
        else {
            cntClique += cb(pp, k-hh);

            for(ui j = 0; j < pp+hh; j++) {
                bufferSCT2V.push_back(bf[j]);
            }
            phSCT.push_back(st + hh);
            ppSCT.push_back(st + hh+pp);
            st += hh+pp;
        }
    }
    fclose(fpEdge);
    cntSCTNode = phSCT.size();
printf("cntSCTNode:%u\n", cntSCTNode);fflush(stdout);

    
    std::vector<ui>().swap(bf);
    std::vector<ui>().swap(tmpphSCT);
    std::vector<ui>().swap(tmpppSCT);

// ull cntClique2 = 0;
// for(ui i = 0; i < cntSCTNode; i++) {
//     ui pp = ppSCT[i+1]-phSCT[i];
//     ui hh = phSCT[i]-ppSCT[i];
// // if(pp >= 30000 || k-hh >= 30) {
// //     printf("%u %u %u\n", pp, hh, i);fflush(stdout);
// // }
//     cntClique2 += cb(pp, k-hh);
// }
// assert(cntClique2 == cntClique);

printf("cliqueCnt:%llu\n", cntClique); fflush(stdout);
}