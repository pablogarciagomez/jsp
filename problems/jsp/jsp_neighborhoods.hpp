/**
 * @file jsp_neighborhoods.hpp
 * @author Pablo
 * @brief JSP Neighborhoods.
 * @version 0.1
 * @date 09-11-2018
 * 
 * @copyright Copyright (c) 2018
 * 
 */
#ifndef JSPNEIGHBORHOODS_HPP_
#define JSPNEIGHBORHOODS_HPP_

#include <algorithm>
#include <type_traits>
#include <utility>
#include <vector>

#include <problems/jsp/jsp_makespan_minimization_solution.hpp>
#include <problems/jsp/jsp_move.hpp>
#include <problems/jsp/jsp_restriction_list.hpp>
#include <problems/jsp/jsp_total_weighted_tardiness_minimization_solution.hpp>
#include <utils/template_utils.hpp>

/**
 * @brief Estimates the makespan that results of changing the order
 * of a group of tasks in the same machine.
 * 
 * @tparam Iter type of the iterator to be used to read the group of tasks.
 * @tparam Solution type of the solution.
 * @param first iterator pointing to the first task in the new order.
 * @param last iterator pointing to the task past the last task in the new order.
 * @param solution solution for which the estimate will be calculated.
 * @param before task that is scheduled in the same machine before the first task in the group.
 * @param after task that is scheduled in the same machine after the last task in the group.
 * @return estimate of the makespan for the new order of the tasks. 
 */
template <typename Iter, typename Solution>
static typename Solution::TimeType EstimateMakespan(Iter first,
                                                    Iter last,
                                                    const Solution& solution,
                                                    const std::optional<std::reference_wrapper<const typename Solution::TaskType>>& before,
                                                    const std::optional<std::reference_wrapper<const typename Solution::TaskType>>& after)
{
    using TaskType = typename Solution::TaskType;
    using TimeType = typename Solution::TimeType;
    //estimate heads
    std::unordered_map<std::reference_wrapper<const TaskType>, TimeType, std::hash<TaskType>, std::equal_to<TaskType>> heads;
    std::optional<std::reference_wrapper<const TaskType>> job_predecessor = solution.GetPrevPrecedenceConstrainedTask(*first);
    heads[*first] = std::max(job_predecessor.has_value() ? solution.GetHead(*job_predecessor) + job_predecessor->get().GetDuration() : TimeType{},
                             before.has_value() ? solution.GetHead(*before) + before->get().GetDuration() : TimeType{});
    for (auto it = std::next(first); it != last; ++it) {
        job_predecessor = solution.GetPrevPrecedenceConstrainedTask(*it);
        heads[*it] = std::max(job_predecessor.has_value() ? solution.GetHead(*job_predecessor) + job_predecessor->get().GetDuration() : TimeType{},
                              heads[*std::prev(it)] + std::prev(it)->get().GetDuration());
    }

    //estimate tails
    std::unordered_map<std::reference_wrapper<const TaskType>, TimeType, std::hash<TaskType>, std::equal_to<TaskType>> tails;
    std::optional<std::reference_wrapper<const TaskType>> job_successor = solution.GetNextPrecedenceConstrainedTask(*std::prev(last));
    tails[*std::prev(last)] = std::max(job_successor.has_value() ? solution.GetTail(*job_successor) + job_successor->get().GetDuration() : TimeType{},
                                       after.has_value() ? solution.GetTail(*after) + after->get().GetDuration() : TimeType{});
    for (auto it = std::prev(last); it-- != first;) {
        job_successor = solution.GetNextPrecedenceConstrainedTask(*it);
        tails[*it] = std::max(job_successor.has_value() ? solution.GetTail(*job_successor) + job_successor->get().GetDuration() : TimeType{},
                              tails[*std::next(it)] + std::next(it)->get().GetDuration());
    }

    //estimate makespan
    TimeType makespan{};
    for (auto it = first; it != last; ++it) {
        makespan = std::max(makespan, heads[*it] + it->get().GetDuration() + tails[*it]);
    }
    return makespan;
}

