#include <unistd.h>

#include <robot.h>

#include "interact.hpp"
#include "vision.hpp"
#include "game.hpp"

static bool sandbox = false;
static robot_h robot;

IllegalMoveException::IllegalMoveException(std::string msg) : msg(msg) {}

const char * IllegalMoveException::what() const throw()
{
  return msg.c_str();
}

static bool is_short_sleep = false;

void interact_short_sleep() 
{
  is_short_sleep = true;
}

static const uint32_t LONG_SLEEP = 220000;
static const uint32_t SHORT_SLEEP = 200000;

static void click_card(uint32_t x, uint32_t y)
{
  robot_mouse_move(robot, x, y);
  robot_mouse_press(robot, ROBOT_BUTTON1_MASK);
  robot_mouse_release(robot, ROBOT_BUTTON1_MASK);

  /* TODO(fyquah): This isn't super reliable, as it is decided based on
   * the processor's (underterministic) speed.
   */
    usleep(SHORT_SLEEP);
}

static void drag_mouse(
    std::pair<uint32_t, uint32_t> from,
    std::pair<uint32_t, uint32_t> to
)
{
  robot_mouse_move(robot, from.first, from.second);
  robot_mouse_press(robot, ROBOT_BUTTON1_MASK);
  robot_mouse_move(robot, to.first, to.second);
  robot_mouse_release(robot, ROBOT_BUTTON1_MASK);

    usleep(SHORT_SLEEP);
}

static void unsafe_remove_card_from_visible_pile(game_state_t *state)
{
  std::cout << "Unsafe operation! Original remaining pile size = "
    << state->remaining_pile_size
    << " original stock pile size = "
    << state->stock_pile_size
    << std::endl;

  state->remaining_pile_size = state->remaining_pile_size - 1;

  if (state->remaining_pile_size == state->stock_pile_size) {
    state->waste_pile_top = Option<card_t>();
  } else {
    state->waste_pile_top = Option<card_t>(recognize_visible_pile_card());
  }
}

static void unsafe_remove_card_from_tableau(
    game_state_t *state, uint32_t deck)
{
  tableau_deck_t & tbl_deck = state->tableau[deck];
  tbl_deck.cards.pop_back();

  if (tbl_deck.cards.size() == 0 && tbl_deck.num_down_cards != 0) {
    tableau_position_t pos = {
      .deck = deck,
      .num_hidden = tbl_deck.num_down_cards - 1,
      .position = 0
    };

    tbl_deck.num_down_cards -= 1;
    tbl_deck.cards.push_back(recognize_tableau_card(pos));
  }
}

static std::pair<uint32_t, uint32_t> get_end_card_position(
    const game_state_t & state, uint32_t deck_pos)
{
  tableau_deck_t tbl_deck = state.tableau[deck_pos];

  return std::make_pair(
    TABLEAU.first
      + (deck_pos * TABLEAU_SIDE_OFFSET)
      + (CARD_WIDTH / 2),
    TABLEAU.second
      + (tbl_deck.num_down_cards * TABLEAU_UNSEEN_OFFSET)
      + ((tbl_deck.cards.size() ? tbl_deck.cards.size() - 1 : 0) * TABLEAU_SEEN_OFFSET)
      + (CARD_HEIGHT / 2)
  );
}

void interact_init(robot_h r)
{
  robot = r;
}

void set_sandbox_mode(bool a)
{
  sandbox = a;
}

game_state_t load_initial_game_state()
{
  tableau_deck_t tableau[7];
  uint32_t stock_pile_size = 24;

  for (uint32_t i = 0 ; i < 7 ; i++) {
    tableau_position_t pos = { .deck = i, .num_hidden = i, .position = 0 };
    tableau[i].num_down_cards = i;
    tableau[i].cards = { recognize_tableau_card(pos) };
  }
  auto none = Option<card_t>();

  /* A sane compiler should be able to inline this at the call site */
  game_state_t ret;
  for (int i = 0 ; i < 4 ; i++) {
    ret.foundation[i] = none;
  }
  ret.waste_pile_top = none;
  for (int i = 0; i < 7; i++) {
    ret.tableau[i] = tableau[i];
  }
  ret.stock_pile_size = stock_pile_size;
  ret.remaining_pile_size = stock_pile_size;

  return ret;
}

