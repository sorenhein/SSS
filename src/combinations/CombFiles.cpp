/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

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
  ifstream is(name, ios::in | ios::binary);
  
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


/*
void CombFiles::unpack(
  const unsigned char data,
  CombEntry& ce) const
{
  ce.unpackFlags(data);
}


void CombFiles::pack(
  const CombEntry& ce,
  unsigned char& data) const
{
  data = ce.packFlags();
}
*/


void CombFiles::getHolding(
  const vector<unsigned>& vHoldings,
  unsigned& pos,
  CombReference& cr) const
{
  cr.unpack(vHoldings, pos);

  // TODO At the moment we neither write nor read holding2.
  // This now lives directly in CombEntry and not in CombReference.
  // Perhaps we should read and write the reference one.
}


void CombFiles::putHolding(
  const CombReference& cr,
  vector<unsigned>& vHoldings,
  unsigned& pos) const
{
  cr.pack(vHoldings, pos);
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
    // CombFiles::unpack(vControl[h], ce);
    ce.unpackFlags(vControl[h]);

    if (ce.isReference())
    {
      // No canonical reference, as this is already canonical.
      ce.unpackMinimals(vHoldings, pos);
      // for (auto& min: ce.minimals)
        // min.unpack(vHoldings, pos);
        // CombFiles::getHolding(vHoldings, pos, min);
    }
    else
      ce.unpackSelf(vHoldings, pos);
      // CombFiles::getHolding(vHoldings, pos, ce.reference);
  }
}


void CombFiles::writeFiles(
  const unsigned cards,
  const vector<CombEntry>& combinations) const
{
  string nameControl, nameHoldings;
  const string prefix = 
    (control.binaryOutputDir() == "" ? "" : control.binaryOutputDir() + "/");
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
    vControl[h] = ce.packFlags();

    if (ce.isReference())
    {
      ce.packMinimals(vHoldings, pos);
      // for (auto& min: ce.minimals)
        // min.pack(vHoldings, pos);
        // CombFiles::putHolding(min, vHoldings, pos);
    }
    else
      ce.packSelf(vHoldings, pos);
      // CombFiles::putHolding(ce.reference, vHoldings, pos);
  }

  vHoldings.resize(pos);

  CombFiles::writeFile<unsigned char>(nameControl, vControl);
  CombFiles::writeFile<unsigned>(nameHoldings, vHoldings);
}

