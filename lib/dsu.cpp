#include "dsu.h"


TDisjointSetUnion::TDisjointSetUnion(const size_t count)
    : Parents(count)
    , Ranks(count)
{
    for (size_t idx = 0; idx < count; ++idx) {
        Parents[idx] = idx;
    }
}

size_t TDisjointSetUnion::GetRoot(const size_t idx) const {
    size_t parent = idx;
    while (Parents[parent] != parent) {
        parent = Parents[parent];
    }
    return parent;
}

size_t TDisjointSetUnion::GetRoot(const size_t idx) {
    const size_t root = ((const TDisjointSetUnion*) this)->GetRoot(idx);

    size_t traveller = idx;
    while (traveller != root) {
        size_t tmp = Parents[traveller];
        Parents[traveller] = root;
        traveller = tmp;
    }

    return root;
}

void TDisjointSetUnion::Unite(const size_t lhs, const size_t rhs) {
    const size_t leftRoot = GetRoot(lhs);
    const size_t rightRoot = GetRoot(rhs);

    const size_t newRoot = Ranks[leftRoot] < Ranks[rightRoot] ? rightRoot : leftRoot;
    Parents[leftRoot] = newRoot;
    Parents[rightRoot] = newRoot;

    if (Ranks[leftRoot] == Ranks[rightRoot]) {
        ++Ranks[newRoot];
    }
}
