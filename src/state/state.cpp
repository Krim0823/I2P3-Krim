#include <iostream>
#include <sstream>
#include <cstdint>
#include <set>
#include <climits>

#include "./state.hpp"
#include "../config.hpp"

static const int move_table_rook_bishop[8][7][2] = {
  {{0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}, {0, 6}, {0, 7}},
  {{0, -1}, {0, -2}, {0, -3}, {0, -4}, {0, -5}, {0, -6}, {0, -7}},
  {{1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}},
  {{-1, 0}, {-2, 0}, {-3, 0}, {-4, 0}, {-5, 0}, {-6, 0}, {-7, 0}},
  {{1, 1}, {2, 2}, {3, 3}, {4, 4}, {5, 5}, {6, 6}, {7, 7}},
  {{1, -1}, {2, -2}, {3, -3}, {4, -4}, {5, -5}, {6, -6}, {7, -7}},
  {{-1, 1}, {-2, 2}, {-3, 3}, {-4, 4}, {-5, 5}, {-6, 6}, {-7, 7}},
  {{-1, -1}, {-2, -2}, {-3, -3}, {-4, -4}, {-5, -5}, {-6, -6}, {-7, -7}},
};
static const int move_table_knight[8][2] = {
  {1, 2}, {1, -2},
  {-1, 2}, {-1, -2},
  {2, 1}, {2, -1},
  {-2, 1}, {-2, -1},
};
static const int move_table_king[8][2] = {
  {1, 0}, {0, 1}, {-1, 0}, {0, -1}, 
  {1, 1}, {1, -1}, {-1, 1}, {-1, -1},
};


/**
 * @brief evaluate the state
 * 
 * @return int 
 */
enum piece_score {
  PAWN = 3, //2
  ROOK = 12, //6
  KNIGHT = 14, //7
  BISHOP = 16, //8
  QUEEN = 50, //20
  KING = 10000, 
};

int State::get_piece_score(int cur_player){
  int score = 0;
  int total_piece[2] = {0, 0};
  int factor[2];
  factor[cur_player] = 1;
  factor[1 - cur_player] = -1;
  for(int pl=0; pl<2; pl+=1){
   for(int i=0; i<BOARD_H; i+=1){
     for(int j=0; j<BOARD_W; j+=1){
       switch(this->board.board[pl][i][j]) {
         case 1:
          score += (factor[pl] * PAWN);
          total_piece[pl]++;
          break;
         case 2:
          score += (factor[pl] * ROOK);
          total_piece[pl]++;
          break;
         case 3:
          score += (factor[pl] * KNIGHT);
          total_piece[pl]++;
          break;
         case 4:
          score += (factor[pl] * BISHOP);
          total_piece[pl]++;
          break;
         case 5:
          score += (factor[pl] * QUEEN);
          total_piece[pl]++;
          break;
         case 6:
          score += (factor[pl] * KING);
          total_piece[pl]++;
          king_location[pl] = Point(i, j);
          break;
         default:
          score += 0;
          break;
       }
     }
   }
  }
  
  int opponent_point[11] = {0, 0, 0, 1, 2, 4, 6, 8, 11, 15, 20};
  int player_point[11] = {0, 0, 0, 0, 0, 2, 8, 12, 15, 15, 15};
  return score - opponent_point[total_piece[1-cur_player]/2] + player_point[total_piece[cur_player]/2];
}

