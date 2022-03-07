#!perl

use strict;
use warnings;

# Check two generations of verbal strategies against each other.

my $file = shift;
open my $fh, '<', $file or die $!;

my $lno = 0;
my ($cards, $holding);
my (@strats, @vstrats);
my (@shead, @vhead);

my $countHeadText = 0;
my $countNum = 0;
my $countText = 0;
my $countSame = 0;

my (@stext, @vtext);
my @countKnown;

# Example: 7/1692, K98 / - missing AHHH.
push @{$stext[0]}, 'East has the top at most doubleton';
push @{$stext[0]}, 'West has the top at most doubleton';

push @{$vtext[0]}, 'East has the singleton honor';
push @{$vtext[0]}, 'The suit splits 2=2';
push @{$vtext[0]}, 'West has the singleton honor';

# Example: 8 / 5076, K987 / - missing AHHH.
push @{$stext[1]}, 'East has the top at most doubleton';
push @{$stext[1]}, 'Neither opponent is void';
push @{$stext[1]}, 'West has the top at most doubleton';

push @{$vtext[1]}, 'East has the singleton honor';
push @{$vtext[1]}, 'Neither opponent is void';
push @{$vtext[1]}, 'The suit splits 2=2';
push @{$vtext[1]}, 'West has the singleton honor';

# Example: 9 / 18978, JT96 / - missing HHHxxx.
push @{$stext[2]}, 'East has a singleton honor';
push @{$stext[2]}, 'The suit splits 2-3 either way; or';
push @{$stext[2]}, 'West has a singleton honor';

push @{$vtext[2]}, 'The suit splits 2-3 either way';
push @{$vtext[2]}, 'West has between 1 and 2 tops; or';

# Example: 9 / 18180, Q76 / 98 missing HHhh.
push @{$stext[3]}, 'East has doubleton honors (HH)';
push @{$stext[3]}, 'East has one top at most doubleton';
push @{$stext[3]}, 'West has doubleton honors (HH)';

push @{$vtext[3]}, 'The suit splits between 2=2 and 3=1, and East has at least one top';
push @{$vtext[3]}, 'West has doubleton honors (HH)';

# Example: 9 / 17967, Q6 / J87 missing HHhh.
push @{$stext[4]}, 'East has doubleton honors (HH)';
push @{$stext[4]}, 'East has one top at most doubleton';
push @{$stext[4]}, 'West has a small singleton';
push @{$stext[4]}, 'West has doubleton honors (HH)';
push @{$stext[4]}, 'West is void';

push @{$vtext[4]}, 'East has the tops';
push @{$vtext[4]}, 'The suit splits between 2=2 and 3=1, and East has at least one top; or';
push @{$vtext[4]}, 'West has doubleton honors (HH)';

# Example: 9 / 17964, Q76 / J8 mising HHhh.
push @{$stext[5]}, 'West has a small singleton';
push @{$stext[5]}, 'West has at least 2 cards';
push @{$stext[5]}, 'West is void';

push @{$vtext[5]}, 'East has the tops';
push @{$vtext[5]}, 'West has at least 2 cards; or';

# Example: 9 / 17730, QJ76 / - missing HHxxx.
push @{$stext[6]}, 'East has a singleton honor';
push @{$stext[6]}, 'East has doubleton honors (HH)';
push @{$stext[6]}, 'The suit splits 2-3 either way';
push @{$stext[6]}, 'West has a singleton honor';
push @{$stext[6]}, 'West has doubleton honors (HH)';

push @{$vtext[6]}, 'East has doubleton honors (HH)';
push @{$vtext[6]}, 'The suit splits 2-3 either way; or';
push @{$vtext[6]}, 'West has doubleton honors (HH)';
push @{$vtext[6]}, 'West has exactly one top';

# Example: 9 / 15282, K876 / - missing AHHHH.
push @{$stext[7]}, 'East has Hx(x)';
push @{$stext[7]}, 'East has the top at most doubleton';
push @{$stext[7]}, 'West has Hx(x)';
push @{$stext[7]}, 'West has the top at most doubleton';

push @{$vtext[7]}, 'East has the top at most doubleton';
push @{$vtext[7]}, 'The suit splits 2-3 either way';
push @{$vtext[7]}, 'West has the top at most doubleton';

# Example: 9 / 13842, KQ76 / - missing Axxxx.
push @{$stext[8]}, 'East has Hx(x)';
push @{$stext[8]}, 'East has the singleton honor';
push @{$stext[8]}, 'West has Hx(x)';
push @{$stext[8]}, 'West has the singleton honor';

push @{$vtext[8]}, 'East has the singleton honor';
push @{$vtext[8]}, 'The suit splits 2-3 either way';
push @{$vtext[8]}, 'West has the singleton honor';

# Example: 9 / 13362, KQJ6 / - missing Axxxx.
push @{$stext[9]}, 'East has Hx(x)';
push @{$stext[9]}, 'West has Hx(x)';

push @{$vtext[9]}, 'The suit splits 2-3 either way';

# Example: 9 / 6303, A6 / J87 missing HHhh.
push @{$stext[10]}, 'East has doubleton honors (HH)';
push @{$stext[10]}, 'West has doubleton honors (HH)';
push @{$stext[10]}, 'West has one top at most doubleton';

push @{$vtext[10]}, 'East has doubleton honors (HH)';
push @{$vtext[10]}, 'The suit splits between 1=3 and 2=2, and West has at least one top';

# Example: 9 / 6150, AT6 / J7 missing HHxx.
push @{$stext[11]}, 'East has one top at most doubleton';
push @{$stext[11]}, 'West has the tops';

push @{$vtext[11]}, 'West has at least 2 cards, and West has at least one top';

# Example: 9 / 5817, A6 / Q87 missing KHHH.
push @{$stext[12]}, 'East has the singleton honor';
push @{$stext[12]}, 'The suit splits between 1=3 and 2=2; or';
push @{$stext[12]}, 'West is void';

push @{$vtext[12]}, 'East has the singleton honor';
push @{$vtext[12]}, 'West has at most a doubleton';

# Example: 9 / 4125, A6 / KJ7 missing Qxxx.
push @{$stext[13]}, 'East has the top at most doubleton';
push @{$stext[13]}, 'West has the top at most doubleton';
push @{$stext[13]}, 'West is void; or';

push @{$vtext[13]}, 'East has the singleton honor';
push @{$vtext[13]}, 'The suit splits 2=2';
push @{$vtext[13]}, 'West has the singleton honor';
push @{$vtext[13]}, 'West is void';

# Example: 9 / 3966, AT6 / KJ missing Qxxx.
push @{$stext[14]}, 'East has the top at most doubleton';
push @{$stext[14]}, 'East is void';
push @{$stext[14]}, 'West has the top at most doubleton';

push @{$vtext[14]}, 'East has the singleton honor';
push @{$vtext[14]}, 'East is void';
push @{$vtext[14]}, 'The suit splits 2=2';
push @{$vtext[14]}, 'West has the singleton honor';

# Example: 9 / 1779, AKT6 / J missing Qxxx.
push @{$stext[15]}, 'East has the top at most doubleton';
push @{$stext[15]}, 'West has at most 3 cards';
push @{$stext[15]}, 'West has the top at most doubleton';

push @{$vtext[15]}, 'East has the singleton honor';
push @{$vtext[15]}, 'The suit splits 2=2';
push @{$vtext[15]}, 'West has at most 3 cards';
push @{$vtext[15]}, 'West has the singleton honor';

# Example: 10 / 56937, JT95 / 6 missing HHHxx.
push @{$stext[16]}, 'East has a small singleton';
push @{$stext[16]}, 'East is void';
push @{$stext[16]}, 'The suit splits 2-3 either way; or';
push @{$stext[16]}, 'West has a singleton honor';

push @{$vtext[16]}, 'The suit splits between 1=4 and 3=2, and West has at least one top; or';
push @{$vtext[16]}, 'West has a small doubleton';
push @{$vtext[16]}, 'West has the tops';

# Example: 1 / 56934, JT965 / - missing HHHxx.
push @{$stext[17]}, 'East has a singleton honor';
push @{$stext[17]}, 'Neither opponent is void';
push @{$stext[17]}, 'The suit splits 2-3 either way; or';
push @{$stext[17]}, 'West has a singleton honor';

