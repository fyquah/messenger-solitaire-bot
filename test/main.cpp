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

/* [TOP_LEFT_CORNER] referes to those of the game window rather than the
 * deck of cards.
 */
auto TOP_LEFT_CORNER = std::make_pair(35, 147);

uint32_t CARD_WIDTH = 30;
uint32_t CARD_HEIGHT = 70;

/* Card positions are referenced by the top left corner */
const auto FOUNDATION_DECK_0 = std::make_pair(40, 152);
const auto FOUNDATION_DECK_1 = std::make_pair(110, 152);
const auto FOUNDATION_DECK_2 = std::make_pair(180, 152);
const auto FOUNDATION_DECK_3 = std::make_pair(250, 152);
const auto DRAW_PILE = std::make_pair(461, 152);
const auto VISIBLE_PILE = std::make_pair(355, 152);
const auto TABLEAU = std::make_pair(40, 266);
const auto TABLEAU_SIDE_OFFSET = 70;  /* Offsets between decks. */
const auto TABLEAU_UNSEEN_OFFSET = 14;  /* Offset between flipped cards. */
const auto TABLEAU_SEEN_OFFSET = 28;  /* Offset between unflipped cards. */

/* A variant of imshow that displays in the _second_ window. This prevents
 * opening a window in the same screen as the game (in a dual monitor setup)
 * which potentially messes up the tiles in xmonad.
 */
void hackish_imshow(robot_h robot, const std::string& winname, cv::InputArray mat)
{
  robot_mouse_move(robot, 2000, 100);
  robot_mouse_press(robot, ROBOT_BUTTON1_MASK);
  robot_mouse_release(robot, ROBOT_BUTTON1_MASK);
  cv::imshow(winname, mat);
}

int entry_point(int argc, const char *argv[])
{
  robot_h robot = robot_init();
  auto pos = TABLEAU;
  uint32_t *dest = new uint32_t[100 * 100];

  pos.first = pos.first + 2 * TABLEAU_SIDE_OFFSET;
  pos.second = pos.second + 2 * TABLEAU_UNSEEN_OFFSET + 0 * TABLEAU_SEEN_OFFSET;

  for (int i = 0 ; i < 1 ; i++) {
    uint32_t x = pos.first + i * 10;
    uint32_t y = pos.second + i * 10;

    printf("Moving to: %u %u\n", x, y);

    robot_mouse_move(robot, x, y);
    robot_mouse_press(robot, ROBOT_BUTTON1_MASK);
    robot_mouse_release(robot, ROBOT_BUTTON1_MASK);
    rectangle_t rect = { .x = x, .y = y, .height = 100, .width = 100 };

    robot_screenshot(robot, rect, dest);
    sleep(1);
  }

  cv::Mat image = cv::Mat(100, 100, CV_8UC4, dest);
  hackish_imshow(robot, "window", image);
  cv::waitKey(0);

  robot_free(robot);
  return 0;
}
