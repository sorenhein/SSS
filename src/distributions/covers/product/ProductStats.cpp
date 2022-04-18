/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "ProductStats.h"
#include "Profile.h"
#include "Product.h"


ProductStats::ProductStats()
{
  ProductStats::resize();
}


void ProductStats::resize()
{
  tableStats.resize(14);
  for (unsigned i = 0; i < tableStats.size(); i++)
    tableStats[i].resize(i+1);

  lengthStats.resize(14);

  lengthTopStats.resize(14);
  for (unsigned i = 0; i < lengthTopStats.size(); i++)
    lengthTopStats[i].resize(i+1);
}


void ProductStats::storeLengthTops(
  const Product& product,
  const unsigned long long code,
  const unsigned char length,
  const unsigned char maxTops,
  const bool newTableauFlag,
  bool& newFlag)
{
  assert(length < lengthTopStats.size());
  assert(maxTops < lengthTopStats[length].size());
  auto& lengthTopMap = lengthTopStats[length][maxTops];

  auto lengthTopIter = lengthTopMap.find(code);
  if (lengthTopIter == lengthTopMap.end())
  {
    newFlag = false;
    LengthTopEntry& entry = lengthTopMap[code] = LengthTopEntry();

    entry.productPtr = &product;
    if (newTableauFlag)
      entry.numTableaux++;
    entry.numUses++;
  }
  else
  {
    newFlag = true;
    LengthTopEntry& entry = lengthTopIter->second;
    if (newTableauFlag)
      entry.numTableaux++;
    entry.numUses++;
  }
}


void ProductStats::storeLength(
  const Product& product,
  const unsigned long long code,
  const unsigned char length,
  const unsigned char maxTops,
  const bool newFlag)
{
  assert(length < lengthStats.size());
  assert(maxTops < lengthStats[length].size());
  auto& lengthMap = lengthStats[length];

  auto lengthIter = lengthMap.find(code);
  if (lengthIter == lengthMap.end())
  {
    assert(! newFlag);
    LengthEntry& entry = lengthMap[code] = LengthEntry();

    entry.productPtr = &product;
    entry.histo.resize(length+1);
    entry.histo[maxTops] = 1;
    entry.numUses++;
  }
  else
  {
    assert(newFlag);
    LengthEntry& entry = lengthIter->second;
    assert(maxTops < entry.histo.size());
    entry.histo[maxTops]++;
    entry.numUses++;
  }
}


void ProductStats::storeTable(
  const unsigned char length,
  const unsigned char maxTops,
  const bool newTableauFlag,
  const bool newFlag)
{
  assert(length < tableStats.size());
  assert(maxTops < tableStats[length].size());
  auto& entry = tableStats[length][maxTops];

  if (newFlag)
    entry.numUniques++;
  if (newTableauFlag)
    entry.numTableaux++;
  entry.numUses++;
}


void ProductStats::store(
  const Product& product,
  const Profile& sumProfile,
  const bool newTableauFlag)
{
  const unsigned long long code = product.code();
  const unsigned char length = sumProfile.length();
  const unsigned char maxTops =
    sumProfile[static_cast<unsigned char>(sumProfile.size())-1];

  bool newFlag;
  ProductStats::storeLengthTops(
    product, 
    code,
    length,
    maxTops,
    newTableauFlag, 
    newFlag);

  ProductStats::storeLength(
    product, 
    code,
    length,
    maxTops,
    newFlag);

  ProductStats::storeTable(
    length,
    maxTops,
    newTableauFlag,
    newFlag);
}


string ProductStats::strTableHeader() const
{
  stringstream ss;

  ss << "ProductStats summary table\n\n";

  ss <<
    setw(16) << "" <<
    setw(12) << "Used in" << "\n";

  ss <<
    setw(16) << "" << string(20, '-');

  ss <<
    setw(8) << "Len-top" <<
    tableStats[0][0].strHeader() << "\n";

  return ss.str();
}


string ProductStats::strTable() const
{
  stringstream ss;

  for (unsigned length = 1; length < tableStats.size(); length++)
  {
    for (unsigned maxTops = 1; maxTops < tableStats[length].size(); 
        maxTops++)
    {
      const TableEntry& entry = tableStats[length][maxTops];
      if (entry.numUniques)
      {
        const string s = to_string(length) + "-" + to_string(maxTops);
        ss <<
          setw(8) << s <<
          setw(8) << entry.str() << "\n";
      }
    }
  }

  return ss.str();
}


string ProductStats::strHeader(
  const string& general,
  const string& detail) const
{
  stringstream ss;

  string overall = "ProductStats by ";
  ss << overall << general << ": " << detail << "\n";

  ss << 
    string(overall.size() + general.size() + detail.size() + 2, '-') 
    << "\n\n";

  return ss.str();
}


string ProductStats::strByLength() const
{
  stringstream ss;

  LengthEntry ltmp;
  Product ptmp;
  const string subheader = ltmp.strHeader() + ptmp.strHeader();

  for (unsigned length = 1; length < lengthStats.size(); length++)
  {
      const string key = to_string(length);
      ss << ProductStats::strHeader("length", key);

      ss << 
        subheader << "\n" <<
        string(subheader.size(), '-') << "\n";

      vector<unsigned> histo(lengthStats.size()+1);
      unsigned sumUses = 0;
      unsigned num = 0;

      for (auto& lpair: lengthStats[length])
      {
        auto& entry = lpair.second;
        if (entry.numUses)
        {
          ss <<
            entry.str() <<
            entry.productPtr->strLine() << "\n";
        
          for (unsigned i = 0; i < entry.histo.size(); i++)
            histo[i] += entry.histo[i];
          sumUses += entry.numUses;
          num++;
        }
      }

      ss << string(subheader.size(), '-') << "\n" << setw(5) << sumUses;

      for (unsigned i = 0; i < histo.size(); i++)
      {
        if (histo[i] == 0)
          ss << setw(5) << "-";
        else
          ss << setw(5) << histo[i];
      }

      ss << "  " << num << " entries\n\n";
  }

  return ss.str();
}


string ProductStats::strByLengthTops() const
{
  stringstream ss;

  LengthTopEntry ltmp;
  Product ptmp;
  const string subheader = ltmp.strHeader() + ptmp.strHeader();

  for (unsigned length = 1; length < lengthTopStats.size(); length++)
  {
    for (unsigned maxTops = 1; maxTops < lengthTopStats[length].size(); 
        maxTops++)
    {
      const string key = to_string(length) + "-" + to_string(maxTops);
      ss << ProductStats::strHeader("length and maximum tops", key);

      ss << 
        subheader << "\n" <<
        string(subheader.size(), '-') << "\n";

      unsigned sumTableaux = 0;
      unsigned sumUses = 0;
      unsigned num = 0;

      for (auto& lpair: lengthTopStats[length][maxTops])
      {
        auto& entry = lpair.second;
        if (entry.numUses)
        {
          ss <<
            entry.str() <<
            entry.productPtr->strLine() << "\n";
        
          sumTableaux += entry.numTableaux;
          sumUses += entry.numUses;
          num++;
        }
      }

      ss <<
        string(subheader.size(), '-') << "\n" <<
          setw(10) << sumTableaux <<
          setw(10) << sumUses <<
          "  " <<
          num << " entries\n\n";
    }
  }

  return ss.str();
}

