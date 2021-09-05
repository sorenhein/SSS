/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_WINNERS_H
#define SSS_WINNERS_H

#include <list>
#include <cassert>

#include "Winner.h"

struct Play;
class Comparer;

using namespace std;


class Winners
{
  friend class Ranks;

  private:

    list<Winner> winners;


    void fillComparer(
      Comparer& comparer,
      const Winners& winners2) const;

    void operator += (const Winner& winner2);

    bool operator != (const Winners& winners2) const;

    bool rankExceeds(const Winners& winners2) const;

    void limitByRank();

    void consolidate();

    string strSingleSided(
      const string& name,
      const Card& winner) const;


  public:

    Winners();

    void reset();

    void setEmpty();

    void set(
      const Side sideIn,
      const Card& card);

    void set(
      const Card& north,
      const Card& south);

    bool empty() const;

    void operator += (const Winners& winners2);

    void operator *= (const Winners& winners2);

    bool operator == (const Winners& winners2) const;

    Compare compare(const Winners& w2) const;

    void flip();

    void update(const Play& play);

    string str() const;

    string strEntry() const;

    string strDebug() const;
};

#endif
