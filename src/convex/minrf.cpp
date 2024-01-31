#include "minrf.h"
#include "cliqueFinderSCT.hpp"

#include <queue>
#include <utility>
#include <algorithm>
#include <iostream>
#include <random>

#define DDEBUG

minrf::minrf(Graph & g, ui k) : 
g(g), k(k)
{}

void minrf::runKCL(ui t) {
    cfKCL cf(g, k);

    // using P = std::pair<ui, ui>;
    // std::priority_queue<P, std::vector<P>, std::greater<P>> que;
    struct P {
        ui first, second;
        P() {}
        P(ui a, ui b):first(a), second(b) {}
        bool operator < (const P & t) const {
            return first < t.first;
        }
        bool operator > (const P & t) const {
            return first > t.first;
        }
    };
    std::priority_queue<P, std::vector<P>, std::greater<P>> que;

    level.resize(g.n);
    sortByR.resize(g.n);
    r.resize(g.n);

    // cf.localCnt(r);

    // for(ui i = 0; i < g.n; i++) level[i] = i;
    // std::mt19937 gen(std::random_device{}());
    // std::shuffle(level.begin(), level.end(), gen);

    // typedef typename std::vector<ui>::iterator itr;
    // auto f = [](const itr & bg, const itr & ed, std::vector<ui> * rr) {
    //     itr st = bg;
    //     for(itr i = bg+1; i != ed; ++i) {
    //         if((*rr)[*i] < (*rr)[*st]) st = i;
    //     }
    //     (*rr)[*st]++;
    // };
    // auto f2 = std::bind(f, 
    //     std::placeholders::_1, std::placeholders::_2, &r);
    // // cf.kcl->shuffle();
    // cf.kcl->scan(f2);
    // for(ui i = 0; i < g.n; i++) sortByR[i] = i;
    // std::sort(sortByR.begin(), sortByR.end(), [&](ui a, ui b) {
    //     return r[a] > r[b];
    // });
    // for(ui i = 0; i < g.n; i++) level[sortByR[i]] = i;
#ifdef DEBUG
g.print();
#endif

    std::random_device rd;
    std::mt19937 gg(rd());
    
    std::uniform_int_distribution<ui> D;

printf("cntClique %llu\n", cf.leftCliqueCnt);
    ui l = g.n;
    for(ui it = 0; it < t; it++) {
        for(ui i = 0; i < g.n; i++) {
            que.push({r[i], i});
        }

        while(cf.leftCliqueCnt > 0) {
// assert(!que.empty());
            P h = que.top(); que.pop();
// assert(h.first == r[h.second]);
// printf(" %llu v%u r%u\n", 
//     cf.leftCliqueCnt, h.second, r[h.second]);
// fflush(stdout);
            if(cf.find(h.second, D, gg)) {
// printf("here22 %llu\n", cf.leftCliqueCnt);fflush(stdout);
                r[h.second]++;
                que.push(P(r[h.second], h.second));
            }
            else {
                level[h.second] = --l;
                sortByR[l] = h.second;
            }
        }
        while(!que.empty()) {
            P h = que.top(); que.pop();
            level[h.second] = --l;
            sortByR[l] = h.second;
        }

        cf.reset();
        l = g.n;
// printf("\n");
    }
    
ui maxRV = 0;
for(ui i = 1; i < g.n; i++) {
    if(r[i] > r[maxRV]) {
        maxRV = i;
    }
}
ull sumR = 0;
for(ui i = 0; i < g.n; i++) {
    sumR += r[i];
}
printf("rmax %u \n", r[maxRV]);
printf("sumR %llu \n", sumR);
printf("here3\n");fflush(stdout);
    pava2(cf);
}