push @{$vtext[17]}, 'Neither opponent is void';
push @{$vtext[17]}, 'The suit splits 2-3 either way';
push @{$vtext[17]}, 'West has between 1 and 2 tops; or';

# Example: 10 / 54540, Q765 / 98 missing HHhh.
push @{$stext[18]}, 'East has doubleton honors (HH)';
push @{$stext[18]}, 'East has one top at most doubleton';
push @{$stext[18]}, 'Neither opponent is void';
push @{$stext[18]}, 'West has doubleton honors (HH)';

push @{$vtext[18]}, 'Neither opponent is void';
push @{$vtext[18]}, 'The suit splits between 2=2 and 3=1, and East has at least one top';
push @{$vtext[18]}, 'West has doubleton honors (HH)';

# Example: 10 / 53931, Q5 / J76 mising HHxxx.
push @{$stext[19]}, 'East has a small singleton';
push @{$stext[19]}, 'East has both tops at most tripleton';
push @{$stext[19]}, 'East is void';
push @{$stext[19]}, 'West has a singleton honor';
push @{$stext[19]}, 'West has a small singleton';
push @{$stext[19]}, 'West has both tops at most tripleton';
push @{$stext[19]}, 'West is void';

push @{$vtext[19]}, 'East has the tops';
push @{$vtext[19]}, 'West has a singleton honor';
push @{$vtext[19]}, 'West has the tops';

# Example: 10 / 53928, Q65 / J7 missing HHxxx.
push @{$stext[20]}, 'East has a singleton honor';
push @{$stext[20]}, 'East has doubleton honors (HH)';
push @{$stext[20]}, 'West has a small doubleton';
push @{$stext[20]}, 'West has a small singleton';
push @{$stext[20]}, 'West has the tops; or';
push @{$stext[20]}, 'West is void';

push @{$vtext[20]}, 'East has a singleton honor';
push @{$vtext[20]}, 'East has the tops';
push @{$vtext[20]}, 'West has the tops';

# Example: 10 / 53928, Q65 / J7 missing HHxxx.
push @{$stext[21]}, 'East has a singleton honor';
push @{$stext[21]}, 'East has an honor doubleton (Hx)';
push @{$stext[21]}, 'East has the tops; or';
push @{$stext[21]}, 'East is void';
push @{$stext[21]}, 'West has doubleton honors (HH)';

push @{$vtext[21]}, 'East has the tops';
push @{$vtext[21]}, 'East is void';
push @{$vtext[21]}, 'The suit splits between 3=2 and 4=1, and East has at least one top; or';
push @{$vtext[21]}, 'West has doubleton honors (HH)';

# Example: 10 / 53910, Q65 / J8 missing HHhh7.
push @{$stext[22]}, 'East has a singleton honor';
push @{$stext[22]}, 'East has an honor doubleton (Hx)';
push @{$stext[22]}, 'East has the tops; or';
push @{$stext[22]}, 'East is void';
push @{$stext[22]}, 'West has doubleton honors (HH)';

push @{$vtext[22]}, 'East has the tops; or';
push @{$vtext[22]}, 'East is void';
push @{$vtext[22]}, 'The suit splits between 3=2 and 4=1, and East has at least one top';
push @{$vtext[22]}, 'West has doubleton honors (HH)';

# Example: 10 / 53904, Q5 / J876 missing HHhh.
push @{$stext[23]}, 'East has doubleton honors (HH)';
push @{$stext[23]}, 'East has one top at most doubleton';
push @{$stext[23]}, 'West has a small singleton';
push @{$stext[23]}, 'West has at most 3 cards';
push @{$stext[23]}, 'West has doubleton honors (HH)';

push @{$vtext[23]}, 'The suit splits between 2=2 and 3=1, and East has at least one top';
push @{$vtext[23]}, 'West has a small singleton';
push @{$vtext[23]}, 'West has at most 3 cards';
push @{$vtext[23]}, 'West has doubleton honors (HH)';

# Example: 10 / 53770, Q9 / J765 missing HHT8.
push @{$stext[24]}, 'East has doubleton honors (HH)';
push @{$stext[24]}, 'East has one top at most doubleton';
push @{$stext[24]}, 'West has a small singleton';
push @{$stext[24]}, 'West has at most 3 cards';
push @{$stext[24]}, 'West has doubleton honors (HH)';

push @{$vtext[24]}, 'East has both tops at most tripleton';
push @{$vtext[24]}, 'The suit splits between 2=2 and 3=1, and East has at least one top; or';
push @{$vtext[24]}, 'West has at most 3 cards';
push @{$vtext[24]}, 'West has doubleton honors (HH)';

# Example: 10 / 53208, QJ65 / - missing HHxxxx.
push @{$stext[25]}, 'East has doubleton honors (HH)';
push @{$stext[25]}, 'The suit splits between 1=5 and 3=3, and Either opponent has at least one top';
push @{$stext[25]}, 'West has doubleton honors (HH)';

push @{$vtext[25]}, 'East has doubleton honors (HH)';
push @{$vtext[25]}, 'The suit splits between 1=5 and 3=3, and West has at least one top';
push @{$vtext[25]}, 'The suit splits between 3=3 and 5=1, and East has at least one top; or';
push @{$vtext[25]}, 'West has doubleton honors (HH)';
push @{$vtext[25]}, 'West has exactly one top; or';

# Example: 10 / 53199, QJ65 / 7 missing HHxxx.
push @{$stext[26]}, 'East has a small doubleton';
push @{$stext[26]}, 'East has doubleton honors (HH)';
push @{$stext[26]}, 'The suit splits 2-3 either way; or';
push @{$stext[26]}, 'West has doubleton honors (HH)';
push @{$stext[26]}, 'West has one top at most doubleton';
push @{$stext[26]}, 'West has the tops';

push @{$vtext[26]}, 'East has doubleton honors (HH)';
push @{$vtext[26]}, 'The suit splits 2-3 either way';
push @{$vtext[26]}, 'The suit splits between 1=4 and 2=3, and West has at least one top; or';
push @{$vtext[26]}, 'West has the tops';

# Example: 10 / 53190, QJ765 / - missing HHxxx.
push @{$stext[27]}, 'East has a singleton honor';
push @{$stext[27]}, 'East has doubleton honors (HH)';
push @{$stext[27]}, 'Neither opponent is void';
push @{$stext[27]}, 'The suit splits 2-3 either way';
push @{$stext[27]}, 'West has a singleton honor';
push @{$stext[27]}, 'West has doubleton honors (HH)';

push @{$vtext[27]}, 'East has doubleton honors (HH)';
push @{$vtext[27]}, 'Neither opponent is void';
push @{$vtext[27]}, 'The suit splits 2-3 either way; or';
push @{$vtext[27]}, 'West has doubleton honors (HH)';
push @{$vtext[27]}, 'West has exactly one top';

# Example: 10 / 53181, QJ65 / 8 missing HHhh7.
push @{$stext[28]}, 'East has a small doubleton';
push @{$stext[28]}, 'East has doubleton honors (HH)';
push @{$stext[28]}, 'The suit splits 2-3 either way; or';
push @{$stext[28]}, 'West has doubleton honors (HH)';
push @{$stext[28]}, 'West has one top at most doubleton';
push @{$stext[28]}, 'West has the tops';

push @{$vtext[28]}, 'East has doubleton honors (HH)';
push @{$vtext[28]}, 'The suit splits 2-3 either way';
push @{$vtext[28]}, 'The suit splits between 1=4 and 2=3, and West has at least one top';
push @{$vtext[28]}, 'West has the tops; or';

# Example: 10 / 45873, K765 / 8 missing AHHHH.
push @{$stext[29]}, 'East has Hx(x)';
push @{$stext[29]}, 'West has Hx(x)';
push @{$stext[29]}, 'West has the top';

push @{$vtext[29]}, 'The suit splits 2-3 either way';
push @{$vtext[29]}, 'West has the top';

# Example: 10 / 45846, K8765 / - missing AHHHH.
push @{$stext[30]}, 'East has Hx(x)';
push @{$stext[30]}, 'East has the top at most doubleton';
push @{$stext[30]}, 'Neither opponent is void';
push @{$stext[30]}, 'West has Hx(x)';
push @{$stext[30]}, 'West has the top at most doubleton';

