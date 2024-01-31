#include "../tools/getArgs.hpp"
#include "../tools/types.hpp"
#include "../graph/graph.h"

int main(int argc, char * argv[]) {
    argsController ac(argc, argv);

    Graph g;
    if(ac.exist("-f_txt"))
        g.readFromText(ac["-f_txt"], ac.exist("noUVM"));
    else
        g.readFromBin(ac["-f"]);
    if(!ac.exist("-th")) {
        printf("-th the threshold of clustering coefficiency\n");
        return 0;
    }
    double th = std::stof(ac["-th"]);

    std::vector<bool> isNei(g.n); 
    for(ui u = 0; u < g.n; u++) {
        ui du = g.degree(u);
        if(du < 10) continue;

        for(ui i = g.pIdx[u]; i < g.pIdx[u+1]; i++) {
            ui v = g.pEdge[i];
            isNei[v] = true;
        }

        ui cnt = 0;
        for(ui i = g.pIdx[u]; i < g.pIdx[u+1]; i++) {
            ui v = g.pEdge[i];
            for(ui j = g.pIdx2[v]; j < g.pIdx[v+1]; j++) {
                ui w = g.pEdge[j];
                if(isNei[w]) cnt++;
            }
        }

        if(cnt / (1.0*du*(du-1)/2) >= th) {
            printf("%u\n", u);
        }

        for(ui i = g.pIdx[u]; i < g.pIdx[u+1]; i++) {
            ui v = g.pEdge[i];
            isNei[v] = false;
        }
    }

    return 0;
}