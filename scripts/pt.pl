#!perl

use strict;
use warnings;

my $file = shift;
open my $fh, '<', $file or die $!;

my $LIMIT = shift;

my @sum;

while (my $line = <$fh>)
{
  if ($line =~ /^TIMER (\d+) (\d+)/)
  {
    my $a = $1;
    my $b = $2;
    $line = <$fh>;
    $line =~ /^\s*(\d+\.\d+) ms/;
    my $c = $1;
    $line = <$fh>;
    $line =~ /^\s*(\d+\.\d+) ms/;
    my $d = $1;

    $sum[$a][$b]{first} += $c;
    $sum[$a][$b]{second} += $d;
    $sum[$a][$b]{count}++;
  }
}

my ($s1, $s2, $t1, $t2);
for (my $a = 0; $a <= $#sum; $a++)
{
  for (my $b = 0; $b <= $#{$sum[$a]}; $b++)
  {
    next unless defined $sum[$a][$b];
    if ($sum[$a][$b]{first} == 0.)
    {
    printf("%4d %4d: %6d %10.2f %10.2f\n",
      $a, $b, $sum[$a][$b]{count}, $sum[$a][$b]{first}, 
      $sum[$a][$b]{second});
    }
    else
    {
    printf("%4d %4d: %6d %10.2f %10.2f %10.2f\n",
      $a, $b, $sum[$a][$b]{count}, $sum[$a][$b]{first}, 
      $sum[$a][$b]{second}, $sum[$a][$b]{second} / $sum[$a][$b]{first});
    }
    # if ($a >= $LIMIT && $b >= $LIMIT)
    if ($a >= $LIMIT || $b >= $LIMIT || $a*$b >= $LIMIT*$LIMIT)
    {
      $s1 += $sum[$a][$b]{first};
      $s2 += $sum[$a][$b]{second};
    }
    else
    {
      $t1 += $sum[$a][$b]{first};
      $t2 += $sum[$a][$b]{second};
    }
  }
}

print "Large, first  ", $s1, "\n";
print "Large, second ", $s2, "\n";
print "Small, first  ", $t1, "\n";
print "Small, second ", $t2, "\n";
print "Optimal       ", $s2 + $t1, "\n";
