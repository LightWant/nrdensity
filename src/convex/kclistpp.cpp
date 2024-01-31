#include "kclistpp.h"
#include <functional>
#include <algorithm>
#include <iostream>
#include "../tools/HLPP.hpp"
#include "../kcliqueEnumerator/sct.h"
#include "../tools/comb.hpp"
// #define DEBUG

// #define DDEBUG

// kclistpp::kclistpp(Graph & g, cliqueE_kClist & e, ui k) 
//     : g(g), enumerator(e), r(g.n), rPava(g.n), level(g.n), k(k)
// {}
kclistpp::kclistpp(Graph & g, enumerator * e, ui k) 
    : g(g), etr(e), r(g.n), rPava(g.n), level(g.n), reordered(g.n), k(k)
{}

// void kclistpp::runSCTPPFlow(ui t) {
//     T += t;
//     auto f = [](ui * sct,
//         ui n, 
//         ui upper, //total count of k-cliques
//         ui *alpha, 
//         std::vector<ui> *V_, 
//         std::vector<ui> * localCnt_,
//         std::vector<ui> * r_ ) {
        
//         std::vector<ui> & r = (*r_);
//         std::vector<ui> & V = (*V_);

//         V.clear();
//         std::vector<ui> & localCnt = (*localCnt_);
//  //sort V 
//         for(ui i = 0; i < n; i++) V.push_back(i);
//         std::sort(V.begin(), V.end(), [&](ui a, ui b) {
//             // if(r[sct[a]] == (r[sct[b]]))
//             //     return localCnt[sct[a]] < localCnt[sct[b]];
//             return r[sct[a]] < r[sct[b]];
//         });

// //the while loop
//         ui st = 0;
//         ui i = 1;
//         while(i < n && r[sct[V[i]]] == r[sct[V[i-1]]]) {
//             i++;
//         }
//         ui totalCnt = 0;//the count of cliques assigned
//         ui gap = i<n ? r[sct[V[i]]]-r[sct[V[i-1]]] : UINT32_MAX;

// #ifdef DEBUG
// printf("st %u, i %u, gap %u, n %u\n", st, i, gap, n);fflush(stdout);
// #endif
//         while(totalCnt < upper) {
// #ifdef DEBUG
// printf("\nV:");
// for(ui v : V) printf("%u ", v); printf("\n");
// printf("R:");
// for(ui v : V) printf("%u ", r[v]); printf("\n");
// printf("L:");
// for(ui v : V) printf("%u ", localCnt[v]); printf("\n");
// printf("upper:%u\n", upper);
// printf("totalCnt:%u\n", totalCnt);
// printf("st %u, i %u, gap %u\n", st, i, gap);fflush(stdout);
// #endif
// // printf("totalCnt %llu\n", totalCnt);fflush(stdout);
// // printf("totalCnt %u\n", totalCnt);fflush(stdout);       
//             do {
//                 ui minV = std::min(gap, upper - totalCnt);
//                 for(ui j = st; j < i; j++) {
//                     minV = std::min(minV, localCnt[V[j]]);
//                 }

// #ifdef DEBUG
// printf("minV %u (i-st) %u upper-totalCnt %u\n", minV, i-st, upper - totalCnt);
// #endif
//                 if(minV*(i-st) <= upper - totalCnt) {
//                     for(ui j = st; j < i; j++) {
//                         r[sct[V[j]]] += minV;
//                         localCnt[V[j]] -= minV;
//                         totalCnt += minV;
//                         alpha[V[j]] += minV;
// if(sct[V[j]] == 1025070) {
//     printf("t0 %u %u %u %x\n", r[sct[V[j]]], alpha[V[j]], V[j], &alpha[V[j]]);
// }
// // assert(r[sct[V[j]]] >= alpha[V[j]]);
// #ifdef DEBUG
// printf("add1 %u %u\n", V[j], minV);
// #endif
//                         if(localCnt[V[j]] == 0) {
//                             std::swap(V[st], V[j]);
//                             st++;
//                         }
//                     }
// // assert(totalCnt <= upper);
//                 }
//                 else {
//                     ui v = (upper - totalCnt) / (i - st);//v <= minV
// // assert(v <= minV);
// // assert(v*(i-st) <= upper - totalCnt);
//                     ui vmod = (upper - totalCnt) % (i - st);
// // assert(st + vmod < i);
// // assert((v+1)*vmod+totalCnt <= upper);
// // assert(totalCnt <= upper);
// // printf("%u %u v%u, vmod%u, i-st-vmod %u\n", totalCnt, upper, v, vmod, i-st-vmod);
//                     for(ui j = st, ed = st + vmod; j < ed; j++) {
//                         r[sct[V[j]]] += v+1;
//                         localCnt[V[j]] -= v+1;
//                         totalCnt += v+1;
//                         alpha[V[j]] += v+1;
// if(sct[V[j]] == 1025070) {
//     printf("t1 %u %u %u %x\n", r[sct[V[j]]], alpha[V[j]], V[j], &alpha[V[j]]);
// }
// // assert(r[sct[V[j]]] >= alpha[V[j]]);
// #ifdef DEBUG
// printf("add2 %u %u, vmod %u, j%u, st + vmod%u\n", V[j], v+1, vmod, j, st + vmod);
// #endif
//                         if(localCnt[V[j]] == 0) {
//                             std::swap(V[st], V[j]);
//                             ++st;
//                         }
//                     }
// // printf("%u\n", totalCnt);fflush(stdout);
// // assert(totalCnt <= upper);
//                     for(ui j = st + vmod; j < i; j++) {
//                         r[sct[V[j]]] += v;
//                         localCnt[V[j]] -= v;
//                         totalCnt += v;
//                         alpha[V[j]] += v;
// if(sct[V[j]] == 1025070) {
//     printf("t2 %u %u %u %x\n", r[sct[V[j]]], alpha[V[j]], V[j], &alpha[V[j]]);
// }
// // assert(r[sct[V[j]]] >= alpha[V[j]]);
// #ifdef DEBUG
// printf("add3 %u %u\n", V[j], v);
// #endif
//                         if(localCnt[V[j]] == 0) {
//                             std::swap(V[st++], V[j]);
//                         }
//                     }
// // assert(totalCnt <= upper);
//                 }
//             } while(totalCnt < upper && r[sct[V[i-1]]] < r[sct[V[i]]] && st < i);
// #ifdef DEBUG
// printf("totalCnt2 %u ", totalCnt);fflush(stdout);
// #endif
//             ui iNew = std::min(i+1, n);
//             while(iNew < n && r[sct[V[iNew]]] == r[sct[V[iNew-1]]]) {
//                 iNew++;
//             }
//             ui gapNew = iNew<n ? r[sct[V[iNew]]]-r[sct[V[iNew-1]]] : UINT32_MAX;
// #ifdef DEBUG
// printf("totalCnt3 %u ", totalCnt);fflush(stdout);
// #endif
//             ui stNew = st;
//             while(stNew < iNew && localCnt[V[stNew]] == 0) {
//                 stNew++;
//             }

