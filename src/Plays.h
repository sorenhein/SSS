#ifndef SSS_PLAYS_H
#define SSS_PLAYS_H

#include <vector>
#include <string>

#include "strategies/Tvectors.h"
#include "const.h"

using namespace std;

class Combinations;
class Combination;
class Distribution;


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

      Tvectors strategies;
    };

    struct LhoNode
    {
      unsigned lho;

      LeadNode * leadPtr;

      Tvectors strategies;
    };

    struct PardNode
    {
      unsigned pard;

      LhoNode * lhoPtr;

      Tvectors strategies;
    };

    struct RhoNode
    {
      unsigned rho;

      bool leadCollapse; // Does the lead rank go away after this trick?
      bool lhoCollapse;
      bool pardCollapse;
      bool rhoCollapse;

      unsigned cardsNew;
      unsigned holdingNew;
      bool rotateNew;
      Combination const * combPtr;

      unsigned trickNS;

      bool knownVoidLho;
      bool knownVoidRho;
      bool voidPard;

      PardNode * pardPtr;
    };

    unsigned cards;

    list<LeadNode> leadNodes;
    list<LhoNode> lhoNodes;
    list<PardNode> pardNodes;
    list<RhoNode> rhoNodes;

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
      const SidePosition side,
      const unsigned lead,
      bool& newFlag);

    LhoNode * logLho(
      const unsigned lho,
      LeadNode * leadPtr,
      bool& newFlag);

    PardNode * logPard(
      const unsigned pard,
      LhoNode * lhoPtr,
      bool& newFlag);

    void logRho(
      const unsigned rho,
      const bool leadCollapse,
      const bool lhoCollapse,
      const bool pardCollapse,
      const bool rhoCollapse,
      const unsigned holding3,
      const bool rotateFlag,
      const unsigned trickNS,
      const bool knownVoidLho,
      const bool knownVoidRho,
      const bool voidPard,
      PardNode * pardPtr);

    string strHeader() const;

  public:
    
    Plays();

    ~Plays();

    void reset();

    void resize(const unsigned cardsIn);

    unsigned size() const;

    void log(
      const SidePosition side,
      const unsigned lead,
      const unsigned lho,
      const unsigned pard,
      const unsigned rho,
      const bool leadCollapse,
      const bool lhoCollapse,
      const bool pardCollapse,
      const bool rhoCollapse,
      const unsigned holding3,
      const bool rotateFlag);

   void setCombPtrs(const Combinations& combinations);

   void strategize(
     Distribution const * distPtr,
     Tvectors& strategies);

   string str() const;
};

#endif
