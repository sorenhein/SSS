#!perl

use strict;
use warnings;

# Make a histogram of #strategies per cards, for void and non-void.

my $file = shift;
open my $fh, '<', $file or die $!;

my @data;

while (my $line = <$fh>)
{
  if ($line =~ /^Cards\s+(\d+):/)
  {
    my $cards = $1;

    $line = <$fh> for (1 .. 6);
    my $void = ($line =~ /^\s+\d/ ? 0 : 1);

    while (1)
    {
      last unless $line = <$fh>;
      last if $line =~ /^Result/;
    }

    $line = <$fh>;
    my @a = split /\s+/, $line;
    my $s = $a[-1] + 1;

    $data[$cards][$void]{num}++;
    $data[$cards][$void]{sum} += $s;
    $data[$cards][$void]{hist}[$s]++;
  }
}

close $fh;

for my $cards (0 .. $#data)
{
  next unless defined $data[$cards];
  for my $void (0 .. 1)
  {
    next unless defined $data[$cards][$void];

    print "Cards $cards void? $void count ",
      $data[$cards][$void]{num}, " avg ",
      $data[$cards][$void]{sum} / $data[$cards][$void]{num}, "\n";
    for my $h (0 .. $#{$data[$cards][$void]{hist}})
    {
      next unless defined $data[$cards][$void]{hist}[$h];
      printf("%4d %6d\n", $h, $data[$cards][$void]{hist}[$h]);
    }
    print "\n";
  }
}
