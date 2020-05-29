#include <iostream>

#include "lib/clustering.h"
#include "lib/elements.h"

#include "args.h"
#include "report.h"
#include "timer.h"

int main(int argc, const char** argv) {
    TClusteringParameters clusteringParameters;

    {
        TArgsParser argsParser;
        argsParser.AddHandler("f", &clusteringParameters.RecallFactor, "recall factor").Optional();
        argsParser.AddHandler("d", &clusteringParameters.RecallDecayFactor, "decay factor").Optional();
        argsParser.AddHandler("t", &clusteringParameters.SimilarityThreshold, "similarity threshold").Optional();
        argsParser.DoParse(argc, argv);
    }

    TTimer timer(std::cerr);

    timer.Reset("loading documents");
    TElements elements;
    std::ios_base::sync_with_stdio(false);
    elements.Load(std::cin);

    timer.ReportAndReset("clustering documents");
    const TClusters clusters = DoCluster(elements, clusteringParameters);

    timer.ReportAndReset("printing clusters");
    PrintClusters(clusters, std::cout);

    timer.Report();

    return 0;
}