/**
 * @brief Estimates the total weighted tardiness that results of changing the order
 * of a group of tasks in the same machine.
 * 
 * @tparam Iter type of the iterator to be used to read the group of tasks.
 * @tparam Solution type of the solution.
 * @param first iterator pointing to the first task in the new order.
 * @param last iterator pointing to the task past the last task in the new order.
 * @param solution solution for which the estimate will be calculated.
 * @param before task that is scheduled in the same machine before the first task in the group.
 * @param after task that is scheduled in the same machine after the last task in the group.
 * @return estimate of the total weighted tardiness for the new order of the tasks. 
 */
template <typename Iter, typename Solution>
static typename Solution::TimeType
EstimateTotalWeightedTardiness(Iter first,
                               Iter last,
                               const Solution& solution,
                               const std::optional<std::reference_wrapper<const typename Solution::TaskType>>& before,
                               const std::optional<std::reference_wrapper<const typename Solution::TaskType>>& after)
{
    using TaskType = typename Solution::TaskType;
    using TimeType = typename Solution::TimeType;
    using JobType = typename Solution::JobType;
    //estimate heads
    std::unordered_map<std::reference_wrapper<const TaskType>, TimeType, std::hash<TaskType>, std::equal_to<TaskType>> heads;
    std::optional<std::reference_wrapper<const TaskType>> job_predecessor = solution.GetPrevPrecedenceConstrainedTask(*first);
    heads[*first] = std::max(job_predecessor.has_value() ? solution.GetHead(*job_predecessor) + job_predecessor->get().GetDuration() : TimeType{},
                             before.has_value() ? solution.GetHead(*before) + before->get().GetDuration() : TimeType{});
    for (auto it = std::next(first); it != last; ++it) {
        job_predecessor = solution.GetPrevPrecedenceConstrainedTask(*it);
        heads[*it] = std::max(job_predecessor.has_value() ? solution.GetHead(*job_predecessor) + job_predecessor->get().GetDuration() : TimeType{},
                              heads[*std::prev(it)] + std::prev(it)->get().GetDuration());
    }

    //estimate tails
    std::vector<std::reference_wrapper<const JobType>> jobs;
    solution.GetProblem().GetJobs(std::back_inserter(jobs));
    TimeType twt{};
    for (const JobType& job: jobs) {
        std::unordered_map<std::reference_wrapper<const TaskType>, TimeType, std::hash<TaskType>, std::equal_to<TaskType>> tails;
        std::optional<std::reference_wrapper<const TaskType>> job_successor = solution.GetNextPrecedenceConstrainedTask(*std::prev(last));
        tails[*std::prev(last)] =
            std::max(job_successor.has_value() ? solution.GetTail(*job_successor, job) + job_successor->get().GetDuration() : TimeType{},
                     after.has_value() ? solution.GetTail(*after, job) + after->get().GetDuration() : TimeType{});
        for (auto it = std::prev(last); it-- != first;) {
            job_successor = solution.GetNextPrecedenceConstrainedTask(*it);
            tails[*it] = std::max(job_successor.has_value() ? solution.GetTail(*job_successor, job) + job_successor->get().GetDuration() : TimeType{},
                                  tails[*std::next(it)] + std::next(it)->get().GetDuration());
        }

        TimeType tardiness{};
        for (auto it = first; it != last; ++it) {
            tardiness = std::max(tardiness, heads[*it] + it->get().GetDuration() + tails[*it]);
        }
        twt += std::max(TimeType{}, tardiness - job.GetDueDate()) * job.GetWeight();
    }

    return twt;
}

/**
 * @brief Returns the quality that results of changing the order
 * of a group of tasks in the same machine.
 * 
 * @tparam Estimate if true an estimate will be used.
 * @tparam Solution type of the solution.
 * @tparam Move type of the move.
 * @tparam Iter type of the iterator to be used to read the group of tasks.
 * @param solution solution to be considered.
 * @param move move to be applied.
 * @param first iterator pointing to the first task in the new order.
 * @param last iterator pointing to the task past the last task in the new order.
 * @param before task that is scheduled in the same machine before the first task in the group.
 * @param after task that is scheduled in the same machine after the last task in the group.
 * @return quality for the new order of the tasks.  
 */
