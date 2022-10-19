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


void Expand::findTag(
  const unsigned group,
  const size_t field,
  const string& expansion,
  string& tag,
  size_t& pos) const
{
  tag = dictionary.groupTag(group);
  auto p = tag.find("%");
  assert(p != string::npos);
  tag.replace(p, 1, to_string(field));

  pos = expansion.find(tag);
  assert(pos != string::npos);
}


void Expand::recomma(string& expansion) const
{
  size_t pos = expansion.find_last_of(",");
  if (pos != string::npos)
    expansion.replace(pos, 1, 
      " " + dictionary.words.get(WORDS_CONJUNCTION).text);
}


string Expand::get(
  const SentencesEnum sentence,
  const RanksNames& ranksNames,
  const list<Completion>& completions,
  const vector<Phrase>& phrases) const
{
  const auto& groupList = 
    dictionary.coverSentences.get(sentence).expansions;

  // A list has room for up to 4 entries, but they need not be present.
  if (sentence != SENTENCE_EXACTLY_LIST)
    assert(phrases.size() == groupList.size());

  string expansion = dictionary.coverSentences.get(sentence).text;
  string fill = "", tag;

  size_t field, pos;
  auto phraseIter = phrases.begin();
  auto gliter = groupList.begin();
  auto complIter = completions.begin();

  for (field = 0; field < phrases.size(); 
      field++, phraseIter++, gliter++)
  {
    const Phrase& phrase = * phraseIter;

    assert(phrase.getGroup() == * gliter);
    assert(complIter != completions.end());

    fill = phrase.str(ranksNames, * complIter);

    // This is the only case with multiple holdings in a list.
    if (phrase.getPhrase() == LIST_HOLDING)
      complIter++;

    // Fill in the placeholder.
    Expand::findTag(* gliter, field, expansion, tag, pos);
    expansion.replace(pos, tag.size(), fill);
  }

  if (sentence == SENTENCE_EXACTLY_LIST)
  {
    // Eliminate the trailing placeholders in a list.
    for ( ; field < groupList.size(); field++, gliter++)
    {
      // Fill in the placeholder.
      Expand::findTag(* gliter, field, expansion, tag, pos);

      // Also delete the leading ", "
      expansion.erase(pos-2, tag.size()+2);
    }
  }

  if (sentence != SENTENCE_TOPS_AND_LOWER &&
      sentence != SENTENCE_EXACTLY_TOPS_AND_LOWER)
  {
    // If there is a comma in a list, turn the last one into " or".
    // The excepted sentence has a comma on purpose.
    Expand::recomma(expansion);
  }

  return expansion;
}

