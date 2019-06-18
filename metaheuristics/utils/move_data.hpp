/**
 * @file move_data.hpp
 * @author Pablo
 * @brief Struture to communicate the problems with the local search algorithms.
 * @version 0.1
 * @date 09-11-2018
 * 
 * @copyright Copyright (c) 2018
 * 
 */
#ifndef MOVEDATA_HPP_
#define MOVEDATA_HPP_

/**
 * @brief Auxiliary class that is used to pass a candidate move along with its estimated quality to the local search algorithms.
 * 
 * @tparam Move type of the move.
 */
template <typename Move> struct MoveData
{
    using MoveType = Move;

    MoveType move; // move that leads to a neighboring solution
    double quality_estimate; // estimate of the quality of the neighboring solution

    /**
     * @brief Constructs a new MoveData.
     * 
     * @param move move that leads to a neighboring solution.
     * @param quality_estimate estimate of the quality of the neighboring solution. 
     */
    MoveData(const MoveType& move, double quality_estimate) : move{move}, quality_estimate{quality_estimate} {};

    bool operator==(const MoveData& other) const
    {
        return move == other.move && quality_estimate == other.quality_estimate;
    };

    bool operator!=(const MoveData& other) const
    {
        return move != other.move || quality_estimate != other.quality_estimate;
    };

    bool operator<(const MoveData& other) const
    {
        return quality_estimate < other.quality_estimate;
    };

    bool operator>(const MoveData& other) const
    {
        return quality_estimate > other.quality_estimate;
    };

    bool operator<=(const MoveData& other) const
    {
        return quality_estimate <= other.quality_estimate;
    };

    bool operator>=(const MoveData& other) const
    {
        return quality_estimate >= other.quality_estimate;
    };
};

#endif /* MOVEDATA_HPP_ */
