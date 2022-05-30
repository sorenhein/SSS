/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_SYMMETRYSTORE_H
#define SSS_SYMMETRYSTORE_H

using namespace std;

#include <vector>
#include <set>


class SymmetryStore
{
  private:

    vector<set<unsigned>> store;


  public:

    SymmetryStore();

    void reset();

    void resize(const unsigned cards);

    void setManual();

    void readFile(const string& filename);

    void writeFile(const string& filename) const;

    bool symmetrize(
      const unsigned cards, 
      const unsigned holding3) const;
};

#endif
