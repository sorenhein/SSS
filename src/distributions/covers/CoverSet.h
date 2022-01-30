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

#include "CoverElement.h"

using namespace std;


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


struct CoverSet
{
  CoverMode mode;
  bool symmFlag;
  CoverElement length;
  CoverElement top1;

  void reset();

  // private
  bool includesLength(
    const unsigned char wlen,
    const unsigned char oppsLength) const;

  // private
  bool includesTop1(
    const unsigned char wtop,
    const unsigned char oppsTops1) const;

  // private
  bool includesLengthAndTop1(
    const unsigned char wtop,
    const unsigned char wlen,
    const unsigned char oppsLength,
    const unsigned char oppsTops1) const;

  // public
  bool includes(
    const unsigned char wtop,
    const unsigned char wlen,
    const unsigned char oppsLength,
    const unsigned char oppsTops1) const;

  // private
  string strLengthEqual(const unsigned char oppsLength) const;

  // private
  string strLengthInside(const unsigned char oppsLength) const;

  // public
  string strLength(const unsigned char oppsLength) const;

  // private
  string strTop1Equal(const unsigned char oppsTops1) const;

  // private
  string strTop1Inside(const unsigned char oppsTops1) const;

  // public
  string strTop1(const unsigned char oppsTops1) const;

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
    const CoverXes& coverXes) const;

  string strTop1Fixed1(
    const unsigned char oppsTops1,
    const string& side,
    const CoverXes& coverXes) const;

  string strTop1Fixed(
    const unsigned char oppsLength,
    const unsigned char oppsTops1) const;

  void setSymm(const bool symmFlagIn);

  string str(
    const unsigned char oppsLength,
    const unsigned char oppsTops1) const;
};

#endif
