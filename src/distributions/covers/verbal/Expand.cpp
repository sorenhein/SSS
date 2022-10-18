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

  groupCheck[SENTENCE_LENGTH_ONLY] =
    { GROUP_PHRASES_PLAYER, GROUP_PHRASES_LENGTH } ;

  groupCheck[SENTENCE_ONETOP_ONLY] =
    { GROUP_PHRASES_PLAYER, GROUP_PHRASES_TOPS };

  groupCheck[SENTENCE_COUNT_ONETOP] =
    { GROUP_PHRASES_PLAYER, GROUP_PHRASES_COUNT, GROUP_PHRASES_TOPS };

  groupCheck[SENTENCE_EXACT_COUNT_ONETOP] =
    { GROUP_PHRASES_PLAYER, GROUP_PHRASES_COUNT, GROUP_PHRASES_TOPS };

  groupCheck[SENTENCE_TOPS_LENGTH] =
    { GROUP_PHRASES_PLAYER, GROUP_PHRASES_TOPS, 
      GROUP_PHRASES_ORDINAL };

  groupCheck[SENTENCE_TOPS_LENGTH_OLD] =
    { GROUP_PHRASES_PLAYER, GROUP_PHRASES_COUNT, 
      GROUP_PHRASES_TOPS, GROUP_PHRASES_ORDINAL };

  groupCheck[SENTENCE_TOPS_LENGTH_OLDER] =
    { GROUP_PHRASES_PLAYER, GROUP_PHRASES_COUNT, 
      GROUP_PHRASES_TOPS, GROUP_PHRASES_ORDINAL };

  groupCheck[SENTENCE_TOPS_BOTH_LENGTH] =
    { GROUP_PHRASES_TOPS, GROUP_PHRASES_COUNT,
      GROUP_PHRASES_TOPS, GROUP_PHRASES_COUNT,
      GROUP_PHRASES_WORD };

  groupCheck[SENTENCE_TOPS_BOTH_SYMM] = 
    { GROUP_PHRASES_TOPS, GROUP_PHRASES_TOPS };

  groupCheck[SENTENCE_TOPS_BOTH_NOT_SYMM] = 
    { GROUP_PHRASES_TOPS, GROUP_PHRASES_TOPS };

  groupCheck[SENTENCE_TOPS_AND_XES] =
    { GROUP_PHRASES_PLAYER, GROUP_PHRASES_TOPS, GROUP_PHRASES_TOPS };

  groupCheck[SENTENCE_TOPS_AND_LOWER] =
    { GROUP_PHRASES_PLAYER, GROUP_PHRASES_TOPS, GROUP_PHRASES_COUNT, 
      GROUP_PHRASES_WORD };

  groupCheck[SENTENCE_TOPS_AND_LOWER_NEW] =
    { GROUP_PHRASES_PLAYER, 
      GROUP_PHRASES_COUNT, 
      GROUP_PHRASES_TOPS, 
      GROUP_PHRASES_COUNT, 
      GROUP_PHRASES_WORD };

  groupCheck[SENTENCE_TOPS_AND_BELOW] =
    { GROUP_PHRASES_PLAYER, GROUP_PHRASES_TOPS, GROUP_PHRASES_COUNT, 
      GROUP_PHRASES_WORD, GROUP_PHRASES_TOPS };

  groupCheck[SENTENCE_TOPS_AND_BELOW_NEW] =
    { GROUP_PHRASES_PLAYER, 
      GROUP_PHRASES_COUNT, 
      GROUP_PHRASES_TOPS, 
      GROUP_PHRASES_COUNT, 
      GROUP_PHRASES_TOPS, 
      GROUP_PHRASES_TOPS };

  groupCheck[SENTENCE_ONLY_BELOW] =
    { GROUP_PHRASES_PLAYER, GROUP_PHRASES_LENGTH, 
      GROUP_PHRASES_WORD, GROUP_PHRASES_TOPS };

  groupCheck[SENTENCE_SINGULAR_EXACT] =
    { GROUP_PHRASES_PLAYER, GROUP_PHRASES_ORDINAL, 
      GROUP_PHRASES_TOPS };

  groupCheck[SENTENCE_HONORS_ORDINAL] =
    { GROUP_PHRASES_PLAYER, GROUP_PHRASES_COUNT,
      GROUP_PHRASES_WORD, GROUP_PHRASES_ORDINAL };

  groupCheck[SENTENCE_SET_UNSET] =
    { GROUP_PHRASES_PLAYER, GROUP_PHRASES_TOPS, 
      GROUP_PHRASES_TOPS };

  // Up to 4 such holdings currently foreseen.
  groupCheck[SENTENCE_LIST] =
    { GROUP_PHRASES_PLAYER, 
      GROUP_PHRASES_LIST, GROUP_PHRASES_LIST, 
      GROUP_PHRASES_LIST, GROUP_PHRASES_LIST };
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
  if (sentence != SENTENCE_LIST)
    assert(phrases.size() == vtgroups.size());

  string expansion = dictionary.coverSentences.get(sentence).text;
  string fill = "";

  size_t field;
  auto phraseIter = phrases.begin();
  auto giter = vtgroups.begin();
  auto complIter = completions.begin();

  for (field = 0; field < phrases.size(); field++, phraseIter++, giter++)
  {
    const Phrase& phrase = * phraseIter;

    const VerbalInstance& instance =
      dictionary.coverPhrases.get(phrase.getPhrase());
    assert(instance.group == * giter);

    assert(complIter != completions.end());

    fill = phrase.str(
      static_cast<PhraseExpansion>(instance.expansion),
      instance.text,
      ranksNames,
      * complIter);

    // This is the only case with multiple holdings in a list.
    if (phrase.getPhrase() == LIST_HOLDING_EXACT)
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

  if (sentence != SENTENCE_TOPS_AND_LOWER &&
      sentence != SENTENCE_TOPS_AND_LOWER_NEW)
  {
    // If there is a comma in a list, turn the last one into " or".
    // The excepted sentence has a comma on purpose.
    auto p = expansion.find_last_of(",");
    if (p != string::npos)
      expansion.replace(p, 1, 
        " " + dictionary.words.get(WORDS_CONJUNCTION).text);
  }

  return expansion;
}

