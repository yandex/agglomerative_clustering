#pragma once

#include <string>

#include "clustering_parameters.h"
#include "elements.h"

using TCluster = std::vector<std::string>;
using TClusters = std::vector<TCluster>;

const TClusters DoCluster(const TElements& elements, const TClusteringParameters& clusteringParameters);
