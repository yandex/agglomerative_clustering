#include "dsu.h"
#include "set_stats.h"

#include "clustering.h"

#include <algorithm>
#include <vector>
#include <set>
#include <unordered_map>

namespace NAgglomerativePrivate {
    struct TUnityReward {
        float Reward = 0.;
        std::pair<size_t, size_t> ClusterIds;

        TUnityReward() {
        }

        TUnityReward(const float reward, const size_t firstClusterIdx, const size_t secondClusterIdx)
            : Reward(reward)
        {
            SetupClusterIds(firstClusterIdx, secondClusterIdx);
        }

        void SetupClusterIds(const size_t firstClusterIdx, const size_t secondClusterIdx) {
            ClusterIds.first = std::min(firstClusterIdx, secondClusterIdx);
            ClusterIds.second = std::max(firstClusterIdx, secondClusterIdx);
        }

        bool operator> (const TUnityReward& rhs) const {
            if (Reward == rhs.Reward) {
                return ClusterIds > rhs.ClusterIds;
            }
            return Reward > rhs.Reward;
        }

        bool operator== (const TUnityReward& rhs) const {
            return Reward == rhs.Reward && ClusterIds == rhs.ClusterIds;
        }
    };

    struct TUnity {
        size_t UnityClusterId;
        size_t EliminatedClusterId;
    };

    class TClusteringPrivate {
    private:
        TClusteringParameters ClusteringParameters;

        std::set<TUnityReward, std::greater<>> UnityRewards;
        std::vector<std::unordered_map<size_t, TSubsetStats>> RelationStats;
        std::vector<TSetStats> ClusterStats;

        TDisjointSetUnion DSU;

        TClusters Clusters;
    public:
        TClusteringPrivate(const TElements& elements, const TClusteringParameters& clusteringParameters)
            : ClusteringParameters(clusteringParameters)
            , RelationStats(elements.Size())
            , ClusterStats(elements.Size())
            , DSU(elements.Size())
        {
            InitRelations(elements);

            while (!UnityRewards.empty()) {
                const TUnity unity = NextUnity();
                ProcessUnityRelations(unity);
                ProcessEliminatedRelations(unity);
            }

            FillClusters(elements);
        }

        const TClusters& GetClusters() const {
            return Clusters;
        }

        size_t GetClusterId(const size_t elementIdx) const {
            return DSU.GetRoot(elementIdx);
        }
    private:
        void FillClusters(const TElements& elements);
        void InitRelations(const TElements& elements);

        TUnity NextUnity();
        void ProcessUnityRelations(const TUnity& unity);
        void ProcessEliminatedRelations(const TUnity& unity);
    };

    void TClusteringPrivate::FillClusters(const TElements& elements) {
        std::unordered_map<size_t, size_t> clusterIdToNumber;
        for (const TElement& element : elements) {
            const size_t clusterId = GetClusterId(element.Index);
            size_t& remappedClusterId = clusterIdToNumber[clusterId];
            if (!remappedClusterId) {
                remappedClusterId = clusterIdToNumber.size();
                Clusters.push_back(TCluster());
            }
            Clusters[remappedClusterId - 1].push_back(element.Id);
        }
    }

    void TClusteringPrivate::InitRelations(const TElements& elements) {
        for (const TElement& element : elements) {
            ClusterStats[element.Index] = TSetStats::InitTrivial(element.SumSimilarities);
        }

        for (const TElement& element : elements) {
            const size_t elementIdx = element.Index;
            for (auto&& neighbourWithSimilarity : element.Similarities) {
                const size_t neighbourIdx = neighbourWithSimilarity.first;
                if (neighbourIdx > elementIdx) {
                    continue;
                }

                const float similarity = neighbourWithSimilarity.second;
                if (similarity < ClusteringParameters.SimilarityThreshold) {
                    continue;
                }

                const TElement& neighbour = elements.GetElement(neighbourIdx);

                TSubsetStats relationStats;
                relationStats.PerMatrixElementSum = 2.f * similarity;
                relationStats.PerRowSum = similarity * (1.f / element.SumSimilarities + 1.f / neighbour.SumSimilarities);
                relationStats.Reward = TSetStats::UnityReward(ClusterStats[elementIdx], ClusterStats[neighbourIdx], relationStats, ClusteringParameters);

                if (relationStats.Reward < 0) {
                    continue;
                }

                RelationStats[elementIdx][neighbourIdx] = relationStats;
                RelationStats[neighbourIdx][elementIdx] = relationStats;

                TUnityReward reward;
                reward.Reward = relationStats.Reward;
                reward.SetupClusterIds(elementIdx, neighbourIdx);
                UnityRewards.insert(reward);
            }
        }
    }

