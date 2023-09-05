import numpy as np
import math
import copy


class Population:

    def __init__(self):
        self.population = []
        self.fronts = []
        self.L_p = 1

    def __len__(self):
        return len(self.population)

    def __iter__(self):
        return self.population.__iter__()

    def extend(self, new_individuals):
        self.population.extend(new_individuals)

    def append(self, new_individual):
        self.population.append(new_individual)
    
    def get_geometry(self):
        # sum1 = 100
        # sum2 = 1
        # while(math.log10(sum1) / sum2 > 0.001):
        #     sum1 = 0
        #     sum2 = 0
        #     for i in range(len(self.population[0].objectives)):
        #         sum1 += pow(self.fronts[0][2].objectives[i], self.L_p)
        #         sum2 += pow(self.fronts[0][2].objectives[i], self.L_p) * math.log(self.fronts[0][2].objectives[i])
        #     self.L_p = self.L_p + (math.log(sum1) / sum2)
        #     print("L_p in while: ", self.L_p)
        self.L_p = 0.5

    def count_survival_score(self, d):
        if (d == 0):
            Extra = []
            Unxtra = copy.deepcopy(self)
            for i in range(len(self.population[0].objectives)):
                # max_ind = np.argmax(np.array(self.population), key = lambda ind: ind.objectives[i])
                population_array = np.array([ind.objectives[i] for ind in self.population])
                max_ind = np.argmax(population_array) # Find the index of the maximum value
                self.population[max_ind].objectives[i] = 10000000
                Extra.append(self.population[max_ind])
                Unxtra.population.pop(max_ind)
            for ind in Unxtra.population:
                ind.proximity = np.linalg.norm(np.array(ind.objectives), self.L_p)
            while (len(Unxtra.population) > 0):
                for ind in Unxtra.population:
                    diver = []
                    for ind2 in self.fronts[d]:
                        x = [np.abs(ind2.features[i] - ind.features[i]) for i in range(len(ind.features))]
                        
                    for i in range(len(self.fronts[d])):
                        diver.append(np.linalg.norm(np.array(x), self.L_p))
                    ind.diversity = min(diver)
                    ind.value = ind.diversity / ind.proximity

                Unxtra_array = np.array([ind.value for ind in Unxtra.population])
                s_max = np.argmax(Unxtra_array) # Find the index of the maximum value
                Unxtra.population[s_max].survival_score = Unxtra.population[s_max].value
                Extra.append(Unxtra.population[s_max])
                Unxtra.population.pop(s_max)
        else:
           for ind in self.fronts[d]:
               sum = np.linalg.norm(np.array(ind.objectives), self.L_p)
               ind.survival_score = 1 / sum
