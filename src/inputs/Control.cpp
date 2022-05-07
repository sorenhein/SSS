#include <iostream>
#include <iomanip>
#include <sstream>

#if defined(__CYGWIN__) && (__GNUC__ < 8)
  #include <experimental/filesystem>
  using namespace std::experimental::filesystem;
#else
  #include <filesystem>
  using namespace std::filesystem;
#endif


#include "Control.h"
#include "Convert.h"
#include "parse.h"
#include "../const.h"

extern Convert convert;

// Undocumented expansion on my system.
#define INPUT_EXPANSION "../data/roudi"


Control::Control()
{
  Control::reset();
}


void Control::reset()
{
  fields.clear();
  fieldCounts.clear();

  Control::configure();
}


void Control::configure()
{
  fields =
  {
    // Example: ../data/simple
    { "SIMPLE_DIRECTORY", CORRESPONDENCE_STRING, CTRL_SIMPLE_DIRECTORY },
    // Example: ../data/constant
    { "CONSTANT_DIRECTORY", CORRESPONDENCE_STRING, 
      CTRL_CONSTANT_DIRECTORY },
    // Example: ../data/roudi
    { "ROUDI_DIRECTORY", CORRESPONDENCE_STRING, CTRL_ROUDI_DIRECTORY }
  };

  fieldCounts =
  {
    CTRL_STRINGS_SIZE,
    0,
    0,
    CTRL_INT_VECTORS_SIZE,
    0,
    CTRL_INTS_SIZE,
    CTRL_BOOLS_SIZE,
    CTRL_DOUBLES_SIZE
  };

  commands =
  {
    { "-n", "--north", CORRESPONDENCE_STRING, CTRL_NORTH, "", 
      "North's hand (x's are possible and encouraged)." },
    { "-s", "--south", CORRESPONDENCE_STRING, CTRL_SOUTH, "", 
      "South's hand (x's are possible and encouraged)." },
    { "-h", "--holding", CORRESPONDENCE_INT, CTRL_HOLDING, "-1",
      "Holding in dec/hex, e.g. 0x1295 (replaces -n and -s)." },
    { "-H", "--hlength", CORRESPONDENCE_INT, CTRL_HOLDING_LENGTH, "13",
      "Holding length." },
    { "-c", "--cards", CORRESPONDENCE_INT, CTRL_CARDS, "13",
      "Number of cards (default: 13)." },
    { "-l", "--lower", CORRESPONDENCE_INT, CTRL_WEST_MIN, "0",
      "Minimum West length (default: 0)." },
    { "-u", "--upper", CORRESPONDENCE_INT, CTRL_WEST_MAX, "-1",
      "Maximum West length (default: cards)." },
    { "-v", "--vacant", CORRESPONDENCE_INT, CTRL_WEST_VACANT, "-1",
      "Number of vacant places with West (default: cards)." },
    { "-L", "--Lower", CORRESPONDENCE_INT, CTRL_EAST_MIN, "0",
      "Minimum East length (default: 0)." },
    { "-U", "--Upper", CORRESPONDENCE_INT, CTRL_EAST_MAX, "-1",
      "Maximum East length (default: cards)." },
    { "-V", "--Vacant", CORRESPONDENCE_INT, CTRL_EAST_VACANT, "-1",
      "Number of vacant places with East (default: cards)." },

    { "-g", "--goal", CORRESPONDENCE_INT, CTRL_GOAL, "0",
      "Minimum number of tricks to target (default: 0)." },
    { "-e", "--expect", CORRESPONDENCE_DOUBLE, CTRL_EXPECT, "-1.",
      "Average tricks expected (e.g. from Roudinesco's book)." },

    { "-a", "--loop", CORRESPONDENCE_BOOL, CTRL_LOOP_FLAG, "yes",
      "Loop over combinations, not single-shot (default: true)." },
    { "-A", "--ranks", CORRESPONDENCE_BOOL, CTRL_ALL_RANKS_FLAG, "yes",
      "In loop (implies -a), process all ranks (default: true)." },

    { "-R", "--read", CORRESPONDENCE_STRING, CTRL_READ_BINARY_DIRECTORY, 
      "", "Read directory of binary files with rank information." },
    { "-W", "--write", CORRESPONDENCE_STRING, CTRL_WRITE_BINARY_DIRECTORY, 
      "", "Write directory of binary files with rank information." },

    { "-i", "--input", CORRESPONDENCE_STRING, CTRL_INPUT_FILE, "",
      "Input batch file with command-line strings." },
    { "-f", "--file", CORRESPONDENCE_STRING, CTRL_CONTROL_FILE, "",
      "Control file with further settings." },
    { "-p", "--print", CORRESPONDENCE_STRING, CTRL_PRINT_TEXT, "",
      "Text to print (e.g. Roudinesco page and number)." },
    { "-r", "--run", CORRESPONDENCE_BIT_VECTOR, CTRL_RUN, "0x3f",
      "Run control (default: 0x3f).  Bits:\n"
      "0x001: All N-S plays, including within ranks\n"
      "0x002: Advanced strategizing including ranges and voids\n"
      "0x004: Optimizations when calculating strategies\n"
      "0x008: Consider ranks when comparing strategies\n"
      "0x010: Describe winning tricks vectors verbally"
      "0x020: Describe winning tricks vectors verbally (manual)"
      },
    { "-o", "--output", CORRESPONDENCE_BIT_VECTOR, CTRL_OUTPUT, "0x1",
      "Output verbosity (default: 0x1).  Bits:\n"
      "0x001: Identification of holding\n"
      "0x002: Diagram, ranks, distributions and basic results" },
    { "-d", "--debug", CORRESPONDENCE_BIT_VECTOR, CTRL_DEBUG, "0x0",
      "Debug verbosity (default: 0x0).  Bits:\n"
      "0x001: Input arguments\n"
      "0x002: Check that final strategies are minimal and ordered" },
    { "-T", "--threads", CORRESPONDENCE_INT, CTRL_NUM_THREADS, "1",
      "Number of threads." }
  };

  entry.init(fieldCounts);
}


