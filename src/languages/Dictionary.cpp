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
  { SENTENCE_LENGTH, "SENTENCE_LENGTH" },
  { SENTENCE_TOPS, "SENTENCE_TOPS" },
  { SENTENCE_LENGTH_BELOW_TOPS, "SENTENCE_LENGTH_BELOW_TOPS" },
  { SENTENCE_COUNT_OF_TOPS_ORDINAL, "SENTENCE_COUNT_OF_TOPS_ORDINAL" },
  { SENTENCE_COUNT_TOPS, "SENTENCE_COUNT_TOPS" },
  { SENTENCE_EXACTLY_COUNT_TOPS, "SENTENCE_EXACTLY_COUNT_TOPS" },
  { SENTENCE_TOPS_ORDINAL, "SENTENCE_TOPS_ORDINAL" },
  { SENTENCE_COUNT_TOPS_ORDINAL, "SENTENCE_COUNT_TOPS_ORDINAL" },
  { SENTENCE_COUNT_HONORS_ORDINAL, "SENTENCE_COUNT_HONORS_ORDINAL" },
  { SENTENCE_EXACTLY_COUNT_TOPS_ORDINAL, 
    "SENTENCE_EXACTLY_COUNT_TOPS_ORDINAL" },
  { SENTENCE_TOPS_AND_LOWER, "SENTENCE_TOPS_AND_LOWER" },
  { SENTENCE_EXACTLY_TOPS_AND_LOWER, 
    "SENTENCE_EXACTLY_TOPS_AND_LOWER" },
  { SENTENCE_TOPS_AND_COUNT_BELOW_CARD, 
    "SENTENCE_TOPS_AND_COUNT_BELOW_CARD" },
  { SENTENCE_EXACTLY_TOPS_MAYBE_UNSET, 
    "SENTENCE_EXACTLY_TOPS_MAYBE_UNSET" },
  { SENTENCE_TOPS_MAYBE_OTHERS, 
    "SENTENCE_TOPS_MAYBE_OTHERS" },
  { SENTENCE_TOPS_AND_XES, "SENTENCE_TOPS_AND_XES" },
  { SENTENCE_EXACTLY_LIST, "SENTENCE_EXACTLY_LIST" },
  { SENTENCE_2SPLIT_TOPS_SYMM, "SENTENCE_2SPLIT_TOPS_SYMM" },
  { SENTENCE_2SPLIT_TOPS_ASYMM, "SENTENCE_2SPLIT_TOPS_ASYMM" },
  { SENTENCE_2SPLIT_TOPS_DIGITS_SYMM, 
    "SENTENCE_2SPLIT_TOPS_DIGITS_SYMM" },
  { SENTENCE_2SPLIT_TOPS_DIGITS_ASYMM, 
    "SENTENCE_2SPLIT_TOPS_DIGITS_ASYMM" }
};

list<TagConnection> sentencesGroupConnection =
{
  { GROUP_SENTENCES, "SENTENCES_GENERAL"}
};


