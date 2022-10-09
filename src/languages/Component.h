/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COMPONENT_H
#define SSS_COMPONENT_H

#include <vector>
#include <list>
#include <map>
#include <string>

struct VerbalConnection;

using namespace std;


struct VerbalInstance
{
  unsigned group;
  unsigned expansion;
  string text;
};


class Component
{
  private:

    vector<VerbalInstance> lookup;

    map<string, unsigned> instanceMap;


  public:

    void init(const list<VerbalConnection>& connections);

    void read(
      const string& language,
      const string& filename);

    const VerbalInstance& get(const unsigned index) const;
};

#endif
