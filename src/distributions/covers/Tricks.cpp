/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <limits>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Tricks.h"

#include "product/Product.h"
#include "product/Profile.h"

#include "../../strategies/result/Result.h"


void Tricks::clear()
{
  tricks.clear();
}


void Tricks::resize(const unsigned len)
{
  tricks.resize(len);

  // See table below.
  lastForward = (len-1) / 2;
  reverseSum = len + len - (len/2) - 1;

/*
cout << "LEN " << len << endl;
cout << "lastF " << lastForward << ", reverseSum " << reverseSum << endl;
for (unsigned ext = 0; ext < len; ext++)
  cout << ext << " " << Tricks::trickIndex(ext) << endl;
cout << "\n";
*/
}


/*
unsigned Tricks::trickIndex(const unsigned extIndex) const
{
  if (extIndex <= lastForward)
    return extIndex;
  else
    return reverseSum - extIndex;
}
*/


const unsigned char& Tricks::element(const unsigned extIndex) const
{
  /*
     Store the front half (including perhaps the middle) one way
     Store the back half backward               

     N   lastF Forward (grouped)  Backward (grouped)  revSum Cost 
      3: 1     0-1                2                   4      1
      4: 1     0-1                3-2                 5      1
      5: 2     0-2                4-3                 7      1
      6: 2     0-2                5-3                 8      0
      7: 3     0-3                6-4                 10     0
      8: 3     0-3                7-4                 11     0
      9: 4     0-4                8-5                 13     0
     10: 4     0-4                9-5                 14     0
     11: 5     0-4   5            10-6                16     0
     12: 5     0-4   5            11-7    6           17     1
     13: 6     0-4   5-6          12-8    7           19     1
     14: 6     0-4   5-6          13-9    8-7         20     1
     15: 7     0-4   5-7          14-10   9-8         22     1
     16: 7     0-4   5-7          15-11   10-8        23     0
     17: 8     0-4   5-8          16-12   11-9        25     0
     18: 8     0-4   5-8          17-13   12-9        26     0
     19: 9     0-4   5-9          18-14   13-10       28     0

     So the average cost is 0.4 extra entries.
  */
  
  if (extIndex <= lastForward)
    return tricks[extIndex];
  else
    return tricks[reverseSum - extIndex];
}


unsigned char& Tricks::element(const unsigned extIndex)
{
  if (extIndex <= lastForward)
    return tricks[extIndex];
  else
    return tricks[reverseSum - extIndex];
}


void Tricks::set(
  const list<Result>& results,
  unsigned char& tricksMin)
{
  // In order to deal with symmetry, we store the first half
  // (including any middle element) forward and the second half
  // in reverse.

  Tricks::resize(results.size());
  tricksMin = numeric_limits<unsigned char>::max();
  unsigned extIndex = 0;

  for (auto& res: results)
  {
    const unsigned char rt = res.getTricks();
    // const unsigned intIndex = Tricks::trickIndex(extIndex);

    Tricks::element(extIndex) = rt;
    if (rt < tricksMin)
      tricksMin = rt;

    extIndex++;
  }

  for (unsigned i = 0; i < tricks.size(); i++)
    tricks[i] -= tricksMin;
}


void Tricks::weigh(
  const vector<unsigned char>& cases,
  unsigned& weight) const
{
  assert(cases.size() == tricks.size());

  weight = 0;
  for (unsigned extIndex = 0; extIndex < tricks.size(); extIndex++)
    weight += cases[extIndex] * Tricks::element(extIndex);
}


void Tricks::prepare(
  const Product& product,
  const bool symmFlag,
  const vector<Profile>& distProfiles,
  const vector<unsigned char>& cases,
  unsigned& weight,
  unsigned char& numDist)
{
  const unsigned len = distProfiles.size();
  assert(len == cases.size());
  Tricks::resize(len);

  weight = 0;
  numDist = 0;

// cout << "Enter prepare\n";
  if (symmFlag)
  {
    for (unsigned extIndex = 0; extIndex < len; extIndex++)
    {
      if (product.includes(distProfiles[extIndex]) ||
          product.includes(distProfiles[len-1-extIndex]))
      {
        // const unsigned intIndex = Tricks::trickIndex(extIndex);
        // tricks[intIndex] = 1;
        Tricks::element(extIndex) = 1;
        weight += static_cast<unsigned>(cases[extIndex]);
        numDist++;
      }
    }
  }
  else
  {
    for (unsigned extIndex = 0; extIndex < len; extIndex++)
    {
      if (product.includes(distProfiles[extIndex]))
      {
        // const unsigned intIndex = Tricks::trickIndex(extIndex);
        // tricks[intIndex] = 1;
        Tricks::element(extIndex) = 1;
        weight += static_cast<unsigned>(cases[extIndex]);
        numDist++;
// cout << "Setting elem " << extIndex << ", weight " << +weight << endl;
      }
    }
  }

if (weight == 0)
{
  cout << "symmFlag " << (symmFlag ? "yes" : "no") << endl;
  for (unsigned extIndex = 0; extIndex < len; extIndex++)
    cout << extIndex << " " << +tricks[extIndex] << 
      "  " << +cases[extIndex] << endl;
  assert(false);
}
}


