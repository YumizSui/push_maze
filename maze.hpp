#include <iostream>
#include <queue>
#include <stack>
#include <vector>
#include <map>
#include <algorithm>
#include <chrono>
#include <random>

typedef std::vector<int> Vecint;
typedef std::vector<Vecint> VecVecint;
enum states
{
    PATH,
    WALL,
    START,
    GOAL
};
struct point
{
    int x;
    int y;
    point();
    point(int x, int y);
    point(const point &p);
};
struct position
{
    //座標
    point pos;
    //ゴール地点との距離
    int distance;
    //ステップ数
    int step;
    //step+distance
    int cost;
    position();
    position(const position &position);
    void setPosition(const point &pos, int step = 0);
    void setdistance();
    void setstep(int step);
};
struct maze_state{
    //プレイヤーの座標
    position psn;
    //その時点の迷路の状態
    VecVecint maze;
    //どの方向に動いたか
    point direction;
    //壁を動かしたか
    bool is_move_wall;
    maze_state();
    maze_state(position psn1, VecVecint maze1, point direction1, bool is_move_wall1);
    maze_state(const maze_state &s);
};
template <typename T>
void clear_any(T &q);
bool is_out_maze(const int N, const int M, const point &pnt);
int VecVecint_at(const VecVecint &maze, const point &pos);
void set_maze(VecVecint &maze, const point &point1, const int state);
char maze_char(int maze_val);
void show_maze(const VecVecint &maze, const point &now);
bool is_exist_same_state(const std::multimap<point, maze_state> &visited_maps, const VecVecint &maze, const point next);
void show_to_goal(VecVecint maze, const std::multimap<point, maze_state> &visited_maps);
void search_maze_asta(VecVecint maze, bool is_pruning= true);
void search_maze_bfs(VecVecint maze, bool is_pruning= true);
bool search_maze_dfs(VecVecint maze, int cutoff, int cutoff_cost, int *sum_step, int *max_state_size, std::chrono::system_clock::time_point start_time);
void search_maze_iidfs(VecVecint maze);
void search_maze_idasta(VecVecint maze);
point get_randam_point(std::vector<point> &points);
void set_maze(VecVecint &maze, const point &point1, const int state);
void shuffle(int *order);
void generate_maze(VecVecint &maze);
void set_start(int x, int y);
void set_goal(int x, int y);
void set_max_time(int x);