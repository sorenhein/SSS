/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_RANKSNAMES_H
#define SSS_RANKSNAMES_H

#include <deque>
#include <vector>

#include "RankNames.h"

using namespace std;

enum Side: unsigned;


/*
 * -H 5 -h 62: AQ/- missing KJT
 *
 * index  side  actFull actShort     absFull absShort  relShort
 *     0    EW jack-ten       JT honor-honor       HH        hh
 *     1    NS    queen        Q       queen        Q    (none)
 *     2    EW     king        K        king        K         H
 *     3    NS      ace        A         ace        A    (none)
 */


class RanksNames
{
  private:

    size_t runningIndex;
    size_t oppsRanks;
    Side sidePrev;

    deque<RankNames> names;

    // Here only the opponent tops count, so in the example: 0, 2
    vector<size_t> indexByTop;


    void reset();


  public:

    RanksNames();

    void setCards(const unsigned char cards);

    void add(const Side side);

    void finish();

    bool used() const;

    const RankNames& getOpponents(const unsigned topNumber) const;

    string strOpponents(
      const unsigned topNumber,
      const unsigned char count) const;

    string strOpponentsExpanded(
      const unsigned topNumber,
      const unsigned char count) const;

    string lowestCard(const unsigned topNumber) const;

    string str() const;

    string strMap() const;
};

#endif
