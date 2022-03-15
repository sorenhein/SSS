/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Term.h"
#include "Length.h"

#include "../../const.h"


struct CoverXes
{
  unsigned char westMax, westMin;
  unsigned char eastMax, eastMin;
  string strWest, strEast;

  string str() const
  {
    stringstream ss;

    ss << "coverXes: " <<
      +westMin << "-" << +westMax << ", " <<
      +eastMin << "-" << +eastMax << ", " <<
      strWest << ", " << strEast << "\n";

    return ss.str();
  };
};



Term::Term()
{
  Term::reset();
}


void Term::reset()
{
  lower = UCHAR_NOT_SET;
  upper = UCHAR_NOT_SET;
  oper = COVER_OPERATOR_SIZE;
  ptr = nullptr;
  symmFlag = false; // Not used (yet?)
  usedFlag = false;
  complexity = 0;
}


void Term::setOperator(const CoverOperator operIn)
{
  oper = operIn;
  if (oper == COVER_EQUAL)
    ptr = &Term::equal;
  else if (oper == COVER_INSIDE_RANGE)
    ptr = &Term::insideRange;
  else
    ptr = nullptr;
}


void Term::set(
  const unsigned char valueIn,
  const CoverOperator operIn)
{
  lower = valueIn;
  Term::setOperator(operIn);
  usedFlag = true;
}

void Term::set(
  const unsigned char lower1In,
  const unsigned char upperIn,
  const CoverOperator operIn)
{
  lower = lower1In;
  upper = upperIn;
  Term::setOperator(operIn);
  usedFlag = true;
}


void Term::setNew(
  const unsigned char lenActual,
  const unsigned char lowerIn,
  const unsigned char upperIn)
{
  // Returns true if actually in non-trivial use
  if (lowerIn == 0 && upperIn == lenActual)
  {
    usedFlag = false;
    complexity = 0;
    return;
  }

  Term::setOperator(lowerIn == upperIn ?
    COVER_EQUAL : COVER_INSIDE_RANGE);

  lower = lowerIn;
  upper = upperIn;
  usedFlag = true;

  if (lowerIn == 0 || upper == lenActual ||lowerIn == upperIn)
    complexity = 1;
  else
    complexity = 2;
}


bool Term::equal(const unsigned char valueIn) const
{
  return (valueIn == lower ? 1 : 0);
}


bool Term::insideRange(const unsigned char valueIn) const
{
  return (valueIn >= lower && valueIn <= upper ? 1 : 0);
}


bool Term::includes(const unsigned char valueIn) const
{
  assert(ptr != nullptr);
  return (this->*ptr)(valueIn);
}


bool Term::used() const
{
  return usedFlag;
}


unsigned char Term::getComplexity() const
{
  return complexity;
}


unsigned char Term::getRange() const
{
  if (! Term::used())
    return 0;
  else
    return (upper - lower);

}


string Term::strTop1Equal(const unsigned char oppsTops1) const
{
  stringstream ss;
  const string side = (symmFlag ? "Either opponent" : "West");
  const unsigned char wtop = lower;

  if (wtop == 0)
  {
    assert(! symmFlag);
    if (oppsTops1 == 1)
      ss << "East has the top";
    else
      ss << "East has the tops";
  }
  else if (wtop == oppsTops1)
  {
    if (oppsTops1 == 1)
      ss << side << " has the top";
    else
      ss << side << " has the tops";
  }
  else if (wtop == 1)
  {
    if (oppsTops1 == 1)
      ss << side << " has the top";
    else
      ss << side << " has exactly one top";
  }
  else if (wtop == oppsTops1-1)
  {
    assert(! symmFlag);
    ss << "East has exactly one top";
  }
  else if (wtop == 2)
  {
    ss << side << " has " <<
      (oppsTops1 == 2 ? "both" : "exactly two") << " tops";
  }
  else
    ss << side << " has exactly " << wtop << " tops";

  return ss.str();
}


