#include <iostream>
#include <queue>
#include <stack>
#include <vector>
#include <map>
#include <algorithm>
#include <chrono>
#include <random>
#include "maze.hpp"


typedef std::vector<int> Vecint;
typedef std::vector<Vecint> VecVecint;

std::random_device seed_gen;
std::mt19937 engine(seed_gen());

int MAX_TIME = 10;

point::point(){
    this->x = 0;
    this->y = 0;
}
point::point(int x, int y)
{
    this->x = x;
    this->y = y;
}
point::point(const point& p){
    this->x = p.x;
    this->y = p.y;
}
bool operator== (const point &point1, const point &point2)
{
    return point1.x == point2.x && point1.y == point2.y;
}
bool operator!= (const point &point1, const point &point2)
{
    return point1.x != point2.x && point1.y != point2.y;
}

bool operator< (const point &point1, const point &point2)
{
    return point1.x < point2.x || (point1.x == point2.x && point1.y < point2.y);
}

point operator+ (const point &point1, const point &point2)
{
    return { point1.x + point2.x , point1.y + point2.y};
}
point operator- (const point &point1, const point &point2)
{
    return { point1.x - point2.x , point1.y - point2.y};
}
point operator* (const point &point1, const int c)
{
    return { point1.x * c , point1.y * c};
}

//上下左右のベクトル
const point directions[4] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
//スタート地点
point start;
//ゴール地点
point goal;

position::position()
{
    this->distance = -1;
    this->step = 0;
    this->cost = -1;
}
position::position(const position& position){
    this->pos = position.pos;
    this->distance = position.distance;
    this->step = position.step;
    this->cost = position.cost;
}
void position::setPosition(const point& pos,  int step){
    this->pos = pos;
    setdistance();
    setstep(step);
}
void position::setdistance(){
    this->distance = abs(this->pos.x  - goal.x )+abs(this->pos.y  - goal.y );
    this->cost = this->step + this->distance;
}
void position::setstep(int step){
    this->step = step;
    this->cost = this->step + this->distance;
}

bool operator<(const position &pos1, const position &pos2)
{
    return pos1.cost < pos2.cost;
}
bool operator>(const position &pos1, const position &pos2)
{
    return pos1.cost > pos2.cost;
}

//迷路の状態を保存する，最短路の出力にも使う
maze_state::maze_state(){
    this->psn = position();
    this->maze = VecVecint();
    this->direction = point();
    this->is_move_wall = false;
}
maze_state::maze_state(position psn1, VecVecint maze1, point direction1, bool is_move_wall1){
    this->psn = psn1;
    this->maze = maze1;
    this->direction = direction1;
    this->is_move_wall = is_move_wall1;
}
maze_state::maze_state(const maze_state& s){
    this->psn = s.psn;
    this->maze = s.maze;
    this->direction = s.direction;
    this->is_move_wall = s.is_move_wall;
}

bool operator< (const maze_state &maze_state1, const maze_state &maze_state2)
{
    return maze_state1.psn < maze_state2.psn;
}

bool operator> (const maze_state &maze_state1, const maze_state &maze_state2)
{
    return maze_state1.psn > maze_state2.psn;
}

//コンテナの要素を空にする
template <typename T> void clear_any(T& q){
    T empty;
    swap(q, empty);
}

//迷路の外側の座標を指しているか
bool is_out_maze(const int N, const int M, const point& pnt){
    return pnt.x<0 || pnt.y < 0 || pnt.x> N - 1 || pnt.y > M - 1;
}

//その地点の迷路の状態
int VecVecint_at(const VecVecint& maze,const point& pos){
    return maze.at(pos.x).at(pos.y);
}

//迷路の座標に状態をセットする
void set_maze(VecVecint& maze,const point& point1,const int state){
    maze.at(point1.x).at(point1.y) = state;
}

