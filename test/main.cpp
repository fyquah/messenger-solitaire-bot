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
  tableau_position_t pos = { .deck = 6, .num_hidden = 6, .position = 0 };
  recognize_tableau_card(pos);
  robot_free(robot);
  return 0;
}
