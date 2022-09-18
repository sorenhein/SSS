/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

// Despite the file name, this file implements Product methods.
// They are separate as there are so many of them.

#include <cassert>

#include "VerbalTemplates.h"
#include "VerbalBlank.h"


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
  templates.resize(TEMPLATES_SIZE);
  language = languageIn;

  if (language == LANGUAGE_ENGLISH_US)
  {
    templates[TEMPLATES_LENGTH_ONLY] =
      { "%0 %1", { BLANK_PLAYER_CAP, BLANK_LENGTH_PHRASE }};
  }
  else if (language == LANGUAGE_GERMAN_DE)
  {
    templates[TEMPLATES_LENGTH_ONLY] =
      { "%0 %1", { BLANK_PLAYER_CAP, BLANK_LENGTH_PHRASE }};
  }
  else
    assert(false);

  dictionary.resize(BLANK_SIZE);
  for (auto i = 0; i < BLANK_SIZE; i++)
    dictionary.resize(BLANK_MAX_VERSIONS);

  auto& blankPlayerCap = dictionary[BLANK_PLAYER_CAP];
  blankPlayerCap[BLANK_PLAYER_CAP_WEST] = "West";
  blankPlayerCap[BLANK_PLAYER_CAP_EAST] = "East";
  blankPlayerCap[BLANK_PLAYER_CAP_EITHER] = "Either opponent";
  blankPlayerCap[BLANK_PLAYER_CAP_EACH] = "Each opponent";
  blankPlayerCap[BLANK_PLAYER_CAP_NEITHER] = "Neither opponent";
  blankPlayerCap[BLANK_PLAYER_CAP_SUIT] = "The suit";

  auto& blankLP = dictionary[BLANK_LENGTH_PHRASE];
  blankLP[BLANK_LENGTH_PHRASE_VOID] = "is void";
  blankLP[BLANK_LENGTH_PHRASE_SINGLE] = "has a singleton";
  blankLP[BLANK_LENGTH_PHRASE_DOUBLE] = "has a doubleton";
  blankLP[BLANK_LENGTH_PHRASE_TRIPLE] = "has a tripleton";
  blankLP[BLANK_LENGTH_PHRASE_EVENLY] = "splits evenly";
  blankLP[BLANK_LENGTH_PHRASE_SINGLE_ATMOST] = "has at most a singleton";
  blankLP[BLANK_LENGTH_PHRASE_DOUBLE_ATMOST] = "has at most a doubleton";
  blankLP[BLANK_LENGTH_PHRASE_TRIPLE_ATMOST] = "has at most a tripleton";
  blankLP[BLANK_LENGTH_PHRASE_CARDS_PARAM] = "has %0 cards";
  blankLP[BLANK_LENGTH_PHRASE_RANGE_PARAMS] = "has %0-%1 cards";
  blankLP[BLANK_LENGTH_PHRASE_SPLIT_PARAMS] = "splits %0=%1";
}


string VerbalTemplates::get(const TemplateData& tdata) const
{
  assert(tdata.blank < templates.size());
  const VerbalTemplate& vt = templates[tdata.blank];

  string s = vt.pattern;
  string fill = "";

  size_t field = 0;
  for (auto blank: vt.blanks)
  {
    if (blank == BLANK_PLAYER_CAP)
    {
      fill = VerbalTemplates::playerCap(tdata);
    }
    else if (blank == BLANK_LENGTH_PHRASE)
    {
      fill = VerbalTemplates::lengthPhrase(tdata);
    }
    else
      assert(false);

    auto p = s.find("%" + to_string(field));
    if (p == string::npos)
      assert(false);

    s.replace(p, 2, fill);
    field++;
  }

  return s;
}


string VerbalTemplates::playerCap(const TemplateData& tdata) const
{
  assert(tdata.numParams == 0);
  assert(tdata.blank < dictionary[BLANK_PLAYER_CAP].size());

  return dictionary[BLANK_PLAYER_CAP][tdata.blank];
}


string VerbalTemplates::lengthPhrase(const TemplateData& tdata) const
{
  assert(tdata.numParams < 2);
  assert(tdata.blank < dictionary[BLANK_LENGTH_PHRASE].size());

  string s = dictionary[BLANK_LENGTH_PHRASE][tdata.blank];

  for (size_t field = 0; field < tdata.numParams; field++)
  {
    auto p = s.find("%" + to_string(field));
    if (p == string::npos)
      assert(false);

    if (field == 0)
      s.replace(p, 2, to_string(tdata.param1));
    else if (field == 1)
      s.replace(p, 2, to_string(tdata.param2));
    else
      assert(false);
  }

  return s;
}

