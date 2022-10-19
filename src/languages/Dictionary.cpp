/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <cassert>

#include "Dictionary.h"

#include "VerbalConnection.h"
#include "PhraseExpansion.h"


#include "connections/cover/sentences.h"
#include "connections/cover/phrases.h"

#include "connections/cards/definite.h"
#include "connections/cards/indefinite.h"
#include "connections/cards/short.h"
#include "connections/cards/preposition.h"

#include "connections/honors/short.h"

#include "connections/numbers/numerals.h"
#include "connections/numbers/ordinals.h"

#include "connections/words.h"


list<VerbalConnection> sentencesConnection =
{
  { SENTENCE_LENGTH, "SENTENCE_LENGTH", PHRASE_NONE },
  { SENTENCE_TOPS, "SENTENCE_TOPS", PHRASE_NONE },
  { SENTENCE_LENGTH_BELOW_TOPS, "SENTENCE_LENGTH_BELOW_TOPS",
    PHRASE_NONE },
  { SENTENCE_COUNT_OF_TOPS_ORDINAL, "SENTENCE_COUNT_OF_TOPS_ORDINAL",
    PHRASE_NONE },
  { SENTENCE_COUNT_TOPS, "SENTENCE_COUNT_TOPS", PHRASE_NONE },
  { SENTENCE_EXACTLY_COUNT_TOPS, "SENTENCE_EXACTLY_COUNT_TOPS",
    PHRASE_NONE },
  { SENTENCE_TOPS_ORDINAL, "SENTENCE_TOPS_ORDINAL", PHRASE_NONE },
  { SENTENCE_COUNT_TOPS_ORDINAL, "SENTENCE_COUNT_TOPS_ORDINAL",
    PHRASE_NONE },
  { SENTENCE_COUNT_HONORS_ORDINAL, 
    "SENTENCE_COUNT_HONORS_ORDINAL",
    PHRASE_NONE },
  { SENTENCE_EXACTLY_COUNT_TOPS_ORDINAL, 
    "SENTENCE_EXACTLY_COUNT_TOPS_ORDINAL",
    PHRASE_NONE },
  { SENTENCE_TOPS_AND_LOWER, "SENTENCE_TOPS_AND_LOWER", PHRASE_NONE },
  { SENTENCE_EXACTLY_TOPS_AND_LOWER, "SENTENCE_EXACTLY_TOPS_AND_LOWER",
    PHRASE_NONE },
  { SENTENCE_TOPS_AND_COUNT_BELOW_CARD, 
    "SENTENCE_TOPS_AND_COUNT_BELOW_CARD",
    PHRASE_NONE },
  { SENTENCE_EXACTLY_TOPS_MAYBE_UNSET, 
    "SENTENCE_EXACTLY_TOPS_MAYBE_UNSET",
    PHRASE_NONE },
  { SENTENCE_TOPS_MAYBE_OTHERS, "SENTENCE_TOPS_MAYBE_OTHERS", PHRASE_NONE },
  { SENTENCE_TOPS_AND_XES, "SENTENCE_TOPS_AND_XES", PHRASE_NONE },
  { SENTENCE_EXACTLY_LIST, "SENTENCE_EXACTLY_LIST", PHRASE_NONE },
  { SENTENCE_2SPLIT_TOPS_SYMM, "SENTENCE_2SPLIT_TOPS_SYMM", PHRASE_NONE },
  { SENTENCE_2SPLIT_TOPS_ASYMM, "SENTENCE_2SPLIT_TOPS_ASYMM", PHRASE_NONE },
  { SENTENCE_2SPLIT_TOPS_DIGITS_SYMM, "SENTENCE_2SPLIT_TOPS_DIGITS_SYMM",
    PHRASE_NONE },
  { SENTENCE_2SPLIT_TOPS_DIGITS_ASYMM, "SENTENCE_2SPLIT_TOPS_DIGITS_ASYMM",
    PHRASE_NONE },
};

list<TagConnection> sentencesGroupConnection =
{
  { GROUP_SENTENCES, "SENTENCES_GENERAL"}
};


