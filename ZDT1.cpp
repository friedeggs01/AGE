#include <bits/stdc++.h>
using namespace std;

#define INF 1e9
#define OBJ_CNT 2
#define N 10
#define ETA 5
#define POP_SIZE 100
#define MAX_GENERATION 500
#define CROSSOVER_RATE 0.9
#define MUTATION_RATE 0.1


double random() {
    return 1.0 * rand() / RAND_MAX;
}
double count_p_norm(double x[], double L_p) {
    double sum = 0;
    for(int i=0; i<OBJ_CNT; i++) {
        sum += pow(x[i], L_p);
    }
    return 1 / pow(sum, 1/L_p);
}

// Class biểu diễn một cá thể
class Individual {
public:
    int dim;                    // Chiều dài chuỗi gen của cá thể
    int num_objectives;         // Số hàm mục tiêu

    double x[N];  // Chuỗi gen biểu diễn đồ vật được chọn
    double fitness[OBJ_CNT];    // Giá trị hàm thích nghi
    double score;
    double proximity;
    double diversity;
    double value;
    int rank;
    Individual() {
        this->dim = N;
        this->num_objectives = OBJ_CNT;
        this->rank = 1;
    }

    Individual(const Individual &p) {
        this->dim = p.dim;
        this->num_objectives = p.num_objectives;
        for (int i=0; i<p.dim; i++)
            this->x[i] = p.x[i];
        for (int i=0; i<num_objectives; i++)
            this->fitness[i] = p.fitness[i];
        this->score = p.score;
        this->proximity = p.proximity;
        this->diversity = p.diversity;
        this->value = p.value;
        this->rank = p.rank;
    }

    void random_init() {
        for (int i=0; i<dim; i++)
            x[i] = random();
    }

    double f1() {
        return x[0];
    }

    double f2() {
        double g = 1.0;
        for (int i=1; i<dim; i++)
            g += 9.0/(dim-1) * x[i];
        return 1 - sqrt(f1() / g);
    }

    void evaluate() {
        for (int i=0; i<dim; i++)
            x[i] = max(0.0, min(x[i], 1.0));
        fitness[0] = f1();
        fitness[1] = f2();
    }

    bool dominate(Individual& other) { // for find min problem
        bool superior = false;
        for (int i=0; i<num_objectives; i++)
            if (this->fitness[i] > other.fitness[i])
                return false;
            else if (this->fitness[i] < other.fitness[i])
                superior = true;
        return superior;
    }

    static vector<Individual> crossover(Individual p1, Individual p2) {
        vector<Individual> children;
        Individual c1(p1);
        Individual c2(p2);

        for (int i=0; i<p1.dim; i++) {
            double u = random();
            double beta;
            if (u <= 0.5)
                beta = pow(2 * u, 1.0 / (ETA + 1.0));
            else
                beta = pow(1.0 / (2.0 - 2 * u), 1.0 / (ETA + 1.0));

            c1.x[i] = 0.5 * ((1.0 + beta) * p1.x[i] + (1.0 - beta) * p2.x[i]);
            c2.x[i] = 0.5 * ((1.0 - beta) * p1.x[i] + (1.0 + beta) * p2.x[i]);
        }

        children.push_back(c1);
        children.push_back(c2);
        return children;
    }

    void mutate() {
        for (int i=0; i<dim; i++) {
            double u = random();
            double sigma;
            if (u <= 0.5)
                sigma = pow(2 * u + (1 - 2 * u) * pow(1 - x[i], ETA + 1.0),
                            1.0 / (ETA + 1.0)) - 1.0;
            else
                sigma = 1.0 - pow(2 * (1 - u) + 2 * (u - 0.5) * pow(x[i], ETA + 1.0),
                                  1.0 / (ETA + 1.0));
            x[i] += sigma;
        }
    }
};

class AGESelection {
public:
    static bool is_pareto_point(Individual p, vector<Individual>& pop) {
        for (Individual& indiv: pop)
            if (indiv.dominate(p))
                return true;
    }
    bool compareByScore(const Individual& ind1, const Individual& ind2) {
        return ind1.score > ind2.score;
    }
    
    static void select(vector<Individual>& pop, int count, double L_p) {
        vector<Individual> new_pop;
        vector<Individual> F_d;
        int d = 1;
        while ((new_pop.size() + F_d.size()) < count) {
            for (Individual p: pop) {
                if (p.rank == d) {
                    F_d.push_back(p);
                }
            }
            count_survival_score(F_d, d, L_p);
            for (Individual p: F_d) {
                new_pop.push_back(p);
            }
            d++;
        }
        sort(F_d.begin(), F_d.end(), compareByScore);
        for (Individual p: F_d) {
            if (new_pop.size() < count) {
                new_pop.push_back(p);
            } else {
                break;
            }
        }
    }

