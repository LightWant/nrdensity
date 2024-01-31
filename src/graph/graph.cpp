#include "graph.h"
#include <algorithm>
#include <iostream>
#include "listLinearHeap.hpp"
#include <cassert>

void Graph::readFromTextDoubleEdges(const std::string & fPath) {
    std::cout << fPath << std::endl;

    fastIO in(fPath, "r");

    in.getUInt(n);
    in.getUllInt(m);

    edges.resize(m);
    edges.clear();

    for(ui i = 0; i < m; i++) {
        ui u, v;
        in.getUInt(u);
        in.getUInt(v);
// std::cout << u << ' ' << v << std::endl;

        edges.push_back(Pair{u, v});
    }
    pEdge.resize(m);
    pIdx.resize(n + 1);

    pIdx[0] = 0;
    for(ui i = 0; i < edges.size(); i++) {
        pIdx[edges[i].first + 1]++;
    }
    for(ui i = 1; i <= n; i++) pIdx[i] += pIdx[i - 1];

    for(ui i = 0; i < edges.size(); i++) {
// if(pIdx[edges[i].first] >= m)
// std::cout << ' ' << edges[i].first << ' ' <<  pIdx[edges[i].first] << std::endl;
        pEdge[pIdx[edges[i].first]++] = edges[i].second;
    }

    for(ui i = n; i >= 1; i--) pIdx[i] = pIdx[i - 1];
    pIdx[0] = 0;

    for(ui u = 0; u < n; u++) {
        std::sort(pEdge.begin() + pIdx[u], pEdge.begin() + pIdx[u + 1]);
        maxD = std::max(maxD, pIdx[u + 1] - pIdx[u]);
    }
}

void Graph::readFromText(const std::string & fPath, bool noUVM) {
    std::cout << fPath << std::endl;

    fastIO in(fPath.c_str(), "r");

    if(noUVM) {
        n = 0;
        m = 0;
    }
    else {
        in.getUInt(n);
        in.getUllInt(m);
    }
    
    if(!noUVM) {
        edges.resize(m);
        edges.clear();
    }

    ui i = 0;
    ui minV = 1<<30;
    
    auto readOneEdge = [&]() {
        ui u, v;
        in.getUInt(u);
        in.getUInt(v);

        if(noUVM) {
            n = std::max(n, u);
            n = std::max(n, v);
        }

        minV = std::min(minV, u);
        minV = std::min(minV, v);

        edges.push_back(Pair{u, v});
    };

    if(noUVM) {
        while(!in.empty()) {
            readOneEdge();
            m++;
        }
    }
    else {
        for(ui i = 0; i < m; i++) readOneEdge();
    }
    
    for(ui i = 0; i < edges.size(); i++) {
        edges[i].first -= minV;
        edges[i].second -= minV;
    }

    if(noUVM) {
        n -= minV;
        n += 1;
    }
    
    m *= 2;

    pEdge.resize(m);
    pIdx.resize(n + 1);
    pIdx2.resize(n);

    pIdx[0] = 0;
    for(ui i = 0; i < edges.size(); i++) {
        pIdx[edges[i].first + 1]++;
        pIdx[edges[i].second + 1]++;
    }
    for(ui i = 1; i <= n; i++) pIdx[i] += pIdx[i - 1];

    for(ui i = 0; i < edges.size(); i++) {
        pEdge[pIdx[edges[i].first]++] = edges[i].second;
        pEdge[pIdx[edges[i].second]++] = edges[i].first;
    }

    for(ui i = n; i >= 1; i--) pIdx[i] = pIdx[i - 1];
    pIdx[0] = 0;

    for(ui u = 0; u < n; u++) {
        std::sort(pEdge.begin() + pIdx[u], pEdge.begin() + pIdx[u + 1]);
        maxD = std::max(maxD, pIdx[u + 1] - pIdx[u]);

        pIdx2[u] = pIdx[u];
        while(pIdx2[u] < pIdx[u + 1] && pEdge[pIdx2[u]] < u) pIdx2[u]++;
    }
}

void Graph::readFromTextNodes(const std::string & fPath) {
    std::cout << fPath << std::endl;

    fastIO in(fPath.c_str(), "r");
    
    in.getUInt(n);

    m = 0;
    for(ui i = 0; i < n; i++) {
        ui u, du;
        in.getUInt(u);
        in.getUInt(du);
        m += du;
    }
printf("edges %llu\n", m);fflush(stdout);
    m *= 2;
    edges.resize(m);
    for(ui i = 0; i < m; i++) {
        ui u, v;
        in.getUInt(u);
        in.getUInt(v);

        edges.push_back(Pair{u, v});
    }
    
    pEdge.resize(m);
    pIdx.resize(n + 1);

    pIdx[0] = 0;
    for(ui i = 0; i < edges.size(); i++) {
        pIdx[edges[i].first + 1]++;
    }
    for(ui i = 1; i <= n; i++) pIdx[i] += pIdx[i - 1];

    for(ui i = 0; i < edges.size(); i++) {
// if(pIdx[edges[i].first] >= m)
// std::cout << ' ' << edges[i].first << ' ' <<  pIdx[edges[i].first] << std::endl;
        pEdge[pIdx[edges[i].first]++] = edges[i].second;
    }

    for(ui i = n; i >= 1; i--) pIdx[i] = pIdx[i - 1];
    pIdx[0] = 0;

    for(ui u = 0; u < n; u++) {
        std::sort(pEdge.begin() + pIdx[u], pEdge.begin() + pIdx[u + 1]);
        maxD = std::max(maxD, pIdx[u + 1] - pIdx[u]);
    }
}

