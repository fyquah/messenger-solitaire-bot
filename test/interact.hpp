#ifndef INTERACT_HPP
#define INTERACT_HPP

#include <string>
#include <exception>

#include <robot.h>

#include "game.hpp"

class IllegalMoveException : public std::exception {
private:
  std::string msg;
public:
  IllegalMoveException(std::string msg);
  virtual const char* what() const throw ();
};
class InconsistentArgument : public std::exception {};

void interact_init(robot_h robot);
void set_sandbox_mode(bool flag);

game_state_t load_initial_game_state();
game_state_t draw_from_stock_pile(const game_state_t &);
game_state_t reset_stock_pile(const game_state_t &);

game_state_t move_from_visible_pile_to_tableau(
    const game_state_t &,
    const uint32_t deck
);
game_state_t move_from_visible_pile_to_foundation(
    const game_state_t &,
    const uint32_t deck
);
game_state_t move_from_tableau_to_foundation(
    const game_state_t &,
    const uint32_t tableau_position,
    const uint32_t destination
);
game_state_t move_from_column_to_column(
    const game_state_t & game,
    const tableau_position_t & position,
    const uint32_t destination
);


bool is_promote_to_foundation_legal(
    const Option<card_t> foundation,
    const card_t & card
);
void interact_short_sleep();
void click_card(uint32_t x, uint32_t y);

#endif
