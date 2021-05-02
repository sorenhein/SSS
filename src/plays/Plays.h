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
class Ranks;
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


    void getStrategies(
      Distribution const * distPtr,
      const DebugPlay debugFlag);

    string strNodeCounts(const string& title = "") const;

  public:
    
    Plays();

    ~Plays();

    void clear();

    void resize(const unsigned cardsIn);

    unsigned size() const;

    void log(const Play& play);

    void setCombPtrs(const Combinations& combinations);

    void strategize(
      const Ranks& ranks,
      Distribution const * distPtr,
      Strategies& strategies,
      const DebugPlay debugFlag = DEBUGPLAY_NONE);

    void strategizeNew(
      const Ranks& ranks,
      Distribution const * distPtr,
      Strategies& strategies,
      const DebugPlay debugFlag = DEBUGPLAY_NONE);

    void strategizeVoid(
      Distribution const * distPtr,
      Strategies& strategies,
      const DebugPlay debugFlag = DEBUGPLAY_NONE);

    string str() const;
};

#endif
