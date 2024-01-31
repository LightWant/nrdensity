#include "../tools/getArgs.hpp"
#include "../tools/fastIO.hpp"
#include "../tools/types.hpp"
#include "../tools/HLPP.hpp"
#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <random>
#include <functional>
#include <algorithm>
#include <cassert>
#include <set>
#include <map>

/**
 * local density problem
 * replace d_S(v) with d_G(v)
 * frank wolfe algorithm
 * */ 



void printUsage1() {
    std::cout << "-f graph file directory(edge.bin & idx.bin)" << std::endl;
    std::cout << "-f_txt graph file text file, each edge exists one time" << std::endl;
   // std::cout << "-f_txtD graph file text file, each edge exists two times" << std::endl;
}

struct Edge
{
    ui u, v, w;
};


void loadGraphs(std::vector<Edge> & edges, 
    const std::function<bool(ui)> & isInR, const std::string & gPath);

void loadBinCSRGraphs(std::vector<Edge> & edges, 
    const std::function<bool(ui)> & isInR, const std::string & gPath);

void shuffleEdges(std::vector<Edge> & edges);

//check whether VS is a densest subgraph itself.
bool isDensest(std::vector<Edge> & edges,
    std::vector<ui> & R, std::set<ui> & Aset,
    std::vector<ui> & VS, ui n, double rho);

