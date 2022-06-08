/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_TRICKCONVERT_H
#define SSS_TRICKCONVERT_H

#include <vector>
#include <list>

using namespace std;

struct ConvertData;


class TrickConvert
{
  private:

    vector<unsigned char> lookupGE;

    void setConstants();


  public:

    TrickConvert();

    size_t profileSize(const size_t len) const;

    void increment(
      unsigned& counter,
      unsigned& accum,
      const unsigned char value,
      unsigned& position,
      unsigned& result) const;

    void finish(
      unsigned& counter,
      unsigned& accum,
      unsigned& position,
      unsigned& result) const;

    unsigned char lookup(
      const vector<unsigned>& profiles,
      const size_t lastForward,
      const size_t index) const;

    bool fullHouse(const unsigned value) const;

    unsigned limit(
      const size_t lastForward,
      const unsigned value) const;

    bool greaterEqual(
      const unsigned arg1,
      const unsigned arg2) const;
};

#endif
