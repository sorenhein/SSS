/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_VERBALCOVER_H
#define SSS_VERBALCOVER_H

#include <vector>
#include <string>

#include "./Completion.h"
#include "Slot.h"

#include "VerbalTemplates.h"
#include "VerbalDimensions.h"

#include "../term/Term.h"

#include "../../../utils/table.h"

using namespace std;

class Profile;
class RanksNames;
struct VerbalData;
enum Opponent: unsigned;
enum CoverOperator: unsigned;


// TODO Put it somewhere and avoid include of table.h?
struct VerbalSide
{
  Opponent side;
  bool symmFlag;

  VerbalPhrase player() const
  {
    if (side == OPP_WEST)
      return (symmFlag ? PLAYER_EITHER : PLAYER_WEST);
    else
      return (symmFlag ? PLAYER_EITHER : PLAYER_EAST);
  };
};


class VerbalCover
{
  // A verbal cover can be a list of completions, e.g. 97x, 97 or 7x.
  // Or it can be the combination of a length constraint, a set of
  // exact West cards and another set of exact East cards.
  // Each of these three components can be present or not.

  private:
    
    Sentence sentence;

    list<Completion> completions;

    vector<Slot> slots;

    // TODO Proper Term (length)?
    bool lengthFlag;
    unsigned char lengthLower;
    unsigned char lengthUpper;
    CoverOperator lengthOper;

    string strTMP; // TODO Fix strGeneral and setGeneral


    Opponent simplestOpponent(const unsigned char oppsLength) const;

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

    void fillLengthAdjElement(
      const unsigned char oppsLength,
      const Opponent simplestOpponent);

    void getOnetopElement(
      const unsigned char oppsValue1,
      const unsigned char oppsValue2,
      const unsigned char oppsSize,
      const unsigned char onetopIndex,
      Slot& slot) const;

    void getOnetopData(
      const unsigned char oppsValue1,
      const unsigned char oppsValue2,
      const unsigned char oppsLength,
      const unsigned char onetopIndex,
      const VerbalPhrase player);




  public:

    VerbalCover();

    void push_back(const Completion& completion);


    // Length only

    void setLength(const Term& length);

    void setLength(
      const unsigned char lower,
      const unsigned char upper,
      const unsigned char maximum);

    void fillLengthOnly(
      const Term& length,
      const unsigned char oppsLength,
      const bool symmFlag);

    string strLengthOnly() const;


    // One top only

    void fillOnetopOnly(
      const Term& top,
      const unsigned char oppsSize,
      const unsigned char onetopIndex,
      const VerbalSide& vside);

    void fillOnetopLength(
      const Term& length,
      const Term& top,
      const Profile& sumProfile,
      const unsigned char onetopIndex,
      const VerbalSide& vside);

    void fillTopsExcluding(const VerbalSide& vside);

    void fillBelow(
      const unsigned char numBottoms,
      const unsigned char rankNo,
      const VerbalSide& vside);

    void fillSingular(
      const unsigned char lenCompletion,
      const VerbalSide& vside);


    Completion& getCompletion();

    list<Completion>& getCompletions();

    // void setSentence(const Sentence sentenceIn);

    void fillCompletion(const VerbalSide& vside);

    void fillCompletionWithLows(const VerbalSide& vside);

    void fillBottoms(const VerbalSide& vside);

    void fillTopsAndLower(
      const VerbalSide& vside,
      const RanksNames& ranksNames,
      const unsigned char numOptions);

    void fillList(const VerbalSide& vside);

    void setGeneral(
      const unsigned char oppsLength,
      const bool symmFlag,
      const RanksNames& ranksNames);

    string str(const RanksNames& ranksNames) const;
};
#endif
