#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <functional>
#include <iostream>
#include <limits>
#include <random>

#include <metaheuristics/evolutionary_algorithm/crossover_operators.hpp>
#include <metaheuristics/evolutionary_algorithm/evolutionary_algorithm.hpp>
#include <metaheuristics/evolutionary_algorithm/memetic_algorithm.hpp>
#include <metaheuristics/evolutionary_algorithm/mutation_operators.hpp>
#include <metaheuristics/evolutionary_algorithm/replacement_operators.hpp>
#include <metaheuristics/evolutionary_algorithm/selection_operators.hpp>
#include <metaheuristics/tabu_search/tabu_search_fixed_length.hpp>
#include <metaheuristics/tabu_search/tabu_search_variable_length.hpp>
#include <metaheuristics/utils/evolutionary_algorithm_logger.hpp>
#include <metaheuristics/utils/local_search_logger.hpp>
#include <problems/jsp/jsp_generation_operators.hpp>
#include <problems/jsp/jsp_job.hpp>
#include <problems/jsp/jsp_machine.hpp>
#include <problems/jsp/jsp_makespan_minimization_solution.hpp>
#include <problems/jsp/jsp_neighborhoods.hpp>
#include <problems/jsp/jsp_readers.hpp>
#include <problems/jsp/jsp_task.hpp>
#include <problems/jsp/jsp_total_weighted_tardiness_minimization_solution.hpp>

template <typename RealNumber> static bool AlmostEqual(RealNumber a, RealNumber b, RealNumber tolerance = std::numeric_limits<RealNumber>::epsilon())
{
    RealNumber d = std::fabs(a - b);
    if (d <= tolerance) {
        return true;
    }
    if (d < std::fmax(std::fabs(a), std::fabs(b)) * tolerance) {
        return true;
    }
    return false;
}

template <typename Problem, typename Solution>
std::tuple<Solution, EvolutionaryAlgorithmLogger<Solution>, LocalSearchLogger<Solution>> MemeticAlgorithm(Problem& problem)
{
    std::random_device rd{};
    std::mt19937 rng(rd());
    JSPRandomPopulationGenerator generation_operator{};
    GOX crossover_op{};
    PairSelection selection_op{};
    Swap mutation_op{};
    Tournament replacement_op{};
    PermutationWithRepetition<GT> encoder_decoder{};
    TabuSearchVariableLength local_search{};

    EvolutionaryAlgorithmLogger<Solution> evolutionary_logger(std::string("Evolutionary Algorithm"), true);
    LocalSearchLogger<Solution> local_logger(std::string("Local Search"), true);

    auto solution = MemeticAlgorithm::FindSolution(
        evolutionary_logger,
        local_logger,
        problem,
        encoder_decoder,
        generation_operator,
        problem.GetNumberOfJobs() * problem.GetNumberOfMachines(),
        selection_op,
        crossover_op,
        1,
        mutation_op,
        0.1,
        replacement_op,
        false,
        [&problem](auto, auto no_improving_generations, auto average_quality, auto best_solution_quality) {
            return no_improving_generations > problem.GetNumberOfJobs() || AlmostEqual(best_solution_quality, average_quality);
        },
        rng,
        local_search,
        1,
        problem.GetNumberOfJobs() + problem.GetNumberOfMachines(),
        2 * (problem.GetNumberOfJobs() + problem.GetNumberOfMachines()),
        [&problem](auto, auto no_improving_iterations) {
            return no_improving_iterations > 2 * problem.GetNumberOfJobs() + problem.GetNumberOfMachines();
        },
        CET<Problem>());

    return std::tie(solution, evolutionary_logger, local_logger);
}

int main(int argc, char** argv)
{
    if (argc < 3) {
        throw std::invalid_argument("Missing arguments");
    }

    using JobType = JSPJob<double>;
    using MachineType = JSPMachine;
    using TaskType = JSPTask<TriangularFuzzyNumber<double>, JobType, MachineType>;
    using ProblemType = JSP<TaskType, JobType, MachineType>;
    using SolutionType = JSPTotalWeightedTardinessMinimizationSolution<ProblemType>;

    std::ifstream instance(argv[1]);
    std::ofstream trace(argv[2]);
    auto problem = read_standard_due_dates<TaskType, JobType, MachineType>(instance);

    std::clock_t start = std::clock();
    auto [solution, evolutionary_logger, local_search_logger] = MemeticAlgorithm<ProblemType, SolutionType>(problem);
    std::clock_t end = std::clock();

    trace << "Execution Time = " << end - start << std::endl;
    trace << "Total Weighted Tardiness = " << solution.GetTotalWeightedTardiness() << std::endl;
    trace << "Expected Total Weighted Tardiness = " << solution.GetTotalWeightedTardiness().ExpectedValue() << std::endl;
    trace << "TRACE" << std::endl;
    trace << evolutionary_logger;

    return 0;
}
