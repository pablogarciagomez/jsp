/**
 * @file jsp_move.hpp
 * @author Pablo
 * @brief JSP Move.
 * @version 0.1
 * @date 09-11-2018
 * 
 * @copyright Copyright (c) 2018
 * 
 */
#ifndef JSPMOVE_HPP_
#define JSPMOVE_HPP_

#include <vector>

/**
 * @brief Move in a JSP.
 * 
 * @tparam Problem type of the problem.
 */
template <typename Problem> class JSPMove
{
  public:
    friend class std::hash<JSPMove>;
    using ProblemType = Problem;
    using TaskType = typename ProblemType::TaskType;

  private:
    std::vector<std::pair<std::reference_wrapper<const TaskType>, std::reference_wrapper<const TaskType>>> changes;

  public:
    /**
     * @brief Adds a change to the move.
     * 
     * @param from task that is scheduled before and will be scheduled after.
     * @param to task that is scheduled after and will be scheduled before.
     */
    void AddChange(const TaskType& from, const TaskType& to)
    {
        changes.push_back(std::make_pair(std::cref(from), std::cref(to)));
    }

    /**
     * @brief Inverts the move.
     * 
     * @return this move inverted.
     */
    JSPMove& Invert()
    {
        std::for_each(changes.begin(), changes.end(), [](auto& pair) { std::swap(pair.first, pair.second); });
        std::reverse(changes.begin(), changes.end());
        return *this;
    }

    /**
     * @brief Inserts in a container all the changes.
     * 
     * @tparam Iter type of the iterator to be used to insert the changes.
     * @param dest iterator to be used to insert the changes.
     * @return an iterator to the change past the last change inserted. 
     */
    template <typename Iter> Iter GetChanges(Iter dest) const
    {
        return std::copy(changes.begin(), changes.end(), dest);
    }

    bool operator==(const JSPMove& other) const
    {
        return distance(changes.begin(), changes.end()) == distance(other.changes.begin(), other.changes.end()) &&
               equal(changes.begin(), changes.end(), other.changes.begin(), std::equal_to<std::pair<TaskType, TaskType>>());
    }

    bool operator!=(const JSPMove& other) const
    {
        return distance(changes.begin(), changes.end()) != distance(other.changes.begin(), other.changes.end()) ||
               !equal(changes.begin(), changes.end(), other.changes.begin(), std::equal_to<std::pair<TaskType, TaskType>>());
    }
};

namespace std
{
    template <typename Problem> struct hash<JSPMove<Problem>>
    {
        size_t operator()(const JSPMove<Problem>& k) const
        {
            std::size_t seed = k.changes.size();
            for (auto& e: k.changes) {
                seed ^= std::hash<typename JSPMove<Problem>::TaskType>{}(e.first) ^
                        (std::hash<typename JSPMove<Problem>::TaskType>{}(e.second) + 0x9e3779b9 + (seed << 6) + (seed >> 2));
            }
            return seed;
        }
    };
}

#endif /* JSPMOVE_HPP_ */
