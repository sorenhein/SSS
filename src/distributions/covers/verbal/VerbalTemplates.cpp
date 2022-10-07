/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "VerbalTemplates.h"
#include "Slot.h"
#include "VerbalBlank.h"

#include "VerbalMap.h"



VerbalTemplates::VerbalTemplates()
{
  VerbalTemplates::reset();
}


void VerbalTemplates::reset()
{
  templates.clear();
}


void VerbalTemplates::set(const Language languageIn)
{
  instanceToGroup.resize(VERBAL_PHRASE_SIZE);
  instanceToExpansion.resize(VERBAL_PHRASE_SIZE);
  instanceToText.resize(VERBAL_PHRASE_SIZE);

  for (auto& vm: verbalMap)
  {
    instanceToGroup[vm.instance] = vm.group;
    instanceToExpansion[vm.instance] = vm.expansion;
    instanceToText[vm.instance] = vm.text;
  }



  templates.resize(SENTENCE_SIZE);
  language = languageIn;

  if (language == LANGUAGE_ENGLISH_US)
  {
    templates[SENTENCE_LENGTH_ONLY] =
      { "%0 %1", { BLANK_PLAYER_CAP, BLANK_LENGTH_VERB },
      { GROUP_PLAYER, GROUP_LENGTH_VERB} };

    templates[SENTENCE_TOPS_ONLY] =
      { "%0 %1", { BLANK_PLAYER_CAP, BLANK_TOPS_PHRASE },
      { GROUP_PLAYER, GROUP_PLAYER }}; // TODO Unused?

    templates[SENTENCE_ONETOP] =
      { "%0 has %1", { BLANK_PLAYER_CAP, BLANK_TOPS},
      { GROUP_PLAYER, GROUP_TOPS }};

    templates[SENTENCE_TOPS_LENGTH] =
      { "%0 has %1 %2", { BLANK_PLAYER_CAP, BLANK_TOPS, 
        BLANK_LENGTH_ADJ},
        { GROUP_PLAYER, GROUP_TOPS, GROUP_LENGTH_ORDINAL }};

    templates[SENTENCE_TOPS_EXCLUDING] =
      { "%0 has %1 and %2 %3", { BLANK_PLAYER_CAP, BLANK_TOPS, 
        BLANK_EXCLUDING, BLANK_TOPS},
        { GROUP_PLAYER, GROUP_TOPS, GROUP_EXCLUDING, GROUP_TOPS }};

    templates[SENTENCE_TOPS_AND_XES] =
      { "%0 has %1%2", { BLANK_PLAYER_CAP, BLANK_TOPS, 
        BLANK_BOTTOMS},
        { GROUP_PLAYER, GROUP_TOPS, GROUP_BOTTOMS }};

    templates[SENTENCE_TOPS_AND_LOWER] =
      { "%0 has %1 and %2%3", { BLANK_PLAYER_CAP, BLANK_TOPS, 
        BLANK_COUNT, BLANK_TOPS},
        { GROUP_PLAYER, GROUP_TOPS, GROUP_COUNT, GROUP_TOPS }};

    // TODO The last one could be a different type to tell us
    // to look up ranksNames.lowestCard or something like it.
    templates[SENTENCE_ONLY_BELOW] =
      { "%0 %1 %2 %3", { BLANK_PLAYER_CAP, BLANK_LENGTH_VERB, 
        BLANK_BELOW, BLANK_TOPS},
        { GROUP_PLAYER, GROUP_LENGTH_VERB, GROUP_BELOW, GROUP_TOPS }};

    // Up to 4 such holdings currently foreseen.
    templates[SENTENCE_LIST] =
      { "%0 has %1, %2, %3, %4", { BLANK_PLAYER_CAP, 
        BLANK_LIST_PHRASE, BLANK_LIST_PHRASE,
        BLANK_LIST_PHRASE, BLANK_LIST_PHRASE },
        { GROUP_PLAYER, GROUP_LIST, GROUP_LIST, GROUP_LIST, GROUP_LIST }};
  }
  else if (language == LANGUAGE_GERMAN_DE)
  {
    assert(false);
  }
  else
    assert(false);

  /*
  dictionary.resize(PHRASE_SIZE);
  for (auto& dict: dictionary)
    dict.resize(BLANK_MAX_VERSIONS);

  // X
  auto& blankPlayerCap = dictionary[BLANK_PLAYER_CAP];
  blankPlayerCap[BLANK_PLAYER_CAP_WEST] = "West";
  blankPlayerCap[BLANK_PLAYER_CAP_EAST] = "East";
  blankPlayerCap[BLANK_PLAYER_CAP_EITHER] = "Either opponent";
  blankPlayerCap[BLANK_PLAYER_CAP_EACH] = "Each opponent";
  blankPlayerCap[BLANK_PLAYER_CAP_NEITHER] = "Neither opponent";
  blankPlayerCap[BLANK_PLAYER_CAP_SUIT] = "The suit";

  // X
  auto& blankLP = dictionary[BLANK_LENGTH_VERB];
  blankLP[BLANK_LENGTH_VERB_VOID] = "is void";
  blankLP[BLANK_LENGTH_VERB_SINGLE] = "has a singleton";
  blankLP[BLANK_LENGTH_VERB_DOUBLE] = "has a doubleton";
  blankLP[BLANK_LENGTH_VERB_TRIPLE] = "has a tripleton";
  blankLP[BLANK_LENGTH_VERB_EVENLY] = "splits evenly";
  blankLP[BLANK_LENGTH_VERB_ODD_EVENLY] = "splits evenly either way";
  blankLP[BLANK_LENGTH_VERB_SINGLE_ATMOST] = "has at most a singleton";
  blankLP[BLANK_LENGTH_VERB_DOUBLE_ATMOST] = "has at most a doubleton";
  blankLP[BLANK_LENGTH_VERB_TRIPLE_ATMOST] = "has at most a tripleton";
  blankLP[BLANK_LENGTH_VERB_CARDS_PARAM] = "has %0 cards";
  blankLP[BLANK_LENGTH_VERB_CARDS_ATMOST_PARAM] = "has at most %0 cards";
  blankLP[BLANK_LENGTH_VERB_RANGE_PARAMS] = "has %0-%1 cards";
  blankLP[BLANK_LENGTH_VERB_SPLIT_PARAMS] = "splits %0=%1";

  // X
  auto& blankPLV = dictionary[PHRASE_LENGTH_VERB];
  blankPLV[XLENGTH_VERB_VOID] = "is void";
  blankPLV[XLENGTH_VERB_XTON] = "has a %0";
  blankPLV[XLENGTH_VERB_EVENLY] = "splits evenly";
  blankPLV[XLENGTH_VERB_ODD_EVENLY] = "splits evenly either way";
  blankPLV[XLENGTH_VERB_XTON_ATMOST] = "has at most a %0";
  blankPLV[XLENGTH_VERB_CARDS] = "has %0 cards";
  blankPLV[XLENGTH_VERB_CARDS_ATMOST] = "has at most %0 cards";
  blankPLV[XLENGTH_VERB_RANGE] = "has %0-%1 cards";
  blankPLV[XLENGTH_VERB_SPLIT] = "splits %0=%1";

  // X
  auto& blankLPA = dictionary[BLANK_LENGTH_ADJ];
  blankLPA[BLANK_LENGTH_ADJ_SINGLE] = "singleton";
  blankLPA[BLANK_LENGTH_ADJ_DOUBLE] = "doubleton";
  blankLPA[BLANK_LENGTH_ADJ_TRIPLE] = "tripleton";
  blankLPA[BLANK_LENGTH_ADJ_LONG] = "%0";
  blankLPA[BLANK_LENGTH_ADJ_SINGLE_ATMOST] = "at most singleton";
  blankLPA[BLANK_LENGTH_ADJ_DOUBLE_ATMOST] = "at most doubleton";
  blankLPA[BLANK_LENGTH_ADJ_TRIPLE_ATMOST] = "at most tripleton";
  blankLPA[BLANK_LENGTH_ADJ_LONG_ATMOST] = "at most %0";
  blankLPA[BLANK_LENGTH_ADJ_23] = "doubleton or tripleton";

  // X
  auto& blankLPO = dictionary[PHRASE_LENGTH_ORDINAL];
  blankLPO[XLENGTH_ORDINAL_EXACT] = "%0";
  blankLPO[XLENGTH_ORDINAL_ATMOST] = "at most %0";
  blankLPO[XLENGTH_ORDINAL_23] = "doubleton or tripleton";

  // X
  auto& blankC = dictionary[BLANK_COUNT];
  blankC[BLANK_COUNT_EQUAL] = "%0";
  blankC[BLANK_COUNT_ATMOST] = "at most %0";
  blankC[BLANK_COUNT_RANGE_PARAMS] = "%0-%1";

  // X
  auto& blank1TP = dictionary[BLANK_TOPS];
  blank1TP[BLANK_TOPS_ONE_ATMOST] = "at most %0 of %1";
  blank1TP[BLANK_TOPS_ONE_ATLEAST] = "at least %0 of %1";
  blank1TP[BLANK_TOPS_ONE_RANGE_PARAMS] = "%0-%1 of %2";
  blank1TP[BLANK_TOPS_ACTUAL] = "%0";
  blank1TP[BLANK_TOPS_LOWER] = ", lower-ranked card%0";
  blank1TP[BLANK_TOPS_BELOW] = " card%0 below the %1";

  auto& blankBot = dictionary[BLANK_BOTTOMS];
  blankBot[BLANK_BOTTOMS_NORMAL] = "%0";

  // X
  auto& blankEx = dictionary[BLANK_EXCLUDING];
  blankEx[BLANK_EXCLUDING_NONE] = "none of";
  blankEx[BLANK_EXCLUDING_NEITHER] = "neither of";
  blankEx[BLANK_EXCLUDING_NOT] = "not";

  // X
  auto& blankBel = dictionary[BLANK_BELOW];
  blankBel[BLANK_BELOW_NORMAL] = "below the";
  blankBel[BLANK_BELOW_COMPLETELY] = "completely below the";

  auto& blankTP = dictionary[BLANK_TOPS_PHRASE];
  blankTP[BLANK_TOPS_PHRASE_HOLDING] = "%0";

  auto& blankLiP = dictionary[BLANK_LIST_PHRASE];
  blankLiP[BLANK_LIST_PHRASE_HOLDING] = "%0";

  auto& blankPL = dictionary[PHRASE_LIST];
  blankPL[XLIST_HOLDING_EXACT] = "%0";
  blankPL[XLIST_HOLDING_WITH_LOWS] = "%0(%1)";
  */
}


