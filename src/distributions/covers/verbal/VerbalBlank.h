/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_VERBALBLANK_H
#define SSS_VERBALBLANK_H

using namespace std;


// A sentence consists of phrases.
// Each phrase has a possible set of phrase instances.
// Each phrase instance has a parameters to instruct its expansion.

enum Sentence: unsigned
{
  SENTENCE_LENGTH_ONLY = 0,
  SENTENCE_TOPS_ONLY = 1,
  SENTENCE_ONETOP = 2,
  SENTENCE_TOPS_LENGTH = 3,
  SENTENCE_TOPS_EXCLUDING = 4,
  SENTENCE_TOPS_AND_XES = 5,
  SENTENCE_TOPS_AND_LOWER = 6,
  SENTENCE_ONLY_BELOW = 7,
  SENTENCE_LIST = 8,
  SENTENCE_SIZE = 9
};

enum PhraseCategory: unsigned
{
  PHRASE_LENGTH_VERB_OBSOLETE = 0,
  PHRASE_LENGTH_ADJ = 1,
  PHRASE_COUNT = 2,
  PHRASE_TOPS = 3,
  PHRASE_TOPS_PHRASE = 4,
  PHRASE_BOTTOMS = 5,
  PHRASE_PLAYER_CAP = 6,
  PHRASE_EXCLUDING = 7,
  PHRASE_BELOW = 8,
  PHRASE_LIST_PHRASE = 9, // TODO Goes away
  PHRASE_LENGTH_ORDINAL = 10, // TODO Becomes #1 later on again
  PHRASE_LENGTH_VERB = 11, // TODO Renumber to #0 again later
  PHRASE_LIST = 12,
  PHRASE_SIZE = 13
};


enum SlotExpansion: unsigned
{
  SLOT_NONE = 0,
  SLOT_NUMERICAL = 1,
  SLOT_RANKS = 2,
  SLOT_COMPLETION_SET = 3,
  SLOT_COMPLETION_UNSET = 4,
  SLOT_COMPLETION_BOTH = 5,
  SLOT_COMPLETION_XES = 6,
  SLOT_COMPLETION_UNCLEAR = 7,
  SLOT_TEXT_LOWER = 8,
  SLOT_TEXT_BELOW = 9,
  SLOT_ORDINAL = 10, // Re-sort after NUMERICAL
  SLOT_RANGE_OF = 11,
  SLOT_SOME_OF = 12,
  SLOT_LENGTH = 13
};




#define BLANK_MAX_VERSIONS 15

enum VerbalBlank: unsigned
{
  BLANK_LENGTH_VERB = 0,
  BLANK_LENGTH_ADJ = 1,
  BLANK_COUNT = 2,
  BLANK_TOPS = 3,
  BLANK_TOPS_PHRASE = 4,
  BLANK_BOTTOMS = 5,
  BLANK_PLAYER_CAP = 6,
  BLANK_EXCLUDING = 7,
  BLANK_BELOW = 8,
  BLANK_LIST_PHRASE = 9,
  BLANK_SIZE = 10
};


enum BlankPlayerCap: unsigned
{
  BLANK_PLAYER_CAP_WEST = 0,
  BLANK_PLAYER_CAP_EAST = 1,
  BLANK_PLAYER_CAP_EITHER = 2,
  BLANK_PLAYER_CAP_EACH = 3,
  BLANK_PLAYER_CAP_NEITHER = 4,
  BLANK_PLAYER_CAP_SUIT = 5
};

enum BlankLengthVerb: unsigned
{
  BLANK_LENGTH_VERB_VOID = 0,
  BLANK_LENGTH_VERB_SINGLE = 1,
  BLANK_LENGTH_VERB_DOUBLE = 2,
  BLANK_LENGTH_VERB_TRIPLE = 3,
  BLANK_LENGTH_VERB_EVENLY = 4,
  BLANK_LENGTH_VERB_ODD_EVENLY = 5,
  BLANK_LENGTH_VERB_SINGLE_ATMOST = 6,
  BLANK_LENGTH_VERB_DOUBLE_ATMOST = 7,
  BLANK_LENGTH_VERB_TRIPLE_ATMOST = 8,
  BLANK_LENGTH_VERB_CARDS_PARAM = 9,
  BLANK_LENGTH_VERB_CARDS_ATMOST_PARAM = 10,
  BLANK_LENGTH_VERB_RANGE_PARAMS = 11,
  BLANK_LENGTH_VERB_SPLIT_PARAMS = 12
};

enum BlankLengthAdj: unsigned
{
  BLANK_LENGTH_ADJ_SINGLE = 0,
  BLANK_LENGTH_ADJ_DOUBLE = 1,
  BLANK_LENGTH_ADJ_TRIPLE = 2,
  BLANK_LENGTH_ADJ_LONG = 3,
  BLANK_LENGTH_ADJ_SINGLE_ATMOST = 4,
  BLANK_LENGTH_ADJ_DOUBLE_ATMOST = 5,
  BLANK_LENGTH_ADJ_TRIPLE_ATMOST = 6,
  BLANK_LENGTH_ADJ_LONG_ATMOST = 7,
  BLANK_LENGTH_ADJ_23 = 8
};

enum LengthOrdinal: unsigned
{
  LENGTH_ORDINAL_EXACT = 0,
  LENGTH_ORDINAL_ATMOST = 1,
  LENGTH_ORDINAL_23 = 2
};

enum BlankCount: unsigned
{
  BLANK_COUNT_EQUAL = 0,
  BLANK_COUNT_ATMOST = 1,
  BLANK_COUNT_RANGE_PARAMS = 2
};

enum BlankTops: unsigned
{
  BLANK_TOPS_ONE_ATMOST = 0,
  BLANK_TOPS_ONE_ATLEAST = 1,
  BLANK_TOPS_ONE_RANGE_PARAMS = 2,
  BLANK_TOPS_ACTUAL = 3,
  BLANK_TOPS_LOWER = 4,
  BLANK_TOPS_BELOW = 5
};

enum BlankBottoms: unsigned
{
  BLANK_BOTTOMS_NORMAL = 0
};

enum BlankExcluding: unsigned
{
  BLANK_EXCLUDING_NONE = 0,
  BLANK_EXCLUDING_NEITHER = 1,
  BLANK_EXCLUDING_NOT = 2
};

enum BlankBelow: unsigned
{
  BLANK_BELOW_NORMAL = 0,
  BLANK_BELOW_COMPLETELY = 1
};

enum BlankTopsPhrase: unsigned
{
  BLANK_TOPS_PHRASE_HOLDING = 0
};

enum BlankListPhrase: unsigned
{
  BLANK_LIST_PHRASE_HOLDING = 0
};

enum PhraseList: unsigned
{
  LIST_HOLDING_EXACT = 0,
  LIST_HOLDING_WITH_LOWS = 1
};

enum LengthVerb: unsigned
{
  LENGTH_VERB_VOID = 0,
  LENGTH_VERB_XTON = 1,
  LENGTH_VERB_EVENLY = 2,
  LENGTH_VERB_ODD_EVENLY = 3,
  LENGTH_VERB_XTON_ATMOST = 4,
  LENGTH_VERB_CARDS = 5,
  LENGTH_VERB_CARDS_ATMOST = 6,
  LENGTH_VERB_RANGE = 7,
  LENGTH_VERB_SPLIT = 8
};

#endif
