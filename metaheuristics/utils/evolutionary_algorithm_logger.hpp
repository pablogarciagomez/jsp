/**
 * @file evolutionary_algorithm_logger.hpp
 * @author Pablo
 * @brief Logger for evolutionary algorithms.
 * @version 0.1
 * @date 13-02-2019
 * 
 * @copyright Copyright (c) 2018
 * 
 */
#ifndef EVOLUTIONARYALGORITHMLOGGER_HPP_
#define EVOLUTIONARYALGORITHMLOGGER_HPP_

#include <string>
#include <vector>

/**
 * @brief Logger to trace the execution of an evolutionary algorithm.
 * 
 * @tparam Solution type of the solutions to be logged.
 */
template <typename Solution> class EvolutionaryAlgorithmLogger
{
  public:
    using SolutionType = Solution;

    /**
     * @brief Structure that is used to log each step of the algorithm.
     * 
     */
    struct Log
    {
        double average_quality; // average quality of the solutions
        double maximum_quality; // maximum quality of the solutions
        std::string msg; // optional message

        /**
         * @brief Constructs a new Log.
         * 
         * @param average_quality average quality of the solutions.
         * @param maximum_quality maximum quality of the solutions.
         * @param msg optional message.
         */
        Log(double average_quality, double maximum_quality, std::string msg = "") :
            average_quality{average_quality},
            maximum_quality{maximum_quality},
            msg{msg} {};

        friend std::ostream& operator<<(std::ostream& os, const Log& log)
        {
            if (log.msg != "") {
                os << log.msg << std::endl;
            }
            return os << "Average quality = " << log.average_quality << std::endl << "Maximum quality = " << log.maximum_quality << std::endl;
        }
    };

  private:
    std::string id;
    std::vector<Log> history;
    std::optional<SolutionType> best_solution;
    bool active;

  public:
    /**
     * @brief Constructs a new EvolutionaryAlgorithmLogger.
     * 
     * @param id identifier of the logger.
     * @param active if true the logger will log the algorithm trace, if false the logger won't log the algorithm trace.
     */
    EvolutionaryAlgorithmLogger(std::string id, bool active = false) : id{id}, active{active} {};

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
     * @param average_quality average quality of the solutions.
     * @param maximum_quality maximum quality of the solutions.
     * @param msg optional message.
     */
    void AddLog(double average_quality, double maximum_quality, std::string msg = "")
    {
        history.emplace_back(average_quality, maximum_quality, msg);
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

    friend std::ostream& operator<<(std::ostream& os, const EvolutionaryAlgorithmLogger& logger)
    {
        if (logger.active) {
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

#endif /* EVOLUTIONARYALGORITHMLOGGER_HPP_ */