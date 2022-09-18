/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

// Despite the file name, this file implements Product methods.
// They are separate as there are so many of them.

#include <iostream>
#include <iomanip>
#include <sstream>
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

    templates[TEMPLATES_TOPS_ONLY] =
      { "%0 %1", { BLANK_PLAYER_CAP, BLANK_TOPS_PHRASE }};

    // Up to 4 such holdings currently foreseen.
    templates[TEMPLATES_LIST] =
      { "%0 has %1, %2, %3, %4", { BLANK_PLAYER_CAP, 
        BLANK_LIST_PHRASE, BLANK_LIST_PHRASE,
        BLANK_LIST_PHRASE, BLANK_LIST_PHRASE }};
  }
  else if (language == LANGUAGE_GERMAN_DE)
  {
    templates[TEMPLATES_LENGTH_ONLY] =
      { "%0 %1", { BLANK_PLAYER_CAP, BLANK_LENGTH_PHRASE }};

    templates[TEMPLATES_TOPS_ONLY] =
      { "%0 %1", { BLANK_PLAYER_CAP, BLANK_TOPS_PHRASE }};

    templates[TEMPLATES_LIST] =
      { "%0 has %1, %2, %3, %4", { BLANK_PLAYER_CAP, 
        BLANK_LIST_PHRASE, BLANK_LIST_PHRASE,
        BLANK_LIST_PHRASE, BLANK_LIST_PHRASE }};
  }
  else
    assert(false);

  dictionary.resize(BLANK_SIZE);
  for (auto& dict: dictionary)
    dict.resize(BLANK_MAX_VERSIONS);

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
  blankLP[BLANK_LENGTH_PHRASE_ODD_EVENLY] = "splits evenly either way";
  blankLP[BLANK_LENGTH_PHRASE_SINGLE_ATMOST] = "has at most a singleton";
  blankLP[BLANK_LENGTH_PHRASE_DOUBLE_ATMOST] = "has at most a doubleton";
  blankLP[BLANK_LENGTH_PHRASE_TRIPLE_ATMOST] = "has at most a tripleton";
  blankLP[BLANK_LENGTH_PHRASE_CARDS_PARAM] = "has %0 cards";
  blankLP[BLANK_LENGTH_PHRASE_CARDS_ATMOST_PARAM] = "has at most %0 cards";
  blankLP[BLANK_LENGTH_PHRASE_RANGE_PARAMS] = "has %0-%1 cards";
  blankLP[BLANK_LENGTH_PHRASE_SPLIT_PARAMS] = "splits %0=%1";

  auto& blankTP = dictionary[BLANK_TOPS_PHRASE];
  blankTP[BLANK_TOPS_PHRASE_HOLDING] = "%0";

  auto& blankLiP = dictionary[BLANK_LIST_PHRASE];
  blankLiP[BLANK_LIST_PHRASE_HOLDING] = "%0";
}


string VerbalTemplates::get(
  const TemplateSentence sentence,
  const vector<TemplateData>& tdata) const
{
  assert(sentence < templates.size());
  const VerbalTemplate& vt = templates[sentence];

  assert(tdata.size() <= vt.blanks.size());
  if (sentence != TEMPLATES_LIST)
    assert(tdata.size() == vt.blanks.size());

  string s = vt.pattern;
  string fill = "";

  size_t field;
  auto ttypeIter = vt.blanks.begin();
  auto tdataIter = tdata.begin();

/*
cout << "sentence " << sentence << endl;
for (unsigned i = 0; i < tdata.size(); i++)
{
  cout << i << ":\n";
  cout << "  " << tdata[i].str();
}
cout << "template " << vt.str() << endl;
*/

  for (field = 0; field < tdata.size(); 
    field++, ttypeIter++, tdataIter++)
  {
    const VerbalBlank blank = * ttypeIter;
    const TemplateData& blankData = * tdataIter;
    assert(blank == blankData.blank);

    if (blank == BLANK_PLAYER_CAP)
    {
      fill = VerbalTemplates::playerCap(blankData);
    }
    else if (blank == BLANK_LENGTH_PHRASE)
    {
      fill = VerbalTemplates::lengthPhrase(blankData);
    }
    else if (blank == BLANK_TOPS_PHRASE)
    {
      fill = VerbalTemplates::topsPhrase(blankData);
    }
    else if (blank == BLANK_LIST_PHRASE)
    {
      fill = VerbalTemplates::listPhrase(blankData);
    }
    else
    {
// cout << "field " << field << endl;
// cout << "blank " << blank << endl;
      assert(false);
    }

    auto p = s.find("%" + to_string(field));
    if (p == string::npos)
    {
// cout << "Looked for " << ("%" + to_string(field)) << endl;
// cout << "in " << s << endl;
      assert(false);
    }

    s.replace(p, 2, fill);
  }

  if (sentence == TEMPLATES_LIST)
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
cout << "looked for ', %'" << field << endl;
        assert(false);
      }

      s.erase(p, 4);
    }
  }

  // If there is a comma, turn the last one into " or".
  auto p = s.find_last_of(",");
  if (p != string::npos)
    s.replace(p, 1, " or");

  return s;
}


string VerbalTemplates::playerCap(const TemplateData& tdata) const
{
  assert(tdata.numParams == 0);
  assert(tdata.instance < dictionary[BLANK_PLAYER_CAP].size());

  return dictionary[BLANK_PLAYER_CAP][tdata.instance];
}


string VerbalTemplates::lengthPhrase(const TemplateData& tdata) const
{
  assert(tdata.numParams <= 2);
  assert(tdata.instance < dictionary[BLANK_LENGTH_PHRASE].size());

// cout << "looking up " << BLANK_LENGTH_PHRASE << ", " << tdata.blank << endl;
  string s = dictionary[BLANK_LENGTH_PHRASE][tdata.instance];
// cout << "length phrase is " << s << endl;
// cout << "tdata is " << tdata.str() << endl;

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


string VerbalTemplates::topsPhrase(const TemplateData& tdata) const
{
  assert(tdata.numParams == 1);
  assert(tdata.instance < dictionary[BLANK_TOPS_PHRASE].size());

// cout << "looking up " << BLANK_TOPS_PHRASE << ", " << tdata.blank << endl;
  string s = dictionary[BLANK_TOPS_PHRASE][tdata.instance];
// cout << "tops phrase is " << s << endl;
// cout << "tdata is " << tdata.str() << endl;

  for (size_t field = 0; field < tdata.numParams; field++)
  {
    auto p = s.find("%" + to_string(field));
    if (p == string::npos)
      assert(false);

    if (field == 0)
      s.replace(p, 2, tdata.text);
    else
      assert(false);
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
      s.replace(p, 2, tdata.text);
    else
      assert(false);
  }

  return s;
}
