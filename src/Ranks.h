#ifndef SSS_RANKS_H
#define SSS_RANKS_H

#include <vector>

#include "Declarer.h"
#include "Opponents.h"
#include "Play.h"


using namespace std;

struct TrickEntry;
struct CombEntry;
class Plays;


class Ranks
{
  private:

    Declarer north;
    Declarer south;
    Opponents opps;

    unsigned holding;
    unsigned cards;
    unsigned maxRank;


    void setConstants();

    void zero();

    void setPlayers();

    unsigned canonicalTrinary(
      const Declarer& dominant,
      const Declarer& recessive) const;

    void canonicalBoth(
      const Declarer& dominant,
      const Declarer& recessive,
      unsigned& holding3,
      unsigned& holding2) const;

    void trivialRanked(
      const unsigned tricks,
      TrickEntry& trivialEntry) const;

    bool trivial(TrickEntry& trivialEntry) const;

    bool leadOK(
      const Declarer& leader,
      const Declarer& partner,
      const unsigned lead) const;

    bool pardOK(
      const Declarer& partner,
      const unsigned toBeat,
      const unsigned pard) const;

    void updateHoldings(
      const Declarer& leader,
      const Declarer& partner,
      const SidePosition side,
      unsigned& holding3,
      bool& rotateFlag) const;

    void logPlay(
      Plays& plays,
      const Declarer& leader,
      const Declarer& partner,
      const SidePosition side,
      Play& play) const;

    void setPlaysLeadWithVoid(
      Declarer& leader,
      Declarer& partner,
      const SidePosition side,
      const unsigned lead,
      Play& play,
      Plays& plays);

    void setPlaysLeadWithoutVoid(
      Declarer& leader,
      Declarer& partner,
      const SidePosition side,
      const unsigned lead,
      Play& play,
      Plays& plays);

    void setPlaysSide(
      Declarer& leader,
      Declarer& partner,
      const SidePosition side,
      Plays& plays);

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

    string strTable() const;

    wstring wstrDiagram() const;
};

#endif
