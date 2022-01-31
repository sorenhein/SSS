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


    void setOperator(const CoverOperator operIn);

    bool equal(const unsigned char valueIn) const;
    
    bool insideRange(const unsigned char valueIn) const;


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

    bool includes(const unsigned char valueIn) const;

    string strRaw() const;

    string str(const string& word) const;
};

#endif
