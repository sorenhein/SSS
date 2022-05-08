/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_DEPTHSTATS_H
#define SSS_DEPTHSTATS_H

#include <vector>
#include <string>

using namespace std;


class DepthStats
{
  private:

    vector<vector<unsigned>> stats;

    
    void makeMarginals(
      vector<unsigned>& sumNominal,
      vector<unsigned>& sumActual,
      unsigned& maxNominal,
      unsigned& maxActual) const;

    string strHeader(const unsigned maxActual) const;


  public:

    DepthStats();
     
    void resize(const unsigned len);

    void store(
      const unsigned nominalDepth,
      const unsigned actualDepth);

    void increment(
      const unsigned nominalDepth,
      const unsigned actualDepth);

    DepthStats& operator += (const DepthStats& ds2);

    unsigned size() const;

    string str() const;
};

#endif
