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

  CTRL_READ_BINARY_DIRECTORY = 5,
  CTRL_WRITE_BINARY_DIRECTORY = 6,

  CTRL_SIMPLE_DIRECTORY = 7,
  CTRL_CONSTANT_DIRECTORY = 8,
  CTRL_ROUDI_DIRECTORY = 9,

  CTRL_STRINGS_SIZE = 10
};

enum ControlIntVectors
{
  CTRL_OUTPUT = 0,
  CTRL_DEBUG = 1,
  CTRL_RUN = 2,

  CTRL_INT_VECTORS_SIZE = 3
};

enum ControlBools
{
  CTRL_LOOP_FLAG = 0,
  CTRL_ALL_RANKS_FLAG = 1,
  CTRL_BOOLS_SIZE = 2
};

enum ControlInts
{
  CTRL_HOLDING = 0,
  CTRL_HOLDING_LENGTH = 1,
  CTRL_CARDS = 2,

  CTRL_WEST_MIN = 3,
  CTRL_WEST_MAX = 4,
  CTRL_WEST_VACANT = 5,
  CTRL_EAST_MIN = 6,
  CTRL_EAST_MAX = 7,
  CTRL_EAST_VACANT = 8,

  CTRL_GOAL = 9,

  CTRL_NUM_THREADS = 10,

  CTRL_INTS_SIZE = 11
};

enum ControlDoubles
{
  CTRL_EXPECT = 0,

  CTRL_DOUBLES_SIZE = 1
};

enum ControlRun
{
  CTRL_RUN_FULL_PLAYS = 0,
  CTRL_RUN_ADVANCED_NODES = 1,
  CTRL_RUN_STRATEGY_OPT = 2,
  CTRL_RUN_RANK_COMPARE = 3
};

enum ControlOutput
{
  CTRL_OUTPUT_HOLDING = 0,
  CTRL_OUTPUT_BASIC_RESULTS = 1,
  CTRL_OUTPUT_BIT2 = 2
};

enum ControlDebug
{
  CTRL_DEBUG_ARGS = 0,
  CTRL_DEBUG_STRAT_WELL_FORMED = 1,
  CTRL_DEBUG_BIT2 = 2
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
    string strBool(const bool val) const;
    string strDouble(const double val) const;
    string strBitVector(
      const vector<int>& bits,
      const string& doc) const;


  public:

    Control();

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
    unsigned char holdingLength() const;
    unsigned char cards() const;
    unsigned char westMin() const;
    unsigned char westMax() const;
    unsigned char westVacant() const;
    unsigned char eastMin() const;
    unsigned char eastMax() const;
    unsigned char eastVacant() const;

    unsigned goal() const;
    double expect() const;

    bool loop() const;
    bool loopAllRanks() const;

    const string& binaryInputDir() const;
    const string& binaryOutputDir() const;

    const string& inputFile() const; 
    const string& controlFile() const; 
    const string& text() const; 

    bool runFullPlays() const;
    bool runAdvancedNodes() const;
    bool runStrategyOptimizations() const;
    bool runRankComparisons() const;

    bool outputHolding() const; 
    bool outputBasicResults() const; 
    bool outputBit2() const; 

    bool debugArgs() const; 
    bool debugStratWellFormed() const;
    bool debugBit2() const; 

    const string& simpleDir() const;
    const string& constantDir() const;
    const string& roudiDir() const;

    unsigned numThreads() const;

    string str() const;
};

#endif
