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

void ccpath::DP() {
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

void ccpath::localPathCnt() {
    std::vector<ui> pIdx(nodes.size() + 1);
    for(ui i = 0; i <= nodes.size(); i++) pIdx[i] = 0;
    ui edges = 0;
    for(ui i = 0; i < nodes.size(); i++) {
        ui u = nodes[i];
        for(ui j = g->pIdx2[u]; j < g->pIdx[u+1]; j++) {
            ui v = g->pEdge[j];
            if(!vis[v]) continue;
            edges++;

            if(reId[v] < reId[u]) pIdx[reId[u]+1]++;
            else pIdx[reId[v]+1]++;
        }
    }
    
    for(ui i = 1; i <= nodes.size(); i++) pIdx[i] += pIdx[i-1];
    std::vector<ui> pEdge(edges);
    for(ui i = 0; i < nodes.size(); i++) {
        ui u = nodes[i];
        for(ui j = g->pIdx2[u]; j < g->pIdx[u+1]; j++) {
            ui v = g->pEdge[j];
            if(!vis[v]) continue;

            if(reId[v] > reId[u]) pEdge[ pIdx[reId[v]]++ ] = reId[u];
            else pEdge[ pIdx[reId[u]]++ ] = reId[v];
        }
    }
    for(ui i = nodes.size(); i > 0; i--) pIdx[i] = pIdx[i-1];
    pIdx[0] = 0;
    for(ui i = 0; i < sortByColor.size(); i++) {
        std::sort(pEdge.begin() + pIdx[i], pEdge.begin() + pIdx[i+1]);
    }

//dpReverse
    ui n = sortByColor.size();
    dpReverse.resize(k+1);
    for(ui i = 0; i <= k; i++) {
        if(dpReverse[i].size() < n) {
            dpReverse[i].resize(n);
        }
    }

    for(ui i = 0; i < n; i++) dpReverse[0][i] = 1;
    for(ui i = 1; i < k; i++) {
        for(ui j = 0; j < n; j++) {
            dpReverse[i][j] = 0;
            for(ui l = pIdx[j]; l < pIdx[j+1]; l++) {
                dpReverse[i][j] += dpReverse[i-1][pEdge[l]];
            }
        }
    }

    cntCCPathPerNode.resize(n);
    for(ui i = 0; i < n; i++) {
        for(ui j = 0; j < k; j++)
            cntCCPathPerNode[i] += dpReverse[j][i] * dp[k-j-1][i];
    }
}

ull ccpath::DPg() {
    ui n = sortByColor.size();
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


double ccpath::sample() {
    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_real_distribution<double> uiDistribution(0, 1);
    
    std::vector<ui> bf(k);
    auto sampleOneTime = [&](ui u) -> int {
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

        // isClique[sampledCCPaths.size()/k-1] = true;
        isClique.push_back(true);
        sampledCliques.push_back(sampledCCPaths.size()/k-1);

        return 1;
    };

    ui sampledT = 0;
    ui toTalZ = 0;
    sampledCCPaths.clear();
    sampledCliques.clear();
    isClique.clear();

    for(ui i = 0; i < sortByColor.size(); i++) {
// if(i % 500 == 0) {
//     std::cout << "sampling:" << i << '/' << sortByColor.size() << std::endl;
// }
        ui t = std::round(1.0 * T * dp[k-1][i] / cntCCPath+1e-12);
        if(t == 0) continue;
        sampledT += t;
        
        ui z = 0;
        for(ui j = 0; j < t; j++) {
            z += sampleOneTime(i);
        }
        toTalZ += z;
    }
std::cout << "sampledT:" << sampledT << std::endl;
std::cout << "toTalZ:" << toTalZ << ' ' << sampledCliques.size() << std::endl;

    double estimateCntClique = 1.0*cntCCPath*toTalZ/sampledT;
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
// for(ui i = 0; i < 50; i++) std::cout << r[sortByR[i]] << ' ';
// std::cout << '\n';

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

// ui groundTh[]={
// 1156284 , 1156553 , 1156566 , 1156660 , 1156663 , 1156664 , 
// 1156671 , 1156673 , 1156771 , 1156776 , 1156778 , 1156785 ,
//  1156792 , 1156793 , 1156799 , 1156800 , 1156802 , 1156803 ,
//   1156804 , 1156806 , 1156807 , 1156809 , 1156810 , 1156811 
//   , 1156812 , 1156814 , 1156815 , 1156817 , 1156818 , 
//   1156819 , 1156820 , 1156821 , 1156822 , 1156823 , 1156826 
//   , 1156827 , 1156828 , 1156829 , 1156831 , 1156832 , 
//   1156834 , 1156835 , 1156836 , 1156838 , 1156839 , 
//   1156840 , 1156841 , 1156842 , 1156843 , 1156845 , 
//   1156847 , 1156848 , 1156849 , 1156850 , 1156851 ,
//    1156852 , 1156855 , 1156856 , 1156857 , 1156858 , 
//    1156859 , 1156860 , 1156861 , 1156862 , 1156863 , 
//    1156864 , 1156865 , 1156866 , 1156867 , 1156868 , 
//    1156869 , 1156870 , 1156871 , 1156872 , 1156873 , 1156874
//     , 1156876 , 1156877 , 1156878 , 1156879 , 1156880 , 
//     1156881 , 1156882 , 1156883 , 1156884 , 1156885 , 1156886
//      , 1156887 , 1156888 , 1156889 , 1156891 , 1156892 , 
//      1156893 , 1156894 , 1156895 , 1156896 , 1156898 ,
//       1156899 , 1156901 , 1156902 , 1156903 , 1156904 , 
//       1156905 , 1156906 , 1156908 , 1156909 , 1156910 , 
//       1156911 , 1156912 , 1156913 , 1156914 , 1156915 ,
//        1156916 , 1156917 , 1156918 , 1156919 , 1156920 ,
//         1156921 , 1156922 , 1156923 , 1156925 , 1156927 , 
//         1156928 , 1156929 , 1156931 , 1157038 , 1157071 , 
//         1157078 , 1157248 , 1157390 , 1157397 , 1157412 , 
//         1157422 , 1157428 , 1157429 , 1157460 , 1157463 , 
//         1157464 , 1157466 , 1157467 , 1157474 , 1157476 , 
//         1157478 , 1157483 , 1157493 , 1157494 , 1157496 , 
//         1157497 , 1157501 , 1157506 , 1157540 , 1157541 , 
//         1157543 , 1157549 , 1157551 , 1157556 , 1157558 , 
//         1157571 , 1157576 , 1157591 , 1157600 , 1157619 , 
//         1157623 , 1157624 , 1157632 , 1157638 , 1157640 , 
//         1157641 , 1157643 , 1157645 , 1157646 , 1157648 , 
//         1157650 , 1157651 , 1157652 , 1157654 , 1157657 , 
//         1157659 , 1157660 , 1157663 , 1157664 , 1157665 , 
//         1157666 , 1157668 , 1157670 , 1157671 , 1157672 , 
//         1157673 , 1157674 , 1157675 , 1157676 , 1157677 , 
//         1157678 , 1157679 , 1157680 , 1157681 , 1157683 , 
//         1157684 , 1157694 , 1157695 , 1157696 , 1157703 , 
//         1157708 , 1157710 , 1157711 , 1157714 , 1157715 , 
//         1157716 , 1157717 , 1157720 , 1157721 , 1157722 , 
//         1157727 , 1157732 , 1157733 , 1157735 , 1157736 , 
//         1157738 , 1157743 , 1157745 , 1157746 , 1157747 , 
//         1157749 , 1157750 , 1157751 , 1157752 , 1157754 , 
//         1157756 , 1157758 , 1157759 , 1157760 , 1157761 , 
//         1157762 , 1157763 , 1157764 , 1157765 , 1157767 , 
//         1157768 , 1157769 , 1157770 , 1157771 , 1157772 , 
//         1157773 , 1157774 , 1157775 , 1157776 , 1157777 , 
//         1157778 , 1157779 , 1157780 , 1157781 , 1157782 , 
//         1157783 , 1157784 , 1157785 , 1157786 , 1157787 , 
//         1157788 , 1157789 , 1157790 , 1157791 , 1157792 , 
//         1157793 , 1157794 , 1157795 , 1157796 , 1157797 , 
//         1157798 , 1157799 , 1157800 , 1157801 , 1157802 , 
//         1157803 , 1157804 , 1157805 , 1157806 , 1157807 , 
//         1157808 , 1157809 , 1157810 , 1157811 , 1157812 , 
//         1157813 , 1157814 , 1157815 , 1157816 , 1157817 , 
//         1157818 , 1157819 , 1157820 , 1157821 , 1157822 , 
//         1157823 , 1157824 , 1157825 , 1157826 };
// const ui ssz = 296;
// ui reIds[ssz] = {0};
// std::cout << ssz << std::endl;
// for(ui i = 0; i < ssz; i++) {
//     for(ui j = 0; j < sortByColor.size(); j++) {
//         if(sortByColor[j] == groundTh[i]) {
//             reIds[i] = j;
//             break;
//         }
//     }
// }
// std::vector<ui> lcTMP(sortByColor.size());
// for(ui i : sampledCliques) {
//     for(ui j = 0; j < k; j++) {
//         lcTMP[sampledCCPaths[i*k+j]]++;
//     }
// }
// std::cout << "here" << std::endl;
// localPathCnt();
// ull sumTMp = 0;
// for(ui i = 0; i < sortByColor.size(); i++) {
//     sumTMp += cntCCPathPerNode[i];
// }
// assert(sumTMp/k == cntCCPath);
// for(ui i = 0; i < ssz; i++) {
//     printf("%u, %u, %u, %u, %u, %llu, sampleSize %u\n", 
//     groundTh[i], reIds[i], 
//     r[reIds[i]], lcTMP[reIds[i]],
//     cntCCPathPerNode[reIds[i]], dp[k-1][reIds[i]],
// (ui)std::round(1.0 * T * dp[k-1][reIds[i]] / cntCCPath+1e-12));
// }


    constexpr int maintainMax = 1;
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
std::cout << "appD_nowIdx_" << nowIdx << ':'<< appD << 
            " tg:"<<tg<<
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

    return maxAppD;
}

double ccpath::sampleV2() {//only store sampled cliques
    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_real_distribution<double> uiDistribution(0, 1);
    
    std::vector<ui> bf(k);
    auto sampleOneTime = [&](ui u) -> int {
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
        
        for(ui i = 0; i < k; i++) {
            for(ui j = i + 1; j < k; j++) {
                // if(!g->connectHash(sortByColor[bf[i]], sortByColor[bf[j]])) return 0;
                if(!connect(bf[i], bf[j])) {
// assert(!g->connectHash(sortByColor[bf[i]], sortByColor[bf[j]]));
                    // isClique[sampledCCPaths.size()/k] = false;
                    // isClique.push_back(false);
                    return 0;
                }
            }
        }
        for(ui i = 0; i < k; i++) sampledCCPaths.push_back(bf[i]);

        return 1;
    };

    ui sampledT = 0;
    ui toTalZ = 0;
    sampledCCPaths.clear();
    // sampledCliques.clear();
    // isClique.clear();

    for(ui i = 0; i < sortByColor.size(); i++) {
// if(i % 500 == 0) {
//     std::cout << "sampling:" << i << '/' << sortByColor.size() << std::endl;
// }
        ui t = std::round(1.0 * T * dp[k-1][i] / cntCCPath+1e-12);
        if(t == 0) continue;
        sampledT += t;
        
        ui z = 0;
        for(ui j = 0; j < t; j++) {
            z += sampleOneTime(i);
        }
        toTalZ += z;
    }
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
    DP();

    if(cntCCPath < T) {
        printf("sparse graph!\n");
        T = cntCCPath;
        // return 0.0;
    }

    if(biased) return biasedSample();
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



double ccpath::biasedSample() {
    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_real_distribution<double> uiDistribution(0, 1);
    
    std::vector<ui> bf(k);
    auto sampleOneTime = [&](ui u) -> int {
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

        // isClique[sampledCCPaths.size()/k-1] = true;
        isClique.push_back(true);
        sampledCliques.push_back(sampledCCPaths.size()/k-1);

        return 1;
    };

    ui sampledT = 0;
    ui toTalZ = 0;
    sampledCCPaths.clear();
    sampledCliques.clear();
    isClique.clear();

    localPathCnt();
printf("here\n");fflush(stdout);
ull sumTmp = 0;
for(ui i = 0; i < sortByColor.size(); i++) {
    sumTmp += cntCCPathPerNode[i];
}
assert(sumTmp/k == cntCCPath);
    
    double estimateCntClique = 0.0;
    for(ui i = 0; i < sortByColor.size(); i++) {
// if(i % 500 == 0) {
//     std::cout << "sampling:" << i << '/' << sortByColor.size() << std::endl;
// }
        // ui t = std::round(0.5*T*cntCCPathPerNode[i]/sumTmp + 0.5*T*dp[k-1][i]/cntCCPath +1e-12);
        ui t = std::round(1.0*T*dp[k-1][i]/cntCCPath +1e-12);
        if(t == 0) continue;
        sampledT += t;
        
        ui z = 0;
        for(ui j = 0; j < t; j++) {
            z += sampleOneTime(i);
        }
        toTalZ += z;
        if(z > 0)
            estimateCntClique += 1.0*dp[k-1][i]*z/t;
    }
std::cout << "sampledT:" << sampledT << std::endl;
std::cout << "toTalZ:" << toTalZ << ' ' << sampledCliques.size() << std::endl;

    // = 1.0*cntCCPath*toTalZ/sampledT;
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
for(ui i = 0; i < 50; i++) std::cout << r[sortByR[i]] << ' ';
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

    constexpr int maintainMax = 1;
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
std::cout << "appD_nowIdx_" << nowIdx << ':'<< appD << 
            " tg:"<<tg<<
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

    return maxAppD;
}