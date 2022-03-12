/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "CoverTableau.h"

#include "../../strategies/result/Result.h"


CoverTableau::CoverTableau()
{
  CoverTableau::reset();
}


void CoverTableau::reset()
{
  rows.clear();
  residuals.clear();
  residualsSum = 0;
  tricksMin = 0;
}


void CoverTableau::setBoundaries(
  const unsigned char maxLengthIn,
  const vector<unsigned char>& topTotalsIn)
{
  maxLength = maxLengthIn;
  topTotalsPtr = &topTotalsIn;
}


void CoverTableau::setTricks(
  const vector<unsigned char>& tricks,
  const unsigned char tmin)
{
  residuals = tricks;
  residualsSum = 0;

  for (auto t: tricks)
    residualsSum += t;

  tricksMin = tmin;
}


bool CoverTableau::attemptGreedy(const CoverNew& cover)
{
  // explained is a dummy vector here.
  vector<unsigned char> explained(cover.size(), 0);
  vector<unsigned char> additions(cover.size());
  unsigned char tricksAdded;

  // First try to add a new row.
  if (cover.possible(explained, residuals, additions, tricksAdded))
  {
    rows.emplace_back(CoverRow());
    CoverRow& row = rows.back();
    row.resize(cover.size());
    row.add(cover, additions, residuals, residualsSum);
    return true;
  }

  // Then look for a row, or the best one, to add to.
  if (rows.empty())
    return false;

  CoverRow * rowBestPtr = nullptr;
  vector<unsigned char> additionsBest(cover.size());
  unsigned char weightBest = 0;

  for (auto& row: rows)
  {
    if (row.attempt(cover, residuals, additions, tricksAdded))
    {
      if (tricksAdded > weightBest)
      {
        rowBestPtr = &row;
        swap(additions, additionsBest);
        weightBest = tricksAdded;
      }
    }
  }

  if (rowBestPtr == nullptr)
    return false;

  // A 1-trick cover always exists separately as well.
  // TODO Actually we should only add deltas that are not stand-alone
  // covers.
  if  (weightBest == 1)
    return false;

/*
cout << "Row before adding:\n";
cout << rowBestPtr->strLines() << "\n";
*/

  rowBestPtr->add(cover, additionsBest, residuals, residualsSum);

/*
cout << "Row before adding:\n";
cout << rowBestPtr->strLines() << "\n";
*/

  return true;
}