//             i= iNew;
//             st = stNew;
//             gap = gapNew;
//         }

//     };
//     using namespace std::placeholders;
//     auto f2 = std::bind(f, _1, _2, _3, _4, _5, _6, &r);
// // g.print();
//     for(int i = 0; i < t; i++) etr->scanSCTFlow(f2);
// }

void kclistpp::runSCTPPFlow(ui t) {
    T += t;

    auto f = [](std::vector<ui> * sct_,
        ui pp, ui n,
        ull upper, //total count of k-cliques
        ui *alpha, 
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
        ull totalCnt = 0;//the count of cliques assigned
        ull gap = i<n ? r[sct[V[i]]]-r[sct[V[i-1]]] : UINT32_MAX;

#ifdef DEBUG
printf("st %u, i %u, gap %u, n %u\n", st, i, gap, n);fflush(stdout);
#endif
        while(totalCnt < upper) {
#ifdef DEBUG
printf("\nV:");
for(ui v : V) printf("%u ", v); printf("\n");
printf("R:");
for(ui v : V) printf("%u ", r[v]); printf("\n");
printf("L:");
for(ui v : V) printf("%u ", localCnt[v]); printf("\n");
printf("upper:%llu\n", upper);
printf("totalCnt:%llu\n", totalCnt);
printf("st %u, i %u, gap %u\n", st, i, gap);fflush(stdout);
#endif
// printf("totalCnt %llu\n", totalCnt);fflush(stdout);
// printf("totalCnt %u\n", totalCnt);fflush(stdout);       
            do {
                ull minV = std::min(gap, upper - totalCnt);
                for(ui j = st; j < i; j++) {
                    minV = std::min(minV, (ull)localCnt[V[j]-pp]);
                }

#ifdef DEBUG
printf("minV %u (i-st) %u upper-totalCnt %llu\n", minV, i-st, upper - totalCnt);
#endif
                if(minV*(i-st) <= upper - totalCnt) {
                    for(ui j = st; j < i; j++) {
                        r[sct[V[j]]] += minV;
                        localCnt[V[j]-pp] -= minV;
                        totalCnt += minV;
                        alpha[V[j]] += minV;
// if(pp == 0) {
//     printf("t0 %u %u %u %u\n", r[sct[V[j]]], alpha[V[j]], V[j], sct[V[j]]);
// }
// if(V[j] == 1) {
//     printf("t00 %u %u %u %u\n", r[sct[V[j]]], alpha[V[j]], V[j], sct[V[j]]);
// }
// assert(r[sct[V[j]]] >= alpha[V[j]]);
#ifdef DEBUG
printf("add1 %u %u\n", V[j], minV);
#endif
                        if(localCnt[V[j]-pp] == 0) {
                            std::swap(V[st], V[j]);
                            st++;
                        }
                    }
// assert(totalCnt <= upper);
                }
                else {
                    ui v = (upper - totalCnt) / (i - st);//v <= minV
// assert(v <= minV);
// assert(v*(i-st) <= upper - totalCnt);
                    ui vmod = (upper - totalCnt) % (i - st);
// assert(st + vmod < i);
// assert((v+1)*vmod+totalCnt <= upper);
// assert(totalCnt <= upper);
// printf("%u %u v%u, vmod%u, i-st-vmod %u\n", totalCnt, upper, v, vmod, i-st-vmod);
                    for(ui j = st, ed = st + vmod; j < ed; j++) {
                        r[sct[V[j]]] += v+1;
                        localCnt[V[j]-pp] -= v+1;
                        totalCnt += v+1;
                        alpha[V[j]] += v+1;
// if(pp == 0) {
//     printf("t1 %u %u %u %u\n", r[sct[V[j]]], alpha[V[j]], V[j], sct[V[j]]);
// }
// if(V[j] == 1) {
//     printf("t11 %u %u %u %u\n", r[sct[V[j]]], alpha[V[j]], V[j], sct[V[j]]);
// }
// assert(r[sct[V[j]]] >= alpha[V[j]]);
#ifdef DEBUG
printf("add2 %u %u, vmod %u, j%u, st + vmod%u\n", V[j], v+1, vmod, j, st + vmod);
#endif
                        if(localCnt[V[j]-pp] == 0) {
                            std::swap(V[st], V[j]);
                            ++st;
                        }
                    }
// printf("%u\n", totalCnt);fflush(stdout);
// assert(totalCnt <= upper);
                    for(ui j = st + vmod; j < i; j++) {
                        r[sct[V[j]]] += v;
                        localCnt[V[j]-pp] -= v;
                        totalCnt += v;
                        alpha[V[j]] += v;
// if(pp == 0) {
//     printf("t2 %u %u %u %u\n", r[sct[V[j]]], alpha[V[j]], V[j], sct[V[j]]);
// }
// if(V[j] == 1) {
//     printf("t22 %u %u %u %u\n", r[sct[V[j]]], alpha[V[j]], V[j], sct[V[j]]);
// }
// assert(r[sct[V[j]]] >= alpha[V[j]]);
#ifdef DEBUG
printf("add3 %u %u\n", V[j], v);
#endif
                        if(localCnt[V[j]-pp] == 0) {
                            std::swap(V[st++], V[j]);
                        }
                    }
// assert(totalCnt <= upper);
                }
            } while(totalCnt < upper && r[sct[V[i-1]]] < r[sct[V[i]]] && st < i);

            ui iNew = std::min(i+1, n);
            while(iNew < n && r[sct[V[iNew]]] == r[sct[V[iNew-1]]]) {
                iNew++;
            }
            ui gapNew = iNew<n ? r[sct[V[iNew]]]-r[sct[V[iNew-1]]] : UINT32_MAX;
#ifdef DEBUG
printf("totalCnt3 %llu ", totalCnt);fflush(stdout);
#endif
            ui stNew = st;
            while(stNew < iNew && localCnt[V[stNew]-pp] == 0) {
                stNew++;
            }

            i= iNew;
            st = stNew;
            gap = gapNew;
        }

// assert(totalCnt == upper);

    };
    using namespace std::placeholders;
// if(r.size() == 0) {
//     printf("error r size kclistpp\n");
//     exit(0);
// }
    auto f2 = std::bind(f, _1, _2, _3, _4, _5, _6, _7, &r);
// g.print();

    for(int i = 0; i < t; i++) etr->scanSCTFlow(f2);
}


