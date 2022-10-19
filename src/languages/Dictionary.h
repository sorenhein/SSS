/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_DICTIONARY_H
#define SSS_DICTIONARY_H

#include <map>

#include "Component.h"
#include "VerbalConnection.h"

using namespace std;


struct Dictionary
{
  map<string, unsigned> sentenceGroupMap;
  map<string, unsigned> phraseGroupMap;

  map<string, unsigned> definiteGroupMap;
  map<string, unsigned> indefiniteGroupMap;
  map<string, unsigned> prepositionGroupMap;
  map<string, unsigned> cardsShortGroupMap;
  map<string, unsigned> honorsShortGroupMap;
  map<string, unsigned> numeralsGroupMap;
  map<string, unsigned> ordinalsGroupMap;
  map<string, unsigned> wordsGroupMap;

  Component coverSentences;
  Component coverPhrases;

  Component cardsDefinite;
  Component cardsIndefinite;
  Component cardsPrepositionOf;
  Component cardsShort;

  Component honorsShort;

  Component numerals;
  Component ordinals;

  Component words;

  vector<string> phraseTags;


  void setMap(
    const list<TagConnection>& groupConnection,
    map<string, unsigned>& groupMap) const;

  void setPhraseTags();

  void read(const string& language);

  const string& phraseTag(const unsigned index) const;
};

#endif
