#pragma once
#include "../state/state.hpp"

class sub_AlphaBeta{
public:
  static Move get_move(State *state, int depth);
};

typedef struct sub_alpha_beta_node{
    int child_num;
    int heuristic;
    int score;
    int depth;
    Move pre_move;
    State* state;
    struct sub_alpha_beta_node* parent;
    struct sub_alpha_beta_node* child[1000];
    int alpha, beta;
}sub_AlphaBetaNode;


sub_AlphaBetaNode* build_sub_alpha_beta_tree(Move move, State* cur_state, int cur_depth, int total_depth, sub_AlphaBetaNode* parent_node);
sub_AlphaBetaNode* build_sub_alpha_beta_node(Move move, State* state, int num_of_child, int cur_depth);
int get_sub_alpha_beta_best_score(sub_AlphaBetaNode* root);
void delete_sub_alpha_beta_tree(sub_AlphaBetaNode* root);