list<VerbalConnection> phrasesConnection =
{
  { PLAYER_WEST, "PLAYER_WEST" },
  { PLAYER_EAST, "PLAYER_EAST" },
  { PLAYER_EITHER, "PLAYER_EITHER" },
  { PLAYER_EACH, "PLAYER_EACH" },
  { PLAYER_NEITHER, "PLAYER_NEITHER" },
  { PLAYER_SUIT, "PLAYER_SUIT" },

  { LENGTH_VOID, "LENGTH_VOID" },
  { LENGTH_NOT_VOID, "LENGTH_NOT_VOID" },
  { LENGTH_XTON, "LENGTH_XTON" },
  { LENGTH_EVENLY, "LENGTH_EVENLY" },
  { LENGTH_ODD_EVENLY, "LENGTH_ODD_EVENLY" },
  { LENGTH_XTON_ATMOST, "LENGTH_XTON_ATMOST" },
  { LENGTH_ATMOST, "LENGTH_ATMOST" },
  { LENGTH_12, "LENGTH_12" },
  { LENGTH_RANGE, "LENGTH_RANGE" },
  { LENGTH_BETWEEN, "LENGTH_BETWEEN" },
  { LENGTH_SPLIT, "LENGTH_SPLIT" },

  { ORDINAL_EXACT, "ORDINAL_EXACT" },
  { ORDINAL_ATMOST, "ORDINAL_ATMOST" },
  { ORDINAL_ADJACENT, "ORDINAL_ADJACENT" },
  { ORDINAL_RANGE, "ORDINAL_RANGE" },

  { COUNT_EXACT, "COUNT_EXACT" },
  { COUNT_ATMOST, "COUNT_ATMOST" },
  { COUNT_ATLEAST, "COUNT_ATLEAST" },
  { COUNT_RANGE, "COUNT_RANGE" },
  { COUNT_OR, "COUNT_OR" },

  { DIGITS_EXACT, "DIGITS_EXACT" },
  { DIGITS_ATMOST, "DIGITS_ATMOST" },
  { DIGITS_ATLEAST, "DIGITS_ATLEAST" },
  { DIGITS_RANGE, "DIGITS_RANGE" },

  { TOPS_LOWEST, "TOPS_LOWEST" },
  { TOPS_INDEFINITE, "TOPS_INDEFINITE" },
  { TOPS_DEFINITE, "TOPS_DEFINITE" },
  { TOPS_OF_DEFINITE, "TOPS_OF_DEFINITE" },
  { TOPS_SET, "TOPS_SET" },
  { TOPS_UNSET, "TOPS_UNSET" },
  { TOPS_XES, "TOPS_XES" },

  { DICT_CARD, "DICT_CARD" },
  { DICT_CARDS, "DICT_CARDS" },
  { DICT_HONOR, "DICT_HONOR" },
  { DICT_HONORS, "DICT_HONORS" },
  { DICT_MID_HONOR, "DICT_MID_HONOR" },
  { DICT_MID_HONORS, "DICT_MID_HONORS" },
  { DICT_BELOW, "DICT_BELOW" },
  { DICT_BELOW_COMPLETELY, "DICT_BELOW_COMPLETELY" },
  { DICT_EITHER_WAY, "DICT_EITHER_WAY" },
  { DICT_ONE_WAY, "DICT_ONE_WAY" },

  { LIST_HOLDING, "LIST_HOLDING" }
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
  { DEFINITE_2, "CARD_DEF_2" },
  { DEFINITE_3, "CARD_DEF_3" },
  { DEFINITE_4, "CARD_DEF_4" },
  { DEFINITE_5, "CARD_DEF_5" },
  { DEFINITE_6, "CARD_DEF_6" },
  { DEFINITE_7, "CARD_DEF_7" },
  { DEFINITE_8, "CARD_DEF_8" },
  { DEFINITE_9, "CARD_DEF_9" },
  { DEFINITE_TEN, "CARD_DEF_TEN" },
  { DEFINITE_JACK, "CARD_DEF_JACK" },
  { DEFINITE_QUEEN, "CARD_DEF_QUEEN" },
  { DEFINITE_KING, "CARD_DEF_KING" },
  { DEFINITE_ACE, "CARD_DEF_ACE" }
};

list<TagConnection> definiteGroupConnection =
{
  { GROUP_DEFINITE, "GROUP_DEFINITE" }
};


list<VerbalConnection> indefiniteConnection =
{
  { INDEFINITE_2, "CARD_INDEF_2" },
  { INDEFINITE_3, "CARD_INDEF_3" },
  { INDEFINITE_4, "CARD_INDEF_4" },
  { INDEFINITE_5, "CARD_INDEF_5" },
  { INDEFINITE_6, "CARD_INDEF_6" },
  { INDEFINITE_7, "CARD_INDEF_7" },
  { INDEFINITE_8, "CARD_INDEF_8" },
  { INDEFINITE_9, "CARD_INDEF_9" },
  { INDEFINITE_TEN, "CARD_INDEF_TEN" },
  { INDEFINITE_JACK, "CARD_INDEF_JACK" },
  { INDEFINITE_QUEEN, "CARD_INDEF_QUEEN" },
  { DEFINITE_KING, "CARD_INDEF_KING" },
  { INDEFINITE_ACE, "CARD_INDEF_ACE" }
};

