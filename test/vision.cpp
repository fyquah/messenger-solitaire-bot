#include <algorithm>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "vision.hpp"
#include "utils.hpp"

static const char* number_filenames[14] = {
  "",  /* 1-index, so 0 is a dnummy */
  "ace.bmp",
  "deuce.bmp",
  "three.bmp",
  "four.bmp",
  "five.bmp",
  "six.bmp",
  "seven.bmp",
  "eight.bmp",
  "nine.bmp",
  "ten.bmp",
  "jack.bmp",
  "queen.bmp",
  "king.bmp",
};

static const char* suite_filenames[4] = 
{
  "diamonds.bmp",
  "clubs.bmp",
  "hearts.bmp",
  "spades.bmp",
};

static robot_h robot;
static cv::Mat number_templates[14];
static cv::Mat suite_templates[4];

static cv::Rect make_crop(uint32_t width, uint32_t height)
{
  uint32_t offset_x = 2;
  uint32_t offset_y = 2;

  cv::Rect roi;
  roi.x = offset_x;
  roi.y = offset_y;
  roi.width = width - (offset_x*2);
  roi.height = height - (offset_y*2);
  return roi;
}

static void simple_threshold(cv::InputArray src, cv::OutputArray dest)
{
  cv::threshold(
    src,
    dest,
    200,
    255,
    0  /* 0 for binary threshold */
  );
}


void vision_init(robot_h arg_robot)
{
  static cv::Rect number_crop = make_crop(
      CARD_NUMBER_WIDTH, CARD_NUMBER_HEIGHT);
  static cv::Rect suite_crop = make_crop(
      CARD_SUITE_WIDTH, CARD_SUITE_HEIGHT);
  robot = arg_robot;

  for (int i = 1 ; i < 14;  i++) {
    std::string filename = std::string("res/numbers/") + number_filenames[i];

    number_templates[i] =
      cv::imread(filename, CV_LOAD_IMAGE_GRAYSCALE)(number_crop);
    simple_threshold(number_templates[i], number_templates[i]);
  }

  for (int i = 0 ; i < 4 ; i++) {
    std::string filename = std::string("res/suites/") + suite_filenames[i];
    suite_templates[i] =
      cv::imread(filename, CV_LOAD_IMAGE_GRAYSCALE)(suite_crop);
    simple_threshold(suite_templates[i], suite_templates[i]);
  }

  /* Ensures that the monitor is awake and not all dark. */
  robot_mouse_move(robot, TOP_LEFT_CORNER.first, TOP_LEFT_CORNER.second);
  robot_mouse_press(robot, ROBOT_BUTTON1_MASK);
  robot_mouse_release(robot, ROBOT_BUTTON1_MASK);
}

static number_t recognize_number(uint32_t *pixels)
{
  const uint32_t height = CARD_NUMBER_HEIGHT;
  const uint32_t width = CARD_NUMBER_WIDTH;
  double results[14];
  cv::Mat image = cv::Mat(height, width, CV_8UC4, pixels);

  cv::cvtColor(image, image, CV_RGBA2GRAY);
  simple_threshold(image, image);

  for (int i = 1 ; i < 14 ; i++) {
    cv::Mat matched;
    double min_val;

    cv::matchTemplate(image, number_templates[i], matched, CV_TM_SQDIFF_NORMED);
    cv::minMaxLoc(matched, &results[i]);
  }

  uint32_t pos = std::min_element(results + 1, results + 14) - results;
  return number_t(pos);
}

static suite_t recognize_suite(uint32_t *pixels)
{
  const uint32_t height = CARD_SUITE_HEIGHT;
  const uint32_t width = CARD_SUITE_WIDTH;
  double results[4];
  cv::Mat image = cv::Mat(height, width, CV_8UC4, pixels);
  cv::cvtColor(image, image, CV_RGBA2GRAY);
  simple_threshold(image, image);

  for (int i = 0 ; i < 4 ; i++) {
    cv::Mat matched;
    double min_val;

    cv::matchTemplate(image, suite_templates[i], matched, CV_TM_SQDIFF_NORMED);
    cv::minMaxLoc(matched, &results[i]);
  }

  uint32_t pos = std::min_element(results, results + 4) - results;
  return suite_t(pos);
}

static card_t recognize_card(uint32_t x, uint32_t y)
{
  uint32_t *number_pixels =
    new uint32_t[CARD_NUMBER_HEIGHT * CARD_NUMBER_WIDTH];
  uint32_t *suite_pixels =
    new uint32_t[CARD_SUITE_HEIGHT * CARD_SUITE_WIDTH];
  const rectangle_t suite_rectangle =
    { .x = x + SUITE_OFFSET,
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
  std::pair<uint32_t, uint32_t> pos;

  if (deck == 0) {
    pos = FOUNDATION_DECK_0;
  } else if (deck == 1) {
    pos = FOUNDATION_DECK_1;
  } else if (deck == 2) {
    pos = FOUNDATION_DECK_2;
  } else if (deck == 3) {
    pos = FOUNDATION_DECK_3;
  } else {
    throw RecognizeException();
  }

  return recognize_card(pos.first, pos.second);
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