//enum statesから文字への変換
char maze_char(int maze_val){
    char c;
    switch (maze_val)
    {
    case PATH:
        c = ' ';
        break;
    case WALL:
        c = '#';
        break;
    case START:
        c = 'S';
        break;
    case GOAL:
        c = 'G';
        break;
    }
    return c;
}

//迷路を文字形式で出力
void show_maze(const VecVecint& maze, const point& now){
    for (int i = 0; i < maze.size(); i++){
        for (int j = 0; j < maze[i].size(); j++){
            if(now.x!=i || now.y !=j){
                int c = VecVecint_at(maze, {i, j});
                printf("%c", maze_char(c));
            }
            else{
                printf("%c", 'o');
            }
        }
        printf("\n");
    }
    printf("\n");
}

//同じ迷路状態か
bool is_exist_same_state(const std::multimap<point, maze_state>& visited_maps,const VecVecint& maze,const point next){

    auto eqr = visited_maps.equal_range(next);

    for(auto itr = eqr.first; itr != eqr.second; ++itr)
    {
        if(itr->second.maze == maze ){
            return true;
        }
    }
    return false;
}

//スタートからゴールまでの行動をshow_mazeする．
void show_to_goal(VecVecint maze, const std::multimap<point, maze_state>& visited_maps){
    //ゴールの状態からスタックしていく
    std::stack<maze_state> st;
    point current_point = goal;

    auto eqr = visited_maps.equal_range(current_point);
    auto itr = eqr.first;
    st.push(itr->second);
    
    maze_state current_state = itr->second;

    //スタート状態になるまでwhile
    while(current_state.psn.step!=0){
        point current_point = current_state.psn.pos;
        int current_step = current_state.psn.step;
        point direction = current_state.direction;
        point next = current_point - direction;
        VecVecint next_maze = current_state.maze;

        //壁を動かした場合は元の位置に戻す
        if (current_state.is_move_wall)
        {
            point front = current_point + direction;
            set_maze(next_maze, current_point, WALL);
            set_maze(next_maze, front, PATH);
        }

        //現在の座標をkeyにして同じ状態を探して，見つかったらスタックにプッシュする．
        auto eqr = visited_maps.equal_range(next);
        for(auto itr = eqr.first; itr != eqr.second; ++itr)
        {
            if(itr->second.psn.step == current_step-1 && itr->second.maze == next_maze){
                current_state = itr->second;
                st.push(itr->second);
            }
        }
    }
    //経路の出力
    while(!st.empty()){
        maze_state current_state = st.top();
        st.pop();
        show_maze(current_state.maze, current_state.psn.pos);
    }
}