list<VerbalConnection> phrasesConnection =
{
  { PLAYER_WEST, "PLAYER_WEST", PHRASE_NONE },
  { PLAYER_EAST, "PLAYER_EAST", PHRASE_NONE },
  { PLAYER_EITHER, "PLAYER_EITHER", PHRASE_NONE },
  { PLAYER_EACH, "PLAYER_EACH", PHRASE_NONE },
  { PLAYER_NEITHER, "PLAYER_NEITHER", PHRASE_NONE },
  { PLAYER_SUIT, "PLAYER_SUIT", PHRASE_NONE },

  { LENGTH_VOID, "LENGTH_VOID", PHRASE_NONE },
  { LENGTH_NOT_VOID, "LENGTH_NOT_VOID", PHRASE_NONE },
  { LENGTH_XTON, "LENGTH_XTON", PHRASE_ORDINAL },
  { LENGTH_EVENLY, "LENGTH_EVENLY", PHRASE_NONE },
  { LENGTH_ODD_EVENLY, "LENGTH_ODD_EVENLY", PHRASE_NONE },
  { LENGTH_XTON_ATMOST, "LENGTH_XTON_ATMOST", PHRASE_ORDINAL },
  { LENGTH_ATMOST, "LENGTH_ATMOST", PHRASE_NUMERICAL },
  { LENGTH_12, "LENGTH_12", PHRASE_NONE },
  { LENGTH_RANGE, "LENGTH_RANGE", PHRASE_NUMERICAL },
  { LENGTH_BETWEEN, "LENGTH_BETWEEN", PHRASE_NUMERICAL },
  { LENGTH_SPLIT, "LENGTH_SPLIT", PHRASE_NUMERICAL },

  { ORDINAL_EXACT, "ORDINAL_EXACT", PHRASE_ORDINAL },
  { ORDINAL_ATMOST, "ORDINAL_ATMOST", PHRASE_ORDINAL },
  { ORDINAL_ADJACENT, "ORDINAL_ADJACENT", PHRASE_ORDINAL },
  { ORDINAL_RANGE, "ORDINAL_RANGE", PHRASE_ORDINAL },

  { COUNT_EXACT, "COUNT_EXACT", PHRASE_NUMERICAL },
  { COUNT_ATMOST, "COUNT_ATMOST", PHRASE_NUMERICAL },
  { COUNT_ATLEAST, "COUNT_ATLEAST", PHRASE_NUMERICAL },
  { COUNT_RANGE, "COUNT_RANGE", PHRASE_NUMERICAL },
  { COUNT_OR, "COUNT_OR", PHRASE_NUMERICAL },

  { DIGITS_EXACT, "DIGITS_EXACT", PHRASE_DIGITS},
  { DIGITS_ATMOST, "DIGITS_ATMOST", PHRASE_DIGITS},
  { DIGITS_ATLEAST, "DIGITS_ATLEAST", PHRASE_DIGITS},
  { DIGITS_RANGE, "DIGITS_RANGE", PHRASE_DIGITS},

  { TOPS_LOWEST, "TOPS_LOWEST", PHRASE_LOWEST_CARD },
  { TOPS_INDEFINITE, "TOPS_INDEFINITE", PHRASE_INDEFINITE_RANK},
  { TOPS_DEFINITE, "TOPS_DEFINITE", PHRASE_DEFINITE_RANK},
  { TOPS_OF_DEFINITE, "TOPS_OF_DEFINITE", PHRASE_OF_DEFINITE_RANK },
  { TOPS_SET, "TOPS_SET", PHRASE_COMPLETION_SET},
  { TOPS_UNSET, "TOPS_UNSET", PHRASE_COMPLETION_UNSET},
  { TOPS_XES, "TOPS_XES", PHRASE_XES},

  { DICT_CARD, "DICT_CARD", PHRASE_NONE },
  { DICT_CARDS, "DICT_CARDS", PHRASE_NONE },
  { DICT_HONOR, "DICT_HONOR", PHRASE_NONE },
  { DICT_HONORS, "DICT_HONORS", PHRASE_NONE },
  { DICT_MID_HONOR, "DICT_MID_HONOR", PHRASE_NONE },
  { DICT_MID_HONORS, "DICT_MID_HONORS", PHRASE_NONE },
  { DICT_BELOW, "DICT_BELOW", PHRASE_NONE },
  { DICT_BELOW_COMPLETELY, "DICT_BELOW_COMPLETELY", PHRASE_NONE },
  { DICT_EITHER_WAY, "DICT_EITHER_WAY", PHRASE_NONE },
  { DICT_ONE_WAY, "DICT_ONE_WAY", PHRASE_NONE },

  { LIST_HOLDING, "LIST_HOLDING", PHRASE_COMPLETION_SET }
};