void CoverTableau::attemptExhaustive(
  list<CoverNew>::const_iterator& coverIter,
  const unsigned coverNo,
  list<StackTableau>& stack,
  list<CoverTableau>& solutions,
  unsigned char& lowestComplexity) const
{
  // This is similar to attemptGreedy, but we don't stop once we have
  // the first match, and we don't yet implement any match.

  // explained is a dummy vector here.
  const CoverNew& cover = * coverIter;
  vector<unsigned char> explained(cover.size(), 0);
  vector<unsigned char> additions(cover.size());
  unsigned char tricksAdded;
  const bool emptyStartFlag = rows.empty();

  const unsigned char complexity = CoverTableau::getComplexity();
  // Keep some extras to inspect for now.
  // TODO This doesn't actually limit the final list to this range,
  // as the complexity may shrink over time.

  /* */
  if (complexity + cover.getComplexity() > lowestComplexity + 2)
  {
    // Too complex.
    return;
  }
  /* */

  // First try to add a new row.
  if (cover.possible(explained, residuals, additions, tricksAdded))
  {
// cout << "Can add to a new row" << endl;
    stack.emplace_back(StackTableau());
    StackTableau& stableau = stack.back();

    CoverTableau& tableau = stableau.tableau;
    tableau = * this;
// cout << "Set up and copied the tableau" <<endl;
    tableau.rows.emplace_back(CoverRow());
    CoverRow& row = tableau.rows.back();
    row.resize(cover.size());
// cout << "Got the row" <<endl;
    row.add(cover, additions, tableau.residuals, tableau.residualsSum);
// cout << "Added" << endl;

    stableau.coverIter = coverIter;
    stableau.coverNumber = coverNo;

// cout << "Tableau now\n";
// cout << tableau.str();
// cout << tableau.strResiduals();

    if (tableau.complete())
    {
// cout << "Got a solution" << endl;
      solutions.push_back(tableau);
      // Done, so eliminate.
      stack.pop_back();

      if (complexity + cover.getComplexity() < lowestComplexity)
        lowestComplexity = complexity + cover.getComplexity();
    }
  }

  // Then look for a row, or the best one, to add to.
  if (emptyStartFlag)
    return;

  unsigned rno = 0, r;
  list<CoverRow>::iterator riter;

  for (auto& row: rows)
  {
    if (row.attempt(cover, residuals, additions, tricksAdded) &&
        tricksAdded < cover.getNumDist())
    {
      // Don't want cover to be completely complementery (use new row).

// cout <<"Can add to an existing row" << endl;
      stack.emplace_back(StackTableau());
      StackTableau& stableau = stack.back();

      CoverTableau& tableau = stableau.tableau;
      tableau = * this;

      // A bit fumbly: Advance to the same place in tableau.
      for (riter = tableau.rows.begin(), r = 0; r < rno; riter++, r++);

      riter->add(cover, additions, tableau.residuals, tableau.residualsSum);

// cout << "Tableau now" << endl;
// cout << tableau.str();
// cout << tableau.strResiduals();

      stableau.coverIter = coverIter;
      stableau.coverNumber = coverNo;

      if (tableau.complete())
      {
// cout << CoverTableau::str() << endl;
 // cout << "Before calling" << endl;
// cout << tableau.str() << endl;
// cout << "Calling getComplexity" << endl;
        const unsigned char c = tableau.getComplexity();
// cout << "got " << +c << endl;

        if (c < lowestComplexity)
          lowestComplexity = c;
 // cout << "Got a solution by augmenting a row" << endl;
        solutions.push_back(tableau);
        // Done, so eliminate.
        stack.pop_back();

      }
    }
    rno++;
  }
}


bool CoverTableau::operator < (const CoverTableau& tableau2) const
{
  const unsigned char compl1 = CoverTableau::getComplexity();
  const unsigned char compl2 = tableau2.getComplexity();

  return (compl1 < compl2 ||
    (compl1 == compl2 &&
      CoverTableau::getOverlap() < tableau2.getOverlap()));
}


bool CoverTableau::complete() const
{
  return (! rows.empty() && residualsSum == 0);
}


unsigned char CoverTableau::getComplexity() const
{
// assert(topTotalsPtr != nullptr);
// cout << "entering getC" << endl;
// cout << CoverTableau::str() << endl;
  unsigned char complexity = 0;
  for (auto& row: rows)
  {
    // TODO TEMP
    assert(complexity + row.getComplexity() > complexity);

    complexity += row.getComplexity();
  }
// cout << "got complexity " << +complexity << endl;
  return complexity;
}


unsigned char CoverTableau::getOverlap() const
{
  unsigned char overlap = 0;
  for (auto& row: rows)
  {
    const unsigned char rowOverlap = row.getOverlap();
// cout << "rOverlap " << +rowOverlap << endl;
    // TODO TEMP
    assert(overlap + rowOverlap >= overlap);

    overlap += rowOverlap;
  }
  return overlap;
}


unsigned char CoverTableau::numRows() const
{
  return static_cast<unsigned char>(rows.size());
}


unsigned char CoverTableau::numCovers() const
{
  unsigned char num = 0;
  for (auto& row: rows)
  {
    // TODO TEMP
    assert(num + row.size() > num);

    num += static_cast<unsigned char>(row.size());
  }
  return num;
}


unsigned char CoverTableau::getResidual() const
{
  return residualsSum;
}


string CoverTableau::str() const
{
  stringstream ss;

  ss << "Always take at least " << +tricksMin << " tricks, and more when\n";
  for (auto& row: rows)
    ss << row.str(maxLength, * topTotalsPtr);
  /*
  {
    ss << row.strHeader();
    ss << row.strLines() << "\n";
  }
  */

  return ss.str() + "\n";
}


string CoverTableau::strResiduals() const
{
  stringstream ss;

  for (unsigned i = 0; i < residuals.size(); i++)
    ss << setw(2) << i << setw(4) << +residuals[i] << "\n";

  return ss.str();
}

