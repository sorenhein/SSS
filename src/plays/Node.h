#ifndef SSS_NODE_H
#define SSS_NODE_H

#include <list>
#include <string>

#include "../strategies/Strategies.h"

#include "../const.h"


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

  public:
    
    Node();

    ~Node();

    void reset();

    void set(
      Node * parentPtr,
      Play * playPtr);

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

    string strPlay(const Level level) const;

    string str(
      const string& title = "",
      const bool rankFlag = false) const;
};

#endif
