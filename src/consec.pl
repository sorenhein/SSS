#!perl

use strict;
use warnings;

# Count the number of times that Minimal appears consecutively.

my $file = shift;
open my $fh, '<', $file or die $!;

my @hist;
my $lno = 0;

while (my $line = <$fh>)
{
  $lno++;

  if ($line =~ /^Minimal holding/)
  {
    my $count = 1;
    while ($line = <$fh>)
    {
      $lno++;
      if ($line =~ /^Minimal holding/)
      {
        $count++;
      }
      else
      {
        if ($count >= 5)
        {
          print "$lno\n";
        }
        $hist[$count]++;
        $count = 0;
        last;
      }
    }
  }
}

for my $h (0 .. $#hist)
{
  print "$h $hist[$h]\n";
}

close $fh;
