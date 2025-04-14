#include "Data.h"

Data::Data(const std::string file_path)
{
    std::ifstream file(file_path);
    if(file.fail())
    {
        std::cout << "Arquivo nao existente\n";
        exit(1);
    }

    file >> n >> m;

    c = std::vector<double>(n);
    b = std::vector<double>(m);
    A = std::vector<std::vector<double>>(m, std::vector<double>(n));
    
    for(size_t i = 0; i < n ; i++)
    { 
        file >> c[i];
    }
    for(size_t j = 0; j < m; j++)
    {
        for(size_t i = 0; i < n; i++)
        {
            file >> A[j][i];
        }
        file >> b[j];
    }
}

void Data::print_cost()
{
    std::cout << "c: ";
    for(size_t i = 0; i < n; i++)
    {
        std::cout << c[i] << " ";
    }
    std::cout << std::endl;
}
void Data::print_b()
{
    std::cout << "b: ";
    for(size_t j = 0; j < m; j++)
    {
        std::cout << b[j] << " ";
    }
    std::cout << std::endl;
}

void Data::print_matrix()
{
    std::cout << "A: \n";
    for(size_t j = 0; j < m; j++)
    {
        for(size_t i = 0; i < n; i++)
        {
            std::cout << A[j][i] << " ";
        }
        std::cout << std::endl;
    }
}
