/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_RESCONVERT_H
#define SSS_RESCONVERT_H

#include <vector>
#include <list>

class Result;
class Ranges;

using namespace std;


class ResConvert
{
  private:

    vector<unsigned char> lookupGE;

    void setConstants();

    void increment(
      unsigned& counter,
      unsigned& profile,
      const unsigned incr,
      list<unsigned>& profiles) const;

    void scrutinizeHalfVector(
      const vector<unsigned char>& quadTricks,
      const unsigned firstNumber,
      const unsigned lastNumber,
      const unsigned offset,
      vector<unsigned>& profiles) const;


  public:

    ResConvert();

    unsigned profileSize(const unsigned len) const;

    unsigned profileSizeNew(const unsigned len) const;

    void scrutinizeRange(
      const list<Result>& results,
      const Ranges& ranges,
      list<unsigned>& profiles) const;

    void scrutinizeConstant(
      const list<Result>& results,
      const unsigned minTricks,
      list<unsigned>& profiles) const;

    void scrutinizeBinary(
      const list<unsigned char>& binaryTricks,
      list<unsigned>& profiles) const;

    void scrutinizeVector(
      const vector<unsigned char>& quadTricks,
      vector<unsigned>& profiles) const;

    unsigned char lookup(
      const vector<unsigned>& profiles,
      const unsigned index) const;

    void scrutinizeVectorNew(
      const vector<unsigned char>& quadTricks,
      const unsigned lastForward,
      vector<unsigned>& profiles) const;

    unsigned char lookupNew(
      const vector<unsigned>& profiles,
      const unsigned lastForward,
      const unsigned index) const;

    bool greaterEqual(
      const unsigned arg1,
      const unsigned arg2) const;
};

#endif
