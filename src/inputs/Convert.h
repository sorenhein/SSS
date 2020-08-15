#ifndef SSS_CONVERT_H
#define SSS_CONVERT_H

#include <map>
#include <list>
#include <vector>
#include <string>

using namespace std;


class Convert
{
  private:

    enum CardPosition
    {
      CONVERT_NORTH = 0,
      CONVERT_SOUTH = 1,
      CONVERT_OPPS = 2
    };


    vector<string> index2card;
    map<string, int> card2index;

    vector<vector<string>> number2card;


  public:

    Convert();

    ~Convert();

    void reset();

    bool holding2cards(
      const int holding,
      const int cards,
      string& north,
      string& south);

    bool cards2holding(
      const string& north,
      const string& south,
      const int cards,
      int& holding);
};

#endif
