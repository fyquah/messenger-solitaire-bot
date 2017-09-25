#ifndef VISION_HPP
#define VISION_HPP

#include <utility>
#include <exception>

#include <robot.h>

#include "game.hpp"

/* [TOP_LEFT_CORNER] referes to those of the game window rather than the
 * deck of cards.
 */
const auto TOP_LEFT_CORNER = std::make_pair(35, 147);

const uint32_t CARD_WIDTH = 30;
const uint32_t CARD_HEIGHT = 70;

/* Card positions are referenced by the top left corner */
const auto FOUNDATION_DECK_0 = std::make_pair(40,  152);
const auto FOUNDATION_DECK_1 = std::make_pair(110, 152);
const auto FOUNDATION_DECK_2 = std::make_pair(180, 152);
const auto FOUNDATION_DECK_3 = std::make_pair(250, 152);
const auto DRAW_PILE = std::make_pair(461, 152);
const auto VISIBLE_PILE = std::make_pair(355, 152);
const auto TABLEAU = std::make_pair(40, 263);
const auto TABLEAU_SIDE_OFFSET = 70;  /* Offsets between decks. */
const auto TABLEAU_UNSEEN_OFFSET = 14;  /* Offset between flipped cards. */
const auto TABLEAU_SEEN_OFFSET = 28;  /* Offset between unflipped cards. */
const auto SUITE_OFFSET = 30u; /* Offset from beginning of card to suite */

const uint32_t CARD_NUMBER_HEIGHT = 30;
const uint32_t CARD_NUMBER_WIDTH = 25;
const uint32_t CARD_SUITE_HEIGHT = 28;
const uint32_t CARD_SUITE_WIDTH = 28;


/* Loads templates for template matching etc. */
void vision_init(robot_h robot);


/* A variant of imshow that displays in the _second_ window. This prevents
 * opening a window in the same screen as the game (in a dual monitor setup)
 * which potentially messes up the tiles in xmonad.
 */
void hackish_imshow(
    const std::string& winname,
    cv::InputArray mat
);


class RecognizeException : public std::exception {
};


/* Throws [RecognizeException]. This is an exception, rather than returning
 * a monad, simply because in practice, this _shouldn't_ happen at all
 * if out model of the game is consistent.
 *
 * These functions are thread safe.
 */
card_t recognize_foundation_card(const int deck);
card_t recognize_visible_pile_card();
card_t recognize_tableau_card(const tableau_position_t & position);

#endif
