/* Copyright 2025, Gurobi Optimization, LLC */

/* Solve the classic diet model, showing how to add constraints
   to an existing model. */

#include "gurobi_c++.h"
using namespace std;

void printSolution(GRBModel& model, int nCategories, int nFoods,
                   GRBVar* buy, GRBVar* nutrition);

int main(int argc,
     char *argv[])
{
  GRBEnv* env = NULL;
  GRBVar* nutrition = NULL;
  GRBVar* buy = NULL;
  try
  {

    // Nutrition guidelines, based on
    // USDA Dietary Guidelines for Americans, 2005
    // http://www.health.gov/DietaryGuidelines/dga2005/
    const int nCategories = 4;
    string Categories[] =
      { "calories", "protein", "fat", "sodium" };
    double minNutrition[] = { 1800, 91, 0, 0 };
    double maxNutrition[] = { 2200, GRB_INFINITY, 65, 1779 };

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
    model.set(GRB_IntParam_Method, 1);

    // Create decision variables for the nutrition information,
    // which we limit via bounds
    nutrition = model.addVars(minNutrition, maxNutrition, 0, 0,
                              Categories, nCategories);

    // Create decision variables for the foods to buy
    //
    // Note: For each decision variable we add the objective coefficient
    //       with the creation of the variable.
    buy = model.addVars(0, 0, cost, 0, x.data(), nFoods);

    // The objective is to minimize the costs
    //
    // Note: The objective coefficients are set during the creation of
    //       the decision variables above.
    model.set(GRB_IntAttr_ModelSense, GRB_MINIMIZE);
    // model.set()

    // Nutrition constraints
    for (int i = 0; i < nCategories; ++i)
    {
      GRBLinExpr ntot = 0;
      for (int j = 0; j < nFoods; ++j)
      {
        ntot += nutritionValues[j][i] * buy[j];
      }
      model.addConstr(ntot == nutrition[i], Categories[i]);
    }

    // Solve
    model.optimize();
    printSolution(model, nCategories, nFoods, buy, nutrition);

    cout << "\nAdding constraint: at most 6 servings of dairy" << endl;
    // model.addConstr(buy[7] + buy[8] <= 6.0, "limit_dairy");
    // model.addConstr(buy[7] <= 6);
    buy[7].set(GRB_DoubleAttr_UB, 6);
    

    // Solve
    model.optimize();
    printSolution(model, nCategories, nFoods, buy, nutrition);

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
                   GRBVar* buy, GRBVar* nutrition)
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
    cout << "\nNutrition:" << endl;
    for (int i = 0; i < nCategories; ++i)
    {
      cout << nutrition[i].get(GRB_StringAttr_VarName) << " " <<
      nutrition[i].get(GRB_DoubleAttr_X) << endl;
    }
  }
  else
  {
    cout << "No solution" << endl;
  }
}
