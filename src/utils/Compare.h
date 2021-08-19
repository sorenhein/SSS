/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/


#ifndef SSS_COMPARE_H
#define SSS_COMPARE_H

enum CompareType
{
  WIN_FIRST = 0,
  WIN_SECOND = 1,
  WIN_EQUAL = 2,
  WIN_DIFFERENT = 3,
  WIN_UNSET = 4
};


class Compare
{
  private:
    
    CompareType val;

  public:
    
    Compare()
    {
    };

    Compare(const CompareType& cmp)
    {
      val = cmp;
    };

    CompareType value() const
    {
      return val;
    };

    Compare& operator = (const CompareType& cmp)
    {
      val = cmp;
      return * this;
    };

    bool operator == (const CompareType& cmp) const
    {
      return (val == cmp);
    };

    bool operator == (const Compare& cmp) const
    {
      return (val == cmp.val);
    };

    bool operator != (const CompareType& cmp) const
    {
      return (val != cmp);
    };
};

#endif
