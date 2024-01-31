#include "../tools/getArgs.hpp"
#include "../tools/types.hpp"
#include "../graph/graph.h"
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <algorithm>
#include <cmath>

std::random_device rd;
std::default_random_engine generator(rd());

std::vector<ui> Get2HopThenChooseFixed(Graph * g, ui stU, ui cnt, std::vector<bool> & vis) {
    std::vector<ui> pools;
    for(ui i = g->pIdx[stU]; i < g->pIdx[stU+1]; i++) {
        ui u = g->pEdge[i];
        for(ui j = g->pIdx[u]; j < g->pIdx[u+1]; j++) {
            ui v = g->pEdge[j];
            if(vis[v]) continue;
            pools.push_back(v);
            vis[v] = true;
        }
    }

    for(ui v : pools) vis[v] = false;

    std::vector<ui> res(cnt);
    std::uniform_int_distribution<ui> sp(0, pools.size()-1);

    for(ui i = 0; i < cnt; i++) {
        do {
            ui v = pools[sp(generator)];
            res[i] = v;
        } while(vis[res[i]]);
        vis[res[i]] = true;
    }

    for(ui u : res) vis[u] = false;

    return res;
}

std::vector<ui> GetStepRandomWalkFixedWalks(
            Graph * g, std::vector<ui> & As, 
            ui DEF_ANCHOR_REPEATS, ui STEPS,
            std::vector<bool> & vis
    ) {
    auto GetRNodeDegreeCap = [&](ui c_max, ui N) {
        double scale = std::min(
            (double)UINT32_MAX, 
            std::max(2.0, std::log(N / c_max)/std::log(2))
        );
        return scale * c_max;
    };
    
    ui maxDegInAs = 0;
    for(ui u : As) maxDegInAs = std::max(maxDegInAs, g->degree(u));
    double rDegreeCap = GetRNodeDegreeCap(
        maxDegInAs, g->n);
    
    std::mt19937 gg(rd());
    typedef std::uniform_int_distribution<ui> distr_t;
    typedef typename distr_t::param_type param_t;
    distr_t D;
    
    std::vector<ui> res;

    for(ui v : As) {
        res.push_back(v);
        vis[v] = true;
        for(ui i = 0; i < DEF_ANCHOR_REPEATS; i++) {
            ui current = v;
            for(ui step = 0; step < STEPS; step++) {
                ui deg = g->degree(current);
                if(deg == 0) break;
                
                ui idx = D(gg, param_t(0, deg));
                current = g->pEdge[g->pIdx[current]+idx];
                if(g->degree(current) <= rDegreeCap) {
                    if(!vis[current]) {
                        res.push_back(current);
                        vis[current] = true;
                    }
                }
            }
        }
    }

    for(ui u : res) vis[u] = false;

    return res;
}

void writeTo(std::vector<ui> & V, std::string pa) {
    std::fstream fOut(pa, std::ios::out);
    std::sort(V.begin(), V.end());
    for(ui u : V) fOut << u << "\n";
    fOut.close();
}

int main(int argc, char * argv[]) {
    argsController ac(argc, argv);

    Graph g;
    if(ac.exist("-f_txt"))
        g.readFromText(ac["-f_txt"], ac.exist("noUVM"));
    else
        g.readFromBin(ac["-f"]);

    std::string notDegenerateNodesDir = ac["-nd"];
    std::string notDegenerateNodesPath = notDegenerateNodesDir + "notDegenerateNodes.txt";
// std::cout << notDegenerateNodesPath << std::endl;
    std::vector<ui> stUs;
    std::fstream finnD(notDegenerateNodesPath, std::ios::in);
    ui u;
    std::string s;
    finnD >> s;
// std::cout << s << std::endl;
    while(finnD >> u) {
        stUs.push_back(u);
    }
    finnD.close();
// printf("notDegenerateSize:%u\n", stUs.size());
    ui wklen = 10;
    if(ac.exist("-wk")) wklen = std::stoi(ac["-wk"]);
    
    std::uniform_int_distribution<ui> getIdxOfStUs(0, stUs.size()-1);
    std::uniform_int_distribution<ui> repretsRd(2, wklen);
    
    ui generateCnt = 500;
    if(ac.exist("-gcnt")) generateCnt = std::stoi(ac["-gcnt"]);

    ui generateCntR = std::min(generateCnt, ui(stUs.size() * 0.5));
    
    std::vector<bool> vis(g.n);
    
    for(ui i = 0; i < generateCntR; i++) {
        ui stNode = stUs[getIdxOfStUs(generator)];
        std::vector<ui> As = Get2HopThenChooseFixed(&g, stNode, 8, vis);

        ui DEF_ANCHOR_REPEATS = repretsRd(generator);
        ui STEPS = repretsRd(generator);
        std::vector<ui> Rs = GetStepRandomWalkFixedWalks(
            &g, As, DEF_ANCHOR_REPEATS, STEPS, vis);

        // if(Rs.size() < As.size()) continue;

        std::string fName = std::to_string(i+1)+"_"+std::to_string(stNode)+"_"+
            std::to_string(DEF_ANCHOR_REPEATS)+"_"+std::to_string(STEPS);
        writeTo(As, notDegenerateNodesDir+fName+".A");
        writeTo(Rs, notDegenerateNodesDir+fName+".R");
    }

    return 0;
}