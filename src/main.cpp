#include "Data.h"
#include "BB.h"
using namespace std;

int main(int argc, char *argv[])
{

    if(argc != 2)
    {
        std::cout << "Uso: ./program instancia.txt\n";
        exit(1);
    }

    GRBEnv env = GRBEnv(true);
    env.start();
    Data data = Data(argv[1]);
    data.print_cost();
    data.print_b();
    data.print_matrix();

    BB solver(data, env);
    solver.solve();
    
 
}
