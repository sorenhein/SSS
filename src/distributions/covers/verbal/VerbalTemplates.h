/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_VERBALTEMPLATES_H
#define SSS_VERBALTEMPLATES_H

#include <vector>
#include <list>
#include <string>

#include "Language.h"
#include "Completion.h"

using namespace std;

class RanksNames;
class Slot;

enum VerbalGroup: unsigned;
enum SlotExpansion: unsigned;


enum Sentence: unsigned
{
  SENTENCE_LENGTH_ONLY = 0,
  SENTENCE_ONETOP_ONLY = 1,
  SENTENCE_TOPS_LENGTH = 2,
  SENTENCE_TOPS_LENGTH_WITHOUT = 3,
  SENTENCE_TOPS_EXCLUDING = 4,
  SENTENCE_TOPS_AND_XES = 5,
  SENTENCE_TOPS_AND_LOWER = 6,
  SENTENCE_ONLY_BELOW = 7,
  SENTENCE_LIST = 8,
  SENTENCE_SIZE = 9
};


struct VerbalTemplate
{
  string pattern;
  list<VerbalGroup> groups;
};



class VerbalTemplates
{
  private:

    vector<VerbalTemplate> templates;

    vector<vector<string>> dictionary;

    Language language;

    vector<VerbalGroup> instanceToGroup;
    vector<SlotExpansion> instanceToExpansion;
    vector<string> instanceToText;


    void setMaps();


  public:

    VerbalTemplates();

    void reset();

    void set(const Language languageIn);

    string get(
      const Sentence sentence,
      const RanksNames& ranksNames,
      const list<Completion>& completions,
      const vector<Slot>& slots) const;
};

#endif