// standard: oppoenent's piece can reach the player's piece within one step
bool State::get_piece_safety(int cur_x, int cur_y, int cur_player){
  Point cur_piece(cur_x, cur_y);
  // if player's king will be eaten in one step, return INT_MIN
  // pawn and king
  for(int i = 0; i < 8; i++) {
    int x = cur_piece.first + move_table_rook_bishop[i][0][0];
    int y = cur_piece.second + move_table_rook_bishop[i][0][1];           
    if(x>=BOARD_H || x<0 || y>=BOARD_W || y<0) continue;
    if(this->board.board[1 - cur_player][x][y] == 6) return false;
    if(cur_player && (i == 4 || i == 5) && this->board.board[1 - cur_player][x][y] == 1) return false;
    if(!cur_player && (i == 6 || i == 7) && this->board.board[1 - cur_player][x][y] == 1) return false;
  }
  // knight
  for(auto move: move_table_knight) {
    int x = cur_piece.first + move[0];
    int y = cur_piece.second + move[1];           
    if(x>=BOARD_H || x<0 || y>=BOARD_W || y<0) continue;
    if(this->board.board[1 - cur_player][x][y] == 3) return false;
  }
  // rook or queen
  for(int i = 0; i < 4; i++) {
    for(int j = 0; j < 7; j++) {
      int x = cur_piece.first + move_table_rook_bishop[i][j][0];
      int y = cur_piece.second + move_table_rook_bishop[i][j][1];
      if(x>=BOARD_H || x<0 || y>=BOARD_W || y<0) break;
      if(this->board.board[cur_player][x][y]) break; // if there's player's piece between them
      if(this->board.board[1 - cur_player][x][y] == 2 || this->board.board[1 - cur_player][x][y] == 5) return false;
    }
  }
  // bishop and queen
  for(int i = 4; i < 8; i++) {
    for(int j = 0; j < 7; j++) {
      int x = cur_piece.first + move_table_rook_bishop[i][j][0];
      int y = cur_piece.second + move_table_rook_bishop[i][j][1];
      if(x>=BOARD_H || x<0 || y>=BOARD_W || y<0) break;
      if(this->board.board[cur_player][x][y]) break; // if there's player's piece between them
      if(this->board.board[1 - cur_player][x][y] == 4 || this->board.board[1 - cur_player][x][y] == 5) return false;
    }
  }
  return true;
}

int State::king_surrounding(int cur_player) {
  Point cur_king(king_location[cur_player].first, king_location[cur_player].second);
  size_t king_init = (cur_player) ? 0 : 5;
  int score[5] = {0, 0, 0, 0, 3};
  int num_of_piece = 0;
  int num_of_opponent = 0;
  for(int i = -1; i <= 1; i++) {
    for(int j = -1; j <= 1; j++) {
      int x = cur_king.first + i;
      int y = cur_king.second + j;
      if(x>=BOARD_H || x<0 || y>=BOARD_W || y<0) continue;
      if(this->board.board[cur_player][x][y]) num_of_piece++;
      if(this->board.board[1 - cur_player][x][y]) num_of_opponent++;
    }
  }
  num_of_piece--;
  int ans = num_of_piece - num_of_opponent;
  if(ans > 4) ans = 4;
  if(ans < 0) ans = 0;
  if(cur_king.first == king_init)  return 5;
  return score[ans];
}

int State::opponent_king_surrounding(int cur_player) {
  Point opponent_king(king_location[1 - cur_player].first, king_location[1 - cur_player].second);
  int score[7] = {-8, -5, 0, 0, 0, 5, 8};
  int num_of_piece = 0;
  for(int i = -2; i <= 2; i++) {
    for(int j = -2; j <= 2; j++) {
      int x = opponent_king.first + i;
      int y = opponent_king.second + j;
      if(x>=BOARD_H || x<0 || y>=BOARD_W || y<0) continue;
      if(this->board.board[cur_player][x][y] && this->board.board[cur_player][x][y] != 1) num_of_piece++;
    }
  }
  if(num_of_piece > 6) num_of_piece = 6;
  return score[num_of_piece];
}

int State::occupy_middle(int cur_player) {
  int score[4] = {0, 3, 6, 10};
  int num_of_piece = 0;
  for(int i = 2; i <= 3; i++) {
    for(int j = 1; j <= 3; j++) {
      if(this->board.board[cur_player][i][j]) num_of_piece++;
    }
  }
  if(num_of_piece > 3) num_of_piece = 3;
  return score[num_of_piece];
}

// standard: opponent's king is in danger, and his next step is in danger too
int State::dangerous_king_zone(int cur_player) {
  Point opponent_king(king_location[1 - cur_player].first, king_location[1 - cur_player].second);
  if (get_piece_safety(opponent_king.first, opponent_king.second, 1 - cur_player)) return 0;
  for(int i = -1; i <= 1; i++) {
    for(int j = -1; j <= 1; j++) {
      int x = opponent_king.first + i;
      int y = opponent_king.second + j;
      if(this->board.board[1 - cur_player][x][y] == 0) {
        if(get_piece_safety(x, y, 1 - cur_player)) return 0;
      }
    }
  }
  return 20;
}

