/**
 * @file jsp_makespan_minimization_solution.hpp
 * @author Pablo
 * @brief JSP Makespan Minimization.
 * @version 0.1
 * @date 09-11-2018
 * 
 * @copyright Copyright (c) 2018
 * 
 */
#ifndef JSPMAKESPANMINIMIZATIONSOLUTION_HPP_
#define JSPMAKESPANMINIMIZATIONSOLUTION_HPP_

#include <optional>
#include <queue>
#include <sstream>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include <utils/template_utils.hpp>
#include <utils/triangular_fuzzy_number.hpp>

/**
 * @brief Solution to a JSP minimizing the makespan.
 * 
 * @tparam Problem type of the problem to be solved.
 * @tparam Tails enables tails.
 */
template <typename Problem, typename Tails = std::false_type> class JSPMakespanMinimizationSolution
{
  public:
    using ProblemType = Problem; // type of the problem to be solved
    using TaskType = typename ProblemType::TaskType; // type of the tasks to be scheduled
    using JobType = typename ProblemType::JobType; // type of the jobs
    using MachineType = typename ProblemType::MachineType; // type of the machines
    using TimeType = typename TaskType::TimeType; // type of the time unit
  private:
    // struct to store the metadata of a task
    struct TaskMetadata
    {
        std::optional<std::reference_wrapper<const TaskType>> job_predecessor;
        std::optional<std::reference_wrapper<const TaskType>> job_successor;
        std::optional<std::reference_wrapper<const TaskType>> machine_predecessor;
        std::optional<std::reference_wrapper<const TaskType>> machine_successor;
        TimeType head;
        TimeType tail;
    };
    std::reference_wrapper<const ProblemType> problem; // problem to be solved
    mutable std::unordered_map<std::reference_wrapper<const TaskType>, TaskMetadata, std::hash<TaskType>, std::equal_to<TaskType>>
        disjunctive_graph; // disjunctive graph with the solution representation
    mutable std::unordered_set<std::reference_wrapper<const TaskType>, std::hash<TaskType>, std::equal_to<TaskType>>
        changes; // tasks that have changed since the last heads and tails update
    mutable TimeType makespan; // the current makespan

  public:
    /**
     * @brief Constructs a new JSPMakespanMinimizationSolution.
     * 
     * @param problem problem to be solved.
     */
    JSPMakespanMinimizationSolution(const ProblemType& problem) : problem{problem} {}

  private:
    /**
     * @brief Updates the head and tail of all the tasks that have been affected by modifications since the last call.
     * 
     */
    void UpdateHeadsAndTails() const
    {
        if (!changes.empty()) {
            std::vector<std::reference_wrapper<const TaskType>> tasks;
            GetTasksTopologicalOrder(std::back_inserter(tasks));
            // update the heads
            // find the first task whose head has to be updated
            auto it = tasks.begin();
            while (it != tasks.end()) {
                if (changes.count(*it) != 0) {
                    break;
                }
                ++it;
            }
            // calculate the head of the tasks that are scheduled after the first modified task
            while (it != tasks.end()) {
                auto [job_predecessor, machine_predecessor] = GetPrevTasks(*it);
                disjunctive_graph[*it].head = TimeType{};
                if (job_predecessor.has_value()) {
                    disjunctive_graph[*it].head =
                        std::max(disjunctive_graph.at(*it).head, disjunctive_graph.at(*job_predecessor).head + job_predecessor->get().GetDuration());
                }
                if (machine_predecessor.has_value()) {
                    disjunctive_graph[*it].head = std::max(
                        disjunctive_graph.at(*it).head, disjunctive_graph.at(*machine_predecessor).head + machine_predecessor->get().GetDuration());
                }
                ++it;
            }
            // update the tails
            if constexpr (Tails::value) {
                // find "the last" task whose tail has to be updated
                auto rit = tasks.rbegin();
                while (rit != tasks.rend()) {
                    if (changes.count(*rit) != 0) {
                        break;
                    }
                    ++rit;
                }
                // calculate the tail of the tasks that are scheduled before "the last" modified task
                while (rit != tasks.rend()) {
                    auto [job_successor, machine_successor] = GetNextTasks(*rit);
                    disjunctive_graph[*rit].tail = TimeType{};
                    if (job_successor.has_value()) {
                        disjunctive_graph[*rit].tail =
                            std::max(disjunctive_graph.at(*rit).tail, disjunctive_graph.at(*job_successor).tail + job_successor->get().GetDuration());
                    }
                    if (machine_successor.has_value()) {
                        disjunctive_graph[*rit].tail = std::max(
                            disjunctive_graph.at(*rit).tail, disjunctive_graph.at(*machine_successor).tail + machine_successor->get().GetDuration());
                    }
                    ++rit;
                }
            }
            changes.clear();
            // calculate the latest task to be completed
            std::vector<std::reference_wrapper<const TaskType>> final_tasks;
            GetProblem().GetFinalTasks(std::back_inserter(final_tasks));
            makespan = TimeType{};
            for (const TaskType& task: final_tasks) {
                makespan = std::max(makespan, task.GetDuration() + disjunctive_graph.at(task).head);
            }
        }
    }

    /**
     * @brief Checks if two numbers are equal.
     * 
     * @param n1 first number.
     * @param n2 second number.
     * @return true if the numbers are equal, false in other case.
     */
    bool EqualTime(const TimeType& n1, const TimeType& n2) const
    {
        if constexpr (is_specialization<TimeType, TriangularFuzzyNumber>::value) {
            return n1.GetSmallest() == n2.GetSmallest() || n1.GetMostProbable() == n2.GetMostProbable() || n1.GetLargest() == n2.GetLargest();
        } else {
            return n1 == n2;
        }
    };

  public:
    /**
     * @brief Returns the makespan.
     * 
     * @return the makespan of the solution. 
     */
    TimeType GetMakespan() const
    {
        UpdateHeadsAndTails();
        return makespan;
    }

    /**
     * @brief Returns the quality of the solution.
     * 
     * @return the quality of the solution. 
     */
    double GetQuality() const
    {
        return 1.0 / (double)GetMakespan();
    }

    /**
     * @brief Inserts in a container the tasks that are critical in this schedule.
     * 
     * @tparam Iter type of the iterator to be used to insert the tasks.
     * @param dest iterator to be used to insert the tasks.
     * @return an iterator to the task past the last task inserted. 
     */
    template <typename Iter> Iter GetCriticalTasks(Iter dest) const
    {
        static_assert(Tails::value, "GetCriticalTasks is only available when template parameter Tails is set to true");
        auto cmax = GetMakespan();
        for (const auto& [task, metadata]: disjunctive_graph) {
            if (EqualTime(metadata.head + metadata.tail + task.GetDuration(), cmax)) {
                *dest++ = task;
            }
        }
        return dest;
    }

    /**
     * @brief Inserts in a container the critical blocks of the schedule.
     * 
     * @tparam Block type of the critical blocks.
     * @tparam Iter type of the iterator to be used to insert the critical blocks.
     * @param dest iterator to be used to insert the critical blocks.
     * @return an iterator to the block past the last critical block inserted. 
     */
    template <typename Block, typename Iter> Iter GetCriticalBlocks(Iter dest) const
    {
        using BlockType = Block;

        UpdateHeadsAndTails();

        const auto recursive_critical_blocks = [this](Iter dest, const auto& lambda, const TaskType& current_task, BlockType current_block) -> Iter {
            // get previous tasks
            auto [job_predecessor, machine_predecessor] = GetPrevTasks(current_task);

            // if there are not any predecessors
            if (!job_predecessor.has_value() && !machine_predecessor.has_value()) {
                if (!current_block.Empty()) {
                    *dest++ = current_block;
                }
            }

            // if the job predecessor is critical
            if (job_predecessor.has_value() && EqualTime(job_predecessor->get().GetDuration() + disjunctive_graph.at(*job_predecessor).head,
                                                         disjunctive_graph.at(current_task).head)) {
                if (!current_block.Empty()) {
                    *dest++ = current_block;
                    dest = lambda(dest, lambda, *job_predecessor, BlockType{});
                } else {
                    dest = lambda(dest, lambda, *job_predecessor, current_block);
                }
            }

            // if the machine predecessor is critical
            if (machine_predecessor.has_value() &&
                EqualTime(machine_predecessor->get().GetDuration() + disjunctive_graph.at(*machine_predecessor).head,
                          disjunctive_graph.at(current_task).head)) {
                current_block.AddRestrictionFront(*machine_predecessor, current_task);
                dest = lambda(dest, lambda, *machine_predecessor, current_block);
            }

            return dest;
        };
        std::vector<std::reference_wrapper<const TaskType>> final_tasks;
        GetProblem().GetFinalTasks(std::back_inserter(final_tasks));
        for (const TaskType& final_task: final_tasks) {
            if (EqualTime(makespan, final_task.GetDuration() + disjunctive_graph.at(final_task).head)) {
                dest = recursive_critical_blocks(dest, recursive_critical_blocks, final_task, BlockType{});
            }
        }

        return dest;
    }

    /**
     * @brief Adds a task to the solution.
     * 
     * @param task task to be added.
     */
    void AddTask(const TaskType& task)
    {
        disjunctive_graph.insert(std::make_pair(std::cref(task), TaskMetadata{}));
        changes.insert(task);
    }

    /**
     * @brief Adds a precedence constraint between two tasks.
     * 
     * @param from task that is scheduled before.
     * @param to task that is scheduled after.
     */
    void AddPrecedenceConstraint(const TaskType& from, const TaskType& to)
    {
        if (disjunctive_graph.at(from).job_successor.has_value()) {
            throw std::invalid_argument("A restriction already exists");
        }
        if (disjunctive_graph.at(to).job_predecessor.has_value()) {
            throw std::invalid_argument("A restriction already exists");
        }
        disjunctive_graph.at(from).job_successor = to;
        disjunctive_graph.at(to).job_predecessor = from;
        changes.insert(from);
        changes.insert(to);
    }

    /**
     * @brief Adds a capacity constraint between two tasks.
     * 
     * @param from task that is scheduled before.
     * @param to task that is scheduled after.
     */
    void AddCapacityConstraint(const TaskType& from, const TaskType& to)
    {
        if (disjunctive_graph.at(from).machine_successor.has_value()) {
            throw std::invalid_argument("A restriction already exists");
        }
        if (disjunctive_graph.at(to).machine_predecessor.has_value()) {
            throw std::invalid_argument("A restriction already exists");
        }
        disjunctive_graph.at(from).machine_successor = to;
        disjunctive_graph.at(to).machine_predecessor = from;
        changes.insert(from);
        changes.insert(to);
    }

    /**
     * @brief Removes an existing precedence constraint between two tasks.
     * 
     * @param from task that was scheduled before.
     * @param to task that was scheduled after.
     */
    void RemovePrecedenceConstraint(const TaskType& from, const TaskType& to)
    {
        if (disjunctive_graph.at(from).job_successor != disjunctive_graph.at(to).job_predecessor) {
            throw std::invalid_argument("Restriction do not exist");
        }
        disjunctive_graph.at(from).job_successor.reset();
        disjunctive_graph.at(to).job_predecessor.reset();
        changes.insert(from);
        changes.insert(to);
    }

    /**
     * @brief Removes an existing capacity constraint between two tasks.
     * 
     * @param from task that was scheduled before.
     * @param to task that was scheduled after.
     */
    void RemoveCapacityConstraint(const TaskType& from, const TaskType& to)
    {
        if (disjunctive_graph.at(from).machine_successor != disjunctive_graph.at(to).machine_predecessor) {
            throw std::invalid_argument("Restriction do not exist");
        }
        disjunctive_graph.at(from).machine_successor.reset();
        disjunctive_graph.at(to).machine_predecessor.reset();
        changes.insert(from);
        changes.insert(to);
    }

    /**
     * @brief Exchanges the position of two tasks in the same machine.
     * This method only checks that both tasks belong to the same machine,
     * it do not check that the new schedule is feasible.
     * 
     * @param task1 first task.
     * @param task2 second task.
     */
    void ExchangeTasks(const TaskType& task1, const TaskType& task2)
    {
        if (task1.GetMachine() != task2.GetMachine()) {
            throw std::invalid_argument("Tasks do not belong to the same machine");
        }
        auto predecessor1 = disjunctive_graph.at(task1).machine_predecessor;
        auto predecessor2 = disjunctive_graph.at(task2).machine_predecessor;
        auto successor1 = disjunctive_graph.at(task1).machine_successor;
        auto successor2 = disjunctive_graph.at(task2).machine_successor;

        if (predecessor1.has_value()) {
            disjunctive_graph.at(*predecessor1).machine_successor = task2;
        }
        if (predecessor2.has_value()) {
            disjunctive_graph.at(*predecessor2).machine_successor = task1;
        }
        if (successor1.has_value()) {
            disjunctive_graph.at(*successor1).machine_predecessor = task2;
        }
        if (successor2.has_value()) {
            disjunctive_graph.at(*successor2).machine_predecessor = task1;
        }
        std::swap(disjunctive_graph.at(task1).machine_predecessor, disjunctive_graph.at(task2).machine_predecessor);
        std::swap(disjunctive_graph.at(task1).machine_successor, disjunctive_graph.at(task2).machine_successor);
        changes.insert(task1);
        changes.insert(task2);
    }

    /**
     * @brief Applies a move to the solution.
     * 
     * @tparam Move type of the move to be applied.
     * @param move move to be applied.
     */
    template <typename Move> void ApplyMove(const Move& move)
    {
        std::vector<std::pair<std::reference_wrapper<const TaskType>, std::reference_wrapper<const TaskType>>> changes;
        move.GetChanges(std::back_inserter(changes));
        for (const auto& [from, to]: changes) {
            ExchangeTasks(from, to);
        }
    }

    /**
     * @brief Inserts in a container all the tasks.
     * 
     * @tparam Iter type of the iterator to be used to insert the tasks.
     * @param dest iterator to be used to insert the tasks.
     * @return an iterator to the task past the last task inserted. 
     */
    template <typename Iter> Iter GetTasks(Iter dest) const
    {
        return GetProblem().GetTasks(dest);
    }

    /**
     * @brief Inserts in a container all the tasks in topological order.
     * 
     * @tparam Iter type of the iterator to be used to insert the tasks.
     * @param dest iterator to be used to insert the tasks.
     * @return an iterator to the task past the last task inserted. 
     */
    template <typename Iter> Iter GetTasksTopologicalOrder(Iter dest) const
    {
        std::unordered_map<std::reference_wrapper<const TaskType>, unsigned short, std::hash<TaskType>, std::equal_to<TaskType>> in_degree;
        std::queue<std::reference_wrapper<const TaskType>> available;
        std::size_t cnt = 0;

        for (const auto& [task, metadata]: disjunctive_graph) {
            auto d = (metadata.job_predecessor.has_value() ? 1 : 0) + (metadata.machine_predecessor.has_value() ? 1 : 0);
            in_degree[task] = d;
            if (d == 0) {
                available.push(task);
            }
        }

        while (!available.empty()) {
            const TaskType& current = available.front();
            available.pop();
            *dest++ = current;
            cnt++;
            auto [job_successor, machine_successor] = GetNextTasks(current);
            if (job_successor.has_value()) {
                if (--in_degree[*job_successor] == 0) {
                    available.push(*job_successor);
                }
            }
            if (machine_successor.has_value()) {
                if (--in_degree[*machine_successor] == 0) {
                    available.push(*machine_successor);
                }
            }
        }

        if (cnt != disjunctive_graph.size()) {
            throw std::invalid_argument("Not a DAG");
        }
        return dest;
    }

    /**
     * @brief Returns a pair with the tasks that are scheduled before the specified task. The first component is
     * the job predecessor and the second component the machine predecessor.
     * 
     * @param task task whose previous tasks will be returned.
     * @return a pair with the tasks that are scheduled before the specified task.
     */
    std::pair<std::optional<std::reference_wrapper<const TaskType>>, std::optional<std::reference_wrapper<const TaskType>>>
    GetPrevTasks(const TaskType& task) const
    {
        return std::make_pair(disjunctive_graph.at(task).job_predecessor, disjunctive_graph.at(task).machine_predecessor);
    }

    /**
     * @brief Returns a pair with the tasks that are scheduled after the specified task. The first component is
     * the job successor and the second component the machine successor.
     * 
     * @param task task whose following tasks will be returned.
     * @return a pair with the tasks that are scheduled after the specified task.
     */
    std::pair<std::optional<std::reference_wrapper<const TaskType>>, std::optional<std::reference_wrapper<const TaskType>>>
    GetNextTasks(const TaskType& task) const
    {
        return std::make_pair(disjunctive_graph.at(task).job_successor, disjunctive_graph.at(task).machine_successor);
    }

    /**
     * @brief Returns the task that is scheduled before in the same job as the specified task.
     * 
     * @param task task whose previous task will be returned.
     * @return the task that is scheduled before in the same job as the specified task.
     */
    std::optional<std::reference_wrapper<const TaskType>> GetPrevPrecedenceConstrainedTask(const TaskType& task) const
    {
        return disjunctive_graph.at(task).job_predecessor;
    }

    /**
     * @brief Returns the task that is scheduled after in the same job as the specified task.
     * 
     * @param task task whose following task will be returned.
     * @return the task that is scheduled after in the same job as the specified task.
     */
    std::optional<std::reference_wrapper<const TaskType>> GetNextPrecedenceConstrainedTask(const TaskType& task) const
    {
        return disjunctive_graph.at(task).job_successor;
    }

    /**
     * @brief Returns the task that is scheduled before in the same machine as the specified task.
     * 
     * @param task task whose previous task will be returned.
     * @return the task that is scheduled before in the same machine as the specified task.
     */
    std::optional<std::reference_wrapper<const TaskType>> GetPrevCapacityConstrainedTask(const TaskType& task) const
    {
        return disjunctive_graph.at(task).machine_predecessor;
    }

    /**
     * @brief Returns the task that is scheduled after in the same machine as the specified task.
     * 
     * @param task task whose following task will be returned.
     * @return the task that is scheduled after in the same machine as the specified task.
     */
    std::optional<std::reference_wrapper<const TaskType>> GetNextCapacityConstrainedTask(const TaskType& task) const
    {
        return disjunctive_graph.at(task).machine_successor;
    }

    /**
     * @brief Inserts in a container the initial tasks.
     * 
     * @tparam Iter type of the iterator to be used to insert the tasks.
     * @param dest iterator to be used to insert the tasks.
     * @return an iterator to the task past the last task inserted. 
     */
    template <typename Iter> Iter GetInitialTasks(Iter dest) const
    {
        return GetProblem().GetInitialTasks(dest);
    }

    /**
     * @brief Inserts in a container the final tasks.
     * 
     * @tparam Iter type of the iterator to be used to insert the tasks.
     * @param dest iterator to be used to insert the tasks.
     * @return an iterator to the task past the last task inserted. 
     */
    template <typename Iter> Iter GetFinalTasks(Iter dest) const
    {
        return GetProblem().GetFinalTasks(dest);
    }

    /**
     * @brief Returns the head of the specified task.
     * 
     * @param task task whose head will be returned.
     * @return the head of the specified task.
     */
    TimeType GetHead(const TaskType& task) const
    {
        UpdateHeadsAndTails();
        return disjunctive_graph.at(task).head;
    }

    /**
     * @brief Returns the tail of the specified task.
     * 
     * @param task task whose tail will be returned.
     * @return the tail of the specified task.
     */
    TimeType GetTail(const TaskType& task) const
    {
        static_assert(Tails::value, "GetTail is only available when template parameter Tails is set to true");
        UpdateHeadsAndTails();
        return disjunctive_graph.at(task).tail;
    }

    /**
     * @brief Returns the problem that the solution solves.
     * 
     * @return the problem that the solution solves.
     */
    const ProblemType& GetProblem() const
    {
        return problem;
    }

    /**
     * @brief Returns a string with the solution.
     * 
     * @return a string with the solution.
     */
    std::string SolutionSequence() const
    {
        std::string solution;
        std::vector<std::reference_wrapper<const TaskType>> ordered_tasks;
        GetTasksTopologicalOrder(std::back_inserter(ordered_tasks));
        std::unordered_map<std::reference_wrapper<const TaskType>, std::size_t, std::hash<TaskType>, std::equal_to<TaskType>> positions;
        std::size_t i = 0;
        for (const TaskType& task: ordered_tasks) {
            positions.insert(std::make_pair(std::cref(task), i++));
        }
        std::vector<std::reference_wrapper<const MachineType>> machines;
        GetProblem().GetMachines(std::back_inserter(machines));
        std::sort(
            machines.begin(), machines.end(), [](const MachineType& m1, const MachineType& m2) { return m1.GetMachineID() < m2.GetMachineID(); });
        for (const MachineType& machine: machines) {
            std::vector<std::reference_wrapper<const TaskType>> machine_tasks;
            GetProblem().GetMachineTasks(std::back_inserter(machine_tasks), machine.GetMachineID());
            std::sort(machine_tasks.begin(), machine_tasks.end(), [&positions](const auto& t1, const auto& t2) {
                return positions.at(t1) < positions.at(t2);
            });
            for (const TaskType& task: machine_tasks) {
                solution += std::to_string(task.GetJob().GetJobID()) + " ";
            }
            solution += "\n";
        }
        return solution;
    }

    /**
     * @brief Checks if the solution has tails enabled.
     * 
     * @return true if the solution has tails enabled, false in other case.    
     */
    constexpr static bool HasTails()
    {
        return Tails::value;
    }

    /**
     * @brief Returns a string representing the solution.
     * 
     * @return a string representing the solution.
     */
    inline std::string ToString() const
    {
        std::stringstream ss;
        ss << "Makespan " << GetMakespan() << "\n" << SolutionSequence();
        return ss.str();
    }

    bool operator==(const JSPMakespanMinimizationSolution& other) const
    {
        UpdateHeadsAndTails();
        return disjunctive_graph == other.disjunctive_graph;
    };

    bool operator!=(const JSPMakespanMinimizationSolution& other) const
    {
        UpdateHeadsAndTails();
        return disjunctive_graph != other.disjunctive_graph;
    };

    bool operator<(const JSPMakespanMinimizationSolution& other) const
    {
        return GetQuality() < other.GetQuality();
    };

    bool operator>(const JSPMakespanMinimizationSolution& other) const
    {
        return GetQuality() > other.GetQuality();
    };

    bool operator<=(const JSPMakespanMinimizationSolution& other) const
    {
        return GetQuality() <= other.GetQuality();
    };

    bool operator>=(const JSPMakespanMinimizationSolution& other) const
    {
        return GetQuality() >= other.GetQuality();
    };

    friend std::ostream& operator<<(std::ostream& os, const JSPMakespanMinimizationSolution& sol)
    {
        return os << sol.ToString();
    }
};

#endif /* JSPMAKESPANMINIMIZATIONSOLUTION_HPP_ */
