/**
 * @file tabu_search_fixed_length.hpp
 * @author Pablo
 * @brief Tabu search with a fixed length tabu list.
 * @version 0.1
 * @date 09-11-2018
 * 
 * @copyright Copyright (c) 2018
 * 
 */
#ifndef TABUSEARCHFIXEDLENGTH_HPP_
#define TABUSEARCHFIXEDLENGTH_HPP_

#include <metaheuristics/utils/local_search_logger.hpp>
#include <metaheuristics/utils/move_data.hpp>
#include <metaheuristics/utils/neighborhoods.hpp>
#include <metaheuristics/utils/tabu_list.hpp>

/**
 * @brief Provides static functions to do a tabu search with a fixed length tabu list.
 * 
 */
class TabuSearchFixedLength
{
  public:
    /**
     * @brief Finds a solution to a problem using a tabu search metaheuristic with a fixed length tabu list.
     * 
     * @tparam Solution type of the solution to be evaluated.
     * @tparam StoppingCriterion type of the stopping criterion to be used to terminate the algorithm.
     * @tparam Neighborhood type of the neighborhood to be used to find adjacent solutions.
     * @tparam Neighborhoods types of the additional neighborhoods to be used.
     * @param logger logger where a trace of the execution will be stored.
     * @param initial_solution solution from where the search will start. 
     * @param tabu_list_size maximum size of the tabu list (the maximum number of moves to be remembered).
     * @param stopping_criterion stopping criterion to be used to terminate the algorithm.
     * @param neighborhood neighborhood to be used to find adjacent solutions.
     * @param neighborhoods additional neighborhoods to be used to find adjacent solutions. 
     * @return the best solution found.
     */
    template <typename Solution, typename StoppingCriterion, typename Neighborhood, typename... Neighborhoods>
    static Solution FindSolution(LocalSearchLogger<Solution>& logger,
                                 const Solution& initial_solution,
                                 unsigned int tabu_list_size,
                                 const StoppingCriterion& stopping_criterion,
                                 const Neighborhood& neighborhood,
                                 const Neighborhoods&... neighborhoods)
    {
        using SolutionType = Solution;
        using MoveType = typename Neighborhood::MoveType;

        if (logger) {
            logger.SetInitialSolution(initial_solution);
        }
        SolutionType current_solution = initial_solution; // the current solution
        SolutionType best_solution = current_solution; // the best found solution so far
        TabuList<MoveType> tabu_list(tabu_list_size); // the tabu list

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
                        tabu_list.ForcePush(move.move.Invert());
                        no_improving_iterations = 0;
                        found_valid_neighbor = true;
                        break;
                    }
                }
                if (!tabu_list.Contains(move.move)) { // if the move is not tabu
                    // establish the neighbor as the current solution and update the tabu list
                    current_solution.ApplyMove(move.move);
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

#endif /* TABUSEARCHFIXEDLENGTH_HPP_ */