// #define DEBUG
// void kclistpp::runSCTPP(int t) {
//     T += t;
//     auto f = [](const itr & hbg, const itr & hed, //the h nodes
//         const itr & pbg, const itr & ped, //the p nodes
//         ui upper, //total count of k-cliques
//         std::vector<ui> *V_, 
//         std::vector<ui> * localCnt_,
//         std::vector<ui> * r_, ui k) {

//         std::vector<ui> & r = (*r_);
//         std::vector<ui> & V = (*V_);
//         V.clear();
//         std::vector<ui> & localCnt = (*localCnt_);
//         ui n = ped - pbg + hed - hbg;
//   //sort V 
//         if(n == k) {
//             itr st = hbg;
//             for(itr ii = hbg+1; ii != ped; ii++) {
//                 if(r[*ii] < r[*st]) {
//                     st = ii;
//                 }
//             }
//             r[*st]++;
//             return;
//         }

//         std::sort(hbg, hed, [&](ui a, ui b) {
//             return r[a] < r[b];
//         });
//         std::sort(pbg, ped, [&](ui a, ui b) {
//             return r[a] < r[b];
//         });

//         itr pp = pbg, ph = hbg;
//         while(pp < ped || ph < hed) {
//             while(ph < hed && r[*ph] <= r[*pp]) V.push_back(*ph++);
//             if(ph == hed) {
//                 while(pp < ped) V.push_back(*pp++);
//                 break;
//             }
//             while(pp < ped && r[*pp] <= r[*ph]) V.push_back(*pp++);
//             if(pp == ped) {
//                 while(ph < hed) V.push_back(*ph++);
//                 break;
//             }
//         }
//         // for(itr ii = hbg; ii != ped; ii++) {
//         //     V.push_back(*ii);
//         // }
//         // std::sort(V.begin(), V.end(), [&](ui a, ui b) {
//         //     return r[a] < r[b];
//         // });

// //the while loop
//         ui st = 0;
//         ui i = 1;
//         while(i < n && r[V[i]] == r[V[i-1]]) {
//             i++;
//         }
//         ui totalCnt = 0;//the count of cliques assigned
//         ui gap = i<n ? r[V[i]]-r[V[i-1]] : UINT32_MAX;
// #ifdef DEBUG
// printf("        new\n");
// for(itr ii = hbg; ii != hed; ii++) {
//     printf("%u ", *ii);
// }
// printf("/ ");
// for(itr ii = pbg; ii != ped; ii++) {
//     printf("%u ", *ii);
// }
// printf("\n");
// #endif

// #ifdef DDEBUG
// printf("st %u, i %u, gap %u, n %u\n", st, i, gap, n);fflush(stdout);
// #endif

//         while(totalCnt < upper) {
// #ifdef DEBUG
// printf("\nV:"); 
// for(ui v : V) printf("%u ", v); printf("\n");
// printf("R:");
// for(ui v : V) printf("%u ", r[v]); printf("\n");
// printf("L:");
// for(ui v : V) printf("%u ", localCnt[v]); printf("\n");
// printf("upper:%u\n", upper);
// printf("totalCnt:%u\n", totalCnt);
// printf("st %u, i %u, gap %u\n", st, i, gap);fflush(stdout);
// #endif
// // printf("totalCnt %llu\n", totalCnt);fflush(stdout);
// // printf("totalCnt %u\n", totalCnt);fflush(stdout);       
//             do {
//                 ui minV = std::min(gap, upper - totalCnt);
//                 for(ui j = st; j < i; j++) {
//                     minV = std::min(minV, localCnt[V[j]]);
//                 }

// #ifdef DEBUG
// printf("minV %u (i-st) %u upper-totalCnt %u\n", minV, i-st, upper - totalCnt);
// #endif
//                 if(minV*(i-st) <= upper - totalCnt) {
//                     for(ui j = st; j < i; j++) {
//                         r[V[j]] += minV;
//                         localCnt[V[j]] -= minV;
//                         totalCnt += minV;
// // assert(localCnt[V[j]] >= 0 && localCnt[V[j]] <= upper);
// #ifdef DEBUG
// printf("add1 %u %u\n", V[j], minV);
// #endif

//                         if(localCnt[V[j]] == 0) {
//                             std::swap(V[st], V[j]);
//                             st++;
//                         }
//                     }
// // assert(totalCnt <= upper);
//                 }
//                 else {
//                     ui v = (upper - totalCnt) / (i - st);//v <= minV
// // assert(v <= minV);
// // assert(v*(i-st) <= upper - totalCnt);
//                     ui vmod = (upper - totalCnt) % (i - st);
// // assert(st + vmod < i);
// // assert((v+1)*vmod+totalCnt <= upper);
// // assert(totalCnt <= upper);
// // printf("%u %u v%u, vmod%u, i-st-vmod %u\n", totalCnt, upper, v, vmod, i-st-vmod);
//                     for(ui j = st, ed = st + vmod; j < ed; j++) {
//                         r[V[j]] += v+1;
//                         localCnt[V[j]] -= v+1;
//                         totalCnt += v+1;
// // assert(localCnt[V[j]] >= 0 && localCnt[V[j]] <= upper);
// #ifdef DEBUG
// printf("add2 %u %u, vmod %u, j%u, st + vmod%u\n", V[j], v+1, vmod, j, st + vmod);
// #endif
//                         if(localCnt[V[j]] == 0) {
//                             std::swap(V[st], V[j]);
//                             ++st;
//                         }
//                     }
// // printf("%u\n", totalCnt);fflush(stdout);
// // assert(totalCnt <= upper);
//                     for(ui j = st + vmod; j < i; j++) {
//                         r[V[j]] += v;
//                         localCnt[V[j]] -= v;
//                         totalCnt += v;
// // assert(localCnt[V[j]] >= 0 && localCnt[V[j]] <= upper);
// #ifdef DEBUG
// printf("add3 %u %u\n", V[j], v);
// #endif
//                         if(localCnt[V[j]] == 0) {
//                             std::swap(V[st++], V[j]);
//                         }
//                     }
// // assert(totalCnt <= upper);
//                 }
//             } while(totalCnt < upper && r[V[i-1]] < r[V[i]] && st < i);

