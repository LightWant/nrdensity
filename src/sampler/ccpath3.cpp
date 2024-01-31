#include "ccpath.h"
#include <random>
#include <algorithm>
#include <iostream>
#include <cassert>
#include "../kcliqueEnumerator/sct.h"


/*
lm=log(m)
6exp(k*lm-k*e^2*r/12) <= d
k*lm-k*e^2*r/12 <= ln(d/6)
lm-e^2*r/12<= ln(d/6) / k
r >= 12*(lm - ln(d/6) / k) / e^2
e = 0.1
d = 0.01
m = 2.07309e+07 (youtube,k=6)
lm= 16.8
k = 102
ln(d/6)=-6.39692965
ln(d/6) / k = -0.06
r >= 12* 17 * 100 = 
*/
double ccpath::realEdgeDensityInCore() {
    ui edges = g->m / 2;
    for(ui u = 0; u < g->n; u++) {
        if(g->pIdx[u+1] - g->pIdx2[u] == g->coreNumber) {
            return 1.0 * edges / (g->n - u);
        }
        edges -= g->pIdx[u+1] - g->pIdx2[u];
    }
    return 0.0;
}

double ccpath::appDensity(bool biased) {
    double rcore = realEdgeDensityInCore();
std::cout << "edgeDensity:" << rcore << std::endl;
    ui stUOfMaxCore = 0;
    for(ui u = 0; u < g->n; u++) {
        if(g->pIdx[u+1] - g->pIdx2[u] >= rcore) {
            stUOfMaxCore = u;
            break;
        }
    }
    // if(stUOfMaxCore + g->n/100 > g->n) {
    //     stUOfMaxCore = g->n - g->n/100;
    // }
// stUOfMaxCore = 0;
std::cout << "stU:" << stUOfMaxCore << std::endl;

    for(ui u = stUOfMaxCore; u < g->n; u++) {
        vis[u] = true;
    }
    ui coreSize = g->n - stUOfMaxCore;
    nodes.resize(coreSize);
    nodes.clear();
    for(ui u = stUOfMaxCore; u < g->n; u++) {
        nodes.push_back(u);
    }

std::cout << "coreSize:" << coreSize << std::endl;

    double rho = appThroughCCpath(biased);
    
    return rho;
}

void ccpath::coloring() {
    std::vector<ui> flag(g->coreNumber + 1, 0);
    colors[nodes[nodes.size()-1]] = 0;
    cntOfColors = 0;
    for(ui i = nodes.size()-1; i > 0; i--) {
        ui u = nodes[i-1];

        for(ui j = g->pIdx2[u]; j < g->pIdx[u+1]; j++) {
            ui v = g->pEdge[j];
            if(!vis[v]) continue;
            flag[colors[v]] = i;
        }

        ui c = 0;
        while(flag[c] == i) c++;
        cntOfColors = std::max(cntOfColors, c);
        colors[u] = c;
    }
    cntOfColors++;
std::cout << "cntOfColors:" << cntOfColors << std::endl;
}

