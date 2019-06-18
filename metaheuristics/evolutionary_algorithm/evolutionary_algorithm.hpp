/**
 * @file evolutionary_algorithm.hpp
 * @author Pablo
 * @brief Evolutionary Algorithm.
 * @version 0.1
 * @date 13-02-2019
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef EVOLUTIONARYALGORITHM_HPP_
#define EVOLUTIONARYALGORITHM_HPP_

#include <algorithm>
#include <numeric>
#include <random>
#include <unordered_set>

#include <metaheuristics/utils/evolutionary_algorithm_logger.hpp>
#include <utils/container_utils.hpp>

/**
 * @brief Provides static functions to run a evolutionary algorithm.
 * 
 */
class EvolutionaryAlgorithm
{
  private:
    /**
     * @brief Representation of an individual in a evolutionary algorithm.
     * 
     */
    struct Individual
    {
        using Chromosome = std::vector<unsigned int>;

        Chromosome chromosome; // the chromosome of the individual
        double quality; // the quality (fitness) of the individual

        /**
         * @brief Returns the quality (fitness) of the individual.
         * 
         * @return the quality (fitness) of the individual. 
         */
        double GetQuality() const
        {
            return quality;
        }

        bool operator==(const Individual& other) const
        {
            return chromosome == other.chromosome;
        }

        bool operator!=(const Individual& other) const
        {
            return chromosome != other.chromosome;
        }

        bool operator<(const Individual& other) const
        {
            return quality < other.quality;
        };

        bool operator>(const Individual& other) const
        {
            return quality > other.quality;
        };

        bool operator<=(const Individual& other) const
        {
            return quality <= other.quality;
        };

        bool operator>=(const Individual& other) const
        {
            return quality >= other.quality;
        };
    };

    struct IndividualHash
    {
        inline std::size_t operator()(const Individual& k) const
        {
            return vector_hash{}(k.chromosome);
        }
    };

