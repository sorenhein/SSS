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

    // Used for lengths.

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

    // Helper fill functions.

    void fillLengthOrdinal(
      const unsigned char oppsLength,
      const Opponent simplestOpponent,
      Phrase& phrase);

    void fillTopsActual(
      const Opponent side,
      Phrase& phrase);

    void fillCountHonorsOrdinal(
      const unsigned char oppsLength,
      const VerbalSide& vside);

    void fillFreeCount(
      const VerbalSide& vside,
      Phrase& phrase) const;

    // SENTENCE_TOPS
    void fillTops(const VerbalSide& vside);


  public:

    VerbalCover();

    void setLength(const Term& length);

    // Fills.

    // SENTENCE_LENGTH
    void fillLength(
      const Term& lengthIn,
      const unsigned char oppsLength,
      const bool symmFlag);

    // SENTENCE_TOPS
    // See above

    // SENTENCE_LENGTH_BELOW_TOPS
    void fillLengthBelowTops(
      const unsigned char numBottoms,
      const unsigned char rankNo,
      const VerbalSide& vside);

    // SENTENCE_COUNT_TOPS
    void fillCountTops(
      const Term& lengthIn,
      const unsigned char oppsLength,
      const bool symmFlag);

    // SENTENCE_FILL_ORDINAL_FROM_TOPS
    void fillOrdinalFromTops(
      const VerbalSide& vside,
      const unsigned char lenCompletion);

 
    // SENTENCE_COUNT_TOPS
    // TODO Occurs multiple times

    // SENTENCE_EXACTLY_COUNT_TOPS
    void fillExactlyCountTops(const VerbalSide& vside);

    // SENTENCE_TOPS_ORDINAL

    // SENTENCE_COUNT_TOPS_ORDINAL
    void fillCountTopsOrdinal(
      const Term& lengthIn,
      const Term& top,
      const Profile& sumProfile,
      const unsigned char onetopIndex,
      const VerbalSide& vside);

    // SENTENCE_COUNT_HONORS_ORDINAL
    // See above

    // SENTENCE_EXACTLY_COUNT_TOPS_ORDINAL
    void fillExactlyCountTopsOrdinal(
      const Profile& sumProfile,
      const VerbalSide& vside);

    // SENTENCE_TOPS_AND_LOWER
    void fillTopsAndLower(const VerbalSide& vside);

    // SENTENCE_EXACTLY_TOPS_AND_LOWER
    void fillExactlyTopsAndLower(const VerbalSide& vside);

    // SENTENCE_TOPS_AND_COUNT_BELOW_CARD
    void fillTopsAndCountBelowCard(
      const VerbalSide& vside,
      const unsigned char numOptions);

    // SENTENCE_EXACTLY_TOPS_MAYBE_UNSET
    void fillExactlyTopsMaybeUnset(const VerbalSide& vside);

    // SENTENCE_TOPS_AND_XES
    void fillTopsAndXes(const VerbalSide& vside);

    // SENTENCE_EXACTLY_LIST
    // SENTENCE_2SPLIT_TOPS_SYMM
    // SENTENCE_2SPLIT_TOPS_ASYMM
    // SENTENCE_2SPLIT_TOPS_DIGITS_SYMM
    // SENTENCE_2SPLIT_TOPS_DIGITS_ASYMM



    void fillOnetopOnlyOld(
      const Term& top,
      const unsigned char oppsSize,
      const unsigned char onetopIndex,
      const VerbalSide& vside);

    void fillCountTops(
      const Term& top,
      const unsigned char oppsSize,
      const unsigned char onetopIndex,
      const VerbalSide& vside);

    void fillOnesided(
      const Profile& sumProfile,
      const VerbalSide& vside);

    void fillTopsBothLength(
      const Profile& sumProfile,
      const VerbalSide& vside);

    void fillTopsBoth(const VerbalSide& vside);

    void fillTopsAndLowerMultiple(
      const Profile& sumProfile,
      const VerbalSide& vside,
      const unsigned char numOptions);

    void fillSingular(
      const Profile& sumProfile,
      const unsigned char lenCompletion,
      const VerbalSide& vside);

    void fillCompletion(const VerbalSide& vside);


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
