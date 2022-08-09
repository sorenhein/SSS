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
 *     0    NS      ace        A         ace        A    (none)
 *     1    EW     king        K        king        K         H
 *     2    NS    queen        Q       queen        Q    (none)
 *     3    EW jack-ten       JT honor-honor       HH        hh
 */


class RanksNames
{
  private:

    size_t runningIndex;
    size_t oppsRanks;
    Side sidePrev;

    deque<RankNames> names;

    vector<RankNames const *> oppsPtrsByTop;


  public:

    RanksNames();

    void setCards(const unsigned char cards);

    void add(const Side side);

    void finish();

    bool used() const;

    const RankNames& getOpponents(const unsigned topNumber) const;

    string str() const;
};

#endif
