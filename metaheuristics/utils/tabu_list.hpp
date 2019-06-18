/**
 * @file tabu_list.hpp
 * @author Pablo
 * @brief Tabu List.
 * @version 0.1
 * @date 06-04-2019
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef TABULIST_HPP_
#define TABULIST_HPP_

#include <deque>
#include <optional>
#include <unordered_set>
#include <utility>

/**
 * @brief Tabu List.
 * 
 * @tparam T type of the elements.
 */
template <typename T> class TabuList
{
  private:
    std::size_t capacity;
    std::deque<T> queue;
    std::unordered_multiset<T> elements;

  public:
    /**
   * @brief Constructs a new TabuList.
   * 
   * @param capacity capacity of the tabu list.
   */
    explicit TabuList(std::size_t capacity) : capacity(capacity) {}

    /**
     * @brief Pushes the element to the end of the tabu list.
     * 
     * @param value value to insert in the tabu list.
     * @return true if the value was inserted false if it was not inserted because the list was full.
     */
    bool Push(const T& value)
    {
        if (queue.size() == capacity) {
            return false;
        } else {
            elements.insert(value);
            queue.push_back(value);
            return true;
        }
    }

    /**
     * @brief Removes the element at the front of the tabu list.
     * 
     * @return the element removed.
     */
    std::optional<T> Pop()
    {
        if (queue.empty()) {
            return std::nullopt;
        } else {
            elements.extract(queue.front());
            std::optional<T> result = queue.front();
            queue.pop_front();
            return result;
        }
    }

    /**
     * @brief Pushes the element to the end of the tabu list removing 
     * the element at the front if the tabu list is full.
     * 
     * @param value value to insert in the tabu list.
     * @return the element removed.
     */
    std::optional<T> ForcePush(const T& value)
    {
        if (queue.size() == capacity) {
            auto old = Pop();
            elements.insert(value);
            queue.push_back(value);
            return old;
        } else {
            elements.insert(value);
            queue.push_back(value);
            return std::nullopt;
        }
    }

    /**
     * @brief Checks if the element is in the tabu list.
     * 
     * @param value value to search for.
     * @return true if the element is in the tabu list, otherwise false. 
     */
    bool Contains(const T& value)
    {
        return elements.count(value) != 0;
    }

    /**
     * @brief Changes the capacity of the tabu list. If the current size is 
     * greater than the new one, the container is reduced to its last elements. 
     * 
     * @param new_capacity new_capacity of the tabu list.
     */
    void ChangeCapacity(std::size_t new_capacity)
    {
        if (queue.size() > new_capacity) {
            std::size_t pop = queue.size() - new_capacity;
            for (std::size_t i = 0; i < pop; i++) {
                elements.extract(queue.front());
                queue.pop_front();
            }
        }
        capacity = new_capacity;
    }

    /**
     * @brief Erases all elements from the tabu list.
     * 
     */
    void Clear()
    {
        elements.clear();
        queue.clear();
    }

    /**
     * @brief Returns the capacity of the tabu list.
     * 
     * @return the capacity of the tabu list.
     */
    std::size_t Capacity() const
    {
        return capacity;
    }

    /**
     * @brief Returns the current number of elements in the tabu list.
     * 
     * @return the current number of elements in the tabu list.
     */
    std::size_t CurrentSize() const
    {
        return queue.size();
    }

    friend std::ostream& operator<<(std::ostream& os, const TabuList& tl)
    {
        for (const auto& v: tl.queue) {
            os << v << " ";
        }
        return os;
    }
};

#endif /* TABULIST_HPP_ */