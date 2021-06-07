/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_PLAYS_H
#define SSS_PLAYS_H

#include <list>
#include <string>

#include "Play.h"
#include "Nodes.h"
#include "DebugPlay.h"


using namespace std;

class Combinations;
class Distribution;
class Strategies;


class Plays
{
  private:

    unsigned cards;

    // We could just always push to the back, but for efficiency
    // we make more play slots in chunks.
    list<Play> plays;
    list<Play>::iterator nextPlaysIter;
    unsigned playChunkSize;

    Nodes nodesLead;
    Nodes nodesLho;
    Nodes nodesPard;
    Nodes nodesRho;
    Node nodeMaster;


    void getNextStrategies(
      Distribution const * distPtr,
      const DebugPlay debugFlag);

    void strategizeSimpleFront(const DebugPlay debugFlag);
    void strategizeSimpleBack(const DebugPlay debugFlag);

    string strNodeCounts(const string& title = "") const;

  public:
    
    Plays();

    ~Plays();

    void clear();

    void clearStrategies();

    void resize(const unsigned cardsIn);

    unsigned size() const;

    void log(const Play& play);

    void setCombPtrs(const Combinations& combinations);

    const Strategies& strategize(
      Distribution const * distPtr,
      const DebugPlay debugFlag = DEBUGPLAY_NONE);

    string str() const;
};

#endif
