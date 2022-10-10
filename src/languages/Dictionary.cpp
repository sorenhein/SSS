/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include "Dictionary.h"

#include "VerbalConnection.h"
#include "PhraseExpansion.h"


#include "connections/cover/sentences.h"
#include "connections/cover/phrases.h"

#include "connections/cards/definite.h"
#include "connections/cards/indefinite.h"
#include "connections/cards/short.h"

#include "connections/honors/short.h"

#include "connections/numbers/numerals.h"
#include "connections/numbers/ordinals.h"

#include "connections/words.h"


list<VerbalConnection> sentencesConnection =
{
  { SENTENCE_LENGTH_ONLY, "SENTENCE_LENGTH_ONLY",
    GROUP_SENTENCES, PHRASE_NONE },
  { SENTENCE_ONETOP_ONLY, "SENTENCE_ONETOP_ONLY",
    GROUP_SENTENCES, PHRASE_NONE },
  { SENTENCE_TOPS_LENGTH, "SENTENCE_TOPS_LENGTH",
    GROUP_SENTENCES, PHRASE_NONE },
  { SENTENCE_TOPS_LENGTH_WITHOUT, "SENTENCE_TOPS_LENGTH_WITHOUT",
    GROUP_SENTENCES, PHRASE_NONE },
  { SENTENCE_TOPS_EXCLUDING, "SENTENCE_TOPS_EXCLUDING",
    GROUP_SENTENCES, PHRASE_NONE },
  { SENTENCE_TOPS_AND_XES, "SENTENCE_TOPS_AND_XES",
    GROUP_SENTENCES, PHRASE_NONE },
  { SENTENCE_TOPS_AND_LOWER, "SENTENCE_TOPS_AND_LOWER",
    GROUP_SENTENCES, PHRASE_NONE },
  { SENTENCE_ONLY_BELOW, "SENTENCE_ONLY_BELOW",
    GROUP_SENTENCES, PHRASE_NONE },
  { SENTENCE_LIST, "SENTENCE_LIST",
    GROUP_SENTENCES, PHRASE_NONE }
};


list<VerbalConnection> wordsConnection =
{
  { WORDS_CARD, "CARD", GROUP_CARDS, PHRASE_NONE },
  { WORDS_CARDS, "CARDS", GROUP_CARDS, PHRASE_NONE },

  { WORDS_HONOR, "HONOR", GROUP_HONORS, PHRASE_NONE },
  { WORDS_HONORS, "HONORS", GROUP_HONORS, PHRASE_NONE },

  { WORDS_SMALL, "SMALL", GROUP_SMALL, PHRASE_NONE },

  { WORDS_CONJUNCTION, "CONJUNCTION_OR", GROUP_CONJUNCTIONS, PHRASE_NONE }
};



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
