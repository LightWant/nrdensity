#include "flowseeds.h"
#include <set>

flowseeds::community flowseeds::run(Graph * g, 
    const std::vector<ui> & R,
    const std::vector<ui> & A) {
    //pR = 0 in default
    //eps = 1.0

    auto getVolume = [](Graph * g, const std::vector<ui> & vs) {
        double ans = 0.0;
        for(ui u : vs) ans += g->degree(u);
        return ans;
    };

    std::vector<bool> inS(g->n);
    auto getCondunctance = [&](
        Graph * g, const std::vector<ui> & S, double volG) {
        double volS = getVolume(g, S);

        double edges = 0;
        for(auto u : S) inS[u] = true;
        for(ui u : S) {
            for(ui i = g->pIdx2[u]; i < g->pIdx[u+1]; i++) {
                ui v = g->pEdge[i];
                if(inS[v]) edges++;
            }
        }
        for(auto u : S) inS[u] = false;

        double cut = volS - edges*2;
// printf("vol:%f\n", volS);
// printf("cut:%f\n", cut);
        return cut / std::min(volS, volG - volS);
    };
// printf("volA:%f\n", 1.0*g->m);
    double alphaCurrent = getCondunctance(g, R, g->m);

    std::vector<ui> BestS = R;
    double alpha0 = 2;
    double alphaBest = alphaCurrent;
    double alphaS;

    std::vector<bool> & inR = inS;

    for(auto u : R) inR[u] = true;
    std::set<ui> Rn;

    for(ui u : R) {
        for(ui i = g->pIdx[u]; i < g->pIdx[u+1]; i++) {
            ui v = g->pEdge[i];
            if(inR[v]) continue;
            Rn.insert(v);
        }
    }

    std::vector<ui> S;

    printf("alphaCurrent:%f\n", alphaCurrent);

    while(alphaCurrent+1e-5 < alpha0) {
        //alphaCurrent*(pR .+ 1).*dr
        // globalPushRelabel(
        //         g, R, alphaCurrent, inR, S);
        localPushRelabel(
                g, R, alphaCurrent, Rn, inR, S);
// printf("S:");
// for(ui u : S) printf("%u ", u); printf("\n");
printf("ssize:%lu\n", S.size());
        if(0 < S.size() && S.size() < g->n) {
            // alphaS = cutval(A,S,R,d,1.0,epsilon,volA,pR,RinS);
            alphaS = cutval(g, S, R, inR, A);
            
            if(alphaS < alphaCurrent) {
                BestS = S;
                alphaBest = alphaS;
            }
            else {
                alphaS = alphaCurrent;
            }
        }

        alpha0 = alphaCurrent;
        alphaCurrent = alphaS;
    }

    community ans;
    ans.alphaStar = alphaBest;
    ans.sourceNodes = BestS;

    return ans;
}

void flowseeds::globalPushRelabel(
    Graph * g, const std::vector<ui> & R, double alpha
    , const std::vector<bool> & inR, std::vector<ui> & res) {
    // Graph * sg;
    // g->buildSgFromNodes2(R, sg);

    ui N = g->n+2;
    ui M = g->m+g->n;
    Network::Vertex s = 0, t = N-1;
    Network network(N, M, s, t);

    // buildNetWork(g, network, s, t);
    for(ui u : R) network.addEdge(s, u+1, alpha*g->degree(u));
    for(ui u = 0; u < g->n; u++) {
        if(inR[u]) continue;
        network.addEdge(u+1, t, alpha*g->degree(u));
    }
    for(ui u = 0; u < g->n; u++) {
        for(ui i = g->pIdx[u]; i < g->pIdx[u+1]; i++) {
            ui v = g->pEdge[i];
            network.addEdge(u+1, v+1, 1.0);
        }
    }
// network.print2();
    double fValue = network.maxFlow();
    res.clear();
    network.getSTCut(res);
    for(auto & u : res) u--;
}

