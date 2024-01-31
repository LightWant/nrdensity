#include "lads.h"

std::vector<ui> orderedSubsetIndices(std::vector<ui> & L, std::vector<ui> & R) {
    std::vector<ui> indices(R.size(), -1);
    ui indL = 0;
    ui indR = 0;
    while(indL < L.size() && indR < R.size()) {
        if(L[indL] == R[indR]) {
            indices[indR] = indL;
            indR++;
        }
        indL++;
    }
    indices.resize(indR);
    
    return indices;
}

void buildNetWorkGA(Graph * g, double alpha,
Network & network,
std::vector<Network::Vertex> & flowNodes,
Network::Vertex & s,
Network::Vertex & t
) {
    for(ui i = 0; i < g->n; i++) {
        flowNodes.push_back(network.AddVertex());
    }

    for(ui u = 0; u < g->n; u++) {
        network.AddEdge(s, flowNodes[u], g->degree(u));
    }

    for(ui u = 0; u < g->n; u++) {
        for(ui i = g->pIdx[u]; i < g->pIdx[u+1]; i++) {
            ui v = g->pEdge[i];
            network.AddEdge(flowNodes[u], flowNodes[v], 1.0);
        }
    }

    for(ui u = 0; u < g->n; u++) {
        network.AddEdge(flowNodes[u], t, alpha);
    }
}

void buildNetWorkGAImproved(
    Graph * g, double alpha, Network & network,
        std::vector<Network::Vertex> & flowNodes,
        Network::Vertex & s,
        Network::Vertex & t, 
        std::vector<ui> & R,
        std::set<ui> & Aset,
        std::vector<bool> & overdensed,
        double volBR) {
    for(ui i = 0; i < g->n; i++) {
        flowNodes.push_back(network.AddVertex());
    }

    for(ui u : R) {
        if(Aset.find(u) == Aset.end())
            network.AddEdge(s, flowNodes[u], g->degree(u));
        else
            network.AddEdge(s, flowNodes[u], 1e15);
    }

    for(ui u = 0; u < g->n; u++) {
        for(ui i = g->pIdx[u]; i < g->pIdx[u+1]; i++) {
            ui v = g->pEdge[i];
            network.AddEdge(flowNodes[u], flowNodes[v], 1);
        }
    }
// printf("%lu\n", overdensed.size());
// for(ui u : Aset) printf("%u %d ", u, (int)overdensed[u]); printf("\n");fflush(stdout);
    for(ui u = 0; u < g->n; u++) {
        if(overdensed[u])
            network.AddEdge(flowNodes[u], t, volBR+1);
        else  {
            network.AddEdge(flowNodes[u], t, alpha);
        }
        // else {
        //     printf("A %u\n", u);fflush(stdout);
        // }
    }
}


// void LADS::modifyAlphaAndResetNetwork(Graph * g, double alpha,
// Network & network,
// std::vector<Network::Vertex> & flowNodes,
// Network::Vertex & s,
// Network::Vertex & t
// ) {
//     for(ui u = 0; u < g->n; u++) {
// // printf("%u ", u); fflush(stdout);
//         network.modifyCapacity(alpha, flowNodes[u], g->degree(u), s, t);
//     }

// }

// #define DEBUG

//each edge has weight 2
// LADS::densestSubgraph 
// LADS::GlobalDensestSubgraph(Graph * g) {
//     double alphaBottom = 1.0 * g->m / g->n;
//     double alphaTop = 1.0 * g->maxD;
// #ifdef DEBUG
// printf("alphaB %f\n", alphaBottom);
// printf("alphaT %f\n", alphaTop);
// #endif
//     ull flowAlphaMinus = 0;
//     double alphaStar = 0.0;
//     densestSubgraph ans;

//     Network network;
//     std::vector<Network::Vertex> flowNodes;
//     flowNodes.reserve(g->n);
//     Network::Vertex s = network.AddVertex(), t = network.AddVertex();
//     buildNetWorkGA(g, alphaBottom, network, flowNodes, s, t);
// // printf("hereGA\n");fflush(stdout);