int main(int argc, char * argv[]) {
    argsController ac(argc, argv);

    if(!ac.exist("-f_txt") && !ac.exist("-f") ) {
        printUsage1();
        return 0;
    }

    bool checkByflow = false;
    if(ac.exist("-flow")) checkByflow = true;

    if(!ac.exist("-R")) {
        std::cout << "-R the file path of R" << std::endl;
        return 0;
    }

    std::vector<ui> A, R;

    if(ac.exist("-A")) {
        std::fstream finA(ac["-A"], std::ios::in);
        ui u;
        while(finA >> u) {
            A.push_back(u);
        }
        finA.close();
    }
    // A.resize(1);
    // A[0] = 4030124;
   // A[1] = 4030125;
    std::set<ui> Aset(A.begin(), A.end());
//build isInA
    auto isInA = [&](ui u) -> bool {
        return Aset.find(u) != Aset.end();
    };
// for(ui u : A) assert(isInA(u));

    std::fstream finR(ac["-R"], std::ios::in);
    ui u;
    while(finR >> u) {
        R.push_back(u);
    }
    finR.close();
    // ui ll = 4030100, rr = 4030151;
    // R.resize(rr - ll + 1);
    // for(ui i = ll; i <= rr; i++) {
    //     R[i-ll] = i;
    // }

printf("Rsz:%lu\n", R.size());
printf("Asz:%lu\n", A.size());

//build isInR
    ui maxInR = 0, minInR = UINT32_MAX;
    for(auto v : R) {
        maxInR = std::max(maxInR, v);
        minInR = std::min(minInR, v);
    }
    std::vector<ui> inR(maxInR - minInR + 1);
    for(auto v : R) {
        inR[v - minInR] = true;
    }
    auto isInR = [&](ui u) -> bool {
        return minInR <= u && u <= maxInR && inR[u - minInR];
    };
for(ui u : R) assert(isInR(u));

    ui T = 10;
    if(ac.exist("-T")) T = std::stoi(ac["-T"]);
printf("T:%u\n", T);

//read graph locally
auto t1 = std::chrono::steady_clock::now();

    std::vector<Edge> edges;
    if(ac.exist("-f_txt")) {
        loadGraphs(edges, isInR, ac["-f_txt"]);
    }
    else {
        loadBinCSRGraphs(edges, isInR, ac["-f"]);
    }
    

    shuffleEdges(edges);
printf("edgeCnt:%lu\n", edges.size());

auto t2 = std::chrono::steady_clock::now();
auto durationOfEnumeration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
// std::cout << gPath << std::endl;
std::cout << "load+shuffle:" << durationOfEnumeration.count() << "ms" << std::endl;

//get all vertices
    std::vector<ui> vs(edges.size());
    vs.clear();
    for(auto & e : edges) {
        vs.push_back(e.u);
        vs.push_back(e.v);
    }
    std::sort(vs.begin(), vs.end());
    ui vsSize = std::unique(vs.begin(), vs.end()) - vs.begin();
    vs.resize(vsSize);
    ui maxV = 0;
    for(auto v : vs) maxV = std::max(maxV, v);
printf("vsSize:%u, vs[-1]:%u\n", vsSize, vs[vsSize-1]);

//run Frank-Wolfe
    std::vector<ui> r(maxV+1);
    for(auto & v : r) v = 0;

#define BETA

    for(ui t = 0; t < T; t++) {
#ifdef BETA
        ui rMax = 0;
#endif
        for(Edge & e : edges) {
            if(r[e.u] < r[e.v]) {
#ifdef BETA
                rMax = std::max(rMax, r[e.u] + e.w);
#endif
                r[e.u] += e.w;
            }
            else if(r[e.u] > r[e.v]) {
#ifdef BETA
                rMax = std::max(rMax, r[e.v] + e.w);
#endif
                r[e.v] += e.w;
            }
            else {
#ifdef BETA
                rMax = std::max(rMax, r[e.u]+(e.w+1)/2);
#endif
                r[e.u] += (e.w+1) / 2;
                r[e.v] += e.w / 2;
            }
        }
#ifdef BETA
        for(ui u : A) {
            r[u] = rMax;
        }
#endif
    }

//run tentative
    std::vector<ui> reordered(vs.size());
    for(ui i = 0; i < reordered.size(); i++) reordered[i] = vs[i];
    std::sort(reordered.begin(), reordered.end(), 
        [&](ui a, ui b) { return r[a] > r[b]; }
    );

#ifndef BETA
//sort vertices in A in the same level
    for(ui i = 0, j = 1; j < reordered.size(); ) {
        while(j < reordered.size() && r[reordered[i]] == r[reordered[j]]) j++;

        for(ui k = i, l = i; k < j; k++) {
            if(isInA(reordered[k])) {
                std::swap(reordered[l], reordered[k]);
                l++;
            } 
        }

        i = j;
        j = j + 1;
    }
#endif
    std::vector<ui> level(maxV+1, UINT32_MAX);
    for(ui i = 0; i < reordered.size(); i++) level[reordered[i]] = i;

//rPava
    std::vector<ui> rPava(vs.size(), 0);
    for(auto e : edges) {

// printf("%u %u %lu, %u %u %lu\n", 
// level[e.u], level[e.v], rPava.size(),
// e.u, e.v, level.size());
// fflush(stdout);
        // if(r[e.u] == r[e.v]) {
        //     if(level[e.u] > level[e.v]) {
        //         rPava[level[e.u]] += (e.w+1)/2;
        //         rPava[level[e.v]] += e.w/2;
        //     }
        //     else {
        //         rPava[level[e.u]] += e.w/2;
        //         rPava[level[e.v]] += (e.w+1)/2;
        //     }
        // }
        // else 
        {
            if(level[e.u] > level[e.v]) {
                rPava[level[e.u]] += e.w;
            }
            else {
                rPava[level[e.v]] += e.w;
            }
        }
    }

    ull sum = 0;
    double rho = 0.0;
    ui sizeDensestSubgraph = 0;

    for (ui i = 0; i < reordered.size(); ++i) {
        sum += rPava[i];

        if ((double)sum / (i + 1) >= rho) {
            rho = (double)sum / (i + 1);
            sizeDensestSubgraph = i + 1;
        }
    }
// printf("here\n");fflush(stdout);
    ui inA = 0;
#ifndef BETA
    for(ui i = sizeDensestSubgraph; i < reordered.size(); ++i) {
        if(isInA(reordered[i])) inA++;
    }
#endif
    printf("szPava:%u+%u\n", sizeDensestSubgraph, inA);
    printf("rhoPava:%f %f\n", rho, rho*sizeDensestSubgraph / (sizeDensestSubgraph+inA)/2);
    printf("size:%u\n", sizeDensestSubgraph + inA);
    printf("upperOfError:%f\n", r[reordered[0]]/rho/T - 1.0);

auto t3 = std::chrono::steady_clock::now();
auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t3 - t2);
std::cout << "time:" << duration.count() << "ms" << std::endl;

//check by flow
    if(checkByflow) {
        bool res = isDensest(edges,
            R, Aset, reordered, sizeDensestSubgraph, rho);
        printf("isDensest:%d\n", (int)res);
    }

    ull ee = 0;
    for(auto e : edges) if(e.w == 2) ee += e.w;
    printf("G(R)density:%f\n", 1.0*ee/R.size());

