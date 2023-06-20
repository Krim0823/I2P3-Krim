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
Move MCTS::get_move(State *state){
  if(!state->legal_actions.size())
    state->get_legal_actions();
  MCTSNode* root = build_mcts_node(state->legal_actions.at(0), state, 0, NULL);
  // expand for root
  root->child_num = root->state->legal_actions.size();
  for(int i = 0; i <= root->child_num; i++) {
    State* new_state = root->state->next_state(root->state->legal_actions.at(i));
    root->child[i] = build_mcts_node(root->state->legal_actions.at(i), new_state, 1, root);
  } 
  // end expand 
  for(int i = 0; i <= 1e5; i++) mcts_tree_update(root);
  float goal_score = get_mcts_best_score(root);
  Move goal_move;
  for(int i = 0; i <= root->child_num; i++) {
    if(root->child[i]->UCB == goal_score) goal_move = root->child[i]->pre_move;
  }
  delete_mcts_tree(root);
  return goal_move;
}


MCTSNode* build_mcts_node(Move move, State* state, int depth, MCTSNode* parent_node) {
  MCTSNode* node = new MCTSNode;
  node->race = node->win = 0;
  node->UCB = 0;
  node->child_num = 0;
  node->pre_move = move;
  node->state = state;
  node->cur_depth = depth;
  node->parent = parent_node;
  return node;
}


void mcts_tree_update(MCTSNode* root) {
  MCTSNode* target_node = root;
  // select leaf
  float best_UCB = FLT_MIN;
  while (target_node->child_num) { // while not leaf
    MCTSNode* next_target_node;
    for(int i = 0; i < target_node->child_num; i++) {
      target_node->child[i]->UCB = get_UCB(target_node->child[i]);
      if(target_node->child[i]->UCB > best_UCB) {
        best_UCB = target_node->child[i]->UCB;
        next_target_node = target_node->child[i];
      }
    }
    target_node = next_target_node;
  }
  // end select

  // expand or rollout
  // end expand or rollout

  // back propagate
  // end back propagate
}



float get_mcts_best_score(MCTSNode* root) {
    float best_score = FLT_MIN;
    for(int i = 0; i < root->child_num; i++) 
      if(root->child[i]->UCB > best_score) best_score = root->child[i]->UCB;
    return best_score;
}


bool mcts_simulation(State* state, int cur_player) {

}


void delete_mcts_tree(MCTSNode* root) {
}