//A*アルゴリズム
void search_maze_asta(VecVecint maze, bool is_pruning){
    //打ち切り時間計測用
    auto start_time = std::chrono::system_clock::now();
    bool find = false;
    const static int N = maze.size();
    const static int M = maze[0].size();
    int step_cnt = 0;
    position current;
    current.setPosition(start, 0);

    //探索候補を優先度付きキューに保存
    std::priority_queue<maze_state, std::vector<maze_state>, std::greater<maze_state>> que;

    ++step_cnt;
    maze_state mazestate(current, maze, point(0, 0), false);
    que.push(mazestate);

    //探索候補が空になるまでwhile
    while (!que.empty())
    {
        //queから探索する状態を取り出す．
        maze_state current_state = que.top();
        que.pop();
        maze = current_state.maze;
        current = current_state.psn;
        //4方向に進む
        for (int i = 0; i < 4; i++)
        {
            point direction = directions[i];
            position next;
            next.setPosition(current.pos +direction, current.step+1);
            int state = VecVecint_at(maze, next.pos);

            //移動先が通路なら進んで状態を保存
            if(state==PATH){
                ++step_cnt;
                maze_state mazestate(next, maze, direction, false);
                que.push(mazestate);
            }

            //移動先が壁
            if(state==WALL){
                point front = next.pos + direction;
                //壁の先が範囲外なら押せない
                if(is_out_maze(N, M, front))
                    continue;

                //壁の先が通路なら押す
                if(VecVecint_at(maze,front)==PATH){
                    ++step_cnt;
                    VecVecint new_maze=maze;
                    
                    set_maze(new_maze, next.pos, PATH);
                    set_maze(new_maze, front, WALL);
                    maze_state mazestate(next, new_maze, direction, true);
                    que.push(mazestate);
                }
            }
            
            //移動先がゴール
            if(state==GOAL){
                ++step_cnt;
                find = true;
                maze_state mazestate(next, maze, direction, false);
                que.push(mazestate);
                //next.pos.x: ゴール座標
                //next.step: 最短路のステップ
                //step_cnt: 探索した数
                //std::cout<< "where: " << next.pos.x<<" "<<next.pos.y <<"\n";
                std::cout<< next.step <<",";
                std::cout<< step_cnt <<",";
                std::cout << 0 <<",";
                clear_any(que);
                break;
            }
        }
        auto now_time = std::chrono::system_clock::now();
        auto sec_time = std::chrono::duration_cast<std::chrono::seconds>(now_time - start_time).count();
        //MAX_TIME秒以上で強制打ち切り
        if(sec_time>MAX_TIME){
            clear_any(que);
        }
    }
    //見つからなかった場合の出力
    if(!find){
        std::cout<< "-1,";
        std::cout<< step_cnt <<",";
        std::cout << 0 <<",";
    }
}

void search_maze_bfs(VecVecint maze, bool is_pruning){
    //打ち切り時間計測用
    auto start_time = std::chrono::system_clock::now();
    bool find = false;
    const static int N = maze.size();
    const static int M = maze[0].size();
    int step_cnt = 0;
    position current;
    current.setPosition(start, 0);
    
    //探索候補をキューに保存
    std::queue<maze_state> que;
        
    ++step_cnt;
    maze_state mazestate(current, maze, point(0,0), false);
    que.push(mazestate);

    //探索候補が空になるまでwhile
    while (!que.empty())
    {
        //queから探索する状態を取り出す．
        maze_state current_state = que.front();
        que.pop();
        maze = current_state.maze;
        current = current_state.psn;
        //4方向に進む
        for (int i = 0; i < 4; i++)
        {
            point direction = directions[i];
            position next;
            next.setPosition(current.pos +direction, current.step+1);
            //同じ状態はスキップ

            int state = VecVecint_at(maze, next.pos);

            //移動先が通路なら進んで状態を保存
            if(state==PATH){
                ++step_cnt;
                maze_state mazestate(next, maze, direction, false);
                que.push(mazestate);
            }

            //移動先が壁
            if(state==WALL){
                point front = next.pos + direction;
                //壁の先が範囲外なら押せない
                if(is_out_maze(N, M, front))
                    continue;

                //壁の先が通路なら押す
                if(VecVecint_at(maze,front)==PATH){
                    ++step_cnt;
                    VecVecint new_maze=maze;
                    
                    set_maze(new_maze, next.pos, PATH);
                    set_maze(new_maze, front, WALL);
                    maze_state mazestate(next, new_maze, direction, true);
                    que.push(mazestate);
                }
            }
            //移動先がゴール
            if(state==GOAL){
                ++step_cnt;
                find = true;
                maze_state mazestate(next, maze, direction, false);
                que.push(mazestate);
                
                //next.pos.x: ゴール座標
                //next.step: 最短路のステップ
                //step_cnt: 探索した数
                //visited_maps.size(): メモリに保存した状態の数
                //std::cout<< "where: " << next.pos.x<<" "<<next.pos.y <<"\n";
                std::cout<< next.step <<",";
                std::cout<< step_cnt <<",";
                std::cout << 0 <<",";
                clear_any(que);
                break;
            }
        }
        auto now_time = std::chrono::system_clock::now();
        auto sec_time = std::chrono::duration_cast<std::chrono::seconds>(now_time - start_time).count();
        //MAX_TIME秒以上で強制打ち切り
        if(sec_time>MAX_TIME){
            clear_any(que);
        }
    }
    if(!find){
        std::cout<< "-1,";
        std::cout<< step_cnt <<",";
        std::cout << 0 <<",";
    }
}

