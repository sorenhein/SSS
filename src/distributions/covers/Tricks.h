/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_TRICKS_H
#define SSS_TRICKS_H

#include <vector>
#include <list>
#include <string>

#include "CoverState.h"

using namespace std;

class Product;
class Profile;
class Result;


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

    void set(
      const Tricks& tricks2,
      unsigned char& sum);

    void set(
      const list<Result>& results,
      unsigned char& tricksMin);

    void set(const unsigned no);

    void weigh(
      const vector<unsigned char>& cases,
      unsigned& weight,
      unsigned char& numDist) const;

    void prepare(
      const Product& product,
      const bool symmFlag,
      const vector<Profile>& distProfiles,
      const vector<unsigned char>& cases,
      unsigned& weight,
      unsigned char& numDist);

    // Will invalidate Tricks if not symmetrizable!
    bool symmetrize(
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

    void subtract(
      Tricks& residuals,
      unsigned char& residualsSum) const;

    Tricks& operator |= (const Tricks& tricks2);

    void orSymm(const Tricks& tricks2);

    bool operator == (const Tricks& tricks2) const;

    bool operator <= (const Tricks& tricks2) const;

    unsigned size() const;

    string strList() const;

    string strShort() const;

    string strSpaced() const;
};

#endif
