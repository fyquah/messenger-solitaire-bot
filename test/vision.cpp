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

static suite_t recognize_suite(uint32_t *pixels)
{
  const uint32_t height = CARD_SUITE_HEIGHT;
  const uint32_t width = CARD_SUITE_WIDTH;
  cv::Mat image = cv::Mat(height, width, CV_8UC4, pixels);
  hackish_imshow("hello", image);
  cv::waitKey(0);
  return HEART;
}

card_t recognize_card(int x, int y)
{
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

card_t recognize_foundation_card(const int deck)
{
  return { .suite = HEART, .number = ACE };
}

card_t recognize_visible_pile_card()
{
  return recognize_card(VISIBLE_PILE.first, VISIBLE_PILE.second);
}

card_t recognize_tableau_card(const tableau_position_t & position)
{
  int x = TABLEAU.first + position.deck * TABLEAU_SIDE_OFFSET;
  int y = TABLEAU.second
    + (position.num_hidden * TABLEAU_UNSEEN_OFFSET)
    + (position.position * TABLEAU_SEEN_OFFSET);
  return recognize_card(x, y);
}

void hackish_imshow(const std::string& winname, cv::InputArray mat)
{
  robot_mouse_move(robot, 2000, 100);
  robot_mouse_press(robot, ROBOT_BUTTON1_MASK);
  robot_mouse_release(robot, ROBOT_BUTTON1_MASK);
  cv::imshow(winname, mat);
}
