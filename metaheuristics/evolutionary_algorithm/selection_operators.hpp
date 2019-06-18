/**
 * @file selection_operators.hpp
 * @author Pablo
 * @brief Evolutionary Algorithm Selection Operators.
 * @version 0.1
 * @date 13-02-2019
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef SELECTIONOPERATORS_HPP_
#define SELECTIONOPERATORS_HPP_

#include <algorithm>
#include <random>

/**
 * @brief Roulette Wheel selection operator.
 * 
 */
class RouletteWheelSelection
{
  public:
    /**
     * @brief Inserts in a container the specified number of couples using the
     * roulette wheel selection algorithm, also known as fitness proportionate
     * selection.
     * 
     * @tparam InputIt type of the iterator to be used to read the individuals.
     * @tparam OutputIt type of the iterator to be used to insert the couples.
     * @tparam RNG type of the random number generator.
     * @param first iterator pointing to the first individual.
     * @param last iterator pointing to the individual past the last individual.
     * @param dest iterator to be used to insert the couples.
     * @param number number of couples to be generated.
     * @param rng random number generator.
     * @return an iterator pointing to the couple past the last couple inserted.
     */
    template <typename InputIt, typename OutputIt, typename RNG>
    static OutputIt Select(InputIt first, InputIt last, OutputIt dest, std::size_t number, RNG& rng)
    {
        // aggregate the quality of the elements to obtain the probability intervals
        std::vector<std::pair<std::reference_wrapper<const typename std::iterator_traits<InputIt>::value_type>, double>> probability_intervals;
        probability_intervals.push_back(std::make_pair(std::cref(*first), first->GetQuality()));
        ++first;
        while (first != last) {
            probability_intervals.push_back(std::make_pair(std::cref(*first), probability_intervals.back().second + first->GetQuality()));
            ++first;
        }

        // select the elements by generating random numbers in the previously calculated interval (spinning the roulette)
        std::uniform_real_distribution<double> dis(0.0, probability_intervals.back().second);
        for (std::size_t i = 0; i < number; i++) {
            ++dest = std::make_pair(
                std::cref(std::upper_bound(probability_intervals.begin(),
                                           probability_intervals.end(),
                                           dis(rng),
                                           [](const auto& t1, const auto& t2) { return t1 < t2.second; })
                              ->first),
                std::cref(std::upper_bound(probability_intervals.begin(), probability_intervals.end(), dis(rng), [](const auto& t1, const auto& t2) {
                              return t1 < t2.second;
                          })->first));
        }
        return dest;
    }
};

/**
 * @brief Pair selection operator 
 * 
 */
class PairSelection
{
  public:
    /**
     * @brief Inserts in a container the specified number of couples by pairing
     * all the individuals in the population.
     * 
     * @tparam InputIt type of the iterator to be used to read the individuals.
     * @tparam OutputIt type of the iterator to be used to insert the couples.
     * @tparam RNG type of the random number generator.
     * @param first iterator pointing to the first individual.
     * @param last iterator pointing to the individual past the last individual.
     * @param dest iterator to be used to insert the couples.
     * @param number number of couples to be generated.
     * @param rng random number generator.
     * @return an iterator pointing to the couple past the last couple inserted.
     */
    template <typename InputIt, typename OutputIt, typename RNG>
    static OutputIt Select(InputIt first, InputIt last, OutputIt dest, std::size_t number, RNG& rng)
    {
        std::vector<std::reference_wrapper<const typename std::iterator_traits<InputIt>::value_type>> population(first, last);
        std::size_t current_number = 0;
        while (current_number < number) {
            std::shuffle(population.begin(), population.end(), rng);
            for (std::size_t i = 0; i < population.size() - 1 && current_number < number; i += 2, current_number++) {
                *dest++ = std::make_pair(std::cref(population.at(i)), std::cref(population.at(i + 1)));
            }
        }
        return dest;
    }
};

#endif /* SELECTIONOPERATORS_HPP_ */
