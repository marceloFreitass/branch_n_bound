#pragma once
#include "gurobi_c++.h"
#include "Data.h"
#include <string>
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

        //GRBconstr* constrs TODO
};
