/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_PRODUCTSTATS_H
#define SSS_PRODUCTSTATS_H

#include <vector>
#include <map>
#include <string>

using namespace std;

class Profile;
struct FactoredProduct;


class ProductStats
{
  private:

    struct TableEntry
    {
      unsigned numUniques;
      unsigned numTableaux;
      unsigned numUses;

      string strHeader() const
      {
        stringstream ss;

        ss <<
          setw(8) << "Uniques" <<
          setw(10) << "Tableaux" <<
          setw(10) << "Solutions" << "\n";

        return ss.str();
      };

      string str() const
      {
        stringstream ss;

        ss << 
          setw(8) << numUniques <<
          setw(10) << numTableaux <<
          setw(10) << numUses;

        return ss.str();
      };
    };

    struct LengthEntry
    {
      FactoredProduct const * factoredProductPtr;
      vector<unsigned> histo;
      unsigned numUses;

      string strHeader() const
      {
        stringstream ss;

        ss << setw(7) << "Uses";

        // No newline
        for (unsigned i = 1; i < histo.size(); i++)
          ss << setw(7) << i;

        return ss.str();
      };

      string str() const
      {
        stringstream ss;

        ss << setw(7) << numUses;

        // No newline
        for (unsigned i = 1; i < histo.size(); i++)
          ss << setw(7) << histo[i];

        return ss.str();
      };
    };

    struct LengthTopEntry
    {
      FactoredProduct const * factoredProductPtr;
      unsigned numTableaux;
      unsigned numUses;

      string strHeader() const
      {
        stringstream ss;

        // This is only the partial header, so no newline.
        ss <<
          setw(10) << "Tableaux" <<
          setw(10) << "Solutions";

        return ss.str();
      };

      string str() const
      {
        stringstream ss;

        ss <<
          setw(10) << numTableaux <<
          setw(10) << numUses;

        return ss.str();
      };
    };


    vector<vector<TableEntry>> tableStats;

    vector<map<unsigned long long, LengthEntry>> lengthStats;

    vector<vector<map<unsigned long long, LengthTopEntry>>> lengthTopStats;

    vector<unsigned char> seenLength;

    vector<vector<unsigned char>> seenLengthTops;


  public:

    ProductStats();
     
    void resize();

    void storeLengthTops(
      const FactoredProduct& factoredProduct,
      const unsigned long long code,
      const unsigned char length,
      const unsigned char maxTops,
      const bool newTableauFlag,
      bool& newFlag);

    void storeLength(
      const FactoredProduct& factoredProduct,
      const unsigned long long code,
      const unsigned char length,
      const unsigned char maxTops);

    void storeTable(
      const unsigned char length,
      const unsigned char maxTops,
      const bool newTableauFlag,
      const bool newFlag);

    string strTableHeader() const;

    string strHeader(
      const string& general,
      const string& detail) const;


  public:

    void store(
      const FactoredProduct& factoredProduct,
      const Profile& sumProfile,
      const bool newTableauFlag);

    string strTable() const;

    string strByLength() const;

    string strByLengthTops() const;
};

#endif