// #ifdef DEBUG
// printf("totalCnt2 %u ", totalCnt);fflush(stdout);
// #endif
//             ui iNew = std::min(i+1, n);
//             while(iNew < n && r[V[iNew]] == r[V[iNew-1]]) {
//                 iNew++;
//             }
//             ui gapNew = iNew<n ? r[V[iNew]]-r[V[iNew-1]] : UINT32_MAX;
// #ifdef DEBUG
// printf("totalCnt3 %u ", totalCnt);fflush(stdout);
// #endif

//             ui stNew = st;
//             while(stNew < iNew && localCnt[V[stNew]] == 0) {
//                 stNew++;
//             }
// #ifdef DEBUG
// printf("totalCnt4 %u %u \n", totalCnt, upper);fflush(stdout);
// #endif
//             i= iNew;
//             st = stNew;
//             gap = gapNew;
//         }
// // assert(totalCnt == upper);

//     };
//     using namespace std::placeholders;
//     auto f2 = std::bind(f, _1, _2, _3, _4, _5, _6, _7, &r, k);

// // g.print();
//     for(int i = 0; i < t; i++) {
//         // etr->sortSCTNodesByMinR(r); //sort SCT nodes
//         etr->scanSCT(f2);
//     }
// }

void kclistpp::runSCTPP(int t) {
    T += t;
    auto f = [](const itr & hbg, const itr & hed, //the h nodes
        const itr & pbg, const itr & ped, //the p nodes
        ull upper, //total count of k-cliques
        std::vector<ui> *V_, 
        std::vector<bool> & isP, comb & cb,
        std::vector<ui> * localCnt_,
        std::vector<ui> * r_, ui k) {

        std::vector<ui> & r = (*r_);
        std::vector<ui> & V = (*V_);
        V.clear();
        std::vector<ui> & localCnt = (*localCnt_);
        ui n = ped - pbg + hed - hbg;
        ui pCnt = ped - pbg;
  //sort V 
        if(n == k) {
            itr st = hbg;
            for(itr ii = hbg+1; ii != ped; ii++) {
                if(r[*ii] < r[*st]) {
                    st = ii;
                }
            }
            r[*st]++;
            return;
        }
        // if(n < 10) {
        //     ull assigned = 0;

        //     while(assigned < upper) {
        //         ui rmin = -1;
        //         ui rminCnt = 0;
        //         for(itr i = hbg; i < ped; i++) {
        //             if(r[*i] < rmin) {
        //                 rmin = r[*i];
        //                 rminCnt = 1;
        //             }
        //             else if(r[*i] == rmin) {
        //                 rminCnt++;
        //             }
        //         }

        //         ui rsecond = -1;
        //         for(itr i = hbg; i < ped; i++) {
        //             if(r[*i] > rmin && r[*i] < rsecond) {
        //                 rsecond = r[*i];
        //             }
        //         }

        //         if(rsecond!=-1 && (rsecond-rmin)*rminCnt <= upper - assigned) {
        //             for(itr i = hbg; i < ped; i++) {
        //                 if(r[*i] == rmin) {
        //                     r[*i] += rsecond - rmin;
        //                 }
        //             }
        //             assigned += (rsecond-rmin)*rminCnt;
        //         }
        //         else {
        //             ui val1 = (upper - assigned) / rminCnt;
        //             ui val2 = (upper - assigned) % rminCnt;
        //             for(itr i = hbg; i < ped; i++) {
        //                 if(r[*i] == rmin) {
        //                     r[*i] += val1;
        //                     if(val2--) r[*i]++;
        //                 }
        //             }
        //             assigned = upper;
        //         }
        //     }
            

        //     return;
        // }

        std::sort(hbg, hed, [&](ui a, ui b) {
            return r[a] < r[b];
        });
        std::sort(pbg, ped, [&](ui a, ui b) {
            return r[a] < r[b];
        });

        itr pp = pbg, ph = hbg;
        while(pp < ped || ph < hed) {
            while(ph < hed && r[*ph] <= r[*pp]) V.push_back(*ph++);
            if(ph == hed) {
                while(pp < ped) V.push_back(*pp++);
                break;
            }
            while(pp < ped && r[*pp] <= r[*ph]) V.push_back(*pp++);
            if(pp == ped) {
                while(ph < hed) V.push_back(*ph++);
                break;
            }
        }
        // for(itr ii = hbg; ii != ped; ii++) {
        //     V.push_back(*ii);
        // }
        // std::sort(V.begin(), V.end(), [&](ui a, ui b) {
        //     return r[a] < r[b];
        // });

//the while loop
        ui st = 0;
        ui i = 1;
        while(i < n && r[V[i]] == r[V[i-1]]) {
            i++;
        }
        ui totalCnt = 0;//the count of cliques assigned
        ui gap = i<n ? r[V[i]]-r[V[i-1]] : UINT32_MAX;
// #define DEBUG
#ifdef DEBUG
if(ped-pbg==92 && hed-hbg==1) {
printf("        new\n");
for(itr ii = hbg; ii != hed; ii++) {
    printf("%u ", *ii);
}
printf("/ ");
for(itr ii = pbg; ii != ped; ii++) {
    printf("%u ", *ii);
}
printf("\n");
}
#endif
// #define DDEBUG
#ifdef DDEBUG
if(ped-pbg==92 && hed-hbg==1) {
printf("st %u, i %u, gap %u, n %u\n", st, i, gap, n);fflush(stdout);
}
#endif

        while(totalCnt < upper) {
#ifdef DEBUG
if(ped-pbg==92 && hed-hbg==1) {
printf("\nV:"); 
for(ui v : V) printf("%u ", v); printf("\n");
printf("R:");
for(ui v : V) printf("%u ", r[v]); printf("\n");
printf("L:");
for(ui v : V) printf("%u ", localCnt[v]); printf("\n");
printf("upper:%u\n", upper);
printf("totalCnt:%u\n", totalCnt);
printf("st %u, i %u, gap %u\n", st, i, gap);fflush(stdout);
}
#endif
// printf("totalCnt %llu\n", totalCnt);fflush(stdout);
// printf("totalCnt %u\n", totalCnt);fflush(stdout);       
            do {
                ui minV = std::min((ull)gap, upper - totalCnt);
                // if(pCnt >= 1) {
                ui pUpper = cb(pCnt-1, k-(hed-hbg)-1);
                for(ui j = st; j < i; j++) {
                    if(localCnt[V[j]] > pUpper && isP[V[j]]) {
                        localCnt[V[j]] = pUpper;
                    }
                    minV = std::min(minV, localCnt[V[j]]);
                }
                // }
                // else {
                //     for(ui j = st; j < i; j++) {
                //         minV = std::min(minV, localCnt[V[j]]);
                //     }
                // }
#ifdef DEBUG
if(ped-pbg==92 && hed-hbg==1) {
printf("minV %u (i-st) %u upper-totalCnt %u\n", minV, i-st, upper - totalCnt);
}
#endif
                if((ull)minV*(i-st) <= upper - totalCnt) {
                    for(ui j = st; j < i; j++) {
                        r[V[j]] += minV;
                        localCnt[V[j]] -= minV;
                        totalCnt += minV;
// assert(localCnt[V[j]] >= 0 && localCnt[V[j]] <= upper);
#ifdef DEBUG
if(ped-pbg==92 && hed-hbg==1) {
printf("add1 %u %u, lc %u\n", V[j], minV, localCnt[V[j]]);
}
#endif

                        if(localCnt[V[j]] == 0) {
                            if(isP[V[j]]) {
                                pCnt--;
                            }
                            std::swap(V[st], V[j]);
                            st++;
                        }
                    }
// assert(totalCnt <= upper);
                }
                else {
                    ui v = (upper - totalCnt) / (i - st);//v <= minV
// assert(v <= minV);
// assert(v*(i-st) <= upper - totalCnt);
                    ui vmod = (upper - totalCnt) % (i - st);
// assert(st + vmod < i);
// assert((v+1)*vmod+totalCnt <= upper);
// assert(totalCnt <= upper);
// printf("%u %u v%u, vmod%u, i-st-vmod %u\n", totalCnt, upper, v, vmod, i-st-vmod);
                    for(ui j = st, ed = st + vmod; j < ed; j++) {
                        r[V[j]] += v+1;
                        localCnt[V[j]] -= v+1;
                        totalCnt += v+1;
// assert(localCnt[V[j]] >= 0 && localCnt[V[j]] <= upper);
#ifdef DEBUG
if(ped-pbg==92 && hed-hbg==1) {
printf("add2 %u %u, vmod %u, j%u, st + vmod%u\n", V[j], v+1, vmod, j, st + vmod);
}
#endif
                        // if(localCnt[V[j]] == 0) {
                        //     std::swap(V[st], V[j]);
                        //     ++st;
                        // }
                    }
// printf("%u\n", totalCnt);fflush(stdout);
// assert(totalCnt <= upper);
                    for(ui j = st + vmod; j < i; j++) {
                        r[V[j]] += v;
                        localCnt[V[j]] -= v;
                        totalCnt += v;
// assert(localCnt[V[j]] >= 0 && localCnt[V[j]] <= upper);
#ifdef DEBUG
if(ped-pbg==92 && hed-hbg==1) {
printf("add3 %u %u\n", V[j], v);
}
#endif
                        // if(localCnt[V[j]] == 0) {
                        //     std::swap(V[st++], V[j]);
                        // }
                    }
// assert(totalCnt == upper);
                }
            } while(totalCnt < upper && r[V[i-1]] < r[V[i]] && st < i);

#ifdef DEBUG
if(ped-pbg==92 && hed-hbg==1) {
printf("totalCnt2 %u ", totalCnt);fflush(stdout);
}
#endif
            ui iNew = std::min(i+1, n);
            while(iNew < n && r[V[iNew]] == r[V[iNew-1]]) {
                iNew++;
            }
            ui gapNew = iNew<n ? r[V[iNew]]-r[V[iNew-1]] : UINT32_MAX;
#ifdef DEBUG
if(ped-pbg==92 && hed-hbg==1) {
printf("totalCnt3 %u ", totalCnt);fflush(stdout);
}
#endif

            ui stNew = st;
            while(stNew < iNew && localCnt[V[stNew]] == 0) {
                stNew++;
            }
#ifdef DEBUG
if(ped-pbg==92 && hed-hbg==1) {
printf("totalCnt4 %u %u \n", totalCnt, upper);fflush(stdout);
}
#endif
            i= iNew;
            st = stNew;
            gap = gapNew;
        }
// assert(totalCnt == upper);

    };
    using namespace std::placeholders;
    auto f2 = std::bind(f, _1, _2, _3, _4, _5, _6, _7, _8, _9, &r, k);

// g.print();
    // for(int i = 0; i < t; i++) {
    //     // etr->sortSCTNodesByMinR(r); //sort SCT nodes
    //     etr->scanSCT(f2);
    // }
    SCT * sctEtr = static_cast<SCT*>(etr);
    for(int i = 0; i < t; i++) {
// printf("ITER:%d\n", i);
        sctEtr->sctppFixedBugPupper(f2);
    }
}