//     double fValue = network.MaxFlow(s, t);
// #ifdef DEBUG
// printf("f0 %f %f %llu\n", fValue, alphaBottom*g->n, g->m);fflush(stdout);
// #endif
//     if(fValue+1e-6 >= g->m) {
//         alphaStar = alphaBottom;
//         //change network alpha
//         // changeNetWorkAlpha(network, t, , alphaStar-);
//         double tmpSmallAlpha = alphaStar-1.0/(1.0*(g->n+2)*(g->n+1));
//         densestSubgraph den = flownetworkAlpha(tmpSmallAlpha);
//         // modifyAlphaAndResetNetwork(g, tmpSmallAlpha, network, flowNodes, s, t);
//         // double fValue = network.MaxFlow(s, t);
//     }
//     else {
// printf("tolerance %f\n", 1.0/(1.0*(g->n+1)));fflush(stdout);
// int iterations = 0;
//         while((alphaTop - alphaBottom)*(g->n+2) > 1.0/(1.0*(g->n+1))) {
// iterations++;
//             double alpha = (alphaBottom + alphaTop) / 2;
// printf("alphatmp %f, L %f, R %f\n",
//     alpha, alphaBottom, alphaTop);fflush(stdout);
//             // modifyAlphaAndResetNetwork(g, alpha, network, flowNodes, s, t);

//             // double fValue = network.MaxFlow(s, t);
// Network network2;
// std::vector<Network::Vertex> flowNodes2;
// flowNodes2.reserve(g->n);
// Network::Vertex s2 = network2.AddVertex(), t2 = network2.AddVertex();
// buildNetWorkGA(g, alpha, network2, flowNodes2, s2, t2);
// double fValue2 = network2.MaxFlow(s2, t2);
// printf("fValue %f, %f, m %f\n", fValue, fValue2, 1.0*g->m);fflush(stdout);
//             if(fValue2+1e-6 >= 1.0*g->m) alphaTop = alpha;
//             else alphaBottom = alpha;

//             if((alphaTop - alphaBottom)*(g->n+2) <= 1.0/(1.0*(g->n+1))) {
//                 network2.findNeighborsByBFS(s2, flowNodes2, ans.sourceNodes);
//                 ui len = ans.sourceNodes.size();
// printf("sizeDensest:%u\n", len);
//                 ans.alphaStar = double((long long)((alphaBottom*len+1)) / (1.0*len));
// printf("density:%f\n", ans.alphaStar);
//             }
//         }
// printf("ites %u\n", iterations);
//         // alpha_star = Float64(floor((alpha_bottom * subgraph_length) + 1) / subgraph_length)
//     }

//     // network.findNeighborsByBFS(s, flowNodes, ans.sourceNodes);
// //     ui len = ans.sourceNodes.size();
// //     ans.alphaStar = double((long long)((alphaBottom*len+1)) / (1.0*len));
//     return ans;
// }

LADS::densestSubgraph LADS::flownetworkAlpha(Graph * g, double alpha) {
    Network network;
    std::vector<Network::Vertex> flowNodes;
    flowNodes.reserve(g->n);
    Network::Vertex s = network.AddVertex(), t = network.AddVertex();
    buildNetWorkGA(g, alpha, network, flowNodes, s, t);
    
    double fValue = network.MaxFlow(s, t);
    densestSubgraph ans;
    ans.alphaStar = fValue;//arbitrary usage

    network.findNeighborsByBFS(s, flowNodes, ans.sourceNodes);

    return ans;
}

