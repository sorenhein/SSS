#!perl

use strict;
use warnings;

# Extract the strategies with depth 5.

my $file = shift;
open my $fh, '<', $file or die $!;

my %hits;
my $lno = 0;
my $tag;

while (my $line = <$fh>)
{
  $lno++;

  if ($line =~ /^Cards\s+(\d+): (\d+)/)
  {
    my ($cards, $holding) = ($1, $2);
    $tag = "$cards/$holding";
  }
  elsif ($line =~ /^Strategy \#(\d+) \[c (\d+)\/(\d+), w (\d+)\/(\d+), d (\d+)\/(\w+)\]/)
  {
    my ($c, $r, $w1, $w2, $depth, $symm) = ($2, $3, $4, $5, $6, $7);

    if ($depth == 5)
    {
      if (defined $hits{$tag})
      {
        $hits{$tag}++;
      }
      else
      {
        $hits{$tag} = 1;
      }
    }
  }
}

close $fh;

for my $k (sort keys %hits)
{
  printf("%12d%12s\n", $hits{$k}, $k);
}