push @{$vtext[30]}, 'East has the top at most doubleton';
push @{$vtext[30]}, 'Neither opponent is void';
push @{$vtext[30]}, 'The suit splits 2-3 either way';
push @{$vtext[30]}, 'West has the top at most doubleton';

# Example: 10 / 41535, KQ65 / 7 missing Axxxx.
push @{$stext[31]}, 'East has Hx(x)';
push @{$stext[31]}, 'West has Hx(x)';
push @{$stext[31]}, 'West has the top at most doubleton';

push @{$vtext[31]}, 'The suit splits 2-3 either way';
push @{$vtext[31]}, 'West has the top at most doubleton';

# Example: 10 / 41526, KQ765 / - missing Axxxx.
push @{$stext[32]}, 'East has Hx(x)';
push @{$stext[32]}, 'East has the singleton honor';
push @{$stext[32]}, 'Neither opponent is void';
push @{$stext[32]}, 'West has Hx(x)';
push @{$stext[32]}, 'West has the singleton honor';

push @{$vtext[32]}, 'East has the singleton honor';
push @{$vtext[32]}, 'Neither opponent is void';
push @{$vtext[32]}, 'The suit splits 2-3 either way';
push @{$vtext[32]}, 'West has the singleton honor';

# Example: 10 / 40089, KQJ5 / 6 missing Axxxx.
push @{$stext[33]}, 'East has Hx(x)';
push @{$stext[33]}, 'West has Hx(x)';
push @{$stext[33]}, 'West has the singleton honor';

push @{$vtext[33]}, 'The suit splits 2-3 either way';
push @{$vtext[33]}, 'West has the singleton honor';

# Example: 10 / 40086, KQJ65 / - missing Axxxx.
push @{$stext[34]}, 'East has Hx(x)';
push @{$stext[34]}, 'Neither opponent is void';
push @{$stext[34]}, 'West has Hx(x)';

push @{$vtext[34]}, 'Neither opponent is void';
push @{$vtext[34]}, 'The suit splits 2-3 either way';

# Example: 10 / 18912, A5 / J876 missing HHhh.
push @{$stext[35]}, 'East has doubleton honors (HH)';
push @{$stext[35]}, 'Neither opponent is void';
push @{$stext[35]}, 'West has doubleton honors (HH)';
push @{$stext[35]}, 'West has one top at most doubleton';

push @{$vtext[35]}, 'East has doubleton honors (HH)';
push @{$vtext[35]}, 'Neither opponent is void';
push @{$vtext[35]}, 'The suit splits between 1=3 and 2=2, and West has at least one top';

# Example: 10 / 18450, AT65 / J7 missing HHxx.
push @{$stext[36]}, 'East has one top at most doubleton';
push @{$stext[36]}, 'West has at least 1 cards';
push @{$stext[36]}, 'West has both tops at most tripleton';

push @{$vtext[36]}, 'The suit splits between 2=2 and 3=1, and West has at least one top';
push @{$vtext[36]}, 'West has at least 1 cards';

# Example: 10 / 18251, AT98 / J missing HHxxx.
push @{$stext[37]}, 'East has a small doubleton';
push @{$stext[37]}, 'East has doubleton honors (HH)';
push @{$stext[37]}, 'West has doubleton honors (HH)';
push @{$stext[37]}, 'West has one top at most doubleton';

push @{$vtext[37]}, 'East has a small doubleton';
push @{$vtext[37]}, 'East has doubleton honors (HH)';
push @{$vtext[37]}, 'The suit splits between 1=4 and 2=3, and West has at least one top';

# Example: 10 / 17573, AJT9 / 6 missing HHhh5.
push @{$stext[38]}, 'East has a small doubleton';
push @{$stext[38]}, 'East has doubleton honors (HH)';
push @{$stext[38]}, 'West has doubleton honors (HH)';
push @{$stext[38]}, 'West has one top at most doubleton';

push @{$vtext[38]}, 'East has doubleton honors (HH)';
push @{$vtext[38]}, 'The suit splits between 1=4 and 2=3, and West has at least one top; or';
push @{$vtext[38]}, 'West has both tops at most tripleton';

# Example: 10 / 11646, AJ65 / K7 missing Qxxx.
push @{$stext[39]}, 'East has the top at most doubleton';
push @{$stext[39]}, 'West has at least 1 cards';
push @{$stext[39]}, 'West has the top at most doubleton';

push @{$vtext[39]}, 'East has the singleton honor';
push @{$vtext[39]}, 'The suit splits 2=2';
push @{$vtext[39]}, 'West has at least 1 cards';
push @{$vtext[39]}, 'West has the singleton honor';

# Example: 10 / 5343, AKT5 / J missing Qxxxx.
push @{$stext[40]}, 'East has Hx(x)';
push @{$stext[40]}, 'East has the top at most doubleton';
push @{$stext[40]}, 'West has Hx(x)';
push @{$stext[40]}, 'West has the top at most doubleton';
push @{$stext[40]}, 'West is void';

push @{$vtext[40]}, 'East has the top at most doubleton';
push @{$vtext[40]}, 'The suit splits 2-3 either way';
push @{$vtext[40]}, 'West has the top at most doubleton';
push @{$vtext[40]}, 'West is void';

# Example: 11 / 174984, T9874 / - missing HHHHxx.
push @{$stext[41]}, 'East has a singleton honor';
push @{$stext[41]}, 'The suit splits 2-4 or better either way; or';
push @{$stext[41]}, 'The suit splits 3=3';
push @{$stext[41]}, 'West has a singleton honor';

push @{$vtext[41]}, 'The suit splits 2-4 or better either way';
push @{$vtext[41]}, 'The suit splits 3=3';
push @{$vtext[41]}, 'West has between 1 and 3 tops; or';

# Example: 11 / 171786, J4 / T965 missing HHHxx.
push @{$stext[42]}, 'East has a singleton honor';
push @{$stext[42]}, 'The suit splits 2-3 either way; or';
push @{$stext[42]}, 'West has a singleton honor';
push @{$stext[42]}, 'West is void; or';

push @{$vtext[42]}, 'The suit splits 2-3 either way';
push @{$vtext[42]}, 'West has between 1 and 2 tops; or';
push @{$vtext[42]}, 'West is void';

# Example: 11 / 171540, J954 / T6 missing HHHxx.
push @{$stext[43]}, 'East has a singleton honor';
push @{$stext[43]}, 'East has a small singleton';
push @{$stext[43]}, 'East has the tops; or';
push @{$stext[43]}, 'East is void';
push @{$stext[43]}, 'The suit splits 2-3 either way; or';

push @{$vtext[43]}, 'East has the tops';
push @{$vtext[43]}, 'West has at least 2 cards; or';

# Example: 11 / 171540, J954 / T6 missing HHHxx.
push @{$stext[44]}, 'East has a singleton honor';
push @{$stext[44]}, 'East is void';
push @{$stext[44]}, 'The suit splits 2-3 either way; or';
push @{$stext[44]}, 'West has a singleton honor';

push @{$vtext[44]}, 'East is void';
push @{$vtext[44]}, 'The suit splits 2-3 either way';
push @{$vtext[44]}, 'West has between 1 and 2 tops; or';

# Example: 11 / 171270, JT54 / 76 missing HHHxx.
push @{$stext[45]}, 'East has a small singleton';
push @{$stext[45]}, 'East is void';
push @{$stext[45]}, 'The suit splits 2-3 either way';

push @{$vtext[45]}, 'The suit splits 2-3 either way; or';
push @{$vtext[45]}, 'West has the tops';

# Example: 11 / 170823, JT94 / 5 missing HHHxxx.
push @{$stext[46]}, 'East has doubleton honors (HH)';
push @{$stext[46]}, 'The suit splits 3=3; or';
push @{$stext[46]}, 'West has doubleton honors (HH)';
push @{$stext[46]}, 'West has one top at most doubleton';
push @{$stext[46]}, 'West has the tops';