  public:
    /**
     * @brief Finds a solution to a problem using a evolutionary algorithm metaheuristic.
     * 
     * @tparam Solution type of the solution to be evaluated.
     * @tparam Problem type of the problem to be evaluated.
     * @tparam EncoderDecoder type of the encoder/decoder to be used to evaluate the chromosomes.
     * @tparam GenerationOp type of the generation operator to be used to generate the initial population.
     * @tparam SelectionOp type of the selection operator to be used to choose the couples of individuals that will reproduce.
     * @tparam CrossoverOp type of the crossover operator to be used to cross the selected couples.
     * @tparam MutationOp type of the mutation operator to be used to mutate the chromosomes of the offsprings.
     * @tparam StoppingCriterion type of the stopping criterion to be used to terminate the algorithm.
     * @tparam ReplacementOp type of the replacement operator to be used to select the new generation.
     * @tparam RNG type of the random number generator.
     * @param logger logger where a trace of the execution will be stored.
     * @param problem problem to be solved.
     * @param encoder_decoder encoder/decoder to be used to evaluate the chromosomes.
     * @param generation_op generation operator to be used to generate the initial population.
     * @param population_size population size (number of individuals in the population).
     * @param selection_op selection operator to be used to choose the couples of individuals that will reproduce.
     * @param crossover_op crossover operator to be used to cross the selected couples.
     * @param cross_prob cross probability.
     * @param mutation_op mutation operator to be used to mutate the chromosomes of the offsprings.
     * @param mutation_prob mutation probability.
     * @param replacement_op replacement operator to be used to select the new generation.
     * @param elitism if true the best individual of each generation will pass untouched to the next generation.
     * @param stopping_criterion stopping criterion to be used to terminate the algorithm.
     * @param rng random number generator to be used.
     * @return the best solution found.
     */
    template <typename Solution,
              typename Problem,
              typename EncoderDecoder,
              typename GenerationOp,
              typename SelectionOp,
              typename CrossoverOp,
              typename MutationOp,
              typename ReplacementOp,
              typename StoppingCriterion,
              typename RNG>
    static Solution FindSolution(EvolutionaryAlgorithmLogger<Solution>& logger,
                                 const Problem& problem,
                                 const EncoderDecoder& encoder_decoder,
                                 const GenerationOp& generation_op,
                                 unsigned int population_size,
                                 const SelectionOp& selection_op,
                                 const CrossoverOp& crossover_op,
                                 double cross_prob,
                                 const MutationOp& mutation_op,
                                 double mutation_prob,
                                 const ReplacementOp& replacement_op,
                                 bool elitism,
                                 const StoppingCriterion& stopping_criterion,
                                 RNG& rng)
    {
        using SolutionType = Solution;
        using Population = std::unordered_multiset<Individual, IndividualHash>;
        using Couple = std::pair<std::reference_wrapper<const Individual>, std::reference_wrapper<const Individual>>;

        // create the initial population
        Population population;
        {
            std::vector<SolutionType> raw_population;
            generation_op.template GetIndividuals<Solution>(std::back_inserter(raw_population), problem, population_size, rng);
            std::transform(std::make_move_iterator(raw_population.begin()),
                           std::make_move_iterator(raw_population.end()),
                           std::inserter(population, population.begin()),
                           [&encoder_decoder](const auto& i) {
                               Individual individual;
                               encoder_decoder.EncodeSolution(std::back_inserter(individual.chromosome), i);
                               individual.quality = i.GetQuality();
                               return individual;
                           });
        }

        // random number generator
        std::uniform_real_distribution<double> dis(0.0, 1.0);

        // keep the best solution
        Individual best_solution =
            *std::max_element(population.begin(), population.end(), [](const auto& t1, const auto& t2) { return t1.quality < t2.quality; });
        // number of generations
        unsigned int generations = 0;
        // number of generations without improving
        unsigned int no_improving_generations = 0;
        // average quality of the solutions
        double average_quality =
            std::accumulate(population.begin(), population.end(), 0.0, [](const auto& t1, const auto& t2) { return t1 + t2.quality; }) /
            population.size();
        if (logger) {
            logger.AddLog(average_quality, best_solution.quality);
        }

        while (!stopping_criterion(generations++, no_improving_generations++, average_quality, best_solution.quality)) // termination criterion
        {
            Population new_generation;
            // elitism, the global best always pass to the next generation
            if (elitism) {
                new_generation.insert(best_solution);
            }
            // select the couples that will reproduce
            std::vector<Couple> couples;
            selection_op.Select(population.begin(), population.end(), std::back_insert_iterator(couples), population_size / 2, rng);
            for (const auto& [parent1, parent2]: couples) {
                // cross the individuals
                Individual offspring1;
                Individual offspring2;
                if (dis(rng) < cross_prob) {
                    crossover_op.Cross(parent1.get().chromosome.begin(),
                                       parent1.get().chromosome.end(),
                                       parent2.get().chromosome.begin(),
                                       parent2.get().chromosome.end(),
                                       std::back_inserter(offspring1.chromosome),
                                       std::back_inserter(offspring2.chromosome),
                                       rng);
                } else {
                    std::copy(parent1.get().chromosome.begin(), parent1.get().chromosome.end(), std::back_inserter(offspring1.chromosome));
                    std::copy(parent2.get().chromosome.begin(), parent2.get().chromosome.end(), std::back_inserter(offspring2.chromosome));
                }

                // mutate the offsprings
                if (dis(rng) < mutation_prob) {
                    mutation_op.Mutate(offspring1.chromosome.begin(), offspring1.chromosome.end(), rng);
                }
                if (dis(rng) < mutation_prob) {
                    mutation_op.Mutate(offspring2.chromosome.begin(), offspring2.chromosome.end(), rng);
                }

                // evaluate the fitness
                if (population.count(offspring1) != 0) {
                    offspring1.quality = population.find(offspring1)->quality;
                } else {
                    offspring1.quality = encoder_decoder.template EvaluateSolutionQuality<Solution>(
                        offspring1.chromosome.begin(), offspring1.chromosome.end(), problem);
                }
                if (population.count(offspring2) != 0) {
                    offspring2.quality = population.find(offspring2)->quality;
                } else {
                    offspring2.quality = encoder_decoder.template EvaluateSolutionQuality<Solution>(
                        offspring2.chromosome.begin(), offspring2.chromosome.end(), problem);
                }

                // select the individuals that will pass to the next generation
                const auto& [descendant1, descendant2] = replacement_op.Choose(parent1.get(), parent2.get(), offspring1, offspring2, rng);

                // insert the descendants in the new generation
                new_generation.insert(descendant1);
                new_generation.insert(descendant2);

                // check if any of the offsprings is the global best
                if (descendant1.get().quality > best_solution.quality) {
                    best_solution = descendant1;
                    no_improving_generations = 0;
                }
                if (descendant2.get().quality > best_solution.quality) {
                    best_solution = descendant2;
                    no_improving_generations = 0;
                }
            }
            // set the new generation as the current generation
            population = std::move(new_generation);
            // update average quality
            average_quality =
                std::accumulate(population.begin(), population.end(), 0.0, [](const auto& t1, const auto& t2) { return t1 + t2.quality; }) /
                population.size();

            if (logger) {
                logger.AddLog(average_quality, best_solution.quality);
            }
        }

        // return the best solution found
        auto best = encoder_decoder.template DecodeSolution<Solution>(best_solution.chromosome.begin(), best_solution.chromosome.end(), problem);
        if (logger) {
            logger.SetBestSolution(best);
        }
        return best;
    }
};

#endif /* EVOLUTIONARYALGORITHM_HPP_ */
