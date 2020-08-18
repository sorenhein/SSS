#ifndef SSS_CONTROL_H
#define SSS_CONTROL_H

#include <map>
#include <list>
#include <vector>
#include <string>

#include "Entity.h"

using namespace std;


enum ControlFieldStrings
{
  CTRL_NORTH = 0,
  CTRL_SOUTH = 1,
  CTRL_INPUT_FILE = 2,
  CTRL_CONTROL_FILE = 3,
  CTRL_TEXT = 4,

  CTRL_SIMPLE_DIRECTORY = 5,
  CTRL_CONSTANT_DIRECTORY = 6,
  CTRL_ROUDI_DIRECTORY = 7,

  CTRL_STRINGS_SIZE = 8
};

enum ControlIntVectors
{
  CTRL_OUTPUT = 0,
  CTRL_DEBUG = 1,

  CTRL_INT_VECTORS_SIZE = 2
};

enum ControlBools
{
  CTRL_BOOLS_SIZE = 0
};

enum ControlInts
{
  CTRL_HOLDING = 0,
  CTRL_CARDS = 1,

  CTRL_WEST_MIN = 2,
  CTRL_WEST_MAX = 3,
  CTRL_WEST_VACANT = 4,
  CTRL_EAST_MIN = 5,
  CTRL_EAST_MAX = 6,
  CTRL_EAST_VACANT = 7,

  CTRL_GOAL = 8,

  CTRL_NUM_THREADS = 9,

  CTRL_INTS_SIZE = 10
};

enum ControlDoubles
{
  CTRL_EXPECT = 0,

  CTRL_DOUBLES_SIZE = 1
};

enum ControlOutput
{
  CTRL_OUTPUT_BIT0 = 0,
  CTRL_OUTPUT_BIT1 = 1
};

enum ControlDebug
{
  CTRL_DEBUG_ARGS = 0,
  CTRL_DEBUG_BIT1 = 1
};


class Control
{
  private:


    list<CorrespondenceEntry> fields;

    vector<unsigned> fieldCounts;

    list<CommandLineEntry> commands;

    Entity entry;


    void configure();

    string strHex(const int val) const;
    string strDouble(const double val) const;
    string strBitVector(
      const vector<int>& bits,
      const string& doc) const;


  public:

    Control();

    ~Control();

    void reset();

    bool parseCommandLine(
      int argc,
      char * argv[]);

    bool readFile(const string& fname);

    bool completeHoldings();
    void completeCounts();

    const string& north() const;
    const string& south() const;
    unsigned holding() const;
    unsigned cards() const;
    int westMin() const;
    int westMax() const;
    int westVacant() const;
    int eastMin() const;
    int eastMax() const;
    int eastVacant() const;

    int goal() const;
    double expect() const;

    const string& inputFile() const; 
    const string& controlFile() const; 
    const string& text() const; 
    bool outputBit0() const; 
    bool outputBit1() const; 
    bool debugArgs() const; 
    bool debugBit1() const; 

    const string& simpleDir() const;
    const string& constantDir() const;
    const string& roudiDir() const;

    int numThreads() const;

    string str() const;
};

#endif