list<TagConnection> phrasesGroupConnection =
{
  { GROUP_PHRASES_PLAYER, "PHRASES_PLAYER" },
  { GROUP_PHRASES_LENGTH, "PHRASES_LENGTH" },
  { GROUP_PHRASES_ORDINAL, "PHRASES_ORDINAL" },
  { GROUP_PHRASES_COUNT, "PHRASES_COUNT" },
  { GROUP_PHRASES_DIGITS, "PHRASES_DIGITS" },
  { GROUP_PHRASES_DICT, "PHRASES_DICT" },
  { GROUP_PHRASES_TOPS, "PHRASES_TOPS" },
  { GROUP_PHRASES_LIST, "PHRASES_LIST" }
};


list<VerbalConnection> definiteConnection =
{
  { DEFINITE_2, "CARD_DEF_2", PHRASE_NONE },
  { DEFINITE_3, "CARD_DEF_3", PHRASE_NONE },
  { DEFINITE_4, "CARD_DEF_4", PHRASE_NONE },
  { DEFINITE_5, "CARD_DEF_5", PHRASE_NONE },
  { DEFINITE_6, "CARD_DEF_6", PHRASE_NONE },
  { DEFINITE_7, "CARD_DEF_7", PHRASE_NONE },
  { DEFINITE_8, "CARD_DEF_8", PHRASE_NONE },
  { DEFINITE_9, "CARD_DEF_9", PHRASE_NONE },
  { DEFINITE_TEN, "CARD_DEF_TEN", PHRASE_NONE },
  { DEFINITE_JACK, "CARD_DEF_JACK", PHRASE_NONE },
  { DEFINITE_QUEEN, "CARD_DEF_QUEEN", PHRASE_NONE },
  { DEFINITE_KING, "CARD_DEF_KING", PHRASE_NONE },
  { DEFINITE_ACE, "CARD_DEF_ACE", PHRASE_NONE }
};

list<TagConnection> definiteGroupConnection =
{
  { GROUP_DEFINITE, "GROUP_DEFINITE" }
};


list<VerbalConnection> indefiniteConnection =
{
  { INDEFINITE_2, "CARD_INDEF_2", PHRASE_NONE },
  { INDEFINITE_3, "CARD_INDEF_3", PHRASE_NONE },
  { INDEFINITE_4, "CARD_INDEF_4", PHRASE_NONE },
  { INDEFINITE_5, "CARD_INDEF_5", PHRASE_NONE },
  { INDEFINITE_6, "CARD_INDEF_6", PHRASE_NONE },
  { INDEFINITE_7, "CARD_INDEF_7", PHRASE_NONE },
  { INDEFINITE_8, "CARD_INDEF_8", PHRASE_NONE },
  { INDEFINITE_9, "CARD_INDEF_9", PHRASE_NONE },
  { INDEFINITE_TEN, "CARD_INDEF_TEN", PHRASE_NONE },
  { INDEFINITE_JACK, "CARD_INDEF_JACK", PHRASE_NONE },
  { INDEFINITE_QUEEN, "CARD_INDEF_QUEEN", PHRASE_NONE },
  { DEFINITE_KING, "CARD_INDEF_KING", PHRASE_NONE },
  { INDEFINITE_ACE, "CARD_INDEF_ACE", PHRASE_NONE }
};

list<TagConnection> indefiniteGroupConnection =
{
  { GROUP_INDEFINITE, "GROUP_INDEFINITE" }
};


list<VerbalConnection> prepositionConnection =
{
  { PREP_OF_2, "PREP_OF_2", PHRASE_NONE },
  { PREP_OF_3, "PREP_OF_3", PHRASE_NONE },
  { PREP_OF_4, "PREP_OF_4", PHRASE_NONE },
  { PREP_OF_5, "PREP_OF_5", PHRASE_NONE },
  { PREP_OF_6, "PREP_OF_6", PHRASE_NONE },
  { PREP_OF_7, "PREP_OF_7", PHRASE_NONE },
  { PREP_OF_8, "PREP_OF_8", PHRASE_NONE },
  { PREP_OF_9, "PREP_OF_9", PHRASE_NONE },
  { PREP_OF_TEN, "PREP_OF_TEN", PHRASE_NONE },
  { PREP_OF_JACK, "PREP_OF_JACK", PHRASE_NONE },
  { PREP_OF_QUEEN, "PREP_OF_QUEEN", PHRASE_NONE },
  { PREP_OF_KING, "PREP_OF_KING", PHRASE_NONE },
  { PREP_OF_ACE, "PREP_OF_ACE", PHRASE_NONE }
};

list<TagConnection> prepositionGroupConnection =
{
  { GROUP_PREP_LOCAL, "GROUP_PREPOSITION" }
};


