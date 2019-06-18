/**
 * @file neighborhoods.hpp
 * @author Pablo
 * @brief Functions to manage neighborhoods.
 * @version 0.1
 * @date 09-11-2018
 * 
 * @copyright Copyright (c) 2018
 * 
 */
#ifndef NEIGHBORHOODS_HPP_
#define NEIGHBORHOODS_HPP_

#include <algorithm>

namespace
{
    template <typename Iter, typename Solution, typename Neighborhood>
    Iter FindNeighbors(Iter dest, const Solution& solution, const Neighborhood& neighborhood)
    {
        return neighborhood.GetNeighbors(dest, solution);
    }

    template <typename Iter, typename Solution, typename Neighborhood, typename... Neighborhoods>
    Iter FindNeighbors(Iter dest, const Solution& solution, const Neighborhood& neighborhood, const Neighborhoods&... neighborhoods)
    {
        dest = neighborhood.GetNeighbors(dest, solution);
        return FindNeighbors(dest, solution, neighborhoods...);
    }
}

/**
 * @brief Inserts in a container all the neighbors of a solution.
 * 
 * @tparam Iter type of the iterator to be used to insert the neighbors.
 * @tparam Solution type of the solution whose neighbors will be calculated.
 * @tparam Neighborhood type of the neighborhood to be used to calculate the neighbors.
 * @tparam Neighborhoods type of the additional neighborhoods to be used to calculate the neighbors.
 * @param dest iterator to be used to insert the neighbors.
 * @param solution solution whose neighbors will be calculated.
 * @param neighborhood neighborhood to be used to calculate the neighbors.
 * @param neighborhoods additional neighborhoods to be used to calculate the neighbors.
 * @return an iterator to the neighbor past the last neighbor inserted. 
 */
template <typename Iter, typename Solution, typename Neighborhood, typename... Neighborhoods>
Iter GetNeighbors(Iter dest, const Solution& solution, const Neighborhood& neighborhood, const Neighborhoods&... neighborhoods)
{
    return FindNeighbors(dest, solution, neighborhood, neighborhoods...);
}

#endif /* NEIGHBORHOODS_HPP_ */