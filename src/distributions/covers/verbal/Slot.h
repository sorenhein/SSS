/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_SLOT_H
#define SSS_SLOT_H

#include <vector>
#include <string>

#include "../../../utils/table.h"

class RanksNames;
class Completion;

enum PhraseCategory: unsigned;
enum SlotExpansion: unsigned;

using namespace std;


enum SlotExpansion: unsigned
{
  SLOT_NONE = 0,
  SLOT_NUMERICAL = 1,
  SLOT_RANKS = 2,
  SLOT_COMPLETION_SET = 3,
  SLOT_COMPLETION_UNSET = 4,
  SLOT_COMPLETION_BOTH = 5,
  SLOT_COMPLETION_XES = 6,
  SLOT_COMPLETION_UNCLEAR = 7,
  SLOT_TEXT_LOWER = 8,
  SLOT_TEXT_BELOW = 9,
  SLOT_ORDINAL = 10, // Re-sort after NUMERICAL
  SLOT_RANGE_OF = 11,
  SLOT_SOME_OF = 12,
  SLOT_LENGTH = 13
};


class Slot
{
  private:

    PhraseCategory phraseCategory;

    // Each PhraseCategory has a different enum for its instances,
    // so we just use an unsigned here.
    unsigned phraseInstance;

    SlotExpansion expansion;

    unsigned char numOpp;
    unsigned char numUchars;
    unsigned char numBools;

    Opponent side;
    vector<unsigned char> uchars;
    vector<bool> bools;

  public:

    Slot();

    void setSemantics(
      const PhraseCategory phraseCategoryIn,
      const unsigned phraseInstanceIn,
      const SlotExpansion expansionIn);

    void setSide(const Opponent sideIn);

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

    PhraseCategory phrase() const;

    string str(
      const vector<vector<string>>& dictionary,
      const RanksNames& ranksNames,
      const Completion& completion) const;
};


#endif
