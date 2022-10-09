/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cassert>

#include "Component.h"
#include "VerbalConnection.h"

#include "../inputs/parse.h"

const static string prefix = "languages/";


void Component::init(const list<VerbalConnection>& connections)
{
  // Figure out the size of the table lookup.
  unsigned highest = 0;
  for (auto& vc: connections)
  {
    if (vc.instance > highest)
      highest = vc.instance;
  }

  lookup.resize(highest+1);

  // Prepare the lookups -- still missing its text.
  for (auto& vc: connections)
  {
    VerbalInstance& vi = lookup[vc.instance];
    vi.group = vc.group;
    vi.expansion = vi.expansion;
    vi.text = "";

    auto mit = instanceMap.find(vc.tag);

    if (mit == instanceMap.end())
    {
      instanceMap[vc.tag] = vc.instance;
    }
    else
    {
      // Should not have duplicated tags.
      cout << "Duplicated tag " << vc.tag << endl;
      assert(false);
    }
  }
}


void Component::read(
  const string& language,
  const string& filename)
{
  ifstream fin;
  const string fname = prefix + language + "/" + filename;
  fin.open(fname);
  if (! fin)
  {
    cout << "Could not open file " << fname << endl;
    assert(false);
  }

  string line, tag, text;

  while (getline(fin, line))
  {
    line.erase(remove(line.begin(), line.end(), '\r'), line.end());

    // Skip a commented or empty line.
    if (line == "" || line.front() == '#')
      continue;

    const string err = "File " + fname + ": Bad line '" + line + "'";

    // Find the first space (there must be one).
    const auto sp = line.find(" ");
    if (sp == string::npos || sp == 0 ||sp == line.size()-1)
    {
      cout << err << endl;
      assert(false);
    }

    const string& tagname = line.substr(0, sp);
    const string rest = line.substr(sp+1);

    if (tagname == "name")
    {
      tag = rest;
    }
    else if (tagname == "string")
    {
      if (! parseQuotedString(rest, text))
      {
        cout << err << endl;
        cout << "Double-quoted text not found" << endl;
        assert(false);
      }

      auto mit = instanceMap.find(tag);
      if (mit == instanceMap.end())
      {
        cout << err << endl;
        cout << "Tag " << tag << " not found" << endl;
        assert(false);
      }
      else
      {
        const unsigned index = mit->second;
        assert(index < lookup.size());
        lookup[index].text = text;
      }
    }
    else
    {
      // Only "name" or "string" are allowed.
      cout << err << endl;
      assert(false);
    }
  }

  fin.close();
}


const VerbalInstance& Component::get(const size_t index) const
{
  assert(index < lookup.size());
  return lookup[index];
}
