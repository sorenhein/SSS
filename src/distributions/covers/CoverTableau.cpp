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

#include "../../strategies/result/Result.h"

// TMP
extern unsigned numCompare, numStack, numSolutions;
extern unsigned numCompareManual, numStackManual, numSolutionsManual;


CoverTableau::CoverTableau()
{
  CoverTableau::reset();
}


void CoverTableau::reset()
{
  rows.clear();
  tricksMin = 0;

  residuals.clear();
  residualWeight = 0;

  complexity.reset();
}


void CoverTableau::init(
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


void CoverTableau::addRow(
  const Cover& cover,
  const Tricks& additions,
  const vector<unsigned char>& cases)
{
  rows.emplace_back(CoverRow());
  CoverRow& row = rows.back();
  row.resize(residuals.size());
  row.add(cover, additions, cases, residuals, residualWeight);
  complexity.addRow(row.getComplexity());
}


void CoverTableau::attempt(
  const vector<unsigned char>& cases,
  set<Cover>::const_iterator& coverIter,
  list<StackEntry>& stack,
  list<CoverTableau>& solutions,
  unsigned char& lowestComplexity) const
{
  Tricks additions;
  additions.resize(residuals.size());
  unsigned char weightAdded;

numCompare++;
  if (coverIter->possible(residuals, cases, additions, weightAdded))
  {
    // A whole new row is possible.
    if (weightAdded < residualWeight)
    {
      // The cover does not lead to a solution.
numStack++;
      stack.emplace_back(StackEntry());
      StackEntry& centry = stack.back();
      centry.coverIter = coverIter;

      CoverTableau& tableau = centry.tableau;
      tableau = * this;
      tableau.addRow(* coverIter, additions, cases);
    }
    else
    {
      // The cover does lead to a solution, so there is no point in
      // looking for existing row to which to add it.
 numSolutions++;
      solutions.push_back(* this);

      CoverTableau& solution = solutions.back();
      solution.addRow(* coverIter, additions, cases);

      if (solution.complexity.sum < lowestComplexity)
        lowestComplexity = solution.complexity.sum;
      
      return;
    }
  }


  const Cover& cover = * coverIter;

  // Then look for a row, or the best one, to add to.
  unsigned rno = 0, r;
  list<CoverRow>::iterator riter;

  for (auto& row: rows)
  {
    if (row.size() >= 2)
    {
      // A row becomes too difficult to read for a human if it
      // has more than two options.
      rno++;
      continue;
    }

    if (! row.attempt(cover, residuals, cases, additions, weightAdded))
    {
      // The row does not fit.
      rno++;
      continue;
    }

    if (weightAdded == cover.getWeight())
    {
      // The cover should not be completely complementary (then we
      // have already used a new row).
      rno++;
      continue;
    }

    if (weightAdded < residualWeight)
    {
numStack++;
      stack.emplace_back(StackEntry());
      StackEntry& centry = stack.back();
      centry.coverIter = coverIter;

      CoverTableau& tableau = centry.tableau;
      tableau = * this;

      // A bit fumbly: Advance to the same place in tableau.
      for (riter = tableau.rows.begin(), r = 0; r < rno; riter++, r++);

      riter->add(cover, additions, cases,
        tableau.residuals, tableau.residualWeight);

      tableau.complexity.addCover(
        cover.getComplexity(),
        riter->getComplexity());
    }
    else
    {
numSolutions++;
      solutions.push_back(* this);
      CoverTableau& solution = solutions.back();

      // A bit fumbly: Advance to the same place in tableau.
      for (riter = solution.rows.begin(), r = 0; r < rno; riter++, r++);

      riter->add(cover, additions, cases,
        solution.residuals, solution.residualWeight);

      solution.complexity.addCover(
        cover.getComplexity(), riter->getComplexity());

      const unsigned char c = solution.getComplexity();

      if (c < lowestComplexity)
        lowestComplexity = c;
    }

    rno++;
  }
}


bool CoverTableau::attemptManually(
  const vector<unsigned char>& cases,
  list<CoverRow>::const_iterator& rowIter,
  list<RowStackEntry>& stack,
  CoverTableau& solution)
{
  // Return true if a solution is found, even if it is inferior to
  // the existing one.
  Tricks additions;
  additions.resize(residuals.size());
  unsigned char weightAdded;

numCompareManual++;
  if (! rowIter->possible(residuals, cases, additions, weightAdded))
    return false;

  if (weightAdded < residualWeight)
  {
numStackManual++;
    stack.emplace_back(RowStackEntry());
    RowStackEntry& rentry = stack.back();
    rentry.rowIter = rowIter;

    CoverTableau& tableau = rentry.tableau;
    tableau = * this;
    tableau.rows.push_back(* rowIter);
    tableau.complexity.addRow(rowIter->getComplexity());

    tableau.residuals -= additions;
    tableau.residualWeight -= weightAdded;
    return false;
  }
  else if (solution.rows.empty())
  {
    // We have a solution for sure, as it is the first one.
numSolutionsManual++;
    solution = * this;
    solution.rows.push_back(* rowIter);
    solution.complexity.addRow(rowIter->getComplexity());
    return true;
  }
  else
  {
numSolutionsManual++;
    // We can use this CoverTableau, as the stack element is about
    // to be popped anyway.
    rows.push_back(* rowIter);
    complexity.addRow(rowIter->getComplexity());

    if (complexity < solution.complexity)
    // if (* this < solution)
      solution = * this;
    return true;
  }
}


unsigned char CoverTableau::complexityHeadroom(
  const CoverTableau& solution) const
{
  return complexity.headroom(solution.complexity);
}


void CoverTableau::updateStats(
  const Profile& sumProfile,
  ProductStats& productStats,
  const bool newTableauFlag) const
{
  for (auto& row: rows)
    row.updateStats(productStats, sumProfile, newTableauFlag);
}


bool CoverTableau::operator < (const CoverTableau& tableau2) const
{
  return (complexity < tableau2.complexity);
}


bool CoverTableau::complete() const
{
  return (! rows.empty() && residualWeight == 0);
}


unsigned char CoverTableau::getComplexity() const
{
  // TODO This method can go later.
  return complexity.sum;
}


unsigned char CoverTableau::getResidualWeight() const
{
  return residualWeight;
}


string CoverTableau::strBracket() const
{
  unsigned weight = 0;

  for (auto& row: rows)
    weight += row.getWeight();

  stringstream ss;
  ss << 
    "[c " << 
    complexity.str() <<
    ", w " << weight << "]";
  return ss.str();
}


string CoverTableau::strResiduals() const
{
  return residuals.strSpaced();
}


string CoverTableau::str(const Profile& sumProfile) const
{
  stringstream ss;

  ss << "Always take at least " << +tricksMin << " tricks, and more when\n";
  for (auto& row: rows)
    ss << row.str(sumProfile);

  return ss.str() + "\n";
}

