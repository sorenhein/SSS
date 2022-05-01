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

    unsigned weight;

    unsigned length;

    // These two are useful pre-calculations.  Might be uchar.
    unsigned lastForward;

    unsigned reverseSum;

    // TODO Parallel implementation for now.
    // The tricks are grouped into 5-element groups of 2 bits each.
    vector<unsigned> signature;


    const unsigned char sigElem(const unsigned extIndex) const;

    unsigned char& element(
      vector<unsigned char>& tricks,
      const unsigned extIndex);

    void weigh(const vector<unsigned char>& cases);

  
  public:

    void clear();

    void resize(const unsigned len);

    void set(
      const list<Result>& results,
      const vector<unsigned char>& cases,
      unsigned char& tricksMin);

    bool prepare(
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

    unsigned getWeight() const;

    Tricks& operator += (const Tricks& tricks2);

    Tricks& operator -= (const Tricks& tricks2);

    Tricks& orNormal(
      const Tricks& tricks2,
      const vector<unsigned char>& cases);

    void orSymm(
      const Tricks& tricks2,
      const vector<unsigned char>& cases);

    bool operator == (const Tricks& tricks2) const;

    bool operator <= (const Tricks& tricks2) const;

    unsigned size() const;

    string strList() const;

    string strShort() const;

    string strSpaced() const;
};

#endif