list<TagConnection> indefiniteGroupConnection =
{
  { GROUP_INDEFINITE, "GROUP_INDEFINITE" }
};


list<VerbalConnection> prepositionConnection =
{
  { PREP_OF_2, "PREP_OF_2" },
  { PREP_OF_3, "PREP_OF_3" },
  { PREP_OF_4, "PREP_OF_4" },
  { PREP_OF_5, "PREP_OF_5" },
  { PREP_OF_6, "PREP_OF_6" },
  { PREP_OF_7, "PREP_OF_7" },
  { PREP_OF_8, "PREP_OF_8" },
  { PREP_OF_9, "PREP_OF_9" },
  { PREP_OF_TEN, "PREP_OF_TEN" },
  { PREP_OF_JACK, "PREP_OF_JACK" },
  { PREP_OF_QUEEN, "PREP_OF_QUEEN" },
  { PREP_OF_KING, "PREP_OF_KING" },
  { PREP_OF_ACE, "PREP_OF_ACE" }
};

list<TagConnection> prepositionGroupConnection =
{
  { GROUP_PREP_LOCAL, "GROUP_PREPOSITION" }
};


list<VerbalConnection> cardsShortConnection =
{
  { SHORT_2, "CARD_SHORT_2" },
  { SHORT_3, "CARD_SHORT_3" },
  { SHORT_4, "CARD_SHORT_4" },
  { SHORT_5, "CARD_SHORT_5" },
  { SHORT_6, "CARD_SHORT_6" },
  { SHORT_7, "CARD_SHORT_7" },
  { SHORT_8, "CARD_SHORT_8" },
  { SHORT_9, "CARD_SHORT_9" },
  { SHORT_TEN, "CARD_SHORT_TEN" },
  { SHORT_JACK, "CARD_SHORT_JACK" },
  { SHORT_QUEEN, "CARD_SHORT_QUEEN" },
  { SHORT_KING, "CARD_SHORT_KING" },
  { SHORT_ACE, "CARD_SHORT_ACE" }
};

list<TagConnection> cardsShortGroupConnection =
{
  { GROUP_SHORT, "GROUP_SHORT" }
};


list<VerbalConnection> honorsShortConnection =
{
  { HONOR_SHORT_1, "HONOR_SHORT_1" },
  { HONOR_SHORT_2, "HONOR_SHORT_2" },
  { HONOR_SHORT_3, "HONOR_SHORT_3" },
  { HONOR_SHORT_4, "HONOR_SHORT_4" },
  { HONOR_SHORT_5, "HONOR_SHORT_5" },
  { HONOR_SHORT_6, "HONOR_SHORT_6" }
};

list<TagConnection> honorsShortGroupConnection =
{
  { GROUP_HONOR_SHORT, "GROUP_HONOR_SHORT" }
};


list<VerbalConnection> numeralsConnection =
{
  { NUMERAL_0, "NUMERAL_0" },
  { NUMERAL_1, "NUMERAL_1" },
  { NUMERAL_2, "NUMERAL_2" },
  { NUMERAL_3, "NUMERAL_3" },
  { NUMERAL_4, "NUMERAL_4" },
  { NUMERAL_5, "NUMERAL_5" },
  { NUMERAL_6, "NUMERAL_6" },
  { NUMERAL_7, "NUMERAL_7" },
  { NUMERAL_8, "NUMERAL_8" },
  { NUMERAL_9, "NUMERAL_9" },
  { NUMERAL_10, "NUMERAL_10" },
  { NUMERAL_11, "NUMERAL_11" },
  { NUMERAL_12, "NUMERAL_12" },
  { NUMERAL_13, "NUMERAL_13" }
};

list<TagConnection> numeralsGroupConnection =
{
  { GROUP_NUMERAL, "GROUP_NUMERAL" }
};


