/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include "Dictionary.h"

#include "connections/cover/sentences.h"
#include "connections/cover/phrases.h"

#include "connections/cards/definite.h"
#include "connections/cards/indefinite.h"
#include "connections/cards/short.h"

#include "connections/honors/short.h"

#include "connections/numbers/numerals.h"
#include "connections/numbers/ordinals.h"

#include "connections/words.h"


void Dictionary::read(const string& language)
{
  coverSentences.init(sentencesConnection);
  coverSentences.read(language, "cover/sentences.txt");

  coverPhrases.init(phrasesConnection);
  coverPhrases.read(language, "cover/phrases.txt");

  cardsDefinite.init(definiteConnection);
  cardsDefinite.read(language, "cards/definite.txt");

  cardsIndefinite.init(indefiniteConnection);
  cardsIndefinite.read(language, "cards/indefinite.txt");

  cardsShort.init(cardsShortConnection);
  cardsShort.read(language, "cards/short.txt");

  honorsShort.init(honorsShortConnection);
  honorsShort.read(language, "honors/short.txt");

  numerals.init(numeralsConnection);
  numerals.read(language, "numbers/numerals.txt");

  ordinals.init(ordinalsConnection);
  ordinals.read(language, "numbers/ordinals.txt");

  words.init(wordsConnection);
  words.read(language, "words.txt");
}
