#ifndef SSS_PLAYS_H
#define SSS_PLAYS_H

#include <vector>
#include <string>

#include "strategies/Card.h"
#include "strategies/Strategies.h"
#include "Play.h"
#include "const.h"

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

    struct LeadNode
    {
      SidePosition side;
      unsigned lead;

      Strategies strategies;
    };

    struct LhoNode
    {
      unsigned lho;

      LeadNode * leadPtr;

      Strategies strategies;
    };

    struct PardNode
    {
      unsigned pard;

      LhoNode * lhoPtr;

      Strategies strategies;
    };

    struct RhoNode
    {
      PardNode * pardPtr;

      // TMP
      Play play;
    };

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

    list<LeadNode> leadNodes;
    list<LhoNode> lhoNodes;
    list<PardNode> pardNodes;
    list<RhoNode> rhoNodes;
    list<RhoStudyNode> rhoStudyNodes;

    unsigned leadNext;
    unsigned lhoNext;
    unsigned pardNext;
    unsigned rhoNext;

    list<LeadNode>::iterator leadNextIter;
    list<LhoNode>::iterator lhoNextIter;
    list<PardNode>::iterator pardNextIter;
    list<RhoNode>::iterator rhoNextIter;

    // The plays come through in order, so we don't have to look up
    // the new play completely.  We just have to check how much of the
    // new play is the same as the old one.
    SidePosition sidePrev;
    unsigned leadPrev;
    unsigned lhoPrev;
    unsigned pardPrev;

    LeadNode * leadPrevPtr;
    LhoNode * lhoPrevPtr;
    PardNode * pardPrevPtr;

    ChunkEntry chunk;


    LeadNode * logLead(
      const Play& play,
      bool& newFlag);

    LhoNode * logLho(
      const Play& play,
      LeadNode * leadPtr,
      bool& newFlag);

    PardNode * logPard(
      const Play& play,
      LhoNode * lhoPtr,
      bool& newFlag);

    void logRho(
      const Play& play,
      PardNode * pardPtr);

    void strategizeRHO(
      Distribution const * distPtr,
      const bool debugFlag);

    void strategizePard(const bool debugFlag);

    void strategizeLHO(const bool debugFlag);

    void strategizeLead(
      Strategies& strategies,
      const bool debugFlag);

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
