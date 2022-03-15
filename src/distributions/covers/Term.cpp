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
  value1 = UCHAR_NOT_SET;
  value2 = UCHAR_NOT_SET;
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
  value1 = valueIn;
  Term::setOperator(operIn);
  usedFlag = true;
}

void Term::set(
  const unsigned char value1In,
  const unsigned char value2In,
  const CoverOperator operIn)
{
  value1 = value1In;
  value2 = value2In;
  Term::setOperator(operIn);
  usedFlag = true;
}


void Term::setNew(
  const unsigned char lenActual,
  const unsigned char value1In,
  const unsigned char value2In)
{
  // Returns true if actually in non-trivial use
  if (value1In == 0 && value2In == lenActual)
  {
    usedFlag = false;
    complexity = 0;
    return;
  }

  Term::setOperator(value1In == value2In ?
    COVER_EQUAL : COVER_INSIDE_RANGE);

  value1 = value1In;
  value2 = value2In;
  usedFlag = true;

  if (value1In == 0 || value2 == lenActual ||value1In == value2In)
    complexity = 1;
  else
    complexity = 2;
}


bool Term::equal(const unsigned char valueIn) const
{
  return (valueIn == value1 ? 1 : 0);
}


bool Term::insideRange(const unsigned char valueIn) const
{
  return (valueIn >= value1 && valueIn <= value2 ? 1 : 0);
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
    return (value2 - value1);

}


/*
string Term::strLengthEqual(const unsigned char lenActual) const
{
  stringstream ss;
  const string side = (symmFlag ? "Either opponent" : "West");

  if (value1 == 0)
    ss << side << " is void";
  else if (value1 == lenActual)
  {
    assert(! symmFlag);
    ss << "East is void";
  }
  else if (value1 == 1)
    ss << side << " has a singleton";
  else if (value1 == lenActual-1)
  {
    assert(! symmFlag);
    ss << "East has a singleton";
  }
  else if (value1 == 2)
  {
    if (lenActual > 4)
      ss << side << " has a doubleton";
    else
      ss << "The suit splits 2=2";
  }
  else
    ss << "The suit splits " << +value1 << "=" << +(lenActual - value1);

  return ss.str();
}



string Term::strLengthInside(const unsigned char lenActual) const
{
  stringstream ss;
  const string side = (symmFlag ? "Either opponent" : "West");

  if (value1 == 0)
  {
    if (value2 == 1)
      ss << side << " has at most a singleton";
    else if (value2 == 2)
      ss << side << " has at most a doubleton";
    else
      ss << side << " has at most " << +value2 << " cards";
  }
  else if (value2 == lenActual)
  {
    ss << side << " has at least " << +value1 << " cards";
  }
  else if (value1 == 1 && value2 == lenActual-1)
  {
    ss << "Neither opponent is void";
  }
  else if (value1 + value2 == lenActual)
  {
    if (value1 + 1 == value2)
    {
      ss << "The suit splits " << +value1 << "-" << +value2 << 
        " either way";
    }
    else
    {
      ss << "The suit splits " << +value1 << "-" << +value2 <<
        " or better either way";
    }
  }
  else
  {
    ss << "The suit splits between " <<
      +value1 << "=" << +(lenActual - value1) << " and " <<
      +value2 << "=" << +(lenActual - value2);
  }

  return ss.str();
}
*/


string Term::strTop1Equal(const unsigned char oppsTops1) const
{
  stringstream ss;
  const string side = (symmFlag ? "Either opponent" : "West");
  const unsigned char wtop = value1;

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
  const unsigned char wtop1 = value1;
  const unsigned char wtop2 = value2;

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
  if (value1 == 0)
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
  // with length.value1 as an argument.  That is all that is needed.
  if (length.value1 == 1)
  {
    if (value1 == 0)
      ss << side << " has a small singleton";
    else
      ss <<  side << " has " << (oppsTops1 == 1 ? "the" : "a") << " " <<
        "singleton honor";
  }
  else
  {
    assert(! length.symmFlag);
    if (value1 == oppsTops1)
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

  if (length.value1 == 2)
  {
    if (value1 == 0)
    {
      if (oppsLength == 4 && oppsTops1 == 2)
      {
        assert(! length.symmFlag);
        ss << "East has doubleton honors (HH)";
      }
      else
        ss << side << " has a small doubleton";
    }
    else if (value1 == 1)
      ss << side << " has " << (oppsTops1 == 1 ? "the" : "an") << " " <<
        "honor doubleton (Hx)";
    else
      ss << side << " has doubleton honors (HH)";
  }
  else
  {
    assert(! length.symmFlag);
    if (value1 == oppsTops1)
      ss << "East has a small doubleton";
    else if (value1 + 1 == oppsTops1)
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

  if (length.value1 == 3)
  {
    if (value1 == 0)
      ss << side << " has a small tripleton";
    else if (value1 == 1)
      ss << side << " has " << (oppsTops1 == 1 ? "the" : "an") << " " <<
        "honor tripleton (Hxx)";
    else if (value1 == 2)
      ss << side << " has " << (oppsTops1 == 2 ? "the" : "two") << " " <<
        "honors tripleton (HHx)";
    else
      ss << side << " has tripleton honors (HHH)";
  }
  else
  {
    assert(! length.symmFlag);
    if (value1 == oppsTops1)
      ss << "East has a small tripleton";
    else if (value1 + 1 == oppsTops1)
      ss << "East has " << (oppsTops1 == 1 ? "the" : "an") << " " <<
        "honor tripleton (Hxx)";
    else if (value1 + 2 == oppsTops1)
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

  ss << +value1 << " to " << +value2 << ", oper ";
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
    if (value1 == value2)
      s = "== " + to_string(+value1);
    else if (value1 == 0)
      s = "<= " + to_string(+value2);
    else if (value2 == lenActual)
      s = ">= " + to_string(+value1);
    else
      s = to_string(+value1) + "-" + to_string(+value2);
    
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
    if (value1 == value2)
      s = "== " + to_string(+value1);
    else if (value1 == 0)
      s = "<= " + to_string(+value2);
    else
      s = to_string(+value1) + "-" + to_string(+value2);
    
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
    ss << "West has exactly " << +value1 << " " << word;
  else if (oper == COVER_INSIDE_RANGE)
    ss << "West has " << word <<  " in range " << 
      +value1 << " to " << +value2 << " " << word << " inclusive";
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

  if (length.value1 == 0 || length.value1 == oppsLength)
    return length.strBothEqual0(side);
  else if (length.value1 == 1 || length.value1 + 1 == oppsLength)
    return Term::strBothEqual1(length, oppsTops1, side);
  else if (length.value1 == 2 || length.value1 + 2 == oppsLength)
    return Term::strBothEqual2(length, oppsLength, oppsTops1, side);
  else if (length.value1 == 3 || length.value1 + 3 == oppsLength)
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
  coverXes.westMax = value2 - top1.value1;
  coverXes.westMin = value1 - top1.value1;

  coverXes.eastMax =
    (oppsLength - value1) - (oppsTops1 - top1.value1);
  coverXes.eastMin =
    (oppsLength - value2) - (oppsTops1 - top1.value1);

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

  if (value1 == 0)
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

  if (value1 == 1)
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

  if (value1 == 0 || value1 == oppsTops1)
  {
    return Term::strTop1Fixed0(length, oppsLength, oppsTops1, side, coverXes);
  }
  else if (value1 == 1 || value1 + 1 == oppsTops1)
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