//real density

ui d = 0;

std::vector<bool> inVs(maxV+1);
for(ui i = 0; i < sizeDensestSubgraph; i++) {
    inVs[reordered[i]] = true;
}

if(ac.exist("-f_txt")) {
    fastIO in(ac["-f_txt"].c_str(), "r");
    while(!in.empty()) {
        ui u, v;
        in.getUInt(u);
        in.getUInt(v);

        if(u > maxV || v > maxV) continue;
        if(inVs[u] && inVs[v]) {
            d++;
        }
    }
}
else {
    std::string directory = ac["-f"];
    fastIO readEdge(directory + "edge.bin", "rb");
    fastIO readIdx(directory + "idx.bin", "rb");

    //load pIdx
    ui n = readIdx.leftBytes() / sizeof(ui) - 1;
    std::vector<ui> pIdx(n + 1);
    int64_t tmp = readIdx.load(pIdx.data());
    if(tmp < 0) {
        printf("load idx.bin error:%lld\n", tmp);
        exit(-1);
    }

    ui maxD = 0;
    for(ui u = 0; u < n; u++) {
        maxD = std::max(maxD, pIdx[u + 1] - pIdx[u]);
    }

    //load pEdge
    ui m = readEdge.leftBytes() / sizeof(ui);
// printf("%u %lu, %lu %u\n", n, m, readEdge.leftBytes(), pIdx[n]);
    std::vector<ui> adj(maxD);
    for(ui u = 0; u < n; u++) {
        ui deg = pIdx[u+1]-pIdx[u];
        readEdge.load(adj.data(), deg);
        for(ui i = 0; i < deg; i++) {
            ui v = adj[i];
            if(u > maxV || v > maxV) continue;
            if(inVs[u] && inVs[v]) {
                d++;
            }
        }
    }
}

printf("realEdgeBasedDensity:%f\n", 1.0 * d / (sizeDensestSubgraph+inA));
// for(ui u : A) {
//     printf("A %u: %u %u\n", u, mp[u], mp2[u]);
// }

//print densest subgraph
std::vector<ui> tmp(sizeDensestSubgraph+inA);
tmp.clear();
for(ui i = 0; i < sizeDensestSubgraph; i++) {
    // printf("%u ", reordered[i]);
    tmp.push_back(reordered[i]);
}
#ifndef BETA
for(ui i = sizeDensestSubgraph; i < reordered.size(); ++i) {
    if(isInA(reordered[i])) tmp.push_back(reordered[i]);
}
#endif
std::sort(tmp.begin(), tmp.end());
printf("den:");
for(ui i = 0; i < sizeDensestSubgraph+inA; i++) {
    // printf("%u:%u:%u ", tmp[i], level[tmp[i]], r[tmp[i]]);
    printf("%u ", tmp[i]);
}
printf("\n");

    return 0;
}

void loadGraphs(std::vector<Edge> & edges, 
    const std::function<bool(ui)> & isInR, const std::string & fPath) {
    
    fastIO in(fPath.c_str(), "r");

    edges.clear();
   
    while(!in.empty()) {
        ui u, v;
        in.getUInt(u);
        in.getUInt(v);

        if(isInR(u)) {
            if(isInR(v)) edges.push_back(Edge{ u, v, 2 });
            else edges.push_back(Edge{ u, v, 1 });
        }
        else if(isInR(v)) {
            edges.push_back(Edge{ u, v, 1 });
        }
        
    }
}

