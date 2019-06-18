/**
 * @file container_utils.hpp
 * @author Pablo
 * @brief Contains utilities to work with the standard library containers.
 * @version 0.1
 * @date 09-11-2018
 * 
 * @copyright Copyright (c) 2018
 * 
 */
#ifndef CONTAINERUTILS_HPP_
#define CONTAINERUTILS_HPP_

#include <utility>

/**
 * @brief A mix of std::copy_if and std::transform
 * 
 * @tparam InputIt type of the iterator to be used to read the elements.
 * @tparam OutputIt type of the iterator to be used to insert the elements.
 * @tparam UnaryPredicate type of the predicate to be used to filter the elements.
 * @tparam UnaryOperation type of function object to be applied to the elements.
 * @param first iterator pointing to the first element to be processed.
 * @param last iterator pointing to the element past the last element to be processed.
 * @param d_first iterator to be used to insert the elements.
 * @param pred predicate to be used to filter the elements.
 * @param unary_op function object to be applied to the elements.
 * @return an iterator to the element past the last element inserted. 
 */
template <class InputIt, class OutputIt, class UnaryPredicate, class UnaryOperation>
OutputIt filter_transform(InputIt first, InputIt last, OutputIt d_first, UnaryPredicate pred, UnaryOperation unary_op)
{
    while (first != last) {
        if (pred(*first))
            *d_first++ = unary_op(*first);
        ++first;
    }
    return d_first;
}

namespace
{
    /**
     * @brief Returns the hash of an object.
     * 
     * @tparam T type of the object.
     * @param v object to be hashed.
     * @return the hash of the object. 
     */
    template <typename T> inline std::size_t hash_combine(const T& v)
    {
        return std::hash<T>{}(v);
    }
}

/**
 * @brief Combines the hash of various objects.
 * 
 * @tparam T type of the first object.
 * @tparam Rest types of the rest of the objects.
 * @param v first object to be hashed.
 * @param rest rest of the objects to be hashed.
 * @return the combined hash of the objects.
 */
template <typename T, typename... Rest> inline std::size_t hash_combine(const T& v, const Rest&... rest)
{
    std::size_t seed = hash_combine(rest...);
    seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    return seed;
}

/**
 * @brief Hash for std::pair.
 * 
 */
struct pair_hash
{
    template <typename T1, typename T2> inline std::size_t operator()(const std::pair<T1, T2>& k) const
    {
        return hash_combine(k.first, k.second);
    }
};

/**
 * @brief Hash for std::vector.
 * 
 */
struct vector_hash
{
    template <typename T, typename ElementHash = std::hash<T>>
    inline std::size_t operator()(const std::vector<T>& v, const ElementHash& hash = ElementHash{}) const
    {
        std::size_t seed = v.size();
        for (auto& e: v) {
            seed ^= hash(e) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};

#endif /* CONTAINERUTILS_HPP_ */