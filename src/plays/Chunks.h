/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/


#ifndef SSS_CHUNKS_H
#define SSS_CHUNKS_H

struct ChunkEntry
{
  unsigned lead;
  unsigned lho;
  unsigned pard;
  unsigned rho;
};

// It's not that important to get these exactly right, but this works
// For efficiency the lists of nodes are not cleared (for a given
// number of cards), and only a pointer to the last used element
// and its number are changed.  The lists themselves are reset once
// for each number of cards.  Therefore the lists will generally
// have more entries than are used, and have to iterate up to
// the stored end iterator and not up to end().

const vector<ChunkEntry> CHUNK_SIZE =
{
  {  1,   1,   1,   1}, //  0
  {  1,   1,   1,   1}, //  1
  {  1,   1,   1,   1}, //  2
  {  1,   1,   1,   1}, //  3
  {  4,   6,   6,  12}, //  4
  {  4,   8,   8,  16}, //  5
  {  6,  12,  12,  16}, //  6
  {  6,  12,  16,  32}, //  7
  {  8,  12,  20,  40}, //  8
  { 10,  18,  24,  72}, //  9
  { 10,  20,  32, 100}, // 10
  { 12,  24,  48, 144}, // 11
  { 12,  28,  60, 208}, // 12
  { 12,  36,  80, 300}, // 13
  { 12,  42, 100, 432}, // 14
  { 14,  50, 128, 600}  // 15
};

#endif