void loadBinCSRGraphs(std::vector<Edge> & edges, 
    const std::function<bool(ui)> & isInR, const std::string & directory) {
    
    fastIO readEdge(directory + "edge.bin", "rb");
    fastIO readIdx(directory + "idx.bin", "rb");

    //load pIdx
    ui n = readIdx.leftBytes() / sizeof(ui) - 1;
    std::vector<ui> pIdx(n + 1);
    int64_t tmp = readIdx.load(pIdx.data());
    if(tmp < 0) {
        printf("load idx.bin error:%lld\n", tmp);
        exit(-1);
    }

    ui maxD = 0;
    for(ui u = 0; u < n; u++) {
        maxD = std::max(maxD, pIdx[u + 1] - pIdx[u]);
    }

    //load pEdge
    ui m = readEdge.leftBytes() / sizeof(ui);
// printf("%u %lu, %lu %u\n", n, m, readEdge.leftBytes(), pIdx[n]);
    std::vector<ui> adj(maxD);
    for(ui u = 0; u < n; u++) {
        ui deg = pIdx[u+1]-pIdx[u];
        readEdge.load(adj.data(), deg);
        for(ui i = 0; i < deg; i++) {
            ui v = adj[i];
            if(isInR(u)) {
                if(isInR(v)) edges.push_back(Edge{ u, v, 2 });
                else edges.push_back(Edge{ u, v, 1 });
            }
            else if(isInR(v)) {
                edges.push_back(Edge{ u, v, 1 });
            }
        }
    }
    
}

void shuffleEdges(std::vector<Edge> & edges) {
    std::random_device rd;
    std::mt19937 gg(rd());

    typedef std::uniform_int_distribution<ui> distr_t;
    typedef typename distr_t::param_type param_t;

    distr_t D;
    if(edges.size() > 1)
    for(ui i = edges.size()-1; i > 0; i--) {
        ui j = D(gg, param_t(0, i));
        std::swap(edges[i], edges[j]);
    }
}

bool isDensest(std::vector<Edge> & edges, 
    std::vector<ui> & R, std::set<ui> & Aset,
    std::vector<ui> & VS, ui sz, double rho) {
    typedef HLPP Network;
    ui N = sz+2;
    ui m = 0;

    ui maxN = 0;
    for(auto & e : edges) {
        maxN = std::max(maxN, std::max(e.u, e.v));
    }
    std::vector<bool> isInVS(maxN+1);
    for(ui u : VS) isInVS[u] = true;
    for(auto & e : edges) {
        if(isInVS[e.u] && isInVS[e.v]) m++;
    }

    std::vector<ui> id(maxN+1);
    for(ui i = 0; i < sz; i++) {
        id[VS[i]] = i;
    }

    std::vector<ui> dRinVS(sz);
    std::set<ui> Rset(R.begin(), R.end());
    auto isInR = [&](ui u)-> bool {
        return Rset.find(u) != Rset.end();
    };
    for(auto & e : edges) {
        if(isInR(e.u)) {
            if(isInR(e.v)) {
                dRinVS[id[e.u]]++;
                dRinVS[id[e.v]]++;
            }
            else {
                dRinVS[id[e.v]]++;
            }
        }
        else {// if(isInR(e.v)) //always true
            dRinVS[id[e.u]]++;
        }
    }

//build network
    ui M = 2*sz + m*2;
    Network::Vertex s = 0, t = N-1;
    Network network(N, M, s, t);
    
    const double INFOFA = 1e10;
    for(ui i = 0; i < sz; i++) {
        if(Aset.find(VS[i]) != Aset.end()) {
            network.addEdge(s, i+1, INFOFA);
        }
        else {
            network.addEdge(s, i+1, dRinVS[i]);
        }
    }

    for(auto & e: edges) {
        if(isInR(e.u)) {
            if(isInR(e.v)) {
                network.addEdge(id[e.u]+1, id[e.v]+1, e.w);
                network.addEdge(id[e.v]+1, id[e.u]+1, e.w);
            }
            else {
                network.addEdge(id[e.v]+1, id[e.u]+1, e.w);
            }
        }
        else {// if(isInR(e.v)) //always true
            network.addEdge(id[e.u]+1, id[e.v]+1, e.w);
        }
        
    }

    for(ui i = 0; i < sz; i++) {
        network.addEdge(i+1, t, rho);
    }
// network.print2();
    double flow = network.maxFlow();

    return fabs(flow - rho*sz) <=  1e-5;
}


//ensestSubgraph(17.674418604651162, [3129, 4193, 5863, 6104, 6191, 6755, 7447, 7655, 8151, 8254, 8466, 8643, 8775, 9283, 9421, 9681, 9762, 10078, 10122, 10355, 10418, 11288, 11664, 12033, 12344, 12375, 12410, 12563, 12626, 12702, 12984, 13438, 13986, 14123, 14161, 14609, 15412, 16396, 17519, 20206, 20276, 22153, 22576])

//R2
//14.709677419354838 (31,)

//lj
// 30.0
// (32,)
// 960.0
// 15.0