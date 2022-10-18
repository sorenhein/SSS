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
#include "connections/cards/preposition.h"

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
  { SENTENCE_COUNT_ONETOP, "SENTENCE_COUNT_ONETOP",
    GROUP_SENTENCES, PHRASE_NONE },
  { SENTENCE_EXACT_COUNT_ONETOP, "SENTENCE_EXACT_COUNT_ONETOP",
    GROUP_SENTENCES, PHRASE_NONE },
  { SENTENCE_TOPS_LENGTH, "SENTENCE_TOPS_LENGTH",
    GROUP_SENTENCES, PHRASE_NONE },
  { SENTENCE_TOPS_LENGTH_OLD, "SENTENCE_TOPS_LENGTH_OLD",
    GROUP_SENTENCES, PHRASE_NONE },
  { SENTENCE_TOPS_LENGTH_OLDER, "SENTENCE_TOPS_LENGTH_OLDER",
    GROUP_SENTENCES, PHRASE_NONE },
  { SENTENCE_TOPS_BOTH_LENGTH, "SENTENCE_TOPS_BOTH_LENGTH",
    GROUP_SENTENCES, PHRASE_NONE },
  { SENTENCE_TOPS_BOTH_SYMM, "SENTENCE_TOPS_BOTH_SYMM",
    GROUP_SENTENCES, PHRASE_NONE },
  { SENTENCE_TOPS_BOTH_NOT_SYMM, "SENTENCE_TOPS_BOTH_NOT_SYMM",
    GROUP_SENTENCES, PHRASE_NONE },
  { SENTENCE_TOPS_AND_XES, "SENTENCE_TOPS_AND_XES",
    GROUP_SENTENCES, PHRASE_NONE },
  { SENTENCE_TOPS_AND_LOWER, "SENTENCE_TOPS_AND_LOWER",
    GROUP_SENTENCES, PHRASE_NONE },
  { SENTENCE_TOPS_AND_LOWER_NEW, "SENTENCE_TOPS_AND_LOWER_NEW",
    GROUP_SENTENCES, PHRASE_NONE },
  { SENTENCE_TOPS_AND_BELOW, "SENTENCE_TOPS_AND_BELOW",
    GROUP_SENTENCES, PHRASE_NONE },
  { SENTENCE_TOPS_AND_BELOW_NEW, "SENTENCE_TOPS_AND_BELOW_NEW",
    GROUP_SENTENCES, PHRASE_NONE },
  { SENTENCE_ONLY_BELOW, "SENTENCE_ONLY_BELOW",
    GROUP_SENTENCES, PHRASE_NONE },
  { SENTENCE_SINGULAR_EXACT, "SENTENCE_SINGULAR_EXACT",
    GROUP_SENTENCES, PHRASE_NONE },
  { SENTENCE_HONORS_ORDINAL, "SENTENCE_HONORS_ORDINAL",
    GROUP_SENTENCES, PHRASE_NONE },
  { SENTENCE_SET_UNSET, "SENTENCE_SET_UNSET",
    GROUP_SENTENCES, PHRASE_NONE },
  { SENTENCE_LIST, "SENTENCE_LIST",
    GROUP_SENTENCES, PHRASE_NONE }
};


