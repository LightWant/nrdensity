#ifndef GRAPH_H
#define GRAPH_H

#include "../tools/types.hpp"
#include "../tools/fastIO.hpp"
#include <vector>
#include <utility>
#include <cstring>
using Pair = std::pair<ui, ui>;

class Graph {
public:
    ui n;
	ull m;
    ui maxD = 0;
    std::vector<ui> pIdx, pIdx2, pEdge;
    std::vector<std::vector<ui>> pDeepIdx;
    std::vector<std::vector<ui>> deg;
    std::vector<Pair> edges;
    ui coreNumber;

//mp[i] = v; mp2[v] = i;
	std::vector<ui> mp, mp2;

    Graph() {}
    void readFromText(const std::string & fPath, bool noUVM = false);
    void readFromTextDoubleEdges(const std::string & fPath);
	void readFromTextNodes(const std::string & fPath);
	
    void readFromBin(const std::string & directory);
	void saveAsBin(const std::string & directory);
    void changeToCoreOrder();

    ui degree(ui u);
    ui degree2(ui u) { return pIdx2[u] - pIdx[u]; }
    // ui degreeDeep(ui deep, ui u) { return pDeepIdx[deep][u] - pIdx[u]; }
    ui degreeDeep(ui deep, ui u) { return deg[deep][u]; }

	void buildSgFromNodes(const std::vector<ui> & vs, Graph * sg);
    void buildSgFromNodes2(const std::vector<ui> & vs, Graph * sg);

    bool connect(ui u, ui v);
    bool connect2(ui u, ui v);

    void print() {
        for(ui u = 0; u < n; u++) {
            printf("%u:", u);
            for(ui i = pIdx[u]; i < pIdx[u + 1]; i++) {
                printf("%u ", pEdge[i]);
            }
            printf("\n");
        }
    }
    void print2() {
        for(ui u = 0; u < n; u++) {
            printf("%u:", u);
            for(ui i = pIdx[u]; i < pIdx2[u]; i++) {
                printf("%u ", pEdge[i]);
            }
            printf("\n");
        }
    }
};

#endif