string VerbalTemplates::get(
  const Sentence sentence,
  const RanksNames& ranksNames,
  const list<Completion>& completions,
  const vector<Slot>& slots) const
{
  assert(sentence < templates.size());
  const VerbalTemplate& vt = templates[sentence];

  assert(slots.size() <= vt.blanks.size());

  if (sentence != SENTENCE_LIST)
    assert(slots.size() == vt.blanks.size());

  string s = vt.pattern;
  string fill = "";

  size_t field;
  auto slotIter = slots.begin();
  auto complIter = completions.begin();
  auto giter = vt.groups.begin();

  for (field = 0; field < slots.size(); field++, slotIter++, giter++)
  {
    const Slot& slot = * slotIter;

    // TODO Should we have a slot.phrase() == effectively blank?

    assert(complIter != completions.end());

    if (slot.isNew())
    {
      const VerbalPhrase vp = slot.getPhrase();
      assert(vp < instanceToGroup.size());
      const VerbalGroup vg = instanceToGroup[vp];
      assert(vg == * giter);

      fill = slot.str(
        instanceToExpansion, 
        instanceToText,
        ranksNames,
        * complIter);
      if (slot.getPhrase() == LIST_HOLDING_EXACT)
        complIter++;
    }
    else
    {
      assert(false);
      fill = slot.str(dictionary, ranksNames, * complIter);
      if (slot.phrase() == PHRASE_LIST)
        complIter++;
    }






    auto p = s.find("%" + to_string(field));
    assert(p != string::npos);
    s.replace(p, 2, fill);
  }

  if (sentence == SENTENCE_LIST)
  {
    // Eliminate the trailing % fields.
    for ( ; field < vt.blanks.size(); field++)
    {
      auto p = s.find(", %" + to_string(field));
      assert(p != string::npos);
      s.erase(p, 4);
    }
  }

  // If there is a comma, turn the last one into " or".
  if (sentence != SENTENCE_TOPS_AND_LOWER)
  {
    auto p = s.find_last_of(",");
    if (p != string::npos)
      s.replace(p, 1, " or");
  }

  return s;
}

