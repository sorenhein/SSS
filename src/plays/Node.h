#ifndef SSS_NODE_H
#define SSS_NODE_H

#include <list>
#include <string>

#include "../strategies/Strategies.h"

#include "../const.h"


using namespace std;

struct Play;


class Node
{
  // A node is a record in the tree of possible plays.
  // A node is part of Nodes.  Nodes knows its level (LHO, RHO, ...).

  private:

    Node * parentPtr;

    Play const * playPtr;

    Strategies strategies;

  public:
    
    Node();

    ~Node();

    void reset();

    void operator *=(const Strategies strat2);

    string strPlay(const Level level) const;

    string str(
      const string& title = "",
      const bool rankFlag = false) const;
};

#endif