list<VerbalConnection> cardsShortConnection =
{
  { SHORT_2, "CARD_SHORT_2", PHRASE_NONE },
  { SHORT_3, "CARD_SHORT_3", PHRASE_NONE },
  { SHORT_4, "CARD_SHORT_4", PHRASE_NONE },
  { SHORT_5, "CARD_SHORT_5", PHRASE_NONE },
  { SHORT_6, "CARD_SHORT_6", PHRASE_NONE },
  { SHORT_7, "CARD_SHORT_7", PHRASE_NONE },
  { SHORT_8, "CARD_SHORT_8", PHRASE_NONE },
  { SHORT_9, "CARD_SHORT_9", PHRASE_NONE },
  { SHORT_TEN, "CARD_SHORT_TEN", PHRASE_NONE },
  { SHORT_JACK, "CARD_SHORT_JACK", PHRASE_NONE },
  { SHORT_QUEEN, "CARD_SHORT_QUEEN", PHRASE_NONE },
  { SHORT_KING, "CARD_SHORT_KING", PHRASE_NONE },
  { SHORT_ACE, "CARD_SHORT_ACE", PHRASE_NONE }
};

list<TagConnection> cardsShortGroupConnection =
{
  { GROUP_SHORT, "GROUP_SHORT" }
};


list<VerbalConnection> honorsShortConnection =
{
  { HONOR_SHORT_1, "HONOR_SHORT_1", PHRASE_NONE },
  { HONOR_SHORT_2, "HONOR_SHORT_2", PHRASE_NONE },
  { HONOR_SHORT_3, "HONOR_SHORT_3", PHRASE_NONE },
  { HONOR_SHORT_4, "HONOR_SHORT_4", PHRASE_NONE },
  { HONOR_SHORT_5, "HONOR_SHORT_5", PHRASE_NONE },
  { HONOR_SHORT_6, "HONOR_SHORT_6", PHRASE_NONE }
};

list<TagConnection> honorsShortGroupConnection =
{
  { GROUP_HONOR_SHORT, "GROUP_HONOR_SHORT" }
};


list<VerbalConnection> numeralsConnection =
{
  { NUMERAL_0, "NUMERAL_0", PHRASE_NONE },
  { NUMERAL_1, "NUMERAL_1", PHRASE_NONE },
  { NUMERAL_2, "NUMERAL_2", PHRASE_NONE },
  { NUMERAL_3, "NUMERAL_3", PHRASE_NONE },
  { NUMERAL_4, "NUMERAL_4", PHRASE_NONE },
  { NUMERAL_5, "NUMERAL_5", PHRASE_NONE },
  { NUMERAL_6, "NUMERAL_6", PHRASE_NONE },
  { NUMERAL_7, "NUMERAL_7", PHRASE_NONE },
  { NUMERAL_8, "NUMERAL_8", PHRASE_NONE },
  { NUMERAL_9, "NUMERAL_9", PHRASE_NONE },
  { NUMERAL_10, "NUMERAL_10", PHRASE_NONE },
  { NUMERAL_11, "NUMERAL_11", PHRASE_NONE },
  { NUMERAL_12, "NUMERAL_12", PHRASE_NONE },
  { NUMERAL_13, "NUMERAL_13", PHRASE_NONE }
};

list<TagConnection> numeralsGroupConnection =
{
  { GROUP_NUMERAL, "GROUP_NUMERAL" }
};


list<VerbalConnection> ordinalsConnection =
{
  { ORDINAL_0, "ORDINAL_0", PHRASE_NONE },
  { ORDINAL_1, "ORDINAL_1", PHRASE_NONE },
  { ORDINAL_2, "ORDINAL_2", PHRASE_NONE },
  { ORDINAL_3, "ORDINAL_3", PHRASE_NONE },
  { ORDINAL_4, "ORDINAL_4", PHRASE_NONE },
  { ORDINAL_5, "ORDINAL_5", PHRASE_NONE },
  { ORDINAL_6, "ORDINAL_6", PHRASE_NONE },
  { ORDINAL_7, "ORDINAL_7", PHRASE_NONE },
  { ORDINAL_8, "ORDINAL_8", PHRASE_NONE },
  { ORDINAL_9, "ORDINAL_9", PHRASE_NONE },
  { ORDINAL_10, "ORDINAL_10", PHRASE_NONE },
  { ORDINAL_11, "ORDINAL_11", PHRASE_NONE },
  { ORDINAL_12, "ORDINAL_12", PHRASE_NONE },
  { ORDINAL_13, "ORDINAL_13", PHRASE_NONE }
};

