#!perl

use strict;
use warnings;

# Makes a list of include files used, based on depends_obj.txt or the like

my $file = shift;
open my $fh, '<', $file or die $!;

my %list;

while (my $line = <$fh>)
{
  my $val;
  if ($line =~ /^.*.obj: (.*)/)
  {
    my @a = split /\s+/, $1;
    for my $v (@a)
    {
      $list{$v} = 1;
    }
  }
}

printf("%-16s%-35s\\\n", "INCLUDE_FILES =", "");
for my $v (sort keys %list)
{
  printf("%-16s%-35s\\\n", "", $v);
}

# So we don't end on a continuation line
print "\n"
