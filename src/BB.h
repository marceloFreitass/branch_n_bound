#pragma once
#include "gurobi_c++.h"
#include "Data.h"
#include <string>
#include <stack>
#include <math.h>

#define EPSILON 10e-6

typedef struct
{
    std::vector<int> zero;
    std::vector<int> one;
    
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

        int get_most_fractional();
        void set_bounds();
        void print_solution();

        //GRBconstr* constrs TODO
};
