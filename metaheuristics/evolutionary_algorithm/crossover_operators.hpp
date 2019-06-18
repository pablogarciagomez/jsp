/**
 * @file crossover_operators.hpp
 * @author Pablo
 * @brief Evolutionary Algorithm Crossover Operators.
 * @version 0.1
 * @date 13-02-2019
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef CROSSOVEROPERATORS_HPP_
#define CROSSOVEROPERATORS_HPP_

#include <algorithm>
#include <random>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <utils/container_utils.hpp>

/**
 * @brief GOX (Generalized Order Crossover) crossover operator.
 * 
 */
class GOX
{
  private:
    /**
     * @brief Implants some genes of a chromosome into another chromosome to generate a new chromosome
     * that combines both.
     * 
     * @tparam InputIt1 type of the iterator to be used to read the receiver chromosome.
     * @tparam InputIt2 type of the iterator to be used to read the donator chromosome.
     * @tparam OutputIt type of the iterator to be used to insert the genes of the resulting chromosome.
     * @param first1 iterator pointing to the first gene of the receiver chromosome.
     * @param last1 iterator pointing to the gene past the last gene of the receiver chromosome.
     * @param first2 iterator pointing to the first gene of the donator chromosome.
     * @param last2 iterator pointing to the gene past the last gene of the donator chromosome.
     * @param dest iterator to be used to insert the genes of the resulting chromosome.
     * @param implant_position position where the genes to be implanted start.
     * @param implant_length number of genes to be implanted.
     * @return an iterator to the gene past the last gene inserted.   
     */
    template <typename InputIt1, typename InputIt2, typename OutputIt>
    static OutputIt
    Implant(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt dest, std::size_t implant_position, std::size_t implant_length)
    {
        using Gene = typename std::iterator_traits<InputIt1>::value_type::first_type;
        std::vector<std::pair<Gene, int>> receiver(first1, last1);
        std::vector<std::pair<Gene, int>> donator(first2, last2);
        if (implant_position + implant_length > donator.size()) {
            // wrapped around

            // get the genes to be implanted
            std::unordered_set<std::pair<Gene, int>, pair_hash> implant_genes;
            implant_genes.insert(donator.begin() + implant_position, donator.end());
            implant_genes.insert(donator.begin(),
                                 donator.begin() + implant_length - std::distance(donator.begin() + implant_position, donator.end()));

            // mark with -1 the genes that have to be removed
            for (auto it = receiver.begin(); it != receiver.end(); ++it) {
                if (implant_genes.count(*it) != 0) {
                    it->second = -1;
                }
            }

            // implant the genes
            receiver.insert(receiver.end(), donator.begin() + implant_position, donator.end());
            receiver.insert(receiver.begin(),
                            donator.begin(),
                            donator.begin() + implant_length - std::distance(donator.begin() + implant_position, donator.end()));

        } else {
            // inside

            // get the genes to be implanted
            std::unordered_set<std::pair<Gene, int>, pair_hash> implant_genes;
            implant_genes.insert(donator.begin() + implant_position, donator.begin() + implant_position + implant_length);
            auto first_element = donator.at(implant_position); // gene at the implant position
            typename std::vector<std::pair<Gene, int>>::iterator insert_position; // position where the genes will be inserted

            // mark with -1 the genes that have to be removed
            for (auto it = receiver.begin(); it != receiver.end(); ++it) {
                if (implant_genes.count(*it) != 0) {
                    if (*it == first_element) {
                        insert_position = it;
                    }
                    it->second = -1;
                }
            }

            // implant the genes
            receiver.insert(insert_position, donator.begin() + implant_position, donator.begin() + implant_position + implant_length);
        }
        // return only the elements that are not marked with -1
        return filter_transform(
            receiver.begin(), receiver.end(), dest, [&](const auto& e) { return e.second != -1; }, [&](const auto& e) { return e.first; });
    }

  public:
    /**
     * @brief Inserts in a container the genes of the two offsprings resulting of crossing the specified parent chromosomes.
     * 
     * @tparam InputIt1 type of the iterator to be used to read the first parent.
     * @tparam InputIt2 type of the iterator to be used to read the second parent.
     * @tparam OutputIt1 type of the iterator to be used to insert the genes of the first offspring.
     * @tparam OutputIt2 type of the iterator to be used to insert the genes of the second offspring.
     * @tparam RNG type of the random number generator.
     * @param first1 iterator pointing to the first gene of the first parent.
     * @param last1 iterator pointing to the gene past the last gene of the first parent.
     * @param first2 iterator pointing to the first gene of the second parent.
     * @param last2 iterator pointing to the gene past the last gene of the second parent.
     * @param dest1 iterator to be used to insert the genes of the first offspring.
     * @param dest2 iterator to be used to insert the genes of the second offspring.
     * @param rng random number generator.
     * @return a pair with the iterators pointing to the gene past the last gene inserted in each offspring.
     */
    template <typename InputIt1, typename InputIt2, typename OutputIt1, typename OutputIt2, typename RNG>
    static std::pair<OutputIt1, OutputIt2>
    Cross(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt1 dest1, OutputIt2 dest2, RNG rng)
    {
        using Gene = typename std::iterator_traits<InputIt1>::value_type;

        if (std::distance(first1, last1) != std::distance(first2, last2)) {
            throw std::invalid_argument("Chromosomes don't have the same size");
        }

        // calculate the relative positions of the genes in the chromosomes
        std::vector<std::pair<Gene, int>> parent1;
        std::vector<std::pair<Gene, int>> parent2;
        std::unordered_map<Gene, unsigned int> repetitions;
        while (first1 != last1) {
            parent1.push_back(std::make_pair(*first1, repetitions[*first1]++));
            ++first1;
        }
        repetitions.clear();
        while (first2 != last2) {
            parent2.push_back(std::make_pair(*first2, repetitions[*first2]++));
            ++first2;
        }

        // get the implant position
        std::uniform_int_distribution<std::size_t> dis(0, parent1.size() - 1);
        auto implant_position = dis(rng);
        // get the implant length
        dis = std::uniform_int_distribution<std::size_t>(parent1.size() / 3, parent1.size() / 2);
        auto implant_length = dis(rng);

        // generate both offsprings
        auto end1 = Implant(parent1.begin(), parent1.end(), parent2.begin(), parent2.end(), dest1, implant_position, implant_length);
        auto end2 = Implant(parent2.begin(), parent2.end(), parent1.begin(), parent1.end(), dest2, implant_position, implant_length);

        return std::make_pair(end1, end2);
    }
};

#endif /* CROSSOVEROPERATORS_HPP_ */
