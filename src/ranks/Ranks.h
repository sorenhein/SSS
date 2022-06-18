/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_RANKS_H
#define SSS_RANKS_H

#include <list>

#include "Declarer.h"
#include "Opponents.h"
#include "Completion.h"
#include "RanksNames.h"

#include "../plays/Play.h"
#include "../utils/CombinationType.h"


using namespace std;

class Result;
class CombEntry;
class Plays;


class Ranks
{
  private:

    Declarer north;
    Declarer south;
    Opponents opps;

    Completion completion;

    size_t holding3;
    unsigned holding4;
    unsigned char cards;
    unsigned char maxGlobalRank;

    RanksNames ranksNames;


    void zero();

    void setPlayers();

    void setReference(CombEntry& centry) const;

    void trivialRanked(
      const unsigned char tricks,
      Result& trivial) const;

    bool makeTrivial(Result& trivial) const;

    bool sideOKRanked(
      const Declarer& leader,
      const Declarer& partner) const;

    bool sideOKUnranked(
      const Declarer& leader,
      const Declarer& partner,
      const Play& play) const;

    bool sideOK(
      const Declarer& leader,
      const Declarer& partner,
      const Play& play) const;

    bool leadOKRanked(
      const Declarer& leader,
      const Declarer& partner,
      Card const * leadPtr) const;

    bool leadOKUnranked(
      const Declarer& leader,
      const Declarer& partner,
      const unsigned char lead) const;

    bool leadOK(
      const Declarer& leader,
      const Declarer& partner,
      Card const * leadPtr,
      const unsigned char lead) const;

    bool pardOK(
      const Declarer& partner,
      Card const * pardPtr,
      const unsigned char toBeat,
      const unsigned char pard) const;

    bool rhoOK(
      const unsigned toBeat,
      const unsigned rho) const;

    void updateHoldings(Play& play) const;

    void finish(Play& play) const;

    void setPlaysLeadLHOVoid(
      Declarer& leader,
      Declarer& partner,
      const unsigned char lead,
      Play& play,
      Plays& plays,
      const bool symmOnlyFlag);

    void setPlaysLeadLHONotVoid(
      Declarer& leader,
      Declarer& partner,
      const unsigned char lead,
      Play& play,
      Plays& plays,
      const bool symmOnlyFlag);

    void setPlaysSide(
      Declarer& leader,
      Declarer& partner,
      Play& play,
      Plays& plays,
      const bool symmOnlyFlag);

    void addMinimal(
      const unsigned char absNumber,
      CombEntry& centry,
      bool& ownFlag) const;


  public:

    Ranks();

    void resize(const unsigned char cardsIn);

    unsigned char size() const;

    void setRanks(
      const size_t holding3In,
      CombEntry& combEntry);

    CombinationType setPlays(
      Plays& plays,
      Result& trivial,
      const bool symmOnlyFlag);

    const RanksNames& getRanksNames() const;

    bool partnerVoid() const;

    bool getMinimals(
      const list<Result>& resultList,
      CombEntry& centry) const;

    unsigned char maxRank() const;

    string strTable() const;

    wstring wstrDiagram() const;
};

#endif
