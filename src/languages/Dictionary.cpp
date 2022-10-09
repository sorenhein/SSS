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

  coverSentences.init(phrasesConnection);
  coverPhrases.read(language, "cover/phrases.txt");

  coverSentences.init(definiteConnection);
  cardsDefinite.read(language, "cards/definite.txt");

  coverSentences.init(indefiniteConnection);
  cardsIndefinite.read(language, "cards/indefinite.txt");

  coverSentences.init(cardsShortConnection);
  cardsShort.read(language, "cards/short.txt");

  coverSentences.init(honorsShortConnection);
  honorsShort.read(language, "honors/short.txt");

  coverSentences.init(numeralsConnection);
  numerals.read(language, "numbers/numerals.txt");

  coverSentences.init(ordinalsConnection);
  ordinals.read(language, "ordinals/ordinals.txt");

  coverSentences.init(wordsConnection);
  words.read(language, "words.txt");
}
