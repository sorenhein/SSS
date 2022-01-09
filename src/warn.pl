#!perl

use strict;
use warnings;

# Check whether all WARN-NONMIN lead to MISMATCH, and if so, which kind.

my $file = shift;
open my $fh, '<', $file or die $!;

my $lno = 0;
my @warn;
my @mismatch;

while (my $line = <$fh>)
{
  $lno++;

  if ($line =~ /^WARN-NONMIN: holding (\d+) uses/)
  {
    $warn[$1] = $1;
  }
  elsif ($line =~ /^MISMATCH: (\d+) /)
  {
    $mismatch[$1] = 1;
  }
  elsif ($line =~ /^Individual timers/)
  {
    for my $w (@warn)
    {
      next unless defined $w;

      if (! defined $mismatch[$w])
      {
        print "Warning $w is not a mismatch\n";
      }
      else
      {
        # print "Warning $w is a mismatch\n";
      }
    }

    @warn = ();
    @mismatch = ();
  }
}

close $fh;
