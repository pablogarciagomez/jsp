/**
 * @file jsp_readers.hpp
 * @author Pablo
 * @brief Readers for JSP instances.
 * @version 0.1
 * @date 09-11-2018
 * 
 * @copyright Copyright (c) 2018
 * 
 */
#ifndef JSPREADERS_HPP_
#define JSPREADERS_HPP_

#include <iostream>
#include <string>
#include <utility>

#include <problems/jsp/jsp.hpp>

/**
 * @brief Reads a JSP instance in standard format.
 * 
 * @tparam Task type of the tasks.
 * @tparam Job type of the jobs.
 * @tparam Machine type of the machines.
 * @param is input stream. 
 * @return the problem read.
 */
template <typename Task, typename Job, typename Machine> JSP<Task, Job, Machine> read_standard(std::istream& is)
{
    JSP<Task, Job, Machine> jsp;
    unsigned int jobs;
    unsigned int machines;
    unsigned int machine;
    typename Task::TimeType duration;
    is >> jobs;
    is >> machines;
    for (unsigned int j = 1; j <= jobs; j++) {
        jsp.AddJob(j);
        for (unsigned int m = 0; m < machines; m++) {
            is >> machine;
            is >> duration;
            jsp.AddMachine(machine + 1);
            jsp.AddTask(j * machines + m + 1, j, machine + 1, duration);
        }
    }
    return jsp;
}

/**
 * @brief Reads a JSP instance with due dates in standard format.
 * 
 * @tparam Task type of the tasks.
 * @tparam Job type of the jobs.
 * @tparam Machine type of the machines.
 * @param is input stream. 
 * @return the problem read.
 */
template <typename Task, typename Job, typename Machine> JSP<Task, Job, Machine> read_standard_due_dates(std::istream& is)
{
    JSP<Task, Job, Machine> jsp;
    unsigned int jobs;
    unsigned int machines;
    unsigned int machine;
    typename Task::TimeType duration;
    typename Job::DateType due_date;
    double weight;
    is >> jobs;
    is >> machines;
    for (unsigned int j = 1; j <= jobs; j++) {
        is >> due_date;
        is >> weight;
        jsp.AddJob(j, due_date, weight);
        for (unsigned int m = 0; m < machines; m++) {
            is >> machine;
            is >> duration;
            jsp.AddMachine(machine + 1);
            jsp.AddTask(j * machines + m + 1, j, machine + 1, duration);
        }
    }
    return jsp;
}

#endif /* JSPREADERS_HPP_ */