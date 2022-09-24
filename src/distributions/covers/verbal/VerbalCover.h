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

#include "../term/Term.h"
#include "../term/CoverOperator.h"

using namespace std;

class RanksNames;
struct TemplateData;
enum Opponent: unsigned;
enum BlankPlayerCap: unsigned;


class VerbalCover
{
  // A verbal cover can be a list of completions, e.g. 97x, 97 or 7x.
  // Or it can be the combination of a length constraint, a set of
  // exact West cards and another set of exact East cards.
  // Each of these three components can be present or not.

  private:
    
    list<Completion> completions;

    vector<TemplateData> templateFills;

    bool lengthFlag;
    unsigned char lengthLower;
    unsigned char lengthUpper;
    CoverOperator lengthOper;

    bool westFlag;
    Completion west;

    bool eastFlag;
    Completion east;


    Opponent simplestOpponent(const unsigned char oppsLength) const;

    void getLengthEqualData(
      const unsigned char oppsLength,
      const Opponent simplestOpponent,
      const bool symmFlag,
      vector<TemplateData>& tdata) const;

    void getLengthInsideData(
      const unsigned char oppsLength,
      const Opponent simplestOpponent,
      const bool symmFlag,
      vector<TemplateData>& tdata) const;

    void getLengthData(
      const unsigned char oppsLength,
      const Opponent simplestOpponent,
      const bool symmFlag,
      vector<TemplateData>& tdata) const;

    void getTopsData(
      const BlankPlayerCap side,
      const Completion& completion,
      const RanksNames& ranksNames,
      vector<TemplateData>& tdata) const;



  public:

    VerbalCover();

    void push_back(const Completion& completion);


    // Length only

    void setLength(const Term& length);

    void fillLengthOnly(
      const unsigned char oppsLength,
      const bool symmFlag);

    string strLengthOnly() const;


    // One top only

    void fillOnetopOnly(
      const Term& top,
      const unsigned char oppsSize,
      const unsigned char onetopIndex,
      const bool symmflag);

    string strOnetopOnly(const RanksNames& ranksNames) const;

    string strOnetopLength(const RanksNames& ranksNames) const;


    Completion& activateSide(const Opponent opponent);

    void setSide(
      const Completion& verbalComb,
      const Opponent opponent);

    void stable_sort();

    unsigned char size() const;

    void makeList(
      const BlankPlayerCap side,
      const RanksNames& ranksNames,
      vector<TemplateData>& tdata) const;

    string strGeneral(
      const unsigned char oppsLength,
      const bool symmFlag,
      const RanksNames& ranksNames,
      vector<TemplateData>& tdata) const;

    // TODO Later on private again
    void getOnetopData(
      const unsigned char oppsValue1,
      const unsigned char oppsValue2,
      const unsigned char oppsLength,
      const unsigned char onetopIndex,
      const BlankPlayerCap side,
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

};
#endif
