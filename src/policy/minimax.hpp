#pragma once
#include "../state/state.hpp"

class MiniMax{
public:
  static Move get_move(State *state, int depth);
};

typedef struct mini_max_node{
    int child_num;
    int heuristic;
    int score;
    int depth;
    Move pre_move;
    State* state;
    struct mini_max_node* parent;
    struct mini_max_node* child[1000];
}MiniMaxNode;

MiniMaxNode* build_tree(Move move, State* cur_state, int cur_depth, int total_depth, MiniMaxNode* parent_node);
MiniMaxNode* build_node(Move move, State* state, int num_of_child, int cur_depth);
int get_best_score(MiniMaxNode* root);
void delete_tree(MiniMaxNode* root);