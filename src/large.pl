#!perl

use strict;
use warnings;

my $file = shift;
open my $fh, '<', $file or die $!;

my ($cards, $lno, $code, $dlen, $dmax);

$lno = 0;
$dmax = 0;

while (my $line = <$fh>)
{
  $lno++;

  if ($line =~ /^Cards\s+(\d+)\s*$/)
  {
    $cards = $1;
    $dmax = 0;
  }
  elsif ($line =~ /^A (\d+)/)
  {
    $code = $1;
  }
  elsif ($line =~ /^Dist\s+(.*)/)
  {
    $dlen = length $line;
    my $est = int($dlen/4);
    if ($dlen > $dmax)
    {
      print "Cards $cards lno $lno code $code est $est\n";
      $dmax = $dlen;
    }
  }
}

close $fh;
