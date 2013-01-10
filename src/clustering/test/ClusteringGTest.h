/*
 * ClusteringTest.h
 *
 *  Created on: 12.12.2012
 *      Author: cls
 */

#ifndef CLUSTERINGGTEST_H_
#define CLUSTERINGGTEST_H_

#include <gtest/gtest.h>


#include "../../aux/log.h"
#include "../Clustering.h"
#include "../Modularity.h"
#include "../ClusteringGenerator.h"
#include "../../graph/GraphGenerator.h"
#include "../LabelPropagation.h"

namespace EnsembleClustering {

class ClusteringGTest: public testing::Test {


};



} /* namespace EnsembleClustering */
#endif /* CLUSTERINGTEST_H_ */
