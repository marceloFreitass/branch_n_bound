#pragma once
#include "gurobi_c++.h"
#include "Data.h"
#include <string>
#include <stack>
#include <math.h>
#include <iomanip>
#define EPSILON 10e-6

typedef struct
{
    std::vector<int> zero;
    std::vector<int> one;
    double father_cost;
    double branch_variable_value;
    int branch_variable;
    bool round; //se round = 0, arrendodou para baixo, 1 c.c.
}Node;

class BB
{
    public:

    BB(Data& data, GRBEnv& env);
    void solve();

    private:
        std::vector<GRBVar> variables;
        GRBModel model;
        size_t n, m;

        std::vector<double> p_menos;
        std::vector<double> p_mais;
        int get_most_fractional();
        void set_bounds();
        void print_solution();
        //retorna a variavel e o valor fracionario dela (util para atualizar os vetores p)
        std::pair<int,double> strong_branching(double father_cost);
        void resetBounds(const Node& no);

};
