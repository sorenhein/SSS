#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Tvector.h"

// TODO Dominance can probably be implemented more efficiently.
// For example the "weight" (sum of all trick counts) indicates
// whether one vector can dominate another or not.  This can be
// extended to halves or thirds etc. of the vectors.  So if we
// can track these statistics efficiently, we can do a comparison
// of these statistics before doing the full loop.


Tvector::Tvector()
{
  Tvector::reset();
}


Tvector::~Tvector()
{
}


void Tvector::reset()
{
  results.clear();
}


void Tvector::log(
  const vector<unsigned>& distributions,
  const vector<unsigned>& tricks)
{
  assert(distributions.size() == tricks.size());

  weightInt = 0;
  for (unsigned i = 0; i < distributions.size(); i++)
  {
    results.emplace_back(TrickEntry());
    TrickEntry& te = results.back();
    te.dist = distributions[i];
    te.tricks = tricks[i];
    weightInt += tricks[i];
  }
}


bool Tvector::operator >= (const Tvector& tv2) const
{
  const unsigned n = results.size();
  assert(tv2.results.size() == n);

  list<TrickEntry>::const_iterator iter1 = results.cbegin();
  list<TrickEntry>::const_iterator iter2 = tv2.results.cbegin();

  while (iter1 != results.end())
  {
    if (* iter1 < * iter2)
      return false;

    iter1++;
    iter2++;
  }
  return true;
}


bool Tvector::operator > (const Tvector& tv2) const
{
  const unsigned n = results.size();
  assert(tv2.results.size() == n);

  list<TrickEntry>::const_iterator iter1 = results.cbegin();
  list<TrickEntry>::const_iterator iter2 = tv2.results.cbegin();

  bool greaterFlag = false;
  while (iter1 != results.end())
  {
    if (* iter1 < * iter2)
      return false;
    else if (* iter1 > * iter2)
      greaterFlag = true;

    iter1++;
    iter2++;
  }
  return greaterFlag;
}


Compare Tvector::compare(const Tvector& tv2) const
{
  // Returns COMPARE_LESS_THAN if *this < tv2.

  const unsigned n = results.size();
  assert(tv2.results.size() == n);

  list<TrickEntry>::const_iterator iter1 = results.cbegin();
  list<TrickEntry>::const_iterator iter2 = tv2.results.cbegin();

  bool possibleLT = true;
  bool possibleGT = true;

  while (iter1 != results.end())
  {
    if (* iter1 > * iter2)
      possibleLT = false;
    else if (* iter1 < * iter2)
      possibleGT = false;
    
    if (! possibleLT && ! possibleGT)
      return COMPARE_INCOMMENSURATE;

    iter1++;
    iter2++;
  }

  if (possibleLT)
    return (possibleGT ? COMPARE_EQUAL : COMPARE_LESS_THAN);
  else
    return COMPARE_GREATER_THAN;
}


void Tvector::operator *=(const Tvector& tv2)
{
  // Here we don't have to have the same length or distributions.
  
  auto iter1 = results.begin();
  auto iter2 = tv2.results.begin();

  while (iter2 != tv2.results.end())
  {
    if (iter1 == results.end() || iter1->dist > iter2->dist)
    {
      results.insert(iter1, * iter2);
      weightInt += iter2->tricks;
      iter2++;
    }
    else if (iter1->dist < iter2->dist)
    {
      iter1++;
    }
    else
    {
      if (iter1->tricks > iter2->tricks)
      {
        weightInt += iter2->tricks - iter1->tricks;
        iter1->tricks = iter2->tricks;
      }
      iter1++;
      iter2++;
    }
  }
}


unsigned Tvector::weight() const
{
  return weightInt;
}


string Tvector::str(const string& title) const
{
  stringstream ss;
  if (title != "")
    ss << title << "\n";
  
  ss << 
    setw(4) << left << "Dist" <<
    setw(6) << "Tricks" << "\n";

  for (const auto& te: results)
    ss <<
      setw(4) << te.dist <<
      setw(6) << te.tricks << "\n";

  return ss.str();
}

