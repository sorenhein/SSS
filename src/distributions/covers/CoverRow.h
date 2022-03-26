/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COVERROW_H
#define SSS_COVERROW_H

#include <list>
#include <vector>
#include <string>

#include "CoverNew.h"
#include "Tricks.h"

using namespace std;

/* A row is a list of covers that are OR'ed together.
 */


class CoverRow
{
  private:

    list<CoverNew const *> coverPtrs;

    // The OR'ed tricks of the covers.
    // vector<unsigned char> tricks;
    Tricks tricks;

    unsigned char numDist;

    unsigned char complexity;


  public:

    CoverRow();

    void reset();

    void resize(const unsigned len);

    bool attempt(
      const CoverNew& cover,
      Tricks& additions,
      const Tricks& residuals,
      // const vector<unsigned char>& residuals,
      // vector<unsigned char>& additions,
      unsigned char& tricksAdded) const;

    void add(
      const CoverNew& cover,
      // const vector<unsigned char>& additions,
      const Tricks& additions,
      // vector<unsigned char>& residuals,
      Tricks& residuals,
      unsigned char& residualsSum);

    unsigned size() const;

    const Tricks& getTricks() const;

    unsigned char getWeight() const;

    unsigned char getComplexity() const;

    unsigned char getOverlap() const;

    // These together yield something like a table,
    // but without semantic explanations.
    string strHeader() const;

    string strLines(const Profile& sumProfile) const;

    string str(const Profile& sumProfile) const;
};

#endif
