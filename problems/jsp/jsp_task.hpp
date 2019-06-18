/**
 * @file jsp_task.hpp
 * @author Pablo
 * @brief JSP Task.
 * @version 0.1
 * @date 09-11-2018
 * 
 * @copyright Copyright (c) 2018
 * 
 */
#ifndef JSPTASK_HPP_
#define JSPTASK_HPP_

#include <string>
#include <utility>

#include <utils/container_utils.hpp>

/**
 * @brief Task of a JSP.
 * 
 * @tparam Time type of the time unit.
 * @tparam Job type of the job to which the task belongs.
 * @tparam Machine type of the machine to which the task belongs.
 */
template <typename Time, typename Job, typename Machine> class JSPTask
{
  public:
    using TimeType = Time; // type of the time unit
    using JobType = Job; // type of the job to which the task belongs
    using MachineType = Machine; // type of the machine to which the task belongs
  private:
    unsigned int taskID; // identifier of the task
    std::reference_wrapper<const JobType> job; // the job to which the task belongs
    std::reference_wrapper<const MachineType> machine; // the machine in which the task has to be processed
    std::size_t position; // position of the task in the job
    TimeType duration; // duration of the task

  public:
    /**
     * @brief Constructs a new JSPTask.
     * 
     * @param taskID identifier of the task.
     * @param jobID the job to which the task belongs.
     * @param machineID the machine in which the task has to be processed.
     * @param position position of the task in the job.
     * @param duration duration of the task.
     */
    JSPTask(unsigned int taskID, const JobType& job, const MachineType& machine, std::size_t position, TimeType duration) :
        taskID{taskID},
        job{job},
        machine{machine},
        position{position},
        duration{duration}
    {}

    /**
     * @brief Returns the identifier of the task.
     * 
     * @return the identifier of the task.
     */
    unsigned int GetTaskID() const
    {
        return taskID;
    }

    /**
     * @brief Returns the job to which the task belongs.
     * 
     * @return the job to which the task belongs.
     */
    const JobType& GetJob() const
    {
        return job;
    }

    /**
     * @brief Returns the machine in which the task has to be processed.
     * 
     * @return the machine in which the task has to be processed.
     */
    const MachineType& GetMachine() const
    {
        return machine;
    }

    /**
     * @brief Returns the position of the task in the job.
     * 
     * @return the position of the task in the job.
     */
    std::size_t GetPosition() const
    {
        return position;
    }

    /**
     * @brief Returns the duration of the task.
     * 
     * @return the duration of the task.
     */
    TimeType GetDuration() const
    {
        return duration;
    }

    bool operator==(const JSPTask& other) const
    {
        return taskID == other.taskID;
    }

    bool operator!=(const JSPTask& other) const
    {
        return taskID != other.taskID;
    }

    friend std::ostream& operator<<(std::ostream& os, const JSPTask& task)
    {
        return os << "{" << task.taskID << "," << task.job << "," << task.machine << "," << task.duration << "," << task.position << "}";
    }
};

namespace std
{
    template <typename Time, typename Job, typename Machine> struct hash<JSPTask<Time, Job, Machine>>
    {
        size_t operator()(const JSPTask<Time, Job, Machine>& k) const
        {
            return k.GetTaskID();
        }
    };
}

#endif /* JSPTASK_HPP_ */
