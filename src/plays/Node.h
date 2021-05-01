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

    Bounds bounds;

    // The number of the entry in the corresponding Nodes.
    unsigned index;

  public:
    
    Node();

    ~Node();

    void reset();

    void set(
      Node * parentPtr,
      Play * playPtr,
      Node const * prevNodePtr);

    void linkRhoToLead();

    void setCombPtr(const Combinations& combinations);

    void getStrategies(
      const Distribution& dist,
      const bool debugFlag);

    void cross(
      const Level level,
      const bool debugFlag);

    void add(
      const Level level,
      const bool debugFlag);

    void operator *=(const Strategy& strat2);

    void operator |=(const Node& node2);

    void bound();

    void propagateBounds();

    void activateBounds();

    void purge(const Strategy& constants);

    const Play& play() const;

    const Strategies& strategies() const;

    unsigned indexParent() const;

    string strPlay(const Level level) const;

    string strPlayLineHeader() const;

    string strPlayLine() const;

    string str(
      const string& title = "",
      const bool rankFlag = false) const;

    // TODO Delete
    unsigned indexTMP() const;
};

#endif
