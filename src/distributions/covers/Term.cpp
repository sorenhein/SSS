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
#include "TermCompare.h"
#include "Length.h"

#include "../../const.h"

extern TermCompare termCompare;


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

  // Hope to break the index if we try to use it unset.
  index = numeric_limits<unsigned short>::max();
  data = termCompare.getData(false, 0, 0);
}


void Term::set(
  const unsigned char valueIn,
  const CoverOperator operIn)
{
  lower = valueIn;
  upper = valueIn; // Just to have something
  oper = operIn;

  index = termCompare.getIndex(lower, upper, oper);
  data = termCompare.getData(true, upper-lower, 0);
}

void Term::set(
  const unsigned char lowerIn,
  const unsigned char upperIn,
  const CoverOperator operIn)
{
  lower = lowerIn;
  upper = upperIn;
  oper = operIn;

  index = termCompare.getIndex(lower, upper, oper);
  data = termCompare.getData(true, upper-lower, 0);
}


void Term::setNew(
  const unsigned char lenActual,
  const unsigned char lowerIn,
  const unsigned char upperIn)
{
  if (lowerIn == 0 && upperIn == lenActual)
  {
    // Not set
    data = termCompare.getData(false, 0, 0);
    return;
  }

  lower = lowerIn;
  upper = upperIn;
  unsigned char complexity;

  if (lower == upper)
  {
    oper = COVER_EQUAL;
    complexity = 1;
  }
  else if (lowerIn == 0)
  {
    oper = COVER_LESS_EQUAL;
    complexity = 1;
  }
  else if (upperIn == lenActual)
  {
    oper = COVER_GREATER_EQUAL;
    complexity = 1;
  }
  else
  {
    oper = COVER_INSIDE_RANGE;
    complexity = 2;
  }

  index = termCompare.getIndex(lower, upper, oper);
  data = termCompare.getData(true, upper-lower, complexity);
}


bool Term::includes(const unsigned char valueIn) const
{
  return termCompare.includes(index, valueIn);
}


bool Term::used() const
{
  return termCompare.used(data);
}


unsigned char Term::getComplexity() const
{
  return termCompare.complexity(data);
}


unsigned char Term::getRange() const
{
  return termCompare.range(data);
}


string Term::strBothEqual0(
  const string& side) const
{
assert(false);

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

  // if (usedFlag)
  if (Term::used())
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

  if (Term::used())
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


string Term::strBothEqual(
  const Length& length,
  const unsigned char oppsLength,
  const unsigned char oppsTops1) const
{
  // const string side = (length.symmFlag ? "Either opponent" : "West");
  const string side = "West";

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
  const unsigned char oppsTops1,
  const string& side,
  const CoverXes& coverXes) const
{
  stringstream ss;

  if (lower == 0)
  {
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
      // cout << coverXes.str();
      // cout << length.str(oppsLength) << ", and " <<
        // Term::strTop1(oppsTops1) << endl;
      assert(false);
    }
  }
  
  return ss.str();
}


string Term::strTop1Fixed1(
  // const Length& length,
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
  // const string side = (length.symmFlag ? "Either opponent" : "West");
  const string side = "West";

  CoverXes coverXes;
  length.strXes(* this, oppsLength, oppsTops1, coverXes);

  if (lower == 0 || lower == oppsTops1)
  {
    return Term::strTop1Fixed0(oppsTops1, side, coverXes);
  }
  else if (lower == 1 || lower + 1 == oppsTops1)
    return Term::strTop1Fixed1(oppsTops1, side, coverXes);
  else
  {
    assert(false);
    return "";
  }
}

