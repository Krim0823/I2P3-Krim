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
Move MCTS::get_move(State *state, int simulate){
  if(!state->legal_actions.size())
    state->get_legal_actions();
  // prevent the case that king is going to be eaten
  int best_evaluate = -10000;
  int safe_index = 0;
  bool king_danger = false;
  int size = state->legal_actions.size();
  for(int i = 0; i < size; i++) {
    State* new_state = state->next_state(state->legal_actions.at(i));
    int evaluate = new_state->evaluate();
    if(evaluate > 8000) return state->legal_actions.at(i);
    if(evaluate < -8000) king_danger = true;
    if(evaluate > best_evaluate) {
      best_evaluate = evaluate;
      safe_index = i;
    }
    if(king_danger && best_evaluate > -8000) return state->legal_actions.at(safe_index);
  }
  // end prevent


  MCTSNode* root = build_mcts_node(state->legal_actions.at(0), state, 0, NULL);
  // expand for root
  root->child_num = root->state->legal_actions.size();
  for(int i = 0; i < root->child_num; i++) {
    State* new_state = root->state->next_state(root->state->legal_actions.at(i));
    root->child[i] = build_mcts_node(root->state->legal_actions.at(i), new_state, 1, root);
  } 
  // end expand 
  for(int i = 0; i < simulate; i++) {
    mcts_tree_update(root);
    std::cout << "simulate:" << i+1 << std::endl;
  }
  Move goal_move = root->child[get_mcts_best_score(root)]->pre_move;
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
  MCTSNode* next_target_node = NULL;
  float best_UCB = FLT_MIN;
  bool player_win = false;
  // select leaf
  while (target_node->child_num) { // while not leaf
    next_target_node = NULL;
    best_UCB = FLT_MIN;
    for(int i = 0; i < target_node->child_num; i++) {
      target_node->child[i]->UCB = get_UCB(target_node->child[i]);
      if(target_node->child[i]->UCB > best_UCB) {
        best_UCB = target_node->child[i]->UCB;
        next_target_node = target_node->child[i];
      }
    }
    target_node = next_target_node;
    //std::cout << "select: " << target_node->cur_depth << std::endl;
  }
  // end select

  // expand 
  if(target_node->race) {
    if(!target_node->state->legal_actions.size())
      target_node->state->get_legal_actions();
    target_node->child_num = target_node->state->legal_actions.size();
    for(int i = 0; i < target_node->child_num; i++) {
      State* new_state = target_node->state->next_state(target_node->state->legal_actions.at(i));
      target_node->child[i] = build_mcts_node(target_node->state->legal_actions.at(i), new_state, target_node->cur_depth + 1, target_node);
    }
    if(target_node->state->legal_actions.size()) {
      target_node = target_node->child[0];
    }
  }
  // end expand

  // rollout
  player_win = mcts_simulation(target_node->state, root->state->player);
  target_node->race++;
  if(player_win) target_node->win++;
  // end rollout

  // back propagate
  while(target_node->parent != NULL) {
    //std::cout << "propogate: " << target_node->cur_depth << std::endl;
    target_node = target_node->parent;
    //std::cout << "race: " << target_node->race << std::endl;
    target_node->race++;
    if(player_win) target_node->win++;
  }
  // end back propagate
  
}


float get_UCB(MCTSNode* node) {
  if(node->race == 0) return FLT_MAX;
  int _win;
  int factor = 2;//sqrt(2), 2, 3
  if((node->cur_depth % 1)) _win = node->win;
  else _win = node->race - node->win;
  return ((float)(_win)/(float)(node->race)) + factor * sqrt(log(node->parent->race) / (float)(node->race));
}


int get_mcts_best_score(MCTSNode* root) {
    float best_score = 0;
    float cur_score = 0;
    int index = 0;
    for(int i = 0; i < root->child_num; i++) {
      cur_score = (float)(root->child[i]->win) / (float)(root->child[i]->race); 
      std::cout << "cur score = " << cur_score << std::endl;
      if(cur_score > best_score) {
        best_score = cur_score;
        index = i;
      }
    }
    std::cout << "best score = " << (float)(root->child[index]->win) / (float)(root->child[index]->race) << std::endl;
    return index;
}


int triplerand() { return (int)((rand() >> 3) ^ (rand() << 7) ^ (rand() >> 17)); }
bool mcts_simulation(State* state, int cur_player) {
  bool player_win = false;
  State* cur_state = state;
  int time = 1;
  int total_time = 30;
  int rand_num;
  while(time <= total_time) {
    if(!cur_state->legal_actions.size())
      cur_state->get_legal_actions();
    if(!cur_state->legal_actions.size()) break; // if no further action
    if(cur_state->game_state == WIN) {
      if(cur_state->player == cur_player) player_win = true;
      else player_win = false;
      break;
    }
    // get random num
    rand_num = triplerand() % cur_state->legal_actions.size();
    // end
    State* new_state = cur_state->next_state(cur_state->legal_actions.at(rand_num));
    cur_state = new_state;
    time++;
  }
  // draw : use get_piece_score
  /*if(time == total_time + 1) {
    if(cur_state->get_piece_score(cur_player) > 40) player_win = true;
  }*/
  return player_win;
}


void delete_mcts_tree(MCTSNode* root) {
  if(root->child_num) {
        for(int i = 0; i < root->child_num; i++) delete_mcts_tree(root->child[i]);
    }
    for(int i = 0; i < root->child_num; i++) delete root->child[i];
}


