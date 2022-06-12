#!perl

use strict;
use warnings;

# Count some cover statistics.
# Process a single file that contains depths and timings.

my (@countStrat, @complexity, @rowcomplexity);
my (@stackmax, @comps, @steps);
my @times;

for my $d (0 .. 5)
{
  for my $s ('s', 'a', 'g')
  {
    $countStrat[$d]{$s} = 0;
  }
}

my $file = shift;
open my $fh, '<', $file or die $!;

my ($stack, $ttff, $numsol, $smax, $comps, $steps, $branch);
my $lno = 0;
my ($numcc, $smaxRun, $compsRun, $stepsRun);
$numcc = 0;
$smaxRun = 0;
$compsRun = 0;
$stepsRun = 0;
my ($c, $r, $w1, $w2, $depth, $symm);

while (my $line = <$fh>)
{
  $lno++;

  if ($line =~ /^CC\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+\.\d+)/)
  {
    ($stack, $ttff, $numsol, $smax, $comps, $steps, $branch) =
      ($1, $2, $3, $4, $5, $6, $7);

    $smaxRun += $smax;
    $compsRun += $comps;
    $stepsRun += $steps;
    $numcc++;
  }
  elsif ($line =~ /^Strategy \#(\d+) \[c (\d+)\/(\d+), w (\d+)\/(\d+), d (\d+)\/(\w+)\]/)
  {
    ($c, $r, $w1, $w2, $depth, $symm) = ($2, $3, $4, $5, $6, $7);

    $countStrat[$depth]{$symm}++;
    $complexity[$depth]{$symm} += $c;
    $rowcomplexity[$depth]{$symm} += $r;

    $stackmax[$depth]{$symm} += $smaxRun;
    $comps[$depth]{$symm} += $compsRun;
    $steps[$depth]{$symm} += $stepsRun;

    $numcc = 0;
    $smaxRun = 0;
    $compsRun = 0;
    $stepsRun = 0;
  }
  elsif ($line =~/^Took\s+(\d+\.\d+) ms/)
  {
    $times[$depth]{$symm} += $1;
  }
}

close $fh;

for my $d (0 .. $#countStrat)
{
  depthDump(
    $d,
    \%{$countStrat[$d]},
    \%{$complexity[$d]},
    \%{$rowcomplexity[$d]},
    \%{$stackmax[$d]},
    \%{$comps[$d]},
    \%{$steps[$d]},
    \%{$times[$d]});
  printf("\n\n");
}


sub dumpEntry
{
  my ($num, $denom) = @_;
  if ($denom == 0)
  {
    return "-";
  }
  else
  {
    return sprintf("%12.2f", $num / $denom);
  }
}


sub dumpLine
{
  my ($text, $ref, $count_ref) = @_;

  my $rsum = 0;
  $rsum += $ref->{s} if defined $ref->{s};
  $rsum += $ref->{a} if defined $ref->{a};
  $rsum += $ref->{g} if defined $ref->{g};
  my $csum = $count_ref->{s} + $count_ref->{a} + $count_ref->{g};

  printf("%-12s%12s%12s%12s%12s\n",
    $text,
    dumpEntry($ref->{s}, $count_ref->{s}),
    dumpEntry($ref->{a}, $count_ref->{a}),
    dumpEntry($ref->{g}, $count_ref->{g}),
    dumpEntry($rsum, $csum));
    
}


sub depthDump
{
  my ($depth, $count_ref, $compl_ref, $row_ref, 
    $stack_ref, $comps_ref, $steps_ref, $times_ref) = @_;

  printf("Depth %-6s%12s%12s%12s%12s\n", $depth, 
    "symm", "anti", "general", "all");
  printf("%-12s%12d%12d%12d%12d\n", "Count",
    $count_ref->{s}, 
    $count_ref->{a}, 
    $count_ref->{g},
    $count_ref->{s} + $count_ref->{a} + $count_ref->{g});

  dumpLine("Time (ms)", $times_ref, $count_ref);
  printf("\n");
  dumpLine("Complexity", $compl_ref, $count_ref);
  dumpLine("Row compl.", $row_ref, $count_ref);
  printf("\n");
  dumpLine("Stack", $stack_ref, $count_ref);
  dumpLine("Compares", $comps_ref, $count_ref);
  dumpLine("Steps", $steps_ref, $count_ref);
}

