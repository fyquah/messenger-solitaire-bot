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
  tableau_position_t pos = { .deck = 2, .num_hidden = 2, .position = 0 };
  game_state_t game_state = load_initial_game_state();
  game_state = move_from_column_to_column(game_state, pos, 6);
  pos = { .deck = 6, .num_hidden = 6, .position = 0 };
  game_state = move_from_column_to_column(game_state, pos, 4);

  std::cout << game_state << std::endl;
  robot_free(robot);
  return 0;
}
