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

int entry_point(int argc, const char *argv[])
{
  static char char_buffer[200];
  robot_h robot = robot_init();

  vision_init(robot);
  tableau_position_t pos = { .deck = 2, .num_hidden = 2, .position = 3 };
  /* card_t card = recognize_tableau_card(pos); */
  card_t card = recognize_foundation_card(0);
  std::cout << card.to_string() << std::endl;
  robot_free(robot);
  return 0;
}