game_state_t draw_from_stock_pile(const game_state_t & state)
{
  game_state_t next_state = state;

  if (state.stock_pile_size <= 0) {
    throw IllegalMoveException(
        "Cannot draw from stock pile when stock pile size is <= 0");
  }

  click_card(
      DRAW_PILE.first + CARD_WIDTH / 2,
      DRAW_PILE.second + CARD_HEIGHT / 2
  );

  next_state.stock_pile_size -= 1;
  next_state.waste_pile_top = Option<card_t>(recognize_visible_pile_card());

  return next_state;
}

game_state_t reset_stock_pile(const game_state_t & state)
{
  game_state_t next_state = state;

  if (state.stock_pile_size != 0) {
    throw IllegalMoveException(
        "Cannot reset stock pile when stock_pile_size != 0");
  }

  click_card(
      DRAW_PILE.first + CARD_WIDTH / 2,
      DRAW_PILE.second + CARD_HEIGHT / 2
  );

  next_state.stock_pile_size = state.remaining_pile_size;
  next_state.waste_pile_top = Option<card_t>();

  return next_state;
}

static bool is_transfer_legal(
    const card_t & card, const tableau_deck_t & deck)
{
  if (deck.num_down_cards == 0
      && deck.cards.size() == 0
      && card.number == KING) {
    return true;

  } else if (deck.cards.size() == 0) {
    return false;

  } else if (deck.cards.back().number - 1 == card.number
      && (deck.cards.back().suite % 2) != (card.suite % 2)) {
    return true;

  } else {
    return false;
  }
}

bool is_promote_to_foundation_legal(
    const Option<card_t> foundation,
    const card_t & card)
{
  return (
      (!foundation.is_some() && card.number == ACE)
      || (foundation.is_some()
          && foundation.get().suite == card.suite
          && foundation.get().number == card.number - 1)
  );
}

game_state_t move_from_visible_pile_to_tableau(
    const game_state_t & state,
    const uint32_t deck
)
{
  std::cout << "Moving from visible pile to tableau" << std::endl;
  if (!state.waste_pile_top.is_some()) {
    throw IllegalMoveException(
        "waste pile is missing. Cannot move to tableau."
    );
  }

  const card_t & waste_pile_top = state.waste_pile_top.get();
  const tableau_deck_t & tableau_deck = state.tableau[deck];

  if (!is_transfer_legal(waste_pile_top, tableau_deck)) {
    throw IllegalMoveException(
        "Intended transfer from waste pile to tableau is illegal. Tbl "
        + std::to_string(deck));
  }

  /* Dragging the card in the game */
  std::pair<uint32_t, uint32_t> from = std::make_pair(
      VISIBLE_PILE.first + CARD_WIDTH / 2 ,
      VISIBLE_PILE.second + CARD_HEIGHT / 2
  );
  std::pair<uint32_t, uint32_t> to = get_end_card_position(state, deck);
  drag_mouse(from, to);

  game_state_t next_state = state;

  next_state.tableau[deck].cards.push_back(waste_pile_top);

  /* See the new card in the pile */
  unsafe_remove_card_from_visible_pile(&next_state);

  return next_state;
}