//cutoff: IIDFS用
//cutoff_cost: IDA*用
//sum_step: ステップ数を加算
//max_state_size:メモリに保存した状態の最大数
bool search_maze_dfs(VecVecint maze,int cutoff,int cutoff_cost,int* sum_step, int *max_state_size, std::chrono::system_clock::time_point start_time){
    
    bool find = false;
    const static int N = maze.size();
    const static int M = maze[0].size();
    int step_cnt = 0;
    position current;
    current.setPosition(start, 0);
    
    //全ての探索した状態を保存する．
    //探索候補をスタックに保存
    std::stack<maze_state> st;

    ++step_cnt;
    maze_state mazestate(current, maze, point(0, 0), false);
    st.push(mazestate);

    //探索候補が空になるまでwhile
    while (!st.empty())
    {
        //stackから探索する状態を取り出す．
        maze_state current_state = st.top();
        st.pop();
        maze = current_state.maze;
        current = current_state.psn;
        //4方向に進む
        for (int i = 0; i < 4; i++)
        {
            point direction = directions[i];
            position next;
            next.setPosition(current.pos +direction, current.step+1);
            //同じ状態はスキップ
            if(cutoff > 0 && cutoff <= next.step)
                break;
            if(cutoff_cost > 0 && cutoff_cost <= next.cost)
                break;
                

            int state = VecVecint_at(maze, next.pos);

            //移動先が通路なら進んで状態を保存
            if(state==PATH){
                ++step_cnt;
                maze_state mazestate(next, maze, direction, false);
                st.push(mazestate);
            }

            //移動先が壁
            if(state==WALL){
                point front = next.pos + direction;
                //壁の先が範囲外なら押せない
                if(is_out_maze(N, M, front))
                    continue;

                //壁の先が通路なら押す
                if(VecVecint_at(maze,front)==PATH){
                    ++step_cnt;
                    VecVecint new_maze=maze;
                    
                    set_maze(new_maze, next.pos, PATH);
                    set_maze(new_maze, front, WALL);
                    maze_state mazestate(next, new_maze, direction, true);
                    st.push(mazestate);
                }
            }

            //移動先がゴール
            if(state==GOAL){
                ++step_cnt;
                find = true;
                maze_state mazestate(next, maze, direction, false);
                st.push(mazestate);
                //next.pos.x: ゴール座標
                //next.step: 最短路のステップ
                //step_cnt: 探索した数
                //visited_maps.size(): メモリに保存した状態の数
                //std::cout<< "where: " << next.pos.x<<" "<<next.pos.y <<"\n";
                std::cout<< next.step <<",";
                //show_maze(maze, next.pos);
                //show_to_goal(maze,visited_maps);
                clear_any(st);
                break;
            }
        }
        auto now_time = std::chrono::system_clock::now();
        auto sec_time = std::chrono::duration_cast<std::chrono::seconds>(now_time - start_time).count();
        //MAX_TIME秒以上で強制打ち切り
        if((int)sec_time>MAX_TIME){
            clear_any(st);
        }
    }

    //総ステップ数を加算，保存した状態数を更新
    *sum_step+=step_cnt;

    return find;
}