int State::evaluate(){
  // [TODO] design your own evaluation function
  // this->player is actually opponent!
  int heuristic = 0;
  int cur_player = 1 - this->player;
  int store = 0;
  store = get_piece_score(cur_player);
  heuristic += store;
  ///////////////////////////////
  int pawn_to_queen_x = (cur_player) ? 1 : 4;
  std::set<int> opponent_eat;
  for(int i=0; i<BOARD_H; i+=1){
    for(int j=0; j<BOARD_W; j+=1){
      switch(this->board.board[cur_player][i][j]) {
        case 1:
         if(!get_piece_safety(i, j, cur_player)) opponent_eat.insert(0);
         if(i == pawn_to_queen_x) {
          if(!get_piece_safety(i, j, cur_player)) opponent_eat.insert(4);
         }
         break;
        case 2:
         if(!opponent_eat.empty())
           if(*opponent_eat.rbegin() > 1) break;
         if(!get_piece_safety(i, j, cur_player)) opponent_eat.insert(1);
         break;
        case 3:
         if(!opponent_eat.empty())
           if(*opponent_eat.rbegin() > 2) break;
         if(!get_piece_safety(i, j, cur_player)) opponent_eat.insert(2);
         break;
        case 4:
         if(!opponent_eat.empty())
           if(*opponent_eat.rbegin() > 3) break;
         if(!get_piece_safety(i, j, cur_player)) opponent_eat.insert(3);
         break;
        case 5:
          if(!opponent_eat.empty())
           if(*opponent_eat.rbegin() > 5) break;
          if(!get_piece_safety(i, j, cur_player)) opponent_eat.insert(5);
         
         break;
        case 6:
          if(!get_piece_safety(i, j, cur_player)) opponent_eat.insert(6);
         break;
        default:
         break;
      }
    }
  }
  if(!opponent_eat.empty()) {
  switch(*opponent_eat.rbegin()) {
    case 0:
     break;
    case 1:
     break;
    case 2:
     heuristic -= 3;
     break;
    case 3:
     heuristic -= 5;
     break;
    case 4: // pawn that are becoming queen
     heuristic -= 6;
     break;
    case 5:
     heuristic -= 30;
     break;
    case 6: // king
     heuristic -= 10000;
     break;
    default:
     break;
  }
  }
  opponent_eat.clear();
  ///////////////////////////////
  //heuristic += dangerous_king_zone(cur_player);
  ///////////////////////////////
  //heuristic += king_surrounding(cur_player);
  ///////////////////////////////
  //heuristic += opponent_king_surrounding(cur_player);
  ///////////////////////////////
  heuristic += occupy_middle(cur_player);
  ///////////////////////////////
  return heuristic;
}


/**
 * @brief return next state after the move
 * 
 * @param move 
 * @return State* 
 */
State* State::next_state(Move move){
  Board next = this->board;
  Point from = move.first, to = move.second;
  
  int8_t moved = next.board[this->player][from.first][from.second];
  //promotion for pawn
  if(moved == 1 && (to.first==BOARD_H-1 || to.first==0)){
    moved = 5;
  }
  if(next.board[1-this->player][to.first][to.second]){
    next.board[1-this->player][to.first][to.second] = 0;
  }
  
  next.board[this->player][from.first][from.second] = 0;
  next.board[this->player][to.first][to.second] = moved;
  
  State* next_state = new State(next, 1-this->player);
  
  if(this->game_state != WIN)
    next_state->get_legal_actions();
  return next_state;
}


/**
 * @brief get all legal actions of now state
 * 
 */
