#include "BB.h"
#include "gurobi_c++.h"

GRBModel createModel(GRBEnv& env)
{
    return GRBModel(env);
}
BB::BB(Data& data, GRBEnv& env) : model(createModel(env))
{


    model.set(GRB_IntParam_Method, 0); //TODO: mudar para dual simplex "GRB_IntParam_Method, 1"
    model.set(GRB_IntAttr_ModelSense, GRB_MAXIMIZE);
    model.set(GRB_StringAttr_ModelName, "bruno_bruck");
    n = data.n;
    m = data.m;
    variables.reserve(n);
    for(size_t i = 0; i < n; i++)
    {
        std::string name = "x";
        name += std::to_string(i);
        variables.push_back(model.addVar(0.0, 1.0, data.cost(i), GRB_CONTINUOUS, name));
    }
    for(size_t j = 0; j < m; j++)
    {
        GRBLinExpr constraint = 0;

        for(size_t i = 0; i < n; i++)
        {
            constraint += data.coefficient(i, j) * variables[i];
        }
        model.addConstr(constraint <= data.RHS(j));
    }

    model.write("teste.lp");
    // variables = model.addVars(0, 0, cost, 0, x.data(), nFoods);

    

}