/**
 * @file mutation_operators.hpp
 * @author Pablo
 * @brief Evolutionary Algorithm Mutation Operators.
 * @version 0.1
 * @date 13-02-2019
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef MUTATIONOPERATORS_HPP_
#define MUTATIONOPERATORS_HPP_

#include <algorithm>
#include <random>

/**
 * @brief Swap mutation operator.
 * 
 */
class Swap
{
  public:
    /**
   * @brief Mutates the specified chromosome by exchanging the positions of two genes.
   * 
   * @tparam Iter type of the iterator to be used to read the chromosome.
   * @tparam RNG type of the random number generator.
   * @param first iterator pointing to the first gene of the chromosome.
   * @param last iterator pointing to the gene past the last gene of the chromosome.
   * @param rng random number generator.
   */
    template <typename Iter, typename RNG> static void Mutate(Iter first, Iter last, RNG& rng)
    {
        std::uniform_int_distribution<unsigned int> dis(0, std::distance(first, last) - 1);
        std::iter_swap(first + dis(rng), first + dis(rng));
    }
};

#endif /* MUTATIONOPERATORS_HPP_ */
