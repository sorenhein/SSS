#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Play.h"

#include "../strategies/Card.h"


unsigned Play::lead(const bool fullFlag) const
{
  if (fullFlag)
    return leadPtr->getNumber();
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


Card const * Play::northTranslate(const unsigned number) const
{
  unsigned northThisTrick;
  if (side == POSITION_NORTH)
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
  if (side == POSITION_NORTH)
    southThisTrick = pardPtr->getNumber();
  else
    southThisTrick = leadPtr->getNumber();

  const unsigned translated = (number < southThisTrick ? number : number+1);
  
  assert(translated < southCardsPtr->size());
  return (* southCardsPtr)[translated];
}


string Play::strTrick(const unsigned number) const
{
  stringstream ss;
  ss << "Play #" << number << ", " <<
    (side == POSITION_NORTH ? "North" : "South") << ": " <<
    leadPtr->getName() << " " <<
    lhoPtr->getName() << " " <<
    pardPtr->getName() << " " <<
    rhoPtr -> getName() << " (" <<
    (rotateFlag ? "rotate" : "no rotate") << ")" << endl;

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
    endl;

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
    setw(5) << (lhoPtr->isVoid() ? "yes" : "") <<
    setw(5) << (rhoPtr->isVoid() ? "yes" : "") <<
    setw(10) << holding3 <<
    endl;

  return ss.str();
}