bool Tricks::symmetrize(
  const vector<unsigned char>& cases,
  unsigned& weight,
  unsigned char& numDist)
{
  // Will invalidate Tricks if not symmetrizable!
  // We only symmetrize if there is no overlap with the mirror.
  // We cannot be sure to double weight and numDist, as the middle
  // element in an odd-length tricks will not be repeated.

  // Loop over the high end of the internal trick numbers.
  unsigned lo, hi;
  for (lo = 0, hi = lastForward+1; hi < tricks.size(); lo++, hi++)
  {
    if (tricks[lo] == tricks[hi])
    {
      // Don't symmetrize if there is overlap.
      if (tricks[lo])
        return false;
    }
    else
    {
      // Overwrite one of them, but no need to test which one.
      tricks[lo] = 1;
      tricks[hi] = 1;
      numDist++;
    }
  }

  Tricks::weigh(cases, weight);
  return true;

  /*
  const unsigned len = tricks.size();

  for (unsigned dno = 0; dno < len/2; dno++)
  {
    const unsigned mno = len - 1 - dno;
    if (tricks[dno] == 0)
    {
      if (tricks[mno] > 0)
      {
        tricks[dno] = tricks[mno];
        weight += cases[mno] * tricks[mno];
        numDist++;
      }
    }
    else if (tricks[mno] == 0)
    {
      tricks[mno] = tricks[dno];
      weight += cases[dno] * tricks[dno];
      numDist++;
    }
    else
      return false;
  }

  return true;
  */
}


bool Tricks::possible(
  const Tricks& explained,
  const Tricks& residuals,
  const vector<unsigned char>& cases,
  Tricks& additions,
  unsigned& weightAdded) const
{
  assert(tricks.size() == explained.size());
  assert(tricks.size() == residuals.size());
  assert(tricks.size() == cases.size());
  assert(tricks.size() == additions.size());

  // The cover is an addition if it is not already set.
  // This can be written a bit more efficiently, but the idea is to
  // make it vectorizable if we go down that path.
  for (unsigned intIndex = 0; intIndex < tricks.size(); intIndex++)
    additions.tricks[intIndex] = 
      tricks[intIndex] & ~explained.tricks[intIndex];

  if (additions <= residuals)
  {
    additions.weigh(cases, weightAdded);
    return (weightAdded > 0);
  }
  else
    return false;
}


Tricks& Tricks::operator += (const Tricks& tricks2)
{
  // No checking that we don't go out of the positive range (0..3).
  assert(tricks.size() == tricks2.tricks.size());

  for (unsigned i = 0; i < tricks.size(); i++)
    tricks[i] += tricks2.tricks[i];
  
  return * this;
}


Tricks& Tricks::operator -= (const Tricks& tricks2)
{
  // No checking of <= first.
  assert(tricks.size() == tricks2.tricks.size());

  for (unsigned i = 0; i < tricks.size(); i++)
    tricks[i] -= tricks2.tricks[i];
  
  return * this;
}


Tricks& Tricks::operator |= (const Tricks& tricks2)
{
  assert(tricks.size() == tricks2.tricks.size());

  for (unsigned i = 0; i < tricks.size(); i++)
    tricks[i] |= tricks2.tricks[i];
  
  return * this;
}


void Tricks::orSymm(const Tricks& tricks2)
{
  const unsigned len = tricks.size();
  assert(len == tricks2.tricks.size());

  /*
  for (unsigned i = 0; i < len; i++)
    tricks[i] |= tricks2.tricks[i] | tricks2.tricks[len-1-i];
    */

  unsigned lo, hi;
  for (lo = 0, hi = lastForward+1; hi < tricks.size(); lo++, hi++)
  {
    const unsigned char orVal = tricks2.tricks[lo] | tricks2.tricks[hi];
    tricks[lo] |= orVal;
    tricks[hi] |= orVal;
  }

  if (lo == lastForward)
  {
    // There is a middle element.
    tricks[lo] |= tricks2.tricks[lo];
  }
}


bool Tricks::operator == (const Tricks& tricks2) const
{
  assert(tricks.size() == tricks2.tricks.size());

  for (unsigned i = 0; i < tricks.size(); i++)
    if (tricks[i] != tricks2.tricks[i])
      return false;
  
  return true;
}


bool Tricks::operator <= (const Tricks& tricks2) const
{
  assert(tricks.size() == tricks2.tricks.size());

  for (unsigned i = 0; i < tricks.size(); i++)
    if (tricks[i] > tricks2.tricks[i])
      return false;
  
  return true;
}


unsigned Tricks::size() const
{
  return tricks.size();
}


string Tricks::strList() const
{
  stringstream ss;

  for (unsigned extIndex = 0; extIndex < tricks.size(); extIndex++)
    ss << 
      setw(2) << extIndex << 
      setw(4) << +Tricks::element(extIndex) << "\n";
      // setw(4) << +tricks[Tricks::trickIndex(extIndex)] << "\n";

  return ss.str();
}


string Tricks::strShort() const
{
  string s;
  for (unsigned extIndex = 0; extIndex < tricks.size(); extIndex++)
    s += (Tricks::element(extIndex) ? "1" : "-");
    // s += (tricks[Tricks::trickIndex(extIndex)] ? "1" : "-");
  return s + "  ";
}


string Tricks::strSpaced() const
{
  stringstream ss;

  for (unsigned extIndex = 0; extIndex < tricks.size(); extIndex++)
    ss << setw(2) << +Tricks::element(extIndex);
    // ss << setw(2) << +tricks[Tricks::trickIndex(extIndex)];

  return ss.str() + "\n";
}

