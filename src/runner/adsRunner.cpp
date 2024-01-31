#include "../graph/graph.h"
#include "../tools/getArgs.hpp"
// #include "../lads/ladsDinic.h"
#include "../lads/ladsHLPP.h"
// #include "../lads/lads.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <set>

void printUsage() {
    std::cout << "-f graph file directory(edge.bin & idx.bin)" << std::endl;
    std::cout << "-f_txt graph file text file, each edge exists one time" << std::endl;
    std::cout << "-f_txtD graph file text file, each edge exists two times" << std::endl;
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

    std::cout << "load graph: n " << g.n << " m/2 " << g.m/2 << " maxD " << g.maxD << std::endl;

    // std::set<ui> A, R;
    std::vector<ui> A, R;
    if(ac.exist("-A")) {
        std::fstream finA(ac["-A"], std::ios::in);
        ui u;
        while(finA >> u) {
            // A.insert(u);
            A.push_back(u);
        }
        finA.close();
    }

    std::fstream finR(ac["-R"], std::ios::in);
    std::cout << "R_Path:" << ac["-R"] << std::endl;
    ui u;
    while(finR >> u) {
        // R.insert(u);
        R.push_back(u);
    }
    finR.close();

    std::sort(R.begin(), R.end());


printf("Rsize:%lu\n", R.size());
printf("Asize:%lu\n", A.size());

auto t1 = std::chrono::steady_clock::now();
    Graph sg;
    g.buildSgFromNodes2(R, &sg);
    // LADS::densestSubgraph den1 = LADS::GlobalDensestSubgraph(&sg);
printf("sgn %u, sgm %llu\n", sg.n, sg.m);
    std::vector<bool> overdensed(sg.n);
    std::vector<ui> sR(sg.n);
    for(ui i = 0; i < sg.n; i++) sR[i] = i;
    std::set<ui> Aset2;
    ui maxVinR = 0;
    for(ui u : R) maxVinR = std::max(maxVinR, u);
    // for(ui u : A) maxVinR = std::max(maxVinR, u);
    std::vector<ui> reId(maxVinR + 1, UINT32_MAX);

    for(ui i = 0; i < R.size(); i++) reId[R[i]] = i;

    for(ui u : A) {
        Aset2.insert(reId[u]);
    }

    LADS::densestSubgraph den1 = LADS::ImprovedGlobalAnchoredDensestSubgraphSetFlow(
        &sg, sR, Aset2, overdensed, 1.0*sg.m/sg.n
    );
printf("inducedRdensity:%f\n", den1.alphaStar);
printf("inducedLen:%lu\n", den1.sourceNodes.size());
fflush(stdout);

// for(ui u : A) {
//     bool f = false;
//     for(ui vId : den1.sourceNodes) {
//         ui v = R[vId];
//         if(u == v) f = true;
//     }
//     if(!f) {
//         printf("Not find------%u\n", u);
//     }
// }
// for(ui u : den1.sourceNodes) printf("%u ", u); printf("\n");

// OverdensedMask = map(v->(GetDegree(B,v)>=GetVolume(B,R)), 1:size(B,1))
    // std::vector<bool> overdensed(g.n);
    // double volBR = 0.0;
    // for(ui u : R) volBR += g.degree(u);
    // for(ui i = 0; i < g.n; i++) {
    //     if(g.degree(i) >= volBR) overdensed[i] = true;
    // }
    // LADS::densestSubgraph den = 
    //     LADS::ImprovedGlobalAnchoredDensestSubgraphSetFlow(&g, R,
    //         overdensed, den1.alphaStar);

// printf("R:");
// for(ui u : R) printf("%u ", u); printf("\n");
// printf("A:");
// for(ui u : A) printf("%u ", u); printf("\n");



    LADS::densestSubgraph den = LADS::LocalAnchoredDensestSubgraph(&g, R, A, den1.alphaStar);
    
    //one order of magenitude faster than julia

auto t2 = std::chrono::steady_clock::now();
auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
std::cout << "time:" << duration.count() << "ms" << std::endl;

    printf("R-density:%f\n", den.alphaStar);
    printf("size:%lu\n", den.sourceNodes.size());
    printf("den:");
    for(ui u : den.sourceNodes) printf("%u ", u);
    printf("\n");  

//real density

ull sumD = 0;
// ui maxV = 0;
// for(ui u : den.sourceNodes) maxV = std::max(maxV, u);
std::vector<bool> inVs(g.n);
for(ui i = 0; i < den.sourceNodes.size(); i++) {
    inVs[den.sourceNodes[i]] = true;
}
for(ui u : den.sourceNodes) {
    for(ui i = g.pIdx2[u]; i < g.pIdx[u+1]; i++) {
        ui v = g.pEdge[i];
        if(inVs[v]) sumD++;
    }
}
printf("realEdgeBasedDensity:%f\n", 1.0 * sumD / (den.sourceNodes.size()));

// for(ui u : A) {
//     printf("A %u: %u %u\n", u, mp[u], mp2[u]);
// }

//local conduntance, epsilon=0.1, pu=0
// ui ESShat = 0;
// ui volRS = 0;
// ui volRShat = 0;

// std::set<ui> Rset(R.begin(), R.end());
// auto inR = [&](ui u) {
//     return Rset.find(u) != Rset.end();
// };

// for(ui u = 0; u < g.n; u++) {
//     for(ui i = g.pIdx2[u]; i < g.pIdx[u+1]; i++) {
//         ui v = g.pEdge[i];
//         if(inVs[u] != inVs[v]) {
//             ESShat++;
//         }
//     }
//     if(inVs[u] && inR(u)) volRS += g.degree(u);
//     if(inVs[u] && !inR(u)) volRS += g.degree(u);
// }
// // printf("%u %u %u\n", ESShat, volRS, volRShat);
// printf("localCondunctance:%f\n", 1.0*ESShat/(volRS - 0.1*volRShat));

    return 0;
}