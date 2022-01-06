/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COMBENTRY_H
#define SSS_COMBENTRY_H

#include <list>

#include "CombReference.h"

#include "../utils/CombinationType.h"

using namespace std;


// CombEntry keeps track of a solved combination.


class CombEntry
{
  private:

    // Bit layout:
    // 7  : Unused
    // 6  : minimalFlag
    // 5  : canonicalFlag
    // 4  : referenceFlag
    // 0-3: combination type
    unsigned char bitvector;

    CombReference reference;
    unsigned refHolding2;
    unsigned refIndex;
  
    // A combination is minimal if it references itself (and maybe others)
    list<CombReference> minimals;


  public:

    CombEntry();

    void reset();

    void setReference(
      const unsigned holding3In,
      const unsigned holding2In,
      const bool rotateFlagIn);

    void addMinimal(
      const unsigned holding3In,
      const bool rotateFlagIn);

    void addMinimalSelf();

    list<CombReference>::iterator begin() 
      { return minimals.begin(); };
    list<CombReference>::iterator end() 
      { return minimals.end(); };
    list<CombReference>::const_iterator begin() const
      { return minimals.begin(); };
    list<CombReference>::const_iterator end() const
      { return minimals.end(); };

    bool operator == (const CombEntry& ce2);

    void consolidateMinimals();

    bool fixMinimals(const vector<CombEntry>& centries);

    void setType(const CombinationType typeIn);
    CombinationType getType() const;

    unsigned getHolding3() const;
    unsigned getHolding2() const;

    void setIndex(const unsigned indexIn);
    unsigned getIndex() const;

    void setReference(const bool referenceFlagIn = true);
    bool isReference() const;

    void setCanonical();
    bool isCanonical() const;

    void setMinimal();
    bool isMinimal() const;
    bool minimalsEmpty() const;

    unsigned char packFlags() const;
    void unpackFlags(const unsigned char data);

    void packSelf(
      vector<unsigned>& vstream,
      unsigned& pos) const;

    void unpackSelf(
      const vector<unsigned>& vstream,
      unsigned& pos);

    void packMinimals(
      vector<unsigned>& vstream,
      unsigned& pos) const;

    void unpackMinimals(
      const vector<unsigned>& vstream,
      unsigned& pos);

    string strHolding() const;

    string strMinimals() const;

    string str() const;
};

#endif
