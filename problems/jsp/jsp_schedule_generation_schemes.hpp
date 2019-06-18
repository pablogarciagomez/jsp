/**
 * @file jsp_schedule_generation_schemes.hpp
 * @author Pablo
 * @brief JSP Schedule Generation Schemes.
 * @version 0.1
 * @date 13-02-2019
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef JSPSCHEDULEGENERATIONSCHEMES_HPP_
#define JSPSCHEDULEGENERATIONSCHEMES_HPP_

#include <algorithm>
#include <iterator>
#include <random>
#include <vector>

/**
 * @brief Builds a JSP solution according to the specified priorities.
 * 
 * @tparam Solution type of the solution to generate.
 * @tparam Iter type of the iterator to be used to read the priorities.
 * @tparam Problem type of the problem to be considered.
 * @param first iterator pointing to the first task.
 * @param last iterator pointing to the task past the last task.
 * @param problem problem to be considered.
 * @return the solution to the problem according to the given priorities.
 */
template <typename Solution, typename Iter, typename Problem> Solution BuildSolution(Iter first, Iter last, const Problem& problem)
{
    using TaskType = typename Problem::TaskType;
    using JobType = typename Problem::JobType;
    using MachineType = typename Problem::MachineType;

    Solution solution(problem);
    std::unordered_map<typename Problem::TaskType, typename std::iterator_traits<Iter>::value_type::second_type> priorities(first, last);

    // establish the precedence constraints between tasks in the same job
    std::vector<std::reference_wrapper<const JobType>> jobs;
    problem.GetJobs(std::back_inserter(jobs));
    for (const JobType& job: jobs) {
        std::vector<std::reference_wrapper<const TaskType>> job_tasks;
        problem.GetJobTasks(std::back_inserter(job_tasks), job.GetJobID());
        solution.AddTask(job_tasks.at(0));
        for (std::size_t i = 1; i < job_tasks.size(); i++) {
            solution.AddTask(job_tasks.at(i));
            solution.AddPrecedenceConstraint(job_tasks.at(i - 1), job_tasks.at(i));
        }
    }

    // establish the capacity constraints between tasks in the same machine according to the specified priorities
    std::vector<std::reference_wrapper<const MachineType>> machines;
    problem.GetMachines(std::back_inserter(machines));
    for (const MachineType& machine: machines) {
        std::vector<std::reference_wrapper<const TaskType>> machine_tasks;
        problem.GetMachineTasks(std::back_inserter(machine_tasks), machine.GetMachineID());
        std::sort(machine_tasks.begin(), machine_tasks.end(), [&priorities](const auto& a, const auto& b) { return priorities[a] < priorities[b]; });
        for (std::size_t i = 1; i < machine_tasks.size(); i++) {
            solution.AddCapacityConstraint(machine_tasks.at(i - 1), machine_tasks.at(i));
        }
    }
    return solution;
}

/**
 * @brief G&T scheduler for JSP.
 * 
 */
class GT
{
  public:
    /**
     * @brief Inserts in a container the earliest starting times of the tasks according to
     * the specified priorities.
     *      
     * @tparam InputIt type of the iterator to be used to read the priorities.
     * @tparam OutputIt type of the iterator to be used to insert the earliest starting times of the tasks.
     * @tparam Problem type of the problem to be considered.
     * @param first iterator pointing to the first element in the encoded solution.
     * @param last iterator pointing to the element past the last element in the encoded solution.
     * @param dest iterator to be used to insert the earliest starting times of the tasks.
     * @param problem problem to be considered.
     * @return the earliest starting times of the tasks.
     */
    template <typename InputIt, typename OutputIt, typename Problem>
    static OutputIt EvaluateSolution(InputIt first, InputIt last, OutputIt dest, const Problem& problem)
    {
        using TaskType = typename Problem::TaskType;
        using MachineType = typename Problem::MachineType;
        using TimeType = typename Problem::TimeType;

        std::unordered_map<std::reference_wrapper<const TaskType>, std::size_t, std::hash<TaskType>, std::equal_to<TaskType>> priority(
            first, last); // priorities of the elements
        std::unordered_map<std::reference_wrapper<const TaskType>, TimeType, std::hash<TaskType>, std::equal_to<TaskType>>
            est; // earliest starting time of the tasks
        std::unordered_map<std::reference_wrapper<const MachineType>, TimeType, std::hash<MachineType>, std::equal_to<MachineType>>
            machine_times; // earliest starting time of a new task in the machines

        // final tasks
        std::unordered_set<std::reference_wrapper<const TaskType>, std::hash<TaskType>, std::equal_to<TaskType>> final_tasks;
        problem.GetFinalTasks(std::inserter(final_tasks, final_tasks.begin()));

        // current available tasks
        std::unordered_set<std::reference_wrapper<const TaskType>, std::hash<TaskType>, std::equal_to<TaskType>> available_tasks;
        problem.GetInitialTasks(std::inserter(available_tasks, available_tasks.begin()));

        while (!available_tasks.empty()) {
            // select task with the earliest possible completion time
            const TaskType& candidate_task =
                *std::min_element(available_tasks.begin(), available_tasks.end(), [&est, &machine_times](const TaskType& t1, const TaskType& t2) {
                    return std::max(est[t1], machine_times[t1.GetMachine()]) + t1.GetDuration() <
                           std::max(est[t2], machine_times[t2.GetMachine()]) + t2.GetDuration();
                });

            auto candidate_task_ect = std::max(est[candidate_task], machine_times[candidate_task.GetMachine()]) + candidate_task.GetDuration();
            // select all tasks in the same machine that may start before the selected task is completed
            std::vector<std::reference_wrapper<const TaskType>> conflict_set;
            std::copy_if(available_tasks.begin(),
                         available_tasks.end(),
                         std::back_inserter(conflict_set),
                         [&est, &machine_times, &candidate_task, &candidate_task_ect](const TaskType& t) {
                             return t.GetMachine() == candidate_task.GetMachine() &&
                                    std::max(est[t], machine_times[t.GetMachine()]) < candidate_task_ect;
                         });

            // select the task with the highest priority
            const TaskType& current_task =
                *std::min_element(conflict_set.begin(), conflict_set.end(), [&priority](const TaskType& t1, const TaskType& t2) {
                    return priority[t1] < priority[t2];
                });
            available_tasks.erase(current_task);
            auto current_task_est = std::max(est[current_task], machine_times[current_task.GetMachine()]);
            est[current_task] = current_task_est;
            machine_times[current_task.GetMachine()] = current_task_est + current_task.GetDuration();
            // add the successor to the available tasks set
            if (final_tasks.count(current_task) == 0) {
                const TaskType& next_task = problem.GetTask(current_task.GetJob().GetJobID(), current_task.GetPosition() + 1);
                available_tasks.insert(next_task);
                est[next_task] = current_task_est + current_task.GetDuration();
            }
            // store the result
            *dest++ = std::make_pair(std::cref(current_task), current_task_est);
        }
        return dest;
    }
};

#endif /* JSPSCHEDULEGENERATIONSCHEMES_HPP_ */