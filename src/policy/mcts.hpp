#pragma once
#include "../state/state.hpp"
#include <cmath>
#include <cfloat>

class MCTS{
public:
  static Move get_move(State *state, int depth);
};

typedef struct mcts_node{
    int child_num; // 0->leaf
    int win;
    int race;
    float UCB;
    int cur_depth;
    Move pre_move; 
    State* state;
    struct mcts_node* parent; // NULL->root
    struct mcts_node* child[1000];
}MCTSNode;

void mcts_tree_update(MCTSNode* root);
MCTSNode* build_mcts_node(Move move, State* state, int depth, MCTSNode* parent_node);
float get_mcts_best_score(MCTSNode* root);
void delete_mcts_tree(MCTSNode* root);
bool mcts_simulation(State* state, int cur_player); // rollout

float get_UCB(MCTSNode* node) {
  if(node->race == 0) return FLT_MAX;
  int _win;
  if(node->cur_depth % 1) _win = node->win;
  else _win = node->race - node->win;
  return ((float)(_win)/(float)(node->race)) + sqrt(2) * sqrt(log(node->parent->race) / (float)(node->race));

}