void Graph::readFromBin(const std::string & directory) {
    fastIO readEdge(directory + "edge.bin", "rb");
    fastIO readIdx(directory + "idx.bin", "rb");

    //load pIdx
    n = readIdx.leftBytes() / sizeof(ui) - 1;
    pIdx.resize(n + 1);
    int64_t tmp = readIdx.load(pIdx.data());
    if(tmp < 0) {
        printf("load idx.bin error:%lld\n", tmp);
        exit(-1);
    }

    //load pEdge
    m = readEdge.leftBytes() / sizeof(ui);
// printf("%u %lu, %lu %u\n", n, m, readEdge.leftBytes(), pIdx[n]);
    pEdge.resize(m);
    tmp = readEdge.load(pEdge.data());
    if(tmp < 0) {
        printf("load edge.bin error:%lld\n", tmp);
        exit(-1);
    }

    for(ui u = 0; u < n; u++) {
        maxD = std::max(maxD, pIdx[u + 1] - pIdx[u]);
    }
    
    pIdx2.resize(n);
    for(ui u = 0; u < n; u++) {
        maxD = std::max(maxD, pIdx[u + 1] - pIdx[u]);

        pIdx2[u] = pIdx[u];
        while(pIdx2[u] < pIdx[u + 1] && pEdge[pIdx2[u]] < u) pIdx2[u]++;
    }
}

void Graph::changeToCoreOrder() {
    ListLinearHeap lheap(n, maxD + 1);
    ui * ids = new ui[n];
    ui * keys = new ui[n + 1];
    for(ui i = 0; i < n; i++) {
        ids[i] = i;
        keys[i] = pIdx[i + 1] - pIdx[i] + 1;
    }
    lheap.init(n, n, ids, keys);

    mp.resize(n);
    mp2.resize(n);

    ui * pDIdx = keys;
    ui * pDEdge = new ui[m];

    for(ui i = 0; i < n; i++) {
        ui v, degV;

        if(!lheap.pop_min(v, degV)) printf("error\n");
        // printf("%u %u\n", v, degV-1);
        mp[i] = v; mp2[v] = i;
        for(ui j = pIdx[v]; j < pIdx[v + 1]; j++) {
            lheap.decrement(pEdge[j]);
        }
    }

    pDIdx[0] = 0;
    for(ui i = 1; i <= n; i++) {
        ui v = mp[i - 1];
        pDIdx[i] = pDIdx[i - 1] + pIdx[v + 1] - pIdx[v];
    }

    for(ui i = 0; i < n; i++) {
        ui k = pIdx[mp[i]];
        for(ui j = pDIdx[i]; j < pDIdx[i + 1]; j++) {
            pDEdge[j] = mp2[pEdge[k++]];
        }
        std::sort(pDEdge + pDIdx[i], pDEdge + pDIdx[i + 1]);
    }

    memcpy(pEdge.data(), pDEdge, sizeof(ui) * m);
    memcpy(pIdx.data(), pDIdx, sizeof(ui) * (n + 1));
    
    delete [] ids;
    delete [] keys;
    // delete [] mp2;
    delete [] pDEdge;

    pIdx2.resize(n);
    coreNumber = 0;
    for(ui i = 0; i < n; i++) {
        pIdx2[i] = pIdx[i + 1];
        for(ui j = pIdx[i]; j < pIdx[i + 1]; j++) {
            if(pEdge[j] > i) {
                pIdx2[i] = j;
                coreNumber = std::max(coreNumber, pIdx[i + 1] - j);
                break;
            }
        }
    }
}

void Graph::saveAsBin(const std::string & directory) {
    std::string idxPath = directory + "idx.bin";
    std::string edgePath = directory + "edge.bin";
    
    FILE * fidx = fopen(idxPath.c_str(), "wb");
    fwrite(pIdx.data(), sizeof(ui), n + 1, fidx);
    fclose(fidx);

    FILE * fedge = fopen(edgePath.c_str(), "wb");
    fwrite(pEdge.data(), sizeof(ui), m, fedge);
    fclose(fedge);
}

ui Graph::degree(ui u) { return pIdx[u + 1] - pIdx[u]; }

bool Graph::connect(ui u, ui v) { 
    return std::binary_search(pEdge.begin() + pIdx[u], pEdge.begin() + pIdx[u + 1], v);
}
bool Graph::connect2(ui u, ui v) {
    return std::binary_search(pEdge.begin() + pIdx[u], pEdge.begin() + pIdx2[u], v);
}

