#include <algorithm>
#include <memory>
#include <vector>
#include <exception>

#include "strategy.hpp"
#include "game.hpp"
#include "interact.hpp"

namespace {

/* TODO(fyquah): Globals? Ewwwwww. */
static bool glob_is_stock_pile_explored;
static std::vector<card_t> glob_stock_pile;

enum location_tag_t
{
  LOC_WASTE_PILE = 0,
  LOC_TABLEAU = 1,
  LOC_FOUNDATION = 2
};

class Location
{
public:
  virtual uint32_t index () = 0;
  virtual uint32_t sub_index () = 0;
  virtual location_tag_t tag () = 0;
};

class WastePile : public Location
{
public:
  uint32_t index ()
  {
    throw std::exception();
  }

  uint32_t sub_index ()
  {
    throw std::exception();
  }

  location_tag_t tag()
  {
    return LOC_WASTE_PILE;
  }
};

class Tableau : public Location
{
private:
  uint32_t sub_index_;
  uint32_t index_;

public:

  Tableau(uint32_t index_, uint32_t sub_index_)
    : index_(index_), sub_index_(sub_index_) {}

  uint32_t index()
  {
    return index_;
  }

  uint32_t sub_index()
  {
    return sub_index_;
  }

  location_tag_t tag()
  {
    return LOC_TABLEAU;
  }
};

class Foundation : public Location
{
private:
  uint32_t index_;

public:
  Foundation(uint32_t index_) : index_(index_) {}

  uint32_t index()
  {
    return index_;
  }

  uint32_t sub_index()
  {
    throw std::exception();
  }

  location_tag_t tag()
  {
    return LOC_FOUNDATION;
  }
};

class Move 
{
public:
  const std::shared_ptr<Location> from;
  const std::shared_ptr<Location> to;

  Move(
      const std::shared_ptr<Location> & from,
      const std::shared_ptr<Location> & to
  ) : from(from), to(to) {}
};

static bool possible_to_play_deuce(
    const game_state_t & state, const card_t & card)
{
  suite_t suite = card.suite;
  bool ret = 
    state.foundation[suite].is_some()
    && state.foundation[suite].get().number == ACE
    && card.number == DEUCE;

  if (ret) {
    std::cout
      << "Possible to promote "
      << card.to_string()
      << " to foundation"
      << std::endl;
  }

  return ret;
}

static inline std::shared_ptr<Location> loc_waste_pile(){
  return std::make_shared<WastePile>();
}

static inline std::shared_ptr<Location> loc_foundation(int index) {
  return std::make_shared<Foundation>(index);
}

static inline std::shared_ptr<Location> loc_tableau(int index, int subindex) {
  return std::make_shared<Tableau>(index, subindex);
}

static inline std::shared_ptr<Move> make_move(
    std::shared_ptr<Location> src,
    std::shared_ptr<Location> dest)
{
  return std::make_shared<Move>(src, dest);
}

/* Obvious moves are moves that strictly lead the game to a better
 * state.
 */
std::shared_ptr<Move> calculate_obvious_move(const game_state_t & state)
{
  std::vector<Move> ret;
  Option<card_t> waste_pile_top = state.waste_pile_top;
  const tableau_deck_t *tbl_deck = state.tableau;

  /* Rule 0: If there's an Ace play it. */
  if (waste_pile_top.is_some() && waste_pile_top.get().number == ACE) {
    const card_t & card = waste_pile_top.get();
    return std::make_shared<Move>(
      std::make_shared<WastePile>(),
      std::make_shared<Foundation>(card.suite));
  }
  
  for (int i = 0; i < 7 ; i++) {
    if (tbl_deck[i].cards.size() != 0
        && tbl_deck[i].cards.back().number == ACE) {
      const card_t & card = tbl_deck[i].cards.back();
      return std::make_shared<Move>(
        std::make_shared<Tableau>(
            i, tbl_deck[i].cards.size() - 1),
        std::make_shared<Foundation>(card.suite));
    }
  }

  /* Rule 1: Play deuce where possible */
  if (waste_pile_top.is_some()) {
    const card_t card = waste_pile_top.get();
    
    if (possible_to_play_deuce(state, card)) {
      return make_move(
          loc_waste_pile(),
          loc_foundation(uint32_t(card.suite)));
    }
  }
  
  for (int i = 0; i < 7 ; i++) {
    if (tbl_deck[i].cards.size() != 0) {
      const std::vector<card_t> & cards = tbl_deck[i].cards;
      const card_t & card = cards.back();

      if (possible_to_play_deuce(state, card)) {

        return make_move(
            loc_tableau(i, cards.size() - 1),
            loc_foundation(uint32_t(card.suite)));
      }
    }
  }

  /* Rule 2: If there is a move that frees a down card - play that move.
   * In case there are multiple such moves, play that one that frees the
   * one with the greatest number of hidden cards. (except for promoting
   * to foundation. That is a little risky)
   */
  std::vector<std::pair<uint32_t, std::pair<uint32_t, uint32_t>>>
    downcard_freeing_candidates;

  for (int i = 0 ; i < 7 ; i++) {
    /* This implies cards.size() != 0 */
    if (tbl_deck[i].num_down_cards == 0) {
      continue;
    }
    card_t src = tbl_deck[i].cards.at(0);

    for (int j = 0 ; j < 7 ; j++) {
      if (i == j) {
        continue;
      }
      card_t dest = tbl_deck[j].cards.back();

      if (src.number == dest.number- 1 &&
          suite_color(src.suite) != suite_color(dest.suite)) {
        downcard_freeing_candidates.push_back(
            std::make_pair(
              tbl_deck[i].num_down_cards,
              std::make_pair(i, j))
        );
      }
    }
  }

  if (downcard_freeing_candidates.size() != 0) {
    const auto & p = *std::max_element(
        downcard_freeing_candidates.begin(),
        downcard_freeing_candidates.end()
    );
    uint32_t src = p.second.first;
    uint32_t dest = p.second.second;

    std::cout << "Moving visible tableau from "
      << src << "("<< state.tableau[src].cards.back().to_string() << ")"
      << " -> "
      << dest << "(" << state.tableau[dest].cards.at(0).to_string() << ")"
      << " to release more hidden cards\n";

    return make_move(
        loc_tableau(src, 0),
        loc_tableau(dest, state.tableau[dest].cards.size() - 1)
    );
  }

  return std::shared_ptr<Move>(NULL);
}

static game_state_t perform_move(
    game_state_t state,
    std::shared_ptr<Move> move
)
{
  Location *from = move.get()->from.get();
  Location *to = move.get()->to.get();

  if (from->tag() == LOC_WASTE_PILE && to->tag() == LOC_TABLEAU) {
    return move_from_visible_pile_to_tableau(state, to->index());

  } else if (from->tag() == LOC_WASTE_PILE && to->tag() == LOC_FOUNDATION) {
    return move_from_visible_pile_to_foundation(state, to->index());

  } else if (from->tag() == LOC_TABLEAU && to->tag() == LOC_FOUNDATION) {
    return move_from_tableau_to_foundation(state, from->index(), to->index());

  } else if (from->tag() == LOC_TABLEAU && to->tag() == LOC_TABLEAU) {
    uint32_t src_deck = from->index();
    const tableau_position_t position = {
      .deck       = src_deck,
      .num_hidden = state.tableau[src_deck].num_down_cards,
      .position   = uint32_t(from->sub_index())
    };

    return move_from_column_to_column(state, position, to->index());
  }


  throw std::exception();
}

}

