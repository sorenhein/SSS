/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <list>
#include <mutex>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "ProductStats.h"
#include "Profile.h"
#include "FactoredProduct.h"

#include "../../../utils/Compare.h"

mutex mtxProductStats;


ProductStats::ProductStats()
{
  ProductStats::resize();
}


void ProductStats::resize()
{
  tableStats.resize(14);
  lengthTopStats.resize(14);
  seenLengthTops.resize(14);
  for (unsigned i = 0; i < 14; i++)
  {
    tableStats[i].resize(i+1);
    lengthTopStats[i].resize(i+1);
    seenLengthTops[i].resize(i+1);
  }

  lengthStats.resize(14);
  seenLength.resize(14, false);
}


void ProductStats::storeLengthTops(
  const FactoredProduct& factoredProduct,
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
    newFlag = true;
    LengthTopEntry& entry = lengthTopMap[code] = LengthTopEntry();

    entry.factoredProductPtr = &factoredProduct;
    if (newTableauFlag)
      entry.numTableaux++;
    entry.numUses++;
  }
  else
  {
    newFlag = false;
    LengthTopEntry& entry = lengthTopIter->second;
    if (newTableauFlag)
      entry.numTableaux++;
    entry.numUses++;
  }
}


void ProductStats::storeLength(
  const FactoredProduct& factoredProduct,
  const unsigned long long code,
  const unsigned char length,
  const unsigned char maxTops)
{
  assert(length < lengthStats.size());
  auto& lengthMap = lengthStats[length];

  auto lengthIter = lengthMap.find(code);
  if (lengthIter == lengthMap.end())
  {
    LengthEntry& entry = lengthMap[code] = LengthEntry();

    entry.factoredProductPtr = &factoredProduct;
    entry.histo.resize(length+1);
    entry.histo[maxTops] = 1;
    entry.numUses++;
  }
  else
  {
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

  seenLength[length]++;
  seenLengthTops[length][maxTops]++;

  if (newFlag)
    entry.numUniques++;
  if (newTableauFlag)
    entry.numTableaux++;
  entry.numUses++;
}


void ProductStats::store(
  const FactoredProduct& factoredProduct,
  const Profile& sumProfile,
  const bool newTableauFlag)
{
  const unsigned long long code = factoredProduct.code();
  const unsigned char length = sumProfile.length();
  const unsigned char maxTops =
    sumProfile[static_cast<unsigned char>(sumProfile.size())-1];

  bool newFlag;

  mtxProductStats.lock();

  ProductStats::storeLengthTops(
    factoredProduct, 
    code,
    length,
    maxTops,
    newTableauFlag, 
    newFlag);

  ProductStats::storeLength(
    factoredProduct, 
    code,
    length,
    maxTops);

  ProductStats::storeTable(
    length,
    maxTops,
    newTableauFlag,
    newFlag);

  mtxProductStats.unlock();
}


string ProductStats::strTableHeader() const
{
  stringstream ss;

  ss << "ProductStats summary table\n";
  ss << string(26, '-') << "\n\n";

  ss <<
    setw(16) << "" <<
    setw(12) << "Used in" << "\n";

  ss <<
    setw(16) << "" << string(20, '-') << "\n";

  ss <<
    setw(8) << "Len-top" <<
    tableStats[0][0].strHeader();

  ss << string(36, '-') << "\n";

  return ss.str();
}


string ProductStats::strTable() const
{
  stringstream ss;

  ss << ProductStats::strTableHeader();

  for (unsigned length = 1; length < tableStats.size(); length++)
  {
    if (! seenLength[length])
      continue;

    for (unsigned maxTops = 1; maxTops < tableStats[length].size(); 
        maxTops++)
    {
      if (! seenLengthTops[length][maxTops])
        continue;

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

  return ss.str() + "\n";
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


void ProductStats::makeLengthList(
  const unsigned length,
  list<LengthEntry const *>& presentationList) const
{
  for (auto &[key, entry]: lengthStats[length])
  {
    if (entry.numUses)
      presentationList.push_back(&entry);
  }

  presentationList.sort([](
    LengthEntry const *& leptr1, 
    LengthEntry const *& leptr2)
  {
    const CompareType c =
      leptr1->factoredProductPtr->presentOrder(
        * leptr2->factoredProductPtr);

    return (c == WIN_FIRST || c == WIN_EQUAL);
  });
}


void ProductStats::makeLengthTopsList(
  const unsigned length,
  const unsigned maxTops,
  list<LengthTopEntry const *>& presentationList) const
{
  for (auto &[key, entry]: lengthTopStats[length][maxTops])
  {
    if (entry.numUses)
      presentationList.push_back(&entry);
  }

  presentationList.sort([](
    LengthTopEntry const *& lteptr1, 
    LengthTopEntry const *& lteptr2)
  {
    const CompareType c =
      lteptr1->factoredProductPtr->presentOrder(
        * lteptr2->factoredProductPtr);
  
    return (c == WIN_FIRST || c == WIN_EQUAL);
  });
}


string ProductStats::strByLength() const
{
  stringstream ss;

  for (unsigned length = 2; length < lengthStats.size(); length++)
  {
    if (! seenLength[length])
      continue;

    const string lstr = to_string(length);
    ss << ProductStats::strHeader("length", lstr);

    const auto& lsentry = lengthStats[length].begin();
    const string subheader = lsentry->second.strHeader() + 
      lsentry->second.factoredProductPtr->strHeader(length);

    ss << 
      subheader << "\n" <<
      string(subheader.size(), '-') << "\n";

    LengthEntry sumEntry;
    sumEntry.histo.resize(length+1);

    list<LengthEntry const *> presentationList;
    ProductStats::makeLengthList(length, presentationList);

    for (auto& pptr: presentationList)
    {
      ss << 
        pptr->strStats() <<
        pptr->factoredProductPtr->strLine() << "\n";
      
      sumEntry += * pptr;
    }

    ss << string(subheader.size(), '-') << "\n";
    ss << sumEntry.strStats();
    ss << setw(8) << presentationList.size() << " entries\n\n";
  }

  return ss.str();
}


string ProductStats::strByLengthTops() const
{
  stringstream ss;

  for (unsigned length = 2; length < lengthTopStats.size(); length++)
  {
    if (! seenLength[length])
      continue;

    for (unsigned maxTops = 1; maxTops < lengthTopStats[length].size(); 
        maxTops++)
    {
      if (! seenLengthTops[length][maxTops])
        continue;

      const string lstr = to_string(length) + "-" + to_string(maxTops);
      ss << ProductStats::strHeader("length and maximum tops", lstr);

      const auto& ltentry = lengthTopStats[length][maxTops].begin();
      const string subheader = ltentry->second.strHeader() + 
        ltentry->second.factoredProductPtr->strHeader(length);

      ss << 
        subheader << "\n" <<
        string(subheader.size(), '-') << "\n";

      LengthTopEntry sumEntry;

      list<LengthTopEntry const *> presentationList;
      ProductStats::makeLengthTopsList(length, maxTops, presentationList);

      for (auto& pptr: presentationList)
      {
        ss <<
          pptr->strStats() <<
          pptr->factoredProductPtr->strLine() << "\n";
      
        sumEntry += * pptr;
      }

      ss <<
        string(subheader.size(), '-') << "\n" <<
          sumEntry.strStats() <<
          setw(8) << presentationList.size() << " entries\n\n";
    }
  }

  return ss.str();
}

