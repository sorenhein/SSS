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
#include "Cover.h"
#include "ResExpl.h"

#include "../../strategies/result/Result.h"


CoverTableau::CoverTableau()
{
  CoverTableau::reset();
}


void CoverTableau::reset()
{
  rows.clear();
  residuals.clear();
  residualWeight = 0;
  tricksMin = 0;
}


void CoverTableau::setBoundaries(const Profile& sumProfileIn)
{
  sumProfile = sumProfileIn;
}


void CoverTableau::setTricks(
  const Tricks& tricks,
  const unsigned char tmin,
  const vector<unsigned char>& cases)
{
  residuals.set(tricks, cases, residualWeight);

  tricksMin = tmin;
}


void CoverTableau::setMinTricks(const unsigned char tmin)
{
  tricksMin = tmin;
}


bool CoverTableau::attemptGreedy(
  const Cover& cover,
  const vector<unsigned char>& cases)
{
  // explained is a dummy vector here.
  Tricks explained;
  explained.resize(cover.size());

  Tricks additions;
  additions.resize(cover.size());

  unsigned char weightAdded;

  // First try to add a new row.
  if (cover.possible(explained, residuals, cases, additions, weightAdded))
  {
    rows.emplace_back(CoverRow());
    CoverRow& row = rows.back();
    row.resize(cover.size());
    row.add(cover, additions, cases, residuals, residualWeight);
    return true;
  }

  // Then look for a row, or the best one, to add to.
  if (rows.empty())
    return false;

  CoverRow * rowBestPtr = nullptr;

  Tricks additionsBest;
  additionsBest.resize(cover.size());

  unsigned char weightBest = 0;

  for (auto& row: rows)
  {
    if (row.attempt(cover, additions, cases, residuals, weightAdded))
    {
      if (weightAdded > weightBest)
      {
        rowBestPtr = &row;
        swap(additions, additionsBest);
        weightBest = weightAdded;
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

  rowBestPtr->add(cover, additionsBest, cases, residuals, residualWeight);

/*
cout << "Row before adding:\n";
cout << rowBestPtr->strLines() << "\n";
*/

  return true;
}


void CoverTableau::attemptExhaustiveRow(
  const vector<unsigned char>& cases,
  list<CoverRow>::const_iterator& rowIter,
  const unsigned coverNo,
  list<ResTableau>& stack,
  list<CoverTableau>& solutions,
  unsigned char& lowestComplexity) const
{
  // explained is a dummy vector here.
  const CoverRow& row = * rowIter;

  Tricks explained;
  explained.resize(residuals.size());

  Tricks additions;
  additions.resize(residuals.size());

  unsigned char weightAdded;

  const unsigned char complexity = CoverTableau::getComplexity();

  if (complexity + row.getComplexity() - 2 > lowestComplexity)
  {
    // Too complex.
    return;
  }

  // Try to add a new row.
  if (row.possible(explained, residuals, cases, additions, weightAdded))
  {
// cout << "Can add to a new row" << endl;
    stack.emplace_back(ResTableau());
    ResTableau& rtableau = stack.back();

    CoverTableau& tableau = rtableau.tableau;
    tableau = * this;
// cout << "Set up and copied the tableau" <<endl;
    tableau.rows.push_back(row);
// cout << "Got the row" <<endl;

    row.subtract(additions, cases,
      tableau.residuals, tableau.residualWeight);

// cout << "residual weight now " << +tableau.residualWeight << endl;
    rtableau.rowIter = rowIter;
    rtableau.rowNumber = coverNo;

    if (tableau.complete())
    {
// cout << "Got a solution" << endl;
      solutions.push_back(tableau);
      // Done, so eliminate.
      stack.pop_back();

      if (complexity + row.getComplexity() < lowestComplexity)
        lowestComplexity = complexity + row.getComplexity();
    }
  }
}


void CoverTableau::attemptExhaustive(
  const vector<unsigned char>& cases,
  set<Cover>::const_iterator& coverIter,
  const unsigned coverNo,
  list<StackTableau>& stack,
  list<CoverTableau>& solutions,
  unsigned char& lowestComplexity) const
{
  // This is similar to attemptGreedy, but we don't stop once we have
  // the first match, and we don't yet implement any match.

  // explained is a dummy vector here.
  const Cover& cover = * coverIter;

  Tricks explained;
  explained.resize(cover.size());

  Tricks additions;
  additions.resize(cover.size());

  unsigned char weightAdded;
  const bool emptyStartFlag = rows.empty();

  const unsigned char complexity = CoverTableau::getComplexity();
  // Keep some extras to inspect for now.
  // TODO This doesn't actually limit the final list to this range,
  // as the complexity may shrink over time.

  /* */
  if (complexity + cover.getComplexity() -2 > lowestComplexity)
  {
    // Too complex.
    return;
  }
  /* */

  // First try to add a new row.
  if (cover.possible(explained, residuals, cases, additions, weightAdded))
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
    row.add(cover, additions, cases, 
      tableau.residuals, tableau.residualWeight);
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
    if (row.attempt(cover, residuals, cases, additions, weightAdded) &&
        weightAdded < cover.getNumDist())
    {
      // Don't want cover to be completely complementery (use new row).

// cout <<"Can add to an existing row" << endl;
      stack.emplace_back(StackTableau());
      StackTableau& stableau = stack.back();

      CoverTableau& tableau = stableau.tableau;
      tableau = * this;

      // A bit fumbly: Advance to the same place in tableau.
      for (riter = tableau.rows.begin(), r = 0; r < rno; riter++, r++);

      riter->add(cover, additions, cases,
        tableau.residuals, tableau.residualWeight);

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


/*
void CoverTableau::toResExpl(ResExpl& resExpl) const
{
  for (auto& row: rows)
    resExpl.insert(row);
}
*/


void CoverTableau::updateStats(
  ProductStats& productStats,
  const bool newTableauFlag) const
{
  for (auto& row: rows)
    row.updateStats(productStats, sumProfile, newTableauFlag);
}


bool CoverTableau::operator < (const CoverTableau& tableau2) const
{
  const unsigned char compl1 = CoverTableau::getComplexity();
  const unsigned char compl2 = tableau2.getComplexity();

  if (compl1 < compl2)
    return true;
  else if (compl1 > compl2)
    return false;

  const unsigned char over1 = CoverTableau::getOverlap();
  const unsigned char over2 = tableau2.getOverlap();

  if (over1 < over2)
    return true;
  else if (over1 > over2)
    return false;
  else
    return (rows.size() <= tableau2.rows.size());
  
  // TODO Is there more we can do here in case of equality?
  // Maybe we like symmetry?
}


bool CoverTableau::complete() const
{
  return (! rows.empty() && residualWeight == 0);
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


unsigned char CoverTableau::getResidualWeight() const
{
  return residualWeight;
}


string CoverTableau::str() const
{
  stringstream ss;

  ss << "Always take at least " << +tricksMin << " tricks, and more when\n";
  for (auto& row: rows)
    ss << row.str(sumProfile);

  return ss.str() + "\n";
}


string CoverTableau::strResiduals() const
{
  return residuals.strSpaced();
}

