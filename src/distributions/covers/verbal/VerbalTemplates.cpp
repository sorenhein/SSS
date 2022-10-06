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

#include "../../../ranks/RanksNames.h"
#include "../../../utils/table.h"

const vector<string> topCount =
{
  "none",
  "one",
  "two",
  "three",
  "four",
  "five",
  "six",
  "seven",
  "eight",
  "nine",
  "ten",
  "eleven",
  "twelve",
  "thirteen"
};

const vector<string> topOrdinal =
{
  "void",
  "singleton",
  "doubleton",
  "tripleton",
  "fourth",
  "fifth",
  "sixth",
  "seventh",
  "eighth",
  "ninth",
  "tenth",
  "eleventh",
  "twelfth",
  "thirteenth"
};



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
  templates.resize(SENTENCE_SIZE);
  language = languageIn;

  if (language == LANGUAGE_ENGLISH_US)
  {
    templates[SENTENCE_LENGTH_ONLY] =
      { "%0 %1", { BLANK_PLAYER_CAP, BLANK_LENGTH_VERB }};

    templates[SENTENCE_TOPS_ONLY] =
      { "%0 %1", { BLANK_PLAYER_CAP, BLANK_TOPS_PHRASE }};

    templates[SENTENCE_ONETOP] =
      { "%0 has %1", { BLANK_PLAYER_CAP, BLANK_TOPS}};

    templates[SENTENCE_TOPS_LENGTH] =
      { "%0 has %1 %2", { BLANK_PLAYER_CAP, BLANK_TOPS, 
        BLANK_LENGTH_ADJ}};

    templates[SENTENCE_TOPS_EXCLUDING] =
      { "%0 has %1 and %2 %3", { BLANK_PLAYER_CAP, BLANK_TOPS, 
        BLANK_EXCLUDING, BLANK_TOPS}};

    templates[SENTENCE_TOPS_AND_XES] =
      { "%0 has %1%2", { BLANK_PLAYER_CAP, BLANK_TOPS, 
        BLANK_BOTTOMS}};

    templates[SENTENCE_TOPS_AND_LOWER] =
      { "%0 has %1 and %2%3", { BLANK_PLAYER_CAP, BLANK_TOPS, 
        BLANK_COUNT, BLANK_TOPS}};

    // TODO The last one could be a different type to tell us
    // to look up ranksNames.lowestCard or something like it.
    templates[SENTENCE_ONLY_BELOW] =
      { "%0 %1 %2 %3", { BLANK_PLAYER_CAP, BLANK_LENGTH_VERB, 
        BLANK_BELOW, BLANK_TOPS}};

    // Up to 4 such holdings currently foreseen.
    templates[SENTENCE_LIST] =
      { "%0 has %1, %2, %3, %4", { BLANK_PLAYER_CAP, 
        BLANK_LIST_PHRASE, BLANK_LIST_PHRASE,
        BLANK_LIST_PHRASE, BLANK_LIST_PHRASE }};
  }
  else if (language == LANGUAGE_GERMAN_DE)
  {
    templates[SENTENCE_LENGTH_ONLY] =
      { "%0 %1", { BLANK_PLAYER_CAP, BLANK_LENGTH_VERB }};

    templates[SENTENCE_TOPS_ONLY] =
      { "%0 %1", { BLANK_PLAYER_CAP, BLANK_TOPS_PHRASE }};

    templates[SENTENCE_ONETOP] =
      { "%0 has %1", { BLANK_PLAYER_CAP, BLANK_TOPS}};

    templates[SENTENCE_TOPS_LENGTH] =
      { "%0 has %1 %2", { BLANK_PLAYER_CAP, BLANK_TOPS, 
        BLANK_LENGTH_ADJ}};

    templates[SENTENCE_TOPS_EXCLUDING] =
      { "%0 has %1 and %2%3", { BLANK_PLAYER_CAP, BLANK_TOPS, 
        BLANK_EXCLUDING, BLANK_LENGTH_ADJ}};

    templates[SENTENCE_TOPS_AND_XES] =
      { "%0 has %1%2", { BLANK_PLAYER_CAP, BLANK_TOPS, 
        BLANK_BOTTOMS}};

    templates[SENTENCE_TOPS_AND_LOWER] =
      { "%0 has %1 and %2 %3", { BLANK_PLAYER_CAP, BLANK_TOPS, 
        BLANK_COUNT, BLANK_TOPS}};

    templates[SENTENCE_ONLY_BELOW] =
      { "%0 %1 %2 %3", { BLANK_PLAYER_CAP, BLANK_LENGTH_VERB, 
        BLANK_BELOW, BLANK_TOPS}};

    templates[SENTENCE_LIST] =
      { "%0 has %1, %2, %3, %4", { BLANK_PLAYER_CAP, 
        BLANK_LIST_PHRASE, BLANK_LIST_PHRASE,
        BLANK_LIST_PHRASE, BLANK_LIST_PHRASE }};
  }
  else
    assert(false);

  dictionary.resize(PHRASE_SIZE);
  for (auto& dict: dictionary)
    dict.resize(BLANK_MAX_VERSIONS);

  auto& blankPlayerCap = dictionary[BLANK_PLAYER_CAP];
  blankPlayerCap[BLANK_PLAYER_CAP_WEST] = "West";
  blankPlayerCap[BLANK_PLAYER_CAP_EAST] = "East";
  blankPlayerCap[BLANK_PLAYER_CAP_EITHER] = "Either opponent";
  blankPlayerCap[BLANK_PLAYER_CAP_EACH] = "Each opponent";
  blankPlayerCap[BLANK_PLAYER_CAP_NEITHER] = "Neither opponent";
  blankPlayerCap[BLANK_PLAYER_CAP_SUIT] = "The suit";

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

  auto& blankPLV = dictionary[PHRASE_LENGTH_VERB];
  blankPLV[LENGTH_VERB_VOID] = "is void";
  blankPLV[LENGTH_VERB_XTON] = "has a %0";
  blankPLV[LENGTH_VERB_EVENLY] = "splits evenly";
  blankPLV[LENGTH_VERB_ODD_EVENLY] = "splits evenly either way";
  blankPLV[LENGTH_VERB_XTON_ATMOST] = "has at most a %0";
  blankPLV[LENGTH_VERB_CARDS] = "has %0 cards";
  blankPLV[LENGTH_VERB_CARDS_ATMOST] = "has at most %0 cards";
  blankPLV[LENGTH_VERB_RANGE] = "has %0-%1 cards";
  blankPLV[LENGTH_VERB_SPLIT] = "splits %0=%1";

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

  auto& blankLPO = dictionary[PHRASE_LENGTH_ORDINAL];
  blankLPO[LENGTH_ORDINAL_EXACT] = "%0";
  blankLPO[LENGTH_ORDINAL_ATMOST] = "at most %0";
  blankLPO[LENGTH_ORDINAL_23] = "doubleton or tripleton";

  auto& blankC = dictionary[BLANK_COUNT];
  blankC[BLANK_COUNT_EQUAL] = "%0";
  blankC[BLANK_COUNT_ATMOST] = "at most %0";
  blankC[BLANK_COUNT_RANGE_PARAMS] = "%0-%1";

  auto& blank1TP = dictionary[BLANK_TOPS];
  blank1TP[BLANK_TOPS_ONE_ATMOST] = "at most %0 of %1";
  blank1TP[BLANK_TOPS_ONE_ATLEAST] = "at least %0 of %1";
  blank1TP[BLANK_TOPS_ONE_RANGE_PARAMS] = "%0-%1 of %2";
  blank1TP[BLANK_TOPS_ACTUAL] = "%0";
  blank1TP[BLANK_TOPS_LOWER] = ", lower-ranked card%0";
  blank1TP[BLANK_TOPS_BELOW] = " card%0 below the %1";

  auto& blankBot = dictionary[BLANK_BOTTOMS];
  blankBot[BLANK_BOTTOMS_NORMAL] = "%0";

  auto& blankEx = dictionary[BLANK_EXCLUDING];
  blankEx[BLANK_EXCLUDING_NONE] = "none of";
  blankEx[BLANK_EXCLUDING_NEITHER] = "neither of";
  blankEx[BLANK_EXCLUDING_NOT] = "not";

  auto& blankBel = dictionary[BLANK_BELOW];
  blankBel[BLANK_BELOW_NORMAL] = "below the";
  blankBel[BLANK_BELOW_COMPLETELY] = "completely below the";

  auto& blankTP = dictionary[BLANK_TOPS_PHRASE];
  blankTP[BLANK_TOPS_PHRASE_HOLDING] = "%0";

  auto& blankLiP = dictionary[BLANK_LIST_PHRASE];
  blankLiP[BLANK_LIST_PHRASE_HOLDING] = "%0";

  auto& blankPL = dictionary[PHRASE_LIST];
  blankPL[LIST_HOLDING_EXACT] = "%0";
  blankPL[LIST_HOLDING_WITH_LOWS] = "%0(%1)";
}


