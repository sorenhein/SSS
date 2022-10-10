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

using namespace std;

class Completion;
class Slot;
class RanksNames;

enum SentencesEnum: unsigned;
enum VerbalGroup: unsigned;
enum SlotExpansion: unsigned;


class VerbalTemplates
{
  private:

    vector<list<VerbalGroup>> templates;

    vector<VerbalGroup> instanceToGroup;
    vector<SlotExpansion> instanceToExpansion;
    vector<string> instanceToText;


    void setMaps();


  public:

    VerbalTemplates();

    void reset();

    void set();

    string get(
      const SentencesEnum sentence,
      const RanksNames& ranksNames,
      const list<Completion>& completions,
      const vector<Slot>& slots) const;
};

#endif
