/**
 * @file jsp_generation_operators.hpp
 * @author Pablo
 * @brief JSP Generation Operators.
 * @version 0.1
 * @date 13-02-2019
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef JSPGENERATIONOPERATORS_HPP_
#define JSPGENERATIONOPERATORS_HPP_

#include <random>

#include <problems/jsp/jsp_genetic_encoders.hpp>
#include <problems/jsp/jsp_schedule_generation_schemes.hpp>

/**
 * @brief Random population generator for JSP.
 * 
 */
class JSPRandomPopulationGenerator
{
  public:
    /**
     * @brief Inserts in a container a population of random individuals.
     * 
     * @tparam Solution type of the solutions.
     * @tparam Iter type of the iterator to be used to insert the individuals.
     * @tparam Problem type of the problem.
     * @tparam RNG random number generator.
     * @param dest iterator to be used to insert the individuals.
     * @param problem base problem.
     * @param population_size size of the population (number of individuals).
     * @param rng random number generator.
     * @return an iterator to the individual past the last individual inserted.   
     */
    template <typename Solution, typename Iter, typename Problem, typename RNG>
    static Iter GetIndividuals(Iter dest, const Problem& problem, unsigned int population_size, RNG& rng)
    {
        using JobType = typename Problem::JobType;
        std::vector<unsigned int> master_pattern;
        std::vector<std::reference_wrapper<const JobType>> jobs;
        problem.GetJobs(std::back_inserter(jobs));
        auto inserter = std::back_inserter(master_pattern);
        for (const JobType& job: jobs) {
            std::fill_n(inserter, problem.GetNumberOfTasksInJob(job.GetJobID()), job.GetJobID());
        }
        for (unsigned int i = 0; i < population_size; i++) {
            std::shuffle(master_pattern.begin(), master_pattern.end(), rng);
            ++dest = std::move(PermutationWithRepetition<GT>::DecodeSolution<Solution>(master_pattern.begin(), master_pattern.end(), problem));
        }
        return dest;
    }
};

#endif /* JSPGENERATIONOPERATORS_HPP_ */
