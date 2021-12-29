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

    Compare& operator = (const CompareType& cmp)
    {
      val = cmp;
      return * this;
    };

    CompareType value() const
    {
      return val;
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

    void operator *= (const Compare cmp)
    {
      if (cmp.val == WIN_EQUAL || cmp.val == val)
        return;
      else if (val == WIN_EQUAL)
        val = cmp.val;
      else
        val = WIN_DIFFERENT;
    };
};

enum CompareDetail
{
  WIN_NEUTRAL_OVERALL = 0x0,
  WIN_EQUAL_OVERALL = 0x1,
  WIN_FIRST_PRIMARY = 0x2,
  WIN_SECOND_PRIMARY = 0x4,
  WIN_FIRST_SECONDARY = 0x8,
  WIN_SECOND_SECONDARY = 0x10,
  WIN_DIFFERENT_PRIMARY = 0x20,
  WIN_DIFFERENT_SECONDARY = 0x40
};

#define WIN_FIRST_OVERALL 0xa
#define WIN_SECOND_OVERALL 0x14
#define WIN_PRIMARY (WIN_FIRST_PRIMARY | WIN_SECOND_PRIMARY | \
  WIN_DIFFERENT_PRIMARY)

#endif
