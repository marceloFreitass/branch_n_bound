#include "BB.h"

GRBModel createModel(GRBEnv& env)
{
    return GRBModel(env);
}
BB::BB(Data& data, GRBEnv& env) : model(createModel(env))
{
    
    model.set(GRB_IntParam_Method, 1); //dual simplex
    model.set(GRB_IntAttr_ModelSense, GRB_MAXIMIZE);
    model.set(GRB_StringAttr_ModelName, "bruno_bruck");
    model.set(GRB_IntParam_OutputFlag, 0);
    n = data.n;
    m = data.m;
    variables.reserve(n);
    //construção do modelo
    for(size_t i = 0; i < n; i++)
    {
        std::string name = "x";
        name += std::to_string(i + 1);
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

    //inicialização dos vetores de pseudocusto
    p_menos = std::vector<double>(n, -1);
    p_mais = std::vector<double>(n, -1);
    
    model.write("teste.lp");
}

//volta aos bounds originais
inline void BB::resetBounds(const Node& no)
{
    for(size_t i = 0; i < no.zero.size(); i++){
        variables[no.zero[i]].set(GRB_DoubleAttr_UB, 1);
    }

    for(size_t i = 0; i < no.one.size(); i++){
        variables[no.one[i]].set(GRB_DoubleAttr_LB, 0);
    }
}

std::pair<int,double> BB::strong_branching(double father_cost)
{
    double best_product = -1;
    int best_variable = -1;
    double best_f = 0;
    double cost;
    for(size_t i = 0; i < n; i++)
    {
        //se a variavel for fracionaria
        if(variables[i].get(GRB_DoubleAttr_X) > EPSILON && variables[i].get(GRB_DoubleAttr_X) < 1 - EPSILON)
        {
            double f = variables[i].get(GRB_DoubleAttr_X);
            //se nao tiver nenhuma estimativa, calculamos a estimativa (se colocarmos para 0)
            if(p_menos[i] == -1)
            {
                variables[i].set(GRB_DoubleAttr_UB, 0);
                model.optimize();
                variables[i].set(GRB_DoubleAttr_UB, 1);
                try{
                    cost = model.get(GRB_DoubleAttr_ObjVal);
                    p_menos[i] = (father_cost - cost)/f;
                } catch(GRBException e){
                }
                
            }
            //se nao tiver nenhuma estimativa, calculamos a estimativa (se colocarmos para 1)
            if(p_mais[i] == -1)
            {
                variables[i].set(GRB_DoubleAttr_LB, 1);
                model.optimize();
                variables[i].set(GRB_DoubleAttr_LB, 0);
                try{
                    cost = model.get(GRB_DoubleAttr_ObjVal);
                    p_mais[i] = (father_cost - cost)/(1 - f);
                } catch(GRBException e){
                }
            }

            //calculamos a estimativa da diminuição da F.O. (colocando a variavel em 0 ou em 1)
            double D_menos = p_menos[i] != -1 ? f * p_menos[i] : 0;
            double D_mais = p_mais[i] != -1 ? (1 - f) * p_mais[i] : 0;
            //pegando a variavel que apresenta o maior produto das diminuicoes
            if(D_menos * D_mais > best_product)
            {
                best_product = D_menos * D_mais;
                best_variable = i;
                best_f = f;
            }


        }
    }
    //retornando a variavel junto com sua parte fracionaria
    //se a solucao for inteira, a variavel é -1
    return {best_variable, best_f};

}

int BB::get_most_fractional(){
    int most = -1;
    double value = 0.5;

    for(size_t i = 0; i < n; i++){
        double v = variables[i].get(GRB_DoubleAttr_X) - 0.5;
        if (value - fabs(v) > EPSILON){
            most = i;
            value = fabs(v);
        }
    }

    return most;
}

void BB::print_solution(){
    for (size_t i = 0; i < n; i++){
        std::cout << variables[i].get(GRB_DoubleAttr_X) << " ";
    }
    std::cout << std::endl;
}

void BB::solve(){
    Node raiz = {{}, {}, -1, -1, -1 , 0}; //inicializao da raiz
    std::vector<bool> best_solution(n, false); //armazena a melhor solucao ate o momento
    std::stack<Node> tree;     //usando DFS
    tree.push(raiz);
    bool is_raiz = true;
    double LB = -1000000000;

    int count = 0; //contador de nos da arvore
    while(!tree.empty()){
        Node no = tree.top();
        tree.pop();
        
        //setando os bounds com respeito ao no
        for(size_t i = 0; i < no.zero.size(); i++){
            variables[no.zero[i]].set(GRB_DoubleAttr_UB, 0);
        }
        for(size_t i = 0; i < no.one.size(); i++){
            variables[no.one[i]].set(GRB_DoubleAttr_LB, 1);
        }
        //resolve o LP
        model.optimize();
        
        double cost;
        
        try{
            cost = model.get(GRB_DoubleAttr_ObjVal);
            if(!is_raiz)
            {
                //atualiza estimativa
                if(no.round)//se arredondou pra cima
                {
                    p_mais[no.branch_variable] = (no.father_cost - cost)/no.branch_variable_value;
                }
                else
                {
                    p_menos[no.branch_variable] = (no.father_cost - cost)/no.branch_variable_value;
                }
                
            }
        } catch(GRBException e){ //o erro acontece quando a solucao é inviavel (poda por inviabilidade)
            resetBounds(no);
            continue;
        }
        
        //supondo que os custos da F.O sao inteiros
        if(cost - LB <= 1 - EPSILON) // poda por limitante
        {
            resetBounds(no);
            continue;
        }

        
        std::pair<int, double> branch_variable_info = strong_branching(cost);
        resetBounds(no);


        if(branch_variable_info.first == -1){ //poda por integralidade
            if(cost > LB) //se a nova solucao inteira supera a anterior
            {
                LB = cost;
                //guardando nova solucao
                for(size_t i = 0; i < n; i++)
                {
                    if(variables[i].get(GRB_DoubleAttr_X) < EPSILON)
                    {
                        best_solution[i] = false;
                    }
                    else
                    {
                        best_solution[i] = true;
                    }
                }
                std::cout << "LB: " << LB << std::endl;
                std::cout << "Total de nós adicionados: " << count << std::endl;
                std::cout << "Tamanho: " << tree.size() << std::endl;
            }
            continue;
        }

        //filho em que a variavel vai para 0
        Node son = no;
        son.branch_variable = branch_variable_info.first;
        son.round = 0;
        son.father_cost = cost;
        son.branch_variable_value = branch_variable_info.second;
        son.zero.push_back(branch_variable_info.first);
        tree.push(son);
        //filho em que a variavel vai para 1
        Node daugther = no;
        daugther.branch_variable = branch_variable_info.first;
        daugther.round = 1;
        daugther.father_cost = cost;
        daugther.branch_variable_value = 1 - branch_variable_info.second;
        daugther.one.push_back(branch_variable_info.first);
        tree.push(daugther);

        count += 2;
        is_raiz = false;
    }
    //printando valor e solucao
    std::cout << "\n\n###### FIM ######" << std::endl;
    std::cout << "OPTIMAL: " << LB << std::endl;
    std::cout << "SOLUCAO: " << std::endl;
    for(size_t i = 0; i < n; i++)
    {
        std::cout << "x_" << i + 1 << ": " << best_solution[i] << std::endl;
    }

}