/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_TERM_H
#define SSS_TERM_H

#include <string>

using namespace std;


enum CoverOperator
{
  COVER_EQUAL = 0,
  COVER_INSIDE_RANGE = 1,
  COVER_OPERATOR_SIZE = 2
};

struct CoverXes;
class Length;


class Term
{
  friend class CoverSet;

  typedef bool 
    (Term::*ComparePtr)(const unsigned char valueIn) const;


  protected: 

    unsigned char value1;
    unsigned char value2;

    CoverOperator oper;

    bool symmFlag;


  private:

    ComparePtr ptr;

    bool usedFlag;

    unsigned char complexity;


    void setOperator(const CoverOperator operIn);

    bool equal(const unsigned char valueIn) const;
    
    bool insideRange(const unsigned char valueIn) const;

    // string strLengthEqual(const unsigned char lenActual) const;

    // string strLengthInside(const unsigned char lenActual) const;
 
    string strTop1Equal(const unsigned char oppsTops1) const;

    string strTop1Inside(const unsigned char oppsTops1) const;

    string strBothEqual0(
      const string& side) const;

    string strBothEqual1(
      const Length& length,
      const unsigned char oppsTops1,
      const string& side) const;

    string strBothEqual2(
      const Length& length,
      const unsigned char oppsLength,
      const unsigned char oppsTops1,
      const string& side) const;

    string strBothEqual3(
      const Length& length,
      const unsigned char oppsLength,
      const unsigned char oppsTops1,
      const string& side) const;

    void strXes(
      const Term& top1,
      const unsigned char oppsLength,
      const unsigned char oppsTops1,
      CoverXes& coverXes) const;

    string strTop1Fixed0(
      const Length& length,
      const unsigned char oppsLength,
      const unsigned char oppsTops1,
      const string& side,
      const CoverXes& coverXes) const;

    string strTop1Fixed1(
      const Length& length,
      const unsigned char oppsTops1,
      const string& side,
      const CoverXes& coverXes) const;

    // The Term being called is the top1 one.
    string strTop1Fixed(
      const Length& length,
      const unsigned char oppsLength,
      const unsigned char oppsTops1) const;

    // The Term being called is the top1 one.
    string strBothEqual(
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

    // string strLength(const unsigned char lenActual) const;

    string strTop1(const unsigned char oppsTops1) const;

    string strLengthTop1(
      // const Term& top1,
      const Length& length,
      const unsigned char oppsLength,
      const unsigned char oppsTops1) const;
};

#endif
