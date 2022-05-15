/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_TABLEAUSTATS_H
#define SSS_TABLEAUSTATS_H

#include <string>


using namespace std;

struct TableauStats
{
  size_t stackActual;
  unsigned firstFix;
  unsigned numSolutions;
  size_t stackMax;
  unsigned numSteps;
  unsigned numCompares;
  size_t numBranches;

  void reset()
  {
    stackActual = 0;
    firstFix = 0;
    numSolutions = 0;
    stackMax = 0;
    numSteps = 0;
    numCompares = 0;
    numBranches = 0;
  };

  string strHeader() const
  {
    stringstream ss;
    ss << 
      setw(2) << "" <<
      setw(12) << "Stack" <<
      setw(8) << "TTFF" <<
      setw(8) << "Numsol" <<
      setw(12) << "Smax" <<
      setw(12) << "Comps" <<
      setw(12) << "Steps" <<
      setw(8) << "Branch" << "\n";
    return ss.str();
  };

  string str(const string& ID) const
  {
    stringstream ss;
    ss << 
      setw(2) << ID <<
      setw(12) << stackActual <<
      setw(8) << firstFix <<
      setw(8) << numSolutions <<
      setw(12) << stackMax <<
      setw(12) << numCompares <<
      setw(12) << numSteps <<
      setw(8) << setprecision(2) << fixed <<
        static_cast<float>(numBranches) / static_cast<float>(numSteps) << "\n";
    return ss.str();
  };
};

#endif
