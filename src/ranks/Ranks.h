#ifndef SSS_RANKS_H
#define SSS_RANKS_H

#include <list>

#include "Declarer.h"
#include "Opponents.h"

#include "../plays/Play.h"
#include "../utils/CombinationType.h"


using namespace std;

class Result;
struct CombEntry;
struct CombReference;
class Plays;


class Ranks
{
  private:

    Declarer north;
    Declarer south;
    Opponents opps;

    unsigned holding;
    unsigned cards;
    unsigned char maxGlobalRank;


    void setConstants();

    void zero();

    void setPlayers();

    unsigned canonicalTrinary(
      const Declarer& dominant,
      const Declarer& recessive) const;

    void canonicalBoth(
      const Declarer& dominant,
      const Declarer& recessive,
      const Opponents& opponents,
      unsigned& holding3,
      unsigned& holding2) const;

    void setRanks(CombReference& combRef) const;

    void trivialRanked(
      const unsigned char tricks,
      Result& trivial) const;

    bool makeTrivial(Result& trivial) const;

    bool leadOK(
      const Declarer& leader,
      const Declarer& partner,
      const unsigned char lead) const;

    bool pardOK(
      const Declarer& partner,
      const unsigned char toBeat,
      const unsigned char pard) const;

    void updateHoldings(Play& play) const;

    void finish(Play& play) const;

    void setPlaysLeadWithVoid(
      Declarer& leader,
      Declarer& partner,
      const unsigned char lead,
      Play& play,
      Plays& plays);

    void setPlaysLeadWithoutVoid(
      Declarer& leader,
      Declarer& partner,
      const unsigned char lead,
      Play& play,
      Plays& plays);

    void setPlaysSide(
      Declarer& leader,
      Declarer& partner,
      Play& play,
      Plays& plays);

    void remainingMinimal(
      const unsigned char criticalRank,
      unsigned char& index,
      Ranks& ranksNew) const;

    void finishMinimal(
      const unsigned holdingRef,
      list<CombReference>& minimals);

  public:

    Ranks();

    ~Ranks();

    void resize(const unsigned cardsIn);

    unsigned size() const;

    void setRanks(
      const unsigned holdingIn,
      CombEntry& combEntry);

    CombinationType setPlays(
      Plays& plays,
      Result& trivial);

    bool partnerVoid() const;

    bool getMinimals(
      const Result& result,
      list<CombReference>& minimals) const;

    string strTable() const;

    wstring wstrDiagram() const;
};

#endif