void kclistpp::runSCTPPWK(int t) {
    T += t;
    SCT * sctEtr = static_cast<SCT*>(etr);
    for(int i = 0; i < t; i++)
        sctEtr->sctppWk(r);
}

void kclistpp::runKclistPP(int t) {
    T += t;

    auto f = [](const itr & bg, const itr & ed, std::vector<ui> * rr) {
        itr st = bg;
        for(itr i = bg+1; i != ed; ++i) {
            if((*rr)[*i] < (*rr)[*st]) st = i;
        }
        (*rr)[*st]++;
    };
    auto f2 = std::bind(f, 
        std::placeholders::_1, std::placeholders::_2, &r);

    for(int i = 0; i < t; i++) etr->scan(f2);
}




void kclistpp::runWatering(int t, int weight) {
    T += t;

    auto f = [](const itr & bg, const itr & ed, ui weight, std::vector<ui> * rr) {
        std::sort(bg, ed, [&](ui a, ui b){ return (*rr)[a] < (*rr)[b]; });
        for(itr i = bg; i+1 != ed && weight; ++i) {
            ui diff = (*rr)[*(i+1)] - (*rr)[*i];
            if(diff == 0) continue;
            if(diff * (i-bg+1) <= weight) {
                for(itr j = bg; j != i+1; j++) (*rr)[*j] += diff;
                weight -= diff * (i-bg+1);
            }
            else {
                ui val = weight / (i-bg+1);
                weight %= (i-bg+1);
                itr j = bg;
                
                for(; j != bg+weight; j++) (*rr)[*j] += 1+val;
                if(val) for(; j != i+1; j++) (*rr)[*j] += val; 
                weight = 0;
            }
        }

        if(weight) {
            ui val = weight / (ed-bg);
            weight %= (ed-bg);
            itr i = bg;
            
            for(; i != bg+weight; i++) (*rr)[*i] += 1+val;
            if(val) for(; i != ed; i++) (*rr)[*i] += val; 
        }
    };
    auto f2 = std::bind(f, 
        std::placeholders::_1, std::placeholders::_2, k, &r);

    for(int i = 0; i < t; i++) etr->scan(f2);
}



