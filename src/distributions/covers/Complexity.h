/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COMPLEXITY_H
#define SSS_COMPLEXITY_H

#include <string>


using namespace std;


class Complexity
{
  private:

    unsigned char sum;

    unsigned char max;

  public:

  void reset();

  void addCover(
    const unsigned char coverComplexity,
    const unsigned char rowComplexity);

  void addRow(const unsigned char rowComplexity);

  unsigned char headroom(const Complexity& solution) const;

  bool operator < (const Complexity& tc2) const;

  string str() const;
};

#endif