game_state_t strategy_init(const game_state_t & initial_state)
{
  /* Due to the way the game is scored, it is okay for us to shuffle through
   * the initial cards to learn what is in the deck.
   */
  game_state_t state = initial_state;

  glob_is_stock_pile_explored = true;

  for (int i = 0 ; i < 24 ; i++) {
    state = draw_from_stock_pile(state);
    glob_stock_pile.push_back(state.waste_pile_top.get());

    while (true) {
      std::shared_ptr<Move> move = calculate_obvious_move(state);

      if (move == NULL) {
        break;
      }

      state = perform_move(state, move);

      if (move.get()->from.get()->tag() == LOC_WASTE_PILE) {
        glob_stock_pile.pop_back();
      }
    }
  }

  /* Knowledge about state: */
  for (int i = 0 ; i < glob_stock_pile.size() ; i++) {
    std::cout << i << " = " << glob_stock_pile[i].to_string() << "\n";
  }

  /* This gets us to square one */
  return reset_stock_pile(state);
}

class FindException : public std::exception {};

static int find_stock_pile_position(const game_state_t & state)
{
  if (!state.waste_pile_top.is_some()) {
    return -1;
  }

  const card_t card = state.waste_pile_top.get();

  for (int i = 0 ; i < glob_stock_pile.size(); i++) {
    if (card == glob_stock_pile[i]) {
      return i;
    }
  }

  throw FindException();
}

static game_state_t enroute_to_obvious_by_peeking(
    const game_state_t & initial_state,
    bool *moved
)
{
  *moved = false;
  return initial_state;
}


game_state_t strategy_step(const game_state_t & start_state, bool *moved)
{
  /* Rule 0 to 2 (the base rules) are in the obvious_move function. */
  game_state_t state = start_state;
  std::shared_ptr<Move> move = calculate_obvious_move(state);

  if (move != NULL) {
    *moved = true;
    return perform_move(state, move);
  }

  /* Rule 3: If there is no obvious way to do rule 0 to 2, let's cheat
   * by looking at the stock_pile to try to do rule rule 0 to 2.
   */
  state = enroute_to_obvious_by_peeking(state, moved);
  if (*moved) {
    return state;
  }

  *moved = false;
  return state;
}