string VerbalTemplates::get(
  const Sentence sentence,
  const RanksNames& ranksNames,
  const list<Completion>& completions,
  const vector<TemplateData>& tdata,
  const vector<Slot>& slots) const
{
  assert(sentence < templates.size());
  const VerbalTemplate& vt = templates[sentence];

  if (tdata.size() > vt.blanks.size())
  {
    cout << "sentence " << sentence << endl;
    cout << "tdata.size " << tdata.size() << endl;
    cout << "vt.blanks.size " << vt.blanks.size() << endl;

    assert(tdata.size() <= vt.blanks.size());
  }

  if (sentence != SENTENCE_LIST)
    assert(tdata.size() == vt.blanks.size());

  string s = vt.pattern;
  string fill = "";

  size_t field;
  auto ttypeIter = vt.blanks.begin();
  auto tdataIter = tdata.begin();
  auto slotIter = slots.begin();
  auto complIter = completions.begin();

  for (field = 0; field < tdata.size(); 
    field++, ttypeIter++, tdataIter++, slotIter++)
  {
    const VerbalBlank blank = * ttypeIter;
    const TemplateData& blankData = * tdataIter;
    const Slot& slot = * slotIter;
    assert(blank == blankData.blank);

    // TODO Should we have a slot.phrase() == effectively blank?

    assert(complIter != completions.end());
    fill = slot.str(dictionary, ranksNames, * complIter);
    if (slot.phrase() == PHRASE_LIST)
      complIter++;

    auto p = s.find("%" + to_string(field));
    assert(p != string::npos);
    s.replace(p, 2, fill);
  }

  if (sentence == SENTENCE_LIST)
  {
    // Eliminate the trailing % fields.
    for ( ; field < vt.blanks.size(); field++, ttypeIter++)
    {
      const VerbalBlank blank = * ttypeIter;
      assert(blank == BLANK_LIST_PHRASE);

      auto p = s.find(", %" + to_string(field));
      if (p == string::npos)
      {
cout << "string now '" << s << "'\n";
cout << "looked for ', %" << field << "'" << endl;
        assert(false);
      }

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


string VerbalTemplates::listPhrase(const TemplateData& tdata) const
{
  // Actually basically the same as topsPhrase.
  assert(tdata.numParams == 1);
  assert(tdata.instance < dictionary[BLANK_LIST_PHRASE].size());

// cout << "looking up " << BLANK_LIST_PHRASE << ", " << tdata.blank << endl;
  string s = dictionary[BLANK_LIST_PHRASE][tdata.instance];
// cout << "tops phrase is " << s << endl;
// cout << "tdata is " << tdata.str() << endl;

  for (size_t field = 0; field < tdata.numParams; field++)
  {
    auto p = s.find("%" + to_string(field));
    if (p == string::npos)
      assert(false);

    if (field == 0)
      s.replace(p, 2, tdata.text1);
    else
      assert(false);
  }

  return s;
}
