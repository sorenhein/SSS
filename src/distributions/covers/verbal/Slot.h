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
enum VerbalPhrase: unsigned;
enum Opponent: unsigned;
enum PhraseExpansion: unsigned;

using namespace std;


/*
enum SlotExpansion: unsigned
{
  SLOT_NONE = 0,
  SLOT_NUMERICAL = 1,
  SLOT_ORDINAL = 2,
  SLOT_RANKS = 3,
  SLOT_TEXT_LOWER = 4,
  SLOT_TEXT_BELOW = 5,
  SLOT_RANGE_OF = 6,
  SLOT_SOME_OF = 7,
  SLOT_SOME_RANK_SET = 8,
  SLOT_FULL_RANK_SET = 9,
  SLOT_COMPLETION_SET = 10,
  SLOT_COMPLETION_BOTH = 11,
  SLOT_COMPLETION_XES = 12,
  SLOT_SIZE = 13
};
*/


class Slot
{
  private:

    VerbalPhrase phrase;

    unsigned char numOpp;
    unsigned char numUchars;
    unsigned char numBools;

    Opponent side;
    vector<unsigned char> uchars;
    vector<bool> bools;



    bool has(
      const unsigned char actOpp,
      const unsigned char actUchars,
      const unsigned char actBools) const;

    void replace(
      string& s,
      const string& percent,
      const string& repl) const;

    void replace(
      string& s,
      const string& percent,
      unsigned char uchar) const;

    void replace(
      string& s,
      const unsigned char field,
      const string& repl) const;

    void replace(
      string& s,
      const unsigned char field,
      unsigned char uchar) const;


  public:

    Slot();

    void setPhrase(const VerbalPhrase phraseIn);

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

    /*
    void setBools(
      const bool bool1,
      const bool bool2);

    void setBools(
      const bool bool1,
      const bool bool2,
      const bool bool3);
      */

    VerbalPhrase getPhrase() const;

    string str(
      const PhraseExpansion expansion,
      const string& text,
      const RanksNames& ranksNames,
      const Completion& completion) const;
};


#endif