game_state_t move_from_visible_pile_to_foundation(
    const game_state_t & state,
    const uint32_t foundation_pos
)
{
  if (!state.waste_pile_top.is_some()) {
    throw IllegalMoveException("Missing visible pile to to move to foundation");
  }

  const Option<card_t> & foundation = state.foundation[foundation_pos];
  const card_t & waste_pile_top = state.waste_pile_top.get();

  if (!is_promote_to_foundation_legal(foundation, waste_pile_top)) {
    throw IllegalMoveException(
        "Intended promotion from waste pile to foundation is illegal");
  }

  std::pair<uint32_t, uint32_t> from = std::make_pair(
      VISIBLE_PILE.first + CARD_WIDTH / 2 ,
      VISIBLE_PILE.second + CARD_HEIGHT / 2
  );
  std::pair<uint32_t, uint32_t> to = std::make_pair(
      FOUNDATION_DECKS[foundation_pos].first + CARD_WIDTH / 2 ,
      FOUNDATION_DECKS[foundation_pos].second + CARD_HEIGHT / 2
  );
  drag_mouse(from, to);

  game_state_t next_state = state;
  next_state.foundation[foundation_pos] = Option<card_t>(waste_pile_top);
  unsafe_remove_card_from_visible_pile(&next_state);

  return next_state;
}

game_state_t move_from_tableau_to_foundation(
  const game_state_t & state,
  const uint32_t tableau_position,
  const uint32_t foundation_position 
)
{
  const tableau_deck_t & tbl_deck = state.tableau[tableau_position];

  if (tbl_deck.cards.size() == 0) {
    throw IllegalMoveException(
        "No cards in tableau to move to foundation.");
  }

  const Option<card_t> & foundation = state.foundation[foundation_position];
  const card_t & foundation_bound = tbl_deck.cards.back();

  if (!is_promote_to_foundation_legal(foundation, foundation_bound)) {
    throw IllegalMoveException(
        "Intended promotion from tableau to foundation is illegal");
  }

  std::pair<uint32_t, uint32_t> from = get_end_card_position(
      state, tableau_position);
  std::pair<uint32_t, uint32_t> to = std::make_pair(
      FOUNDATION_DECKS[foundation_position].first + CARD_WIDTH / 2 ,
      FOUNDATION_DECKS[foundation_position].second + CARD_HEIGHT / 2
  );
  drag_mouse(from, to);

  game_state_t next_state = state;
  next_state.foundation[foundation_position] = Option<card_t>(foundation_bound);
  unsafe_remove_card_from_tableau(&next_state, tableau_position);

  return next_state;

}

game_state_t move_from_column_to_column(
    const game_state_t & state,
    const tableau_position_t & position,
    const uint32_t destination
)
{
  const tableau_deck_t & src_deck = state.tableau[position.deck];
  const tableau_deck_t & dest_deck = state.tableau[destination];

  if (position.num_hidden != src_deck.num_down_cards) {
    throw InconsistentArgument();
  }

  if (position.position >= src_deck.cards.size()
      || !is_transfer_legal(src_deck.cards[position.position], dest_deck)) {
    throw IllegalMoveException(
        "Transfer from deck "
        + std::to_string(position.deck)
        + "."
        + std::to_string(position.position)
        + " to deck "
        + std::to_string(destination)
        + " is illegal"
    );
  }

  std::pair<uint32_t, uint32_t> from = std::make_pair(
    TABLEAU.first + (position.deck * TABLEAU_SIDE_OFFSET),
    TABLEAU.second
      + (position.num_hidden * TABLEAU_UNSEEN_OFFSET)
      + (position.position * TABLEAU_SEEN_OFFSET)
  );
  std::pair<uint32_t, uint32_t> to = get_end_card_position(
      state, destination);
  drag_mouse(from, to);

  game_state_t next_state = state;

  next_state.tableau[destination].cards.insert(
      next_state.tableau[destination].cards.end(),
      next_state.tableau[position.deck].cards.begin() + position.position,
      next_state.tableau[position.deck].cards.end()
  );

  /* unsafe_remove_card removes one card, so we should remove all but the
   * last one
   */
  next_state.tableau[position.deck].cards.erase(
      next_state.tableau[position.deck].cards.begin() + (position.position + 1),
      next_state.tableau[position.deck].cards.end());
  unsafe_remove_card_from_tableau(&next_state, position.deck);

  return next_state;
}
