#include <cstdlib>
#include <climits>
#include <iostream>

#include "../state/state.hpp"
#include "./minimax.hpp"


/**
 * @brief Randomly get a legal action
 * 
 * @param state Now state
 * @param depth You may need this for other policy
 * @return Move 
 */
Move MiniMax::get_move(State *state, int depth){
  if(!state->legal_actions.size())
    state->get_legal_actions();
  MiniMaxNode* root = build_tree(state->legal_actions.at(0), state, 0, depth, NULL);
  int goal_score = get_best_score(root);
  Move goal_move;
  for(int i = 0; i <= root->child_num; i++) {
    if(root->child[i]->score == goal_score) {
      goal_move = root->child[i]->pre_move;
      break;
    }
  }
  delete_tree(root);
  return goal_move;
}


MiniMaxNode* build_node(Move move, State* state, int num_of_child, int cur_depth) {
    MiniMaxNode* node = new MiniMaxNode;
    node->pre_move = move;
    node->state = state;
    node->heuristic = state->evaluate();
    if(node->heuristic < -8000) node->heuristic = -1e4; // if the state will lead to lose
    if(node->heuristic > 8000) node->heuristic = 1e4; // if the state will lead to win
    node->score = node->heuristic;
    node->child_num = num_of_child;
    node->depth = cur_depth;
    node->parent = NULL;
    for(int i = 0; i < num_of_child; i++) node->child[i] = NULL;
    return node;
}


MiniMaxNode* build_tree(Move move, State* cur_state, int cur_depth, int total_depth, MiniMaxNode* parent_node) {
  if(!cur_state->legal_actions.size())
    cur_state->get_legal_actions();
  MiniMaxNode* new_node = build_node(move, cur_state, cur_state->legal_actions.size(), cur_depth);
  new_node->parent = parent_node;

  if(cur_depth == total_depth) {
    new_node->child_num = 0;
    new_node->score = new_node->heuristic;
    return new_node;    
  }
  // win, lose case
  if(new_node->heuristic == 1e4 && cur_depth) {
    new_node->child_num = 0;
    new_node->score = (new_node->depth % 2) ? 1e4 : -1e4;
    return new_node;  
  }
  if(new_node->heuristic == -1e4 && cur_depth) {
    new_node->child_num = 0;
    new_node->score = (new_node->depth % 2) ? -1e4 : 1e4;
    return new_node;  
  }

  for(int i = 0; i < new_node->child_num; i++) {
    State* new_state = new_node->state->next_state(cur_state->legal_actions.at(i));
    new_node->child[i] = build_tree(new_node->state->legal_actions.at(i), new_state, cur_depth + 1, total_depth, new_node);
  }
  return new_node;
}


int get_best_score(MiniMaxNode* root) {
    if(root->child_num == 0) {
      /*std::cout << root->heuristic << " ";
        std::cout << root->score << "depth:" << root->depth << "child_num:" << root->child_num << std::endl;*/
      return root->score;
    }

    int best_score = -1e5;
    int worst_score = 1e5;
    int store_score = 0;
    if((root->depth % 2) == 0) {  // currently in player's state
        for(int i = 0; i < root->child_num; i++) {
            store_score = get_best_score(root->child[i]);
            if(store_score > best_score) best_score = store_score;
        }
    }
    else {  // currently in opponent's state
        for(int i = 0; i < root->child_num; i++) {
            store_score = get_best_score(root->child[i]);
            if(store_score < worst_score) worst_score = store_score;
        }
    }
    int ans;
    if((root->depth % 2) == 0) ans = best_score;
    else ans = worst_score;
    root->score = ans;
    return ans;
}


void delete_tree(MiniMaxNode* root) {
    if(root->child_num) {
        for(int i = 0; i < root->child_num; i++) delete_tree(root->child[i]);
    }
    for(int i = 0; i < root->child_num; i++) delete root->child[i];
}