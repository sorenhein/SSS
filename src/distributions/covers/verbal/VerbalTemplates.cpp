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
#include "VerbalMap.h"

#include "../../../languages/Dictionary.h"

extern Dictionary dictionary;


VerbalTemplates::VerbalTemplates()
{
  VerbalTemplates::reset();
}


void VerbalTemplates::reset()
{
  templates.clear();
}


void VerbalTemplates::setMaps()
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
}


void VerbalTemplates::set(const Language languageIn)
{
  templates.resize(SENTENCE_SIZE);
  language = languageIn;
  VerbalTemplates::setMaps();

  if (language == LANGUAGE_ENGLISH_US)
  {
    templates[SENTENCE_LENGTH_ONLY] =
      { "%0 %1", { GROUP_PLAYER, GROUP_LENGTH_VERB} };

    templates[SENTENCE_ONETOP_ONLY] =
      { "%0 has %1", { GROUP_PLAYER, GROUP_TOPS }};

    templates[SENTENCE_TOPS_LENGTH] =
      { "%0 has %1 %2",
        { GROUP_PLAYER, GROUP_TOPS, GROUP_LENGTH_ORDINAL }};

    templates[SENTENCE_TOPS_LENGTH_WITHOUT] =
      { "%0 has %1 %2 without %3",
        { GROUP_PLAYER, GROUP_TOPS, GROUP_LENGTH_ORDINAL, GROUP_TOPS }};

    templates[SENTENCE_TOPS_EXCLUDING] =
      { "%0 has %1 and %2 %3",
        { GROUP_PLAYER, GROUP_TOPS, GROUP_EXCLUDING, GROUP_TOPS }};

    templates[SENTENCE_TOPS_AND_XES] =
      { "%0 has %1%2",
        { GROUP_PLAYER, GROUP_TOPS, GROUP_BOTTOMS }};

    templates[SENTENCE_TOPS_AND_LOWER] =
      { "%0 has %1 and %2%3",
        { GROUP_PLAYER, GROUP_TOPS, GROUP_COUNT, GROUP_TOPS }};

    templates[SENTENCE_ONLY_BELOW] =
      { "%0 %1 %2 %3",
        { GROUP_PLAYER, GROUP_LENGTH_VERB, GROUP_BELOW, GROUP_TOPS }};

    // Up to 4 such holdings currently foreseen.
    templates[SENTENCE_LIST] =
      { "%0 has %1, %2, %3, %4", { GROUP_PLAYER, 
        GROUP_LIST, GROUP_LIST, GROUP_LIST, GROUP_LIST }};
  }
  else if (language == LANGUAGE_GERMAN_DE)
  {
    assert(false);
  }
  else
    assert(false);

}


string VerbalTemplates::get(
  const Sentence sentence,
  const RanksNames& ranksNames,
  const list<Completion>& completions,
  const vector<Slot>& slots) const
{
  assert(sentence < templates.size());
  const VerbalTemplate& vt = templates[sentence];

  // A list has room for up to 4 entries, but they need not be present.
  if (sentence != SENTENCE_LIST)
    assert(slots.size() == vt.groups.size());

  string expansion = vt.pattern;
  string fill = "";

  size_t field;
  auto slotIter = slots.begin();
  auto complIter = completions.begin();
  auto giter = vt.groups.begin();

  for (field = 0; field < slots.size(); field++, slotIter++, giter++)
  {
    const Slot& slot = * slotIter;

    assert(slot.getPhrase() < instanceToGroup.size());
    assert(instanceToGroup[slot.getPhrase()] == * giter);

    assert(complIter != completions.end());

    fill = slot.str(
      instanceToExpansion, 
      instanceToText,
      ranksNames,
      * complIter);

    if (slot.getPhrase() == LIST_HOLDING_EXACT)
      complIter++;

    // Fill in the placeholder.
    auto p = expansion.find("%" + to_string(field));
    assert(p != string::npos);
    expansion.replace(p, 2, fill);
  }

  if (sentence == SENTENCE_LIST)
  {
    // Eliminate the trailing placeholders in a list.
    for ( ; field < vt.groups.size(); field++)
    {
      auto p = expansion.find(", %" + to_string(field));
      assert(p != string::npos);
      expansion.erase(p, 4);
    }
  }

  if (sentence != SENTENCE_TOPS_AND_LOWER)
  {
    // If there is a comma in a list, turn the last one into " or".
    // The excepted sentence has a comma on purpose.
    auto p = expansion.find_last_of(",");
    if (p != string::npos)
      expansion.replace(p, 1, " or");
  }

  return expansion;
}