string Term::strTop1Inside(const unsigned char oppsTops1) const
{
  stringstream ss;
  const string side = (symmFlag ? "Either opponent" : "West");
  const unsigned char wtop1 = lower;
  const unsigned char wtop2 = upper;

  if (wtop1 == 0)
  {
    if (wtop2 == oppsTops1-1)
    {
      assert(! symmFlag);
      ss << "East has at least one top";
    }
    else
      ss << side << " has at most " << +wtop2 << " tops";
  }
  else if (wtop2 == oppsTops1)
  {
    if (wtop1 == 1)
      ss << side << " has at least one top";
    else
      ss << side << " has at least " << +wtop1 << " tops";
  }
  else
  {
      ss << side <<
        " has between " << +wtop1 << " and " << +wtop2 << " tops";
  }

  return ss.str();
}


string Term::strBothEqual0(
  const string& side) const
{
  if (lower == 0)
    return side + " is void";
  else
    return "East is void";
}


string Term::strBothEqual1(
  const Length& length,
  const unsigned char oppsTops1,
  const string& side) const
{
  stringstream ss;

  // TODO Could maybe switch around and call the top1 one here,
  // with length.lower as an argument.  That is all that is needed.
  if (length.lower == 1)
  {
    if (lower == 0)
      ss << side << " has a small singleton";
    else
      ss <<  side << " has " << (oppsTops1 == 1 ? "the" : "a") << " " <<
        "singleton honor";
  }
  else
  {
    assert(! length.symmFlag);
    if (lower == oppsTops1)
      ss << "East has a small singleton";
    else
      ss << "East has " << (oppsTops1 == 1 ? "the" : "a") << " " <<
        "singleton honor";
  }

  return ss.str();
}


string Term::strBothEqual2(
  const Length& length,
  const unsigned char oppsLength,
  const unsigned char oppsTops1,
  const string& side) const
{
  stringstream ss;

  if (length.lower == 2)
  {
    if (lower == 0)
    {
      if (oppsLength == 4 && oppsTops1 == 2)
      {
        assert(! length.symmFlag);
        ss << "East has doubleton honors (HH)";
      }
      else
        ss << side << " has a small doubleton";
    }
    else if (lower == 1)
      ss << side << " has " << (oppsTops1 == 1 ? "the" : "an") << " " <<
        "honor doubleton (Hx)";
    else
      ss << side << " has doubleton honors (HH)";
  }
  else
  {
    assert(! length.symmFlag);
    if (lower == oppsTops1)
      ss << "East has a small doubleton";
    else if (lower + 1 == oppsTops1)
      ss << "East has " << (oppsTops1 == 1 ? "the" : "an") << " " <<
        "honor doubleton (Hx)";
    else
      ss << "East has doubleton honors (HH)";
  }

  return ss.str();
}


string Term::strBothEqual3(
  const Length& length,
  [[maybe_unused]] const unsigned char oppsLength,
  const unsigned char oppsTops1,
  const string& side) const
{
  stringstream ss;

  if (length.lower == 3)
  {
    if (lower == 0)
      ss << side << " has a small tripleton";
    else if (lower == 1)
      ss << side << " has " << (oppsTops1 == 1 ? "the" : "an") << " " <<
        "honor tripleton (Hxx)";
    else if (lower == 2)
      ss << side << " has " << (oppsTops1 == 2 ? "the" : "two") << " " <<
        "honors tripleton (HHx)";
    else
      ss << side << " has tripleton honors (HHH)";
  }
  else
  {
    assert(! length.symmFlag);
    if (lower == oppsTops1)
      ss << "East has a small tripleton";
    else if (lower + 1 == oppsTops1)
      ss << "East has " << (oppsTops1 == 1 ? "the" : "an") << " " <<
        "honor tripleton (Hxx)";
    else if (lower + 2 == oppsTops1)
      ss << "East has " << (oppsTops1 == 2 ? "the" : "two") << " " <<
        "honors tripleton (HHx)";
    else
      ss << "East has tripleton honors (HHH)";
  }

  return ss.str();
}


