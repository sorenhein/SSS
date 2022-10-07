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
#include "VerbalBlank.h"

using namespace std;

class RanksNames;
class Slot;

enum VerbalBlank: unsigned;
enum VerbalGroup: unsigned;
enum SlotExpansion: unsigned;


struct VerbalTemplate
{
  string pattern;

  list<VerbalBlank> blanks;

  list<VerbalGroup> groups;

  string str() const
  {
    stringstream ss;
    ss << "pattern '" << pattern << "'\n";
    for (auto i: blanks)
    ss << i << " ";
    ss << "\n";
    return ss.str();
  };
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