void Graph::buildSgFromNodes(const std::vector<ui> & vs, Graph * sg) {
    sg->n = vs.size();
    sg->pIdx.resize(sg->n + 1);
    sg->pIdx2.resize(sg->n + 1);
// printf("here\n");fflush(stdout);
    ull sm = 0;
    for(ui i = 0; i < sg->n; i++) {
// printf("here, i %u\n", i);fflush(stdout);
        for(ui j = i + 1; j < sg->n; j++) {
// printf("here, ij %u, %u %u\n", j, vs[i], vs[j]);fflush(stdout);
            if(connect(vs[i], vs[j])) {
                sg->pIdx[i+1]++;
                sg->pIdx[j+1]++;
                sm++;
            }
        }
    }
// printf("here, sm %llu\n", sm);fflush(stdout);
    sg->pEdge.resize(sm * 2);

    for(ui i = 1; i <= sg->n; i++) sg->pIdx[i] += sg->pIdx[i - 1];

    for(ui i = 0; i < vs.size(); i++) {
        for(ui j = i + 1; j < vs.size(); j++) {
            if(connect(vs[i], vs[j])) {
                sg->pEdge[sg->pIdx[i]++] = j;
                sg->pEdge[sg->pIdx[j]++] = i;
            }
        }
    }

    for(ui i = sg->n; i >= 1; i--) sg->pIdx[i] = sg->pIdx[i - 1];
    sg->pIdx[0] = 0;
// printf("here\n");fflush(stdout);
    for(ui u = 0; u < sg->n; u++) {
        std::sort(sg->pEdge.begin() + sg->pIdx[u], 
            sg->pEdge.begin() + sg->pIdx[u + 1]);
        sg->maxD = std::max(sg->maxD, sg->pIdx[u + 1] - sg->pIdx[u]);

        sg->pIdx2[u] = sg->pIdx[u];
        while(sg->pIdx2[u] < sg->pIdx[u + 1] && sg->pEdge[sg->pIdx2[u]] < u) 
            sg->pIdx2[u]++;
    }

}

void Graph::buildSgFromNodes2(const std::vector<ui> & vs, Graph * sg) {
    sg->n = vs.size();
    sg->pIdx.resize(sg->n + 1);
    sg->pIdx2.resize(sg->n);
// printf("here\n");fflush(stdout);

    ui maxV = 0;
    for(ui u : vs) maxV = std::max(maxV, u);
    std::vector<ui> reId(maxV + 1, UINT32_MAX);
    for(ui i = 0; i < vs.size(); i++) reId[vs[i]] = i;

    ull sm = 0;
    for(ui i = 0; i < sg->n; i++) {
        ui u = vs[i];
// printf("%u:", u);
        for(ui j = pIdx[u]; j < pIdx[u+1]; j++) {
            ui v = pEdge[j];
            if(v <= maxV && reId[v] != UINT32_MAX) {
                sg->pIdx[i+1]++;
                // sg->pIdx[reId[v]+1]++;
                sm++;
// printf("%u ", v);
            }
        }
// printf("\n");
    }
// printf("here, sm %llu\n", sm);fflush(stdout);
    sg->pEdge.resize(sm);
    sg->m = sm;

    sg->pIdx[0] = 0;
    for(ui i = 1; i <= sg->n; i++) sg->pIdx[i] += sg->pIdx[i - 1];

    for(ui i = 0; i < sg->n; i++) {
        ui u = vs[i];
        for(ui j = pIdx[u]; j < pIdx[u+1]; j++) {
            ui v = pEdge[j];
            if(v <= maxV && reId[v] != UINT32_MAX) {
                sg->pEdge[sg->pIdx[i]++] = reId[v];
                // sg->pEdge[sg->pIdx[reId[v]]++] = i;
            }
        }
    }

    for(ui i = sg->n; i >= 1; i--) sg->pIdx[i] = sg->pIdx[i - 1];
    sg->pIdx[0] = 0;
// printf("here\n");fflush(stdout);
    for(ui u = 0; u < sg->n; u++) {
        std::sort(sg->pEdge.begin() + sg->pIdx[u], 
            sg->pEdge.begin() + sg->pIdx[u + 1]);
        sg->maxD = std::max(sg->maxD, sg->pIdx[u + 1] - sg->pIdx[u]);

        sg->pIdx2[u] = sg->pIdx[u];
        while(sg->pIdx2[u] < sg->pIdx[u + 1] && sg->pEdge[sg->pIdx2[u]] < u) 
            sg->pIdx2[u]++;
    }

//assert
// for(ui u = 0; u < sg->n; u++) {
//     for(ui i = sg->pIdx[u]; i < sg->pIdx[u+1]; i++) {
//         ui v = sg->pEdge[i];
//         bool f = false;
//         for(ui j = sg->pIdx[v]; j < sg->pIdx[v+1]; j++) {
//             if(sg->pEdge[j] == u) f = true;
//         }
//         // assert(f);
//         if(f == false) printf("errorerror!\n");
//     }
// }
}