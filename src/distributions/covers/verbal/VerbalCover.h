/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_VERBALCOVER_H
#define SSS_VERBALCOVER_H

#include <vector>
#include <list>
#include <string>

#include "./Completion.h"
#include "Phrase.h"

#include "../term/Term.h"

using namespace std;

class Profile;
class RanksNames;
struct VerbalSide;
enum SentencesEnum: unsigned;
enum Opponent: unsigned;


class VerbalCover
{
  private:
    
    SentencesEnum sentence;

    list<Completion> completions;

    vector<Phrase> phrases;

    Term length;


    // Used to make a synthetic length of small cards.

    void setLength(
      const unsigned char lower,
      const unsigned char upper,
      const unsigned char maximum);

    void getLengthEqualData(
      const unsigned char oppsLength,
      const VerbalSide& vside,
      const bool abstractableFlag);

    void getLengthInsideData(
      const unsigned char oppsLength,
      const VerbalSide& vside,
      const bool abstractableFlag);

    void getLengthData(
      const unsigned char oppsLength,
      const VerbalSide& vside,
      const bool abstractableFlag);

    void fillLengthOrdinal(
      const unsigned char oppsLength,
      const Opponent simplestOpponent,
      Phrase& phrase);

    void fillTopsActual(
      const Opponent side,
      Phrase& phrase);


  public:

    VerbalCover();


    // Length only

    void setLength(const Term& length);

    // Fills.

    // SENTENCE_LENGTH_ONLY
    void fillLengthOnly(
      const Term& lengthIn,
      const unsigned char oppsLength,
      const bool symmFlag);

    // SENTENCE_ONETOP
    void fillOnetopOnly(
      const Term& top,
      const unsigned char oppsSize,
      const unsigned char onetopIndex,
      const VerbalSide& vside);

    // SENTENCE_TOPS_LENGTH
    void fillOnetopLength(
      const Term& lengthIn,
      const Term& top,
      const Profile& sumProfile,
      const unsigned char onetopIndex,
      const VerbalSide& vside);

    // SENTENCE_TOPS_LENGTH
    void fillOnesided(
      const Profile& sumProfile,
      const VerbalSide& vside);

    // SENTENCE_TOPS_BOTH_LENGTH
    void fillTopsBothLength(
      const Profile& sumProfile,
      const VerbalSide& vside);

    // SENTENCE_TOPS_BOTH
    void fillTopsBoth(const VerbalSide& vside);

    // SENTENCE_TOPS_AND_XES
    void fillTopsAndXes(const VerbalSide& vside);

    // SENTENCE_TOPS_AND_LOWER
    void fillTopsAndLower(
      const VerbalSide& vside,
      const unsigned char numOptions);

    // SENTENCE_ONLY_BELOW
    void fillBelow(
      const unsigned char numBottoms,
      const unsigned char rankNo,
      const VerbalSide& vside);

    // SENTENCE_TOPS_LENGTH
    void fillSingular(
      const unsigned char lenCompletion,
      const VerbalSide& vside);

    // SENTENCE_LIST
    void fillCompletion(const VerbalSide& vside);

    // SENTENCE_LIST
    void fillCompletionWithLows(const VerbalSide& vside);

    // SENTENCE_LIST
    // SENTENCE_ONETOP_ONLY
    void fillList(const VerbalSide& vside);


    // Direct manipulation of completions

    void push_back(const Completion& completion);

    Completion& getCompletion();

    list<Completion>& getCompletions();


    string str(const RanksNames& ranksNames) const;
};
#endif