list<VerbalConnection> phrasesConnection =
{
  { PLAYER_WEST, "PLAYER_WEST", GROUP_PHRASES_PLAYER, PHRASE_NONE },
  { PLAYER_EAST, "PLAYER_EAST", GROUP_PHRASES_PLAYER, PHRASE_NONE },
  { PLAYER_EITHER, "PLAYER_EITHER", GROUP_PHRASES_PLAYER, PHRASE_NONE },
  { PLAYER_EACH, "PLAYER_EACH", GROUP_PHRASES_PLAYER, PHRASE_NONE },
  { PLAYER_NEITHER, "PLAYER_NEITHER", GROUP_PHRASES_PLAYER, PHRASE_NONE },
  { PLAYER_SUIT, "PLAYER_SUIT", GROUP_PHRASES_PLAYER, PHRASE_NONE },

  { LENGTH_VOID, "LENGTH_VOID", 
    GROUP_PHRASES_LENGTH, PHRASE_NONE },
  { LENGTH_NOT_VOID, "LENGTH_NOT_VOID", 
    GROUP_PHRASES_LENGTH, PHRASE_NONE },
  { LENGTH_XTON, "LENGTH_XTON", 
    GROUP_PHRASES_LENGTH, PHRASE_ORDINAL },
  { LENGTH_EVENLY, "LENGTH_EVENLY", 
    GROUP_PHRASES_LENGTH, PHRASE_NONE },
  { LENGTH_ODD_EVENLY, "LENGTH_ODD_EVENLY", 
    GROUP_PHRASES_LENGTH, PHRASE_NONE },
  { LENGTH_XTON_ATMOST, "LENGTH_XTON_ATMOST", 
    GROUP_PHRASES_LENGTH, PHRASE_ORDINAL },
  { LENGTH_ATMOST, "LENGTH_ATMOST", 
    GROUP_PHRASES_LENGTH, PHRASE_NUMERICAL },
  { LENGTH_12, "LENGTH_12", 
    GROUP_PHRASES_LENGTH, PHRASE_NONE },
  { LENGTH_RANGE, "LENGTH_RANGE", 
    GROUP_PHRASES_LENGTH, PHRASE_NUMERICAL },
  { LENGTH_BETWEEN, "LENGTH_BETWEEN", 
    GROUP_PHRASES_LENGTH, PHRASE_NUMERICAL },
  { LENGTH_SPLIT, "LENGTH_SPLIT", 
    GROUP_PHRASES_LENGTH, PHRASE_NUMERICAL },

  { ORDINAL_EXACT, "ORDINAL_EXACT", 
    GROUP_PHRASES_ORDINAL, PHRASE_ORDINAL },
  { ORDINAL_ATMOST, "ORDINAL_ATMOST", 
    GROUP_PHRASES_ORDINAL, PHRASE_ORDINAL },
  { ORDINAL_ADJACENT, "ORDINAL_ADJACENT", 
    GROUP_PHRASES_ORDINAL, PHRASE_ORDINAL },
  { ORDINAL_RANGE, "ORDINAL_RANGE", 
    GROUP_PHRASES_ORDINAL, PHRASE_ORDINAL },

  { COUNT_EXACT, "COUNT_EXACT", 
    GROUP_PHRASES_COUNT, PHRASE_NUMERICAL },
  { COUNT_ATMOST, "COUNT_ATMOST", 
    GROUP_PHRASES_COUNT, PHRASE_NUMERICAL },
  { COUNT_ATLEAST, "COUNT_ATLEAST", 
    GROUP_PHRASES_COUNT, PHRASE_NUMERICAL },
  { COUNT_RANGE, "COUNT_RANGE", 
    GROUP_PHRASES_COUNT, PHRASE_NUMERICAL },
  { COUNT_OR, "COUNT_OR", 
    GROUP_PHRASES_COUNT, PHRASE_NUMERICAL },

  { DIGITS_RANGE, "DIGITS_RANGE", GROUP_PHRASES_COUNT, PHRASE_DIGITS},

  { TOPS_LOWEST, "TOPS_LOWEST", 
    GROUP_PHRASES_TOPS, PHRASE_LOWEST_CARD },
  { TOPS_INDEFINITE, "TOPS_INDEFINITE", 
    GROUP_PHRASES_TOPS, PHRASE_INDEFINITE_RANK},
  { TOPS_DEFINITE, "TOPS_DEFINITE", 
    GROUP_PHRASES_TOPS, PHRASE_DEFINITE_RANK},
  { TOPS_OF_DEFINITE, "TOPS_OF_DEFINITE", 
    GROUP_PHRASES_TOPS, PHRASE_OF_DEFINITE_RANK },
  { TOPS_SET, "TOPS_SET", 
    GROUP_PHRASES_TOPS, PHRASE_COMPLETION_SET},
  { TOPS_UNSET, "TOPS_UNSET", 
    GROUP_PHRASES_TOPS, PHRASE_COMPLETION_UNSET},
  { TOPS_XES, "TOPS_XES", 
    GROUP_PHRASES_TOPS, PHRASE_XES},

  // TODO Not really a top?
  { TOPS_CARDS_WORD, "TOPS_CARDS_WORD", 
    GROUP_PHRASES_TOPS, PHRASE_CARDS_WORD },

  { DICT_CARD, "DICT_CARD", 
    GROUP_PHRASES_WORD, PHRASE_NONE },
  { DICT_CARDS, "DICT_CARDS", 
    GROUP_PHRASES_WORD, PHRASE_NONE },
  { DICT_HONOR, "DICT_HONOR", 
    GROUP_PHRASES_WORD, PHRASE_NONE },
  { DICT_HONORS, "DICT_HONORS", 
    GROUP_PHRASES_WORD, PHRASE_NONE },
  { DICT_MID_HONOR, "DICT_MID_HONOR", 
    GROUP_PHRASES_WORD, PHRASE_NONE },
  { DICT_MID_HONORS, "DICT_MID_HONORS", 
    GROUP_PHRASES_WORD, PHRASE_NONE },
  { DICT_BELOW, "DICT_BELOW", 
    GROUP_PHRASES_WORD, PHRASE_NONE },
  { DICT_BELOW_COMPLETELY, "DICT_BELOW_COMPLETELY", 
    GROUP_PHRASES_WORD, PHRASE_NONE },
  { DICT_EITHER_WAY, "DICT_EITHER_WAY", 
    GROUP_PHRASES_WORD, PHRASE_NONE },
  { DICT_ONE_WAY, "DICT_ONE_WAY", 
    GROUP_PHRASES_WORD, PHRASE_NONE },

  { BELOW_NORMAL, "BELOW_NORMAL", 
    GROUP_PHRASES_BELOW, PHRASE_NONE },
  { BELOW_COMPLETELY, "BELOW_COMPLETELY", 
    GROUP_PHRASES_BELOW, PHRASE_NONE },

  { LIST_HOLDING_EXACT, "LIST_HOLDING_EXACT", 
    GROUP_PHRASES_LIST, PHRASE_COMPLETION_SET },

  { EITHER_WAY, "EITHER_WAY", GROUP_PHRASES_EITHER_WAY, PHRASE_NONE },
  { ONE_WAY, "ONE_WAY", GROUP_PHRASES_EITHER_WAY, PHRASE_NONE },

  { HONORS_ONE, "HONORS_ONE", 
    GROUP_PHRASES_HONORS, PHRASE_NONE },
  { HONORS_MULTIPLE, "HONORS_MULTIPLE", 
    GROUP_PHRASES_HONORS, PHRASE_NUMERICAL }
};


