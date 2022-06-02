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
              //               simple  new
       44282, // KJ976  :   68 ->  14   OK
       44286, // KJ975  :   54 ->  14   OK

       41012, // KQT8   :   23 ->  11   7
       41048, // KQT7   :   34 ->   8   6
       44288, // KJ97   :   90 ->  18   OK
       44300, // KJ96   :   36 ->  12   OK
       44408, // KJ86   :   25 ->   7   OK
       45260, // KT97   :   22 ->   8   OK
    })
  {
    store[10].insert(h);
  }

  for (auto h: 
    {
      132842, // KJ9765 :   68 ->  14   OK
      132846, // KJ9764 :   68 ->  14   OK
      132858, // KJ9754 :   54 ->  14   OK

       44288, // AQT86  :   84 ->   6   OK

      122696, // KQT96  :   22 ->   6    4
      122984, // KQT87  :   22 ->   2   OK
      123020, // KQT86  :  172 ->  32   16
      123032, // KQT85  :   34 ->  10    6
      123036, // KQT84  :   34 ->  10    6
      123128, // KQT76  :   38 ->  10    8
      123140, // KQT75  :   34 ->   8    6
      123144, // KQT74  :   34 ->   8    6
      123992, // KQ986  :   30 ->   6    7
      124100, // KQ976  :   23 ->   5   OK

      131768, // KJT86  :   73 ->  18   10
      131876, // KJT76  :   24 ->   5   OK
      132740, // KJ986  :   40 ->   8   OK
      132848, // KJ976  :  118 ->  18   15
      132860, // KJ975  :  894 ->  60   OK
      132864, // KJ974  :  160 ->  16   OK
      132896, // KJ965  :   39 ->  15   OK
      132900, // KJ964  :   36 ->  12   OK
      133184, // KJ875  :   66 ->  16   OK
      133220, // KJ865  :   27 ->   9   OK
      133224, // KJ864  :   25 ->   7   OK

      135764, // KT976  :   22 ->   6   OK
      135776, // KT975  :   82 ->  18   OK
      135780, // KT974  :   23 ->   5   OK
      136100, // KT875  :   34 ->  10   OK

      159104, // QJ975  :   63 ->  15   OK
      162020, // QT975  :   26 ->   8   OK

       44414, // AQT7   :  186 ->  12   OK
       45386, // AQ97   :   49 ->   7   OK
       44306, // AQT8   :   48 ->  12   OK

       53162, // AJT7   :   25 ->   5   OK

      123146, // KQT7   :   37 ->   9   OK

      131894, // KJT7   :   26 ->   2   OK
      132866, // KJ97   : 1832 ->  83   84
      132902, // KJ96   : 9619 -> 167  135
      132914, // KJ95   :   84 ->  18   OK
      132918, // KJ94   :   48 ->  12   OK
      133190, // KJ87   :  289 ->  27   24
      133226, // KJ86   : 1957 ->  35   OK
      133238, // KJ85   :  188 ->  12   OK
      133242, // KJ84   :  188 ->  12   OK
      133334, // KJ76   :   24 ->   6   OK

      135782, // KT97   :  248 ->  30   28
      135818, // KT96   :  700 ->  37   29
      135830, // KT95   :   25 ->   6   OK
      136106, // KT87   :   67 ->  14   13
      136142, // KT86   :   89 ->  12   11
      136154, // KT85   :   49 ->   7   OK
      136158, // KT84   :   49 ->   7   OK

      159110, // QJ97   :  187 ->  23   22
      159146, // QJ96   :  513 ->  31   29
      159434, // QJ87   :   58 ->  10   OK
      159470, // QJ86   :   72 ->   8   OK
      159482, // QJ85   :   25 ->   5   OK
      159486, // QJ84   :   25 ->   5   OK
      162026, // QT97   :   61 ->  13   OK
      162062, // QT96   :   99 ->  13   OK
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
  const unsigned char cards,
  const size_t holding3) const
{
  return (store[cards].find(static_cast<unsigned>(holding3)) != 
    store[cards].end());
}
