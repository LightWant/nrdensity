#include "bufferKclist.h"
#include <random>
#include <cassert>

void bufferKclist::scan(
    std::function<
        void(const std::vector<ui>::iterator &, 
            const std::vector<ui>::iterator &)> f
    ) 
{
    cntClique = 0;
    cliques.clear();
    for(ui u = 0; u < g->n; u++) {
        ui d = g->pIdx[u+1] - g->pIdx2[u];
        for(ui i = 0; i < d; i++) g2sg[g->pEdge[g->pIdx2[u] + i]] = i;
        for(ui i = 0; i < d; i++) sg2g[i] = g->pEdge[g->pIdx2[u] + i];
        for(ui i = 0; i < d; i++) cand[0][i] = i;
        for(ui i = 0; i < d; i++) label[i] = 0;
        for(ui i = 0; i < d; i++) {
            deg[0][i] = 0;
            ui v = g->pEdge[g->pIdx2[u] + i];
            ui dv = g->pIdx[v+1] - g->pIdx2[v];
            for(ui j = 0; j < dv; j++) {
                ui w = g->pEdge[g->pIdx2[v] + j];
                if(g2sg[w] < d) sg[i][deg[0][i]++] = g2sg[w]; 
            }
        }

        stk.clear();
        stk.push_back(u);
        kclist(d, 0, f);

        for(ui i = 0; i < d; i++) g2sg[g->pEdge[g->pIdx2[u] + i]] = UINT32_MAX;
    }
    if(cntClique > 0) shuffle_fflush(f);
}

void bufferKclist::kclist(ui sz, ui deep, std::function<
        void(const std::vector<ui>::iterator &, 
            const std::vector<ui>::iterator &)> & f)
{
    std::vector<ui> & d = deg[deep];
    std::vector<ui> & cd = cand[deep];

    if(deep + 3 == k) {
        for(ui i = 0; i < sz; i++) {
            ui sgu = cd[i];
            for(ui j = 0; j < d[sgu]; j++) {
                ui sgv = sg[sgu][j];
                if((cntClique+1)*k > bufferSize) {
// printf("here %u %u %u\n",cntClique,k,bufferSize);fflush(stdout);
                    shuffle_fflush(f);
                }

                cntClique++;
                for(auto v : stk) {
                    cliques.push_back(v);
// assert(v < g->n);
                }
// assert(sg2g[sgu] < g->n);
// assert(sg2g[sgv] < g->n);
// assert(stk.size() == k-2);
                cliques.push_back(sg2g[sgu]);
                cliques.push_back(sg2g[sgv]);
            }
        }
        return;
    }

    for(ui i = 0; i < sz; i++) {
        ui sgu = cd[i];
        ui dsgu = d[sgu];

        if(deep+1+dsgu+1 < k) continue;

        ui newSz = 0;
        for(ui j = 0; j < dsgu; j++) {
            ui sgv = sg[sgu][j];
            // if(label[sgv] == deep) {
                label[sgv] = deep + 1;
                cand[deep+1][newSz++] = sgv;
            // }
        }
        
        for(ui j = 0; j < newSz; j++) {
            ui sgv = cand[deep+1][j];
            ui & newD = deg[deep+1][sgv];
            newD = d[sgv];
            for(ui l = 0; l < newD; ) {
                if(label[sg[sgv][l]] == deep+1) l++;
                else std::swap(sg[sgv][l], sg[sgv][--newD]);
            } 
        }

        stk.push_back(sg2g[sgu]);
        kclist(newSz, deep+1, f);
        stk.pop_back();

        for(ui j = 0; j < newSz; j++) {
            label[cand[deep+1][j]] = deep;
        }
    }
}

void bufferKclist::shuffle() {
    std::random_device rd;
    std::mt19937 gg(rd());
    
    typedef std::uniform_int_distribution<ui> distr_t;
    typedef typename distr_t::param_type param_t;
    
    distr_t D;
    if(cntClique > 1)
    for(ui i = cntClique-1; i > 0; i--) {
        ui j = D(gg, param_t(0, i));
        for(ui l = 0; l < k; l++) {
            std::swap(cliques[i*k+l], cliques[j*k+l]);
        }
    }
}

void bufferKclist::shuffle_fflush(std::function<
        void(const std::vector<ui>::iterator &, 
            const std::vector<ui>::iterator &)> & f)
{
// printf("here1\n");fflush(stdout);
    shuffle();
// printf("here2\n");fflush(stdout);
    for(ui i = 0; i < cntClique; i++) {
// printf("here_i%u_%u", cntClique,i);
// for(ui j = i*k; j < i*k+k; j++) {
//     printf(" %u", cliques[j]);
// }printf("\n");
// fflush(stdout);
        f(cliques.begin()+i*k, cliques.begin()+i*k+k);
    }
// printf("here3\n");fflush(stdout);
    cntClique = 0;
    cliques.clear();
}