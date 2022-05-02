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

using namespace std;

class Product;
class Profile;
class Result;


class Tricks
{
  private:

    unsigned length;

    unsigned weight;

    // The tricks are grouped as shown in ResConvert.  This makes
    // comparisons etc. faster, but costs some time to set up.
    vector<unsigned> signature;

    // Useful pre-calculation.
    unsigned lastForward;


    const unsigned char lookup(const unsigned extIndex) const;

    void weigh(const vector<unsigned char>& cases);

    unsigned char productValue(
      const Product& product,
      const bool symmFlag,
      const vector<Profile>& distProfiles,
      const unsigned extIndex) const;

  
  public:

    void clear();

    void resize(const unsigned len);

    void setByResults(
      const list<Result>& results,
      const vector<unsigned char>& cases,
      unsigned char& tricksMin);

    bool setByProduct(
      const Product& product,
      const bool symmFlag,
      const vector<Profile>& distProfiles,
      const vector<unsigned char>& cases);

    // Will invalidate Tricks if not symmetrizable!
    bool symmetrize();

    bool possible(
      const Tricks& explained,
      const Tricks& residuals,
      const vector<unsigned char>& cases,
      Tricks& additions) const;

    Tricks& orNormal(
      const Tricks& tricks2,
      const vector<unsigned char>& cases);

    void orSymm(
      const Tricks& tricks2,
      const vector<unsigned char>& cases);

    Tricks& operator += (const Tricks& tricks2);

    Tricks& operator -= (const Tricks& tricks2);

    bool operator == (const Tricks& tricks2) const;

    bool operator <= (const Tricks& tricks2) const;

    unsigned size() const;

    unsigned getWeight() const;

    string strList() const;

    string strShort() const;

    string strSpaced() const;
};

#endif
