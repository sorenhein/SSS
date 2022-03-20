/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

/*
   A Term contains a single set of constraints on either length or
   a specific top.  The Term applies either to West or East.
   The Term does not store information about the available number of
   cards or tops.
 */

#ifndef SSS_TERM_H
#define SSS_TERM_H

#include <string>

#include "TermHelp.h"

using namespace std;


struct CoverXes;
class Length;


class Term
{
  friend class CoverSet;
  friend class Product;

  protected: 

    unsigned char lower;
    unsigned char upper;
    CoverOperator oper;

    // The bit layout is given in TermCompare.
    unsigned short index;

  private:

    // The bit layout is given in TermCompare.
    unsigned char data;


    void strXes(
      const Term& top1,
      const unsigned char oppsLength,
      const unsigned char oppsTops1,
      CoverXes& coverXes) const;

    string strTop1Fixed0(
      const unsigned char oppsTops1,
      const string& side,
      const CoverXes& coverXes) const;

    string strTop1Fixed1(
      const unsigned char oppsTops1,
      const string& side,
      const CoverXes& coverXes) const;

    // The Term being called is the top1 one.
    string strTop1Fixed(
      const Length& length,
      const unsigned char oppsLength,
      const unsigned char oppsTops1) const;


  public:

    Term();

    void reset();

    void set(
      const unsigned char valueIn,
      const CoverOperator operIn);

    void set(
      const unsigned char value1In,
      const unsigned char value2In,
      const CoverOperator operIn);

    void setNew(
      const unsigned char lenActual,
      const unsigned char value1In,
      const unsigned char value2In);

    bool includes(const unsigned char valueIn) const;

    bool used() const;

    unsigned char getComplexity() const;

    unsigned char getRange() const;

    string strRaw() const;

    string strShort(const unsigned char lenActual) const;

    string strShort() const;

    string str(const string& word) const;
};

#endif