template <typename Estimate, typename Solution, typename Move, typename Iter>
static double GetQuality(Solution& solution,
                         Move& move,
                         [[maybe_unused]] Iter first,
                         [[maybe_unused]] Iter last,
                         const std::optional<std::reference_wrapper<const typename Solution::TaskType>>& before,
                         const std::optional<std::reference_wrapper<const typename Solution::TaskType>>& after)
{
    if constexpr (Estimate::value) {
        if constexpr (is_specialization<Solution, JSPMakespanMinimizationSolution>::value) {
            return 1.0 / EstimateMakespan(first, last, solution, before, after);
        } else if constexpr (is_specialization<Solution, JSPTotalWeightedTardinessMinimizationSolution>::value) {
            return 1.0 / EstimateTotalWeightedTardiness(first, last, solution, before, after);
        }
    } else {
        solution.ApplyMove(move);
        auto quality = solution.GetQuality();
        solution.ApplyMove(move.Invert());
        move.Invert();
        return quality;
    }
}

/**
 * @brief CET Neighborhood for JSP.
 * 
 * @tparam Problem JSP.
 * @tparam Move type of the moves to be returned.
 * @tparam Estimate if true an estimate will be used.
 */
template <typename Problem, template <typename> class Move = JSPMove, typename Estimate = std::false_type> class CET
{
  public:
    using ProblemType = Problem;
    using TaskType = typename ProblemType::TaskType;
    using MoveType = Move<Problem>;
    using BlockType = JSPRestrictionList<Problem>;

    /**
     * @brief Inserts in a container the CET neighbors of a solution.
     * 
     * @tparam Iter type of the iterator to be used to insert the neighbors.
     * @tparam Solution type of the solution whose neighbors will be calculated.
     * @param dest iterator to be used to insert the neighbors.
     * @param solution solution whose neighbors will be calculated.
     * @return an iterator to the neighbor past the last neighbor inserted. 
     */
    template <typename Iter, typename Solution> static Iter GetNeighbors(Iter dest, const Solution& solution)
    {
        std::vector<BlockType> critical_blocks;
        solution.template GetCriticalBlocks<BlockType>(std::back_inserter(critical_blocks));
        Solution copy(solution);
        for (const auto& block: critical_blocks) {
            std::deque<std::pair<std::reference_wrapper<const TaskType>, std::reference_wrapper<const TaskType>>> edges;
            block.GetRestrictions(std::back_inserter(edges));
            auto edge = edges.front();
            MoveType move;
            move.AddChange(edge.first, edge.second);
            std::vector<std::reference_wrapper<const TaskType>> new_order = {edge.second, edge.first};
            *dest++ = MoveData(move,
                               GetQuality<Estimate>(copy,
                                                    move,
                                                    new_order.begin(),
                                                    new_order.end(),
                                                    solution.GetPrevCapacityConstrainedTask(edge.first),
                                                    solution.GetNextCapacityConstrainedTask(edge.second)));
            if (block.GetNumberRestrictions() > 1) {
                edge = edges.back();
                move = MoveType{};
                move.AddChange(edge.first, edge.second);
                std::vector<std::reference_wrapper<const TaskType>> new_order = {edge.second, edge.first};
                *dest++ = MoveData(move,
                                   GetQuality<Estimate>(copy,
                                                        move,
                                                        new_order.begin(),
                                                        new_order.end(),
                                                        solution.GetPrevCapacityConstrainedTask(edge.first),
                                                        solution.GetNextCapacityConstrainedTask(edge.second)));
            }
        }
        return dest;
    }

    /**
     * @brief Checks if the solution uses estimates.
     * 
     * @return true if the solution uses estimates, false in other case.  
     */
    constexpr static bool UsesEstimates()
    {
        return Estimate::value;
    }
};

/**
 * @brief CEI Neighborhood for JSP.
 * 
 * @tparam Problem JSP.
 * @tparam Move type of the moves to be returned.
 * @tparam Estimate if true an estimate will be used.
 */
