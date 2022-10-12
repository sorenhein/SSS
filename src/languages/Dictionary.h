/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_DICTIONARY_H
#define SSS_DICTIONARY_H

#include "Component.h"

using namespace std;


struct Dictionary
{
  Component coverSentences;
  Component coverPhrases;

  Component cardsDefinite;
  Component cardsIndefinite;
  Component cardsPrepositionOf;
  Component cardsShort;

  Component honorsShort;

  Component numerals;
  Component ordinals;

  Component words;


  void read(const string& language);
};

#endif