double flowseeds::cutval(Graph * g, const std::vector<ui> & S,
        const std::vector<ui> & R, const std::vector<bool> & inR,
        const std::vector<ui> & A) {
    double volA = g->m;

    double volS = 0.0;
    for(ui u : S) volS += g->degree(u);

    double edgeS = 0.0;
    std::set<ui> Sset(S.begin(), S.end());
    for(ui u : S) {
        for(ui i = g->pIdx2[u]; i < g->pIdx[u+1]; i++) {
            ui v = g->pEdge[i];
            if(Sset.find(v) != Sset.end()) {
                edgeS += 1.0;
            }
        }
    }

    double cutS = volS - edgeS*2;

    double volR = 0.0;
    for(ui u : R) volR += g->degree(u);

    double volRS = 0.0;
    for(ui u : S) if(inR[u]) {
        volRS += g->degree(u);
    }

    double cutScore = cutS - volRS + volR + (volS-volRS);

    if(cutScore < 0) {
        printf("error cntScore\n");fflush(stdout);
    }
    double relcond = cutS / (volRS - (volS-volRS));

printf("cts %f\nvolRs %f\nvolS %f\nedges %f\nrealcond %f xx\n\n", 
    cutS, volRS, volS,  edgeS*2, relcond);

    return relcond;
}

