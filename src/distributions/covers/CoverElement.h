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
  COVER_LESS_EQUAL = 0,
  COVER_EQUAL = 1,
  COVER_GREATER_EQUAL = 2,
  COVER_INSIDE_RANGE = 3,
  COVER_OPERATOR_SIZE = 4,
  COVER_OUTSIDE_RANGE = 5
  // TODO Reorder later
};

class CoverElement
{
  typedef bool 
    (CoverElement::*ComparePtr)(const unsigned char valueIn) const;

  private: 


    ComparePtr ptr;


    bool equal(const unsigned char valueIn) const;
    
    bool insideRange(const unsigned char valueIn) const;


  public:

    // TODO private again later?
    unsigned char value1;
    unsigned char value2;

    CoverOperator oper;

    CoverElement();

    void reset();

    void setOperator(const CoverOperator operIn);

    void setValue(const unsigned char valueIn);

    void setValues(
      const unsigned char value1In,
      const unsigned char value2In);

    void set(
      const unsigned char valueIn,
      const CoverOperator operIn);

    void set(
      const unsigned char value1In,
      const unsigned char value2In,
      const CoverOperator operIn);

    bool includes(const unsigned char valueIn) const;

    string strRaw() const;

    string str(const string& word) const;
};

#endif
