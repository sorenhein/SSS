/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_TRICKS_H
#define SSS_TRICKS_H

#include <vector>
#include <string>

#include "CoverHelp.h"

using namespace std;

class Product;
class Profile;


class Tricks
{
  private:

    vector<unsigned char> tricks;

  
  public:

    void clear();

    void resize(const unsigned len);

    void set(
      const vector<unsigned char>& values,
      unsigned char& sum);

    void prepare(
      const Product& product,
      const vector<Profile>& distProfiles,
      const vector<unsigned char>& cases,
      unsigned& weight,
      unsigned char& numDist);

    bool possible(
      const Tricks& explained,
      const Tricks& residuals,
      Tricks& additions,
      unsigned char& tricksAdded) const;

    CoverState explain(Tricks& tricks2) const;

    void add(
      const Tricks& additions,
      Tricks& residuals,
      unsigned char& residualsSum,
      unsigned char& numDist);

    bool operator == (const Tricks& tricks2) const;

    unsigned size() const;

    string strList() const;

    string strShort() const;

    string strSpaced() const;
};

#endif