void ccpath::reIdNodesByColors() {
    sortByColor.resize(nodes.size());
    std::vector<ui> pColor(cntOfColors + 1);
    for(ui u : nodes) pColor[colors[u] + 1]++;
    for(ui c = 1; c <= cntOfColors; c++) pColor[c] += pColor[c-1];
    for(ui u : nodes) sortByColor[ pColor[colors[u]]++ ] = u;
    for(ui i = 0; i < nodes.size(); i++) reId[sortByColor[i]] = i;
// for(ui i = 0; i+1 < nodes.size(); i++) 
// assert(colors[sortByColor[i]] <= colors[sortByColor[i+1]]);

    pIdx.resize(nodes.size() + 1);
    for(ui i = 0; i <= nodes.size(); i++) pIdx[i] = 0;
    ui edges = 0;
    for(ui i = 0; i < nodes.size(); i++) {
        ui u = nodes[i];
        for(ui j = g->pIdx2[u]; j < g->pIdx[u+1]; j++) {
            ui v = g->pEdge[j];
            if(!vis[v]) continue;
            edges++;

            if(reId[v] < reId[u]) pIdx[reId[v]+1]++;
            else pIdx[reId[u]+1]++;
        }
    }
    
    for(ui i = 1; i <= nodes.size(); i++) pIdx[i] += pIdx[i-1];
    pEdge.resize(edges);
    for(ui i = 0; i < nodes.size(); i++) {
        ui u = nodes[i];
        for(ui j = g->pIdx2[u]; j < g->pIdx[u+1]; j++) {
            ui v = g->pEdge[j];
            if(!vis[v]) continue;

            if(reId[v] < reId[u]) pEdge[ pIdx[reId[v]]++ ] = reId[u];
            else pEdge[ pIdx[reId[u]]++ ] = reId[v];
        }
    }
    for(ui i = nodes.size(); i > 0; i--) pIdx[i] = pIdx[i-1];
    pIdx[0] = 0;
    for(ui i = 0; i < sortByColor.size(); i++) {
        std::sort(pEdge.begin() + pIdx[i], pEdge.begin() + pIdx[i+1]);
    }


std::cout << "shrinkedGraphEdges:" << edges << std::endl;
}

ull ccpath::DPLocalSg(ui u, ui n) {
    //build subgraph
    for(ui i = pIdx[u]; i < pIdx[u+1]; i++) {
        reIdForLocal[pEdge[i]] = i-pIdx[u];
    }

    ui deg = pIdx[u+1] - pIdx[u];
    for(ui i = 0; i <= deg; i++) ppIdx[i] = 0;
    for(ui i = pIdx[u]; i < pIdx[u+1]; i++) {
        ui v = pEdge[i];
        for(ui j = pIdx[v]; j < pIdx[v+1]; j++) {
            ui w = pEdge[j];
            if(reIdForLocal[w] != n+1) {
                ppIdx[i-pIdx[u]+1]++;
            }
        }
    }
    for(ui i = 0; i < deg; i++) ppIdx[i + 1] += ppIdx[i];
    for(ui i = pIdx[u]; i < pIdx[u+1]; i++) {
        ui v = pEdge[i];
        for(ui j = pIdx[v]; j < pIdx[v+1]; j++) {
            ui w = pEdge[j];
            if(reIdForLocal[w] != n+1) {
                ppEdge[ppIdx[i-pIdx[u]]++] = reIdForLocal[w];
            }
        }
    }
    for(ui i = deg; i > 0; i--) ppIdx[i] = ppIdx[i-1];
    ppIdx[0] = 0;
// if(u == 100) {
//     for(ui i = 0; i < deg; i++) {
//         printf("%u: ", i);
//         for(ui j = ppIdx[i]; j < ppIdx[i+1]; j++) {
//             printf("%u ", ppEdge[j]);
//         }
//         printf("\n");
//     }
//     printf("\n");
// }
    //dp
    for(ui i = 0; i < deg; i++) dp[0][i] = 1;
    for(ui i = 1; i < k-1; i++) {
        for(ui j = 0; j < deg; j++) {
            dp[i][j] = 0;
            for(ui l = ppIdx[j]; l < ppIdx[j+1]; l++) {
                dp[i][j] += dp[i-1][ppEdge[l]];
            }
        }
    }

    for(ui i = pIdx[u]; i < pIdx[u+1]; i++) {
        reIdForLocal[pEdge[i]] = n+1;
    }

    ull ret = 0;
    for(ui i = 0; i < deg; i++) ret += dp[k-2][i];

    return ret;
}
void ccpath::DP() {
    ui n = sortByColor.size();
    for(ui i = 0; i <= k; i++) {
        if(dp[i].size() < n) {
            dp[i].resize(n);
        }
    }
    if(reIdForLocal.size() < n) reIdForLocal.resize(n);
    for(ui i = 0; i < n; i++) reIdForLocal[i] = n+1;
    if(ppIdx.size() < n) ppIdx.resize(n);
    if(ppEdge.size() < pEdge.size()) ppEdge.resize(pEdge.size());

    cntCCPath = 0;
    for(ui u = 0; u < n; u++) {
        cntCCPath += DPLocalSg(u, n);
    }

std::cout << "cntCCPath:" << cntCCPath << std::endl;
}

