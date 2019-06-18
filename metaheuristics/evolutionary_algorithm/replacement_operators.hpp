/**
 * @file replacement_operators.hpp
 * @author Pablo
 * @brief Evolutionary Algorithm Replacement Operators.
 * @version 0.1
 * @date 13-02-2019
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef REPLACEMENTOPERATORS_HPP_
#define REPLACEMENTOPERATORS_HPP_

#include <algorithm>
#include <random>
#include <utility>

/**
 * @brief Generational replacement operator.
 * 
 */
class Generational
{
  public:
    /**
     * @brief Returns a pair with the two offsprings.
     * 
     * @tparam Individual type of the individuals.
     * @tparam RNG type of the random number generator.
     * @param offspring1 first offspring.
     * @param offspring2 second offspring.
     * @return a pair with the two offsprings. 
     */
    template <typename Individual, typename RNG>
    static std::pair<std::reference_wrapper<const Individual>, std::reference_wrapper<const Individual>>
    Choose(const Individual&, const Individual&, const Individual& offspring1, const Individual& offspring2, RNG&)
    {
        return std::make_pair(std::cref(offspring1), std::cref(offspring2));
    }
};

/**
 * @brief Tournament replacement operator.
 * 
 */
class Tournament
{
  public:
    /**
     * @brief Returns a pair with the two best individuals.
     * 
     * @tparam Individual type of the individuals.
     * @tparam RNG type of the random number generator.
     * @param parent1 first parent.
     * @param parent2 second parent.
     * @param offspring1 first offspring.
     * @param offspring2 second offspring.
     * @return a pair with the two best individuals. 
     */
    template <typename Individual, typename RNG>
    static std::pair<std::reference_wrapper<const Individual>, std::reference_wrapper<const Individual>>
    Choose(const Individual& parent1, const Individual& parent2, const Individual& offspring1, const Individual& offspring2, RNG&)
    {
        std::vector<std::reference_wrapper<const Individual>> individuals = {parent1, parent2, offspring1, offspring2};
        std::partial_sort(individuals.begin(), individuals.begin() + 2, individuals.end(), std::greater<Individual>());
        return std::make_pair(std::cref(individuals.at(0)), std::cref(individuals.at(1)));
    }
};

#endif /* REPLACEMENTOPERATORS_HPP_ */