push @{$vtext[46]}, 'The suit splits between 1=5 and 3=3, and West has at least one top; or';
push @{$vtext[46]}, 'The suit splits between 3=3 and 4=2, and West has at most 1 tops; or';
push @{$vtext[46]}, 'West has the tops';

# Example: 11 / 170820, JT954 / - missing HHHxxx.
push @{$stext[47]}, 'East has a singleton honor';
push @{$stext[47]}, 'East has doubleton honors (HH)';
push @{$stext[47]}, 'The suit splits 2-4 or better either way';
push @{$stext[47]}, 'The suit splits 3=3';
push @{$stext[47]}, 'West has a singleton honor';
push @{$stext[47]}, 'West has doubleton honors (HH)';

push @{$vtext[47]}, 'East has doubleton honors (HH)';
push @{$vtext[47]}, 'The suit splits 2-4 or better either way; or';
push @{$vtext[47]}, 'The suit splits 3=3';
push @{$vtext[47]}, 'West has between 1 and 2 tops';
push @{$vtext[47]}, 'West has doubleton honors (HH)';

# Example: 11 / 170814, JT94 / 65 missing HHHxx.
push @{$stext[48]}, 'East has a singleton honor';
push @{$stext[48]}, 'East has a small singleton';
push @{$stext[48]}, 'East is void';
push @{$stext[48]}, 'The suit splits 2-3 either way; or';
push @{$stext[48]}, 'West has a singleton honor';

push @{$vtext[48]}, 'West has a small doubleton';
push @{$vtext[48]}, 'West has at least one top';

# Example: 11 / 170799, JT94 / 7 missing HHH8xx.
push @{$stext[49]}, 'East has doubleton honors (HH)';
push @{$stext[49]}, 'The suit splits 3=3; or';
push @{$stext[49]}, 'West has doubleton honors (HH)';
push @{$stext[49]}, 'West has one top at most doubleton';
push @{$stext[49]}, 'West has the tops';

push @{$vtext[49]}, 'The suit splits between 1=5 and 3=3, and West has at least one top; or';
push @{$vtext[49]}, 'The suit splits between 3=3 and 4=2, and West has at most 1 tops';
push @{$vtext[49]}, 'West has the tops; or';

# Example: 11 / 163890, Q654 / 87 missing HHhhh.
push @{$stext[50]}, 'East has a small doubleton';
push @{$stext[50]}, 'The suit splits 2-3 either way; or';
push @{$stext[50]}, 'West has doubleton honors (HH)';
push @{$stext[50]}, 'West has the tops';

push @{$vtext[50]}, 'The suit splits 2-3 either way';
push @{$vtext[50]}, 'West has the tops';

# Example: 11 / 163647, Q654 / 987 missing HHhh.
push @{$stext[51]}, 'East has doubleton honors (HH)';
push @{$stext[51]}, 'East has one top at most doubleton';
push @{$stext[51]}, 'West has at least 1 cards';
push @{$stext[51]}, 'West has doubleton honors (HH)';

push @{$vtext[51]}, 'The suit splits between 2=2 and 3=1, and East has at least one top';
push @{$vtext[51]}, 'West has at least 1 cards';
push @{$vtext[51]}, 'West has doubleton honors (HH)';

# Example: 11 / 161820, Q54 / J6 mnissing HHxxxx.
push @{$stext[52]}, 'East has HH(xxxx); or';
push @{$stext[52]}, 'East has a singleton honor';
push @{$stext[52]}, 'East has an honor doubleton (Hx)';
push @{$stext[52]}, 'East is void';
push @{$stext[52]}, 'West has doubleton honors (HH)';

push @{$vtext[52]}, 'East has the tops; or';
push @{$vtext[52]}, 'East is void';
push @{$vtext[52]}, 'The suit splits between 4=2 and 5=1, and East has at least one top';
push @{$vtext[52]}, 'West has doubleton honors (HH)';

# Example: 11 / 161796, Q4 / J765 missing HHxxx.
push @{$stext[53]}, 'East has a small singleton';
push @{$stext[53]}, 'East has both tops at most tripleton';
push @{$stext[53]}, 'East is void';
push @{$stext[53]}, 'West has at most 3 cards';
push @{$stext[53]}, 'West has both tops at most tripleton';

push @{$vtext[53]}, 'East has both tops at most tripleton';
push @{$vtext[53]}, 'West has at most 3 cards';
push @{$vtext[53]}, 'West has the tops';

# Example: 11 / 161796, Q4 / J765 missing HHxxx.
push @{$stext[54]}, 'East has a small doubleton';
push @{$stext[54]}, 'East has doubleton honors (HH)';
push @{$stext[54]}, 'The suit splits 2-3 either way; or';
push @{$stext[54]}, 'West has doubleton honors (HH)';
push @{$stext[54]}, 'West has one top at most doubleton';
push @{$stext[54]}, 'West has the tops';
push @{$stext[54]}, 'West is void';

push @{$vtext[54]}, 'East has doubleton honors (HH)';
push @{$vtext[54]}, 'The suit splits 2-3 either way';
push @{$vtext[54]}, 'The suit splits between 1=4 and 2=3, and West has at least one top; or';
push @{$vtext[54]}, 'West has the tops';
push @{$vtext[54]}, 'West is void';

# Example: 11 / 161796, Q4 / J765 missing HHxxx.
push @{$stext[55]}, 'East has a singleton honor';
push @{$stext[55]}, 'East has doubleton honors (HH)';
push @{$stext[55]}, 'The suit splits 2-3 either way';
push @{$stext[55]}, 'West has a singleton honor';
push @{$stext[55]}, 'West has doubleton honors (HH)';
push @{$stext[55]}, 'West is void';

push @{$vtext[55]}, 'East has doubleton honors (HH)';
push @{$vtext[55]}, 'The suit splits 2-3 either way; or';
push @{$vtext[55]}, 'West has doubleton honors (HH)';
push @{$vtext[55]}, 'West has exactly one top';
push @{$vtext[55]}, 'West is void';

# Example: 11 / 161784, Q654 / J7 missing HHxxx.
push @{$stext[56]}, 'East has both tops at most tripleton';
push @{$stext[56]}, 'West has a small singleton';
push @{$stext[56]}, 'West has at least 2 cards';
push @{$stext[56]}, 'West has both tops at most tripleton';
push @{$stext[56]}, 'West is void';

push @{$vtext[56]}, 'East has the tops';
push @{$vtext[56]}, 'West has at least 2 cards';
push @{$vtext[56]}, 'West has both tops at most tripleton';

# Example: 11 / 161784, Q654 / J7 missing HHxxx.
push @{$stext[57]}, 'East has a singleton honor';
push @{$stext[57]}, 'East has an honor doubleton (Hx)';
push @{$stext[57]}, 'East has the tops; or';
push @{$stext[57]}, 'East is void';
push @{$stext[57]}, 'The suit splits 2-3 either way';
push @{$stext[57]}, 'West has doubleton honors (HH)';

push @{$vtext[57]}, 'East has the tops';
push @{$vtext[57]}, 'East is void';
push @{$vtext[57]}, 'The suit splits 2-3 either way';
push @{$vtext[57]}, 'The suit splits between 3=2 and 4=1, and East has at least one top; or';
push @{$vtext[57]}, 'West has doubleton honors (HH)';

# Example: 11 / 161784, Q654 / J7 missing HHxxx.
push @{$stext[58]}, 'East has a singleton honor';
push @{$stext[58]}, 'East has doubleton honors (HH)';
push @{$stext[58]}, 'East is void';
push @{$stext[58]}, 'The suit splits 2-3 either way';
push @{$stext[58]}, 'West has a singleton honor';
push @{$stext[58]}, 'West has doubleton honors (HH)';

push @{$vtext[58]}, 'East has doubleton honors (HH)';
push @{$vtext[58]}, 'East is void';
push @{$vtext[58]}, 'The suit splits 2-3 either way; or';
push @{$vtext[58]}, 'West has doubleton honors (HH)';
push @{$vtext[58]}, 'West has exactly one top';

# Example: 11 / 161784, Q654 / J7 missing HHxxx.
push @{$stext[59]}, 'East has a small doubleton';
push @{$stext[59]}, 'The suit splits 2-3 either way; or';
push @{$stext[59]}, 'West has a singleton honor';
push @{$stext[59]}, 'West has doubleton honors (HH)';
push @{$stext[59]}, 'West has the tops';