template <typename Problem, template <typename> class Move = JSPMove, typename Estimate = std::false_type> class CEI
{
  public:
    using ProblemType = Problem;
    using TaskType = typename ProblemType::TaskType;
    using TimeType = typename ProblemType::TimeType;
    using MoveType = Move<Problem>;
    using BlockType = JSPRestrictionList<Problem>;

    /**
     * @brief Inserts in a container the CEI neighbors of a solution.
     * 
     * @tparam Iter type of the iterator to be used to insert the neighbors.
     * @tparam Solution type of the solution whose neighbors will be calculated.
     * @param dest iterator to be used to insert the neighbors.
     * @param solution solution whose neighbors will be calculated.
     * @return an iterator to the neighbor past the last neighbor inserted. 
     */
    template <typename Iter, typename Solution> static Iter GetNeighbors(Iter dest, const Solution& solution)
    {
        std::vector<BlockType> critical_blocks;
        solution.template GetCriticalBlocks<BlockType>(std::back_inserter(critical_blocks));
        Solution copy(solution);

        for (const auto& block: critical_blocks) {
            std::deque<std::pair<std::reference_wrapper<const TaskType>, std::reference_wrapper<const TaskType>>> edges;
            block.GetRestrictions(std::back_inserter(edges));
            // shift the operations at the end
            for (auto it1 = edges.begin(); it1 != edges.end(); ++it1) {
                MoveType move;
                std::deque<std::reference_wrapper<const TaskType>> new_order;
                auto successor = solution.GetNextPrecedenceConstrainedTask(it1->first);
                auto completion_time =
                    successor.has_value() ? solution.GetHead(successor.value()) + successor.value().get().GetDuration() : TimeType{};

                for (auto it2 = it1; it2 != edges.end(); ++it2) {
                    auto predecessor = solution.GetPrevPrecedenceConstrainedTask(it2->second);
                    auto head = predecessor.has_value() ? solution.GetHead(predecessor.value()) : TimeType{};
                    if (head >= completion_time) {
                        break;
                    }
                    move.AddChange(it1->first, it2->second);
                    new_order.push_back(it2->second);
                }
                if (!new_order.empty()) {
                    new_order.push_back(it1->first);
                    *dest++ = MoveData(move,
                                       GetQuality<Estimate>(copy,
                                                            move,
                                                            new_order.begin(),
                                                            new_order.end(),
                                                            solution.GetPrevCapacityConstrainedTask(new_order.at(new_order.size() - 1)),
                                                            solution.GetNextCapacityConstrainedTask(new_order.at(new_order.size() - 2))));
                }
            }
            // shift the operations at the beginning
            for (auto it1 = edges.rbegin(); it1 != edges.rend(); ++it1) {
                MoveType move;
                std::deque<std::reference_wrapper<const TaskType>> new_order;
                auto predecessor = solution.GetPrevPrecedenceConstrainedTask(it1->second);
                auto head = predecessor.has_value() ? solution.GetHead(predecessor.value()) : TimeType{};

                for (auto it2 = it1; it2 != edges.rend(); ++it2) {
                    auto successor = solution.GetNextPrecedenceConstrainedTask(it2->first);
                    auto completion_time =
                        successor.has_value() ? solution.GetHead(successor.value()) + successor.value().get().GetDuration() : TimeType{};
                    if (head >= completion_time) {
                        break;
                    }
                    move.AddChange(it2->first, it1->second);
                    new_order.push_front(it2->first);
                }
                if (!new_order.empty()) {
                    new_order.push_front(it1->second);
                    *dest++ = MoveData(move,
                                       GetQuality<Estimate>(copy,
                                                            move,
                                                            new_order.begin(),
                                                            new_order.end(),
                                                            solution.GetPrevCapacityConstrainedTask(new_order.at(1)),
                                                            solution.GetNextCapacityConstrainedTask(new_order.at(0))));
                }
            }
        }
        return dest;
    }

    /**
     * @brief Checks if the solution uses estimates.
     * 
     * @return true if the solution uses estimates, false in other case.  
     */
    constexpr static bool UsesEstimates()
    {
        return Estimate::value;
    }
};

#endif /* JSPNEIGHBORHOODS_HPP_ */
