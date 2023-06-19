#pragma once
#include "../state/state.hpp"
#include <cmath>
#include <cfloat>

class MCTS{
public:
  static Move get_move(State *state, int depth);
};

typedef struct mcts_node{
    int child_num;
    int win;
    int race;
    float UCB;
    Move pre_move;
    State* state;
    struct mcts_node* parent;
    struct mcts_node* child[1000];
}MCTSNode;

MCTSNode* build_mcts_tree();
MCTSNode* build_mcts_node();
float get_mcts_best_score(MCTSNode* root);
void delete_mcts_tree(MCTSNode* root);

float get_UCB(MCTSNode* node) {
    if(node->race == 0) return FLT_MAX;
    return ((float)(node->win)/(float)(node->race)) + sqrt(2) * sqrt(log(node->parent->race) / (float)(node->race));

}