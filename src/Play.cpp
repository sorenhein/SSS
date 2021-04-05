#include <iostream>
#include <iomanip>
#include <sstream>

#include "Play.h"
#include "strategies/Card.h"


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
