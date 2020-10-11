#ifndef SSS_PLAYS_H
#define SSS_PLAYS_H

#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>

#include "const.h"

using namespace std;


class Plays
{
  private:

    struct PlayEntry
    {
      SidePosition side;
      unsigned lead;
      unsigned lho;
      unsigned pard;
      unsigned rho;
      unsigned trickNS;

      bool knownVoidWest;
      bool knownVoidEast;

      unsigned holdingNew;
      bool rotateNew;

      bool leadCollapse; // Does the lead rank go away after this trick?
      bool lhoCollapse;
      bool pardCollapse;
      bool rhoCollapse;

      void update(
        const SidePosition sideIn,
        const unsigned leadIn,
        const unsigned lhoIn,
        const unsigned pardIn,
        const unsigned rhoIn,
        const bool leadCollapseIn,
        const bool lhoCollapseIn,
        const bool pardCollapseIn,
        const bool rhoCollapseIn,
        const unsigned holding3In,
        const bool rotateFlagIn)
      {
        side = sideIn;
        lead = leadIn;
        lho = lhoIn;
        pard = pardIn;
        rho = rhoIn;
        leadCollapse = leadCollapseIn;
        lhoCollapse = lhoCollapseIn;
        pardCollapse = pardCollapseIn;
        rhoCollapse = rhoCollapseIn;
        holdingNew = holding3In;
        rotateNew = rotateFlagIn;

        trickNS = (max(lead, pard) > max(lho, rho) ? 1 : 0);
        if (side == SIDE_NORTH)
        {
          knownVoidWest = (rho == 0);
          knownVoidEast = (lho == 0);
        }
        else
        {
          knownVoidWest = (lho == 0);
          knownVoidEast = (rho == 0);
        }
      }

      string strHeader() const
      {
        stringstream ss;
        ss << right << 
          setw(4) << "Side" <<
          setw(5) << "Lead" <<
          setw(5) << "LHO" <<
          setw(5) << "Pard" <<
          setw(5) << "RHO" <<
          setw(5) << "Win?" <<
          setw(5) << "W vd" <<
          setw(5) << "E vd" <<
          setw(10) << "Holding" <<
          endl;
        return ss.str();
      }

      string str() const
      {
        stringstream ss;
        ss << right << 
          setw(4) << (side == SIDE_NORTH ? "N" : "S") <<
          setw(5) << lead <<
          setw(5) << (lho == 0 ? "-" : to_string(lho)) <<
          setw(5) << (pard == 0 ? "-" : to_string(pard)) <<
          setw(5) << (rho == 0 ? "-" : to_string(rho)) <<
          setw(5) << (trickNS == 1 ? "+" : "") <<
          setw(5) << (knownVoidWest ? "yes" : "") <<
          setw(5) << (knownVoidEast ? "yes" : "") <<
          setw(10) << holdingNew <<
          endl;
        return ss.str();
      }
    };

    unsigned chunk;
    vector<PlayEntry> playRecord;
    unsigned nextNo;

  public:
    
    Plays();

    ~Plays();

    void reset();

    void resize(const unsigned cards);

    unsigned size() const;

    void log(
      const SidePosition sideIn,
      const unsigned leadIn,
      const unsigned lhoIn,
      const unsigned pardIn,
      const unsigned rhoIn,
      const bool leadCollapseIn,
      const bool lhoCollapseIn,
      const bool pardCollapseIn,
      const bool rhoCollapseIn,
      const unsigned holding3In,
      const bool rotateFlagIn);

};

#endif
