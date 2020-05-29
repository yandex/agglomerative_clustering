#pragma once

#include <vector>

class TDisjointSetUnion {
private:
    std::vector<size_t> Parents;
    std::vector<size_t> Ranks;
public:
    TDisjointSetUnion(const size_t count);
    size_t GetRoot(const size_t idx) const;
    size_t GetRoot(const size_t idx);
    void Unite(const size_t lhs, const size_t rhs);
};