double kclistpp::appDensity(bool printNodes) {
    // std::vector<ui> reordered(g.n);

    for(ui i = 0; i < g.n; i++) reordered[i] = i;

// auto ff = [](const itr & bg, const itr & ed, 
//      std::vector<ui> * r) {
//     for(itr i = bg; i != ed; ++i) {
//         (*r)[*i]++;
//     }
// };
// using namespace std::placeholders;
// auto ff2 = std::bind(ff, _1, _2, &rPava);
// etr->scan(ff2);

    std::sort(reordered.begin(), reordered.end(), 
        [&](ui a, ui b) { return r[a] > r[b]; }
    );
    for(ui i = 0; i < g.n; i++) level[reordered[i]] = i;
    
    auto f = [](const itr & bg, const itr & ed, 
        const std::vector<ui> & level, std::vector<ui> * rPava) {
        itr st = bg;
// printf("st %u ", *st);fflush(stdout);
// printf("%u %u %u\n", *st, level[*st], rPava[level[*st]]);fflush(stdout);
        for(itr i = bg+1; i != ed; ++i) {
// printf("ii %u ", *i);fflush(stdout);
            if(level[*i] > level[*st]) {
                st = i;
            }
        }
// printf("st %u %u", *st, level[*st]);fflush(stdout);
// printf(" xx %u ", (*rPava)[level[*st]]);fflush(stdout);
        (*rPava)[level[*st]]++;
// printf("%u %u %u\n", *st, level[*st], rPava[level[*st]]);fflush(stdout);
    };
    using namespace std::placeholders;
    for(ui i = 0; i < g.n; i++) rPava[i] = 0;
    auto f2 = std::bind(f, _1, _2, level, &rPava);
    // enumerator.scan(f2);

    etr->scan(f2);

    ull sum = 0;
    rho = 0.0;
    for (ui i = 0; i < g.n; ++i) {
        sum += rPava[i];
        if ((double)sum / (i + 1) > rho) {
            rho = (double)sum / (i + 1);
            sizeDensestSubgraph = i + 1;
        }
    }

    if(printNodes) {
        printf("NodesOfDensest:");
        for(ui i = 0; i < sizeDensestSubgraph; i++) {
            printf("%u ", reordered[i]);
        }
        printf("\n");
    }

    return rho;
}

double kclistpp::appDensitySCT(bool printNodes) {
    // std::vector<ui> reordered(g.n);

    for(ui i = 0; i < g.n; i++) reordered[i] = i;
    std::sort(reordered.begin(), reordered.end(), 
        [&](ui a, ui b) { return r[a] > r[b]; }
    );
    for(ui i = 0; i < g.n; i++) level[reordered[i]] = i;

    for(ui i = 0; i < g.n; i++) rPava[i] = 0;
    etr->scanSCTPava(level, rPava);

    ull sum = 0;
    rho = 0.0;
    for (ui i = 0; i < g.n; ++i) {
        sum += rPava[i];
        if ((double)sum / (i + 1) > rho) {
            rho = (double)sum / (i + 1);
            sizeDensestSubgraph = i + 1;
        }
    }

    if(printNodes) {
        printf("NodesOfDensest:");
        for(ui i = 0; i < sizeDensestSubgraph; i++) {
            printf("%u ", reordered[i]);
        }
        printf("\n");
    }

    return rho;
}

// bool kclistpp::checkExact() {
//     auto f = [](const itr & bg, const itr & ed, 
//         const std::vector<ui> & level, ui sz,
//         ull * cntClique,
//         std::vector<ui> & localCnt) {
//         for(itr i = bg; i != ed; ++i) {
//             if(level[*i] >= sz) return;
//         }
//         (*cntClique)++;
//         for(itr i = bg; i != ed; ++i) {
//             localCnt[*i]++;
//         }
//     };

//     ull cntClique = 0;
//     std::vector<ui> localCnt(sizeDensestSubgraph);
//     using namespace std::placeholders;
//     auto f2 = std::bind(f, _1, _2, level, 
//         sizeDensestSubgraph, &cntClique, localCnt);
//     // enumerator.scan(f2);
//     etr->scan(f2);

// std::cout << "runningflowCntInClique:" << cntClique << std::endl;

//     ui cntOfV = sizeDensestSubgraph + cntClique + 2;
//     ull cntOfE = cntClique*(k+1) + sizeDensestSubgraph;
//     ui s = 0, t = 1;

//     HLPP hlpp(cntOfV, cntOfE, s, t);
// std::cout << "starting to build flow network" << std::endl;

//     //prealloc memory. avoid memory crash
//     hlpp.resize(s, cntClique);
//     hlpp.resize(t, sizeDensestSubgraph);
//     for(ui i = 2; i < sizeDensestSubgraph+2; i++) {
//         hlpp.resize(i, localCnt[i-2]+1);
//     }
//     for(ui i = sizeDensestSubgraph + 2; i < cntOfV; i++) {
//         hlpp.resize(i, k+1);
//     }

//     auto f3 = [](const itr & bg, const itr & ed, 
//         const std::vector<ui> & level, ui sz, ui s,
//         HLPP * hlpp, ull * idClique)
//     {
//         for(itr i = bg; i != ed; ++i) {
//             if(level[*i] >= sz) return;
//         }

//         for(itr i = bg; i != ed; ++i) {
//             (*hlpp).addEdge(*idClique, level[*i]+2, sz);
//         }
//         (*hlpp).addEdge(s, *idClique, sz);  
//         (*idClique)++;
//     };
//     ull id = sizeDensestSubgraph + 2;
//     auto f4 = std::bind(f3, _1, _2, level,
//         sizeDensestSubgraph, s, &hlpp, &id);
//     // enumerator.scan(f4);
//     etr->scan(f4);

//     for(ui i = 2; i < sizeDensestSubgraph+2; i++) {
//         hlpp.addEdge(i, t, cntClique);
//     }
// std::cout << "network_build" << std::endl;

//     ull flow = hlpp.maxFlow();
// std::cout << "flow:" << flow << ' ';
// std::cout << "n*m:" << cntClique*sizeDensestSubgraph << std::endl;
//     return flow == cntClique*sizeDensestSubgraph;
// }