LADS::densestSubgraph 
LADS::GlobalDensestSubgraph(Graph * g) {
    double alphaBottom = 1.0 * g->m / g->n;
    double alphaTop = 1.0 * g->maxD;
#ifdef DEBUG
printf("alphaB %f\n", alphaBottom);
printf("alphaT %f\n", alphaTop);
#endif
    ull flowAlphaMinus = 0;
    double alphaStar = 0.0;
    densestSubgraph ans;

    densestSubgraph den = flownetworkAlpha(g, alphaBottom);
    double fValue = den.alphaStar;
#ifdef DEBUG
printf("f0 %f %f %llu\n", fValue, alphaBottom*g->n, g->m);fflush(stdout);
#endif

    if(fValue+1e-6 >= g->m) {
        alphaStar = alphaBottom;
       
        double tmpSmallAlpha = alphaStar-1.0/(1.0*(g->n+2)*(g->n+1));
        ans = flownetworkAlpha(g, tmpSmallAlpha);

        ull len = ans.sourceNodes.size();
        ull sum = tmpSmallAlpha * len + 1;
        double realAlpha = 1.0 * sum / len;
        ans.alphaStar = realAlpha;
    }
    else {
// printf("tolerance %f\n", 1.0/(1.0*(g->n+1)));fflush(stdout);
// int iterations = 0;
        while(true) {
// iterations++;
            double alpha = (alphaBottom + alphaTop) / 2;
// printf("alphatmp %f, L %f, R %f\n",
//     alpha, alphaBottom, alphaTop);fflush(stdout);
            densestSubgraph den = flownetworkAlpha(g, alpha);
            double fValue = den.alphaStar;

            if(fValue+1e-6 >= 1.0*g->m) alphaTop = alpha;
            else alphaBottom = alpha;

            if((alphaTop - alphaBottom)*(g->n+2) <= 1.0/(1.0*(g->n+1))) {
               

                double tmpSmallAlpha = alpha-1.0/(1.0*(g->n+2)*(g->n+1));
                ans = flownetworkAlpha(g, tmpSmallAlpha);

                ull len = ans.sourceNodes.size();
                ull sum = alphaBottom * len + 1;
                double realAlpha = 1.0 * sum / len;
                ans.alphaStar = realAlpha;
                
                break;
            }
        }
// printf("ites %u\n", iterations);
        // alpha_star = Float64(floor((alpha_bottom * subgraph_length) + 1) / subgraph_length)
    }

    return ans;
}

LADS::densestSubgraph 
LADS::LocalAnchoredDensestSubgraph(
    Graph * g, 
    std::vector<ui> & R, 
    std::vector<ui> & A, double inducedR) {

    std::vector<ui> Expaneded;
    std::vector<bool> isInExpaneded(g->n);
    
    std::sort(R.begin(), R.end());
    // std::set<ui> stR(R.begin(), R.end());

    std::vector<ui> Frontier(R);
    double alpha  = 0;
    std::vector<ui> S, L;
    std::set<ui> SUnion;

    std::vector<bool> overdensedMask(g->n);

    densestSubgraph ans;

    auto getVolume = [](Graph * g, const std::vector<ui> & vs) {
        double ans = 0.0;
        for(ui u : vs) ans += g->degree(u);
        return ans;
    };
    double volBR = getVolume(g, R);

    while(Frontier.size()) {

        for(auto v : Frontier) {
            if(!isInExpaneded[v]) {
                isInExpaneded[v] = true;
                Expaneded.push_back(v);
            }
        }
        // L = sort(union(L, GetComponentAdjacency(B, Frontier, true))) 
        //insert the neighbors of Frontier (self = true) into L
        getComponentAdjacency(g, Frontier, L);
printf("L %lu\n", L.size());fflush(stdout);
        //subg
        Graph sg;
        g->buildSgFromNodes2(L, &sg);
        
// printf("sg n %u m %llu maxD %u\n", sg.n, sg.m, sg.maxD);fflush(stdout);
        //volR, not used?
        
        // double volumeR = getVolume(&sg, orderedSubsetIndices(L, R))

        //overdensed vertices
        // OverdensedMask = map(v->(GetDegree(B,v)>=GetVolume(B,R)), L)

        for(ui u : L) if(g->degree(u) >= volBR) {
            overdensedMask[u] = true;
        }
// printf("volBR:%f\n", volBR);fflush(stdout);
        //binary search on the new network
        // result_S = ImprovedGlobalAnchoredDensestSubgraphSetFlow
        //(subgraph, orderedSubsetIndices(L, RSorted), OverdensedMask, InducedDS)
        // densestSubgraph den = GlobalDensestSubgraph(&sg);
        std::vector<ui> sR = orderedSubsetIndices(L, R);
        std::vector<ui> sA = orderedSubsetIndices(L, A);
        std::set<ui> Aset(sA.begin(), sA.end());

        densestSubgraph den = ImprovedGlobalAnchoredDensestSubgraphSetFlow(
            &sg, sR, Aset, overdensedMask, inducedR);

        ans.alphaStar = den.alphaStar;
        //Snew, //S = L[result_S.source_nodes]
        S.clear();
        for(ui u : den.sourceNodes) S.push_back(L[u]);

        // SUnion = union(SUnion, S)
        // for(ui u : S) SUnion.insert(u);

        //Frontier = setdiff(Snew, Expanded)
        Frontier.clear();
        for(ui u : S) if(!isInExpaneded[u]) {
            Frontier.push_back(u);
        }

    }

    ans.sourceNodes.resize(S.size());
    ans.sourceNodes.clear();
    for(ui u : S) ans.sourceNodes.push_back(u);

    return ans;
}