list<TagConnection> ordinalsGroupConnection =
{
  { GROUP_ORDINAL, "GROUP_ORDINAL" }
};



list<VerbalConnection> wordsConnection =
{
  { WORDS_CARD, "CARD", PHRASE_NONE },
  { WORDS_CARDS, "CARDS", PHRASE_NONE },

  { WORDS_HONOR, "HONOR", PHRASE_NONE },
  { WORDS_HONORS, "HONORS", PHRASE_NONE },
  { WORDS_MID_HONOR, "MID_HONOR", PHRASE_NONE },
  { WORDS_MID_HONORS, "MID_HONORS", PHRASE_NONE },

  { WORDS_SMALL, "SMALL", PHRASE_NONE },

  { WORDS_CONJUNCTION, "CONJUNCTION_OR", PHRASE_NONE },

  { WORDS_PARTICLE_DEF_PLURAL, "PARTICLE_DEF_PLURAL", 
    PHRASE_NONE },

  { WORDS_DEF_PLURAL_OF, "PREPOSITION_DEF_PLURAL_OF", 
    PHRASE_NONE },

  { WORDS_THEY_HOLD, "THEY_HOLD", PHRASE_NONE }
};

list<TagConnection> wordsGroupConnection =
{
  { GROUP_CARDS, "GROUP_CARDS" },
  { GROUP_HONORS, "GROUP_HONORS" },
  { GROUP_SMALL, "GROUP_SMALL" },
  { GROUP_CONJUNCTIONS, "GROUP_CONJUNCTIONS" },
  { GROUP_PARTICLES, "GROUP_PARTICLES" },
  { GROUP_PREPOSITIONS, "GROUP_PREPOSITIONS" },
  { GROUP_THEY_HOLD, "GROUP_THEY_HOLD" }
};



void Dictionary::setMap(
  const list<TagConnection>& groupConnection,
  map<string, unsigned>& groupMap) const
{
  for (const auto& gc: groupConnection)
  {
    auto gcit = groupMap.find(gc.tag);
    if (gcit != groupMap.end())
    {
      cout << "Duplicate " << gc.tag << " in map" << endl;
      assert(false);
    }
    else
    {
      groupMap[gc.tag] = gc.group;
    }
  }
}


void Dictionary::read(const string& language)
{
  Dictionary::setMap(sentencesGroupConnection, sentenceGroupMap);
  coverSentences.init(sentencesConnection);
  coverSentences.read(language, "cover/sentences.txt", sentenceGroupMap);

  Dictionary::setMap(phrasesGroupConnection, phraseGroupMap);
  coverPhrases.init(phrasesConnection);
  coverPhrases.read(language, "cover/phrases.txt", phraseGroupMap);

  Dictionary::setMap(definiteGroupConnection, definiteGroupMap);
  cardsDefinite.init(definiteConnection);
  cardsDefinite.read(language, "cards/definite.txt", definiteGroupMap);

  Dictionary::setMap(indefiniteGroupConnection, indefiniteGroupMap);
  cardsIndefinite.init(indefiniteConnection);
  cardsIndefinite.read(language, "cards/indefinite.txt",
    indefiniteGroupMap);

  Dictionary::setMap(prepositionGroupConnection, prepositionGroupMap);
  cardsPrepositionOf.init(prepositionConnection);
  cardsPrepositionOf.read(language, "cards/preposition.txt",
    prepositionGroupMap);

  Dictionary::setMap(cardsShortGroupConnection, cardsShortGroupMap);
  cardsShort.init(cardsShortConnection);
  cardsShort.read(language, "cards/short.txt",
    cardsShortGroupMap);

  Dictionary::setMap(honorsShortGroupConnection, honorsShortGroupMap);
  honorsShort.init(honorsShortConnection);
  honorsShort.read(language, "honors/short.txt", honorsShortGroupMap);

  Dictionary::setMap(numeralsGroupConnection, numeralsGroupMap);
  numerals.init(numeralsConnection);
  numerals.read(language, "numbers/numerals.txt", numeralsGroupMap);

  Dictionary::setMap(ordinalsGroupConnection, ordinalsGroupMap);
  ordinals.init(ordinalsConnection);
  ordinals.read(language, "numbers/ordinals.txt", ordinalsGroupMap);

  Dictionary::setMap(wordsGroupConnection, wordsGroupMap);
  words.init(wordsConnection);
  words.read(language, "words.txt", wordsGroupMap);
}
