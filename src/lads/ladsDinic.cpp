#include "ladsDinic.h"
#include <algorithm>

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
        if(ans.sourceNodes.capacity() < den.sourceNodes.size()) {
            ans.sourceNodes.resize(den.sourceNodes.size());
        }
        ans.sourceNodes.clear();
        for(ui u : den.sourceNodes) ans.sourceNodes.push_back(L[u]);

        // SUnion = union(SUnion, S)
        // for(ui u : S) SUnion.insert(u);

        //Frontier = setdiff(Snew, Expanded)
        Frontier.clear();
        for(ui u : ans.sourceNodes) if(!isInExpaneded[u]) {
            Frontier.push_back(u);
        }
    }

    // ans.sourceNodes.resize(S.size());
    // ans.sourceNodes.clear();
    // for(ui u : S) ans.sourceNodes.push_back(u);

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

    ui N = g->n+2;
    ui M = R.size()+g->m+g->n;
    Network::Vertex s = 0, t = N-1;
    Network network(N, M, s, t);

    buildNetWorkIGAOverdensed(g, alphaBottom, network, s, t, R,
        Aset, overdensed, volBR);
// double st = clock();
    double fValue = network.maxFlow(1e17);
// double ed = clock();
// printf("%f\n", (ed-st)/ CLOCKS_PER_SEC);

// modifyAlphaAndResetNetworkOverdensed(g, alphaBottom, network, s, t, R, Aset, overdensed, volBR);
// fValue = network.maxFlow(1e17);
printf("Init %f %f\n", fValue, volBR);
    if(fValue+1e-6 >= volBR) {
        network.getSTCut(ans.sourceNodes);
        double tmpSmallAlpha = alphaBottom-1.0/(1.0*(g->n+2)*(g->n+1));

        while(ans.sourceNodes.size() == 0) {
// printf("aa\n");
            tmpSmallAlpha -= 1e-5;
            modifyAlphaAndResetNetworkOverdensed(g, tmpSmallAlpha, network, s, t, R, Aset, overdensed, volBR);
            double fValue = network.maxFlow(1e17);
            network.getSTCut(ans.sourceNodes);
        }
        for(ui & u : ans.sourceNodes) u--;//start from 1
        
        ans.alphaStar = alphaBottom;
    }
    else {
        while(true) {
            double alpha = (alphaBottom + alphaTop) / 2;

            modifyAlphaAndResetNetworkOverdensed(g, alpha, network, s, t, R, Aset, overdensed, volBR);
            double fValue = network.maxFlow(1e15);
// printf("L %f, R %f, mid %f\n", alphaBottom, alphaTop, alpha);
// printf("f %f, m %f\n", fValue, volBR);
            if(fValue+1e-8 >= volBR) alphaTop = alpha;
            else alphaBottom = alpha;

            if((alphaTop - alphaBottom)*(g->n+2) <= 1e-5+1.0/(1.0*(g->n+1))) {
                double tmpSmallAlpha = alphaBottom-1.0/(1.0*(g->n+2)*(g->n+1));
                
                do {
                    tmpSmallAlpha -= 1e-5;
// printf("tmpSmallAlpha %f\n", tmpSmallAlpha);fflush(stdout);
                    modifyAlphaAndResetNetworkOverdensed(g, tmpSmallAlpha, network, s, t, R, Aset, overdensed, volBR);
                    double fValue = network.maxFlow(1e15);
                    network.getSTCut(ans.sourceNodes);
                } while(ans.sourceNodes.size() == 0);
                for(ui & u : ans.sourceNodes) u--;

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

void LADS::buildNetWorkIGAOverdensed(
        Graph * g, double alpha, Network & network,
        Network::Vertex & s,
        Network::Vertex & t, 
        std::vector<ui> & R,
        std::set<ui> & Aset,
        std::vector<bool> & overdensed,
        double volBR){
    for(ui u : R) {
        if(Aset.find(u) == Aset.end())
            network.addEdge(s, u+1, g->degree(u));
        else
            network.addEdge(s, u+1, 1e15);
    }

    for(ui u = 0; u < g->n; u++) {
        for(ui i = g->pIdx[u]; i < g->pIdx[u+1]; i++) {
            ui v = g->pEdge[i];
            network.addEdge(u+1, v+1, 1);
        }
    }
// printf("%lu\n", overdensed.size());
// for(ui u : Aset) printf("%u %d ", u, (int)overdensed[u]); printf("\n");fflush(stdout);
    for(ui u = 0; u < g->n; u++) {
        if(overdensed[u])
            network.addEdge(u+1, t, volBR+1);
        else  {
            network.addEdge(u+1, t, alpha);
        }
    }  
}

void LADS::modifyAlphaAndResetNetworkOverdensed(Graph * g, double alpha, Network & network,
        Network::Vertex & s,
        Network::Vertex & t,
        std::vector<ui> & R,
        std::set<ui> & Aset,
        std::vector<bool> & overdensed,
        double volBR) {
    ui eCnt = 0;
    for(ui u : R) {
        if(Aset.find(u) == Aset.end())
            network.reSetEdge(eCnt++, g->degree(u));
        else
            network.reSetEdge(eCnt++, 1e15);
    }

    for(ui u = 0; u < g->n; u++) {
        for(ui i = g->pIdx[u]; i < g->pIdx[u+1]; i++) {
            ui v = g->pEdge[i];
            network.reSetEdge(eCnt++, 1);
        }
    }
// printf("%lu\n", overdensed.size());
// for(ui u : Aset) printf("%u %d ", u, (int)overdensed[u]); printf("\n");fflush(stdout);
    for(ui u = 0; u < g->n; u++) {
        if(overdensed[u])
            network.reSetEdge(eCnt++, volBR+1);
        else  {
            network.reSetEdge(eCnt++, alpha);
        }
    }  
}