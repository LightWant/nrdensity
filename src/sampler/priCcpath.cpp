#include "priCcpath.h"
#include <random>
#include <algorithm>
#include <iostream>
#include <cassert>
#include "../kcliqueEnumerator/sct.h"

double priCcpath::realEdgeDensityInCore() {
    ui edges = g->m / 2;
    for(ui u = 0; u < g->n; u++) {
        if(g->pIdx[u+1] - g->pIdx2[u] == g->coreNumber) {
            return 1.0 * edges / (g->n - u);
        }
        edges -= g->pIdx[u+1] - g->pIdx2[u];
    }
    return 0.0;
}

double priCcpath::appDensity() {
    // ui stUOfMaxCore = 0;
    // for(ui u = 0; u < g->n; u++) {
    //     if(g->pIdx[u+1] - g->pIdx2[u] == ui(g->coreNumber*0.9)) {
    //         stUOfMaxCore = u;
    //         break;
    //     }
    // }

    double rcore = realEdgeDensityInCore();
std::cout << "edgeDensity:" << rcore << std::endl;
    ui stUOfMaxCore = 0;
    for(ui u = 0; u < g->n; u++) {
        if(g->pIdx[u+1] - g->pIdx2[u] >= rcore) {
            stUOfMaxCore = u;
            break;
        }
    }

    // stUOfMaxCore = g->n - 2000;
    // if(stUOfMaxCore + g->n/100 > g->n) {
    //     stUOfMaxCore = g->n - g->n/100;
    // }
    for(ui u = stUOfMaxCore; u < g->n; u++) {
        vis[u] = true;
    }
    ui coreSize = g->n - stUOfMaxCore;
    nodes.resize(coreSize);
    nodes.clear();
    for(ui u = stUOfMaxCore; u < g->n; u++) {
        nodes.push_back(u);
    }

    // ui coreSize = 0;
    // nodes.clear();
    // std::vector<bool> ok(g->n);
    // for(ui u = 0; u < g->n; u++) {
    //     ui deg = 0;
    //     for(ui i = g->pIdx2[u]; i < g->pIdx[u+1]; i++) {
    //         ok[g->pEdge[i]] = true;
    //     }

    //     for(ui i = g->pIdx2[u]; i < g->pIdx[u+1]; i++) {
    //         ui v = g->pEdge[i];
    //         for(ui j = g->pIdx2[v]; j < g->pIdx[v+1]; j++) {
    //             if(ok[g->pEdge[j]]) deg++;
    //         }
    //     }

    //     for(ui i = g->pIdx2[u]; i < g->pIdx[u+1]; i++) {
    //         ok[g->pEdge[i]] = false;
    //     }

    //     if(deg >= k*(g->pIdx[u+1]-g->pIdx2[u])) {
    //         coreSize++;
    //         vis[u] = true;
    //     }
    // }

    // for(ui u = g->n - 2*k; u < g->n; u++) {
    //     if(!vis[u]) {
    //         vis[u] = true;
    //         coreSize++;
    //     }
    // }
    // nodes.resize(coreSize);
    // nodes.clear();
    // for(ui u = 0; u < g->n; u++) if(vis[u]) nodes.push_back(u);


std::cout << "coreSize:" << coreSize << std::endl;

    double rho = appThroughCCpath();
    
    return rho;
}

