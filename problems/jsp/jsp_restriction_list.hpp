/**
 * @file jsp_restriction_list.hpp
 * @author Pablo
 * @brief JSP Restriction List.
 * @version 0.1
 * @date 09-11-2018
 * 
 * @copyright Copyright (c) 2018
 * 
 */
#ifndef JSPRESTRICTIONLIST_HPP_
#define JSPRESTRICTIONLIST_HPP_

#include <deque>

/**
 * @brief List that contains JSP restrictions (pairs of tasks).
 * 
 * @tparam Problem type of the problem.
 */
template <typename Problem> class JSPRestrictionList
{
  public:
    using ProblemType = Problem;
    using TaskType = typename ProblemType::TaskType;

  private:
    std::deque<std::pair<std::reference_wrapper<const TaskType>, std::reference_wrapper<const TaskType>>> restrictions;

  public:
    /**
     * @brief Adds a restriction to the begining of the list.
     * 
     * @param from task that is scheduled before.
     * @param to task that is scheduled after.
     */
    void AddRestrictionFront(const TaskType& from, const TaskType& to)
    {
        restrictions.push_front(std::make_pair(std::cref(from), std::cref(to)));
    }

    /**
     * @brief Adds a restriction to the end of the list.
     * 
     * @param from task that is scheduled before.
     * @param to task that is scheduled after.
     */
    void AddRestrictionBack(const TaskType& from, const TaskType& to)
    {
        restrictions.push_back(std::make_pair(std::cref(from), std::cref(to)));
    }

    /**
     * @brief Checks if the list has no elements.
     * 
     * @return true if the list has no elements, false in other case.
     */
    bool Empty() const
    {
        return restrictions.empty();
    }

    /**
     * @brief Returns the number of restrictions in the list.
     * 
     * @return the number of restrictions in the list.
     */
    int GetNumberRestrictions() const
    {
        return restrictions.size();
    }

    template <typename Iter> Iter GetRestrictions(Iter dest) const
    {
        return std::copy(restrictions.begin(), restrictions.end(), dest);
    }

    bool operator==(const JSPRestrictionList& other) const
    {
        return distance(restrictions.begin(), restrictions.end()) == distance(other.restrictions.begin(), other.restrictions.end()) &&
               equal(restrictions.begin(), restrictions.end(), other.restrictions.begin(), std::equal_to<std::pair<TaskType, TaskType>>());
        ;
    }

    bool operator!=(const JSPRestrictionList& other) const
    {
        return distance(restrictions.begin(), restrictions.end()) != distance(other.restrictions.begin(), other.restrictions.end()) ||
               !equal(restrictions.begin(), restrictions.end(), other.restrictions.begin(), std::equal_to<std::pair<TaskType, TaskType>>());
        ;
    }
};

#endif /* JSPRESTRICTIONLIST_HPP_ */
