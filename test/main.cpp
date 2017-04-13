#include <iostream>
#include <thread>
#include <chrono>

#include <robot.h>


int main()
{
  robot_h robot = robot_init();
  for (int i = 0 ; i < 10 ; i++) {
    robot_mouse_move(robot, 500, 500);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
  robot_free(robot);
}
