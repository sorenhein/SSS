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

    vector<unsigned char> tricks;

    // These two are useful pre-calculations.
    unsigned lastForward;

    unsigned reverseSum;

    // TODO Parallel implementation for now.
    // The tricks are grouped into 5-element groups of 2 bits each.
    vector<unsigned> signature;


    const unsigned char sigElem(const unsigned extIndex) const;

    const unsigned char& element(const unsigned extIndex) const;

    unsigned char& element(const unsigned extIndex);

  
  public:

    void clear();

    void resize(const unsigned len);

    void set(
      const list<Result>& results,
      unsigned char& tricksMin);

    void weigh(
      const vector<unsigned char>& cases,
      unsigned& weight) const;

    bool prepare(
      const Product& product,
      const bool symmFlag,
      const vector<Profile>& distProfiles,
      const vector<unsigned char>& cases,
      unsigned& weight);

    // Will invalidate Tricks if not symmetrizable!
    bool symmetrize(
      const vector<unsigned char>& cases,
      unsigned& weight);

    bool possible(
      const Tricks& explained,
      const Tricks& residuals,
      const vector<unsigned char>& cases,
      Tricks& additions,
      unsigned& weightAdded) const;

    Tricks& operator += (const Tricks& tricks2);

    Tricks& operator -= (const Tricks& tricks2);

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