void flowseeds::localPushRelabel(Graph * g, 
        const std::vector<ui> & R, double alpha,
        const std::set<ui> & Rn, 
        const std::vector<bool> & inR, std::vector<ui> & res) {
    std::vector<ui> C_global = R;
    std::vector<ui> I_global(Rn.begin(), Rn.end());
    std::vector<bool> isInC_global(g->n);
    for(ui u : C_global) isInC_global[u] = true;
    std::vector<bool> isInI_global(g->n);
    for(ui u : I_global) isInI_global[u] = true;

    std::vector<ui> Local2Global(R.size() + Rn.size());
    Local2Global.clear();
    for(auto u : R) Local2Global.push_back(u);
    for(auto u : Rn) Local2Global.push_back(u);

    std::vector<ui> Global2Local(g->n, -1);
    ui Lsize = Local2Global.size();
    for(ui i = 0; i < Lsize; i++) {
        Global2Local[Local2Global[i]] = i;
    }

    std::vector<ui> C_local(R.size());
    std::vector<ui> I_local(Rn.size());
    for(ui i = 0; i < C_local.size(); i++) {
        C_local[i] = i;
    }
    for(ui i = 0; i < I_local.size(); i++) {
        I_local[i] = i + C_local.size();
    }
    ui numI = I_local.size();

    // std::vector<ui> S_local(Lsize);
    std::vector<ui> & S_local = res;
    S_local.clear();

    std::vector<ui> E_local;
printf("Lsize %u\n", Lsize);
printf("I_local %lu %lu\n", I_local.size(), I_global.size());
printf("C_local %lu %lu\n", C_local.size(), C_global.size());

//Local2Global, Global2Local
// C_local, C_global isInC_global
//I_local, I_global isInI_global
    auto buildNetwork = [&](Network & network, 
        Network::Vertex s, Network::Vertex t) {
ui M = 0;
ui L = 0;
        for(ui ul : C_local) if(inR[Local2Global[ul]]) {
            network.addEdge(s, ul+1, alpha*g->degree(Local2Global[ul]));
M++;
        }
            
        for(ui ul : I_local) if(!inR[Local2Global[ul]]) {
                network.addEdge(ul+1, t, alpha*g->degree(Local2Global[ul]));
M++;
        }

        for(ui ul : C_local) {
            ui u = Local2Global[ul];
            for(ui i = g->pIdx[u]; i < g->pIdx[u+1]; i++) {
                ui v = g->pEdge[i];
                ui vl = Global2Local[v];
                if(isInC_global[v] ) {
                    network.addEdge(ul+1, vl+1, 1.0);
M++;L++;
                }
                else if(isInI_global[v]) {
                    network.addEdge(ul+1, vl+1, 1.0);
                    network.addEdge(vl+1, ul+1, 1.0);
M += 2;L+=2;
                }
            }
        }
    // network.print2();
printf("realM %u\n", M);  
printf("L %u\n", L);   
    };
    auto step1 = [&]() {
        ui N = Lsize + 2;
        ui M = 0;
        for(ui ul : C_local) if(inR[Local2Global[ul]]) M += 1;
        for(ui ul : I_local) if(!inR[Local2Global[ul]]) M += 1;
        for(ui u : C_global) {
            for(ui i = g->pIdx[u]; i < g->pIdx[u+1]; i++) {
                ui v = g->pEdge[i];
                if(isInC_global[v]) M += 1;//AcToc
                else if(isInI_global[v]) M += 2; //AcToI
            }
        }
// printf("M %u\n", M);

        Network::Vertex s = 0, t = N-1;
        Network network(N, M, s, t);

        buildNetwork(network, s, t);
        double fValue = network.maxFlow();
        
        S_local.clear();
        network.getSTCut(S_local);
        for(auto & u : S_local) u--;
printf("S_local_size %lu\n", S_local.size());
    };

    auto updateL = [&]() {
        // E_local = setdiff(S_local,C_local) 
        E_local.clear();
        for(ui ul : S_local) {
            if(isInC_global[Local2Global[ul]]) continue;
            E_local.push_back(ul);
        }
printf("E_local_size %lu\n", E_local.size());
printf("\n");

        if(E_local.size() == 0) return false;

        //C_local
        for(ui ul : E_local) C_local.push_back(ul);
        //C_global
        for(ui ul : E_local) {
            C_global.push_back(Local2Global[ul]);
            isInC_global[Local2Global[ul]] = true;
        }

        // I_local = setdiff(I_local,E_local)
        // I_global = Local2Global[I_local]
        ui newI_local_size = 0;
        ui newI_global_size = 0;
        for(ui ul : E_local) {
            ui u = Local2Global[ul];
            if(isInI_global[u]) {
                isInI_global[u] = false;
            }
        }
        for(ui i = 0; i < I_global.size(); i++) {
            ui u = I_global[i];
            if(isInI_global[u] == true) {
                I_global[newI_global_size++] = u;
            }
        }
        I_global.resize(newI_global_size);
        for(ui i = 0; i < I_local.size(); i++) {
            ui ul = I_local[i];
            ui u = Local2Global[ul];
            if(isInI_global[u] == true) {
                I_local[newI_local_size++] = ul;
            }
        }
        I_local.resize(newI_local_size);

        //Lnew

        for(ui ul : E_local) {
            ui u = Local2Global[ul];
            for(ui i = g->pIdx[u]; i < g->pIdx[u+1]; i++) {
                ui v = g->pEdge[i];
                if(isInC_global[v] || isInI_global[v]) continue;
                Local2Global.push_back(v);
                Global2Local[v] = Lsize;
                I_global.push_back(v);
                isInI_global[v] = true;
                I_local.push_back(Lsize++);
            }
        }
printf("Lsize %u\n", Lsize);
printf("I_local %lu %lu\n", I_local.size(), I_global.size());
printf("C_local %lu %lu\n", C_local.size(), C_global.size());
// printf("\n");
        return true;
    };

    while(true) {
        step1();//get S_local
        if(!updateL()) break;
    }

    for(ui & u : S_local) u = Local2Global[u];
}

// void flowseeds::localPushRelabel(Graph * g, 
//         const std::vector<ui> & R, double alpha,
//         const std::vector<ui> & Rn, 
//         const std::vector<bool> & inR, std::vector<ui> & res) {
//     std::vector<ui> C_global = R;
//     std::vector<ui> I_global = Rn;
//     std::vector<bool> isInC_global(g->n);
//     for(ui u : C_global) isInC_global[u] = true;
//     std::vector<bool> isInI_global(g->n);
//     for(ui u : I_global) isInI_global[u] = true;

