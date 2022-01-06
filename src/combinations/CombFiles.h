/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COMBFILES_H
#define SSS_COMBFILES_H

using namespace std;

class CombEntry;
class CombReference;


class CombFiles
{
  private:

    void makeNames(
      const unsigned cards,
      const string& prefix,
      string& nameControl,
      string& nameHoldings) const;

    template <class T>
    void readFile(
      const string& name,
      vector<T>& v) const;

    template <class T>
    void writeFile(
      const string& name,
      vector<T>& v) const;

    /*
    void unpack(
      const unsigned char data,
      CombEntry& ce) const;

    void pack(
      const CombEntry& ce,
      unsigned char& data) const;
      */

    void getHolding(
      const vector<unsigned>& vHoldings,
      unsigned& pos,
      CombReference& cr) const;

    void putHolding(
      const CombReference& cr,
      vector<unsigned>& vHoldings,
      unsigned& pos) const;

  public:

    void readFiles(
      const unsigned cards,
      vector<CombEntry>& combinations) const;

    void writeFiles(
      const unsigned cards,
      const vector<CombEntry>& combinations) const;
};

#endif
