例
`clang++ -c main.cpp; clang++ -c show_to_goal.cpp`

`clang++ -o push_maze show_to_goal.o main.o`

`./push_maze_happyou 11 > result.txt`

`bash map_show.sh result.txt`