string Term::strRaw() const
{
  stringstream ss;

  ss << +lower << " to " << +upper << ", oper ";
  if (oper == COVER_EQUAL)
    ss << "EQUAL";
  else if (oper == COVER_INSIDE_RANGE)
    ss << "INSIDE";
  else
    ss << "UNKNOWN";
  ss << "\n";

  return ss.str();
}


string Term::strShort(const unsigned char lenActual) const
{
  stringstream ss;

  if (usedFlag)
  {
    string s;
    if (lower == upper)
      s = "== " + to_string(+lower);
    else if (lower == 0)
      s = "<= " + to_string(+upper);
    else if (upper == lenActual)
      s = ">= " + to_string(+lower);
    else
      s = to_string(+lower) + "-" + to_string(+upper);
    
    ss << setw(8) << s;
  }
  else
  {
    ss << setw(8) << "unused";
  }

  return ss.str();
}


string Term::strShort() const
{
  stringstream ss;

  if (usedFlag)
  {
    string s;
    if (lower == upper)
      s = "== " + to_string(+lower);
    else if (lower == 0)
      s = "<= " + to_string(+upper);
    else
      s = to_string(+lower) + "-" + to_string(+upper);
    
    ss << setw(8) << s;
  }
  else
  {
    ss << setw(8) << "unused";
  }

  return ss.str();
}


string Term::str(const string& word) const
{
  stringstream ss;

  if (oper == COVER_EQUAL)
    ss << "West has exactly " << +lower << " " << word;
  else if (oper == COVER_INSIDE_RANGE)
    ss << "West has " << word <<  " in range " << 
      +lower << " to " << +upper << " " << word << " inclusive";
  else
    assert(false);

  return ss.str();
}


/*
string Term::strLength(const unsigned char lenActual) const
{
  if (oper == COVER_EQUAL)
    return Term::strLengthEqual(lenActual);
  else if (oper == COVER_INSIDE_RANGE)
    return Term::strLengthInside(lenActual);
  else
  {
    assert(false);
    return "";
  }
}
*/


string Term::strTop1(const unsigned char oppsTops1) const
{
  if (oper == COVER_EQUAL)
    return Term::strTop1Equal(oppsTops1);
  else if (oper == COVER_INSIDE_RANGE)
    return Term::strTop1Inside(oppsTops1);
  else
  {
    assert(false);
    return "";
  }
}


string Term::strBothEqual(
  const Length& length,
  const unsigned char oppsLength,
  const unsigned char oppsTops1) const
{
  const string side = (length.symmFlag ? "Either opponent" : "West");

  if (length.lower == 0 || length.lower == oppsLength)
    return length.strBothEqual0(side);
  else if (length.lower == 1 || length.lower + 1 == oppsLength)
    return Term::strBothEqual1(length, oppsTops1, side);
  else if (length.lower == 2 || length.lower + 2 == oppsLength)
    return Term::strBothEqual2(length, oppsLength, oppsTops1, side);
  else if (length.lower == 3 || length.lower + 3 == oppsLength)
    return Term::strBothEqual3(length, oppsLength, oppsTops1, side);
  else
  {
    assert(false);
    return "";
  }
}


void Term::strXes(
  const Term& top1,
  const unsigned char oppsLength,
  const unsigned char oppsTops1,
  CoverXes& coverXes) const
{
  coverXes.westMax = upper - top1.lower;
  coverXes.westMin = lower - top1.lower;

  coverXes.eastMax =
    (oppsLength - lower) - (oppsTops1 - top1.lower);
  coverXes.eastMin =
    (oppsLength - upper) - (oppsTops1 - top1.lower);

  coverXes.strWest = string(coverXes.westMin, 'x') +
    "(" + string(coverXes.westMax - coverXes.westMin, 'x') + ")";
  coverXes.strEast = string(coverXes.eastMin, 'x') +
    "(" + string(coverXes.eastMax - coverXes.eastMin, 'x') + ")";
}


