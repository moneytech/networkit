/*
 * ApproxBetweenness.cpp
 *
 *  Created on: 09.04.2014
 *      Author: cls
 */

#include "ApproxBetweenness.h"
#include "../auxiliary/Random.h"
#include "../properties/Diameter.h"
#include "../graph/Sampling.h"
#include "../graph/Dijkstra.h"
#include "../graph/BFS.h"
#include "../graph/SSSP.h"
#include "../auxiliary/Log.h"

#include <math.h>
#include <algorithm>
#include <memory>
#include <omp.h>

namespace NetworKit {

ApproxBetweenness::ApproxBetweenness(const Graph& G, double epsilon, double delta, count diameterSamples) : Centrality(G, true), epsilon(epsilon), delta(delta), diameterSamples(diameterSamples) {

}


void ApproxBetweenness::run() {
	scoreData.clear();
	scoreData.resize(G.upperNodeIdBound());

	double c = 0.5; // universal positive constant - see reference in paper


	edgeweight vd = 0;
	if (diameterSamples == 0) {
		INFO("estimating vertex diameter pedantically");
		vd = Diameter::estimatedVertexDiameterPedantic(G);
	} else {
		/**
		* This is an optimization which deviates from the original algorithm.
		* Instead of getting an estimate for each of possibly thousands of connected component and taking the maximum,
		* we sample the graph and take the maximum diameter found. This has a high chance of  hitting the component with the maximum vertex diameter.
		*/
		INFO("estimating vertex diameter roughly");
		vd = Diameter::estimatedVertexDiameter(G, diameterSamples);
	}

	INFO("estimated diameter: ", vd);
	r = ceil((c / (epsilon * epsilon)) * (floor(log(vd - 2)) + 1 + log(1 / delta)));

	INFO("taking ", r, " path samples");

	// parallelization:
	count maxThreads = omp_get_max_threads();
	DEBUG("max threads: ", maxThreads);
	std::vector<std::vector<double> > scorePerThread(maxThreads, std::vector<double>(G.upperNodeIdBound()));
	DEBUG("score per thread size: ", scorePerThread.size());

	#pragma omp parallel for
	for (count i = 1; i <= r; i++) {
		count thread = omp_get_thread_num();
		DEBUG("sample ", i);
		// if (i >= 1000) throw std::runtime_error("too many iterations");
		// DEBUG
		// sample random node pair
		node u, v;
		u = Sampling::randomNode(G);
		do {
			v = Sampling::randomNode(G);
		} while (v == u);

		// runs faster for unweighted graphs
		std::unique_ptr<SSSP> sssp;
		if (G.isWeighted()) {
			sssp.reset(new Dijkstra(G, u));
		} else {
			sssp.reset(new BFS(G, u));
		}
		DEBUG("running shortest path algorithm for node ", u);
		sssp->run(); // TODO: this can be optimized by stopping the search once the target node has been reached
		if (sssp->numberOfPaths(v) > 0) { // at least one path between {u, v} exists
			DEBUG("updating estimate for path ", u, " <-> ", v);
			// random path sampling and estimation update
			// node s = v;
			node t = v;
			while (t != u)  {
				// sample z in P_u(t) with probability sigma_uz / sigma_us
				std::vector<std::pair<node, double> > choices;

				for (node z : sssp->getPredecessors(t)) {
					choices.emplace_back(z, sssp->numberOfPaths(z) / (double) sssp->numberOfPaths(t)); 	// sigma_uz / sigma_us
				}
				node z = Aux::Random::weightedChoice(choices);
				assert (z <= G.upperNodeIdBound());
				if (z != u) {
					scorePerThread[thread][z] += 1 / (double) r;
				}
				// s = t;
				t = z;
			}
		}
	}

	INFO("adding thread-local scores");
	// add up all thread-local values
	for (auto local : scorePerThread) {
		G.parallelForNodes([&](node v){
			scoreData[v] += local[v];
		});
	}

}


count ApproxBetweenness::numberOfSamples() {
	return r;
}


} /* namespace NetworKit */