void priCcpath::coloring() {
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

void priCcpath::reIdNodesByColors() {
    sortByColor.resize(nodes.size());
    std::vector<ui> pColor(cntOfColors + 1);
    for(ui u : nodes) pColor[colors[u] + 1]++;
    for(ui c = 1; c <= cntOfColors; c++) pColor[c] += pColor[c-1];
    for(ui u : nodes) sortByColor[ pColor[colors[u]]++ ] = u;
    for(ui i = 0; i < sortByColor.size(); i++) reId[sortByColor[i]] = i;
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

// for(ui i = 0; i < sortByColor.size(); i++) {
//     for(ui j = i + 1; j < sortByColor.size(); j++) {
//         bool f = false;
//         for(ui l = pIdx[i]; l < pIdx[i+1]; l++) {
//             if(pEdge[l] == j) {
//                 f = true;
//                 break;
//             }
//         }
// auto st = pEdge.begin() + pIdx[i];
// auto ed = pEdge.begin() + pIdx[i+1];
// assert(f == std::binary_search(st, ed, j));
//         if(g->connectHash(sortByColor[i], sortByColor[j])) {
            
// assert(f);
//         }
//         else {
// assert(!f);
//         }
//     }
// }

std::cout << "shrinkedGraphEdges:" << edges << std::endl;
}

void priCcpath::triangleCntWeightBuilder() {
    weight.resize(sortByColor.size());
    std::vector<bool> isNei(sortByColor.size());
    for(ui i = 0; i < sortByColor.size(); i++) {
        for(ui l = pIdx[i]; l < pIdx[i+1]; l++) {
            ui j = pEdge[l];
            isNei[j] = true;
        }

        for(ui l = pIdx[i]; l < pIdx[i+1]; l++) {
            ui j = pEdge[l];
            for(ui p = pIdx[j]; p < pIdx[j+1]; p++) {
                ui w = pEdge[p];
                if(isNei[w]) {
                    weight[i]++;
                    weight[j]++;
                    weight[w]++;
                }
            }
        }

        for(ui l = pIdx[i]; l < pIdx[i+1]; l++) {
            ui j = pEdge[l];
            isNei[j] = false;
        }
    }


    std::vector<ui> sortByWeight(sortByColor.size());
    for(ui i = 0; i < sortByWeight.size(); i++) {
        sortByWeight[i] = i;
    }
    std::sort(sortByWeight.begin(), sortByWeight.end(), 
        [&](ui a, ui b) { return weight[a] < weight[b]; });
    ui st = 0;
    while(weight[sortByWeight[st]] == 0 && st < sortByWeight.size()) st++;
    // for(ui i = st, j = sortByWeight.size()-1; i < j; i++, j--) {
    //     std::swap(weight[sortByWeight[i]], weight[sortByWeight[j]]);
    // }

    for(ui i = st; i < sortByWeight.size(); i++) {
        weight[i] = 1;
    }

    // ui prew = weight[sortByWeight[0]];
    // weight[sortByWeight[0]] = 1;
    // if(prew == 0) weight[sortByWeight[0]] = 0;
    // for(ui i = 1; i < sortByWeight.size(); i++) {
    //     ui j = sortByWeight[i];
    //     ui w = weight[j];
    //     if(prew == w) {
    //         weight[j] = weight[sortByWeight[i-1]];
    //     }
    //     else if(prew > 0) {
    //         weight[j] = weight[sortByWeight[i-1]] + 
    //     }
    //     // else weight[j] = 1;
    //     prew = w;
    // }

}

void priCcpath::DP() {
    ui n = sortByColor.size();
    for(ui i = 0; i <= k; i++) {
        if(dp[i].size() < n) {
            dp[i].resize(n);
        }
    }

    for(ui i = 0; i < n; i++) dp[0][i] = weight[i];
    for(ui i = 1; i < k; i++) {
        for(ui j = 0; j < n; j++) {
            dp[i][j] = 0;
            for(ui l = pIdx[j]; l < pIdx[j+1]; l++) {
                dp[i][j] += weight[j] + dp[i-1][pEdge[l]];
            }
        }
    }

    cntCCPathWeight = 0;
    for(ui i = 0; i < n; i++) cntCCPathWeight += dp[k-1][i];
std::cout << "cntCCPath(weighted):" << cntCCPathWeight << std::endl;
}

void priCcpath::DPForTotalPathCnt() {
    ui n = sortByColor.size();
    for(ui i = 0; i <= k; i++) {
        if(dp[i].size() < n) {
            dp[i].resize(n);
        }
    }

    for(ui i = 0; i < n; i++) dp[0][i] = 1;
    for(ui i = 1; i < k; i++) {
        for(ui j = 0; j < n; j++) {
            dp[i][j] = 0;
            for(ui l = pIdx[j]; l < pIdx[j+1]; l++) {
                dp[i][j] += dp[i-1][pEdge[l]];
            }
        }
    }

    cntCCPath = 0;
    for(ui i = 0; i < n; i++) cntCCPath += dp[k-1][i];
std::cout << "cntCCPath:" << cntCCPath << std::endl;
}

ull priCcpath::DPg() {
    ui n = sortByColor.size();
    for(ui i = 0; i < n; i++) dp[0][i] = 1;
    for(ui i = 1; i < k; i++) {
        for(ui j = 0; j < n; j++) if(isg[j]) {
            dp[i][j] = 0;
            for(ui l = pIdx[j]; l < pIdx[j+1]; l++) if(isg[pEdge[l]]) {
                dp[i][j] += dp[i-1][pEdge[l]];
            }
        }
    }

    ull ansg = 0;
    for(ui i = 0; i < n; i++) if(isg[i]) ansg += dp[k-1][i];

    return ansg;
}


double priCcpath::sample() {
    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_real_distribution<double> uiDistribution(0, 1);
    
    std::vector<ui> bf(k);
    auto sampleOneTime = [&](ui u, double & toTalZ, double & toTalP) {
        bf[0] = u;
        for(ui i = 1; i < k; i++) {
            double sumD = 0.0;
            for(ui j = pIdx[u]; j < pIdx[u + 1]; j++) {
                sumD += dp[k - i - 1][pEdge[j]];
            }
// std::cout << sumD << ' ';
            double x = uiDistribution(generator);
            double sumTmp = 0.0;
// bool choosed = false;
            for(ui j = pIdx[u]; j < pIdx[u + 1]; j++) {
                sumTmp += dp[k - i - 1][pEdge[j]];
                if(sumTmp + 1e-10 >= x * sumD) {
                    u = bf[i] = pEdge[j];
// choosed = true;
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
        for(ui i = 0; i < k; i++) sampledCCPaths.push_back(bf[i]);
        ui sumW = 0;
        for(ui i = 0; i < k; i++) sumW += weight[bf[i]];
        toTalP += 1.0 / sumW;

        for(ui i = 0; i < k; i++) {
            for(ui j = i + 1; j < k; j++) {
                // if(!g->connectHash(sortByColor[bf[i]], sortByColor[bf[j]])) return 0;
                if(!connect(bf[i], bf[j])) {
// assert(!g->connectHash(sortByColor[bf[i]], sortByColor[bf[j]]));
                    // isClique[sampledCCPaths.size()/k] = false;
                    isClique.push_back(false);
                    return 0;
                }
            }
        }

        toTalZ += 1.0 / sumW;

        // isClique[sampledCCPaths.size()/k-1] = true;
        isClique.push_back(true);
        sampledCliques.push_back(sampledCCPaths.size()/k-1);

        return 1;
    };

    ui sampledT = 0;
    double toTalZ = 0;
    ui toTalHittedClique = 0;
    double toTalP = 0;
    sampledCCPaths.clear();
    sampledCliques.clear();
    isClique.clear();
    
    for(ui i = 0; i < sortByColor.size(); i++) {
// if(i % 500 == 0) {
//     std::cout << "sampling:" << i << '/' << sortByColor.size() << std::endl;
// }
        ui t = std::round(1.0 * T * dp[k-1][i] / cntCCPathWeight);
        if(t == 0) continue;
        sampledT += t;
        
        ui z = 0;
        for(ui j = 0; j < t; j++) {
            z += sampleOneTime(i, toTalZ, toTalP);
        }
        toTalHittedClique += z;
    }
std::cout << "sampledT:" << sampledT << std::endl;
std::cout << "toTalZ:" << toTalZ << ' ' << sampledCliques.size() << std::endl;
std::cout << "toTalP:" << toTalP << std::endl;
std::cout << "toTalZ/toTalP:" << toTalZ/toTalP << std::endl;
std::cout << "toTalHittedClique:" << toTalHittedClique << std::endl;
    double estimateCntClique = cntCCPath * toTalZ / toTalP;
std::cout << "estimateCntClique:" << estimateCntClique << std::endl;

    //kclist++
    r.resize(sortByColor.size());
    for(ui i = 0; i < sortByColor.size(); i++) r[i] = 0;
    for(ui t = 0; t < 20; t++) {
        for(ui i : sampledCliques) {
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
for(ui i = 0; i < 20; i++) std::cout << r[sortByR[i]] << ' ';
std::cout << '\n';


    std::vector<ui> rPava(sortByColor.size());
    for(ui i : sampledCliques) {
        ui minU = level[sampledCCPaths[i*k]];
        for(ui j = 1; j < k; j++) {
            if(level[sampledCCPaths[i*k+j]] > minU) 
                minU = level[sampledCCPaths[i*k+j]]; 
        }
        rPava[minU] += 1;
    }
// for(ui i = 0; i < 50; i++) std::cout << rPava[i] << ' ';
// std::cout << '\n';

    constexpr int maintainMax = 5;
    double sumRhoMax[maintainMax];
    for(int i = 0; i < maintainMax; i++) sumRhoMax[i] = 0.0;
    ui sizeDensestSubgraph[maintainMax] = {0};
    ull sum = 0;
    for (ui i = 0; i < sortByR.size(); ++i) {
        sum += rPava[i];
        if ((double)sum / (i + 1) > sumRhoMax[0]) {
            // sumRho = (double)sum / (i + 1);
            // sizeDensestSubgraph = i + 1;
            sumRhoMax[0] = (double)sum / (i + 1);
            sizeDensestSubgraph[0] = i + 1;

            for(int j = 1; j < maintainMax; j++) {
                if(sumRhoMax[j-1] > sumRhoMax[j]) {
                    std::swap(sumRhoMax[j-1], sumRhoMax[j]);
                    std::swap(sizeDensestSubgraph[j-1], sizeDensestSubgraph[j]);
                }
                else break;
            }
// for(int j = 0; j+1 < maintainMax; j++) {
// assert(sumRhoMax[j] <= sumRhoMax[j+1]);
// }
        }
    }
// assert(sum > 0);
// for(int j = 0; j < maintainMax; j++) {
// std::cout << sumRhoMax[j] << ' ' << sizeDensestSubgraph[j] << std::endl;
// }
    ui maxSizeDensestSubgraph = sizeDensestSubgraph[0];
    double maxAppD = 0.0;
    ui maxtg = 0;
    ui maxz = 0;
    ull maxpg = 0;

    isg.resize(sortByColor.size());
    for (ui i = 0; i < isg.size(); ++i) isg[i] = false;

    int maxNowIdx = 0;
    for(int nowIdx = 0; nowIdx < maintainMax; nowIdx++) {
        for(ui i = 0; i < sizeDensestSubgraph[nowIdx]; i++) {
            isg[sortByR[i]] = true;
        }

        //g:sortByR[0, sizeDensestSubgraph)
        ui tg = 0;
        ui z = 0;
        for(ui i = 0; i < sampledT; i++) {
            bool isIng = true;
            for(ui j = 0; j < k; j++) {
                if(!isg[sampledCCPaths[i*k + j]]) {
                    isIng = false;
                    break;
                }
            }
            if(isIng) {
                tg++;
                if(isClique[i]) z++;
            }
        }

        ull pg = DPg();

        double appD = 1.0*pg*z/tg/sizeDensestSubgraph[nowIdx];
std::cout << "appD_nowIdx:" << nowIdx << 
            " appD:"<< appD << 
            " tg:"<<tg<<
            " pg:"<<pg<<
            " z:"<<z<<
            " sizeDensestSubgraph:" << sizeDensestSubgraph[nowIdx]
            << std::endl;
        if(appD > maxAppD) {
            maxNowIdx = nowIdx;
            maxAppD = appD;
            maxtg = tg;
            maxz = z;
            maxpg = pg;
            maxSizeDensestSubgraph = sizeDensestSubgraph[nowIdx];
        }

        for(ui i = 0; i < sizeDensestSubgraph[nowIdx]; i++) {
            isg[sortByR[i]] = false;
        }
    }
    
std::cout << "sizeDensestSubgraph:" << maxSizeDensestSubgraph << std::endl;
std::cout << "tg:" << maxtg << std::endl;
std::cout << "z:" << maxz << std::endl;
std::cout << "pg:" << maxpg << std::endl;
std::cout << "appD:" << maxAppD << std::endl;

    appDensestSg.resize(sizeDensestSubgraph[maxNowIdx]);
    for(ui i = 0; i < sizeDensestSubgraph[maxNowIdx]; i++) {
        appDensestSg[i] = sortByColor[sortByR[i]];
    }
for(ui i = 0; i < 5; i++) std::cout << sortByColor[sortByR[i]] << ' ';
std::cout << '\n';
    return maxAppD;
}

double priCcpath::appThroughCCpath() {
    coloring();
    reIdNodesByColors();
    DPForTotalPathCnt();
    triangleCntWeightBuilder();
    DP();

    if(cntCCPath < T) {
        printf("sparse graph!\n");
        T = cntCCPath;
        // return 0.0;
    }
    return sample();
}

double priCcpath::realDensityInAppSg() {
// printf("appSg %u\n", appDensestSg.size());fflush(stdout);
// for(ui i = 0; i < 10; i++) printf("%u ", appDensestSg[i]);
// printf("\n"); fflush(stdout);
    Graph * sg = new Graph();
    
    g->buildSgFromNodes(appDensestSg, sg);
    // sg->initHash();

    SCT sct(sg, k);
    sct.enumerate();
    delete sg;

    return 1.0 * sct.getCntClique() / appDensestSg.size();
}

double priCcpath::realDensityInCore() {
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
    // sg->initHash();

    SCT sct(sg, k);
    sct.enumerate();
    delete sg;

    return 1.0 * sct.getCntClique() / coreVs.size();
}

void priCcpath::printAppSg() {
    for(ui i = 0; i < appDensestSg.size(); i++) {
        std::cout << appDensestSg[i] << ' ';
    }
    std::cout << std::endl;
}