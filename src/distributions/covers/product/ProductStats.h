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
class Product;


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
      Product const * productPtr;
      vector<unsigned> histo;
      unsigned numUses;

      string strHeader() const
      {
        stringstream ss;

        ss << setw(5) << "Uses";

        // No newline
        for (unsigned i = 1; i < histo.size(); i++)
          ss << setw(5) << i;

        return ss.str();
      };

      string str() const
      {
        stringstream ss;

        ss << setw(5) << numUses;

        // No newline
        for (unsigned i = 1; i < histo.size(); i++)
          ss << setw(5) << histo[i];

        return ss.str();
      };
    };

    struct LengthTopEntry
    {
      Product const * productPtr;
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


  public:

    ProductStats();
     
    void resize();

    void storeLengthTops(
      const Product& product,
      const unsigned long long code,
      const unsigned char length,
      const unsigned char maxTops,
      const bool newTableauFlag,
      bool& newFlag);

    void storeLength(
      const Product& product,
      const unsigned long long code,
      const unsigned char length,
      const unsigned char maxTops,
      const bool newFlag);

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
      const Product& product,
      const Profile& sumProfile,
      const bool newTableauFlag);

    string strTable() const;

    string strByLength() const;

    string strByLengthTops() const;
};

#endif
