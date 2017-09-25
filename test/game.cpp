#include <string.h>

#include "game.hpp"

const char * print_suite(suite_t suite)
{
  const char* names[] = {
    "DIAMOND",
    "CLUBS",
    "HEARTS",
    "SPADES",
  };
  return names[suite];
}

const char * print_number(number_t number)
{
  const char* names[] = {
    "<NONE>",
    "ACE",
    "DEUCE",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9",
    "10",
    "JACK",
    "QUEEN",
    "KING",
  };
  return names[number];
}

std::string card_t::to_string()
{
  const char * suite = print_suite(this->suite);
  const char * number = print_number(this->number);
  char buffer[100] = "<  ";
  strcat(buffer, number);
  strcat(buffer, ", ");
  strcat(buffer, suite);
  strcat(buffer, " >");
  return std::string(buffer);
}

