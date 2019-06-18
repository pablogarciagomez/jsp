/**
 * @file jsp_genetic_encoders.hpp
 * @author Pablo
 * @brief JSP Genetic Encoders.
 * @version 0.1
 * @date 13-02-2019
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef JSPGENETICENCODERS_HPP_
#define JSPGENETICENCODERS_HPP_

#include <iterator>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include <problems/jsp/jsp_makespan_minimization_solution.hpp>
#include <problems/jsp/jsp_schedule_generation_schemes.hpp>
#include <problems/jsp/jsp_total_weighted_tardiness_minimization_solution.hpp>
#include <utils/template_utils.hpp>

/**
 * @brief Permutation with repetition encoder for JSP.
 * 
 * @tparam Decoder type of the decoder to be used.
 */
template <typename Decoder> class PermutationWithRepetition
{
  private:
    /**
     * @brief Inserts in a container the tasks and its priorities according to the genes in
     * the encoded solution.
     * 
     * @tparam InputIt type of the iterator to be used to read the encoded solution.
     * @tparam OutputIt type of the iterator to be used to insert the tasks with its priorities.
     * @tparam Problem type of the problem.
     * @param first iterator pointing to the first gene of the encoded solution.
     * @param last iterator pointing to the gene past the last gene of the encoded solution.
     * @param dest iterator to be used to insert the tasks with its priorities.
     * @param problem problem to evaluate.
     * @return an iterator to the pair past the last pair inserted.   
     */
    template <typename InputIt, typename OutputIt, typename Problem>
    static OutputIt CalculatePriorities(InputIt first, InputIt last, OutputIt dest, const Problem& problem)
    {
        std::unordered_map<unsigned int, std::size_t> job_position;
        std::size_t current_priority = 0;
        while (first != last) {
            *dest++ = std::make_pair(std::cref(problem.GetTask(*first, job_position[*first]++)), current_priority++);
            ++first;
        }
        return dest;
    }

  public:
    /**
     * @brief Inserts in a container the genes of the encoded solution.
     * 
     * @tparam Iter type of the iterator to be used to insert the genes of the encoded solution.
     * @tparam Solution type of the solution to be encoded.
     * @param dest iterator to be used to insert the genes of the encoded solution.
     * @param solution solution to be encoded.
     * @return an iterator to the gene past the last gene inserted.    
     */
    template <typename Iter, typename Solution> static Iter EncodeSolution(Iter dest, const Solution& solution)
    {
        using TaskType = typename Solution::TaskType;
        // get the tasks in topological order
        std::vector<std::reference_wrapper<const TaskType>> tasks;
        solution.GetTasksTopologicalOrder(std::back_inserter(tasks));
        // substitute each task with the identifier of its job
        return std::transform(tasks.begin(), tasks.end(), dest, [](const TaskType& task) { return task.GetJob().GetJobID(); });
    }

    /**
     * @brief Decodes an encoded solution and updates the chromosome accordingly.
     * 
     * @tparam Solution type of the solution to be decoded.
     * @tparam Iter type of the iterator to be used to read the genes of the encoded solution.
     * @tparam Problem type of the problem to be decoded.
     * @param first iterator pointing to the first gene of the encoded solution.
     * @param last iterator pointing to the gene past the last gene of the encoded solution.
     * @param problem problem to which the solution to be decoded belongs.
     * @return the decoded solution.
     */
    template <typename Solution, typename Iter, typename Problem> static Solution DecodeSolution(Iter first, Iter last, const Problem& problem)
    {
        using TaskType = typename Problem::TaskType;
        using TimeType = typename Problem::TimeType;
        // calculate the priorities of the tasks
        std::unordered_map<std::reference_wrapper<const TaskType>, std::size_t, std::hash<TaskType>, std::equal_to<TaskType>> priorities;
        CalculatePriorities(first, last, std::inserter(priorities, priorities.begin()), problem);
        // calculate the earliest starting time of the tasks
        std::unordered_map<std::reference_wrapper<const TaskType>, TimeType, std::hash<TaskType>, std::equal_to<TaskType>> est;
        Decoder::EvaluateSolution(
            std::make_move_iterator(priorities.begin()), std::make_move_iterator(priorities.end()), std::inserter(est, est.begin()), problem);
        // update encoding with the new values
        std::transform(est.begin(), est.end(), first, [](const auto& t) { return t.first.get().GetJob().GetJobID(); });
        // build the graph
        return BuildSolution<Solution>(std::make_move_iterator(est.begin()), std::make_move_iterator(est.end()), problem);
    }

    /**
     * @brief Returns the makespan of an encoded solution and updates the chromosome accordingly.
     * 
     * @tparam Solution type of the solution to be evaluated.
     * @tparam Iter type of the iterator to be used to read the genes of the encoded solution. 
     * @tparam Problem type of the problem to be evaluated.
     * @param first iterator pointing to the first gene of the encoded solution.
     * @param last iterator pointing to the gene past the last gene of the encoded solution.
     * @param problem problem to which the solution to be decoded belongs.
     * @return the makespan of the solution.
     */
    template <typename Solution, typename Iter, typename Problem>
    static typename Problem::TimeType EvaluateSolutionMakespan(Iter first, Iter last, const Problem& problem)
    {
        using TaskType = typename Problem::TaskType;
        using TimeType = typename Problem::TimeType;
        // calculate the priorities of the tasks
        std::unordered_map<std::reference_wrapper<const TaskType>, std::size_t, std::hash<TaskType>, std::equal_to<TaskType>> priorities;
        CalculatePriorities(first, last, std::inserter(priorities, priorities.begin()), problem);
        // calculate the earliest starting time of the tasks
        std::vector<std::pair<std::reference_wrapper<const TaskType>, TimeType>> ordered_est;
        Decoder::EvaluateSolution(std::make_move_iterator(priorities.begin()),
                                  std::make_move_iterator(priorities.end()),
                                  std::inserter(ordered_est, ordered_est.begin()),
                                  problem);
        // update encoding with the new values
        std::transform(ordered_est.begin(), ordered_est.end(), first, [](const auto& t) { return t.first.get().GetJob().GetJobID(); });
        // store the earliest starting times in a map
        std::unordered_map<std::reference_wrapper<const TaskType>, TimeType, std::hash<TaskType>, std::equal_to<TaskType>> est(ordered_est.begin(),
                                                                                                                               ordered_est.end());
        // get the final tasks
        std::vector<std::reference_wrapper<const TaskType>> final_tasks;
        problem.GetFinalTasks(std::back_inserter(final_tasks));
        // calculate the makespan
        TimeType makespan{};
        for (const TaskType& task: final_tasks) {
            makespan = std::max(makespan, task.GetDuration() + est.at(task));
        }
        return makespan;
    }

    /**
     * @brief Returns the total weighted tardiness of an encoded solution and updates the chromosome accordingly.
     * 
     * @tparam Solution type of the solution to be evaluated.
     * @tparam Iter type of the iterator to be used to read the genes of the encoded solution. 
     * @tparam Problem type of the problem to be evaluated.
     * @param first iterator pointing to the first gene of the encoded solution.
     * @param last iterator pointing to the gene past the last gene of the encoded solution.
     * @param problem problem to which the solution to be decoded belongs.
     * @return the total weighted tardiness of the solution.
     */
    template <typename Solution, typename Iter, typename Problem>
    static typename Problem::TimeType EvaluateSolutionTotalWeightedTardiness(Iter first, Iter last, const Problem& problem)
    {
        using TaskType = typename Problem::TaskType;
        using TimeType = typename Problem::TimeType;
        // calculate the priorities of the tasks
        std::unordered_map<std::reference_wrapper<const TaskType>, std::size_t, std::hash<TaskType>, std::equal_to<TaskType>> priorities;
        CalculatePriorities(first, last, std::inserter(priorities, priorities.begin()), problem);
        // calculate the earliest starting time of the tasks
        std::vector<std::pair<std::reference_wrapper<const TaskType>, TimeType>> ordered_est;
        Decoder::EvaluateSolution(std::make_move_iterator(priorities.begin()),
                                  std::make_move_iterator(priorities.end()),
                                  std::inserter(ordered_est, ordered_est.begin()),
                                  problem);
        // update encoding with the new values
        std::transform(ordered_est.begin(), ordered_est.end(), first, [](const auto& t) { return t.first.get().GetJob().GetJobID(); });
        // store the earliest starting times in a map
        std::unordered_map<std::reference_wrapper<const TaskType>, TimeType, std::hash<TaskType>, std::equal_to<TaskType>> est(ordered_est.begin(),
                                                                                                                               ordered_est.end());
        // get the final tasks
        std::vector<std::reference_wrapper<const TaskType>> final_tasks;
        problem.GetFinalTasks(std::back_inserter(final_tasks));
        // calculate the total weighted tardiness
        TimeType twt{};
        for (const TaskType& task: final_tasks) {
            auto job = task.GetJob();
            auto tardiness = est.at(task) + task.GetDuration() - job.GetDueDate();
            twt += std::max(TimeType{}, tardiness) * job.GetWeight();
        }
        return twt;
    }

    /**
     * @brief Returns the quality of an encoded solution.
     * 
     * @tparam Solution type of the solution to be evaluated.
     * @tparam Iter type of the iterator to be used to read the genes of the encoded solution. 
     * @tparam Problem type of the problem to be evaluated.
     * @param first iterator pointing to the first gene of the encoded solution.
     * @param last iterator pointing to the gene past the last gene of the encoded solution.
     * @param problem problem to which the solution to be decoded belongs.
     * @return the quality of the solution.
     */
    template <typename Solution, typename Iter, typename Problem> static double EvaluateSolutionQuality(Iter first, Iter last, const Problem& problem)
    {
        // return the quality of the solution depending on the solution type
        if constexpr (is_specialization<Solution, JSPMakespanMinimizationSolution>::value) {
            return 1.0 / EvaluateSolutionMakespan<Solution>(first, last, problem);
        } else if constexpr (is_specialization<Solution, JSPTotalWeightedTardinessMinimizationSolution>::value) {
            return 1.0 / EvaluateSolutionTotalWeightedTardiness<Solution>(first, last, problem);
        }
    }
};

#endif /* JSPGENETICENCODERS_HPP_ */