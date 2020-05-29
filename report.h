#pragma once

#include "lib/clustering.h"

#include <ostream>

static inline void PrintClusters(const TClusters& clusters, std::ostream& out) {
    for (size_t clusterIdx = 0; clusterIdx < clusters.size(); ++clusterIdx) {
        if (clusters[clusterIdx].size() == 1) {
            continue;
        }

        for (const std::string& id : clusters[clusterIdx]) {
            out << clusterIdx << "\t" << id << "\n";
        }
    }
}
