#pragma once

#include <cmath>

struct TClusteringParameters {
    float SimilarityThreshold = 0.2f;

    float RecallFactor = 1.f;
    float RecallDecayFactor = 0.;

    double CombineMetrics(const double precision, const double recall, const size_t uniteSize) const {
        float recallFactor = RecallFactor;
        if (RecallDecayFactor) {
            recallFactor *= RecallDecayFactor / (RecallDecayFactor + uniteSize);
        }
        return std::pow(recall, recallFactor) * precision;
    }
};
