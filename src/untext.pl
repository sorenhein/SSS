#!perl

use strict;
use warnings;

# Count the cover's that are not stated in words.

my $file = shift;
open my $fh, '<', $file or die $!;

my $numText = 0;
my $numUntext = 0;
my $lno = 0;

while (my $line = <$fh>)
{
  $lno++;

  if ($line =~ /^Strategy \#(\d+) \[c (\d+)\/(\d+), w (\d+)\/(\d+), d (\d+)\/(\w+)\]/)
  {
    while (my $line2 = <$fh>)
    {
      $lno++;

      chomp $line2;
      $line2 =~ s///g;
      $line2 =~ s/^\s*$//g;
      last if $line2 eq "";

      $line2 =~ s/^\* //;

      if ($line2 =~ /unused/)
      {
        $numUntext++;
      }
      else
      {
        $numText++;
      }
    }
  }
}

close $fh;

print "Number text:   $numText\n";
print "Number untext: $numUntext\n";
