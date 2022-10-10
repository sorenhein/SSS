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
#include "Completion.h"
#include "VerbalMap.h"

#include "../../../languages/Dictionary.h"
#include "../../../languages/connections/cover/sentences.h"

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


void VerbalTemplates::set()
{
  templates.resize(SENTENCE_SIZE);
  VerbalTemplates::setMaps();

    templates[SENTENCE_LENGTH_ONLY] =
      { GROUP_PLAYER, GROUP_LENGTH_VERB } ;

    templates[SENTENCE_ONETOP_ONLY] =
      { GROUP_PLAYER, GROUP_TOPS };

    templates[SENTENCE_TOPS_LENGTH] =
        { GROUP_PLAYER, GROUP_TOPS, GROUP_LENGTH_ORDINAL };

    templates[SENTENCE_TOPS_LENGTH_WITHOUT] =
        { GROUP_PLAYER, GROUP_TOPS, GROUP_LENGTH_ORDINAL, GROUP_TOPS };

    templates[SENTENCE_TOPS_EXCLUDING] =
        { GROUP_PLAYER, GROUP_TOPS, GROUP_EXCLUDING, GROUP_TOPS };

    templates[SENTENCE_TOPS_AND_XES] =
        { GROUP_PLAYER, GROUP_TOPS, GROUP_BOTTOMS };

    templates[SENTENCE_TOPS_AND_LOWER] =
        { GROUP_PLAYER, GROUP_TOPS, GROUP_COUNT, GROUP_TOPS };

    templates[SENTENCE_ONLY_BELOW] =
        { GROUP_PLAYER, GROUP_LENGTH_VERB, GROUP_BELOW, GROUP_TOPS };

    // Up to 4 such holdings currently foreseen.
    templates[SENTENCE_LIST] =
        { GROUP_PLAYER, GROUP_LIST, GROUP_LIST, GROUP_LIST, GROUP_LIST };
}


string VerbalTemplates::get(
  const SentencesEnum sentence,
  const RanksNames& ranksNames,
  const list<Completion>& completions,
  const vector<Slot>& slots) const
{
  assert(sentence < templates.size());
  const auto& vtgroups = templates[sentence];

  // A list has room for up to 4 entries, but they need not be present.
  if (sentence != SENTENCE_LIST)
    assert(slots.size() == vtgroups.size());

  // string expansion = vt.pattern;
  string expansion = dictionary.coverSentences.get(sentence).text;
  string fill = "";

  size_t field;
  auto slotIter = slots.begin();
  auto complIter = completions.begin();
  auto giter = vtgroups.begin();

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
    for ( ; field < vtgroups.size(); field++)
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

