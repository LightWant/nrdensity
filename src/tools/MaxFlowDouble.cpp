#define BOOST_DISABLE_ASSERTS
#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include "MaxFlowDouble.h"
using namespace std;
using namespace boost;

Network::Network(): g(), rev(get(edge_reverse, g)), capacity(get(edge_capacity, g)), residual_capacity(get(edge_residual_capacity, g)) {}

Network::Vertex Network::AddVertex() {
  return add_vertex(g);
}
void Network::preAllocMemoryForV(Network::Vertex n) {
  Vertex preSize = g.m_vertices.size();
  g.m_vertices.resize(n);
  g.m_vertices.resize(preSize);
}


Network::Edge Network::AddEdge(Vertex &v1, Vertex &v2, const double capacity) {
  Traits::edge_descriptor e1 = add_edge(v1, v2, g).first;
  Traits::edge_descriptor e2 = add_edge(v2, v1, g).first;
  put(edge_capacity, g, e1, capacity);
  rev[e1] = e2;
  rev[e2] = e1;
  return e1;
}

double Network::MaxFlow(Vertex &s, Vertex &t) {
  return push_relabel_max_flow(g, s, t); // Boost library also provides boykov_kolmogorov_max_flow (needs "#include <boost/graph/boykov_kolmogorov_max_flow.hpp>")
}

struct Network::NonSaturatedEdges {
  NonSaturatedEdges() {}
  NonSaturatedEdges(property_map<Graph, edge_residual_capacity_t>::type residual_capacity):
    residual_capacity(residual_capacity) {}
  property_map<Graph, edge_residual_capacity_t>::type residual_capacity;
  bool operator ()(const Edge &e) const {
    return residual_capacity[e] > 1e-7;
  }
};

void Network::BfsOnResidualGraph(
Vertex &s, std::vector<Network::Vertex> & R) {
  NonSaturatedEdges filter(get(edge_residual_capacity, g));
  filtered_graph<Graph, NonSaturatedEdges> fg(g, filter);
  property_map<Graph, vertex_color_t>::type color = get(vertex_color, g);
  boost::queue<Vertex> Q;
  default_bfs_visitor vis;
  breadth_first_search(fg, s, Q, vis, color);

  typedef property_map<Graph, vertex_color_t>::type ColorMap;
  typedef typename property_traits< ColorMap >::value_type ColorValue;
  typedef color_traits< ColorValue > Color;
  for(unsigned i = 0; i < R.size(); i++) {
    printf("%u %u %u\n", i, color[R[i]], Color::white());
  }
}
void Network::findNeighborsByBFS(Vertex &s, std::vector<Network::Vertex> & R, std::vector<unsigned> & nodes) {
  NonSaturatedEdges filter(get(edge_residual_capacity, g));
  filtered_graph<Graph, NonSaturatedEdges> fg(g, filter);
  color = get(vertex_color, g);

  typedef property_map<Graph, vertex_color_t>::type ColorMap;
  typedef typename property_traits< ColorMap >::value_type ColorValue;
  typedef color_traits< ColorValue > Color;

  put(color, s, Color::gray());
  for(unsigned i = 0; i < R.size(); i++) {
    put(color, R[i], Color::white());
  }
// printf("s %u \n",  color[s]);
//   for(unsigned i = 0; i < R.size(); i++) {
// printf("%u %u \n", i, color[R[i]]);
//   }
  
  boost::queue<Vertex> Q;
  default_bfs_visitor vis;
  breadth_first_search(fg, s, Q, vis, color);

  nodes.clear();
// printf("s2 %u, gray %u, black %u \n",
  // color[s], Color::gray(), Color::black());
  for(unsigned i = 0; i < R.size(); i++) {
// printf("%u %u %u\n", i, color[R[i]], color[s]);
    if(color[s] == color[R[i]]) {
      nodes.push_back(i);
    }
  }
}

void Network::modifFyCapacity(double alpha, double reAlpha, const Vertex &t) {
  boost::graph_traits<Graph>::out_edge_iterator eo, eo_end;

  for( boost::tie(eo, eo_end) = out_edges(t, g); eo != eo_end; ++eo)
  {
      put(edge_capacity, g, *eo, reAlpha);
      Traits::edge_descriptor ei = boost::edge(boost::target(*eo, g), t, g).first;
      put(edge_capacity, g, ei, alpha);
  }

}

void Network::modifyCapacity(double alpha, Vertex &u, unsigned deg, Vertex & s, Vertex & t) {
  boost::graph_traits<Graph>::out_edge_iterator eo, eo_end;
  for( boost::tie(eo, eo_end) = out_edges(u, g); eo != eo_end; ++eo) {
      if(boost::target(*eo, g) == s) {
        put(edge_residual_capacity, g, *eo, 0.0);
        // Traits::edge_descriptor ei = boost::edge(s, u, g).first;
        put(edge_capacity, g, rev[*eo], deg);
      }
      else if(boost::target(*eo, g) == t) {
        put(edge_capacity, g, *eo, alpha);
        // Traits::edge_descriptor ei = boost::edge(t, u, g).first;
        put(edge_residual_capacity, g, rev[*eo], 0.0);
      }
      else {
        put(edge_capacity, g, *eo, 1.0);
        // Traits::edge_descriptor ei = boost::edge(boost::target(*eo, g), u, g).first;
        put(edge_residual_capacity, g, rev[*eo], 0.0);
      }
  }
}
