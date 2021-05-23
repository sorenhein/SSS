#ifndef SSS_NODES_H
#define SSS_NODES_H

#include <list>
#include <string>

#include "Node.h"


using namespace std;


class Nodes
{
  // Nodes are a collection of Node's with some information about
  // the Node's are hooked up.  Nodes knows its level (LHO, RHO, ...).
  //
  // For efficiency we don't add a new node every time log() is called.
  // Instead we allocate chunks whenever we run out of space.
  // begin() and end() only loop over the used part of the list.

  struct ParentConstants
  {
    Node * parentPtr;
    Strategy constants;
  };

  private:

    Level level;

    list<Node> nodes;

    list<Node>::iterator nextIter;

    Node * prevPtr;

    Play const * prevPlayPtr;

    unsigned nextEntryNumber;

    unsigned chunkSize;


    void makeRanges();

    void removeRanges();


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

    list<Node>::const_iterator begin() const;
    list<Node>::const_iterator end() const;

    void strategizeDeclarer(const bool debugFlag);
    void strategizeDeclarerAdvanced(const bool debugFlag);

    void strategizeDefenders(const bool debugFlag);
    void strategizeDefendersAdvanced(const bool debugFlag);

    unsigned size() const;
    unsigned used() const;

    string strCount() const;

    string strSimple() const;
};

#endif
