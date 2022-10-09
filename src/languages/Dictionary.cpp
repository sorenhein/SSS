/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include "Dictionary.h"


void Dictionary::read(const string& language)
{
  coverSentences.read(language, "cover/sentences.txt");
  coverPhrases.read(language, "cover/phrases.txt");

  cardsDefinite.read(language, "cards/definite.txt");
  cardsIndefinite.read(language, "cards/indefinite.txt");
  cardsShort.read(language, "cards/short.txt");

  honorsShort.read(language, "honors/short.txt");

  numerals.read(language, "numbers/numerals.txt");
  ordinals.read(language, "ordinals/numerals.txt");

  words.read(language, "words.txt");
}
