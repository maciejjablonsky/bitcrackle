#pragma once

#include <list>
#include <ranges>
#include <vector>

namespace bit::peep
{
class peeper
{
    std::list<std::vector<double>> data_;

  public:
    template <typename Projection = std::identity>
    void plot(const std::ranges::range auto& range, Projection projection = {})
    {
    }

    void show();
};
} // namespace bit::peep
