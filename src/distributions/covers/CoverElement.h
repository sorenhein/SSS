/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COVERELEMENT_H
#define SSS_COVERELEMENT_H

#include <iostream>
#include <iomanip>
#include <sstream>
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
  typedef unsigned char 
    (CoverElement::*ComparePtr)(const unsigned char valueIn) const;

  private: 

    unsigned char value1;
    unsigned char value2;

    CoverOperator oper;

    ComparePtr ptr;


    unsigned char lessEqual(const unsigned char valueIn) const
    {
      return (valueIn <= value1 ? 1 : 0);
    };

    unsigned char equal(const unsigned char valueIn) const
    {
      return (valueIn == value1 ? 1 : 0);
    };

    unsigned char greaterEqual(const unsigned char valueIn) const
    {
      return (valueIn >= value1 ? 1 : 0);
    };

    unsigned char insideRange(const unsigned char valueIn) const
    {
      return (valueIn >= value1 && valueIn <= value2 ? 1 : 0);
    };

    unsigned char outsideRange(const unsigned char valueIn) const
    {
      return (valueIn <= value1 && valueIn >= value2 ? 1 : 0);
    };


  public:

    // TODO Should be private once Covers is cleaner
    void setOperator(const CoverOperator operIn)
    {
      oper = operIn;
      if (oper == COVER_LESS_EQUAL)
        ptr = &CoverElement::lessEqual;
      else if (oper == COVER_EQUAL)
        ptr = &CoverElement::equal;
      else if (oper == COVER_GREATER_EQUAL)
        ptr = &CoverElement::greaterEqual;
      else if (oper == COVER_INSIDE_RANGE)
        ptr = &CoverElement::insideRange;
      else if (oper == COVER_OUTSIDE_RANGE)
        ptr = &CoverElement::outsideRange;
      else
        ptr = nullptr;
    };

    void setValue(const unsigned char valueIn)
    {
      value1 = valueIn;
      value2 = UCHAR_NOT_SET;
    };

    void setValues(
      const unsigned char value1In,
      const unsigned char value2In)
    {
      value1 = value1In;
      value2 = value2In;
    };

    void set(
      const unsigned char valueIn,
      const CoverOperator operIn)
    {
      value1 = valueIn;
      setOperator(operIn);
    };

    void set(
      const unsigned char value1In,
      const unsigned char value2In,
      const CoverOperator operIn)
    {
      value1 = value1In;
      value2 = value2In;
      setOperator(operIn);
    };

    unsigned char includes(const unsigned char valueIn) const
    {
      assert(ptr != nullptr);
      return (this->*ptr)(valueIn);
    };

    string str(const string& word) const
    {
      stringstream ss;

      if (oper == COVER_LESS_EQUAL)
        ss << "West has at most " << +value1 << " " << word;
      else if (oper == COVER_EQUAL)
        ss << "West has exactly " << +value1 << " " << word;
      else if (oper == COVER_GREATER_EQUAL)
        ss << "West has at least " << +value1 << " " << word;
      else if (oper == COVER_INSIDE_RANGE)
        ss << "West has " << word <<  " in range " << 
          +value1 << " to " << +value2 << " " << word << " inclusive";
      else if (oper == COVER_OUTSIDE_RANGE)
      {
        assert(value1 < value2);
        ss << "West has up to " << +value1 << " " <<
        " or at least " << +value2 << word << " inclusive";
      }
      else
        assert(false);

      return ss.str();
    };
};

#endif
