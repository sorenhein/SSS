/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_SLOT_H
#define SSS_SLOT_H

#include <vector>
#include <string>

class RanksNames;
class Completion;

enum PhraseCategory: unsigned;
enum SlotExpansion: unsigned;

using namespace std;


class Slot
{
  private:

    PhraseCategory phraseCategory;

    // Each PhraseCategory has a different enum for its instances,
    // so we just use an unsigned here.
    unsigned phraseInstance;

    SlotExpansion expansion;

    unsigned char numUchars;
    unsigned char numBools;

    vector<unsigned char> uchars;
    vector<bool> bools;

  public:

    Slot();

    void setSemantics(
      const PhraseCategory phraseCategoryIn,
      const unsigned phraseInstanceIn,
      const SlotExpansion expansionIn);

    void setValues(const unsigned char value1);

    void setValues(
      const unsigned char value1,
      const unsigned char value2);

    void setValues(
      const unsigned char value1,
      const unsigned char value2,
      const unsigned char value3);

    void setBools(const bool bool1);

    void setBools(
      const bool bool1,
      const bool bool2);

    void setBools(
      const bool bool1,
      const bool bool2,
      const bool bool3);

    string str(
      const vector<vector<string>>& dictionary,
      const RanksNames& ranksNames,
      const Completion& completion) const;
};


#endif
