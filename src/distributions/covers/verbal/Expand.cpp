/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Expand.h"

#include "Completion.h"
#include "Phrase.h"

#include "../../../languages/Dictionary.h"
#include "../../../languages/connections/cover/sentences.h"
#include "../../../languages/connections/cover/phrases.h"
#include "../../../languages/connections/words.h"

extern Dictionary dictionary;


Expand::Expand()
{
  groupCheck.resize(SENTENCE_SIZE);

  groupCheck[SENTENCE_LENGTH] =
    { GROUP_PHRASES_PLAYER, GROUP_PHRASES_LENGTH } ;

  groupCheck[SENTENCE_TOPS] =
    { GROUP_PHRASES_PLAYER, GROUP_PHRASES_TOPS };

  groupCheck[SENTENCE_LENGTH_BELOW_TOPS] =
    { GROUP_PHRASES_PLAYER, GROUP_PHRASES_LENGTH, 
      GROUP_PHRASES_DICT, GROUP_PHRASES_TOPS };

  groupCheck[SENTENCE_COUNT_OF_TOPS_ORDINAL] =
    { GROUP_PHRASES_PLAYER, GROUP_PHRASES_COUNT, 
      GROUP_PHRASES_TOPS, GROUP_PHRASES_ORDINAL };

  groupCheck[SENTENCE_COUNT_TOPS] =
    { GROUP_PHRASES_PLAYER, GROUP_PHRASES_COUNT, GROUP_PHRASES_TOPS };

  groupCheck[SENTENCE_EXACTLY_COUNT_TOPS] =
    { GROUP_PHRASES_PLAYER, GROUP_PHRASES_COUNT, GROUP_PHRASES_TOPS };

  groupCheck[SENTENCE_TOPS_ORDINAL] =
    { GROUP_PHRASES_PLAYER, GROUP_PHRASES_TOPS, 
      GROUP_PHRASES_ORDINAL };

  groupCheck[SENTENCE_COUNT_TOPS_ORDINAL] =
    { GROUP_PHRASES_PLAYER, GROUP_PHRASES_COUNT, 
      GROUP_PHRASES_TOPS, GROUP_PHRASES_ORDINAL };

  groupCheck[SENTENCE_COUNT_HONORS_ORDINAL] =
    { GROUP_PHRASES_PLAYER, GROUP_PHRASES_COUNT,
      GROUP_PHRASES_DICT, GROUP_PHRASES_ORDINAL };

  groupCheck[SENTENCE_EXACTLY_COUNT_TOPS_ORDINAL] =
    { GROUP_PHRASES_PLAYER, GROUP_PHRASES_COUNT, 
      GROUP_PHRASES_TOPS, GROUP_PHRASES_ORDINAL };

  groupCheck[SENTENCE_TOPS_AND_LOWER] =
    { GROUP_PHRASES_PLAYER, GROUP_PHRASES_TOPS, GROUP_PHRASES_COUNT, 
      GROUP_PHRASES_DICT };

  groupCheck[SENTENCE_EXACTLY_TOPS_AND_LOWER] =
    { GROUP_PHRASES_PLAYER, 
      GROUP_PHRASES_COUNT, 
      GROUP_PHRASES_TOPS, 
      GROUP_PHRASES_COUNT, 
      GROUP_PHRASES_DICT };

  groupCheck[SENTENCE_TOPS_AND_COUNT_BELOW_CARD] =
    { GROUP_PHRASES_PLAYER, GROUP_PHRASES_TOPS, GROUP_PHRASES_COUNT, 
      GROUP_PHRASES_DICT, GROUP_PHRASES_TOPS };

  groupCheck[SENTENCE_EXACTLY_TOPS_MAYBE_UNSET] =
    { GROUP_PHRASES_PLAYER, GROUP_PHRASES_TOPS, 
      GROUP_PHRASES_TOPS };

  groupCheck[SENTENCE_TOPS_MAYBE_OTHERS] =
    { GROUP_PHRASES_PLAYER, GROUP_PHRASES_TOPS };

  groupCheck[SENTENCE_TOPS_AND_XES] =
    { GROUP_PHRASES_PLAYER, GROUP_PHRASES_TOPS, GROUP_PHRASES_TOPS };

  // Up to 4 such holdings currently foreseen.
  groupCheck[SENTENCE_EXACTLY_LIST] =
    { GROUP_PHRASES_PLAYER, 
      GROUP_PHRASES_LIST, GROUP_PHRASES_LIST, 
      GROUP_PHRASES_LIST, GROUP_PHRASES_LIST };

  groupCheck[SENTENCE_2SPLIT_TOPS_SYMM] = 
    { GROUP_PHRASES_TOPS, GROUP_PHRASES_TOPS };

  groupCheck[SENTENCE_2SPLIT_TOPS_ASYMM] = 
    { GROUP_PHRASES_TOPS, GROUP_PHRASES_TOPS };

  groupCheck[SENTENCE_2SPLIT_TOPS_DIGITS_SYMM] = 
    { GROUP_PHRASES_TOPS, GROUP_PHRASES_DIGITS,
      GROUP_PHRASES_TOPS, GROUP_PHRASES_DIGITS };

  groupCheck[SENTENCE_2SPLIT_TOPS_DIGITS_ASYMM] = 
    { GROUP_PHRASES_TOPS, GROUP_PHRASES_DIGITS,
      GROUP_PHRASES_TOPS, GROUP_PHRASES_DIGITS };
}