bool kclistpp::checkExact() {
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
    std::vector<ull> localCnt(sizeDensestSubgraph, 0);
    using namespace std::placeholders;
    auto f2 = std::bind(f, _1, _2, level, 
        sizeDensestSubgraph, &cntClique, &localCnt);
    // enumerator.scan(f2);
    etr->scan(f2);
// ui tmpCntSum = 0;
// for(ui i = 0; i < sizeDensestSubgraph; i++) {
//     tmpCntSum += localCnt[i];
// }
// assert(tmpCntSum == cntClique*k);
std::cout << "runningflowCntInClique:" << cntClique << std::endl;

    ull cntOfV = sizeDensestSubgraph + cntClique + 2;
    ull cntOfE = cntClique*(k+1) + sizeDensestSubgraph;
    ui s = 0, t = 1;

    HLPP_MemoryOptimized hlpp(cntOfV, cntOfE, s, t);
std::cout << "starting to build flow network" << std::endl;

    //prealloc memory. avoid memory crash
    // hlpp.resize(s, cntClique);
    // hlpp.resize(t, sizeDensestSubgraph);
    // for(ui i = 2; i <= sizeDensestSubgraph+2; i++) {
    //     hlpp.resize(i, localCnt[i-2]+1);
    // }
    // for(ui i = sizeDensestSubgraph + 2; i < cntOfV; i++) {
    //     hlpp.resize(i, k+1);
    // }
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
std::cout << "alloc memory" << std::endl;
    auto f3 = [](const itr & bg, const itr & ed, 
        const std::vector<ui> & level, ui sz, ui s,
        HLPP_MemoryOptimized * hlpp, ull * idClique)
    {
        for(itr i = bg; i != ed; ++i) {
            if(level[*i] >= sz) return;
        }

        for(itr i = bg; i != ed; ++i) {
// assert((*idClique) < 20215019+133+2);
// assert(level[*i] < sz);
            (*hlpp).addEdge(*idClique, level[*i]+2, sz);
        }
        (*hlpp).addEdge(s, *idClique, sz);  
        (*idClique)++;
    };
    ull id = sizeDensestSubgraph + 2;
    using namespace std::placeholders;
    auto f4 = std::bind(f3, _1, _2, level,
        sizeDensestSubgraph, s, &hlpp, &id);
    // enumerator.scan(f4);
    etr->scan(f4);
std::cout << "add edge type 1 & 2" << std::endl;

    for(ui i = 2; i < sizeDensestSubgraph+2; i++) {
        hlpp.addEdge(i, t, cntClique);
    }

    for(ull i = cntOfV; i > 0; i--) {
        hlpp.pIdx[i] = hlpp.pIdx[i - 1];
    }
    hlpp.pIdx[0] = 0;

// assert(hlpp.pIdx[1] == cntClique);
// assert(hlpp.pIdx[2]-hlpp.pIdx[1] == sizeDensestSubgraph);
// for(ui i = 2; i < sizeDensestSubgraph+2; i++) {
//     assert(hlpp.pIdx[i+1] - hlpp.pIdx[i] == localCnt[i-2]+1);
//     assert(hlpp.pIdx[i+1] < hlpp.pEdge.size());
// }
// for(ull i = sizeDensestSubgraph + 2; i < cntOfV; i++) {
//     assert(hlpp.pIdx[i+1] - hlpp.pIdx[i] == k+1);
//     assert(hlpp.pIdx[i+1] <= hlpp.pEdge.size());
// }
// assert(hlpp.pIdx[cntOfV] == hlpp.pEdge.size());
std::cout << "network_build" << std::endl;

    ull flow = hlpp.maxFlow();
std::cout << "flow:" << flow << ' ';
std::cout << "n*m:" << cntClique*sizeDensestSubgraph << std::endl;
    return flow == cntClique*sizeDensestSubgraph;
}