push @{$vtext[59]}, 'The suit splits 2-3 either way';
push @{$vtext[59]}, 'West has a singleton honor';
push @{$vtext[59]}, 'West has the tops';

# Example: 11 / 161730, Q654 / J8 missing HHhh7.
push @{$stext[60]}, 'East has a singleton honor';
push @{$stext[60]}, 'East has an honor doubleton (Hx)';
push @{$stext[60]}, 'East has the tops; or';
push @{$stext[60]}, 'East is void';
push @{$stext[60]}, 'The suit splits 2-3 either way';
push @{$stext[60]}, 'West has doubleton honors (HH)';

push @{$vtext[60]}, 'East has the tops; or';
push @{$vtext[60]}, 'East is void';
push @{$vtext[60]}, 'The suit splits 2-3 either way';
push @{$vtext[60]}, 'The suit splits between 3=2 and 4=1, and East has at least one top';
push @{$vtext[60]}, 'West has doubleton honors (HH)';

# Example: 11 / 161662, Q8 / J654 missing HHhh7.
push @{$stext[61]}, 'East has a small doubleton';
push @{$stext[61]}, 'East has doubleton honors (HH)';
push @{$stext[61]}, 'The suit splits 2-3 either way; or';
push @{$stext[61]}, 'West has doubleton honors (HH)';
push @{$stext[61]}, 'West has one top at most doubleton';
push @{$stext[61]}, 'West has the tops';
push @{$stext[61]}, 'West is void';

push @{$vtext[61]}, 'East has doubleton honors (HH)';
push @{$vtext[61]}, 'The suit splits 2-3 either way';
push @{$vtext[61]}, 'The suit splits between 1=4 and 2=3, and West has at least one top';
push @{$vtext[61]}, 'West has the tops; or';
push @{$vtext[61]}, 'West is void';

# Example: 11 / 159642, QJ54 / - mising HHxxxxx.
push @{$stext[62]}, 'East has both tops at most tripleton';
push @{$stext[62]}, 'East has doubleton honors (HH)';
push @{$stext[62]}, 'East has one top at most doubleton; or';
push @{$stext[62]}, 'West has both tops at most tripleton';
push @{$stext[62]}, 'West has doubleton honors (HH)';
push @{$stext[62]}, 'West has one top at most doubleton; or';

push @{$vtext[62]}, 'East has both tops at most tripleton';
push @{$vtext[62]}, 'The suit splits between 1=6 and 2=5, and West has at least one top';
push @{$vtext[62]}, 'The suit splits between 5=2 and 6=1, and East has at least one top';
push @{$vtext[62]}, 'West has both tops at most tripleton';

# Example: 11 / 159633, QJ54 / 6 mising HHxxxx.
push @{$stext[63]}, 'East has doubleton honors (HH)';
push @{$stext[63]}, 'The suit splits 3=3; or';
push @{$stext[63]}, 'West has doubleton honors (HH)';
push @{$stext[63]}, 'West has one top at most doubleton';
push @{$stext[63]}, 'West has the tops';

push @{$vtext[63]}, 'East has both tops at most tripleton';
push @{$vtext[63]}, 'The suit splits between 1=5 and 3=3, and West has at least one top; or';
push @{$vtext[63]}, 'West has doubleton honors (HH)';
push @{$vtext[63]}, 'West has the tops';

# Example: 11 / 159633, QJ54 / 6 mising HHxxxx.
push @{$stext[64]}, 'East has doubleton honors (HH)';
push @{$stext[64]}, 'The suit splits 3=3; or';
push @{$stext[64]}, 'West has one top at most doubleton';
push @{$stext[64]}, 'West has the honors tripleton (HHx)';
push @{$stext[64]}, 'West has the tops';

push @{$vtext[64]}, 'East has both tops at most tripleton';
push @{$vtext[64]}, 'The suit splits between 1=5 and 3=3, and West has at least one top; or';
push @{$vtext[64]}, 'West has the honors tripleton (HHx)';
push @{$vtext[64]}, 'West has the tops';

# Example: 11 / 159624, QJ654 / - missing HHxxxx.
push @{$stext[65]}, 'East has doubleton honors (HH)';
push @{$stext[65]}, 'The suit splits 2-4 or better either way';
push @{$stext[65]}, 'The suit splits between 1=5 and 3=3, and Either opponent has at least one top';
push @{$stext[65]}, 'West has doubleton honors (HH)';

push @{$vtext[65]}, 'East has doubleton honors (HH)';
push @{$vtext[65]}, 'The suit splits 2-4 or better either way';
push @{$vtext[65]}, 'The suit splits between 1=5 and 3=3, and West has at least one top';
push @{$vtext[65]}, 'The suit splits between 3=3 and 5=1, and East has at least one top; or';
push @{$vtext[65]}, 'West has doubleton honors (HH)';
push @{$vtext[65]}, 'West has exactly one top; or';

# Example: 11 / 159606, QJ54 / 76 missing HHxxx.
push @{$stext[66]}, 'The suit splits 2-3 either way, and West has at least one top';
push @{$stext[66]}, 'West has a singleton honor';
push @{$stext[66]}, 'West has at least 2 cards; or';

push @{$vtext[66]}, 'The suit splits between 1=4 and 3=2, and West has at least one top';
push @{$vtext[66]}, 'West has at least 2 cards';

# Example: 11 / 159597, QJ654 / 7 missing HHxxx.
push @{$stext[67]}, 'East has a small doubleton';
push @{$stext[67]}, 'East has doubleton honors (HH)';
push @{$stext[67]}, 'Neither opponent is void';
push @{$stext[67]}, 'The suit splits 2-3 either way; or';
push @{$stext[67]}, 'West has doubleton honors (HH)';
push @{$stext[67]}, 'West has one top at most doubleton';
push @{$stext[67]}, 'West has the tops';

push @{$vtext[67]}, 'East has doubleton honors (HH)';
push @{$vtext[67]}, 'The suit splits 2-3 either way';
push @{$vtext[67]}, 'The suit splits between 1=4 and 2=3, and West has at least one top; or';
push @{$vtext[67]}, 'West has HH(xx)';
push @{$vtext[67]}, 'West has at least 1 cards';

# Example: 11 / 159543, QJ654 / 8 missing HHhh7.
push @{$stext[68]}, 'East has a small doubleton';
push @{$stext[68]}, 'East has doubleton honors (HH)';
push @{$stext[68]}, 'Neither opponent is void';
push @{$stext[68]}, 'The suit splits 2-3 either way; or';
push @{$stext[68]}, 'West has doubleton honors (HH)';
push @{$stext[68]}, 'West has one top at most doubleton';
push @{$stext[68]}, 'West has the tops';

push @{$vtext[68]}, 'East has doubleton honors (HH)';
push @{$vtext[68]}, 'The suit splits 2-3 either way';
push @{$vtext[68]}, 'The suit splits between 1=4 and 2=3, and West has at least one top';
push @{$vtext[68]}, 'West has HH(xx); or';
push @{$vtext[68]}, 'West has at least 1 cards';

# Example: 11 / 158184, QJT54 / - missing HHxxxx.
push @{$stext[69]}, 'East has a singleton honor';
push @{$stext[69]}, 'East has doubleton honors (HH)';
push @{$stext[69]}, 'The suit splits 2-4 or better either way';
push @{$stext[69]}, 'West has a singleton honor';
push @{$stext[69]}, 'West has doubleton honors (HH)';

push @{$vtext[69]}, 'East has doubleton honors (HH)';
push @{$vtext[69]}, 'The suit splits 2-4 or better either way; or';
push @{$vtext[69]}, 'West has doubleton honors (HH)';
push @{$vtext[69]}, 'West has exactly one top';

# Example: 11 / 137646, K654 / 87 missing AHHHH.
push @{$stext[70]}, 'East has Hx(x)';
push @{$stext[70]}, 'East has the singleton honor';
push @{$stext[70]}, 'West has Hx(x)';
push @{$stext[70]}, 'West has the top';

push @{$vtext[70]}, 'West has H(xx)';
push @{$vtext[70]}, 'West has at least 2 cards';

