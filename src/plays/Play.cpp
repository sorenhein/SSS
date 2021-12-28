/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Play.h"

#include "../combinations/Combinations.h"
#include "../inputs/Control.h"
#include "../utils/Card.h"

extern Control control;


unsigned Play::lead(const bool fullFlag) const
{
  if (fullFlag)
    return leadPtr->getAbsNumber();
    // I think abs is right
    // return leadPtr->getNumber();
  else
    return leadPtr->getRank();
}


unsigned Play::lho(const bool fullFlag) const
{
  if (fullFlag)
    return lhoPtr->getNumber();
  else
    return lhoPtr->getRank(); 
}


unsigned Play::pard(const bool fullFlag) const
{
  if (fullFlag)
    return pardPtr->getNumber();
  else
    return pardPtr->getRank();
}


unsigned Play::rho(const bool fullFlag) const
{
  if (fullFlag)
    return rhoPtr->getNumber();
  else
    return rhoPtr->getRank(); 
}


void Play::setCombPtr(const Combinations& combinations)
{
  // TODO We don't deal with multiple minimals yet.
  // Once ranks don't matter, we can switch to COMB_MIN_FULL
  // (or abolish the whole parameter).
  bool rotateMoreFlag;
  combPtr = combinations.getPtr(cardsLeft, holding3, 
    // COMB_MIN_IGNORE, rotateMoreFlag);
    COMB_MIN_SINGLE, rotateMoreFlag);

  rotateFlag ^= rotateMoreFlag;
}


void Play::setVoidFlags(
  bool& westFlag,
  bool& eastFlag) const
{
  if (side == SIDE_NORTH)
  {
    westFlag = rhoPtr->isVoid();
    eastFlag = lhoPtr->isVoid();
  }
  else
  {
    westFlag = lhoPtr->isVoid();
    eastFlag = rhoPtr->isVoid();
  }
}


Card const * Play::northTranslate(const unsigned number) const
{
  unsigned northThisTrick;
  if (side == SIDE_NORTH)
    northThisTrick = leadPtr->getNumber();
  else
    northThisTrick = pardPtr->getNumber();

  // The North card of this play punched out one of the North cards.
  // So when we have to translate a later North winner, we have to
  // increment its number except if it was a low later winner.
  const unsigned translated = (number < northThisTrick ? number : number+1);

  assert(translated < northCardsPtr->size());
  return (* northCardsPtr)[translated];
}


Card const * Play::southTranslate(const unsigned number) const
{
  unsigned southThisTrick;
  if (side == SIDE_NORTH)
    southThisTrick = pardPtr->getNumber();
  else
    southThisTrick = leadPtr->getNumber();

  const unsigned translated = (number < southThisTrick ? number : number+1);
  
  assert(translated < southCardsPtr->size());
  return (* southCardsPtr)[translated];
}


bool Play::samePartial(
  const Play& play2,
  const Level level) const
{
  // TODO When we switch to number rather than rank, we have a problem
  // here: Both a void and a lowest card have number 0...
  const bool flag = control.runRankComparisons();

  if (level == LEVEL_LEAD)
    return (side == play2.side && Play::lead(flag) == play2.lead(flag));
  else if (level == LEVEL_LHO)
    return (Play::lho() == play2.lho());
  else if (level == LEVEL_PARD)
    return (Play::pard(flag) == play2.pard(flag));
  else if (level == LEVEL_RHO)
    return false;
  else
  {
    assert(false);
    return false;
  }
}


string Play::strPartialTrick(const Level level) const
{
  stringstream ss;

  ss <<
    (side == SIDE_NORTH ? "North" : "South") << ": " <<
      leadPtr->getName() << " ";

  if (level == LEVEL_RHO)
  {
    ss <<
      lhoPtr->getName() << " " <<
      pardPtr->getName() << " " <<
      rhoPtr -> getName() << " (" <<
      (rotateFlag ? "rotate" : "no rotate") << ")";
  }
  else if (level == LEVEL_PARD)
    ss << lhoPtr->getName() << " " << pardPtr->getName();
  else if (level == LEVEL_LHO)
    ss << lhoPtr->getName() << " ";
  else
    assert(level == LEVEL_LEAD);

  return ss.str() + "\n";
}


string Play::strTrick(const unsigned number) const
{
  stringstream ss;
  ss << "Play #" << number << ", " <<
    (side == SIDE_NORTH ? "North" : "South") << ": " <<
    leadPtr->getName() << " " <<
    lhoPtr->getName() << " " <<
    pardPtr->getName() << " " <<
    rhoPtr -> getName() << " (" <<
    (rotateFlag ? "rotate" : "no rotate") << ")" << "\n";

  return ss.str();
}


string Play::strHeader() const
{
  stringstream ss;

  ss << right <<
    setw(4) << "Side" <<
    setw(5) << "Lead" <<
    setw(5) << "LHO" <<
    setw(5) << "Pard" <<
    setw(5) << "RHO" <<
    setw(5) << "Win?" <<
    setw(5) << "W vd" <<
    setw(5) << "E vd" <<
    setw(10) << "Holding" <<
    setw(4) << "Rot" <<
    "\n";

  return ss.str();
}


string Play::strLine() const
{
  stringstream ss;

  ss << right <<
    setw(4) << (side == SIDE_NORTH ? "N" : "S") <<
    setw(5) << leadPtr->getName() <<
    setw(5) << lhoPtr->getName() <<
    setw(5) << pardPtr->getName() <<
    setw(5) << rhoPtr->getName() <<
    setw(5) << (trickNS == 1 ? "+" : "") <<
    setw(5) << (lhoPtr->isVoid() ? "yes" : "-") <<
    setw(5) << (rhoPtr->isVoid() ? "yes" : "-") <<
    setw(10) << holding3 <<
    setw(4) << (rotateFlag ? "yes" : "-") <<
    "\n";

  return ss.str();
}
