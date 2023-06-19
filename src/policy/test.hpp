#pragma once
#include "../state/state.hpp"

class Test{
public:
  static Move get_move(State *state, int depth);
};

typedef struct test_node{
    int child_num;
    int heuristic;
    int score;
    int depth;
    Move pre_move;
    State* state;
    struct test_node* parent;
    struct test_node* child[1000];
}TestNode;

TestNode* build_tree(Move move, State* cur_state, int cur_depth, int total_depth, TestNode* parent_node);
TestNode* build_node(Move move, State* state, int num_of_child, int cur_depth);
int get_best_score(TestNode* root);
void delete_tree(TestNode* root);