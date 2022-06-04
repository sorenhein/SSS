#!perl

use strict;
use warnings;

my $file = shift;
open my $fh, '<', $file or die $!;

my ($cards, $holding, $north, $void);

my $lno = 0;

while (my $line = <$fh>)
{
  $lno++;

  if ($line =~ /^Cards\s+(\d+): (\d+)/)
  {
    $cards = $1;
    $holding = $2;

    $line = <$fh>; # Empty
    $line = <$fh>; # North
    $line =~ /^\s+(\w+)\s+$/;
    $north = $1;
    $line = <$fh>; # Top
    $line = <$fh>; # Middle
    $line = <$fh>; # Bottom
    $line = <$fh>; # South
    $line =~ s/^\s+(\S+)\s+$//;
    $void = ($1 eq '-' ? 'void' : $1);
    $lno += 6;

  }
  elsif ($line =~ /^Dist\s+.*\s(\d+)\s*$/)
  {
    # next unless $void;
    my $max = $1;
    print $max+1, ": ", $cards, "/", $holding, ", $north / $void\n";
  }
}

close $fh;