bool Control::parseCommandLine(
  int argc, 
  char * argv[])
{
  if (! entry.setCommandLineDefaults(commands))
  {
    cout << "Could not parse defaults\n";
    return false;
  }

  const string fullname = argv[0];
  path pathObj(fullname);
  const string basename = pathObj.filename().string();

  if (argc <= 1)
  {
    cout << entry.usage(basename, commands);
    return false;
  }

  vector<string> commandLine;
  for (unsigned i = 1; i < static_cast<unsigned>(argc); i++)
    commandLine.push_back(string(argv[i]));

  if (! entry.parseCommandLine(commandLine, commands))
  {
    cout << entry.usage(basename, commands);
    return false;
  }

  // Undocumented expansion of input file argument.
  const string cfile = entry.getString(CTRL_INPUT_FILE);
  if (cfile.size() == 2)
  {
    entry.getString(CTRL_INPUT_FILE) = INPUT_EXPANSION +
      cfile + ".txt";
  }

  if (! Control::readFile(entry.getString(CTRL_CONTROL_FILE)))
    cout << "Control file not specified.\n";

  if (! Control::completeHoldings())
    return false;

  Control::completeCounts();

  return true;
}


bool Control::readFile(const string& fname)
{
  if (! entry.readTagFile(fname, fields, fieldCounts))
    return false;

  return true;
}


bool Control::completeHoldings()
{
  if (Control::cards() < 1 || Control::cards() > MAX_CARDS)
  {
    cout << "Number of cards must be 1-" << MAX_CARDS << ".\n";
    return false;
  }

  string n = Control::north();
  string s = Control::south();
  int h = entry.getInt(CTRL_HOLDING);

  if (n.empty() != s.empty())
  {
    cout << "Either both or neither of -n and -s should be present.\n";
    return false;
  }


  if (n.empty() &&
      s.empty() &&
      h == -1 &&
      Control::inputFile().empty() &&
      ! Control::loop())
  {
    cout << "Need an input combination or an input file.\n";
    return false;
  }

  if ((! n.empty() || ! s.empty()) && h != -1)
  {
    cout << "-n/-s and -h are mutually exclusive.\n";
    return false;
  }

  if (Control::north() == "void")
  {
    entry.setString(CTRL_NORTH, "");
    n = "";
  }
  if (Control::south() == "void")
  {
    entry.setString(CTRL_SOUTH, "");
    s = "";
  }

  if (! n.empty() || ! s.empty())
  {
    unsigned hU;
    if (! convert.cards2holding(n, s, Control::cards(), hU))
    {
      cout << "Could not parse card strings into holding.\n";
      return false;
    }
    else
    {
      entry.setInt(CTRL_HOLDING, static_cast<int>(hU));
      entry.setBool(CTRL_LOOP_FLAG, false);
    }
  }
  else if (h != -1)
  {
    const unsigned hU = static_cast<unsigned>(h);
    if (! convert.holding2cards(hU, Control::cards(), n, s))
    {
      cout << "could not parse holding into card strings.\n";
      return false;
    }
    else
    {
      entry.setString(CTRL_NORTH, n);
      entry.setString(CTRL_SOUTH, s);
      entry.setBool(CTRL_LOOP_FLAG, false);
    }
  }
  return true;
}