string Term::strTop1Fixed0(
  const Length& length,
  const unsigned char oppsLength,
  const unsigned char oppsTops1,
  const string& side,
  const CoverXes& coverXes) const
{
  stringstream ss;

  if (lower == 0)
  {
    assert(! length.symmFlag);
    if (oppsTops1 == 1)
    {
      if (coverXes.eastMax == 1)
        ss << "East has the top at most doubleton";
      else
        ss << "East has H" << coverXes.strEast;
    }
    else if (oppsTops1 == 2)
    {
      if (coverXes.eastMax == 1)
        ss << "East has both tops at most tripleton";
      else
        ss << "East has HH" << coverXes.strEast;
    }
    else
      assert(false);
  }
  else
  {
    if (oppsTops1 == 1)
    {
      if (coverXes.westMax == 1)
        ss << side << " has the top at most doubleton";
      else
        ss << side << " has H" << coverXes.strWest;
    }
    else if (oppsTops1 == 2)
    {
      if (coverXes.westMax == 1)
        ss << side << " has both tops at most tripleton";
      else
        ss << side << " has HH" << coverXes.strWest;
    }
    else if (oppsTops1 == 3)
      ss << side << " has HHH" << coverXes.strWest;
    else
    {
      cout << coverXes.str();
      cout << length.str(oppsLength) << ", and " <<
        Term::strTop1(oppsTops1) << endl;
        // Term::strTop1(oppsTops1) << endl;
      assert(false);
    }
  }
  
  return ss.str();
}


string Term::strTop1Fixed1(
  const Length& length,
  const unsigned char oppsTops1,
  const string& side,
  const CoverXes& coverXes) const
{
  // TODO Call top1?
  stringstream ss;

  if (lower == 1)
  {
    if (oppsTops1 == 2)
    {
      // Look at it from the shorter side
      if (coverXes.westMax <= coverXes.eastMax)
      {
        if (coverXes.westMax == 1)
          ss << side << " has one top at most doubleton";
        else
          ss << side << " has H" << coverXes.strWest;
      }
      else
      {
        assert(! length.symmFlag);
        if (coverXes.eastMax == 1)
          ss << "East has one top at most doubleton";
        else
          ss << "East has H" << coverXes.strEast;
      }
    }
    else
    {
      if (coverXes.westMax == 1)
        ss << side << " has one top at most doubleton";
      else
        ss << side << " has H" << coverXes.strWest;
    }
  }
  else
  {
    assert(! length.symmFlag);
    if (coverXes.eastMax == 1)
      ss << "East has one top at most doubleton";
    else
      ss << "East has H" << coverXes.strEast;
  }

  return ss.str();
}


string Term::strTop1Fixed(
  const Length& length,
  const unsigned char oppsLength,
  const unsigned char oppsTops1) const
{
  stringstream ss;
  const string side = (length.symmFlag ? "Either opponent" : "West");

  CoverXes coverXes;
  length.strXes(* this, oppsLength, oppsTops1, coverXes);

  if (lower == 0 || lower == oppsTops1)
  {
    return Term::strTop1Fixed0(length, oppsLength, oppsTops1, side, coverXes);
  }
  else if (lower == 1 || lower + 1 == oppsTops1)
    return Term::strTop1Fixed1(length, oppsTops1, side, coverXes);
  else
  {
    assert(false);
    return "";
  }
}


string Term::strLengthTop1(
  // const Term& top1,
  const Length& length,
  const unsigned char oppsLength,
  const unsigned char oppsTops1) const
{
  // We are a top1.
  if (length.oper == COVER_EQUAL)
  {
    if (oper == COVER_EQUAL)
      return Term::strBothEqual(length, oppsLength, oppsTops1);
    else
      return length.str(oppsLength) + ", and " +
        Term::strTop1(oppsTops1);
  }
  else
  {
    if (oper == COVER_EQUAL)
      return Term::strTop1Fixed(length, oppsLength, oppsTops1);
    else
      return length.str(oppsLength) + ", and " +
        Term::strTop1(oppsTops1);
  }
}

