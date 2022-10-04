/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_VERBALCOVER_H
#define SSS_VERBALCOVER_H

#include <vector>
#include <string>

#include "Completion.h"

#include "VerbalBlank.h"
#include "VerbalTemplates.h"

#include "../term/Term.h"
#include "../term/CoverOperator.h"

#include "../../../utils/table.h"

using namespace std;

class Profile;
class RanksNames;
struct TemplateData;
struct VerbalData;
enum Opponent: unsigned;


struct VerbalSide
{
  Opponent side;
  bool symmFlag;

  BlankPlayerCap blank() const
  {
    if (side == OPP_WEST)
      return (symmFlag ? BLANK_PLAYER_CAP_EITHER : BLANK_PLAYER_CAP_WEST);
    else
      return (symmFlag ? BLANK_PLAYER_CAP_EITHER : BLANK_PLAYER_CAP_EAST);
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

    vector<TemplateData> templateFills;

    bool lengthFlag;
    unsigned char lengthLower;
    unsigned char lengthUpper;
    CoverOperator lengthOper;

    Completion west;

    Completion east;

    Completion completion;

    string strTMP; // TODO Fix strGeneral and setGeneral


    Opponent simplestOpponent(const unsigned char oppsLength) const;

    void getLengthEqualData(
      const unsigned char oppsLength,
      const VerbalSide& vside,
      const bool abstractableFlag,
      vector<TemplateData>& tdata) const;

    void getLengthInsideData(
      const unsigned char oppsLength,
      const VerbalSide& vside,
      const bool abstractableFlag,
      vector<TemplateData>& tdata) const;

    void getLengthData(
      const unsigned char oppsLength,
      const VerbalSide& vside,
      const bool abstractableFlag,
      vector<TemplateData>& tdata) const;

    void getTopsData(
      const BlankPlayerCap side,
      const Completion& completion,
      const RanksNames& ranksNames,
      vector<TemplateData>& tdata) const;

    void fillLengthAdjElement(
      const unsigned char oppsLength,
      const Opponent simplestOpponent);

    void getOnetopElement(
      const unsigned char oppsValue1,
      const unsigned char oppsValue2,
      const unsigned char oppsSize,
      const unsigned char onetopIndex,
      TemplateData& telement) const;



  public:

    VerbalCover();

    void push_back(const Completion& completion);


    // Length only

    void setLength(const Term& length);

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

    void fillTopsExcluding(
      const VerbalSide& vside,
      const RanksNames& ranksNames);

    void fillBelow(
      const unsigned char freeLower,
      const unsigned char freeUpper,
      const unsigned char numBottoms,
      const RanksNames& ranksNames,
      const unsigned char rankNo,
      const VerbalSide& vside);

    void fillSingular(
      const unsigned char lenCompletion,
      const VerbalSide& vside);


    Completion& getCompletion();

    void setSentence(const Sentence sentenceIn);

    void stable_sort();

    unsigned char size() const;

    void fillCompletion(
      const VerbalSide& vside,
      const RanksNames& ranksNames);

    void fillCompletionWithLows(
      const VerbalSide& vside,
      const RanksNames& ranksNames);

    void fillBottoms(
      const VerbalSide& vside,
      const RanksNames& ranksNames);

    void fillTopsAndLower(
      const VerbalSide& vside,
      const RanksNames& ranksNames,
      const unsigned char numOptions);

    void fillList(
      const VerbalSide& vside,
      const RanksNames& ranksNames,
      const list<Completion>& completions);

    void setGeneral(
      const unsigned char oppsLength,
      const bool symmFlag,
      const RanksNames& ranksNames);

    // TODO Later on private again
    void getOnetopData(
      const unsigned char oppsValue1,
      const unsigned char oppsValue2,
      const unsigned char oppsLength,
      const unsigned char onetopIndex,
      const BlankPlayerCap side,
      vector<TemplateData>& tdata) const;

    string str(const RanksNames& ranksNames) const;
};
#endif
