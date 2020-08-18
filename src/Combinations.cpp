#include <iostream>
#include <iomanip>
#include <sstream>

#include "Combinations.h"
#include "Ranks.h"


Combinations::Combinations()
{
  Combinations::reset();
}


Combinations::~Combinations()
{
}


void Combinations::reset()
{
  maxCards = 0;
  combinations.clear();
  uniques.clear();
  counts.clear();
}


void Combinations::resize(const int maxCardsIn)
{
  maxCards = maxCardsIn;

  combinations.resize(maxCardsIn+1);
  uniques.resize(maxCardsIn+1);

  // There are three combinations with 1 card: It may be with
  // North, South or the opponents.
  int numCombinations = 3;

  for (int cards = 1; cards <= maxCards; cards++)
  {
    combinations[cards].resize(numCombinations);
    numCombinations *= 3;
    uniques[cards].clear();
  }

  counts.resize(maxCardsIn+1);
  for (unsigned cards = 0; cards < counts.size(); cards++)
  {
    counts[cards].total = 0;
    counts[cards].unique = 0;
  }
}


void Combinations::runUniques(const int cards)
{
  vector<CombEntry>& combs = combinations[cards];
  vector<int>& uniqs = uniques[cards];
  Ranks ranks;

  ranks.resize(cards);

  for (int holding = 0; holding < static_cast<int>(combs.size()); holding++)
  {
    ranks.set(holding, cards, combs[holding]);
// cout << ranks.str();

    counts[cards].total++;
    if (holding == combs[holding].canonicalHolding)
    {
      combs[holding].canonicalIndex = uniques.size();
      uniqs.push_back(holding);

      counts[cards].unique++;
    }
  }
}


string Combinations::strUniques(const int cards) const
{
  int cmin, cmax;
  if (cards == 0)
  {
    cmin = 1;
    cmax = maxCards;
  }
  else
  {
    cmin = cards;
    cmax = cards;
  }

  stringstream ss;
  ss <<
    setw(5) << "Cards" <<
    setw(9) << "Total" <<
    setw(9) << "Unique" <<
    setw(9) << "%" <<
    "\n";

  for (int c = cmin; c <= cmax; c++)
  {
    if (counts[c].total == 0)
      continue;

    ss <<
      setw(5) << c <<
      setw(9) << counts[c].total <<
      setw(9) << counts[c].unique <<
      setw(8) << fixed << setprecision(2) <<
        (100. * counts[c].unique) / counts[c].total <<
        "%\n";
  }

  return ss.str() + "\n";
}

