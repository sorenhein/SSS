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

    unsigned raw;


  public:

  void reset();

  bool match(
    const unsigned char coverComplexity,
    const unsigned char rowComplexity,
    const unsigned rawWeight,
    const Complexity& solution) const;

  void addCover(
    const unsigned char coverComplexity,
    const unsigned char rowComplexity,
    const unsigned rawWeight);

  void addRow(
    const unsigned char rowComplexity,
    const unsigned rawWeight);

  unsigned char headroom(const Complexity& solution) const;

  bool operator < (const Complexity& comp2) const;

  bool compareAgainstPartial(
    const Complexity& partial,
    const unsigned complexityAdder = 0) const;

  unsigned complexitySum() const;

  string str() const;
};

#endif
