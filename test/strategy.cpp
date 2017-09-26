#include <memory>
#include <vector>
#include <exception>

#include "strategy.hpp"
#include "game.hpp"
#include "interact.hpp"

namespace {

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
    && state.foundation[suite].get().number == ACE;

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
          loc_foundation(uint32_t(card.suite))
      );
    }
  }
  
  for (int i = 0; i < 7 ; i++) {
    if (tbl_deck[i].cards.size() != 0) {
      const std::vector<card_t> & cards = tbl_deck[i].cards;
      const card_t & card = cards.back();

      if (possible_to_play_deuce(state, card)) {

        return make_move(
            loc_tableau(i, cards.size() - 1),
            loc_foundation(uint32_t(card.suite))
        );
      }
    }
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

game_state_t strategy_step(const game_state_t & state, bool *moved)
{
  std::shared_ptr<Move> move = calculate_obvious_move(state);

  if (move == NULL) {
    *moved = false;
    return state;
  }

  *moved = true;
  return perform_move(state, move);
}
