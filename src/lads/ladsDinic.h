#ifndef LADSDINIC_H
#define LADSDINIC_H

#include "../tools/types.hpp"
#include "../graph/graph.h"
#include "../tools/dinic.hpp"
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
    
    typedef Dinic<ui, ui, double> Network;

    void modifyAlphaAndResetNetworkOverdensed(
        Graph * g, double alpha, Network & network,
        Network::Vertex & s,
        Network::Vertex & t,
        std::vector<ui> & R,
        std::set<ui> & Aset,
        std::vector<bool> & overdensed,
        double volBR);
    densestSubgraph GlobalDensestSubgraph(Graph * g);

    void buildNetWorkGA(Graph * g, double alpha,
                    Network & network,
                    std::vector<Network::Vertex> & flowNodes,
                    Network::Vertex & s,
                    Network::Vertex & t
    );

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
    void buildNetWorkGAImproved(
        Graph * g, double alpha, Network & network,
        std::vector<Network::Vertex> & flowNodes,
        Network::Vertex & s,
        Network::Vertex & t, 
        std::vector<ui> & R,
        std::set<ui> & Aset,
        std::vector<bool> & overdensed,
        double volBR) ;
    void buildNetWorkIGAOverdensed(
        Graph * g, double alpha, Network & network,
        Network::Vertex & s,
        Network::Vertex & t, 
        std::vector<ui> & R,
        std::set<ui> & Aset,
        std::vector<bool> & overdensed,
        double volBR);
};

#endif