/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_VERBALDATA_H
#define SSS_VERBALDATA_H

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

using namespace std;


struct VerbalData
{
  unsigned char topsUsed;
  unsigned char ranksUsed;
  unsigned char ranksActive; // Non-zero rank entry
  unsigned char lowestRankUsed;
  unsigned char lowestRankActive; // Non-zero rank entry
  bool partialFlag; // Entry used (> 0) by both sides
  bool zeroUsedFlag; // Entry with a zero active value occurs
  unsigned char topsFull; // Count of tops in the ranksOver ranks
  unsigned char ranksFull; // Nuber of ranks active, but not on other side
  unsigned char freeLower;
  unsigned char freeUpper;

  void reset()
  {
    topsUsed = 0;
    ranksUsed = 0;
    ranksActive = 0;
    lowestRankUsed = 0;
    lowestRankActive = 0;
    partialFlag = false;
    zeroUsedFlag = false;
    topsFull = 0;
    ranksFull = 0;
    freeLower = 0;
    freeUpper = 0;
  }


  void update(
    const unsigned char topNo,
    const unsigned char value,
    const unsigned char valueMax)
  {
    topsUsed += value;
    ranksUsed++;

    lowestRankUsed = topNo;
    if (value)
    {
      ranksActive++;
      lowestRankActive = topNo;
    }

    if (value == valueMax)
    {
      topsFull += valueMax;
      ranksFull++;
    }

    if (value == 0)
      zeroUsedFlag = true;

    if (value > 0 && value < valueMax)
      partialFlag = true;
  }


  string strXes(
    const bool dashFlag,
    const bool expandFlag) const
  {
    // We only have to set the x'es.
    const string text = 
      (expandFlag ?  " as well as " : (dashFlag ? "-" : ""));

    if (freeLower > 0)
    {
      return text + string(freeLower, 'x') +
        "(" + string(freeUpper - freeLower, 'x') + ")";
    }
    else
    {
      return text + "(" + string(freeUpper, 'x') + ")";
    }
  }


  string strFreeSemantic() const
  {
    if (freeLower == 0)
    {
      if (freeUpper == 1)
        return "at most a singleton";
      else if (freeUpper == 2)
        return "at most a doubleton";
      else if (freeUpper == 3)
        return "at most a tripleton";
      else
        return ("at most " + to_string(freeUpper) + " cards");
    }
    else if (freeLower == 1)
    {
      if (freeUpper == 1)
        return "a singleton";
      else if (freeUpper == 2)
        return "a singleton or doubleton";
      else
        return ( "1-" + to_string(freeUpper) + " cards");
    }
    else if (freeLower == 2 && freeUpper == 2)
      return "a doubleton";
    else if (freeLower == 3 && freeUpper == 3)
      return "a tripleton";
    else
      return to_string(freeLower) + "-" + to_string(freeUpper) + " cards";
  }


  string strOtherSemantic() const
  {
    if (freeLower == 0)
    {
      if (freeUpper == 1)
        return "at most one card";
      else if (freeUpper == 2)
        return "at most two cards";
      else if (freeUpper == 3)
        return "at most three cards";
      else
        return ("at most " + to_string(freeUpper) + " cards");
    }
    else if (freeLower == 1)
    {
      if (freeUpper == 1)
        return "one card";
      else if (freeUpper == 2)
        return "one or two cards";
      else
        return ( "1-" + to_string(freeUpper) + " cards");
    }
    else if (freeLower == 2 && freeUpper == 2)
      return "two cards";
    else if (freeLower == 3 && freeUpper == 3)
      return "three cards";
    else
      return to_string(freeLower) + "-" + to_string(freeUpper) + " cards";
  }


  string strFreeCount() const
  {
    if (freeLower == 0)
    {
      if (freeUpper == 1)
        return "at most one";
      else if (freeUpper == 2)
        return "at most two";
      else if (freeUpper == 3)
        return "at most three";
      else
        return "at most " + to_string(freeUpper);
    }
    else if (freeLower == 1)
    {
      if (freeUpper == 1)
        return "one";
      else
        return "1-" + to_string(freeUpper);
    }
    else if (freeLower == 2 && freeUpper == 2)
      return "two";
    else if (freeLower == 3 && freeUpper == 3)
      return "three";
    else
      return to_string(freeLower) + "-" + to_string(freeUpper);
  }


  string str(const string& header) const
  {
    stringstream ss;
    ss << header << "\n";;
    ss << "Top cards used     " << +topsUsed << "\n";
    ss << "Ranks used         " << +ranksUsed << "\n";
    ss << "Ranks active       " << +ranksActive << "\n";
    ss << "Lowest rank used   " << +lowestRankUsed << "\n";
    ss << "1+ partial ranks   " << (partialFlag ? "true" : "false") << "\n";
    ss << "Tops of full rank  " << +topsFull << "\n";
    ss << "Full ranks used    " << +ranksFull << "\n";
    ss << "Lowest rank active " << +lowestRankActive << "\n";
    ss << "Free low cards     " << +freeLower << " to " << 
      +freeUpper << "\n";
    return ss.str();
  };
};

#endif
