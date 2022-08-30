#!perl

use strict;
use warnings;

# perl csplit.pl num45_3
# Split the verbal e.g. of type "*F" into buckets.

my @types;
my $file = shift;

open my $fh, '<', $file or die $!;
while (my $line = <$fh>)
{
  last if $line =~ /^Count/;
  next unless $line =~ /^\*G/;
  $line =~ s///g;
  $line =~ s/^\*F //;
  $line =~ s/; or\s*//;
  $line =~ s/\[\d+\/\d+\]//;
  chomp $line;

  if ($line =~ /lower than the/)
  {
    $types[0]{$line}++;
  }
  elsif ($line =~ /only/)
  {
    $types[1]{$line}++;
  }
  elsif ($line =~ /West/ && $line =~ /East/)
  {
    $types[2]{$line}++;
  }
  else
  {
    $types[3]{$line}++;
  }
}

close $fh;

for my $i (0 .. $#types)
{
  for my $key (sort keys %{$types[$i]})
  {
    printf("%6d %s\n", $types[$i]{$key}, $key);
  }
}
