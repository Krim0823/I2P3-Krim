#include <cstdlib>
#include <climits>
#include <cfloat>
#include <iostream>

#include "../state/state.hpp"
#include "./mcts.hpp"


/**
 * @brief Randomly get a legal action
 * 
 * @param state Now state
 * @param depth You may need this for other policy
 * @return Move 
 */
Move MCTS::get_move(State *state, int depth){
  if(!state->legal_actions.size())
    state->get_legal_actions();
  MCTSNode* root = build_mcts_tree();
  float goal_score = get_mcts_best_score(root);
  Move goal_move;
  for(int i = 0; i <= root->child_num; i++) {
    if(root->child[i]->UCB == goal_score) goal_move = root->child[i]->pre_move;
  }
  delete_mcts_tree(root);
  return goal_move;
}


MCTSNode* build_mcts_node() {
}


MCTSNode* build_mcts_tree() {
}


float get_mcts_best_score(MCTSNode* root) {
    float best_score = FLT_MIN;
    for(int i = 0; i < root->child_num; i++) 
      if(root->child[i]->UCB > best_score) best_score = root->child[i]->UCB;
    return best_score;
}


void delete_mcts_tree(MCTSNode* root) {
}