void minrf::run() {
    cfSCT cf(g, k);

printf("here2 %llu\n", cf.leftCliqueCnt);fflush(stdout);
    // using P = std::pair<ui, ui>;
    struct P {
        ui first, second;
        P() {}
        P(ui a, ui b):first(a), second(b) {}
        bool operator < (const P & t) const {
            return first < t.first;
        }
        bool operator > (const P & t) const {
            return first > t.first;
        }
    };
    std::priority_queue<P, std::vector<P>, std::greater<P>> que;
    // std::priority_queue<P, std::vector<P>> que;

    level.resize(g.n);
    r.resize(g.n);

    for(ui i = 0; i < g.n; i++) {
        que.push({r[i], i});
    }
    
    std::random_device rd;
    std::mt19937 gg(rd());
    
    std::uniform_int_distribution<ui> D;
    // D(gg, param_t(0, i));

    ui l = g.n;
    while(cf.leftCliqueCnt > 0) {
assert(!que.empty());
        P h = que.top(); que.pop();
printf("here21 %llu %u %u %u\n", 
    cf.leftCliqueCnt, h.second, r[h.second], h.first);
fflush(stdout);
        if(cf.find(h.second, D, gg)) {
printf("here22 %llu\n", cf.leftCliqueCnt);fflush(stdout);
            r[h.second]++;
            que.push({r[h.second], h.second});
        }
        else level[h.second] = --l;
    }
printf("rmax %u\n", r[level[0]]);
printf("here3\n");fflush(stdout);
    pava();
}
void minrf::pava() {}

void minrf::pava2(cfKCL & cf) {
    std::vector<ui> rPava(g.n);
    typedef typename std::vector<ui>::iterator itr;
    auto f3 = [](const itr & bg, const itr & ed, 
        std::vector<ui> * rPava, std::vector<ui> * level, ui cid) {
        itr st = bg;
        for(itr i = bg + 1; i != ed; ++i) {
            if((*level)[*i] > (*level)[*st]) st = i;
        }
        (*rPava)[*st]++;
#ifdef DEBUG
printf("apply:");
for(itr i = bg; i != ed; ++i) printf("%u ", *i);
printf(" : %u\n", *st);
#endif
    };
    auto f4 = std::bind(f3, std::placeholders::_1, 
        std::placeholders::_2, &rPava, &level, std::placeholders::_3);
    for(ull i = 0; i < cf.cntClique; i++) {
        cf.kcl->applyToAClique(i, f4);
    }
    
#ifdef DEBUG
printf("level:");
for(ui i = 0; i < g.n; i++) printf("%u ", level[i]);
printf("\nsortByR:");
for(ui i = 0; i < g.n; i++) printf("%u ", sortByR[i]);
printf("\nrPava:");
for(ui i = 0; i < g.n; i++) printf("%u ", rPava[sortByR[i]]);
printf("\n");
#endif
    // sortByR.resize(g.n);
    // std::sort(sortByR.begin(), sortByR.end(), [&](ui a, ui b) {
    //     return rPava[a] > rPava[b];
    // });
// printf("%u %u\n", 0, r[sortByR[0]]);
// for (ui i = 1; i < sortByR.size(); ++i) {
//     if(r[sortByR[i]] < r[sortByR[i-1]])
//     printf("%u %u\n", i, r[sortByR[i]]);
// }

    double sumRhoMax = 0;
    ui sizeDensestSubgraph= 0;
    ull sum = 0;
    for (ui i = 0; i < sortByR.size(); ++i) {
        sum += rPava[sortByR[i]];
        if ((double)sum / (i + 1) > sumRhoMax) {
            sumRhoMax = (double)sum / (i + 1);
            sizeDensestSubgraph = i + 1;

        }
    }

//after pava
ui rma = 0;
ui rmi = 10000000;
for(ui j = 0; j <= sizeDensestSubgraph; j++) {
    rma = std::max(rma, r[sortByR[j]]);
    rmi = std::min(rmi, r[sortByR[j]]);
}
printf("rmax-rmin: %u, %u\n", rma, rmi);
// for(ui j = 0; j <= sizeDensestSubgraph; j++) {
//     printf("%u ", r[sortByR[j]]);
// }printf("\n");

std::cout << "sizeDensestSubgraph:" << sizeDensestSubgraph << std::endl;
std::cout << "appD:" << sumRhoMax << std::endl;

    appDensestSg.resize(sizeDensestSubgraph);
    for(ui i = 0; i < sizeDensestSubgraph; i++) {
        appDensestSg[i] = sortByR[i];
    }

    // return sumRhoMax[0];
    cntCliqueSG = sumRhoMax*sizeDensestSubgraph + 1e-12;
    szSG = sizeDensestSubgraph;
}