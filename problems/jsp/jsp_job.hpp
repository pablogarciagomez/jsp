/**
 * @file jsp_job.hpp
 * @author Pablo
 * @brief JSP Job.
 * @version 0.1
 * @date 21-02-2019
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef JSPJOB_HPP_
#define JSPJOB_HPP_

#include <string>
#include <utility>

/**
 * @brief Job of a JSP.
 * 
 * @tparam Date type of the date unit.
 */
template <typename Date> class JSPJob
{
  public:
    using DateType = Date; // type of the date unit
  private:
    unsigned int jobID; // identifier of the job
    DateType due_date; // due date of the job
    double weight; // weight (importance) of the job

  public:
    /**
     * @brief Constructs a new JSPJob.
     * 
     * @param jobID identifier of the job.   
     * @param due_date due date of the job.
     * @param weight weight (importance) of the job.
     */
    JSPJob(unsigned int jobID, DateType due_date = DateType(), double weight = 1) : jobID{jobID}, due_date{due_date}, weight{weight} {}

    /**
     * @brief Returns the identifier of the job.
     * 
     * @return the identifier of the job.
     */
    unsigned int GetJobID() const
    {
        return jobID;
    }

    /**
     * @brief Returns the due date of the job.
     * 
     * @return the due date of the job.
     */
    DateType GetDueDate() const
    {
        return due_date;
    }

    /**
     * @brief Returns the weight of the job.
     * 
     * @return the weight of the job.
     */
    double GetWeight() const
    {
        return weight;
    }

    bool operator==(const JSPJob& other) const
    {
        return jobID == other.jobID;
    }

    bool operator!=(const JSPJob& other) const
    {
        return jobID != other.jobID;
    }

    friend std::ostream& operator<<(std::ostream& os, const JSPJob& job)
    {
        return os << "{" << job.jobID << "," << job.due_date << "," << job.weight << "}";
    }
};

namespace std
{
    template <typename Date> struct hash<JSPJob<Date>>
    {
        size_t operator()(const JSPJob<Date>& k) const
        {
            return k.GetJobID();
        }
    };
}

#endif /* JSPJOB_HPP_ */
