#pragma once
#include "../state/state.hpp"

class AlphaBeta{
public:
  static Move get_move(State *state, int depth);
};

typedef struct alpha_beta_node{
    int child_num;
    int heuristic;
    int score;
    int depth;
    Move pre_move;
    State* state;
    struct alpha_beta_node* parent;
    struct alpha_beta_node* child[1000];
    int alpha, beta;
}AlphaBetaNode;


AlphaBetaNode* build_alpha_beta_tree(Move move, State* cur_state, int cur_depth, int total_depth, AlphaBetaNode* parent_node);
AlphaBetaNode* build_alpha_beta_node(Move move, State* state, int num_of_child, int cur_depth);
int get_alpha_beta_best_score(AlphaBetaNode* root);
void delete_alpha_beta_tree(AlphaBetaNode* root);