void Control::completeCounts()
{
  const int c = entry.getInt(CTRL_CARDS);

  if (entry.getInt(CTRL_WEST_MAX) == -1)
    entry.setInt(CTRL_WEST_MAX, c);
  if (entry.getInt(CTRL_EAST_MAX) == -1)
    entry.setInt(CTRL_EAST_MAX, c);

  if (entry.getInt(CTRL_WEST_VACANT) == -1)
    entry.setInt(CTRL_WEST_VACANT, c);
  if (entry.getInt(CTRL_EAST_VACANT) == -1)
    entry.setInt(CTRL_EAST_VACANT, c);

  if (Control::westMin() > Control::westMax())
  {
    cout << "No possible West length left.\n";
    return;
  }

  if (Control::eastMin() > Control::eastMax())
  {
    cout << "No possible East length left.\n";
    return;
  }

  const int rest = entry.getInt(CTRL_CARDS) - 
    static_cast<int>(Control::north().length()) - 
    static_cast<int>(Control::south().length());

  if (entry.getInt(CTRL_WEST_MAX) > rest)
    entry.setInt(CTRL_WEST_MAX, rest);
  if (entry.getInt(CTRL_EAST_MAX) > rest)
    entry.setInt(CTRL_EAST_MAX, rest);

  if (Control::westMax() > Control::westVacant())
    entry.setInt(CTRL_WEST_MAX, rest);
  if (Control::eastMax() > Control::eastVacant())
    entry.setInt(CTRL_EAST_MAX, rest);

  if (entry.getInt(CTRL_WEST_MIN) < rest - entry.getInt(CTRL_EAST_MAX))
    entry.setInt(CTRL_WEST_MIN, rest - entry.getInt(CTRL_EAST_MAX));
  if (entry.getInt(CTRL_EAST_MIN) < rest - entry.getInt(CTRL_WEST_MAX))
    entry.setInt(CTRL_EAST_MIN, rest - entry.getInt(CTRL_WEST_MAX));
}


const string& Control::north() const
{
  return entry.getString(CTRL_NORTH);
}


const string& Control::south() const
{
  return entry.getString(CTRL_SOUTH);
}


unsigned Control::holding() const
{
  const int h = entry.getInt(CTRL_HOLDING);
  return (h == -1 ? UCHAR_NOT_SET : static_cast<unsigned>(h));
}


unsigned char Control::holdingLength() const
{
  const int h = entry.getInt(CTRL_HOLDING_LENGTH);
  return (h == -1 ? UCHAR_NOT_SET : static_cast<unsigned char>(h));
}


unsigned char Control::cards() const
{
  const int c = entry.getInt(CTRL_CARDS);
  return (c == -1 ? UCHAR_NOT_SET : static_cast<unsigned char>(c));
}


unsigned char Control::westMin() const
{
  return static_cast<unsigned char>(entry.getInt(CTRL_WEST_MIN));
}


unsigned char Control::westMax() const
{
  return static_cast<unsigned char>(entry.getInt(CTRL_WEST_MAX));
}


unsigned char Control::westVacant() const
{
  return static_cast<unsigned char>(entry.getInt(CTRL_WEST_VACANT));
}


unsigned char Control::eastMin() const
{
  return static_cast<unsigned char>(entry.getInt(CTRL_EAST_MIN));
}


unsigned char Control::eastMax() const
{
  return static_cast<unsigned char>(entry.getInt(CTRL_EAST_MAX));
}


unsigned char Control::eastVacant() const
{
  return static_cast<unsigned char>(entry.getInt(CTRL_EAST_VACANT));
}


unsigned Control::goal() const
{
  return static_cast<unsigned>(entry.getInt(CTRL_GOAL));
}


bool Control::loop() const
{
  return entry.getBool(CTRL_LOOP_FLAG);
}


bool Control::loopAllRanks() const
{
  return entry.getBool(CTRL_ALL_RANKS_FLAG);
}


double Control::expect() const
{
  return entry.getDouble(CTRL_EXPECT);
}


const string& Control::binaryInputDir() const
{
  return entry.getString(CTRL_READ_BINARY_DIRECTORY);
}


const string& Control::binaryOutputDir() const
{
  return entry.getString(CTRL_WRITE_BINARY_DIRECTORY);
}


const string& Control::inputFile() const
{
  return entry.getString(CTRL_INPUT_FILE);
}


const string& Control::controlFile() const
{
  return entry.getString(CTRL_CONTROL_FILE);
}


const string& Control::text() const
{
  return entry.getString(CTRL_PRINT_TEXT);
}


bool Control::runFullPlays() const
{
  // TODO Implement
  return (entry.getIntVector(CTRL_RUN)[CTRL_RUN_FULL_PLAYS] != 0);
}

bool Control::runAdvancedNodes() const
{
  return (entry.getIntVector(CTRL_RUN)[CTRL_RUN_ADVANCED_NODES] != 0);
}

