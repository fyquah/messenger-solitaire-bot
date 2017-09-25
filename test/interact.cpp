#include <robot.h>

#include "interact.hpp"
#include "vision.hpp"
#include "game.hpp"

static bool sandbox = false;

void set_sandbox_mode(bool a)
{
  sandbox = a;
}

game_state_t load_initial_game_state()
{
  tableau_deck_t tableau[7];
  uint32_t stock_pile_size = 24;

  for (uint32_t i = 0 ; i < 7 ; i++) {
    tableau_position_t pos = { .deck = i, .num_hidden = i, .position = 0 };
    tableau[i].num_down_cards = i;
    tableau[i].cards = { recognize_tableau_card(pos) };
  }
  auto none = Option<card_t>();

  /* A sane compiler should be able to inline this at the call site */
  game_state_t ret;
  for (int i = 0 ; i < 4 ; i++) {
    ret.foundation[i] = none;
  }
  ret.waste_pile_top = none;
  for (int i = 0; i < 7; i++) {
    ret.tableau[i] = tableau[i];
  }
  ret.stock_pile_size = stock_pile_size;

  return ret;
}
