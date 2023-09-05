from utils import AGEUtils
from population import Population
from tqdm import tqdm


class Evolution:

    def __init__(self, problem, num_of_generations=1000, num_of_individuals=100, num_of_tour_particips=2,
                 tournament_prob=0.9, crossover_param=2, mutation_param=5):
        self.utils = AGEUtils(problem, num_of_individuals, num_of_tour_particips, tournament_prob, crossover_param,
                                mutation_param)
        self.population = None
        self.num_of_generations = num_of_generations
        self.on_generation_finished = []
        self.num_of_individuals = num_of_individuals

    # def evolve(self):
    #     self.population = self.utils.create_initial_population()
    #     self.utils.fast_nondominated_sort(self.population)
    #     for front in self.population.fronts:
    #         self.utils.calculate_crowding_distance(front)
    #     children = self.utils.create_children(self.population)
    #     returned_population = None
    #     for i in tqdm(range(self.num_of_generations)):
    #         self.population.extend(children)
    #         self.utils.fast_nondominated_sort(self.population)
    #         new_population = Population()
    #         front_num = 0
    #         while len(new_population) + len(self.population.fronts[front_num]) <= self.num_of_individuals:
    #             self.utils.calculate_crowding_distance(self.population.fronts[front_num])
    #             new_population.extend(self.population.fronts[front_num])
    #             front_num += 1
    #         self.utils.calculate_crowding_distance(self.population.fronts[front_num])
    #         self.population.fronts[front_num].sort(key=lambda individual: individual.crowding_distance, reverse=True)
    #         new_population.extend(self.population.fronts[front_num][0:self.num_of_individuals - len(new_population)])
    #         returned_population = self.population
    #         self.population = new_population
    #         self.utils.fast_nondominated_sort(self.population)
    #         for front in self.population.fronts:
    #             self.utils.calculate_crowding_distance(front)
    #         children = self.utils.create_children(self.population)
    #     return returned_population.fronts[0]
    
    def evolve(self):
        # Initialize population
        self.population = self.utils.create_initial_population()
        self.utils.fast_nondominated_sort(self.population)
        # for front in self.population.fronts:
        #     self.utils.calculate_crowding_distance(front)
        returned_population = None
        for i in tqdm(range(self.num_of_generations)):
            # Create offspring
            children = self.utils.create_children(self.population)
            self.population.population = children
            # Get_geometry
            print("L_p before: ", self.population.L_p)
            self.population.get_geometry()
            print("L_p after: ", self.population.L_p)
            # Sort_into_ranks
            self.utils.fast_nondominated_sort(self.population)
            # Selection
            returned_population = self.utils.AGESelection(self.population)
            
        self.population = returned_population
        self.utils.fast_nondominated_sort(self.population)  
        return returned_population.fronts[0]