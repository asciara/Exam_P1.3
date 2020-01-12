#include "bst.hpp"

#include <iostream>
#include <cstdlib>   // rand
#include <ctime>
#include <chrono>
#include <map>
    
int main(int argc, char ** argv)
{
    try
    {    
        std::srand(std::time(nullptr));
        //std::srand(3);

        if (argc != 2)
            throw std::runtime_error("Bad number of arguments");

        int n_nodes = std::atoi(argv[1]);

        bst<int,int> tree {};
        std::map<int, int> map{};

        for (int i = 0; i < n_nodes; ++i)
        {
            int j = std::rand();
            tree.insert({j,j});
            map.insert({j,j});
        }

        for (int i = 0; i < 11; i += 1)
        {
            auto t0 = std::chrono::high_resolution_clock::now();
            tree.find(i);
            auto t1 = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0);
            std::cout << elapsed.count() << std::endl;
        }

        std::cout << std::endl;

        tree.balance();

        for (int i = 0; i < 11; i += 1)
        {
            auto t0 = std::chrono::high_resolution_clock::now();
            tree.find(i);
            auto t1 = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0);
            std::cout << elapsed.count() << std::endl;
        }

        std::cout << std::endl;

        std::map<int,int>::iterator it;

        for (int i = 0; i < 11; i += 1)
        {
            auto t0 = std::chrono::high_resolution_clock::now();
            it = map.find(i);
            auto t1 = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0);
            //if (it != map.end())
            //    std::cout << "found" << std::endl;
            std::cout << elapsed.count() << std::endl;
        }

        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }
    catch (...)
    {
        std::cerr << "Unknown exception occurred" << '\n';
        return -1;
    }

}
