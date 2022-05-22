#!perl

use strict;
use warnings;

# Count some cover statistics.

my $file = shift;
open my $fh, '<', $file or die $!;

my $lno = 0;
my (@CC_ttff, @CC_numsol, $CCn);
my ($CC_smax, $CC_comps, $CC_steps, $CC_branch);
my (@RR_ttff, @RR_numsol, $RRn);
my ($RR_smax, $RR_comps, $RR_steps, $RR_branch);

my $CC_worst_stack = 0;
my $lnoprev = -99;

while (my $line = <$fh>)
{
  $lno++;

  if ($line =~ /^CC\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+\.\d+)/)
  {
    my ($stack, $ttff, $numsol, $smax, $comps, $steps, $branch) =
      ($1, $2, $3, $4, $5, $6, $7);
    
    $CC_ttff[$ttff]++;
    $CC_numsol[$numsol]++;
    $CC_smax += $smax;
    $CC_comps += $comps;
    $CC_steps += $steps;
    $CC_branch += $branch;
    $CCn++;

    if ($smax > $CC_worst_stack)
    {
      $CC_worst_stack = $smax;
      print "New worst stack in line $lno: $smax\n";
    }

    if ($lno == $lnoprev+2)
    {
      if ($numsol != 0)
      {
        print "Unexpected in line $lno\n";
      }
    }

    $lnoprev = $lno;
  }
  elsif ($line =~ /^RR\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+\.\d+)/)
  {
    my ($stack, $ttff, $numsol, $smax, $comps, $steps, $branch) =
      ($1, $2, $3, $4, $5, $6, $7);
    
    $RR_ttff[$ttff]++;
    $RR_numsol[$numsol]++;
    $RR_smax += $smax;
    $RR_comps += $comps;
    $RR_steps += $steps;
    $RR_branch += $branch;
    $RRn++;
  }
}

close $fh;

mdump(\@CC_ttff, "CC ttff");
mdump(\@CC_numsol, "CC numsol");
printf("%-12s%12.2f\n", "CC smax", $CC_smax / $CCn);
printf("%-12s%12.2f\n", "CC comps", $CC_comps / $CCn);
printf("%-12s%12.2f\n", "CC steps", $CC_steps / $CCn);
printf("%-12s%12.2f\n", "branch", $CC_branch / $CCn);
print "\n";

mdump(\@RR_ttff, "RR ttff");
mdump(\@RR_numsol, "RR numsol");
if ($RRn)
{
  printf("%-12s%12.2f\n", "RR smax", $RR_smax / $RRn);
  printf("%-12s%12.2f\n", "RR comps", $RR_comps / $RRn);
  printf("%-12s%12.2f\n", "RR steps", $RR_steps / $RRn);
  printf("%-12s%12.2f\n", "branch", $RR_branch / $RRn);
  print "\n";
}

hdump(\@CC_numsol, "CC numsol");
hdump(\@RR_numsol, "RR numsol");


sub mdump
{
  my ($href, $name) = @_;
  my $sum = 0;
  my $cnt = 0;
  for my $i (0 .. $#$href)
  {
    next unless defined $href->[$i];
    $sum += $href->[$i] * $i;
    $cnt += $href->[$i];
  }

  if ($cnt == 0)
  {
    print("-\n");
  }
  else
  {
    printf("%-12s%12.2f\n", $name, $sum / $cnt);
  }
}


sub hdump
{
  my ($href, $name) = @_;
  my $sum = 0;
  my $cnt = 0;
  print "$name\n";
  for my $i (0 .. $#$href)
  {
    next unless defined $href->[$i];
    printf("%-12d%12d\n", $i, $href->[$i]);
  }
  print "\n";
}
