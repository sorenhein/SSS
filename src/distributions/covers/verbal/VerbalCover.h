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

#include "../term/CoverOperator.h"

using namespace std;

class Length;
class RanksNames;
struct TemplateData;
enum Opponent: unsigned;


class VerbalCover
{
  // A verbal cover can be a list of completions, e.g. 97x, 97 or 7x.
  // Or it can be the combination of a length constraint, a set of
  // exact West cards and another set of exact East cards.
  // Each of these three components can be present or not.

  private:
    
    list<Completion> completions;

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

    string strCompletions(const RanksNames& ranksNames) const;


  public:

    VerbalCover();

    void push_back(const Completion& completion);

    void setLength(const Length& length);

    Completion& activateSide(const Opponent opponent);

    void setSide(
      const Completion& verbalComb,
      const Opponent opponent);

    void stable_sort();

    unsigned char size() const;

    string str(const RanksNames& ranksNames) const;

    string strGeneral(
      const unsigned char oppsLength,
      const bool symmFlag,
      const RanksNames& ranksNames,
      vector<TemplateData>& tdata) const;
};
#endif