list<VerbalConnection> wordsConnection =
{
  { WORDS_CARD, "CARD", GROUP_CARDS, PHRASE_NONE },
  { WORDS_CARDS, "CARDS", GROUP_CARDS, PHRASE_NONE },

  { WORDS_HONOR, "HONOR", GROUP_HONORS, PHRASE_NONE },
  { WORDS_HONORS, "HONORS", GROUP_HONORS, PHRASE_NONE },
  { WORDS_MID_HONOR, "MID_HONOR", GROUP_HONORS, PHRASE_NONE },
  { WORDS_MID_HONORS, "MID_HONORS", GROUP_HONORS, PHRASE_NONE },

  { WORDS_SMALL, "SMALL", GROUP_SMALL, PHRASE_NONE },

  { WORDS_CONJUNCTION, "CONJUNCTION_OR", GROUP_CONJUNCTIONS, PHRASE_NONE },

  { WORDS_PARTICLE_DEF_PLURAL, "PARTICLE_DEF_PLURAL", 
    GROUP_PARTICLES, PHRASE_NONE },

  { WORDS_DEF_PLURAL_OF, "PREPOSITION_DEF_PLURAL_OF", 
    GROUP_PREPOSITIONS, PHRASE_NONE },

  { WORDS_THEY_HOLD, "THEY_HOLD", GROUP_THEY_HOLD, PHRASE_NONE }
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

  cardsPrepositionOf.init(prepositionConnection);
  cardsPrepositionOf.read(language, "cards/preposition.txt");

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
