#ifndef SSS_RANKS_H
#define SSS_RANKS_H

#include <vector>

#include "Player.h"

#include "strategies/Winner.h"
#include "strategies/Card.h"

#include "const.h"

using namespace std;

struct TrickEntry;
struct CombEntry;
class Plays;


class Ranks
{
  private:

    Player north;
    Player south;
    Player opps;

    unsigned holding;
    unsigned cards;
    unsigned maxRank;

    vector<string> namesNorth; // Indexed by rank
    vector<string> namesSouth;
    vector<string> namesOpps;

    vector<Card> cardsNorth; // Indexed by number
    vector<Card> cardsSouth;
    vector<Card> cardsOpps;


    void setConstants();

    void zero();

    void setRanks();

    void setNames();

    void setOrderTablesRemainder(
      Player& posInfo,
      const vector<string>& names);

    void setOrderTables();

    unsigned canonicalTrinary(
      const Player& dominant,
      const Player& recessive) const;
      // const vector<unsigned>& fullCount1,
      // const vector<unsigned>& fullCount2) const;

    void canonicalBoth(
      const Player& dominant,
      const Player& recessive,
      unsigned& holding3,
      unsigned& holding2) const;

    void trivialRanked(
      const unsigned tricks,
      const unsigned winRank,
      TrickEntry& trivialEntry) const;

    bool trivial(TrickEntry& trivialEntry) const;

    bool leadOK(
      const Player& leader,
      const Player& partner,
      const unsigned lead) const;

    bool pardOK(
      const Player& partner,
      const unsigned toBeat,
      const unsigned pard) const;

    void updateHoldings(
      const Player& leader,
      const Player& partner,
      const SidePosition side,
      unsigned& holding3,
      bool& rotateFlag) const;

    void logPlay(
      Plays& plays,
      const Player& leader,
      const Player& partner,
      const SidePosition side,
      const unsigned lead,
      const unsigned lho,
      const unsigned pard,
      const unsigned rho,
      const bool leadCollapse,
      const bool pardCollapse,
      const unsigned holding3,
      const bool rotateFlag) const;

    void setPlaysLeadWithVoid(
      Player& leader,
      Player& partner,
      const SidePosition side,
      const unsigned lead,
      const bool leadCollapse,
      Plays& plays);

    void setPlaysLeadWithoutVoid(
      Player& leader,
      Player& partner,
      const SidePosition side,
      const unsigned lead,
      const bool leadCollapse,
      Plays& plays);

    void setPlaysSide(
      Player& leader,
      Player& partner,
      const SidePosition side,
      Plays& plays);

    string strPosition(
      const string& cards,
      const string& player) const;

    wstring strPlays(
      const Player& posInfo,
      const vector<string>& names) const;

  public:

    Ranks();

    ~Ranks();

    void resize(const unsigned cardsIn);

    unsigned size() const;

    void set(
      const unsigned holdingIn,
      CombEntry& combEntry);

    CombinationType setPlays(
      Plays& plays,
      TrickEntry& trivialEntry);

    string str() const;

    wstring strDiagram() const;
};

#endif
