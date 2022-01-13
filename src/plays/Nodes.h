/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_NODES_H
#define SSS_NODES_H

#include <list>
#include <string>

#include "Node.h"

class Distribution;

using namespace std;


class Nodes
{
  // Nodes are a collection of Node's with some information about
  // the Node's are hooked up.  Nodes knows its level (LHO, RHO, ...).


  private:

    Level level;

    list<Node> nodes;

    // For efficiency we don't add a new node every time log() is called.
    // Instead we allocate chunks whenever we run out of space.
    // begin() and end() only loop over the used part of the list.
    // The following variables are needed to keep track of this.

    list<Node>::iterator nextIter;

    Node * prevPtr;

    Play const * prevPlayPtr;

    unsigned nextEntryNumber;

    unsigned chunkSize;


    void removeNodes();


  public:
    
    Nodes();

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

    void strategizeDeclarer(
      const Distribution& distribution,
      const bool debugFlag);

    void strategizeDeclarerAdvanced(
      const Distribution& distribution,
      const bool debugFlag);

    void strategizeDefenders(const bool debugFlag);
    void strategizeDefendersAdvanced(const bool debugFlag);

    unsigned size() const;
    unsigned used() const;

    string strCountHeader() const;
    string strCount() const;

    string strSimple() const;
};

#endif
