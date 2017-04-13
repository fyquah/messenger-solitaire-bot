#include <iostream>
#include <thread>
#include <chrono>

#include <robot.h>


int entry_point(int argc, char *argv[])
{
  std::cout << "Hello" << std::endl;
  robot_h robot = robot_init();
  for (int i = 0 ; i < 10 ; i++) {
    robot_mouse_press(robot, ROBOT_BUTTON1_MASK);
    robot_mouse_move(robot, 500 + i * 10, 500 + i * 10);
    robot_mouse_release(robot, ROBOT_BUTTON1_MASK);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
  robot_free(robot);
  return 0;
}
