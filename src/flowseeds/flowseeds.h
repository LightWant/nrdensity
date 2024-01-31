#ifndef FLOWSEEDS
#define FLOWSEEDS

#include "../tools/types.hpp"
#include "../graph/graph.h"
#include "../tools/HLPP.hpp"
#include <vector>
#include <set>

namespace flowseeds {
    struct community {
        double alphaStar;
        std::vector<ui> sourceNodes;
    };
    typedef HLPP Network;

    community run(Graph * g, 
        const std::vector<ui> & R, 
        const std::vector<ui> & A);

    void globalPushRelabel(Graph * g, 
        const std::vector<ui> & R, double alpha, 
        const std::vector<bool> & inR, std::vector<ui> & res);
    
    void localPushRelabel(Graph * g, 
        const std::vector<ui> & R, double alpha,
        const std::set<ui> & Rn, 
        const std::vector<bool> & inR, std::vector<ui> & res);
    
    double cutval(Graph * g, const std::vector<ui> & S,
        const std::vector<ui> & R,  const std::vector<bool> & inR,
        const std::vector<ui> & A);
};

#endif