void search_maze_iidfs(VecVecint maze){
    //打ち切り時間計測用
    auto start_time = std::chrono::system_clock::now();
    //cutoff
    int cutoff = 1;
    bool find = false;
    int sum_step =0;
    int max_state_size = 0;
    //見つかるまで
    while (!find){
        find = search_maze_dfs(maze,cutoff,-1, &sum_step, &max_state_size, start_time);
        //少しずつ深くする
        ++cutoff;
        //MAX_TIME秒以上で強制打ち切り
        auto now_time = std::chrono::system_clock::now();
        auto sec_time = std::chrono::duration_cast<std::chrono::seconds>(now_time - start_time).count();
        if(sec_time>MAX_TIME){
            break;
        }
    }
    //見つからなかった場合の出力
    if(!find){
        std::cout<< "-1,";
        std::cout<< sum_step <<",";
        std::cout << 0 <<",";
    }else{                
        //sum_step: 探索した数
        //max_state_size: メモリに保存した状態の数
        std::cout << sum_step  << ",";
        std::cout << 0 << ",";
    }
}

void search_maze_idasta(VecVecint maze){
    //打ち切り時間計測用
    auto start_time = std::chrono::system_clock::now();
    //costについてのcutoff
    int cutoff_cost = 1;
    bool find = false;
    int sum_step =0;
    int max_state_size = 0;
    while(!find){
        find = search_maze_dfs(maze,-1,cutoff_cost, &sum_step, &max_state_size, start_time);
        //少しずつ深くする
        ++cutoff_cost;
        //MAX_TIME秒以上で強制打ち切り
        auto now_time = std::chrono::system_clock::now();
        auto sec_time = std::chrono::duration_cast<std::chrono::seconds>(now_time - start_time).count();
        if(sec_time>MAX_TIME){
            break;
        }
    }
    //見つからなかった場合の出力
    if(!find){
        std::cout<< "-1,";
        std::cout << sum_step  << ",";
        std::cout << max_state_size << ",";
    }else{
        //sum_step: 探索した数
        //max_state_size: メモリに保存した状態の数
        std::cout << sum_step  << ",";
        std::cout << max_state_size << ",";
    }
}

//座標集合から座標をランダムに所得する．取り出した座標は消す
point get_randam_point(std::vector<point>& points){
    
    int idx = engine()%points.size();
    point point_ = points[idx];
    points.erase(points.begin() + idx);
    return point_;
}

//順列をシャッフルする
void shuffle(int* order){

    for (int i = 0; i < 4; i++){
        int j = engine()%4;
        int tmp = order[i];
        order[i] = order[j];
        order[j] = tmp;
    }
}

//穴掘り法で迷路生成
void generate_maze(VecVecint& maze){
    const int N = maze.size();
    const int M = maze.at(0).size();
    const int width =N / 2;
    const int height = M / 2;
    int order[4] = {0, 1, 2, 3};

    //初期座標の集合を設定
    std::vector<point> initial_points(width*height);
    for (int i = 0; i < width;++i){
        for (int j = 0; j < height;++j){
            initial_points[width*i+j] = point(2*i+1, 2*j+1);
        }
    }

    //ランダムに取り出して掘る．
    point current = get_randam_point(initial_points);
    set_maze(maze, current, PATH);
    //ここをスタートにする
    start = current;

    //掘る場所がなくなったら終了
    while (initial_points.size() != 0)
    {
        //シャッフル
        shuffle(order);
        for (int i = 0; i < 5; ++i)
        {
            if(i==4){
                current = get_randam_point(initial_points);
                break;
            }
            //シャッフルした方向に進む
            point direction = directions[order[i]];
            point front = current + direction * 2;
            //範囲外なら掘らない
            if (is_out_maze(N, N, front))
                continue;
            //その方向が掘られてなければ掘る
            if(VecVecint_at(maze,front) != PATH){
                set_maze(maze, current+ direction, PATH);
                set_maze(maze, front, PATH);
                current = front;
                //暫定的なゴールを設定
                goal = front;
                break;
            }
        }
    }
    //start,goalを設定
    set_maze(maze, start, START);
    set_maze(maze, goal, GOAL);
}
void set_start(int x, int y){
    start = point(x, y);
}
void set_goal(int x, int y){
    goal = point(x, y);
}
void set_max_time(int x){
    MAX_TIME = x;
}