# Example: 11 / 137646, K654 / 87 missing AHHHH.
push @{$stext[71]}, 'East has Hx(x)';
push @{$stext[71]}, 'East has the top at most doubleton';
push @{$stext[71]}, 'East is void';
push @{$stext[71]}, 'West has Hx(x)';
push @{$stext[71]}, 'West has the top at most doubleton';

push @{$vtext[71]}, 'East has the top at most doubleton';
push @{$vtext[71]}, 'East is void';
push @{$vtext[71]}, 'The suit splits 2-3 either way';
push @{$vtext[71]}, 'West has the top at most doubleton';

# Example: 11 / 137619, K7654 / 8 missing AHHHH.
push @{$stext[72]}, 'East has Hx(x)';
push @{$stext[72]}, 'Neither opponent is void';
push @{$stext[72]}, 'West has Hx(x)';
push @{$stext[72]}, 'West has the top';

push @{$vtext[72]}, 'The suit splits 2-3 either way';
push @{$vtext[72]}, 'West has H(xxx)';
push @{$vtext[72]}, 'West has at least 1 cards';

# Example: 11 / 131166, K654 / Q7 missing Axxxx.
push @{$stext[73]}, 'East has Hx(x)';
push @{$stext[73]}, 'East has the top at most doubleton';
push @{$stext[73]}, 'West has Hx(x)';

push @{$vtext[73]}, 'East has the top at most doubleton';
push @{$vtext[73]}, 'The suit splits 2-3 either way';

# Example: 11 / 129018, K4 / QJ65 missing Axxxx.
push @{$stext[74]}, 'East has Hx(x)';
push @{$stext[74]}, 'East has the singleton honor';
push @{$stext[74]}, 'West has Hx(x)';

push @{$vtext[74]}, 'East has the singleton honor';
push @{$vtext[74]}, 'The suit splits 2-3 either way';

# Example: 11 / 124605, KQ654 / 7 missing Axxxx.
push @{$stext[75]}, 'East has Hx(x)';
push @{$stext[75]}, 'Neither opponent is void';
push @{$stext[75]}, 'West has Hx(x)';
push @{$stext[75]}, 'West has the top at most doubleton';

push @{$vtext[75]}, 'Neither opponent is void';
push @{$vtext[75]}, 'The suit splits 2-3 either way';
push @{$vtext[75]}, 'West has the top at most doubleton';

# Example: 11 / 120267, KQJ54 / 6 missing Axxxx.
push @{$stext[76]}, 'East has Hx(x)';
push @{$stext[76]}, 'Neither opponent is void';
push @{$stext[76]}, 'West has Hx(x)';
push @{$stext[76]}, 'West has the singleton honor';

push @{$vtext[76]}, 'Neither opponent is void';
push @{$vtext[76]}, 'The suit splits 2-3 either way';
push @{$vtext[76]}, 'West has the singleton honor';

# Example: 11 / 57615, A9874 / T missing HHHxx.
push @{$stext[77]}, 'East has a singleton honor';
push @{$stext[77]}, 'The suit splits 2-3 either way; or';
push @{$stext[77]}, 'West has a singleton honor';
push @{$stext[77]}, 'West has at most 4 cards';

push @{$vtext[77]}, 'The suit splits 2-3 either way';
push @{$vtext[77]}, 'West has at most 4 cards';
push @{$vtext[77]}, 'West has between 1 and 2 tops; or';

# Example: 11 / 56736, A54 / J876 missing HHhh.
push @{$stext[78]}, 'East has doubleton honors (HH)';
push @{$stext[78]}, 'West has at most 3 cards';
push @{$stext[78]}, 'West has doubleton honors (HH)';
push @{$stext[78]}, 'West has one top at most doubleton';

push @{$vtext[78]}, 'East has doubleton honors (HH)';
push @{$vtext[78]}, 'The suit splits between 1=3 and 2=2, and West has at least one top';
push @{$vtext[78]}, 'West has at most 3 cards';

# Example: 11 / 55482, A984 / JT missing HHxxx.
push @{$stext[79]}, 'East has a small singleton';
push @{$stext[79]}, 'East is void';
push @{$stext[79]}, 'The suit splits 2-3 either way, and West has at least one top';
push @{$stext[79]}, 'West has a singleton honor';

push @{$vtext[79]}, 'The suit splits between 1=4 and 3=2, and West has at least one top; or';
push @{$vtext[79]}, 'West has the tops';

# Example: 11 / 55398, AT4 / J5 missing HHxxxx.
push @{$stext[80]}, 'East has a singleton honor';
push @{$stext[80]}, 'East has an honor doubleton (Hx)';
push @{$stext[80]}, 'East is void';
push @{$stext[80]}, 'West has doubleton honors (HH)';

push @{$vtext[80]}, 'East has one top at most doubleton';
push @{$vtext[80]}, 'East is void';
push @{$vtext[80]}, 'West has doubleton honors (HH)';

# Example: 11 / 55386, AT54 / J6 missing HHxxx.
push @{$stext[81]}, 'East has a small doubleton';
push @{$stext[81]}, 'West has a small singleton';
push @{$stext[81]}, 'West has at least 2 cards';
push @{$stext[81]}, 'West has doubleton honors (HH)';
push @{$stext[81]}, 'West is void';

push @{$vtext[81]}, 'East has the tops';
push @{$vtext[81]}, 'West has at least 2 cards; or';
push @{$vtext[81]}, 'West has both tops at most tripleton';

# Example: 11 / 55386, AT54 / J6 missing HHxxx.
push @{$stext[82]}, 'East has a small doubleton';
push @{$stext[82]}, 'East has doubleton honors (HH)';
push @{$stext[82]}, 'The suit splits 2-3 either way; or';
push @{$stext[82]}, 'West has a small singleton';
push @{$stext[82]}, 'West has doubleton honors (HH)';
push @{$stext[82]}, 'West has the tops';
push @{$stext[82]}, 'West is void';

push @{$vtext[82]}, 'East has doubleton honors (HH)';
push @{$vtext[82]}, 'East has the tops';
push @{$vtext[82]}, 'The suit splits 2-3 either way; or';
push @{$vtext[82]}, 'West has the tops';

# Example: 11 / 55386, AT54 / J6 missing HHxxx.
push @{$stext[83]}, 'East has one top at most doubleton';
push @{$stext[83]}, 'East is void';
push @{$stext[83]}, 'The suit splits 2-3 either way';
push @{$stext[83]}, 'West has a small singleton';
push @{$stext[83]}, 'West has doubleton honors (HH)';
push @{$stext[83]}, 'West is void';

push @{$vtext[83]}, 'East has one top at most doubleton';
push @{$vtext[83]}, 'East has the tops';
push @{$vtext[83]}, 'East is void';
push @{$vtext[83]}, 'The suit splits 2-3 either way; or';
push @{$vtext[83]}, 'West has doubleton honors (HH)';

# Example: 11 / 55386, AT54 / J6 missing HHxxx.
push @{$stext[84]}, 'East has a singleton honor';
push @{$stext[84]}, 'East is void';
push @{$stext[84]}, 'The suit splits 2-3 either way';
push @{$stext[84]}, 'West has a singleton honor';
push @{$stext[84]}, 'West has doubleton honors (HH)';

push @{$vtext[84]}, 'East is void';
push @{$vtext[84]}, 'The suit splits 2-3 either way; or';
push @{$vtext[84]}, 'West has doubleton honors (HH)';
push @{$vtext[84]}, 'West has exactly one top';

# Example: 11 / 54912, AT94 / J5 missing HHxxx.
push @{$stext[85]}, 'The suit splits 2-3 either way, and West has at least one top';
push @{$stext[85]}, 'West has a singleton honor';

push @{$vtext[85]}, 'The suit splits between 1=4 and 3=2, and West has at least one top';

# Example: 11 / 52725, AJT94 / 5 missing HHxxx.
push @{$stext[86]}, 'East has a small doubleton';
push @{$stext[86]}, 'East has doubleton honors (HH)';
push @{$stext[86]}, 'Neither opponent is void';
push @{$stext[86]}, 'West has doubleton honors (HH)';
push @{$stext[86]}, 'West has one top at most doubleton';