string Expand::get(
  const SentencesEnum sentence,
  const RanksNames& ranksNames,
  const list<Completion>& completions,
  const vector<Phrase>& phrases) const
{
  assert(sentence < groupCheck.size());
  const auto& vtgroups = groupCheck[sentence];

  // A list has room for up to 4 entries, but they need not be present.
  if (sentence != SENTENCE_EXACTLY_LIST)
    assert(phrases.size() == vtgroups.size());

  const auto& groupList = 
    dictionary.coverSentences.get(sentence).expansions;
  assert(groupList.size() == vtgroups.size());

  string expansion = dictionary.coverSentences.get(sentence).text;
  string fill = "";
  string tag;

  size_t field;
  auto phraseIter = phrases.begin();
  auto giter = vtgroups.begin();
  auto gliter = groupList.begin();
  auto complIter = completions.begin();

  for (field = 0; field < phrases.size(); 
      field++, phraseIter++, giter++, gliter++)
  {
    const Phrase& phrase = * phraseIter;

    assert(phrase.getGroup() == * giter);
    assert(phrase.getGroup() == * gliter);
    assert(complIter != completions.end());

    fill = phrase.str(ranksNames, * complIter);

    // This is the only case with multiple holdings in a list.
    if (phrase.getPhrase() == LIST_HOLDING)
      complIter++;

    // Fill in the placeholder.
    tag = dictionary.groupTag(* gliter);
    auto p = tag.find("%");
    assert(p != string::npos);
    tag.replace(p, 1, to_string(field));

    auto q = expansion.find(tag);
    assert(q != string::npos);
    expansion.replace(q, tag.size(), fill);

    // auto p = expansion.find("%" + to_string(field));
    // assert(p != string::npos);
    // expansion.replace(p, 2, fill);
  }

  if (sentence == SENTENCE_EXACTLY_LIST)
  {
// cout << "Start with: " << expansion << endl;
    // Eliminate the trailing placeholders in a list.
    for ( ; field < vtgroups.size(); field++, gliter++)
    {
      // Fill in the placeholder.
      // TODO Expand::findTag(* gliter, tag, pos);
      tag = ", " + dictionary.groupTag(* gliter);
      auto p = tag.find("%");
      assert(p != string::npos);
      tag.replace(p, 1, to_string(field));

      auto q = expansion.find(tag);
      assert(q != string::npos);
      expansion.erase(q, tag.size());

// cout << "Now: " << expansion << endl;
      // auto p = expansion.find(", %" + to_string(field));
      // assert(p != string::npos);
      // expansion.erase(p, 4);
    }
  }

  if (sentence != SENTENCE_TOPS_AND_LOWER &&
      sentence != SENTENCE_EXACTLY_TOPS_AND_LOWER)
  {
    // If there is a comma in a list, turn the last one into " or".
    // The excepted sentence has a comma on purpose.
    // TODO Expand::recomma(expansion);
    auto p = expansion.find_last_of(",");
    if (p != string::npos)
      expansion.replace(p, 1, 
        " " + dictionary.words.get(WORDS_CONJUNCTION).text);
  }

  return expansion;
}

