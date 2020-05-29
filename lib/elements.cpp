#include <iostream>

#include "elements.h"

std::vector<TElement>::const_iterator TElements::begin() const {
    return Elements.begin();
}

std::vector<TElement>::const_iterator TElements::end() const {
    return Elements.end();
}

const TElement& TElements::GetElement(const size_t index) const {
    return Elements[index];
}

size_t TElements::Size() const {
    return Elements.size();
}

void TElements::Load(std::istream& in) {
    std::string first, second;
    float similarity;

    size_t loadedPairs = 0;
    while (in >> first >> second >> similarity) {
        similarity = std::min(1.f, std::max(0.f, similarity));

        const size_t firstIndex = GetElementIndex(first);
        const size_t secondIndex = GetElementIndex(second);

        TElement& firstElement = Elements[firstIndex];
        TElement& secondElement = Elements[secondIndex];

        firstElement.SumSimilarities += similarity;
        secondElement.SumSimilarities += similarity;

        firstElement.Similarities[secondIndex] = similarity;
        secondElement.Similarities[firstIndex] = similarity;

        ++loadedPairs;
    }

    std::cerr << "loaded " << loadedPairs << " pairs\n";
}

size_t TElements::GetElementIndex(const std::string& id) {
    size_t& index = IdToIndex[id];
    if (index == 0) {
        index = IdToIndex.size();
        Elements.emplace_back(TElement(id, index - 1));
    }
    return index - 1;
}
