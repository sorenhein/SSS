/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_VERBALMAP_H
#define SSS_VERBALMAP_H


using namespace std;

#include "Slot.h"
#include "VerbalDimensions.h"


struct VerbalElement
{
  VerbalPhrase instance;
  string tag;
  VerbalGroup group;
  SlotExpansion expansion;
  string text;
};


list<VerbalElement> verbalMap =
{
  { PLAYER_WEST, "PLAYER_WEST", GROUP_PLAYER, 
    SLOT_NONE, "West" },
  { PLAYER_EAST, "PLAYER_EAST", GROUP_PLAYER, 
    SLOT_NONE, "East" },
  { PLAYER_EITHER, "PLAYER_EITHER", GROUP_PLAYER, 
    SLOT_NONE, "Either opponent" },
  { PLAYER_EACH, "PLAYER_EACH", GROUP_PLAYER, 
    SLOT_NONE, "Each opponent" },
  { PLAYER_NEITHER, "PLAYER_NEITHER", GROUP_PLAYER, 
    SLOT_NONE, "Neither opponent" },
  { PLAYER_SUIT, "PLAYER_SUIT", GROUP_PLAYER, 
    SLOT_NONE, "The suit" },

  { LENGTH_VERB_VOID, "LENGTH_VERB_VOID", GROUP_LENGTH_VERB, 
    SLOT_NONE, "is void" },
  { LENGTH_VERB_XTON, "LENGTH_VERB_XTON", GROUP_LENGTH_VERB, 
    SLOT_ORDINAL, "has a %0" },
  { LENGTH_VERB_EVENLY, "LENGTH_VERB_EVENLY", GROUP_LENGTH_VERB, 
    SLOT_NONE, "splits evenly" },
  { LENGTH_VERB_ODD_EVENLY, "LENGTH_VERB_ODD_EVENLY", GROUP_LENGTH_VERB, 
    SLOT_NONE, "splits evenly either way" },
  { LENGTH_VERB_XTON_ATMOST, "LENGTH_VERB_XTON_ATMOST", GROUP_LENGTH_VERB, 
    SLOT_ORDINAL, "has at most a %0" },
  { LENGTH_VERB_CARDS, "LENGTH_VERB_CARDS", GROUP_LENGTH_VERB, 
    SLOT_NUMERICAL, "has %0 cards" },
  { LENGTH_VERB_ATMOST, "LENGTH_VERB_ATMOST", GROUP_LENGTH_VERB,
    SLOT_NUMERICAL, "has at most %0 cards" },
  { LENGTH_VERB_12, "LENGTH_VERB_12", GROUP_LENGTH_VERB, 
    SLOT_NONE, "has a singleton or doubleton" },
  { LENGTH_VERB_RANGE, "LENGTH_VERB_RANGE", GROUP_LENGTH_VERB, 
    SLOT_NUMERICAL, "has %0-%1 cards" },
  { LENGTH_VERB_SPLIT, "LENGTH_VERB_SPLIT", GROUP_LENGTH_VERB, 
    SLOT_NUMERICAL, "splits %0=%1" },

  { LENGTH_ORDINAL_EXACT, "LENGTH_ORDINAL_EXACT", GROUP_LENGTH_ORDINAL, 
    SLOT_ORDINAL, "%0" },
  { LENGTH_ORDINAL_ATMOST, "LENGTH_ORDINAL_ATMOST", GROUP_LENGTH_ORDINAL, 
    SLOT_ORDINAL, "at most %0" },
  { LENGTH_ORDINAL_ADJACENT, "LENGTH_ORDINAL_ADJACENT", GROUP_LENGTH_ORDINAL, 
    SLOT_ORDINAL, "%0 or %1" },
  { LENGTH_ORDINAL_RANGE, "LENGTH_ORDINAL_RANGE", GROUP_LENGTH_ORDINAL, 
    SLOT_ORDINAL, "%0 to %1" },

  { COUNT_EXACT, "COUNT_EXACT", GROUP_COUNT, 
    SLOT_NUMERICAL, "%0" },
  { COUNT_ATMOST, "COUNT_ATMOST", GROUP_COUNT, 
    SLOT_NUMERICAL, "at most %0" },
  { COUNT_RANGE, "COUNT_RANGE", GROUP_COUNT, 
    SLOT_NUMERICAL, "%0-%1" },

  { TOPS_ATMOST, "TOPS_ATMOST", GROUP_TOPS, 
    SLOT_SOME_OF, "at most %0 of %1" },
  { TOPS_ATLEAST, "TOPS_ATLEAST", GROUP_TOPS, 
    SLOT_SOME_OF, "at least %0 of %1" },
  { TOPS_RANGE, "TOPS_RANGE", GROUP_TOPS, 
    SLOT_RANGE_OF, "%0-%1 of %2" },
  { TOPS_ACTUAL, "TOPS_ACTUAL", GROUP_TOPS, 
    SLOT_COMPLETION_SET, "%0" },
  { TOPS_RANKS, "TOPS_RANKS", GROUP_TOPS, 
    SLOT_RANKS, "%0" },
  { TOPS_LOWER, "TOPS_LOWER", GROUP_TOPS, 
    SLOT_TEXT_LOWER, ", lower-ranked %0" },
  { TOPS_BELOW, "TOPS_BELOW", GROUP_TOPS, 
    SLOT_TEXT_BELOW, " %0 below the %1" },

  { EXCLUDING_NONE, "EXCLUDING_NONE", GROUP_EXCLUDING, 
    SLOT_NONE, "none of" },
  { EXCLUDING_NEITHER, "EXCLUDING_NEITHER", GROUP_EXCLUDING, 
    SLOT_NONE, "neither of" },
  { EXCLUDING_NOT, "EXCLUDING_NOT", GROUP_EXCLUDING, 
    SLOT_NONE, "not" },

  { BELOW_NORMAL, "BELOW_NORMAL", GROUP_BELOW, 
    SLOT_NONE, "below the" },
  { BELOW_COMPLETELY, "BELOW_COMPLETELY", GROUP_BELOW, 
    SLOT_NONE, "completely below the" },

  { BOTTOMS_NORMAL, "BOTTOMS_NORMAL", GROUP_BOTTOMS, 
    SLOT_COMPLETION_XES, "%0" },

  { LIST_HOLDING_EXACT, "LIST_HOLDING_EXACT", GROUP_LIST, 
    SLOT_COMPLETION_SET, "%0" },
  { LIST_HOLDING_WITH_LOWS, "LIST_HOLDING_WITH_LOWS", GROUP_LIST, 
    SLOT_COMPLETION_BOTH, "%0(%1)" }
};

#endif
