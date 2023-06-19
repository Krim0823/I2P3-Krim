#include <cstdlib>
#include <climits>
#include <iostream>

#include "../state/state.hpp"
#include "./alphabeta.hpp"


/**
 * @brief Randomly get a legal action
 * 
 * @param state Now state
 * @param depth You may need this for other policy
 * @return Move 
 */
Move AlphaBeta::get_move(State *state, int depth){
  if(!state->legal_actions.size())
    state->get_legal_actions();
  AlphaBetaNode* root = build_alpha_beta_tree(state->legal_actions.at(0), state, 0, depth, NULL);
  int goal_score = get_alpha_beta_best_score(root);
  Move goal_move;
  for(int i = 0; i <= root->child_num; i++) {
    if(root->child[i]->score == goal_score) {
      goal_move = root->child[i]->pre_move;
      break;
    }
  }
  delete_alpha_beta_tree(root);
  return goal_move;
}


AlphaBetaNode* build_alpha_beta_node(Move move, State* state, int num_of_child, int cur_depth) {
    AlphaBetaNode* node = new AlphaBetaNode;
    node->pre_move = move;
    node->state = state;
    node->heuristic = state->evaluate();
    if(node->heuristic < -8000) node->heuristic = -1e4; // if the state will lead to lose
    if(node->heuristic > 8000) node->heuristic = 1e4; // if the state will lead to win
    node->score = node->heuristic;
    node->child_num = num_of_child;
    node->depth = cur_depth;
    node->parent = NULL;
    // alpha_beta
    node->alpha = -1e4;
    node->beta = 1e4;
    // end alpha_beta
    for(int i = 0; i < num_of_child; i++) node->child[i] = NULL;
    return node;
}


AlphaBetaNode* build_alpha_beta_tree(Move move, State* cur_state, int cur_depth, int total_depth, AlphaBetaNode* parent_node) {
  if(!cur_state->legal_actions.size())
    cur_state->get_legal_actions();
  AlphaBetaNode* new_node = build_alpha_beta_node(move, cur_state, cur_state->legal_actions.size(), cur_depth);
  new_node->parent = parent_node;

  // alpha_beta
  if(new_node->depth % 2) new_node->alpha = new_node->parent->alpha;
  else if(new_node->depth > 0) new_node->beta = new_node->parent->beta;
  // end alpha_beta

  if(cur_depth == total_depth) {
    new_node->child_num = 0;
    if(cur_depth % 2) new_node->beta = new_node->score = new_node->heuristic;
    else if(cur_depth > 0) new_node->alpha = new_node->score = new_node->heuristic;
    return new_node;    
  }
  // win, lose case
  if(new_node->heuristic == 1e4 && cur_depth) {
    new_node->child_num = 0;
    new_node->score = (new_node->depth % 2) ? 1e4 : -1e4;
    if(cur_depth % 2) new_node->beta = 1e4;
    else new_node->alpha = -1e4;
    return new_node;  
  }
  if(new_node->heuristic == -1e4 && cur_depth) {
    new_node->child_num = 0;
    new_node->score = (new_node->depth % 2) ? -1e4 : 1e4;
    if(cur_depth % 2) new_node->beta = -1e4;
    else new_node->alpha = 1e4;
    return new_node;  
  }

  for(int i = 0; i < new_node->child_num; i++) {
    State* new_state = new_node->state->next_state(cur_state->legal_actions.at(i));
    new_node->child[i] = build_alpha_beta_tree(new_node->state->legal_actions.at(i), new_state, cur_depth + 1, total_depth, new_node);

    // alpha_beta
    if(new_node->depth % 2) {
      if(new_node->beta > new_node->child[i]->alpha) new_node->beta = new_node->child[i]->alpha;
    }
    else {
      if(new_node->alpha < new_node->child[i]->beta) new_node->alpha = new_node->child[i]->beta;
    }
    if(new_node->alpha >= new_node->beta) {
        new_node->child_num = i + 1;
        break;
    }
    // end alpha_beta
  }
  return new_node;
}


int get_alpha_beta_best_score(AlphaBetaNode* root) {
    if(root->child_num == 0) {
        return root->score;
    }

    int best_score = -1e5;
    int worst_score = 1e5;
    int store_score = 0;
    if((root->depth % 2) == 0) {  // currently in player's state
        for(int i = 0; i < root->child_num; i++) {
            store_score = get_alpha_beta_best_score(root->child[i]);
            if(store_score > best_score) best_score = store_score;
        }
    }
    else {  // currently in opponent's state
        for(int i = 0; i < root->child_num; i++) {
            store_score = get_alpha_beta_best_score(root->child[i]);
            if(store_score < worst_score) worst_score = store_score;
        }
    }
    int ans;
    if((root->depth % 2) == 0) ans = best_score;
    else ans = worst_score;
    root->score = ans;
    return ans;
}


void delete_alpha_beta_tree(AlphaBetaNode* root) {
    if(root->child_num) {
        for(int i = 0; i < root->child_num; i++) delete_alpha_beta_tree(root->child[i]);
    }
    delete root->state;
    for(int i = 0; i < root->child_num; i++) delete root->child[i];
}