void State::get_legal_actions(){
  // [Optional]
  // This method is not very efficient
  // You can redesign it
  this->game_state = NONE;
  std::vector<Move> all_actions;
  auto self_board = this->board.board[this->player];
  auto oppn_board = this->board.board[1 - this->player];
  
  int now_piece, oppn_piece;
  for(int i=0; i<BOARD_H; i+=1){
    for(int j=0; j<BOARD_W; j+=1){
      if((now_piece=self_board[i][j])){
        // std::cout << this->player << "," << now_piece << ' ';
        switch (now_piece){
          case 1: //pawn
            if(this->player && i<BOARD_H-1){
              //black
              if(!oppn_board[i+1][j] && !self_board[i+1][j])
                all_actions.push_back(Move(Point(i, j), Point(i+1, j)));
              if(j<BOARD_W-1 && (oppn_piece=oppn_board[i+1][j+1])>0){
                all_actions.push_back(Move(Point(i, j), Point(i+1, j+1)));
                if(oppn_piece==6){
                  this->game_state = WIN;
                  this->legal_actions = all_actions;
                  return;
                }
              }
              if(j>0 && (oppn_piece=oppn_board[i+1][j-1])>0){
                all_actions.push_back(Move(Point(i, j), Point(i+1, j-1)));
                if(oppn_piece==6){
                  this->game_state = WIN;
                  this->legal_actions = all_actions;
                  return;
                }
              }
            }else if(!this->player && i>0){
              //white
              if(!oppn_board[i-1][j] && !self_board[i-1][j])
                all_actions.push_back(Move(Point(i, j), Point(i-1, j)));
              if(j<BOARD_W-1 && (oppn_piece=oppn_board[i-1][j+1])>0){
                all_actions.push_back(Move(Point(i, j), Point(i-1, j+1)));
                if(oppn_piece==6){
                  this->game_state = WIN;
                  this->legal_actions = all_actions;
                  return;
                }
              }
              if(j>0 && (oppn_piece=oppn_board[i-1][j-1])>0){
                all_actions.push_back(Move(Point(i, j), Point(i-1, j-1)));
                if(oppn_piece==6){
                  this->game_state = WIN;
                  this->legal_actions = all_actions;
                  return;
                }
              }
            }
            break;
          
          case 2: //rook
          case 4: //bishop
          case 5: //queen
            int st, end;
            switch (now_piece){
              case 2: st=0; end=4; break; //rook
              case 4: st=4; end=8; break; //bishop
              case 5: st=0; end=8; break; //queen
              default: st=0; end=-1;
            }
            for(int part=st; part<end; part+=1){
              auto move_list = move_table_rook_bishop[part];
              for(int k=0; k<std::max(BOARD_H, BOARD_W); k+=1){
                int p[2] = {move_list[k][0] + i, move_list[k][1] + j};
                
                if(p[0]>=BOARD_H || p[0]<0 || p[1]>=BOARD_W || p[1]<0) break;
                now_piece = self_board[p[0]][p[1]];
                if(now_piece) break;
                
                all_actions.push_back(Move(Point(i, j), Point(p[0], p[1])));
                
                oppn_piece = oppn_board[p[0]][p[1]];
                if(oppn_piece){
                  if(oppn_piece==6){
                    this->game_state = WIN;
                    this->legal_actions = all_actions;
                    return;
                  }else
                    break;
                };
              }
            }
            break;
          
          case 3: //knight
            for(auto move: move_table_knight){
              int x = move[0] + i;
              int y = move[1] + j;
              
              if(x>=BOARD_H || x<0 || y>=BOARD_W || y<0) continue;
              now_piece = self_board[x][y];
              if(now_piece) continue;
              all_actions.push_back(Move(Point(i, j), Point(x, y)));
              
              oppn_piece = oppn_board[x][y];
              if(oppn_piece==6){
                this->game_state = WIN;
                this->legal_actions = all_actions;
                return;
              }
            }
            break;
          
          case 6: //king
            for(auto move: move_table_king){
              int p[2] = {move[0] + i, move[1] + j};
              
              if(p[0]>=BOARD_H || p[0]<0 || p[1]>=BOARD_W || p[1]<0) continue;
              now_piece = self_board[p[0]][p[1]];
              if(now_piece) continue;
              
              all_actions.push_back(Move(Point(i, j), Point(p[0], p[1])));
              
              oppn_piece = oppn_board[p[0]][p[1]];
              if(oppn_piece==6){
                this->game_state = WIN;
                this->legal_actions = all_actions;
                return;
              }
            }
            break;
        }
      }
    }
  }
  std::cout << "\n";
  this->legal_actions = all_actions;
}


const char piece_table[2][7][5] = {
  {" ", "♙", "♖", "♘", "♗", "♕", "♔"},
  {" ", "♟", "♜", "♞", "♝", "♛", "♚"}
};
/**
 * @brief encode the output for command line output
 * 
 * @return std::string 
 */
std::string State::encode_output(){
  std::stringstream ss;
  int now_piece;
  for(int i=0; i<BOARD_H; i+=1){
    for(int j=0; j<BOARD_W; j+=1){
      if((now_piece = this->board.board[0][i][j])){
        ss << std::string(piece_table[0][now_piece]);
      }else if((now_piece = this->board.board[1][i][j])){
        ss << std::string(piece_table[1][now_piece]);
      }else{
        ss << " ";
      }
      ss << " ";
    }
    ss << "\n";
  }
  return ss.str();
}


/**
 * @brief encode the state to the format for player
 * 
 * @return std::string 
 */
std::string State::encode_state(){
  std::stringstream ss;
  ss << this->player;
  ss << "\n";
  for(int pl=0; pl<2; pl+=1){
    for(int i=0; i<BOARD_H; i+=1){
      for(int j=0; j<BOARD_W; j+=1){
        ss << int(this->board.board[pl][i][j]);
        ss << " ";
      }
      ss << "\n";
    }
    ss << "\n";
  }
  return ss.str();
}