push @{$vtext[86]}, 'East has a small doubleton';
push @{$vtext[86]}, 'East has doubleton honors (HH)';
push @{$vtext[86]}, 'Neither opponent is void';
push @{$vtext[86]}, 'The suit splits between 1=4 and 2=3, and West has at least one top';

# Example: 11 / 52719, AJT94 / 6 missing HHhh5.
push @{$stext[87]}, 'East has a small doubleton';
push @{$stext[87]}, 'East has doubleton honors (HH)';
push @{$stext[87]}, 'Neither opponent is void';
push @{$stext[87]}, 'West has doubleton honors (HH)';
push @{$stext[87]}, 'West has one top at most doubleton';

push @{$vtext[87]}, 'East has doubleton honors (HH)';
push @{$vtext[87]}, 'Neither opponent is void';
push @{$vtext[87]}, 'The suit splits between 1=4 and 2=3, and West has at least one top; or';
push @{$vtext[87]}, 'West has both tops at most tripleton';

# Example: 11 / 52446, A4 / Q765 missing Kxxxx.
push @{$stext[88]}, 'East has Hx(x)';
push @{$stext[88]}, 'East has the top';
push @{$stext[88]}, 'West has Hx(x)';
push @{$stext[88]}, 'West has the singleton honor';

push @{$vtext[88]}, 'East has H(xx)';
push @{$vtext[88]}, 'West has at most 3 cards';

# Example: 11 / 52434, A654 / Q7 missing Kxxxx.
push @{$stext[89]}, 'East has Hx(x)';
push @{$stext[89]}, 'East has the top';
push @{$stext[89]}, 'West has Hx(x)';

push @{$vtext[89]}, 'East has the top';
push @{$vtext[89]}, 'The suit splits 2-3 either way';

# Example: 11 / 38146, A9 / KT54 mising HHxxx.
push @{$stext[90]}, 'East has a singleton honor';
push @{$stext[90]}, 'East has doubleton honors (HH)';
push @{$stext[90]}, 'The suit splits 2-3 either way';
push @{$stext[90]}, 'West has a singleton honor';

push @{$vtext[90]}, 'East has doubleton honors (HH)';
push @{$vtext[90]}, 'The suit splits 2-3 either way; or';
push @{$vtext[90]}, 'West has exactly one top';

# Example: 11 / 38142, A954 / KT missing HHxxx.
push @{$stext[91]}, 'East has a singleton honor';
push @{$stext[91]}, 'The suit splits 2-3 either way';
push @{$stext[91]}, 'West has a small singleton';
push @{$stext[91]}, 'West is void';

push @{$vtext[91]}, 'East has a singleton honor';
push @{$vtext[91]}, 'East has the tops';
push @{$vtext[91]}, 'The suit splits 2-3 either way; or';

# Example: 11 / 38142, A954 / KT missing HHxxx.
push @{$stext[92]}, 'East has doubleton honors (HH)';
push @{$stext[92]}, 'The suit splits 2-3 either way';
push @{$stext[92]}, 'West has a small singleton';
push @{$stext[92]}, 'West is void';

push @{$vtext[92]}, 'East has doubleton honors (HH)';
push @{$vtext[92]}, 'East has the tops';
push @{$vtext[92]}, 'The suit splits 2-3 either way; or';

# Example: 11 / 38142, A954 / KT missing HHxxx.
push @{$stext[93]}, 'East has a singleton honor';
push @{$stext[93]}, 'The suit splits 2-3 either way';
push @{$stext[93]}, 'West has a singleton honor';
push @{$stext[93]}, 'West has doubleton honors (HH)';

push @{$vtext[93]}, 'The suit splits 2-3 either way; or';
push @{$vtext[93]}, 'West has doubleton honors (HH)';
push @{$vtext[93]}, 'West has exactly one top';

# Example: 11 / 37986, A984 / KT missing HHxxx.
push @{$stext[94]}, 'East has a singleton honor';
push @{$stext[94]}, 'East has doubleton honors (HH)';
push @{$stext[94]}, 'East is void';
push @{$stext[94]}, 'The suit splits 2-3 either way';
push @{$stext[94]}, 'West has a small singleton';
push @{$stext[94]}, 'West is void';

push @{$vtext[94]}, 'East has a singleton honor';
push @{$vtext[94]}, 'East has doubleton honors (HH)';
push @{$vtext[94]}, 'East has the tops';
push @{$vtext[94]}, 'East is void';
push @{$vtext[94]}, 'The suit splits 2-3 either way; or';

# Example: 11 / 35716, AT / KJ54 missing Qxxxx.
push @{$stext[95]}, 'East has Hx(x)';
push @{$stext[95]}, 'East has a small singleton';
push @{$stext[95]}, 'East has the singleton honor';
push @{$stext[95]}, 'East has the top at most doubleton';
push @{$stext[95]}, 'East is void';
push @{$stext[95]}, 'West has Hx(x)';
push @{$stext[95]}, 'West has the singleton honor';
push @{$stext[95]}, 'West has the top at most doubleton';
push @{$stext[95]}, 'West is void';

push @{$vtext[95]}, 'East has the top at most doubleton';
push @{$vtext[95]}, 'West has at least 2 cards';
push @{$vtext[95]}, 'West has the singleton honor';
push @{$vtext[95]}, 'West has the top at most doubleton';
push @{$vtext[95]}, 'West is void';

# Example: 11 / 35712, AT54 / KJ missing Qxxxx.
push @{$stext[96]}, 'East has Hx(x)';
push @{$stext[96]}, 'East has Hxxx(x)';
push @{$stext[96]}, 'East has the singleton honor';
push @{$stext[96]}, 'East has the top at most doubleton';
push @{$stext[96]}, 'East is void';
push @{$stext[96]}, 'West has Hx(x)';
push @{$stext[96]}, 'West has the singleton honor';
push @{$stext[96]}, 'West has the top at most doubleton';

push @{$vtext[96]}, 'East has the singleton honor';
push @{$vtext[96]}, 'East has the top at most doubleton';
push @{$vtext[96]}, 'East is void';
push @{$vtext[96]}, 'West has at most 3 cards';
push @{$vtext[96]}, 'West has the top at most doubleton';

# Example: 11 / 32535, A654 / KQ9 missing HHxx.
push @{$stext[97]}, 'East has a singleton honor';
push @{$stext[97]}, 'East has doubleton honors (HH)';
push @{$stext[97]}, 'West has a small singleton';
push @{$stext[97]}, 'West is void';

push @{$vtext[97]}, 'East has a singleton honor';
push @{$vtext[97]}, 'East has the tops';

# Example: 11 / 32070, A4 / KQT5 missing Jxxxx.
push @{$stext[98]}, 'East has Hx(x)';
push @{$stext[98]}, 'East has the singleton honor';
push @{$stext[98]}, 'West has Hx(x)';
push @{$stext[98]}, 'West has the singleton honor';
push @{$stext[98]}, 'West is void';

push @{$vtext[98]}, 'East has the singleton honor';
push @{$vtext[98]}, 'The suit splits 2-3 either way';
push @{$vtext[98]}, 'West has the singleton honor';
push @{$vtext[98]}, 'West is void';

# Example: 11 / 25026, AQ94 / KT missing Jxxxx.
push @{$stext[99]}, 'East has Hx(x)';
push @{$stext[99]}, 'East has the singleton honor';
push @{$stext[99]}, 'East is void';
push @{$stext[99]}, 'West has Hx(x)';
push @{$stext[99]}, 'West has the singleton honor';

push @{$vtext[99]}, 'East has the singleton honor';
push @{$vtext[99]}, 'East is void';
push @{$vtext[99]}, 'The suit splits 2-3 either way';
push @{$vtext[99]}, 'West has the singleton honor';

# Example: 11 / 18546, AK84 / T9 missing HHxxx.
push @{$stext[100]}, 'East has a small doubleton';
push @{$stext[100]}, 'West has at least one top';
push @{$stext[100]}, 'West has doubleton honors (HH)';

push @{$vtext[100]}, 'West has at least one top';
push @{$vtext[100]}, 'West has both tops at most tripleton';

# Example: 11 / 18462, AK94 / T5 missing HHxxx.
push @{$stext[101]}, 'East has a singleton honor';
push @{$stext[101]}, 'The suit splits 2-3 either way, and West has at least one top';
push @{$stext[101]}, 'West has the tops; or';

