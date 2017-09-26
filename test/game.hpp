#ifndef GAME_HPP
#define GAME_HPP

#include <stdint.h>

#include <iostream>
#include <string>
#include <vector>

#include "utils.hpp"

enum suite_t {  /* Ordering equivalent to chinese poker. */
  DIAMOND,
  CLUB,
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

  std::string to_string() const;
};

struct tableau_position_t {
  uint32_t deck; /* 0 to 6 */
  uint32_t num_hidden;
  uint32_t position; 
};

struct tableau_deck_t {
  uint32_t num_down_cards;
  std::vector<card_t> cards; /* New cards get added to the end of queue */
};

const char * print_suite(suite_t suite);
const char * print_number(number_t number);

/* TODO(fyquah): Ideally I want to have all the fields below be [const].
 * it is, however, fairly hard to initialize structs with array fields in
 * C++.
 * */
struct game_state_t {
  Option<card_t> foundation[4];
  tableau_deck_t tableau[7];
  Option<card_t> waste_pile_top;
  uint32_t stock_pile_size;
  uint32_t remaining_pile_size;
};

std::ostream& operator<<(std::ostream & out, const game_state_t & game);

#endif
