#ifndef STRATEGY_HPP
#define STRATEGY_HPP

#include "game.hpp"

game_state_t strategy_init(const game_state_t & state);
game_state_t strategy_step(const game_state_t & state, bool *moved);
game_state_t strategy_term(game_state_t state);
void strategy_print_internal_state();

#endif
