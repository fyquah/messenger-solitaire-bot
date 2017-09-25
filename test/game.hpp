#ifndef GAME_HPP
#define GAME_HPP

#include <stdint.h>

enum suite_t {
  CLUB,
  DIAMOND,
  HEART,
  SPADE,
};

enum number_t {
  ACE = 1,
  DEUCE = 2,
  THREE = 3,
  FOUR = 4,
  FIVE = 5,
  SIX = 6,
  SEVEN = 7,
  EIGHT = 8,
  NINE = 9,
  TEN = 10,
  JACK = 11,
  QUEEN = 12,
  KING = 13,
};

struct card_t {
  suite_t suite;
  number_t number;
};

struct tableau_position_t {
  uint32_t deck; /* 0 to 6 */
  uint32_t num_hidden;
  uint32_t position; 
};

#endif
