#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "vision.hpp"

static cv::Mat suite_templates[4];
static robot_h robot;

void vision_init(robot_h arg_robot)
{
  robot = arg_robot;
}

static number_t recognize_number(uint32_t *number_pixels)
{
  return ACE;
}

static suite_t recognize_suite(uint32_t *suite_pixels)
{
}

card_t recognize_foundation_card(const int deck)
{
  return { .suite = HEART, .number = ACE };
}

card_t recognize_visible_pile_card()
{
  uint32_t x = VISIBLE_PILE.first;
  uint32_t y = VISIBLE_PILE.second;
  uint32_t *number_pixels =
    new uint32_t[CARD_NUMBER_HEIGHT * CARD_NUMBER_WIDTH];
  uint32_t *suite_pixels =
    new uint32_t[CARD_SUITE_HEIGHT * CARD_SUITE_WIDTH];
  const rectangle_t suite_rectangle =
    { .x = x,
      .y = y,
      .height = CARD_SUITE_HEIGHT,
      .width = CARD_SUITE_WIDTH };
  const rectangle_t number_rectangle =
    { .x = x,
      .y = y,
      .height = CARD_NUMBER_HEIGHT,
      .width = CARD_NUMBER_WIDTH };

  robot_screenshot(robot, number_rectangle, number_pixels);
  robot_screenshot(robot, suite_rectangle, suite_pixels);

  return {
    .suite = recognize_suite(suite_pixels),
    .number = recognize_number(number_pixels)
  };
}

card_t recognize_tableau_card(const tableau_position_t & position)
{
  return { .suite = HEART, .number = ACE };
}

void hackish_imshow(robot_h robot, const std::string& winname, cv::InputArray mat)
{
  robot_mouse_move(robot, 2000, 100);
  robot_mouse_press(robot, ROBOT_BUTTON1_MASK);
  robot_mouse_release(robot, ROBOT_BUTTON1_MASK);
  cv::imshow(winname, mat);
}
