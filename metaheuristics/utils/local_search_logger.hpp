/**
 * @file local_search_logger.hpp
 * @author Pablo
 * @brief Logger for local search algorithms.
 * @version 0.1
 * @date 09-11-2018
 * 
 * @copyright Copyright (c) 2018
 * 
 */
#ifndef LOCALSEARCHLOGGER_HPP_
#define LOCALSEARCHLOGGER_HPP_

#include <string>
#include <vector>

/**
 * @brief Logger to trace the evolution of a local search algorithm.
 * 
 * @tparam Solution type of the solutions to be logged.
 */
template <typename Solution> class LocalSearchLogger
{
  public:
    using SolutionType = Solution;

    /**
     * @brief Structure that is used to log each step of the algorithm.
     * 
     */
    struct Log
    {
        double quality; // quality of the selected solution
        unsigned int neighbors_generated; // neighbors generated in the iteration
        unsigned int neighbors_evaluated; // neighbors evaluated in the iteration
        std::string msg; // optional message

        /**
         * @brief Constructs a new Log.
         * 
         * @param quality quality of the selected solution.
         * @param neighbors_generated neighbors generated in the iteration.
         * @param neighbors_evaluated neighbors evaluated in the iteration.
         * @param msg optional message.
         */
        Log(double quality, unsigned int neighbors_generated, unsigned int neighbors_evaluated, std::string msg = "") :
            quality{quality},
            neighbors_generated{neighbors_generated},
            neighbors_evaluated{neighbors_evaluated},
            msg{msg} {};

        friend std::ostream& operator<<(std::ostream& os, const Log& log)
        {
            if (log.msg != "") {
                os << log.msg << std::endl;
            }
            return os << "Neighbors generated = " << log.neighbors_generated << std::endl
                      << "Neighbors evaluated = " << log.neighbors_evaluated << std::endl
                      << "Quality = " << log.quality << std::endl;
        }
    };

  private:
    std::string id;
    std::vector<Log> history;
    std::optional<SolutionType> initial_solution;
    std::optional<SolutionType> best_solution;
    bool active;

  public:
    /**
     * @brief Constructs a new LocalSearchLogger.
     * 
     * @param id identifier of the logger.
     * @param active if true the logger will log the algorithm trace, if false the logger won't log the algorithm trace.
     */
    LocalSearchLogger(std::string id, bool active = false) : id{id}, active{active} {};

    /**
     * @brief Sets the initial solution.
     * 
     * @param solution solution to be set as the starting solution.
     */
    void SetInitialSolution(const SolutionType& solution)
    {
        initial_solution = solution;
    }

    /**
     * @brief Returns the initial solution.
     * 
     * @return the initial solution.
     */
    SolutionType GetInitialSolution()
    {
        return initial_solution;
    }

    /**
     * @brief Sets the best solution found by the algorithm.
     * 
     * @param solution to set as the best solution found by the algorithm.
     */
    void SetBestSolution(const SolutionType& solution)
    {
        best_solution = solution;
    }

    /**
     * @brief Returns the best solution found by the algorithm.
     * 
     * @return the best solution found by the algorithm. 
     */
    SolutionType GetBestSolution()
    {
        return best_solution;
    }

    /**
     * @brief Adds a log.
     * 
     * @param quality quality of the selected solution.
     * @param neighbors_generated neighbors generated in the iteration.
     * @param neighbors_evaluated neighbors evaluated in the iteration.
     * @param msg optional message.
     */
    void AddLog(double quality, unsigned int neighbors_generated, unsigned int neighbors_evaluated, std::string msg = "")
    {
        history.emplace_back(quality, neighbors_generated, neighbors_evaluated, msg);
    }

    /**
     * @brief Inserts in a container all the logged iterations.
     * 
     * @tparam Iter type of the iterator to be used to insert the logs.
     * @param dest iterator to be used to insert the logs.
     * @return an iterator to the log past the last log inserted. 
     */
    template <typename Iter> Iter GetHistory(Iter dest)
    {
        return std::copy(history.begin(), history.end(), dest);
    }

    /**
     * @brief Returns if the logger is active or not.
     * 
     * @return true if the logger is active. 
     * @return false if the logger is inactive.
     */
    explicit operator bool() const
    {
        return active;
    }

    friend std::ostream& operator<<(std::ostream& os, const LocalSearchLogger& logger)
    {
        if (logger.active) {
            os << "Initial solution" << std::endl << (logger.initial_solution.has_value() ? logger.initial_solution->ToString() : "");
            std::size_t i = 0;
            for (const auto& log: logger.history) {
                os << "Iteration " << i++ << std::endl << log;
            }
            os << "Best solution found" << std::endl << (logger.best_solution.has_value() ? logger.best_solution->ToString() : "");
            return os;
        } else {
            return os;
        }
    }
};

#endif /* LOCALSEARCHLOGGER_HPP_ */