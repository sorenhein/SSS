/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_NODE_H
#define SSS_NODE_H

#include <string>

#include "../strategies/Strategies.h"


using namespace std;

class Distribution;
class Combinations;
struct Play;


class Node
{
  // A node is a record in the tree of possible plays.
  // A node is part of Nodes.  Nodes knows its level (LHO, RHO, ...).

  private:

    Node * parentPtr;

    Play * playPtr;

    Strategies strats;

    Strategy simpleStrat;

    Strategy constants;

    // The number of the entry in the corresponding Nodes.
    unsigned index;

  public:
    
    Node();

    void reset();

    void resetStrategies();

    void set(
      Node * parentPtr,
      Play * playPtr,
      Node const * prevNodePtr);

    void setCombPtr(const Combinations& combinations);

    void linkRhoToLead();

    void getNextStrategies(
      const Distribution& dist,
      const bool debugFlag);

    void propagateRanges();

    void purgeRanges(const bool debugFlag);

    void reactivate();

    void cross(
      const Level level,
      const bool debugFlag);

    void add(
      const Level level,
      const bool debugFlag);

    bool removePlay();

    const Play& play() const;

    Strategies& strategies();
    const Strategies& strategies() const;

    string strRanges(
      const string& title = "",
      const bool rankFlag = false) const;

    string strPlay(const Level level) const;

    string strSimple() const;
    string strSimpleParent() const;
};

#endif