//     std::vector<ui> Local2Global(R.size() + Rn.size());
//     Local2Global.clear();
//     for(auto u : R) Local2Global.push_back(u);
//     for(auto u : Rn) Local2Global.push_back(u);

//     ui Lsize = Local2Global.size();
//     std::vector<ui> Global2Local(g->n, -1);
//     for(ui i = 0; i < Lsize; i++) {
//         Global2Local[Local2Global[i]] = i;
//     }

//     std::vector<ui> C_local(R.size());
//     std::vector<ui> I_local(Rn.size());
//     for(ui i = 0; i < C_local.size(); i++) {
//         C_local[i] = i;
//     }
//     for(ui i = 0; i < I_local.size(); i++) {
//         I_local[i] = i + C_local.size();
//     }

//     ui numI = I_local.size();

//     // AcToI; AcToc
//     // L = [AcToc AcToI; 
//     // AcToI' spzeros(numI,numI)]

//     // tToL = reshape(tWeights[Local2Global],Lsize)
//     // sToL = reshape(sWeights[Local2Global],Lsize)

//     //build network
//     ui N = Lsize + 2;
//     ui M = 0;
//     for(ui ul : C_local) if(inR[Local2Global[ul]]) M += 1;
//     for(ui ul : I_local) if(!inR[Local2Global[ul]]) M += 1;
//     for(ui u : C_global) {
//         for(ui i = g->pIdx[u]; i < g->pIdx[u+1]; i++) {
//             ui v = g->pEdge[i];
//             if(isInC_global[v]) M += 1;//AcToc
//             else if(isInI_global[v]) M += 2; //AcToI
//         }
//     }

//     Network::Vertex s = 0, t = N-1;
//     Network network(N, M, s, t);

//     for(ui ul : C_local) if(inR[Local2Global[ul]]) 
//         network.addEdge(s, ul+1, alpha*g->degree(Local2Global[ul]));
//     for(ui ul : I_local) if(!inR[Local2Global[ul]]) 
//         network.addEdge(ul+1, t, alpha*g->degree(Local2Global[ul]));

//     for(ui ul : C_local) {
//         ui u = Local2Global[ul];
//         for(ui i = g->pIdx[u]; i < g->pIdx[u+1]; i++) {
//             ui v = g->pEdge[i];
//             ui vl = Global2Local[v];
//             if(isInC_global[v] ) {
//                 network.addEdge(ul+1, vl+1, 1.0);
//             }
//             else if(isInI_global[v]) {
//                 network.addEdge(ul+1, vl+1, 1.0);
//                 network.addEdge(vl+1, ul+1, 1.0);
//             }
//         }
//     }
// // network.print2();
//     double fValue = network.maxFlow();
//     std::vector<ui> S_local(Lsize);
//     S_local.clear();
//     network.getSTCut(S_local);
//     for(auto & u : S_local) u--;

// // E_local = setdiff(S_local,C_local)
//     std::vector<ui> E_local;
//     for(ui ul : S_local) {
//         if(isInC_global[Local2Global[ul]]) continue;
//         E_local.push_back(ul);
//     }

//     while(E_local.size() > 0) {
//         for(ui ul : E_local) {
//             C_local.push_back(ul);
//             C_global.push_back(Local2Global[ul]);
//         }
        
//         // I_local = setdiff(I_local,E_local)
//         ui newI_local_size = 0;
//         for(ui ul : E_local) {
//             ui u = Local2Global[ul];
//             if(isInI_global[u]) {
//                 isInI_global[u] = false;
//             }
//         }
//         for(ui i = 0; i < I_local.size(); i++) {
//             ui ul = I_local[i];
//             ui u = Local2Global[ul];

//             if(isInI_global[u] == false) {
//                 I_local[newI_local_size++] = ul;
//             }
//         }
//         // I_global = Local2Global[I_local]

//         //build network

//     }
// }