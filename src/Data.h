#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
class Data
{
    public:
        Data(const std::string file_path);
        size_t n;
        size_t m;

        inline double cost(const int i){return c[i];}
        inline double RHS(const int i){return b[i];}
        inline double coefficient(const int variable, const int constraint){return A[constraint][variable];}

        void print_cost();
        void print_b();
        void print_matrix();

        std::vector<double> c;
    private:

        
        std::vector<double> b;
        std::vector<std::vector<double>> A;




};