bool kclistpp::checkExactSCT() {
    auto f = [](const itr & hbg, const itr & hed, //the h nodes
        const itr & pbg, const itr & ped, //the p nodes
        ui upper, //total count of k-cliques
        std::vector<ui> *V_, 
        std::vector<ui> * localCnt_,
        std::vector<ull> * r_
        ,ui sz, const std::vector<ui> & level, comb * cb, ui k
        , ull * cntClique
         ) {
// printf("here0 "); fflush(stdout);
        for(itr i = hbg; i != hed; ++i) {
            if(level[*i] >= sz) return;
        }
// printf("here1 "); fflush(stdout);
        ui h = hed - hbg;
        ui p = 0;
        for(itr i = pbg; i != ped; ++i) {
            if(level[*i] < sz) p++;
        }
        if(p + h < k) return;
        ull totalCnt = (*cb)(p, k-h);
        (*cntClique) += totalCnt;
// printf("here2 "); fflush(stdout);
        for(itr it = hbg; it != hed; it++) {
            (*r_)[level[*it]] += totalCnt;
        }
        if(p > 0 && k >= h+1)
        for(itr it = pbg; it != ped; it++) {
            if(level[*it] >= sz) continue;
            (*r_)[level[*it]] += (*cb)(p-1, k-h-1);
        }
// printf("here3\n"); fflush(stdout);
    };
// printf("here\n"); fflush(stdout);
    comb cb;
    ull cntClique = 0;
    using namespace std::placeholders;
    std::vector<ull> localCnt(sizeDensestSubgraph);
    auto f2 = std::bind(f, _1, _2, _3, _4, _5, _6, _7, 
        &localCnt, sizeDensestSubgraph, level, &cb, k, &cntClique);
// printf("here\n"); fflush(stdout);
    etr->scanSCT(f2);
   
std::cout << "runningflowCntInClique:" << cntClique << std::endl;

    ull cntOfV = sizeDensestSubgraph + cntClique + 2;
    ull cntOfE = cntClique*(k+1) + sizeDensestSubgraph;
    ui s = 0, t = 1;

    HLPP_MemoryOptimized hlpp(cntOfV, cntOfE, s, t);
std::cout << "starting to build flow network" << std::endl;

    //prealloc memory. avoid memory crash
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

    auto f3 = [](const itr & hbg, const itr & hed, //the h nodes
        const itr & pbg, const itr & ped, //the p nodes
        ui upper, //total count of k-cliques
        std::vector<ui> *V_, 
        std::vector<ui> * localCnt_,
        ui s, ui sz, const std::vector<ui> & level,
        HLPP_MemoryOptimized * hlpp, ull * idClique,
        comb * cb, ui k
         ) {
        for(itr i = hbg; i != hed; ++i) {
            if(level[*i] >= sz) return;
        }
        ui h = hed - hbg;
        ui p = 0;
        for(itr i = pbg; i != ped; ++i) {
            if(level[*i] < sz) p++;
        }
        if(p + hed-hbg < k) return;
        //ids from (*idClique) to (*idClique)+(*localCnt_)[*it]
        ull totalCnt = (*cb)(p, k-h);
ull tmp = 0;

        for(ull i = 0; i < totalCnt; i++) {
            (*hlpp).addEdge(s, (*idClique)+i, sz); 
            for(itr it = hbg; it != hed; it++) {
                (*hlpp).addEdge((*idClique)+i, level[*it]+2, sz);
tmp++;
            }

            ui nowId = 0;
            ui choosed = 0;
            ui j = 0;
            for(itr it = pbg; it != ped; it++) {
                if(level[*it] >= sz) continue;
                //choose j: cb(p-j-1, k-h-choosed-1)
                //not choose j: cb(p-j-1, k-h-choosed)
                
                if(nowId + (*cb)(p-j-1, k-h-choosed-1) > i) {
                    choosed++;
                    (*hlpp).addEdge((*idClique)+i, level[*it]+2, sz);
tmp++;
                    if(choosed + h == k) {
assert(nowId == i);
                        break;
                    }
                }
                else {
                    nowId += (*cb)(p-j-1, k-h-choosed-1);
                }

                j++;
            }
assert(choosed == k-h);
        }
assert(tmp == totalCnt*k);
        (* idClique) += totalCnt;
    };
    ull id = sizeDensestSubgraph + 2;
// ull cntClique2 = 0;
    auto f4 = std::bind(f3, _1, _2, _3, _4, _5, _6, _7,
        s, sizeDensestSubgraph, level, &hlpp, &id, &cb, k);
    etr->scanSCT(f4);

    for(ui i = 2; i < sizeDensestSubgraph+2; i++) {
        hlpp.addEdge(i, t, cntClique);
    }

    for(ull i = cntOfV; i > 0; i--) {
        hlpp.pIdx[i] = hlpp.pIdx[i - 1];
    }
    hlpp.pIdx[0] = 0;
// for(ui i = 2; i < sizeDensestSubgraph+2; i++) {
// if(hlpp.pIdx[i+1]-hlpp.pIdx[i] != localCnt[i-2]+1) {
// printf("%u %u %llu\n", i, hlpp.pIdx[i+1]-hlpp.pIdx[i], localCnt[i-2]+1);
// }
// assert(hlpp.pIdx[i+1]-hlpp.pIdx[i] == localCnt[i-2]+1);
// }
std::cout << "network_build" << std::endl;

    ull flow = hlpp.maxFlow();
std::cout << "flow:" << flow << ' ';
std::cout << "n*m:" << cntClique*sizeDensestSubgraph << std::endl;
    return flow == cntClique*sizeDensestSubgraph;
}

bool kclistpp::strongStableSubset() {
    ui maxR = r[reordered[0]];
    ui minR = r[reordered[sizeDensestSubgraph-1]];
    return maxR - minR <= k * T;
}

bool kclistpp::stableSubset() {
//     ui maxR = r[reordered[0]];
//     ui minR = r[reordered[sizeDensestSubgraph-1]];
// printf("diff:%u\n", maxR /T - minR / T);
    // return maxR - minR <= k * T;

    SCT * sctEtr = static_cast<SCT*>(etr); 
// assert(sctEtr->checkSummaryOfAlpha(r, T));
    ui tmp;
    return sctEtr->checkStableFlow(r, 
        sizeDensestSubgraph, reordered, level, T, tmp);

    // SCT * sctEtr = static_cast<SCT*>(etr); 
    // ui nexSize = sizeDensestSubgraph;
    // while(!sctEtr->checkStableFlow(r, 
    //     sizeDensestSubgraph, reordered, level, T, nexSize)) {
    //     if(nexSize == sizeDensestSubgraph+1) return true;
    //     sizeDensestSubgraph = nexSize;
    // }
    // printf("sizeDensestSubgraphStable:%u\n", sizeDensestSubgraph);
    // return true;
}


// bool kclistpp::runConvexToFlow() {
//     // //get kcliques in stable subsets
//     // cliqueE_kClist kcl;
//     // kcl.buildCliquesFormSCT();
//     // kcl.shuffle();
//     // kclistpp kpp(g, etr, k);
    
//     // kpp.runKclistPP()
// }

ui kclistpp::shrinkNodesByAppRho(double appRho) {
    auto f = [](const itr & hbg, const itr & hed, //the h nodes
        const itr & pbg, const itr & ped, //the p nodes
        ui upper, //total count of k-cliques
        std::vector<ui> *V_, 
        std::vector<ui> * localCnt_,
        std::vector<ull> * r_
         ) {
        for(itr it = hbg; it != hed; it++) {
            (*r_)[*it] += (*localCnt_)[*it];
        }
        for(itr it = pbg; it != ped; it++) {
            (*r_)[*it] += (*localCnt_)[*it];
        }
    };
    using namespace std::placeholders;

    std::vector<ull> r(g.n);
    // for(ui i = 0; i < g.n; i++) r[i] = 0;

    auto f2 = std::bind(f, _1, _2, _3, _4, _5, _6, _7, &r);

    etr->scanSCT(f2);

    // if(reordered.size() == 0) {
    //     reordered.resize(g.n);
    //     for(ui i = 0; i < g.n; i++) reordered[i] = i;
    // }
    for(ui i = 0; i < g.n; i++) reordered[i] = i;
    std::sort(reordered.begin(), reordered.end(), 
        [&](ui a, ui b) { return r[a] > r[b]; }
    );
    for(ui i = 0; i < g.n; i++) level[reordered[i]] = i;

// for(ui i = 0; i < 10; i++) printf("%llu ", r[reordered[i]]);
// printf("\n");
// printf("mI:%u\n", reordered[0]);

    sizeDensestSubgraph = 0;
    while(sizeDensestSubgraph < g.n && 
            r[reordered[sizeDensestSubgraph]] >= appRho) {
        sizeDensestSubgraph++;
    }

    std::vector<ull>().swap(r);

    return sizeDensestSubgraph;
}

bool kclistpp::updateEnumeratorAfterShrink() {
    SCT * sctEtr = static_cast<SCT*>(etr);
    return sctEtr->shrink(level, sizeDensestSubgraph);
}