bool Control::runStrategyOptimizations() const
{
  return (entry.getIntVector(CTRL_RUN)[CTRL_RUN_STRATEGY_OPT] != 0);
}

bool Control::runRankComparisons() const
{
  return (entry.getIntVector(CTRL_RUN)[CTRL_RUN_RANK_COMPARE] != 0);
}

bool Control::runVerbalTricks() const
{
  return (entry.getIntVector(CTRL_RUN)[CTRL_RUN_VERBAL_TRICKS] != 0);
}

bool Control::runVerbalTricksManually() const
{
  return (entry.getIntVector(CTRL_RUN)[CTRL_RUN_VERBAL_TRICKS_MANUAL] != 0);
}

bool Control::outputHolding() const
{
  return (entry.getIntVector(CTRL_OUTPUT)[CTRL_OUTPUT_HOLDING] != 0);
}

bool Control::outputBasicResults() const
{
  return (entry.getIntVector(CTRL_OUTPUT)[CTRL_OUTPUT_BASIC_RESULTS] != 0);
}


bool Control::outputBit2() const
{
  return (entry.getIntVector(CTRL_OUTPUT)[CTRL_OUTPUT_BIT2] != 0);
}


bool Control::debugArgs() const
{
  return (entry.getIntVector(CTRL_DEBUG)[CTRL_DEBUG_ARGS] != 0);
}


bool Control::debugStratWellFormed() const
{
  return (entry.getIntVector(CTRL_DEBUG)[CTRL_DEBUG_STRAT_WELL_FORMED] != 0);
}

bool Control::debugBit2() const
{
  return (entry.getIntVector(CTRL_DEBUG)[CTRL_DEBUG_BIT2] != 0);
}


const string& Control::simpleDir() const
{
  return entry.getString(CTRL_SIMPLE_DIRECTORY);
}


const string& Control::constantDir() const
{
  return entry.getString(CTRL_CONSTANT_DIRECTORY);
}


const string& Control::roudiDir() const
{
  return entry.getString(CTRL_ROUDI_DIRECTORY);
}


unsigned Control::numThreads() const
{
  return static_cast<unsigned>(entry.getInt(CTRL_NUM_THREADS));
}


string Control::strHex(const int val) const
{
  stringstream ss;
  ss << "0x" << hex << val;
  return ss.str();
}


string Control::strBool(const bool val) const
{
  return (val ? "yes" : "no");
}


string Control::strDouble(const double val) const
{
  if (val < 0.)
    return "-";

  stringstream ss;
  ss << setprecision(2) << val;
  return ss.str();
}

string Control::strBitVector(
  const vector<int>& bits,
  const string& doc) const
{
  // This is a kludge to pull apart the documentation text and to
  // put it together again including the values.

  vector<string> lines;
  tokenize(doc, lines, "\n");

  if (bits.size() < lines.size()-1)
  {
    cout << bits.size() << " VS " << lines.size() << "\n";
    return "";
  }

  stringstream ss;
  ss << lines[0];

  for (unsigned i = 1; i < lines.size(); i++)
  {
    const string set = (bits[i-1] ? "1" : "-");
    ss <<
      "\n" <<
      setw(13) << "" <<
      setw(8) << right << set << " " <<
      left << lines[i];
  }
  return ss.str();
}


string Control::str() const
{
  string s = "Parameters:\n";

  for (auto& cmd: commands)
  {
    string val = "";
    string doc = cmd.documentation;

    switch(cmd.corrType)
    {
      case CORRESPONDENCE_STRING:
        val = entry.getString(cmd.no);
        if (val.empty())
          val = "-";
        break;
      case CORRESPONDENCE_STRING_VECTOR:
      case CORRESPONDENCE_STRING_MAP:
      case CORRESPONDENCE_FLOAT_VECTOR:
      case CORRESPONDENCE_BOOL:
        val = Control::strBool(entry.getBool(cmd.no));
        break;
      case CORRESPONDENCE_INT_VECTOR:
        doc = Control::strBitVector(entry.getIntVector(cmd.no), doc);
        break;
      case CORRESPONDENCE_INT:
        if (cmd.no == CTRL_HOLDING)
          val = Control::strHex(entry.getInt(cmd.no));
        else
          val = to_string(entry.getInt(cmd.no));
        break;
      case CORRESPONDENCE_DOUBLE:
        val = Control::strDouble(entry.getDouble(cmd.no));
        break;
      case CORRESPONDENCE_SIZE:
      default:
        cout << "Shouldn't happen: " <<
          static_cast<unsigned>(cmd.corrType) << "\n";
        break;
    }

    stringstream ss;
    ss << 
      setw(2) << cmd.singleDash << " " <<
      setw(9) << cmd.doubleDash << " " <<
      setw(8) << right << val << "  " <<
      left << doc << "\n";
    s += ss.str();
  }
  return s + "\n";
}

