/**
 * @file tabu_search_variable_length.hpp
 * @author Pablo
 * @brief Tabu search with a variable length tabu list.
 * @version 0.1
 * @date 09-11-2018
 * 
 * @copyright Copyright (c) 2018
 * 
 */
#ifndef TABUSEARCHVARIABLELENGTH_HPP_
#define TABUSEARCHVARIABLELENGTH_HPP_

#include <metaheuristics/utils/local_search_logger.hpp>
#include <metaheuristics/utils/move_data.hpp>
#include <metaheuristics/utils/neighborhoods.hpp>
#include <metaheuristics/utils/tabu_list.hpp>

/**
 * @brief Provides static functions to do a tabu search with a variable length tabu list.
 * 
 */
class TabuSearchVariableLength
{
  public:
    /**
     * @brief Finds a solution to a problem using a tabu search metaheuristic with a variable length tabu list.
     * 
     * @tparam Solution type of the solution to be evaluated.
     * @tparam StoppingCriterion type of the stopping criterion to be used to terminate the algorithm.
     * @tparam Neighborhood type of the neighborhood to be used to find adjacent solutions.
     * @tparam Neighborhoods types of the additional neighborhoods to be used.
     * @param logger logger where a trace of the execution will be stored.
     * @param initial_solution solution from where the search will start. 
     * @param min minimum size of the tabu list (the minimum size to which the list can shrink).
     * @param max maximum size of the tabu list (the maximum size to which the list can grow).
     * @param stopping_criterion stopping criterion to be used to terminate the algorithm.
     * @param neighborhood neighborhood to be used to find adjacent solutions.
     * @param neighborhoods additional neighborhoods to be used to find adjacent solutions. 
     * @return the best solution found.
     */
    template <typename Solution, typename StoppingCriterion, typename Neighborhood, typename... Neighborhoods>
    static Solution FindSolution(LocalSearchLogger<Solution>& logger,
                                 const Solution& initial_solution,
                                 unsigned int min,
                                 unsigned int max,
                                 const StoppingCriterion& stopping_criterion,
                                 const Neighborhood& neighborhood,
                                 const Neighborhoods&... neighborhoods)
    {
        using SolutionType = Solution;
        using MoveType = typename Neighborhood::MoveType;

        if (min == 0) {
            throw std::invalid_argument("min cannot be zero");
        }
        if (min >= max) {
            throw std::invalid_argument("max must be greater than min");
        }

        if (logger) {
            logger.SetInitialSolution(initial_solution);
        }
        SolutionType current_solution = initial_solution; // the current solution
        SolutionType best_solution = current_solution; // the best found solution so far
        TabuList<MoveType> tabu_list(1); // the tabu list

        unsigned int iterations = 0; // number of iterations
        unsigned int no_improving_iterations = 0; // number of iterations without improving

        while (!stopping_criterion(iterations++, no_improving_iterations++)) {
            bool found_valid_neighbor = false;
            std::vector<MoveData<MoveType>> moves;
            GetNeighbors(std::inserter(moves, moves.begin()), current_solution, neighborhood, neighborhoods...);
            std::sort(moves.begin(), moves.end(), std::greater<MoveData<MoveType>>());
            unsigned int neighbors_evaluated = 0; // logging variable
            for (auto& move: moves) {
                neighbors_evaluated++;
                if (move.quality_estimate > best_solution.GetQuality()) { // aspiration criterion
                    SolutionType neighbor(current_solution);
                    neighbor.ApplyMove(move.move);
                    if (neighbor > best_solution) {
                        current_solution = neighbor;
                        best_solution = current_solution;
                        tabu_list.ChangeCapacity(1);
                        tabu_list.ForcePush(move.move.Invert());
                        found_valid_neighbor = true;
                        no_improving_iterations = 0;
                        break;
                    }
                }
                if (!tabu_list.Contains(move.move)) { // if the move is not tabu
                    // update the tabu list length
                    SolutionType neighbor(current_solution);
                    neighbor.ApplyMove(move.move);
                    if (neighbor > current_solution) {
                        if (tabu_list.Capacity() > min) {
                            tabu_list.ChangeCapacity(tabu_list.Capacity() - 1);
                        }
                    } else {
                        if (tabu_list.Capacity() < max) {
                            tabu_list.ChangeCapacity(tabu_list.Capacity() + 1);
                        }
                    }
                    // establish the neighbor as the current solution
                    current_solution = neighbor;
                    tabu_list.ForcePush(move.move.Invert());
                    found_valid_neighbor = true;
                    break;
                }
            }
            if (!found_valid_neighbor) { // if all moves are tabu choose the best neighbor
                if (moves.empty()) {
                    logger.AddLog(current_solution.GetQuality(), moves.size(), neighbors_evaluated, "No neighbors available");
                    break;
                }
                auto move = *moves.begin();
                current_solution.ApplyMove(move.move);
                tabu_list.ForcePush(move.move.Invert());
            }
            if (logger) {
                logger.AddLog(current_solution.GetQuality(), moves.size(), neighbors_evaluated, !found_valid_neighbor ? "All moves tabu" : "");
            }
        }
        if (logger) {
            logger.SetBestSolution(best_solution);
        }
        return best_solution;
    }
};

#endif /* TABUSEARCHVARIABLELENGTH_HPP_ */