/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COMBREFERENCE_H
#define SSS_COMBREFERENCE_H

#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;


class CombReference
{
  private:

    unsigned holding3;
    bool rotateFlag;
  

  public:

    void set(
      const unsigned holding3In,
      const bool rotateFlagIn)
    {
      holding3 = holding3In;
      rotateFlag = rotateFlagIn;
    }

    void rotateBy(const CombReference& cr)
    {
      rotateFlag ^= cr.rotateFlag;
    }

    void pack(
      vector<unsigned>& vstream,
      unsigned& pos) const
    {
      vstream[pos++] = ((rotateFlag ? 1 : 0) << 31) | holding3;
    }

    void unpack(
      const vector<unsigned>& vstream,
      unsigned& pos)
    {
      const unsigned u = vstream[pos++];
      rotateFlag = ((u >> 31) ? true : false);
      holding3 = (u & 0x7fff);
    }

    bool operator < (const CombReference& cr2) const
    {
      return (holding3 < cr2.holding3);
    }

    bool operator == (const CombReference& cr2) const
    {
      return (holding3 == cr2.holding3 && rotateFlag == cr2.rotateFlag);
    }

    bool operator != (const CombReference& cr2) const
    {
      return ! (* this == cr2);
    }

    unsigned getHolding3() const
    {
      return holding3;
    }

    bool getRotateFlag() const
    {
      return rotateFlag;
    }

    string strSimple() const
    {
      return to_string(holding3) + " " + 
        (rotateFlag ? "(rot)" : "(nonrot)");
    }

    string strTable() const
    {
      return to_string(holding3) + (rotateFlag ? "R" : "");
    }

    string strHolding() const
    {
      stringstream ss;
      ss << holding3 << " | " << "0x" << hex << holding3;
      return ss.str();
    }
};

#endif