    TUnity TClusteringPrivate::NextUnity() {
        const TUnityReward bestRelation = *UnityRewards.begin();
        UnityRewards.erase(UnityRewards.begin());

        const size_t leftClusterIdx = bestRelation.ClusterIds.first;
        const size_t rightClusterIdx = bestRelation.ClusterIds.second;

        DSU.Unite(leftClusterIdx, rightClusterIdx);

        TUnity unity;
        unity.UnityClusterId = RelationStats[leftClusterIdx].size() > RelationStats[rightClusterIdx].size() ? leftClusterIdx : rightClusterIdx;
        unity.EliminatedClusterId = unity.UnityClusterId == leftClusterIdx ? rightClusterIdx : leftClusterIdx;

        ClusterStats[unity.UnityClusterId] = TSetStats::UnityStats(
            ClusterStats[leftClusterIdx],
            ClusterStats[rightClusterIdx],
            RelationStats[leftClusterIdx][rightClusterIdx]);
        ClusterStats[unity.EliminatedClusterId] = {};

        RelationStats[rightClusterIdx].erase(leftClusterIdx);
        RelationStats[leftClusterIdx].erase(rightClusterIdx);

        return unity;
    }

    void TClusteringPrivate::ProcessUnityRelations(const TUnity& unity) {
        std::unordered_map<size_t, TSubsetStats>& unityRelationStats = RelationStats[unity.UnityClusterId];
        std::unordered_map<size_t, TSubsetStats>& eliminatedRelationStats = RelationStats[unity.EliminatedClusterId];

        for (std::unordered_map<size_t, TSubsetStats>::iterator it = unityRelationStats.begin(); it != unityRelationStats.end(); ) {
            const size_t neighbour = it->first;
            TSubsetStats& relationStats = it->second;

            TUnityReward unityReward(relationStats.Reward, unity.UnityClusterId, neighbour);
            UnityRewards.erase(unityReward);

            std::unordered_map<size_t, TSubsetStats>::const_iterator eliminatedRelationIt = eliminatedRelationStats.find(neighbour);
            if (eliminatedRelationIt != eliminatedRelationStats.end()) {
                relationStats += eliminatedRelationIt->second;
            }
            unityReward.Reward = TSetStats::UnityReward(ClusterStats[unity.UnityClusterId], ClusterStats[neighbour], relationStats, ClusteringParameters);

            if (unityReward.Reward < 0.) {
                unityRelationStats.erase(it++);
                RelationStats[neighbour].erase(unity.UnityClusterId);
            } else {
                UnityRewards.insert(unityReward);

                relationStats.Reward = unityReward.Reward;

                RelationStats[neighbour][unity.UnityClusterId] = relationStats;

                ++it;
            }
        }
    }

    void TClusteringPrivate::ProcessEliminatedRelations(const TUnity& unity) {
        std::unordered_map<size_t, TSubsetStats>& unityRelationStats = RelationStats[unity.UnityClusterId];
        std::unordered_map<size_t, TSubsetStats>& eliminatedRelationStats = RelationStats[unity.EliminatedClusterId];

        for (auto&& eliminatedNeighbourWithStats : eliminatedRelationStats) {
            const size_t neighbour = eliminatedNeighbourWithStats.first;
            TSubsetStats relationStats = eliminatedNeighbourWithStats.second;

            TUnityReward unityReward(relationStats.Reward, unity.EliminatedClusterId, neighbour);
            UnityRewards.erase(unityReward);

            RelationStats[neighbour].erase(unity.EliminatedClusterId);

            std::unordered_map<size_t, TSubsetStats>::const_iterator unityRelationStatsIt = unityRelationStats.find(neighbour);
            if (unityRelationStatsIt != unityRelationStats.end()) {
                continue;
            }

            unityReward.Reward = TSetStats::UnityReward(ClusterStats[unity.UnityClusterId], ClusterStats[neighbour], relationStats, ClusteringParameters);
            if (unityReward.Reward < 0.) {
                continue;
            }

            unityReward.SetupClusterIds(unity.UnityClusterId, neighbour);
            UnityRewards.insert(unityReward);

            relationStats.Reward = unityReward.Reward;

            RelationStats[unity.UnityClusterId][neighbour] = relationStats;
            RelationStats[neighbour][unity.UnityClusterId] = relationStats;
        }

        eliminatedRelationStats = {};
    }
}

const TClusters DoCluster(const TElements& elements, const TClusteringParameters& clusteringParameters) {
    NAgglomerativePrivate::TClusteringPrivate clustering(elements, clusteringParameters);
    return clustering.GetClusters();
}
