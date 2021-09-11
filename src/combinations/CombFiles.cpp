/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#include <vector>
#include <iostream>
#include <fstream>
#include <cassert>

#include "CombFiles.h"
#include "CombEntry.h"

#include "../inputs/Control.h"

extern Control control;


void CombFiles::makeNames(
  const unsigned cards,
  const string& prefix,
  string& nameControl,
  string& nameHoldings) const
{
  nameControl = prefix + "c" + to_string(cards) + "flags.dat";
  nameHoldings = prefix + "c" + to_string(cards) + "holdings.dat";
}


template <class T>
void CombFiles::readFile(
  const string& name,
  vector<T>& v) const
{
  ifstream is(name);
  
  // Determine the file length
  is.seekg(0, ios_base::end);
  size_t size = static_cast<size_t>(is.tellg());
  is.seekg(0, ios_base::beg);

  v.resize(size / sizeof(T));

  is.read(reinterpret_cast<char *>(&v[0]), size);
  is.close();
}


template <class T>
void CombFiles::writeFile(
  const string& name,
  vector<T>& v) const
{
  auto os = fstream(name, ios::out | ios::binary);
  os.write(reinterpret_cast<const char *>(&v[0]), 
    v.size() * sizeof(T) / sizeof(char));
  os.close();
}


void CombFiles::unpack(
  const unsigned char data,
  CombEntry& ce) const
{
  // Bit layout:
  // 7   canonicalFlag
  // 6   minimalFlag
  // 3-5 combination type (should stay < 8 I think)
  // 0-2 Size of minimals (should be < 8 I think)
  
  ce.canonicalFlag = ((data & 0x40) ? true : false);
  ce.minimalFlag = ((data & 0x20) ? true : false);
  ce.type = static_cast<CombinationType>((data >> 3) & 0x7);
  ce.minimals.resize(data & 0x7);
}


void CombFiles::pack(
  const CombEntry& ce,
  unsigned char& data) const
{
  data = (ce.minimals.size() & 0x7) |
    (static_cast<unsigned char>(ce.type << 3)) |
    ((ce.minimalFlag ? 1 : 0) << 6) |
    ((ce.canonicalFlag ? 1 : 0) << 7);
}


void CombFiles::getHolding(
  const vector<unsigned>& vHoldings,
  unsigned& pos,
  CombReference& cr) const
{
  // The first unsigned has holding2 on the bottom 31 bits and
  // rotateFlg in the topmost bit.
  // The second unsigned is holding3.
  
  const unsigned u1 = vHoldings[pos++];
  cr.rotateFlag = ((u1 & 0x8000) ? true : false);
  cr.holding2 = (u1 & 0x3fff);

  cr.holding3 = vHoldings[pos++];
}


void CombFiles::putHolding(
  const CombReference& cr,
  vector<unsigned>& vHoldings,
  unsigned& pos) const
{
  vHoldings[pos++] = ((cr.rotateFlag ? 1 : 0) << 31) | cr.holding2;
  vHoldings[pos++] = cr.holding3;
}


void CombFiles::readFiles(
  const unsigned cards,
  vector<CombEntry>& combinations) const
{
  string nameControl, nameHoldings;
  const string prefix = 
    (control.binaryInputDir() == "" ? "" : control.binaryInputDir() + "/");
  CombFiles::makeNames(cards, prefix, nameControl, nameHoldings);

  vector<unsigned char> vControl;
  CombFiles::readFile<unsigned char>(nameControl, vControl);

  const unsigned size = vControl.size();
  assert(size == combinations.size());

  vector<unsigned> vHoldings;
  CombFiles::readFile<unsigned>(nameHoldings, vHoldings);
  unsigned pos = 0; // Position in the holding input file

  for (unsigned h = 0; h < size; h++)
  {
    CombEntry& ce = combinations[h];
    CombFiles::unpack(vControl[h], ce);

    if (ce.canonicalFlag)
    {
      // No canonical reference, as this is already canonical.
      for (auto& min: ce.minimals)
        CombFiles::getHolding(vHoldings, pos, min);
    }
    else
      CombFiles::getHolding(vHoldings, pos, ce.canonical);
  }
}


void CombFiles::writeFiles(
  const unsigned cards,
  const vector<CombEntry>& combinations) const
{
  string nameControl, nameHoldings;
  const string prefix = 
    (control.binaryInputDir() == "" ? "" : control.binaryOutputDir() + "/");
  CombFiles::makeNames(cards, prefix, nameControl, nameHoldings);

  vector<unsigned char> vControl;
  vector<unsigned> vHoldings;

  const unsigned size = combinations.size();
  vControl.resize(size);
  vHoldings.resize(3*size);  // This is too much

  unsigned pos = 0; // Position in the holding output vector

  for (unsigned h = 0; h < size; h++)
  {
    const CombEntry& ce = combinations[h];
    CombFiles::pack(ce, vControl[h]);

    if (ce.canonicalFlag)
    {
      for (auto& min: ce.minimals)
        CombFiles::putHolding(min, vHoldings, pos);
    }
    else
      CombFiles::putHolding(ce.canonical, vHoldings, pos);
  }

  vHoldings.resize(pos);

  CombFiles::writeFile<unsigned char>(nameControl, vControl);
  CombFiles::writeFile<unsigned>(nameHoldings, vHoldings);
}

