/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

/*
   A Term contains a single set of constraints on either length or
   a specific top.  The Term applies to West.
   The Term does not store information about the available number of
   cards or tops.
 */

#ifndef SSS_TERM_H
#define SSS_TERM_H

#include <string>

#include "TermHelp.h"

#include "../../utils/table.h"

using namespace std;


class Term
{
  // TODO Delete at some point
  friend class CoverSet;

  protected: 

    // TODO Avoid at some point (use index)?
    unsigned char lower;
    unsigned char upper;
    CoverOperator oper;

    // The bit layout is given in TermCompare.
    // It combines lower, upper and oper.
    unsigned short index;

  private:

    // The bit layout is given in TermCompare.
    // It combines usedFlag, complexity and range.
    unsigned char data;


  public:

    Term();

    void reset();

    void set(
      const unsigned char oppSize,
      const unsigned char lowerIn,
      const unsigned char upperIn);

    bool includes(const unsigned char value) const;

    Opponent simplestOpponent(const unsigned char maximum) const;

    CoverOperator getOperator() const;

    bool used() const;

    unsigned char complexity() const;

    unsigned char range() const;

    string strGeneral() const;
};

#endif