list<VerbalConnection> ordinalsConnection =
{
  { ORDINAL_0, "ORDINAL_0" },
  { ORDINAL_1, "ORDINAL_1" },
  { ORDINAL_2, "ORDINAL_2" },
  { ORDINAL_3, "ORDINAL_3" },
  { ORDINAL_4, "ORDINAL_4" },
  { ORDINAL_5, "ORDINAL_5" },
  { ORDINAL_6, "ORDINAL_6" },
  { ORDINAL_7, "ORDINAL_7" },
  { ORDINAL_8, "ORDINAL_8" },
  { ORDINAL_9, "ORDINAL_9" },
  { ORDINAL_10, "ORDINAL_10" },
  { ORDINAL_11, "ORDINAL_11" },
  { ORDINAL_12, "ORDINAL_12" },
  { ORDINAL_13, "ORDINAL_13" }
};

list<TagConnection> ordinalsGroupConnection =
{
  { GROUP_ORDINAL, "GROUP_ORDINAL" }
};



list<VerbalConnection> wordsConnection =
{
  { WORDS_CARD, "CARD" },
  { WORDS_CARDS, "CARDS" },

  { WORDS_HONOR, "HONOR" },
  { WORDS_HONORS, "HONORS" },
  { WORDS_MID_HONOR, "MID_HONOR" },
  { WORDS_MID_HONORS, "MID_HONORS" },

  { WORDS_SMALL, "SMALL" },

  { WORDS_CONJUNCTION, "CONJUNCTION_OR" },

  { WORDS_PARTICLE_DEF_PLURAL, "PARTICLE_DEF_PLURAL" },

  { WORDS_DEF_PLURAL_OF, "PREPOSITION_DEF_PLURAL_OF" },

  { WORDS_THEY_HOLD, "THEY_HOLD" }
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


void Dictionary::setPhraseTags()
{
  phraseTags.resize(PHRASE_SIZE);
  phraseTags[PHRASE_NONE] = "{NONE%}";
  phraseTags[PHRASE_DIGITS] = "{DIGITS%}";
  phraseTags[PHRASE_NUMERICAL] = "{NUMERICAL%}";
  phraseTags[PHRASE_ORDINAL] = "{ORDINAL%}";
  phraseTags[PHRASE_LOWEST_CARD] = "{LOWEST%}";
  phraseTags[PHRASE_INDEFINITE_RANK] = "{INDEFINITE%}";
  phraseTags[PHRASE_DEFINITE_RANK] = "{DEFINITE%}";
  phraseTags[PHRASE_OF_DEFINITE_RANK] = "{OF_DEFINITE%}";
  phraseTags[PHRASE_COMPLETION_SET] = "{TOPS%}";
  phraseTags[PHRASE_COMPLETION_UNSET] = "{LOWS%}";
  phraseTags[PHRASE_XES] = "{XES%}";

  // TODO When we add expansions of sentences as well,
  // they might to in the same map?
  phraseExpansionGroup["DIGITS"] = PHRASE_DIGITS;
  phraseExpansionGroup["NUMERICAL"] = PHRASE_NUMERICAL;
  phraseExpansionGroup["ORDINAL"] = PHRASE_ORDINAL;
  phraseExpansionGroup["LOWEST"] = PHRASE_LOWEST_CARD;
  phraseExpansionGroup["INDEFINITE"] = PHRASE_INDEFINITE_RANK;
  phraseExpansionGroup["DEFINITE"] = PHRASE_DEFINITE_RANK;
  phraseExpansionGroup["OF_DEFINITE"] = PHRASE_OF_DEFINITE_RANK;
  phraseExpansionGroup["TOPS"] = PHRASE_COMPLETION_SET;
  phraseExpansionGroup["LOWS"] = PHRASE_COMPLETION_UNSET;
  phraseExpansionGroup["XES"] = PHRASE_XES;
}


void Dictionary::read(const string& language)
{
  Dictionary::setPhraseTags();

  Dictionary::setMap(sentencesGroupConnection, sentenceGroupMap);
  coverSentences.init(sentencesConnection);
  coverSentences.read(language, "cover/sentences.txt", 
    sentenceGroupMap);

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
  cardsShort.read(language, "cards/short.txt", cardsShortGroupMap);

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


const string& Dictionary::phraseTag(const unsigned index) const
{
  assert(index < phraseTags.size());
  return phraseTags[index];
}


unsigned Dictionary::phraseGroup(const string& tag) const
{
  const auto pit = phraseExpansionGroup.find(tag);
  assert(pit != phraseExpansionGroup.end());
  return pit->second;
}

