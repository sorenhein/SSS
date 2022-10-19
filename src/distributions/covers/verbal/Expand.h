/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_EXPAND_H
#define SSS_EXPAND_H

#include <vector>
#include <list>
#include <string>

using namespace std;

class RanksNames;
class Completion;
class Phrase;

enum SentencesEnum: unsigned;


class Expand
{
  private:

    void findTag(
      const unsigned group,
      const size_t field,
      const string& expansion,
      string& tag,
      size_t& pos) const;

    void recomma(string& expansion) const;


  public:

    string get(
      const SentencesEnum sentence,
      const RanksNames& ranksNames,
      const list<Completion>& completions,
      const vector<Phrase>& phrases) const;
};

#endif