push @{$vtext[101]}, 'West has at least 2 cards, and West has at least one top';
push @{$vtext[101]}, 'West has both tops at most tripleton';

# Example: 11 / 18303, AK984 / T missing HHxxx.
push @{$stext[102]}, 'East has a singleton honor';
push @{$stext[102]}, 'East has doubleton honors (HH)';
push @{$stext[102]}, 'The suit splits 2-3 either way';
push @{$stext[102]}, 'West has a singleton honor';
push @{$stext[102]}, 'West has at most 4 cards';
push @{$stext[102]}, 'West has doubleton honors (HH)';

push @{$vtext[102]}, 'East has doubleton honors (HH)';
push @{$vtext[102]}, 'The suit splits 2-3 either way; or';
push @{$vtext[102]}, 'West has at most 4 cards';
push @{$vtext[102]}, 'West has doubleton honors (HH)';
push @{$vtext[102]}, 'West has exactly one top';

# Example: 11 / 17478, AK54 / J6 missing Qxxxx.
push @{$stext[103]}, 'East has Hx(x)';
push @{$stext[103]}, 'East has Hxxx(x)';
push @{$stext[103]}, 'East has the singleton honor';
push @{$stext[103]}, 'West has Hx(x)';

push @{$vtext[103]}, 'East has the top';
push @{$vtext[103]}, 'The suit splits 2-3 either way; or';

# Example: 11 / 16029, AKT54 / J missing Qxxxx.
push @{$stext[104]}, 'East has Hx(x)';
push @{$stext[104]}, 'East has the top at most doubleton';
push @{$stext[104]}, 'West has Hx(x)';
push @{$stext[104]}, 'West has at most 4 cards';
push @{$stext[104]}, 'West has the top at most doubleton';

push @{$vtext[104]}, 'East has the top at most doubleton';
push @{$vtext[104]}, 'The suit splits 2-3 either way';
push @{$vtext[104]}, 'West has at most 4 cards';
push @{$vtext[104]}, 'West has the top at most doubleton';

# Example: 11 / 6101, AKQ87 / 9 missing HHxxx.
push @{$stext[105]}, 'East has a small doubleton';
push @{$stext[105]}, 'East is void';
push @{$stext[105]}, 'Neither opponent is void';
push @{$stext[105]}, 'West has doubleton honors (HH)';

push @{$vtext[105]}, 'West has at least 1 cards';
push @{$vtext[105]}, 'West has both tops at most tripleton';

# Example: 11 / 5912, AKQ98 / - missing HHxxxx.
push @{$stext[106]}, 'East has a singleton honor';
push @{$stext[106]}, 'East has doubleton honors (HH)';
push @{$stext[106]}, 'The suit splits 2-4 or better either way';
push @{$stext[106]}, 'The suit splits 3=3';
push @{$stext[106]}, 'West has a singleton honor';
push @{$stext[106]}, 'West has doubleton honors (HH)';

push @{$vtext[106]}, 'East has doubleton honors (HH)';
push @{$vtext[106]}, 'The suit splits 2-4 or better either way; or';
push @{$vtext[106]}, 'The suit splits 3=3';
push @{$vtext[106]}, 'West has doubleton honors (HH)';
push @{$vtext[106]}, 'West has exactly one top';

# Example: 11 / 5343, AKQ94 / T missing Jxxxx.
push @{$stext[107]}, 'East has Hx(x)';
push @{$stext[107]}, 'East has the singleton honor';
push @{$stext[107]}, 'West has Hx(x)';
push @{$stext[107]}, 'West has at most 4 cards';
push @{$stext[107]}, 'West has the singleton honor';

push @{$vtext[107]}, 'East has the singleton honor';
push @{$vtext[107]}, 'The suit splits 2-3 either way';
push @{$vtext[107]}, 'West has at most 4 cards';
push @{$vtext[107]}, 'West has the singleton honor';



while (my $line = <$fh>)
{
  $lno++;

  if ($lno == 4896)
  {
    # print "HERE\n";
  }

  if ($line =~ /^Cards\s+(\d+):\s+(\d+)/)
  {
    $cards = $1;
    $holding = $2;
    @strats = ();
  }
  elsif ($line =~ /^Strategy #(\d+): (.*)/)
  {
    my $sno = $1;
    my $rest = $2;
    $rest =~ s///;
    $shead[$sno] = $rest;

    while (my $line2 = <$fh>)
    {
      chomp $line2;
      $line2 =~ s///;
      $lno++;
      last if ($line2 =~ /^\s*$/);
      $line2 =~ s/^\*\s+//;
      $line2 =~ s/^\s*-\s+//;
      $line2 =~ s/\s*\[.*\]//;
      $line2 =~ s/^YY //;
      $line2 =~ s/^\s+//;
      push @{$strats[$sno]}, $line2;
    }
  }
  elsif ($line =~ /^VStrategy #(\d+): (.*)/)
  {
    my $vno = $1;
    my $rest = $2;
    $rest =~ s///;
    $vhead[$vno] = $rest;
    @vstrats = ();

    while (my $line2 = <$fh>)
    {
      chomp $line2;
      $line2 =~ s///;
      $lno++;
      last if ($line2 =~ /^\s*$/);
      $line2 =~ s/^\s+//;
      $line2 =~ s/^\*\s+//;
      $line2 =~ s/\s*\[.*\]//;
      $line2 =~ s/\s+$//;
      push @vstrats, $line2;
    }

    my @vsorted = sort @vstrats;

    if ($shead[$vno] ne $vhead[$vno])
    {
      $countHeadText++;
      next;
    }

    for my $i (reverse 0 .. $#{$strats[$vno]})
    {
      my $t = $strats[$vno][$i];
      if ($t =~ /^Either opponent/)
      {
        my $t1 = $t;
        $t1 =~ s/^Either opponent/West/;
        push @{$strats[$vno]}, $t1;

        $t1 = $t;
        $t1 =~ s/^Either opponent/East/;
        push @{$strats[$vno]}, $t1;

        splice(@{$strats[$vno]}, $i, 1);
      }
    }

    my @ssorted = sort @{$strats[$vno]};

    if ($#ssorted != $#vsorted)
    {
      my $overall = 0;
      my $seen;
      for my $c (0 .. $#stext)
      {
        next if ($#{$stext[$c]} != $#ssorted);
        next if ($#{$vtext[$c]} != $#vsorted);

        my $match = 1;
        for my $d (0 .. $#ssorted)
        {
          if ($stext[$c][$d] ne $ssorted[$d])
          {
            $match = 0;
            last;
          }
        }
        next unless $match;

        for my $d (0 .. $#vsorted)
        {
          if ($vtext[$c][$d] ne $vsorted[$d])
          {
            $match = 0;
            last;
          }
        }
        next unless $match;
        $overall = 1;
        $seen = $c;
        last;
      }
      
      if ($overall)
      {
        $countKnown[$seen]++;
      }
      else
      {
print "$lno C\n";
        $countNum++;

        my $ssize = 1 + $#stext;
        for my $k (0 .. $#ssorted)
        {
          printf("push \@\{\$stext\[$ssize\]\}, '%s';\n", $ssorted[$k]);
        }
        print "\n";

        my $vsize = 1 + $#vtext;
        for my $k (0 .. $#vsorted)
        {
          printf("push \@\{\$vtext\[$vsize\]\}, '%s';\n", $vsorted[$k]);
        }
        print "\n";
      }
    }
    else
    {
      my $sameFlag = 1;
      for my $i (0 .. $#ssorted)
      {
        if ($ssorted[$i] ne $vsorted[$i])
        {
          $sameFlag = 0;
          last;
        }
      }

      if ($sameFlag)
      {
        $countSame++;
      }
      else
      {
print "$lno T\n";
        $countText++;
      }
    }
  }
}

close $fh;

print "Head text       ", $countHeadText, "\n";
print "Number of lines ", $countNum, "\n";
print "Text content    ", $countText, "\n";
print "Same            ", $countSame, "\n";

for my $i (0 .. $#countKnown)
{
  printf("  Known %2d\t%d\n", $i, $countKnown[$i]);
}