double ccpath::sampleV2() {//only store sampled cliques
    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_real_distribution<double> uiDistribution(0, 1);
    
    std::vector<ui> bf(k);
    auto sampleOneTime = [&](ui u, ull ps) -> int {
        bf[0] = u;

        double sumD = 0.0;
        double x = uiDistribution(generator);
        ui l = 0;
        ui deg = pIdx[u+1] - pIdx[u];
        for(ui i = 0; i < deg; i++) {
            sumD += dp[k-2][i];
            if(sumD >= ps*x) {
                bf[1] = pEdge[i+pIdx[u]];
                l = i;
                break;
            }
        }

        for(ui i = 2; i < k; i++) {
            sumD = 0.0;
            for(ui j = ppIdx[l]; j < ppIdx[l + 1]; j++) {
                sumD += dp[k - i - 1][ppEdge[j]];
            }
// std::cout << sumD << ' ';
            x = uiDistribution(generator);
            double sumTmp = 0.0;
// bool choosed = false;
            for(ui j = ppIdx[l]; j < ppIdx[l + 1]; j++) {
                sumTmp += dp[k - i - 1][ppEdge[j]];
                if(sumTmp >= x * sumD) {
                    l = ppEdge[j];
                    bf[i] = pEdge[ppEdge[j]+pIdx[u]];
// choosed = true;
// for(ui c = 0; c < i; c++) assert(bf[i] != bf[c]);
// bool ff = false;
// for(ui c = pIdx[u]; c < pIdx[u+1]; c++) {
//     if(bf[i]==pEdge[c]) {
//         ff = true; break;
//     }
// }
// assert(ff);
                    break;
                }
            }
// assert(choosed);
        }

        auto connect = [&](ui u, ui v) {
// assert(u < v);
            auto st = pEdge.begin() + pIdx[u];
            auto ed = pEdge.begin() + pIdx[u+1];
            return std::binary_search(st, ed, v);
        };
        
        for(ui i = 1; i < k; i++) {
            for(ui j = i + 1; j < k; j++) {
                // if(!g->connectHash(sortByColor[bf[i]], sortByColor[bf[j]])) return 0;
                if(!connect(bf[i], bf[j])) {
// assert(!g->connectHash(sortByColor[bf[i]], sortByColor[bf[j]]));
                    // isClique[sampledCCPaths.size()/k] = false;
                    return 0;
                }
            }
        }
// printf("here\n");
        for(ui i = 0; i < k; i++) sampledCCPaths.push_back(bf[i]);

        return 1;
    };

    ui sampledT = 0;
    ui toTalZ = 0;
    sampledCCPaths.clear();
// ull tmpp = 0;
    for(ui i = 0; i < sortByColor.size(); i++) {
        ull ps = DPLocalSg(i, sortByColor.size());
        ui t = std::round(1.0 * T * ps / cntCCPath+1e-12);
// tmpp += ps;
        sampledT += t;
        
        ui z = 0;
        for(ui j = 0; j < t; j++) {
            z += sampleOneTime(i, ps);
        }
        toTalZ += z;
    }
// printf("%llu\n", tmpp);
// assert(tmpp == cntCCPath);

std::cout << "sampledT:" << sampledT << std::endl;
std::cout << "toTalZ:" << toTalZ << std::endl;

    double estimateCntClique = 1.0*cntCCPath*toTalZ/sampledT;
std::cout << "estimateCntClique:" << estimateCntClique << std::endl;

    //kclist++
    r.resize(sortByColor.size());
    for(ui i = 0; i < sortByColor.size(); i++) r[i] = 0;

    ui cntSampledCCPaths = sampledCCPaths.size() / k;
    for(ui t = 0; t < 20; t++) {
        for(ui i = 0; i < cntSampledCCPaths; i++) {
            ui minU = sampledCCPaths[i*k];
            for(ui j = 1; j < k; j++) {
                if(r[sampledCCPaths[i*k+j]] < r[minU]) 
                    minU = sampledCCPaths[i*k+j]; 
            }
            r[minU]++;
        }
    }

    // Pava();
    std::vector<ui> sortByR(sortByColor.size());
    std::vector<ui> level(sortByColor.size());
    for(ui i = 0; i < sortByR.size(); i++) sortByR[i] = i;
    std::sort(sortByR.begin(), sortByR.end(), 
        [&](ui u, ui v){ return r[u] > r[v]; }
    );
    for(ui i = 0; i < sortByR.size(); i++) level[sortByR[i]] = i;
// for(ui i = 0; i < 50; i++) std::cout << r[sortByR[i]] << ' ';
// std::cout << '\n';

    std::vector<ui> rPava(sortByColor.size());
    for(ui i = 0; i < cntSampledCCPaths; i++) {
        ui minU = level[sampledCCPaths[i*k]];
        for(ui j = 1; j < k; j++) {
            if(level[sampledCCPaths[i*k+j]] > minU) 
                minU = level[sampledCCPaths[i*k+j]]; 
        }
        rPava[minU] += 1;
    }
// for(ui i = 0; i < 50; i++) std::cout << rPava[i] << ' ';
// std::cout << '\n';


    double sumRhoMax = 0.0;
    ui sizeDensestSubgraph = 0;
    ull sum = 0;
    for (ui i = 0; i < sortByR.size(); ++i) {
        sum += rPava[i];
        if ((double)sum / (i + 1) > sumRhoMax) {
            sumRhoMax = (double)sum / (i + 1);
            sizeDensestSubgraph = i + 1;
        }
    }
    
std::cout << "sizeDensestSubgraph:" << sizeDensestSubgraph << std::endl;

    appDensestSg.resize(sizeDensestSubgraph);
    for(ui i = 0; i < sizeDensestSubgraph; i++) {
        appDensestSg[i] = sortByColor[sortByR[i]];
    }

    return sumRhoMax;
}

