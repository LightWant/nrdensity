#include <vector>
#include <queue>
#include <cstring>
#include <cstdio>

template<typename vT, typename eT, typename fT>
struct FlowGraph {
	vT n;
    eT m;
	std::vector<vT> head, vertex, nxt;
	std::vector<fT> capcity;
	eT cnt = 0;

	void init(vT n, eT m) {
		this->n = n;
		this->m = m;
		head.resize(n + 1, -1);
		vertex.resize(m * 2 + 1);
		nxt.resize(m * 2 + 1, -1);
		capcity.resize(m * 2 + 1);
		cnt = 0;
	}

	void addEdge(vT u, vT v, fT w) {
		vertex[cnt] = v;
		capcity[cnt] = w;
		nxt[cnt] = head[u];
		head[u] = cnt++;
	}
};

template<typename vT, typename eT, typename fT>
struct Dinic
{
	typedef vT Vertex;
	typedef eT Edge;

	std::vector<vT> distance;//限制增广路不要重复走点
    std::vector<vT> currentArc;
	vT s, t, n, m;
	FlowGraph<vT, eT, fT> g;

	Dinic(vT n, eT m, vT s, vT t):n(n), m(m), s(s), t(t) {
		g.init(n, m);
		distance.resize(n + 1);
        currentArc.resize(n + 1);
	}

	void addEdge(vT u, vT v, fT w, fT existedFlow=0) {
		g.addEdge(u, v, w-existedFlow);
		g.addEdge(v, u, existedFlow);//反向边
	}

	bool bfs() {
		std::fill(distance.begin(), distance.end(), -1);
		
        std::queue<vT> que;
        que.push(s);
	    distance[s] = 0;

        while(!que.empty()) {
            vT u = que.front(); que.pop();
			if (u == t) return true;

            for(eT e = g.head[u]; ~e; e = g.nxt[e]) {
                vT v = g.vertex[e];
                if(g.capcity[e] < 1e-5 || distance[v] != -1) continue;

                distance[v] = distance[u] + 1;
                que.push(v);
            }
        }
        
        return distance[t] != -1;
	}

    fT dfs(vT u, fT inFlow) {
        fT outFlow = 0;
		for (eT e = currentArc[u]; ~e && outFlow < inFlow; e = g.nxt[e]) {
            currentArc[u] = e;

            vT v = g.vertex[e];
			if (g.capcity[e] < 1e-5 || distance[v] != distance[u] + 1)
				continue;
            
            fT minF = std::min(g.capcity[e], inFlow - outFlow);
			fT pathFlow = v == t ? minF : dfs(v, minF);
            outFlow += pathFlow;
			
            g.capcity[e] -= pathFlow;
            g.capcity[e ^ 1] += pathFlow;
		}

		return outFlow;
	}

	fT maxFlow(fT INF) {
		fT totalFlow = 0;

        while(bfs()) {
	// printf("bfs\n");fflush(stdout);
            memcpy(currentArc.data(), g.head.data(), sizeof(vT) * (g.n + 1));
    // printf("copy !\n", totalFlow);fflush(stdout);
			totalFlow += dfs(s, INF);
// printf("nowFlow %llu\n", totalFlow);fflush(stdout);
        }

// totalFlow = 0;
// for (eT e = currentArc[t]; ~e ; e = g.nxt[e]) {
// 	totalFlow += g.capcity[e];
// }
		
		return totalFlow;
	}

	void getSTCut(std::vector<Vertex> & vs) {
		// bfs();
		vs.clear();
		for(vT u = 0; u < n; u++) {
			if(u != s && distance[u] != -1) {
				vs.push_back(u);
			}
		}
	}

	void modifyCapacity(double alpha, Vertex u, double deg, Vertex & s, Vertex & t) {
		for(eT e = g.head[u]; ~e; e = g.nxt[e]) {
			vT v = g.vertex[e];

			if(v == s) {
				g.capcity[e^1] = deg;
				g.capcity[e] = 0;
			}
			else if(v == t) {
				g.capcity[e] = alpha;
				g.capcity[e^1] = 0.0;
// if(e & 1) printf("errorrrrrr\n");
			}
			else if(e & 1){
				g.capcity[e] = 0;
			}
			else {
				g.capcity[e] = deg;
			}
		}
	}

	void reSetEdge(Edge e, double c) {
		g.capcity[2*e] = c;
		g.capcity[2*e+1] = 0;
	}

	void print() {
		for(vT u = 0; u < n; u++) {
			printf("%u:", u);
			for(eT e = g.head[u]; ~e; e = g.nxt[e]) {
				vT v = g.vertex[e];
				printf("%u ", v);
			}
			printf("\n");
		}
	}
	void print2() {
		for(vT u = 0; u < n; u++) {
			printf("%u:", u);
			for(eT e = g.head[u]; ~e; e = g.nxt[e]) {
				vT v = g.vertex[e];
				printf("%u:%.1f ", v, g.capcity[e]);
			}
			printf("\n");
		}
	}
};


// int main() 
// {
// 	int n, m, s, t;
// 	scanf("%d %d %d %d", &n, &m, &s, &t);

// 	Dinic<int, int, double> dinic(n, m, s, t);

// 	for (int i = 1; i <= m; i++) {
// 		int u, v;
// 		double w;
// 		scanf("%d %d %lf", &u, &v, &w);

// 		dinic.addEdge(u, v, w);
// 	}

// 	// FF.print();

// 	printf("%.0f\n", dinic.maxFlow());

// 	return 0;
// }