    static void count_survival_score(vector<Individual> F_d, int d, double L_p) {
        if (d == 1) {
            vector<Individual> Extra;
            vector<Individual> Unxtra = F_d;
            vector<Individual> extreme_point(OBJ_CNT);
            for (Individual p: F_d) {
                for (Individual p1: F_d) {
                    for (int i = 0; i < OBJ_CNT; i++) {
                        if (p.fitness[i] > p1.fitness[i]) {
                            extreme_point[i] = p;
                        } else {
                            extreme_point[i] = p1;
                        }
                    }
                }
            }
            Extra.insert(Extra.end(), extreme_point.begin(), extreme_point.end());
            Unxtra.erase(remove(Unxtra.begin(), Unxtra.end(), extreme_point), Unxtra.end());

            for (Individual p: Unxtra) {
                p.proximity = count_p_norm(p.fitness, L_p);
            }
            while (!Unxtra.empty()) {
                for (Individual p: Unxtra) {
                    double diver[F_d.size()];
                    for(int i = 0; i < F_d.size(); i++) {
                        double x[F_d.size()];
                        diver[i] = count_p_norm(x, L_p);
                    }
                    p.diversity = *min_element(diver, diver + F_d.size());
                    p.value = p.diversity / p.proximity;
                }
                Individual Smax = *max_element(Unxtra.begin(), Unxtra.end(),
                                               [](const Individual& ind1, const Individual& ind2) 
                                                  {return ind1.value < ind2.value;});
                Smax.score = Smax.value;
                Extra.push_back(Smax);
                Unxtra.erase(remove(Unxtra.begin(), Unxtra.end(), Smax), Unxtra.end());
            }
        } else {
            for (Individual p: F_d) {
                double sum = 0;
                for (int i = 0; i < OBJ_CNT; i++) {
                    sum += pow(p.fitness[i], L_p);
                }
                p.score = 1 / (pow(sum, L_p));
            }
        }
    }
};

// Class biểu diễn một quần thể
class Population {
public:
    vector<Individual> population;
    double L_p = 1;
    void init() {
        for (int i=0; i<POP_SIZE; i++) {
            Individual p;
            p.random_init();
            p.evaluate();
            population.push_back(p);
        }
    }

    // Offspring
    void reproduction(int count) {
        vector<Individual> offspring;

        // crossover
        while (offspring.size() < count) {
            int i1 = rand() % POP_SIZE;
            int i2 = rand() % POP_SIZE;
            while (i2 == i1)
                i2 = rand() % POP_SIZE;

            Individual p1 = population[i1];
            Individual p2 = population[i2];
            if (random() < CROSSOVER_RATE) {
                vector<Individual> children = Individual::crossover(p1, p2);
                offspring.insert(offspring.end(), children.begin(), children.end());
            }
            else {
                offspring.push_back(Individual(p1));
                offspring.push_back(Individual(p2));
            }
        }
        while (offspring.size() > count)
            offspring.pop_back();

        // mutation
        for (Individual &indiv: offspring)
            if (random() < MUTATION_RATE)
                indiv.mutate();

        population.insert(population.end(), offspring.begin(), offspring.end());
    }

    void sortIndividualsIntoRanks() {
        const int populationSize = population.size();

        for (int i = 0; i < populationSize; ++i) {
            for (int j = 0; j < populationSize; ++j) {
                if (i == j) {
                    continue;  // Skip self-comparison
                }

                bool iDominated = false;

                // Check if i dominates j
                if (population[i].dominate(population[j])) {
                    iDominated = true;
                }

                if (iDominated) {
                    population[j].rank++;
                }
            }
        }
    }

    void get_geometry(){
        double sum1 = 10, sum2 = 1;
        while (log(sum1) / sum2 > 0.001) {
            sum1 = 0;
            sum2 = 0;
            for (int i = 0; i < OBJ_CNT; i++) {
                sum1 += pow(population[2].fitness[i], L_p);
                sum2 += pow(population[2].fitness[i], L_p) * log10f(population[2].fitness[i]);
            }
            L_p = L_p + log10f(sum1) / sum2;
        }
    }
    // Chọn lọc theo AGE
    void selection(int count) {
        AGESelection::select(population, count, L_p);
    }
};

int main() {
    srand ( time(NULL) );

    /*=== Thuật toán AGE-MOEA ===*/
    // Initialize population
    Population pop;
    pop.init();
    cout << "Generation 0" << endl;

    // Loops
    for (int t=1; t<=MAX_GENERATION; t++) {
        // Create offspring
        pop.reproduction(POP_SIZE);

        // Evaluate individual
        for (Individual &indiv: pop.population)
            indiv.evaluate();

        pop.get_geometry();

        pop.sortIndividualsIntoRanks();
    
        pop.selection(POP_SIZE);

        // Print solution each generation
        cout << "Generation " << t << endl;
    }

    cout << "===========" << endl;
    cout << "FINAL RESULT:" << endl;
    for (Individual p: pop.population)
        cout << p.fitness[0] << " " << p.fitness[1] << endl;
}
