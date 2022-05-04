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

enum Opponent: unsigned;
enum CoverOperator: unsigned;
enum CompareType: unsigned;

using namespace std;


enum SymmTerm
{
  TERM_SYMMETRIZABLE = 0,
  TERM_NOT_SYMMETRIZABLE = 1,
  TERM_OPEN_CONSECUTIVE = 2,
  TERM_OPEN_CENTERED = 3
};


class Term
{
  friend class Top;
  friend class Length;

  protected: 

    // The bit layout is given in TermCompare.
    // It combines lower, upper and oper.
    unsigned short index;

  private:

    // The bit layout is given in TermCompare.
    // It combines usedFlag and complexity.
    unsigned char data;


    unsigned char lower() const;

    unsigned char upper() const;


  public:

    Term();

    void reset();

    void set(
      const unsigned char oppSize,
      const unsigned char lower,
      const unsigned char upper);

    bool includes(const unsigned char value) const;

    SymmTerm symmetrizable(const unsigned char maximum) const;

    Opponent simplestOpponent(const unsigned char maximum) const;

    CoverOperator getOperator() const;

    bool used() const;

    unsigned char complexity() const;

    CompareType presentOrder(const Term& term2) const;

    string strGeneral() const;
};

#endif
