#ifndef LADS_H
#define LADS_H

#include "../tools/types.hpp"
#include "../graph/graph.h"
#include "../tools/MaxFlowDouble.h"
#include <vector>
#include <set>

namespace LADS {
    struct densestSubgraph {
        double alphaStar;
        std::vector<ui> sourceNodes;
    };

    densestSubgraph 
        LocalAnchoredDensestSubgraph(
            Graph * g, 
            std::vector<ui> & R, 
            std::vector<ui> & A, double inducedR);

    void modifyAlphaAndResetNetwork(Graph * g, double alpha, Network & network,
        std::vector<Network::Vertex> & flowNodes,
        Network::Vertex & s,
        Network::Vertex & t);
    densestSubgraph GlobalDensestSubgraph(Graph * g);

    void getComponentAdjacency(Graph * g, std::vector<ui>&, std::vector<ui>&);

    densestSubgraph flownetworkAlpha(Graph * g, double alpha);
//IGA
    densestSubgraph
    ImprovedGlobalAnchoredDensestSubgraphSetFlow(Graph * g, 
        std::vector<ui>&R,  std::set<ui> & Aset,
          std::vector<bool>&  overdensed, double InducedDS);
    densestSubgraph flownetworkAlphaImproved(Graph * g, double alpha, 
        std::vector<ui> & R, std::set<ui> & Aset,
        std::vector<bool>& overdensed, double volBR);
};

#endif