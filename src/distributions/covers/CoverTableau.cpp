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
}


void CoverTableau::setTricks(const list<Result>& tricks)
{
  residuals.resize(tricks.size());
  residualsSum = 0;

  // TODO Here we turn a list into a vector.  Perhaps we could use
  // lists all the way down?
 
  auto titer = tricks.begin();
  unsigned i;

  for (i = 0; i < tricks.size(); i++, titer++)
  {
    const unsigned char t = titer->getTricks();
    residuals[i] = t;
    residualsSum += t;
  }
}


bool CoverTableau::attemptGreedy(const CoverNew& cover)
{
  // explained is a dummy vector here.
  vector<unsigned char> explained(cover.getNumDist(), 0);
  vector<unsigned char> additions(cover.getNumDist());
  unsigned char tricksAdded;

  // First try to add a new row.
  if (cover.possible(explained, residuals, additions, tricksAdded))
  {
    rows.emplace_back(CoverRow());
    CoverRow& row = rows.back();
    row.add(cover, additions, residuals, residualsSum);
    return true;
  }

  // Then look for a row, or the best one, to add to.
  if (rows.empty())
    return false;

  CoverRow * rowBestPtr = nullptr;
  vector<unsigned char> additionsBest(cover.getNumDist());
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

  rowBestPtr->add(cover, additions, residuals, residualsSum);
  return true;
}


void CoverTableau::attemptExhaustive(
  list<CoverNew>::const_iterator& coverIter,
  list<StackTableau>& stack,
  list<CoverTableau>& solutions) const
{
  // This is similar to attemptGreedy, but we don't stop once we have
  // the first match, and we don't yet implement any match.

  // explained is a dummy vector here.
  const CoverNew& cover = * coverIter;
  vector<unsigned char> explained(cover.getNumDist(), 0);
  vector<unsigned char> additions(cover.getNumDist());
  unsigned char tricksAdded;

  // First try to add a new row.
  if (cover.possible(explained, residuals, additions, tricksAdded))
  {
    stack.emplace_back(StackTableau());
    StackTableau& stableau = stack.back();

    CoverTableau& tableau = stableau.tableau;
    tableau = * this;
    tableau.rows.emplace_back(CoverRow());
    CoverRow& row = tableau.rows.back();
    row.add(cover, additions, tableau.residuals, tableau.residualsSum);

    stableau.coverIter = coverIter;

    if (tableau.complete())
    {
      solutions.push_back(tableau);
      stack.pop_back();
    }
  }

  // Then look for a row, or the best one, to add to.
  if (rows.empty())
    return;

  unsigned rno = 0, r;
  list<CoverRow>::iterator riter;

  for (auto& row: rows)
  {
    if (row.attempt(cover, residuals, additions, tricksAdded))
    {
      stack.emplace_back(StackTableau());
      StackTableau& stableau = stack.back();

      CoverTableau& tableau = stableau.tableau;
      tableau = * this;

      // A bit fumbly: Advance to the same place in tableau.
      for (riter = tableau.rows.begin(), r = 0; r < rno; riter++, r++);

      riter->add(cover, additions, tableau.residuals, tableau.residualsSum);

      stableau.coverIter = coverIter;

      if (tableau.complete())
      {
        solutions.push_back(tableau);
        stack.pop_back();
      }
    }
  }
}


bool CoverTableau::operator < (const CoverTableau& tableau2) const
{
  return (CoverTableau::getComplexity() < tableau2.getComplexity());
}


bool CoverTableau::complete() const
{
  return (! rows.empty() && residualsSum == 0);
}


unsigned char CoverTableau::getComplexity() const
{
  unsigned char complexity = 0;
  for (auto& row: rows)
  {
    // TODO TEMP
    assert(complexity + row.getComplexity() > complexity);

    complexity += row.getComplexity();
  }
  return complexity;
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


string CoverTableau::str() const
{
  stringstream ss;

  for (auto& row: rows)
  {
    ss << row.strHeader();
    ss << row.strLines() << "\n";
  }

  return ss.str();
}

