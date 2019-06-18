/**
 * @file jsp.hpp
 * @author Pablo
 * @brief JSP formulation.
 * @version 0.1
 * @date 09-11-2018
 * 
 * @copyright Copyright (c) 2018
 * 
 */
#ifndef JSP_HPP_
#define JSP_HPP_

#include <algorithm>
#include <deque>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <utils/triangular_fuzzy_number.hpp>

/**
 * @brief JSP formulation.
 * 
 * @tparam Task type of the tasks to be scheduled.
 * @tparam Job type of the jobs to be scheduled.
 * @tparam Machine type of the machines to be scheduled.
 */
template <typename Task, typename Job, typename Machine> class JSP
{
  public:
    using TaskType = Task;
    using JobType = Job;
    using MachineType = Machine;
    using TimeType = typename TaskType::TimeType;
    using DateType = typename JobType::DateType;

  private:
    std::unordered_map<unsigned int, TaskType> tasks;
    std::unordered_map<unsigned int, JobType> jobs;
    std::unordered_map<unsigned int, MachineType> machines;
    std::unordered_map<unsigned int, std::deque<std::reference_wrapper<const TaskType>>> job_tasks;
    std::unordered_map<unsigned int, std::unordered_set<std::reference_wrapper<const TaskType>, std::hash<TaskType>, std::equal_to<TaskType>>>
        machine_tasks;

  public:
    /**
     * @brief Adds a task to the problem after the last task of the specified job to be proccesed in the specified machine.
     * 
     * @param taskID identifier of the task.
     * @param jobID identifier of the job to which the task belongs.
     * @param machineID identifier of the machine in which the task has to be proccesed.
     * @param duration duration of the task.
     */
    void AddTask(unsigned int taskID, unsigned int jobID, unsigned int machineID, TimeType duration)
    {
        if (jobs.count(jobID) == 0) {
            throw std::invalid_argument("the job is not registered");
        }
        if (machines.count(machineID) == 0) {
            throw std::invalid_argument("the machine is not registered");
        }
        tasks.insert(std::make_pair(taskID, TaskType{taskID, jobs.at(jobID), machines.at(machineID), job_tasks[jobID].size(), duration}));
        job_tasks[jobID].push_back(tasks.at(taskID));
        machine_tasks[machineID].insert(tasks.at(taskID));
    }

    /**
     * @brief Adds a job to the problem.
     * 
     * @param jobID identifier of the job.
     * @param due_date due date of the job.
     * @param weight weight(importance) of the job.
     */
    void AddJob(unsigned int jobID, DateType due_date = DateType(), double weight = 1)
    {
        jobs.insert(std::make_pair(jobID, JobType{jobID, due_date, weight}));
    }

    /**
     * @brief Adds a machine to the problem.
     * 
     * @param machineID identifier of the machine.
     */
    void AddMachine(unsigned int machineID)
    {
        machines.insert(std::make_pair(machineID, MachineType{machineID}));
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
        return std::transform(tasks.begin(), tasks.end(), dest, [](const auto& pair) { return std::cref(pair.second); });
    }

    /**
     * @brief Inserts in a container all the jobs.
     * 
     * @tparam Iter type of the iterator to be used to insert the jobs.
     * @param dest iterator to be used to insert the jobs.
     * @return an iterator to the job past the last job inserted.  
     */
    template <typename Iter> Iter GetJobs(Iter dest) const
    {
        return std::transform(jobs.begin(), jobs.end(), dest, [](const auto& pair) { return std::cref(pair.second); });
    }

    /**
     * @brief Inserts in a container all the machines.
     * 
     * @tparam Iter type of the iterator to be used to insert the machines.
     * @param dest iterator to be used to insert the machines.
     * @return an iterator to the machine past the last machine inserted.   
     */
    template <typename Iter> Iter GetMachines(Iter dest) const
    {
        return std::transform(machines.begin(), machines.end(), dest, [](const auto& pair) { return std::cref(pair.second); });
    }

    /**
     * @brief Returns the task with the specified identifier.
     * 
     * @param taskID identifier of the task.
     * @return the task with the specified identifier 
     */
    const TaskType& GetTask(unsigned int taskID) const
    {
        return tasks.at(taskID);
    }

    /**
     * @brief Returns the task of the specified job that is in the specified position.
     * 
     * @param jobID identifier of the job to which the task belongs.
     * @param position position relative to the job in which the task is processed.
     * @return the task of the specified job that is in the specified position. 
     */
    const TaskType& GetTask(unsigned int jobID, std::size_t position) const
    {
        return job_tasks.at(jobID).at(position);
    }

    /**
     * @brief Returns the job with the specified identifier.
     * 
     * @param jobID identifier of the job.
     * @return the job with the specified identifier. 
     */
    const JobType& GetJob(unsigned int jobID) const
    {
        return jobs.at(jobID);
    }

    /**
     * @brief Returns the machine with the specified identifier.
     * 
     * @param machineID identifier of the machine.
     * @return the machine with the specified identifier. 
     */
    const MachineType& GetMachine(unsigned int machineID) const
    {
        return machines.at(machineID);
    }

    /**
     * @brief Inserts in a container all the tasks that belong to the specified job.
     * 
     * @tparam Iter type of the iterator to be used to insert the tasks.
     * @param dest iterator to be used to insert the tasks. 
     * @param jobID identifier of the job.
     * @return an iterator to the task past the last task inserted. 
     */
    template <typename Iter> Iter GetJobTasks(Iter dest, unsigned int jobID) const
    {
        return std::copy(job_tasks.at(jobID).begin(), job_tasks.at(jobID).end(), dest);
    }

    /**
     * @brief Inserts in a container all the tasks that have to be processed in the specified machine.
     * 
     * @tparam Iter type of the iterator to be used to insert the tasks.
     * @param dest iterator to be used to insert the tasks. 
     * @param machineID identifier of the machine.
     * @return an iterator to the task past the last task inserted. 
     */
    template <typename Iter> Iter GetMachineTasks(Iter dest, unsigned int machineID) const
    {
        return std::copy(machine_tasks.at(machineID).begin(), machine_tasks.at(machineID).end(), dest);
    }

    /**
     * @brief Returns the number of tasks.
     * 
     * @return the number of tasks.
     */
    std::size_t GetNumberOfTasks() const
    {
        return tasks.size();
    }

    /**
     * @brief Returns the number of jobs.
     * 
     * @return the number of jobs.
     */
    std::size_t GetNumberOfJobs() const
    {
        return jobs.size();
    }

    /**
     * @brief Returns the number of machines.
     * 
     * @return the number of machines.
     */
    std::size_t GetNumberOfMachines() const
    {
        return machines.size();
    }

    /**
     * @brief Returns the number of tasks that belong to the specified job.
     * 
     * @param jobID identifier of the job.
     * @return the number of tasks that belong to the specified job.
     */
    std::size_t GetNumberOfTasksInJob(unsigned int jobID) const
    {
        return job_tasks.at(jobID).size();
    }

    /**
     * @brief Returns the number of tasks that have to be processed in the specified machine.
     * 
     * @param machineID identifier of the machine.
     * @return the number of tasks that have to be processed in the specified machine.
     */
    std::size_t GetNumberOfTasksInMachine(unsigned int machineID) const
    {
        return machine_tasks.at(machineID).size();
    }

    /**
     * @brief Inserts in a container the initial tasks (the first task of each job).
     * 
     * @tparam Iter type of the iterator to be used to insert the tasks.
     * @param dest iterator to be used to insert the tasks. 
     * @return an iterator to the task past the last task inserted. 
     */
    template <typename Iter> Iter GetInitialTasks(Iter dest) const
    {
        for (const auto& job: job_tasks) {
            *dest++ = job.second.front();
        }
        return dest;
    }

    /**
     * @brief Inserts in a container the final tasks (the last task of each job).
     * 
     * @tparam Iter type of the iterator to be used to insert the tasks.
     * @param dest iterator to be used to insert the tasks. 
     * @return an iterator to the task past the last task inserted. 
     */
    template <typename Iter> Iter GetFinalTasks(Iter dest) const
    {
        for (const auto& job: job_tasks) {
            *dest++ = job.second.back();
        }
        return dest;
    }
};

#endif /* JSP_HPP_ */
