#include "../graph/graph.h"
#include "../tools/getArgs.hpp"
#include "../kcliqueEnumerator/kclist.h"
// #include "../kcliqueEnumerator/enumerator.h"
#include "../tools/comb.hpp"
#include <iostream>
#include <chrono>
#include <algorithm>
#include <iomanip>
#include <queue>

void printUsage() {
    std::cout << "-f graph file directory(edge.bin & idx.bin)" << std::endl;
    std::cout << "-f_txt graph file text file, each edge exists one time" << std::endl;
    std::cout << "-f_txtD graph file text file, each edge exists two times" << std::endl;
    std::cout << "-k clique size" << std::endl;
    std::cout << "-w fix watering weight" << std::endl;
    // std::cout << "-p xxx  previous loops, 0 in default" << std::endl;
    std::cout << "-flow, false in default" << std::endl;
    std::cout << "-s, shrink graph, true in default" << std::endl;
}

int main(int argc, char * argv[]) {
    argsController ac(argc, argv);

    if(!ac.exist("-f_txt") && !ac.exist("-f") && !ac.exist("-f_txtD")) {
        printUsage();
        return 0;
    }

    bool noVUM = false;
    if(ac.exist("noUVM")) noVUM = true;
    Graph g;
    if(ac.exist("-f_txt")) {
        g.readFromText(ac["-f_txt"], noVUM);
    }
    else if(ac.exist("-f_txtD")) g.readFromTextDoubleEdges(ac["-f_txtD"]);
    else g.readFromBin(ac["-f"]);

    std::cout << "load graph: n " << g.n << " m " << g.m << " maxD " << g.maxD << std::endl;

    auto s1 = std::chrono::steady_clock::now();
    g.changeToCoreOrder();
    auto s2 = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(s2 - s1);
    std::cout << "changeToCoreOrder:" << duration.count() << "ms" << std::endl;
    std::cout << "coreNumber:" << g.coreNumber << std::endl;

    // g.initHash();

    int k = 4;
    if(ac.exist("-k")) k =  std::stoi(ac["-k"]);
    std::cout << "k:" << k << std::endl;

    cliqueE_kClist * kcl = new cliqueE_kClist(&g, k);
    kcl->enumerate();

    std::cout << "totalCntClique:" << kcl->getCntClique() << std::endl;

    auto t1 = std::chrono::steady_clock::now();

//line 1 of Alg6
    using ll = long long;
    std::vector<ll> gamma(g.n);
    comb cb;
    for(ui i = 0, x = 0; i < g.n; i++) {
        if(g.pIdx[i+1]-g.pIdx2[i] > x) {
            x = g.pIdx[i+1]-g.pIdx2[i];
        }

        gamma[i] = (ll)cb(x, k-1);
    }
//line 2 of Alg6
    std::vector<ll> sortedByGamma(g.n);
    for(ui i = 0; i < g.n; i++) sortedByGamma[i] = i;
    std::sort(sortedByGamma.begin(), sortedByGamma.end(), 
        [&](ui a, ui b) {
            return gamma[a] > gamma[b];
        }
    );
//line 3 of Alg6
    ui kmax = 0;
    std::vector<ui> S;
    std::vector<ui> W;
    std::vector<bool> inW(g.n);

    W.push_back(sortedByGamma[0]);
    inW[sortedByGamma[0]] = true;
    
    ll maxGammaVsetminusW = 0;
    for(ui i = 1; i < g.n; i++) {
        if(gamma[sortedByGamma[i]] == gamma[sortedByGamma[0]]) {
            W.push_back(sortedByGamma[i]);
            inW[sortedByGamma[i]] = true;
        }
        else {
            maxGammaVsetminusW = gamma[sortedByGamma[i]];
            break;
        }
    }
    ui preSizeOfW = W.size();
//line 4 of Alg6
    
    std::vector<ui> cd(g.n);
    std::vector<ui> cliquesInW;
    using itr = std::vector<ui>::iterator;
    kcl->scan([&cliquesInW, &inW](const itr & st, const itr & ed) {
        for(itr i = st; i != ed; i++) {
            if(inW[*i] == false) return;
        }
        for(itr i = st; i != ed; i++) {
            cliquesInW.push_back(*i);
        }
    });

    while(maxGammaVsetminusW >= kmax) {
std::cout << "W:" << W.size() << std::endl;
std::cout << "maxGammaVsetminusW:" << maxGammaVsetminusW << std::endl;
std::cout << "kmax:" << kmax << std::endl;
//line 5
        for(ui v : W) cd[v] = 0;
        for(ui i = 0; i < cliquesInW.size(); i++) {
            cd[cliquesInW[i]]++;
        }
//line 6
        ui kl = cd[W[0]], ku = cd[W[0]];
        for(ui v : W) {
            kl = std::min(kl, cd[v]);
            ku = std::max(ku, cd[v]);
        }
//line 7
        ui nowK = std::max(kl, kmax+1);
//line 8

        while(nowK <= ku && W.size() > 0) {
std::cout << "nowK:" << nowK << ' '<<kl<<' '<<ku 
    << ' '<<cliquesInW.size()/k<<' ' << W.size() << std::endl;
//line 9~10
            std::queue<ui> que;
            for(ui v : W) if(cd[v] < nowK) {
                que.push(v);
            }
            ui cntToDelete = 0;

            if(que.empty()) {
                ui minDeg = cd[W[0]];
                for(ui v : W) if(cd[v] < minDeg) {
                    minDeg = cd[v];
                }
                kmax = std::max(kmax, minDeg);
                nowK = minDeg+1;
                for(ui v : W) if(cd[v] < nowK) {
                    que.push(v);
                }
            }

            while(!que.empty()) {
                ui v = que.front(); que.pop();
                if(!inW[v]) continue;

                for(ui i = 0, ed = cliquesInW.size()/k; i < ed; i++) {
                    bool f = true;
                    bool hasV = false;
                    for(ui j = 0; j < k; j++) {
                        if(v == cliquesInW[i*k + j]) {
                            hasV = true;
                        }
                        if(!inW[cliquesInW[i*k + j]]) {
                            f = false; break;
                        }
                    }
                    if(!f || !hasV) continue;

                    for(ui j = 0; j < k; j++) {
                        --cd[cliquesInW[i*k + j]];
                        if(cd[cliquesInW[i*k + j]] < nowK) {
                            que.push(v);
                        }
                    }
                }

                inW[v] = false;
                cntToDelete++;
            }
//line 11
            if(cntToDelete == W.size()) {
                // W.clear();
                break;
            }
//maintain W   
            std::vector<ui> tmpW(W.size() - cntToDelete);
            tmpW.clear();
            for(ui v : W) if(inW[v]) tmpW.push_back(v);
            W.clear();
            for(ui v : tmpW) W.push_back(v);
//line 12
            if(nowK > kmax) {
                kmax = nowK;
                S.clear();
                for(ui v : W) S.push_back(v);
            }
            nowK++;
//maintain cd, cliquesInW
            std::vector<ui> tmpCliquesInW;
            for(ui i = 0, ed = cliquesInW.size()/k; i < ed; i++) {
                bool f = true;
                for(ui j = 0; j < k; j++) {
                    if(!inW[cliquesInW[i*k + j]]) {
                        f = false; break;
                    }
                }
                if(!f) continue;
                for(ui j = 0; j < k; j++) {
                    tmpCliquesInW.push_back(cliquesInW[i*k + j]);
                }
            }
            cliquesInW.clear();
            for(ui v : tmpCliquesInW) cliquesInW.push_back(v);
            for(ui v : W) cd[v] = 0;
            for(ui v : cliquesInW) cd[v]++;
        }
//line 15
        preSizeOfW = std::min(preSizeOfW*2, g.n);
        for(ui v : W) inW[v] = false;
        W.clear();
        for(ui i = 0; i < preSizeOfW; i++) {
            W.push_back(sortedByGamma[i]);
            inW[sortedByGamma[i]] = true;
        }
        if(preSizeOfW+1 < g.n) {
            maxGammaVsetminusW = gamma[sortedByGamma[preSizeOfW+1]];
        }
        else maxGammaVsetminusW = 0;
        cliquesInW.clear();
        kcl->scan([&cliquesInW, &inW](const itr & st, const itr & ed) {
            for(itr i = st; i != ed; i++) {
                if(inW[*i] == false) return;
            }
            for(itr i = st; i != ed; i++) {
                cliquesInW.push_back(*i);
            }
        });
    }

    std::vector<bool> & inS = inW;
    for(ui i = 0; i < g.n; i++) inS[i] = false;
    for(ui i : S) inS[i] = true;
    std::vector<ui> & cliquesInS = cliquesInW;
    cliquesInS.clear();
    kcl->scan([&cliquesInS, &inS](const itr & st, const itr & ed) {
        for(itr i = st; i != ed; i++) {
            if(inS[*i] == false) return;
        }
        for(itr i = st; i != ed; i++) {
            cliquesInS.push_back(*i);
        }
    });

    std::cout << "cntClique:" << cliquesInS.size() << std::endl;
    std::cout << "size:" << S.size() << std::endl;
    std::cout << "density:" << 1.0*cliquesInS.size()/k/S.size() << std::endl;
    // cliqueE_kClist * kcl = nullptr;
    // kcl = new cliqueE_kClist(&g, k);
    // kcl->enumerate();
    // auto t2 = std::chrono::steady_clock::now();
    // auto durationOfEnumeration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    // std::cout << "enumerateTime:" << durationOfEnumeration.count() << "ms" << std::endl;
    // std::cout << "cntClique:" << kcl->getCntClique() << std::endl;

    // delete kcl;

    return 0;
}