double ccpath::appThroughCCpath(bool biased) {
    coloring();
    reIdNodesByColors();
    DP();//pernode, get summary

    if(cntCCPath < T) {
        printf("sparse graph!\n");
        T = cntCCPath;
        // return 0.0;
    }

    return sampleV2();
}

double ccpath::realDensityInAppSg() {
// printf("appSg %u\n", appDensestSg.size());fflush(stdout);
// for(ui i = 0; i < 10; i++) printf("%u ", appDensestSg[i]);
// printf("\n"); fflush(stdout);
    Graph * sg = new Graph();
    
    g->buildSgFromNodes(appDensestSg, sg);
    sg->initHash();

    SCT sct(sg, k);

    sct.enumerate();

    delete sg;

    return 1.0 * sct.getCntClique() / appDensestSg.size();
}

double ccpath::realDensityInCore() {
    Graph * sg = new Graph();
    
    ui stUOfMaxCore = 0;
    for(ui u = 0; u < g->n; u++) {
        if(g->pIdx[u+1] - g->pIdx2[u] == g->coreNumber) {
            stUOfMaxCore = u;
            break;
        }
    }
    std::vector<ui> coreVs(g->n - stUOfMaxCore);
    for(ui i = stUOfMaxCore; i < g->n; i++) {
        coreVs[i - stUOfMaxCore] = i;
    }
    g->buildSgFromNodes(coreVs, sg);
    sg->initHash();

    SCT sct(sg, k);
    sct.enumerate();
    delete sg;

    return 1.0 * sct.getCntClique() / coreVs.size();
}

void ccpath::printAppSg() {
    for(ui i = 0; i < appDensestSg.size(); i++) {
        std::cout << appDensestSg[i] << ' ';
    }
    std::cout << std::endl;
}

