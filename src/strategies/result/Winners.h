/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_WINNERS_H
#define SSS_WINNERS_H

#include <list>

#include "Winner.h"

class Comparer;

using namespace std;


class Winners
{
  private:

    list<Winner> winners;

    // There is a subtle difference between an empty winners list
    // (a) that is unset (setFlag == false) and (b) that is set to
    // be empty, i.e. to have no declarer constraints (setFlag == true).

    bool setFlag;


    void fillComparer(
      Comparer& comparer,
      const Winners& winners2) const;

    void addCore(const Winner& winner);


  public:

    Winners();

    void reset();

    void set(const Winner& winner);

    void set(
      const Card& north,
      const Card& south);

    bool empty() const;

    void operator += (const Winners& winners2);
    void operator += (const Winner& winner2);

    void operator *= (const Winners& winners2);
    void operator *= (const Winner& winner);

    bool operator == (const Winners& winners2) const;
    bool operator != (const Winners& winners2) const;

    Compare compare(const Winners& w2) const;

    void flip();

    void expand(const char rankAdder);

    unsigned char rank() const;

    unsigned char absNumber() const;

    const Winner& constantWinner() const;

    string strEntry() const;

    string strDebug() const;
};

#endif
