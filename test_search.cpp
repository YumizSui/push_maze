#include "maze.hpp"

int main(int argc, char *argv[]){    
    int N,M;
    int count = 1;
    //argv[0]:縦幅
    //argv[1]:横幅
    //argv[2]:テスト回数
    //argv[3]:最大実行時間(s)
    if (argc == 4){
        N = std::stoi(argv[1]);
        M = N;
        if(N%2==0)
            return 1;
        count= std::stoi(argv[2]);
        set_max_time(std::stoi(argv[3]));
    }else{
        return 1;
    }
    //計測はcountの数だけ
    for (int i = 0; i < count;++i){
        VecVecint maze(N, Vecint(M, 1));
        generate_maze(maze);

        std::chrono::system_clock::time_point start;
        std::chrono::system_clock::time_point end;
        int64_t msec;

        std::cout << i << ",BFS,"<< N <<",";
        start = std::chrono::system_clock::now();
        search_maze_bfs(maze);
        end = std::chrono::system_clock::now();
        msec = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << msec << " \n";

        std::cout << 1*count+i << ",IDDFS,"<< N <<",";
        start = std::chrono::system_clock::now();
        search_maze_iidfs(maze);
        end = std::chrono::system_clock::now();
        msec = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << msec << " \n";

        std::cout << 2*count+i << ",A*," << N << ",";
        start = std::chrono::system_clock::now();
        search_maze_asta(maze);
        end = std::chrono::system_clock::now();
        msec = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << msec << " \n";

        std::cout << 3*count+i << ",IDA*,"<< N <<",";
        start = std::chrono::system_clock::now();
        search_maze_idasta(maze);
        end = std::chrono::system_clock::now();
        msec = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << msec << " \n";

    }

    return 0;
}