/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <fstream>

#include "SymmetryStore.h"


SymmetryStore::SymmetryStore()
{
  SymmetryStore::reset();
}


void SymmetryStore::reset()
{
  store.clear();
}


void SymmetryStore::resize(const unsigned cards)
{
  store.resize(cards+1);
}


void SymmetryStore::setManual()
{
  for (auto h: 
    {
      44282, // KJ976 / void: 68
      44286, // KJ975 / void: 54

      41012, // KQT8  / void: 23
      41048, // KQT7  / void: 34
      44288, // KJ97  / void: 90
      44300, // KJ96  / void: 36
      44408, // KJ86  / void: 25
      45260, // KT97  / void: 22
    })
  {
    store[10].insert(h);
  }

  for (auto h: 
    {
      132842, // KJ9765 / void:   68
      132846, // KJ9764 / void:   68
      132858, // KJ9754 / void:   54

       44288, // AQT86  / void:   84

      122696, // KQT96  / void:   22
      122984, // KQT87  / void:   22
      123020, // KQT86  / void:  172
      123032, // KQT85  / void:   34
      123036, // KQT84  / void:   34
      123128, // KQT76  / void:   38
      123140, // KQT75  / void:   34
      123144, // KQT74  / void:   34
      123992, // KQ986  / void:   30
      124100, // KQ976  / void:   23

      131768, // KJT86  / void:   73
      131876, // KJT76  / void:   24
      132740, // KJ986  / void:   40
      132848, // KJ976  / void:  118
      132860, // KJ975  / void:  984
      132864, // KJ974  / void:  160
      132896, // KJ965  / void:   39
      132900, // KJ964  / void:   36
      133184, // KJ875  / void:   66
      133220, // KJ865  / void:   27
      133224, // KJ864  / void:   25

      135764, // KT976  / void:   22
      135776, // KT975  / void:   82
      135780, // KT974  / void:   23
      136100, // KT875  / void:   34

      159104, // QJ975  / void:   63
      162020, // QT975  / void:   26

       44414, // AQT7   / void:  186
       45386, // AQ97   / void:   49
       44306, // AQT8   / void:   48

       53162, // AJT7   / void:   25

      123146, // KQT7   / void:   37

      131894, // KJT7   / void:   26
      132866, // KJ97   / void: 1832
      132902, // KJ96   / void: 9619
      132914, // KJ95   / void:   84
      132918, // KJ94   / void:   48
      133190, // KJ87   / void:  289
      133226, // KJ86   / void: 1957
      133238, // KJ85   / void:  188
      133242, // KJ84   / void:  188
      133334, // KJ76   / void:   24

      135782, // KT97   / void:  248
      135818, // KT96   / void:  700
      135830, // KT95   / void:   25
      136106, // KT87   / void:   67
      136142, // KT86   / void:   89
      136154, // KT85   / void:   49
      136158, // KT84   / void:   49

      159110, // QJ97   / void:  187
      159416, // QJ96   / void:  513
      159434, // QJ87   / void:   58
      159470, // QJ86   / void:   72
      159482, // QJ85   / void:   25
      159486, // QJ84   / void:   25
      162026, // QT97   / void:   61
      162062, // QT96   / void:   99
    })
  {
    store[11].insert(h);
  }
}


void SymmetryStore::readFile(const string& filename)
{
  ifstream is(filename, ios::in, ios::binary);

  // Determine the file length
  is.seekg(0, ios_base::end);
  size_t size = static_cast<size_t>(is.tellg());
  is.seekg(0, ios_base::beg);

  vector<unsigned> v;
  v.resize(size / sizeof(unsigned));

  is.read(reinterpret_cast<char *>(&v[0]), size);
  is.close();

  for (unsigned i = 0; i < v.size(); i += 2)
    store[v[i]].insert(v[i+1]);
}


void SymmetryStore::writeFile(const string& filename) const
{
  auto os = fstream(filename, ios::out | ios::binary);

  for (unsigned cards = 1; cards < store.size(); cards++)
  {
    const size_t s = store[cards].size();
    if (s == 0)
      continue;

    vector<unsigned> v(2*s);
    unsigned i = 0;
    for (auto holding3: store[cards])
    {
      v[i] = cards;
      v[i+1] = holding3;
      i += 2;
    }

    os.write(reinterpret_cast<const char *>(&v[0]),
      2*s * sizeof(unsigned) / sizeof(char));
  }
}


bool SymmetryStore::symmetrize(
  const unsigned card,
  const unsigned holding3) const
{
  return (store[card].find(holding3) != store[card].end());
}
