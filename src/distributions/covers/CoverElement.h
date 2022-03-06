/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COVERELEMENT_H
#define SSS_COVERELEMENT_H

#include <string>

#include "../../const.h"


enum CoverOperator
{
  COVER_EQUAL = 0,
  COVER_INSIDE_RANGE = 1,
  COVER_OPERATOR_SIZE = 2
};

struct CoverXes;

/*
struct CoverXes
{
  unsigned char westMax, westMin;
  unsigned char eastMax, eastMin;
  string strWest, strEast;

  string str() const
  {
    stringstream ss;

    ss << "coverXes: " <<
      westMin << "-" << westMax << ", " <<
      eastMin << "-" << eastMax << ", " <<
      strWest << ", " << strEast << "\n";

    return ss.str();
  };
};
*/


class CoverElement
{
  friend class CoverSet;

  typedef bool 
    (CoverElement::*ComparePtr)(const unsigned char valueIn) const;

  private: 

    unsigned char value1;
    unsigned char value2;

    CoverOperator oper;

    ComparePtr ptr;

    bool symmFlag;
    bool usedFlag;

    unsigned char complexity;


    void setOperator(const CoverOperator operIn);

    bool equal(const unsigned char valueIn) const;
    
    bool insideRange(const unsigned char valueIn) const;

    string strLengthEqual(const unsigned char lenActual) const;

    string strLengthInside(const unsigned char lenActual) const;
 
    string strTop1Equal(const unsigned char oppsTops1) const;

    string strTop1Inside(const unsigned char oppsTops1) const;

    string strBothEqual0(
      const string& side) const;

    string strBothEqual1(
      const CoverElement& top1,
      const unsigned char oppsTops1,
      const string& side) const;

    string strBothEqual2(
      const CoverElement& top1,
      const unsigned char oppsLength,
      const unsigned char oppsTops1,
      const string& side) const;

    string strBothEqual3(
      const CoverElement& top1,
      const unsigned char oppsLength,
      const unsigned char oppsTops1,
      const string& side) const;

    void strXes(
      const CoverElement& top1,
      const unsigned char oppsLength,
      const unsigned char oppsTops1,
      CoverXes& coverXes) const;

    string strTop1Fixed0(
      const CoverElement& top1,
      const unsigned char oppsLength,
      const unsigned char oppsTops1,
      const string& side,
      const CoverXes& coverXes) const;

    string strTop1Fixed1(
      const CoverElement& top1,
      const unsigned char oppsTops1,
      const string& side,
      const CoverXes& coverXes) const;

    string strTop1Fixed(
      const CoverElement& top1,
      const unsigned char oppsLength,
      const unsigned char oppsTops1) const;

    // The CoverElement being called is the length one.
    string strBothEqual(
      const CoverElement& top1,
      const unsigned char oppsLength,
      const unsigned char oppsTops1) const;


  public:

    CoverElement();

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

    string strRaw() const;

    string strShort(const unsigned char lenActual) const;

    string strShort() const;

    string str(const string& word) const;

    string strLength(const unsigned char lenActual) const;

    string strTop1(const unsigned char oppsTops1) const;

    string strLengthTop1(
      const CoverElement& top1,
      const unsigned char oppsLength,
      const unsigned char oppsTops1) const;
};

#endif
