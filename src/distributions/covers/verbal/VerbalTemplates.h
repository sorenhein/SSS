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

class RanksNames;
class Completion;
class Slot;

enum PhrasesGroup: unsigned;
enum SentencesEnum: unsigned;


class VerbalTemplates
{
  private:

    vector<list<PhrasesGroup>> templates;


  public:

    VerbalTemplates();

    string get(
      const SentencesEnum sentence,
      const RanksNames& ranksNames,
      const list<Completion>& completions,
      const vector<Slot>& slots) const;
};

#endif
