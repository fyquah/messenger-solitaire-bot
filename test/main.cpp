#include <iostream>
#include <thread>
#include <chrono>
#include <utility>

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <robot.h>

#include "game.hpp"
#include "vision.hpp"
#include "interact.hpp"

int entry_point(int argc, const char *argv[])
{
  static char char_buffer[200];
  robot_h robot = robot_init();

  vision_init(robot);
  interact_init(robot);
  tableau_position_t pos = { .deck = 2, .num_hidden = 2, .position = 3 };
  game_state_t game_state = load_initial_game_state();
  card_t c;
  c.number = ACE;
  c.suite = DIAMOND;
  game_state.stock_pile_size = 21;
  game_state.waste_pile_top = Option<card_t>(c);
  game_state = move_from_visible_pile_to_foundation(game_state, 2);

  std::cout << game_state << std::endl;
  robot_free(robot);
  return 0;
}
