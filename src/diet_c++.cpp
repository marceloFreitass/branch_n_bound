/* Copyright 2025, Gurobi Optimization, LLC */

/* Solve the classic diet model, showing how to add constraints
   to an existing model. */

#include "gurobi_c++.h"
using namespace std;

void printSolution(GRBModel& model, int nCategories, int nFoods,
                   GRBVar* buy);

int main(int argc,
     char *argv[])
{
  GRBEnv* env = NULL;
  GRBVar* nutrition = NULL;
  GRBVar* buy = NULL;
  int numVars;
  try
  {

    const int nCategories = 4;
    string Categories[] =
      { "calories", "protein", "fat", "sodium" };
    double minNutrition[] = { 1800, 91, 0, 0 };
    double maxNutrition[] = { 2200, 1000, 65, 1779 };

    // Set of foods
    const int nFoods = 9;
    std::vector<string> x ={ "yakisoba", "famoso", "hot dog", "fries",
      "macaroni", "pizza", "salad", "milk", "ice cream" };
      // x.data()
    double cost[] =
      { 2.49, 2.89, 1.50, 1.89, 2.09, 1.99, 2.49, 0.89, 1.59 };

    // Nutrition values for the foods
    double nutritionValues[][nCategories] = {
                      { 410, 24, 26, 730 },    // hamburger
                      { 420, 32, 10, 1190 },   // chicken
                      { 560, 20, 32, 1800 },   // hot dog
                      { 380, 4, 19, 270 },     // fries
                      { 320, 12, 10, 930 },    // macaroni
                      { 320, 15, 12, 820 },    // pizza
                      { 320, 31, 12, 1230 },   // salad
                      { 100, 8, 2.5, 125 },    // milk
                      { 330, 8, 10, 180 }      // ice cream
                    };
    // Model
    env = new GRBEnv();
    // env.set(GRB.IntParam.Method, 1);
    GRBModel model = GRBModel(*env);
    model.set(GRB_StringAttr_ModelName, "diet");
    /* 0 - Primal Simplex
       1 - Dual simplex
    */
    model.set(GRB_IntParam_Method, 1);
    /*
    nao pode ser 0, se nao ele sempre descarta o Vbasis e Cbasis, provavelmente tem que usar LPWARMstart = 2
    para ele rodar o presolve mesmo com a base inicial sendo enviada
    */
    model.set(GRB_IntParam_LPWarmStart, 1);

    buy = model.addVars(0, 0, cost, 0, x.data(), nFoods);
    
    model.set(GRB_IntAttr_ModelSense, GRB_MAXIMIZE);

    // Nutrition constraints
    //modificado para tentar deixar o primero dual inviavel no dual simplex
    for (int i = 0; i < nCategories; ++i)
    {
      GRBLinExpr ntot = 0;
      for (int j = 0; j < nFoods; ++j)
      {
        ntot += nutritionValues[j][i] * buy[j];
      }
      // model.addConstr(ntot >= minNutrition[i], Categories[i]);
      model.addConstr(ntot <= maxNutrition[i], Categories[i]);
    }
    
    model.update(); //necessario na primeira vez para manipular as variaveis e restricoes antes de usar .optimize()
    GRBConstr* constrs = model.getConstrs();
    for(int i = 0; i < 9; i++)
    {
      if(i <= 6)
        buy[i].set(GRB_IntAttr_VBasis, -1);
      else
        buy[i].set(GRB_IntAttr_VBasis, 0);
    }
    constrs[0].set(GRB_IntAttr_CBasis, -1);
    constrs[1].set(GRB_IntAttr_CBasis, 0);
    constrs[2].set(GRB_IntAttr_CBasis, 0);
    constrs[3].set(GRB_IntAttr_CBasis, -1);
    
    // Solve
    model.optimize();
    std::cout << "VBasis: \n";
    for(int i = 0; i < 9; i++)
    {
      std::cout << buy[i].get(GRB_IntAttr_VBasis) << std::endl;
    }
    std::cout << "CBasis: \n";
    
    for(int i = 0; i < nCategories; i++)
    {
      std::cout << constrs[i].get(GRB_IntAttr_CBasis) << std::endl;
    }
    printSolution(model, nCategories, nFoods, buy);


    cout << "\nAdding constraint: at most 6 servings of dairy" << endl;


    model.optimize();

    
    printSolution(model, nCategories, nFoods, buy);

    buy[0].set(GRB_DoubleAttr_LB, 1);
    model.optimize();
    printSolution(model, nCategories, nFoods, buy);

    // model.addConstr(buy[7] <= 5);
    // buy[7].set(GRB_DoubleAttr_UB, 5);
    model.optimize();
    printSolution(model, nCategories, nFoods, buy);
    
    std::cout << "RESTRICOES: "  << model.get(GRB_IntAttr_NumConstrs) << std::endl;
    //setar LB/UB da variavel nao adiciona uma nova restricao.
    // std::cout << buy[7].get(GRB_DoubleAttr_RC) << std::endl;

  }
  catch (GRBException e)
  {
    cout << "Error code = " << e.getErrorCode() << endl;
    cout << e.getMessage() << endl;
  }
  catch (...)
  {
    cout << "Exception during optimization" << endl;
  }

  delete[] nutrition;
  delete[] buy;
  delete env;
  return 0;
}

void printSolution(GRBModel& model, int nCategories, int nFoods,
                   GRBVar* buy)
{
  if (model.get(GRB_IntAttr_Status) == GRB_OPTIMAL)
  {
    cout << "\nCost: " << model.get(GRB_DoubleAttr_ObjVal) << endl;
    cout << "\nBuy:" << endl;
    for (int j = 0; j < nFoods; ++j)
    {
      if (buy[j].get(GRB_DoubleAttr_X) > 0.0001)
      {
        std::cout << "J: " << j << std::endl;

        cout << buy[j].get(GRB_StringAttr_VarName) << " " <<
        buy[j].get(GRB_DoubleAttr_X) << endl;
      }
    }
    // cout << "\nNutrition:" << endl;
    // for (int i = 0; i < nCategories; ++i)
    // {
    //   cout << nutrition[i].get(GRB_StringAttr_VarName) << " " <<
    //   nutrition[i].get(GRB_DoubleAttr_X) << endl;
    // }
  }
  else
  {
    cout << "No solution" << endl;
  }
}
