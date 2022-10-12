#!perl

use strict;
use warnings;

# perl csplit.pl num45_3
# Split the verbal e.g. of type "*F" into buckets.

my $file = shift;
my @tags = qw(C D E F G H);

for my $tag (@tags)
{
  my @types;

  open my $fh, '<', $file or die $!;
  while (my $line = <$fh>)
  {
    last if $line =~ /^Count/;
    next unless $line =~ /^\*${tag}/;
    $line =~ s///g;
    $line =~ s/^\*${tag} //;
    $line =~ s/; or\s*//;
    $line =~ s/\[\d+\/\d+\]//;
    chomp $line;

    $types[0]{$line}++;
  }

  close $fh;

  open my $fo, '>', "${tag}0" or die $!;

  for my $i (0 .. $#types)
  {
    for my $key (sort keys %{$types[$i]})
    {
      printf $fo ("%6d %s\n", $types[$i]{$key}, $key);
    }
  }

  close $fo;
}
