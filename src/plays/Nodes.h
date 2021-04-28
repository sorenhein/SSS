#ifndef SSS_NODES_H
#define SSS_NODES_H

#include <list>
#include <string>

#include "Node.h"

#include "../const.h"


using namespace std;


class Nodes
{
  // Nodes are a collection of Node's with some information about
  // the Node's are hooked up.  Nodes knows its level (LHO, RHO, ...).
  //
  // For efficiency we don't add a new node every time log() is called.
  // Instead we allocate chunks whenever we run out of space.
  // begin() and end() only loop over the used part of the list.

  private:

    Level level;

    list<Node> nodes;

    list<Node>::iterator nextIter;

    Node * prevPtr;

    Play const * prevPlayPtr;

    unsigned nextEntryNumber;

    unsigned chunkSize;

  public:
    
    Nodes();

    ~Nodes();

    void reset();

    void clear();

    void resize(
      const Level levelIn,
      const unsigned cards);

    Node * log(
      Node * parentPtr,
      Play * playPtr,
      bool& newFlag);

    list<Node>::iterator begin();
    list<Node>::iterator end();

    unsigned size() const;
    unsigned used() const;

    string strCount() const;
};

#endif
