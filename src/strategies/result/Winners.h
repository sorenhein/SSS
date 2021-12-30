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
  private:

    list<Winner> winners;


    void fillComparer(
      Comparer& comparer,
      const Winners& winners2) const;

    bool operator != (const Winners& winners2) const;

    bool rankExceeds(const Winners& winners2) const;

    string strSingleSided(
      const string& name,
      const Card& winner) const;


  public:

    Winners();

    void reset();

    void addEmpty();

    void set(const Winner& winner);

    void set(
      const Side sideIn,
      const Card& card);

    void set(
      const Card& north,
      const Card& south);

    bool empty() const;

    void push_back(const Winner& winner);

    unsigned char rank() const;

    unsigned char absNumber() const;

    void operator += (const Winners& winners2);
    void operator += (const Winner& winner2);

    void operator *= (const Winners& winners2);
    void operator *= (const Winner& winner);

    bool operator == (const Winners& winners2) const;

    Compare compare(const Winners& w2) const;

    void flip();

    void expand(const char rankAdder);

    // TODO Should not be needed once Result uses a single Winner
    const Winner& front() const;
    unsigned size() const;

    string str() const;

    string strEntry() const;

    string strDebug() const;
};

#endif
