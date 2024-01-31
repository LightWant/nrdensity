
// #include "../tools/MaxFlowDouble.h"
// #include "../tools/dinic.hpp"
#include "../tools/HLPP.hpp"
#include "../tools/types.hpp"
#include <vector>
#include <cstdio>

int main() {
    // typedef Dinic<ui, ui, double> Network;
    typedef HLPP Network;
    ui n = 6;
    ui m = 11;
    ui N = n+2;
    ui M = n + m*2 + n;
    std::vector<ui> ct(N);

    Network::Vertex s = 0, t = N-1;
    Network network(N, M, s, t);
    // std::vector<Network::Vertex> R;

    struct Edge {
        ui a, b;
    };
    std::vector<Edge> edges;
    edges.push_back(Edge{0,1});
    edges.push_back(Edge{0,2});
    edges.push_back(Edge{0,3});
    edges.push_back(Edge{0,4});
    edges.push_back(Edge{0,5});
    edges.push_back(Edge{1,2});
    edges.push_back(Edge{1,3});
    edges.push_back(Edge{1,4});
    edges.push_back(Edge{2,3});
    edges.push_back(Edge{2,4});
    edges.push_back(Edge{3,4});

    std::vector<ui> R = {0,1,2,3,4};
    double sumR = 0;

    for(ui i = 0; i < R.size(); i++) {
        ui u = R[i];
        ui di = 0;
        for(Edge e : edges) {
            if(e.a == u || e.b == u) di++;
        }
        sumR += di;

        network.addEdge(s, u+1, (double)di);
    }

    for(Edge e : edges) {
        network.addEdge(e.a+1, e.b+1, 1.0);       
    }
    for(Edge e : edges) {
        network.addEdge(e.b+1, e.a+1, 1.0);       
    }

    for(ui i = 0; i < n; i++) {
        network.addEdge(i+1, t, 1.0);
    }
network.print2();
printf("\n");

    double l = 1.0, r = 5;

    do {
        double mid = (l + r) / 2;
        for(ui u = 0; u < N; u++) ct[u] = 0;
        for(ui i = 0; i < R.size(); i++) {
            ui u = R[i];
            ui di = 0;
            for(Edge e : edges) {
                if(e.a == u || e.b == u) di++;
            }
            network.reSetEdge(s, u+1, ct[s]++, ct[u+1]++, di);
        }
        for(Edge e : edges) {
            network.reSetEdge(e.a+1, e.b+1,ct[e.a+1]++,ct[e.b+1]++, 1.0);       
        }
        for(Edge e : edges) {
            network.reSetEdge(e.b+1, e.a+1,ct[e.b+1]++,ct[e.a+1]++, 1.0);       
        }

        for(ui i = 0; i < n; i++) {
            network.reSetEdge(i+1, t,ct[i+1]++,ct[t]++, mid);
        }

network.print2();
        double fValue = network.maxFlow();
printf("%f %f, l %f, r %f\n", mid, fValue, l, r);
std::vector<Network::Vertex> vs;
network.getSTCut(vs);
for(ui u : vs) printf("%u ", u); printf("\n");fflush(stdout);
printf("\n");

        if(fValue >= sumR) r = mid;
        else l = mid;

    }while( (r - l) > 1e-4);

    // network.print();
    // printf("%f\n", network.maxFlow());fflush(stdout);
    // network.print2();printf("\n");

    
//modify edge capacity
    
    
    
    // network.print2();printf("\n");
    // printf("%f\n", network.maxFlow());
    // network.print2();printf("\n");
    // network.getSTCut(vs);
    // for(ui u : vs) printf("%u ", u); printf("\n");
//modify edge capacity
    // for(ui i = 0; i < n; i++) {
    //     ui di = 0;
    //     for(Edge e : edges) {
    //         if(e.a == i || e.b == i) di++;
    //     }
    //     network.modifyCapacity(1.51, i+1, di, s, t);
    // }
    
    // // network.print2();
    // printf("%f\n", network.maxFlow());
    // network.getSTCut(vs);
    // for(ui u : vs) printf("%u ", u); printf("\n");

    return 0;
}