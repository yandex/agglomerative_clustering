#pragma once

#include <istream>
#include <string>
#include <vector>
#include <unordered_map>

struct TElement {
    std::unordered_map<size_t, float> Similarities;
    double SumSimilarities = 1.;

    std::string Id;
    size_t Index = 0;

    TElement() {
    }

    TElement(const std::string& id, const size_t index)
        : Id(id)
        , Index(index)
    {
    }
};

class TElements {
private:
    std::vector<TElement> Elements;
    std::unordered_map<std::string, size_t> IdToIndex;
public:
    std::vector<TElement>::const_iterator begin() const;
    std::vector<TElement>::const_iterator end() const;

    const TElement& GetElement(const size_t index) const;

    size_t Size() const;
    void Load(std::istream& in);
private:
    size_t GetElementIndex(const std::string& id);
};
