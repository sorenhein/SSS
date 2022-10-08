/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_VERBALSIDE_H
#define SSS_VERBALSIDE_H


using namespace std;

enum Opponent: unsigned;
enum VerbalPhrase: unsigned;

struct VerbalSide
{
  Opponent side;
  bool symmFlag;

  VerbalPhrase player() const;
};


#endif
