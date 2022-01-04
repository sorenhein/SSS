#!perl

use strict;
use warnings;

# Find an example of a Result that shrinks a lot when reduced.

my $RESMIN = 75;
my $FACTOR = 2;

my $file = shift;
open my $fh, '<', $file or die $!;

my ($full, $reduced);
my $lno = 0;

while (my $line = <$fh>)
{
  $lno++;

  if ($line =~ /^Result\s*$/)
  {
    $line = <$fh>;
    $lno++;
    if ($line =~ /\s+(\d+)\s*$/)
    {
      # Last integer
      $full = $1 + 1;
      next;
    }
    else
    {
      die "Bad next line: $line\n";
    }
  }
  elsif ($line =~ /^Reduced result\s*$/)
  {
    $line = <$fh>;
    $lno++;
    if ($line =~ /\s+(\d+)\s*$/)
    {
      # Last integer
      $reduced = $1 + 1;
    }
    else
    {
      die "Bad next line: $line\n";
    }

    if ($full >= $RESMIN && $reduced <= $RESMIN / $FACTOR)
    {
      print "line $lno\n";
      print "full $full, reduced $reduced\n";
    }
  }
}

close $fh;
