#include "BB.h"

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
        name += std::to_string(i + 1);
        variables.push_back(model.addVar(0.0, 1.0, data.cost(i), GRB_CONTINUOUS, name));
    }
    model.set(GRB_IntParam_OutputFlag, 0);
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
}

int BB::get_most_fractional(){
    int most = -1;
    double value = 0.5;

    for(size_t i = 0; i < n; i++){
        double v = variables[i].get(GRB_DoubleAttr_X) - 0.5;
        if (fabs(v) < value){
            most = i;
            value = fabs(v);
        }
    }

    return most;
}

void BB::print_solution(){
    for (size_t i = 0; i < n; i++){
        std::cout << "x_" << i << " " << variables[i].get(GRB_DoubleAttr_X) << std::endl;
    }
}

void BB::solve(){
    Node raiz;
    std::stack<Node> tree;

    tree.push(raiz);

    double LB = -1;

    int count = 0;
    while(!tree.empty()){
        std::cout << "TREE: " << tree.size() << std::endl;
        Node no = tree.top();
        tree.pop();
        count++;

        for(int i = 0; i < no.zero.size(); i++){
            variables[no.zero[i]].set(GRB_DoubleAttr_UB, 0);
        }

        for(int i = 0; i < no.one.size(); i++){
            variables[no.one[i]].set(GRB_DoubleAttr_LB, 1);
        }

        std::string nome = "teste";
        nome += std::to_string(count);
        nome += ".lp";

        model.optimize();
        // model.write(nome);
        // getchar();
        double cost;
        
        for(int i = 0; i < no.zero.size(); i++){
            variables[no.zero[i]].set(GRB_DoubleAttr_UB, 1);
        }

        for(int i = 0; i < no.one.size(); i++){
            variables[no.one[i]].set(GRB_DoubleAttr_LB, 0);
        }

        try{
            cost = model.get(GRB_DoubleAttr_ObjVal);
        } catch(GRBException e){
            std::cout << e.getMessage() << std::endl;
            continue;
        }
        
        // std::cout << "cost: " << cost << std::endl;
        // std::cout << "LB: " << LB << std::endl;
        // std::cout << "tree size: " << tree.size() << std::endl;

        if(cost - LB <= EPSILON)
            continue;

        int most_fractional = get_most_fractional();
        // std::cout << "mais fracionado: " << most_fractional << std::endl;

        // print_solution();
        //getchar();
        
        if(most_fractional == -1){
            // std::cout << "COUNT: " << count << std::endl;
            // getchar();
            LB = cost > LB ? cost : LB;
            std::cout << "LB: " << LB << std::endl;
            continue;
        }

        Node son = no;
        son.zero.push_back(most_fractional);
        tree.push(son);

        Node daugther = no;
        daugther.one.push_back(most_fractional);
        tree.push(daugther);
    }
    
    std::cout << "OPTIMAL: " << LB << std::endl;
}