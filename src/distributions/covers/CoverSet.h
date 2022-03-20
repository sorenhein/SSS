/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COVERSET_H
#define SSS_COVERSET_H

#include <iostream>
#include <sstream>
#include <string>

#include "Term.h"
#include "Length.h"

#include "../../utils/table.h"

using namespace std;

struct ProductProfile;


enum CoverMode
{
  COVER_MODE_NONE = 0,
  COVER_LENGTHS_ONLY = 1,
  COVER_TOPS_ONLY = 2,
  COVER_LENGTHS_AND_TOPS = 3
};

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


class CoverSet
{
  private:

    CoverMode mode;

    bool symmFlag;

    Length length;

    Term top1;


    bool includesLength(
      const unsigned char wlen,
      const unsigned char oppsLength) const;

    bool includesTop1(
      const unsigned char wtop,
      const unsigned char oppsTops1) const;

    bool includesLengthAndTop1(
      const unsigned char wtop,
      const unsigned char wlen,
      const unsigned char oppsLength,
      const unsigned char oppsTops1) const;

    string strLength(
      const unsigned char oppsLength,
      const Opponent simplestOpponent) const;

    string strTop1Equal(
      const unsigned char oppsTops1,
      const Opponent simplestOpponent) const;

    string strTop1Inside(
      const unsigned char oppsTops1,
      const Opponent simplestOpponent) const;

    string strTop1(
      const unsigned char oppsTops1,
      const Opponent simplestOpponent) const;

    string strBothEqual0(
      const string& side) const;

    string strBothEqual1(
      const unsigned char oppsTops1,
      const string& side) const;

    string strBothEqual2(
      const unsigned char oppsLength,
      const unsigned char oppsTops1,
      const string& side) const;

    string strBothEqual3(
      const unsigned char oppsLength,
      const unsigned char oppsTops1,
      const string& side) const;

    string strBothEqual(
      const unsigned char oppsLength,
      const unsigned char oppsTops1) const;

    void strXes(
      const unsigned char oppsLength,
      const unsigned char oppsTops1,
      CoverXes& coverXes) const;

    string strTop1Fixed0(
      const unsigned char oppsLength,
      const unsigned char oppsTops1,
      const string& side,
      const CoverXes& coverXes,
      const Opponent simplestOpponent) const;

    string strTop1Fixed1(
      const unsigned char oppsTops1,
      const string& side,
      const CoverXes& coverXes) const;

    string strTop1Fixed(
      const unsigned char oppsLength,
      const unsigned char oppsTops1,
      const Opponent simplestOpponent) const;


  public:

  void reset();

  void setSymm(const bool symmFlagIn);

  void setMode(const CoverMode modeIn);

  CoverMode getMode() const;

  void setLength(
    const unsigned char len,
    const unsigned char oppsLength);

  void setLength(
    const unsigned char len1,
    const unsigned char len2,
    const unsigned char oppsLength);

  void setTop1(
    const unsigned char tops,
    const unsigned char oppsSize);

  void setTop1(
    const unsigned char tops1,
    const unsigned char tops2,
    const unsigned char oppsSize);

  bool includes(
    const ProductProfile& distProfile,
    const unsigned char oppsLength,
    const unsigned char oppsTops1) const;

  string str(
    const unsigned char oppsLength,
    const unsigned char oppsTops1) const;
};

#endif
