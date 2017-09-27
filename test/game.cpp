#include <string.h>

#include <iostream>

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

std::string card_t::to_string() const
{
  const char * suite = print_suite(this->suite);
  const char * number = print_number(this->number);
  char buffer[100] = "<";
  strcat(buffer, number);
  strcat(buffer, " ");
  strcat(buffer, suite);
  strcat(buffer, ">");
  return std::string(buffer);
}

bool operator==(const card_t & a, const card_t & b)
{
  return a.number == b.number && a.suite == b.suite;
}

std::ostream& operator<<(std::ostream & out, const game_state_t & game)
{
  out << ">> Game state\n";
  out << "Foundation: \n";

  for (int i = 0 ; i < 4 ; i++) {
    if (game.foundation[i].is_some()) {
      out << i << ": " << game.foundation[i].get().to_string() << "\n";
    } else {
      out << i << ": <NONE>\n";
    }
  }

  out << "Waste pile top: ";
  if (game.waste_pile_top.is_some()) {
    out << game.waste_pile_top.get().to_string();
  } else {
    out << "<NONE>";
  }
  out << "\n";

  out << "Stock pile size = " << game.stock_pile_size << "\n";
  out << "Remaining pile size = " << game.remaining_pile_size << "\n";

  for (int i = 0 ; i < 7 ; i++) {
    const tableau_deck_t & deck = game.tableau[i];

    out << "Deck " << i << ": ";

    for (int j = 0 ; j < deck.num_down_cards ; j++) {
      out << "* ";
    }

    for (const auto & card : deck.cards) {
      out << card.to_string() << " " ;
    }

    out << "\n";
  }

  out << "<< End of game state\n";
}

