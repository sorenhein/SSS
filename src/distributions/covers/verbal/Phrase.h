/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_PHRASE_H
#define SSS_PHRASE_H

#include <vector>
#include <string>

class RanksNames;
class Completion;

enum VerbalPhrase: unsigned;
enum Opponent: unsigned;
enum PhrasesEnum: unsigned;
enum PhraseExpansion: unsigned;

using namespace std;


class Phrase
{
  private:

    PhrasesEnum phrase;

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

    string strNone(
      const string& text,
      const RanksNames& ranksNames,
      const Completion& completion) const;

    string strDigits(
      const string& text,
      const RanksNames& ranksNames,
      const Completion& completion) const;

    string strNumerical(
      const string& text,
      const RanksNames& ranksNames,
      const Completion& completion) const;

    string strOrdinal(
      const string& text,
      const RanksNames& ranksNames,
      const Completion& completion) const;

    string strCardsWord(
      const string& text,
      const RanksNames& ranksNames,
      const Completion& completion) const;

    string strHonorsWord(
      const string& text,
      const RanksNames& ranksNames,
      const Completion& completion) const;

    string strMidHonorsWord(
      const string& text,
      const RanksNames& ranksNames,
      const Completion& completion) const;

    string strLowestCard(
      const string& text,
      const RanksNames& ranksNames,
      const Completion& completion) const;

    string strIndefiniteRank(
      const string& text,
      const RanksNames& ranksNames,
      const Completion& completion) const;

    string strDefiniteRank(
      const string& text,
      const RanksNames& ranksNames,
      const Completion& completion) const;

    string strOfDefiniteRank(
      const string& text,
      const RanksNames& ranksNames,
      const Completion& completion) const;

    string strCompletionSet(
      const string& text,
      const RanksNames& ranksNames,
      const Completion& completion) const;

    string strCompletionUnset(
      const string& text,
      const RanksNames& ranksNames,
      const Completion& completion) const;

    string strXes(
      const string& text,
      const RanksNames& ranksNames,
      const Completion& completion) const;




  public:

    Phrase();

    void setPhrase(const PhrasesEnum phraseIn);

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

    PhrasesEnum getPhrase() const;

    string str(
      const PhraseExpansion expansion,
      const string& text,
      const RanksNames& ranksNames,
      const Completion& completion) const;
};


#endif
