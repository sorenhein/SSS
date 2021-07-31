#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Comparer.h"

using namespace std;


Comparer::Comparer()
{
  Comparer::reset();
}


void Comparer::reset()
{
  dim1 = 0;
  dim2 = 0;
  matrix.clear();
}


void Comparer::resize(
  const unsigned size1,
  const unsigned size2)
{
  dim1 = size1;
  dim2 = size2;

  matrix.resize(size1);
  for (unsigned i = 0; i < size1; i++)
    matrix[i].resize(size2);
}


void Comparer::log(
  const unsigned n1,
  const unsigned n2,
  const WinnerCompare cmp)
{
  assert(n1 < dim1);
  assert(n2 < dim2);
  matrix[n1][n2] = cmp;
}


bool Comparer::logForEquality(
  const unsigned n1,
  const unsigned n2,
  const WinnerCompare cmp)
{
  assert(n1 < dim1);
  assert(n2 < dim2);

  if (cmp == WIN_FIRST || cmp == WIN_SECOND)
    return false;
  else
  {
    matrix[n1][n2] = cmp;
    return true;
  }
}


void Comparer::makeMarginals(
  vector<vector<unsigned>>& marginal1,
  vector<vector<unsigned>>& marginal2) const
{
  for (unsigned i = 0; i < dim1; i++)
  {
    for (unsigned j = 0; j < dim2; j++)
    {
      const WinnerCompare c = matrix[i][j];
      marginal1[i][c]++;
      marginal2[j][c]++;
    }
  }
}


void Comparer::summarize(
  const vector<vector<unsigned>>& marginal,
  vector<unsigned>& sum) const
{
  sum.resize(WIN_UNSET);

  for (auto& elem: marginal)
  {
    if (elem[WIN_FIRST])
    {
      assert(! elem[WIN_SECOND] && ! elem[WIN_EQUAL]);
      sum[WIN_FIRST]++;
    }
    else if (elem[WIN_SECOND])
    {
      assert(! elem[WIN_EQUAL]);
      sum[WIN_SECOND]++;
    }
    else if (elem[WIN_EQUAL])
      sum[WIN_EQUAL]++;
    else
      sum[WIN_DIFFERENT]++;
  }
}


void Comparer::makeSums(
  vector<unsigned>& sum1,
  vector<unsigned>& sum2) const
{
  vector<vector<unsigned>> marginal1, marginal2;

  marginal1.resize(dim1);
  for (unsigned i = 0; i < dim1; i++)
    marginal1[i].resize(WIN_UNSET);

  marginal2.resize(dim2);
  for (unsigned j = 0; j < dim2; j++)
    marginal2[j].resize(WIN_UNSET);

  Comparer::makeMarginals(marginal1, marginal2);

  Comparer::summarize(marginal1, sum1);
  Comparer::summarize(marginal2, sum2);
}


WinnerCompare Comparer::compare() const
{
  vector<unsigned> sum1, sum2;
  Comparer::makeSums(sum1, sum2);

/* */
cout << "sum1\n";
cout << "FIRST  " << sum1[WIN_FIRST] << "\n";
cout << "SECOND " << sum1[WIN_SECOND] << "\n";
cout << "EQUAL  " << sum1[WIN_EQUAL] << "\n";
cout << "DIFF   " << sum1[WIN_DIFFERENT] << "\n";
cout << "\n";

cout << "sum1\n";
cout << "FIRST  " << sum2[WIN_FIRST] << "\n";
cout << "SECOND " << sum2[WIN_SECOND] << "\n";
cout << "EQUAL  " << sum2[WIN_EQUAL] << "\n";
cout << "DIFF   " << sum2[WIN_DIFFERENT] << "\n";
cout << "\n";

/* */

  if (sum1[WIN_DIFFERENT] && sum2[WIN_DIFFERENT])
    return WIN_DIFFERENT;

  if (sum1[WIN_FIRST] && sum1[WIN_SECOND])
    return WIN_DIFFERENT;

  if (sum2[WIN_FIRST] && sum2[WIN_SECOND])
    return WIN_DIFFERENT;

  if (sum1[WIN_DIFFERENT])
  {
    // Only #2 can be better for declarer.
    if (! sum2[WIN_FIRST])
      return WIN_SECOND;
    else
      return WIN_DIFFERENT;
  }
  else if (sum2[WIN_DIFFERENT])
  {
    // Only #1 can be better for declarer.
    if (! sum1[WIN_SECOND])
      return WIN_FIRST;
    else
      return WIN_DIFFERENT;
  }
  else if (sum1[WIN_FIRST])
    return WIN_FIRST;
  else if (sum2[WIN_SECOND])
    return WIN_SECOND;
  else
    return WIN_EQUAL;
}


bool Comparer::equal() const
{
  assert(dim1 == dim2);

  vector<unsigned> sum1, sum2;
  Comparer::makeSums(sum1, sum2);

  assert(sum1[WIN_EQUAL] == sum2[WIN_EQUAL]);
  return (sum1[WIN_EQUAL] == dim1);
}


string Comparer::strHeader() const
{
  stringstream ss;
  ss << setw(3) << "" << " | ";
  for (unsigned j = 0; j < dim2; j++)
    ss << setw(3) << j;
  ss << "\n";

  ss << "----+-" << string(3 * dim2, '-') << "\n";
  return ss.str();
}


string Comparer::str() const
{
  stringstream ss;
  ss << Comparer::strHeader();

  for (unsigned i = 0; i < dim1; i++)
  {
    ss << setw(3) << i << " | ";
    for (unsigned j = 0; j < dim2; j++)
    {
      string s;
      const WinnerCompare c = matrix[i][j];
      if (c == WIN_FIRST)
        s = ">";
      else if (c == WIN_SECOND)
        s = "<";
      else if (c == WIN_EQUAL)
        s = "=";
      else
        s = "?";
      ss << setw(3) << s;
    }
    ss << "\n";
  }
  ss << endl;
  return ss.str();
}
