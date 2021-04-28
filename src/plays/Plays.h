#ifndef SSS_PLAYS_H
#define SSS_PLAYS_H

#include <vector>
#include <string>

#include "../strategies/Card.h"
#include "../strategies/Strategies.h"

#include "Play.h"
#include "Nodes.h"

#include "../const.h"

using namespace std;

class Combinations;
class Combination;
class Distribution;
class Ranks;


struct ChunkEntry
{
  unsigned lead;
  unsigned lho;
  unsigned pard;
  unsigned rho;
};


class Plays
{
  private:

    struct RhoStudyNode
    {
      Play const * playPtr;
      unsigned playNo;
      unsigned leadNo;

      Strategies strategies;

      // Distribution-wise statistics across all strategies in the play.
      Bounds bounds;
    };

    unsigned cards;

    list<RhoStudyNode> rhoStudyNodes;

    ChunkEntry chunk;

    list<Play> plays;
    Nodes nodesRho, nodesPard, nodesLho, nodesLead;
    Node nodeMaster;


    void strategizeRHO(
      Distribution const * distPtr,
      const bool debugFlag);

    void strategizePard(const bool debugFlag);

    void strategizeLHO(const bool debugFlag);

    void strategizeLead(const bool debugFlag);

    unsigned studyRHO(
      Distribution const * distPtr,
      const bool debugFlag);

    void studyGlobal(
      vector<Bounds>& boundsLead,
      const bool debugFlag);

    bool removePlay(
      const Strategies& strategies,
      Strategies& simpleStrat) const;

    void removeConstants(
      const vector<Bounds>& boundsLead,
      vector<Strategies>& simpleStrats);

    void removeDominatedDefenses(
      const vector<Bounds>& boundsLead,
      vector<Strategies>& simpleStrats);

    void removeLaterCollapses();

  public:
    
    Plays();

    ~Plays();

    void reset();

    void resize(const unsigned cardsIn);

    unsigned size() const;

    void log(const Play& play);

    void setCombPtrs(const Combinations& combinations);

    void strategize(
      const Ranks& ranks,
      Distribution const * distPtr,
      Strategies& strategies,
      const bool debugFlag = false);

    void strategizeVoid(
      Distribution const * distPtr,
      Strategies& strategies,
      const bool debugFlag = false);

    string str() const;
};

#endif
