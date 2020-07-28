#include "maze.hpp"

typedef std::vector<int> Vecint;
typedef std::vector<Vecint> VecVecint;

int main(int argc, char *argv[]){    
    int N,M;
    if (argc == 2){
        N = std::stoi(argv[1]);
        M = N;
        if(N%2==0)
            return 1;
    } 
    else if(argc == 3){
        //size Nのマップ
        N = std::stoi(argv[1]);
        M = std::stoi(argv[2]);
        if(N%2==0||M%2==0)
            return 1;
    }else{
        return 1;
    }
    //計測はcountの数だけ
    VecVecint maze(N, Vecint(M, 1));
    generate_maze(maze);
    show_maze(maze, {0,0});
    auto start = std::chrono::system_clock::now();
    search_maze_asta(maze);
    auto end = std::chrono::system_clock::now();
    auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "execution time : " << msec << " ms\n";

    return 0;
}