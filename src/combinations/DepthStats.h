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
     
    void resize(const size_t len);

    void increment(
      const size_t nominalDepth,
      const size_t actualDepth);

    DepthStats& operator += (const DepthStats& ds2);

    size_t size() const;

    string str() const;
};

#endif
