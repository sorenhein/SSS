#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>

#include "Plays.h"


const vector<unsigned> PLAY_CHUNK_SIZE =
{
    1, //  0
    2, //  1
    2, //  2
    2, //  3
    4, //  4
    6, //  5
   10, //  6
   15, //  7
   25, //  8
   40, //  9
   50, // 10
   70, // 11
   90, // 12
  110, // 13
  130, // 14
  150, // 15
};


Plays::Plays()
{
  Plays::reset();
}


Plays::~Plays()
{
}


void Plays::reset()
{
  nextNo = 0;
}


void Plays::resize(const unsigned cards)
{
  chunk = PLAY_CHUNK_SIZE[cards];
  playRecord.resize(chunk);
  nextNo = 0;
}


unsigned Plays::size() const
{
  return nextNo;
}


void Plays::log(
  const SidePosition sideIn,
  const unsigned leadIn,
  const unsigned lhoIn,
  const unsigned pardIn,
  const unsigned rhoIn,
  const bool leadCollapseIn,
  const bool lhoCollapseIn,
  const bool pardCollapseIn,
  const bool rhoCollapseIn,
  const unsigned holding3In,
  const bool rotateFlagIn)
{
  if (nextNo >= playRecord.size())
    playRecord.resize(playRecord.size() + chunk);

  playRecord[nextNo].update(
    sideIn, leadIn, lhoIn, pardIn, rhoIn,
    leadCollapseIn, lhoCollapseIn, pardCollapseIn, rhoCollapseIn,
    holding3In, rotateFlagIn);

  nextNo++;
}

