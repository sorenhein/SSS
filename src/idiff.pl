#!perl

use strict;
use warnings;

# Intelligent diff (hopefully) of two output files with respect
# to different cover.

my $DIFFNORMAL = "diffNormal.txt";
my $DIFFOR = "diffOr.txt";

my $file1 = shift;
my $file2 = shift;

open my $fh1, '<', $file1 or die $!;
open my $fh2, '<', $file2 or die $!;

open my $fo1, '>', $DIFFNORMAL or die $!;
open my $fo2, '>', $DIFFOR or die $!;

my ($lno1, $cards1, $holding1, $header1, $common1, @strats1);
my ($lno2, $cards2, $holding2, $header2, $common2, @strats2);

$lno1 = 0;
$lno2 = 0;

my @equalStats;
my @diffStats;
my @diffOrStats;
my $diffCum = 0;
my $diffOrCum = 0;

for my $i (0 .. 11)
{
  $equalStats[$i] = 0;
  $diffStats[$i] = 0;
  $diffOrStats[$i] = 0;
}

# Advance to 4 cards.
while (1)
{
  advance($fh1, \$lno1, \$cards1, \$holding1, \$header1);
  last if $cards1 == 4;
}

while (1)
{
  advance($fh2, \$lno2, \$cards2, \$holding2, \$header2);
  last if $cards2 == 4;
}

while (1)
{
  die "Different cards" unless $cards1 == $cards2;
  die "Different holdings" unless $holding1 == $holding2;

  my $oldHeader = $header1;

  slurp($fh1, \$lno1, \$common1, \@strats1, \$header1, \$cards1, \$holding1);
  slurp($fh2, \$lno2, \$common2, \@strats2, \$header2, \$cards2, \$holding2);

  die "Different number of strategies" unless $#strats1 == $#strats2;
  die "Different commons" unless $common1 eq $common2;

  last if ($header1 eq "" && $header2 eq "");

  my $diffFlag = 0;
  my $diffNormalFlag = 0;
  my $diffOrFlag = 0;
  my @diff;
  my @diffNormal;
  my @diffOr;
  for my $i (0 .. $#strats1)
  {
    $diff[$i] = stratDiff(\%{$strats1[$i]}, \%{$strats2[$i]});

    if ($diff[$i])
    {
      if ($strats1[$i]{text} =~ /; or/)
      {
        $diffOrStats[$cards1]++;
        $diffOrCum += $strats2[$i]{complexity} - $strats1[$i]{complexity};
        $diffOrFlag = 1;
        $diffOr[$i] = 1;
      }
      else
      {
        $diffStats[$cards1]++;
        $diffCum += $strats2[$i]{complexity} - $strats1[$i]{complexity};
        $diffNormalFlag = 1;
        $diffNormal[$i] = 1;
      }
      $diffFlag = 1;
    }
    else
    {
      $equalStats[$cards1]++;
      $diffOr[$i] = 0;
      $diffNormal[$i] = 0;
    }
  }

  next unless $diffFlag;

  if ($diffNormalFlag)
  {
    print $fo1 $oldHeader;
    print $fo1 $common1;
  }

  if ($diffOrFlag)
  {
    print $fo2 $oldHeader;
    print $fo2 $common1;
  }

  for my $i (0 .. $#strats1)
  {
    if ($diffNormal[$i])
    {
      print $fo1 "ORIGINAL\n";
      print $fo1 $strats1[$i]{text};
      print $fo1 "\nNEW\n";
      print $fo1 $strats2[$i]{text};
      print $fo1 "\n";
    }
    elsif ($diffOr[$i])
    {
      print $fo2 "ORIGINAL\n";
      print $fo2 $strats1[$i]{text};
      print $fo2 "\nNEW\n";
      print $fo2 $strats2[$i]{text};
      print $fo2 "\n";
    }
  }
}

close $fh1;
close $fh2;

close $fo1;
close $fo2;

print "\n";

printf("%2s %8s %8s %8s\n", "c", "same", "diff", "diffOr");

for my $i (4 .. $#equalStats)
{
  printf("%2d %8d %8d %8d\n", $i, $equalStats[$i], $diffStats[$i],
    $diffOrStats[$i]);
}
print "\n";

printf("%2s %8s %8d %8d\n", "", "", $diffCum, $diffOrCum);
print "\n";


sub advance
{
  my ($fh_ref, $lno_ref, $cards_ref, $holding_ref, $header_ref) = @_;
  
  my $line;
  while ($line = <$fh_ref>)
  {
    $$lno_ref++;
    if ($line =~ /^Cards  4: (\d+)/)
    {
      $$holding_ref = $1;
      $$cards_ref = 4;
      $line =~ s///g;
      $$header_ref = $line;
      return;
    }
  }
}


sub slurp
{
  my ($fh_ref, $lno_ref, $common_ref, 
    $strats_ref, $header_ref, $cards_ref, $holding_ref) = @_;

  my $line;
  my $num_empty = 0;
  $$common_ref = "";
  @$strats_ref = ();
  $$header_ref = "";

  while ($line = <$fh_ref>)
  {
    $$lno_ref++;
    $num_empty++ if $line =~ /^\s*$/;
    $line =~ s///g;
    $$common_ref .= $line;
    last if $num_empty == 5;
  }

  while ($line = <$fh_ref>)
  {
    if ($line =~ /^Cards\s+(\d+):\s+(\d+)/)
    {
      $$cards_ref = $1;
      $$holding_ref = $2;
      $line =~ s///g;
      $$header_ref = $line;
      return;
    }
    elsif ($line =~ /^Strategy \#(\d+) \[c (\d+)\/(\d+), w (\d+)\/(\d+), d (\d+)\/(\w+)\]/)
    {
      my $no = 1 + $#$strats_ref;
      $strats_ref->[$no]{number} = $1;
      $strats_ref->[$no]{complexity} = $2;
      $strats_ref->[$no]{row} = $3;
      $strats_ref->[$no]{weight} = $4;
      $strats_ref->[$no]{companion} = $5;

      $line =~ s///g;
      $strats_ref->[$no]{text} = $line;
      while ($line = <$fh_ref>)
      {
        last if $line =~ /^\s*$/;
        $line =~ s///g;
        $strats_ref->[$no]{text} .= $line;
      }
    }
  }
}


sub stratDiff
{
  my ($strat1_ref, $strat2_ref) = @_;

  return 1 if ($strat1_ref->{number} != $strat2_ref->{number});
  return 1 if ($strat1_ref->{complexity} != $strat2_ref->{complexity});
  return 1 if ($strat1_ref->{row} != $strat2_ref->{row});
  return 1 if ($strat1_ref->{weight} != $strat2_ref->{weight});
  return 1 if ($strat1_ref->{companion} != $strat2_ref->{companion});
  return 1 if ($strat1_ref->{text} ne $strat2_ref->{text});
  return 0;
}