void LADS::getComponentAdjacency(Graph * g, std::vector<ui>& Frontier, std::vector<ui>& L) {
    std::set<ui> st(L.begin(), L.end());
    for(ui u : Frontier) {
        for(ui i = g->pIdx[u]; i < g->pIdx[u+1]; i++) {
            ui v = g->pEdge[i];
            st.insert(v);
        }
    }
    for(ui u : Frontier) st.insert(u);

    if(st.size() > L.size()) {
        L.resize(st.size());
        L.clear();
        for(auto v : st) L.push_back(v);
    }
}

// LADS::densestSubgraph
//     LADS::ImprovedGlobalAnchoredDensestSubgraphSetFlow(Graph * g, std::vector<ui> & R,
//           std::vector<bool>& overdensed, double InducedDS) {
            
//     double alphaBottom = InducedDS;
//     double alphaTop = std::min(1.0 * g->maxD, 1.0*R.size());
// // printf("alphaB %f\n", alphaBottom);
// // printf("alphaT %f\n", alphaTop);
//     ull flowAlphaMinus = 0;
//     double alphaStar = 0.0;
//     densestSubgraph ans;

//     double volBR = 0.0;
//     for(ui u : R) volBR += g->degree(u);

//     Network network;
//     std::vector<Network::Vertex> flowNodes;
//     flowNodes.reserve(g->n);
//     Network::Vertex s = network.AddVertex(), t = network.AddVertex();
//     buildNetWorkIGAOverdensed(g, alphaBottom, network,
//          flowNodes, s, t, R, overdensed, volBR);

// // printf("hereGA\n");fflush(stdout);
//     double fValue = network.MaxFlow(s, t);
// // printf("f0 %f\n", fValue);fflush(stdout);
//     if(fValue+1e-6 >= g->m) {
//         alphaStar = alphaBottom;
//         //change network alpha
//         // changeNetWorkAlpha(network, t, , alphaStar-);
//         double tmpSmallAlpha = alphaStar-1.0/(1.0*(g->n+2)*(g->n+1));
//         modifyAlphaAndResetNetworkOverdensed(g, tmpSmallAlpha, network, flowNodes, s, t, overdensed, volBR);
//         double fValue = network.MaxFlow(s, t);
//     }
//     else {
// // printf("tolerance %f\n", 1.0/(1.0*(g->n+1)));fflush(stdout);
// // int iterations = 0;
//         while((alphaTop - alphaBottom)*(g->n+2) > 1.0/(1.0*(g->n+1))) {
// // iterations++;
//             double alpha = (alphaBottom + alphaTop) / 2;
// // printf("alphatmp %f, %f %f\n", alpha, alphaBottom, alphaTop);fflush(stdout);
//             modifyAlphaAndResetNetworkOverdensed(g, alpha, network, flowNodes, s, t, overdensed, volBR);
//             double fValue = network.MaxFlow(s, t);
// // printf("fValue %f %f\n", fValue, 1.0*g->m);fflush(stdout);
//             if(fValue+1e-6 >= 1.0*g->m) alphaTop = alpha;
//             else alphaBottom = alpha;
//         }
// // printf("ites %u\n", iterations);
//         // alpha_star = Float64(floor((alpha_bottom * subgraph_length) + 1) / subgraph_length)
//     }

//     network.findNeighborsByBFS(s, flowNodes, ans.sourceNodes);
//     ui len = ans.sourceNodes.size();
// printf("sizeDensest:%u\n", len);
//     ans.alphaStar = double((long long)((alphaBottom*len+1)) / (1.0*len));
// printf("density:%f\n", ans.alphaStar);
//     return ans;
// }

