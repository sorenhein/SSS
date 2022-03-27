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

#include "CoverHelp.h"

using namespace std;

class Product;
class Profile;
class Cover;
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

    void prepare(
      const Product& product,
      const vector<Profile>& distProfiles,
      const vector<unsigned char>& cases,
      unsigned& weight,
      unsigned char& numDist);

    // TODO Shouldn't need CoverSpec longer-term
    void prepare(
      // const CoverSpec& spec,
      const Cover& cover,
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

    bool operator <= (const Tricks& tricks2) const;

    unsigned size() const;

    string strList() const;

    string strShort() const;

    string strSpaced() const;
};

#endif
