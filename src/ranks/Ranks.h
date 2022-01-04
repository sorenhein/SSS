#ifndef SSS_RANKS_H
#define SSS_RANKS_H

#include <list>

#include "Declarer.h"
#include "Opponents.h"
#include "Completion.h"

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

    Completion completion;

    unsigned holding3;
    unsigned holding4;
    unsigned cards;
    unsigned char maxGlobalRank;


    void zero();

    void setPlayers();

    void setOwnRanks(CombReference& combRef) const;

    void setReference(CombReference& combRef) const;

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

    void addMinimal(
      const unsigned char absNumber,
      list<CombReference>& minimals,
      bool& ownFlag) const;


  public:

    Ranks();

    void resize(const unsigned cardsIn);

    unsigned size() const;

    void setRanks(
      const unsigned holding3In,
      CombEntry& combEntry);

    CombinationType setPlays(
      Plays& plays,
      Result& trivial);

    bool partnerVoid() const;

    bool getMinimals(
      const list<Result>& resultList,
      list<CombReference>& minimals) const;

    unsigned char maxRank() const;

    string strTable() const;

    wstring wstrDiagram() const;
};

#endif