LADS::densestSubgraph
    LADS::ImprovedGlobalAnchoredDensestSubgraphSetFlow(Graph * g, 
        std::vector<ui> & R, std::set<ui> & Aset,
        std::vector<bool>& overdensed, double InducedDS) {
            
    double alphaBottom = InducedDS;
    double alphaTop = std::min(1.0 * g->maxD, 1.0*R.size());
printf("alphaBIGA %f\n", alphaBottom);
// printf("alphaTIGA %f=min(%f, %f)\n", alphaTop, 1.0 * g->maxD, 1.0*R.size());
    ull flowAlphaMinus = 0;
    double alphaStar = 0.0;
    densestSubgraph ans;

    double volBR = 0.0;
    for(ui u : R) volBR += g->degree(u);

    // Network network;
    // std::vector<Network::Vertex> flowNodes;
    // flowNodes.reserve(g->n);
    // Network::Vertex s = network.AddVertex(), t = network.AddVertex();
    // buildNetWorkIGAOverdensed(g, alphaBottom, network,
    //      flowNodes, s, t, R, overdensed, volBR);
    densestSubgraph den = 
        flownetworkAlphaImproved(g, alphaBottom, R, Aset, overdensed, volBR);
    double fValue = den.alphaStar;
// printf("Init %f %f\n", fValue, volBR);fflush(stdout);
    if(fValue+1e-6 >= volBR) {
        double tmpSmallAlpha = alphaBottom-1.0/(1.0*(g->n+2)*(g->n+1));
        do {
            ans = 
                flownetworkAlphaImproved(g, 
                    tmpSmallAlpha, R, Aset, overdensed, volBR);
            tmpSmallAlpha -= 1e-5;
        }while(ans.sourceNodes.size() == 0);
        
        ans.alphaStar = alphaBottom;
    }
    else {
        while(true) {
            double alpha = (alphaBottom + alphaTop) / 2;

            densestSubgraph den = 
                flownetworkAlphaImproved(g, alpha, R, Aset, overdensed, volBR);
            double fValue = den.alphaStar;
printf("L %f, R %f, mid %f\n", alphaBottom, alphaTop, alpha);
printf("f %f, m %f\n", fValue, volBR);
            if(fValue+1e-4 >= volBR) alphaTop = alpha;
            else alphaBottom = alpha;

            if((alphaTop - alphaBottom)*(g->n+2) <= 1e-3+1.0/(1.0*(g->n+1))) {
                double tmpSmallAlpha = alphaBottom-1.0/(1.0*(g->n+2)*(g->n+1));
                
                do {
                    ans = flownetworkAlphaImproved(g, tmpSmallAlpha, R, Aset, overdensed, volBR);
                    tmpSmallAlpha -= 1e-5;
                } while(ans.sourceNodes.size() == 0);
                
                ull len = ans.sourceNodes.size();

                ull sum = alphaBottom * len;
                sum += 1;
                double realAlpha = 1.0 * sum / len;
                ans.alphaStar = realAlpha;
// printf("IGAlen:%llu, IGAalpha %f\n", len, realAlpha);
                break;
            }
        }
    }

    return ans;
}

LADS::densestSubgraph LADS::flownetworkAlphaImproved(Graph * g, double alpha, 
    std::vector<ui> & R, std::set<ui> & Aset,
    std::vector<bool>& overdensed, double volBR) {
    Network network;
    std::vector<Network::Vertex> flowNodes;
    flowNodes.resize(g->n+2);
    flowNodes.clear();
    network.preAllocMemoryForV(g->n+2);

    Network::Vertex s = network.AddVertex(), t = network.AddVertex();
// printf("here1\n");fflush(stdout);
    buildNetWorkGAImproved(g, alpha, network, 
        flowNodes, s, t
       ,R, Aset, overdensed, volBR);
// printf("here2\n");fflush(stdout);  
    double fValue = network.MaxFlow(s, t);
    densestSubgraph ans;
    ans.alphaStar = fValue;//arbitrary usage

    network.findNeighborsByBFS(s, flowNodes, ans.sourceNodes);

    return ans;
}


// void LADS::modifyAlphaAndResetNetworkOverdensed(Graph * g, double alpha, Network & network,
//         std::vector<Network::Vertex> & flowNodes,
//         Network::Vertex & s,
//         Network::Vertex & t,
//         std::vector<bool> & overdensed,
//         double volBR) {
//     // std::set<ui> stR(R.begin(), R.end());
//     for(ui u = 0; u < g->n; u++) {
//         if(overdensed[u]) {
//             // if(stR.find(u) != stR.end()) {
//             //     network.modifyCapacity(volBR+1, flowNodes[u], g->degree(u), s, t);
//             // }
//             // else {
//             //     network.modifyCapacity(volBR+1, flowNodes[u], g->degree(u), s, t);
//             // }
//             network.modifyCapacity(volBR+1, flowNodes[u], g->degree(u), s, t);
//         }
//         else {
//             network.modifyCapacity(alpha, flowNodes[u], g->degree(u), s, t);
//         }
//     }
// }