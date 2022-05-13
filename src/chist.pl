#!perl

use strict;
use warnings;

# Count some cover statistics.

my $file = shift;
open my $fh, '<', $file or die $!;

my $lno = 0;
my (@CC_ttff, @CC_numsol, @CC_smax, @CC_comps, @CC_steps, $CC_branch, $CCn);
my (@RR_ttff, @RR_numsol, @RR_smax, @RR_comps, @RR_steps, $RR_branch, $RRn);

while (my $line = <$fh>)
{
  $lno++;

  if ($line =~ /^CC\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+\.\d+)/)
  {
    my ($stack, $ttff, $numsol, $smax, $comps, $steps, $branch) =
      ($1, $2, $3, $4, $5, $6, $7);
    
    $CC_ttff[$ttff]++;
    $CC_numsol[$numsol]++;
    $CC_smax[$smax]++;
    $CC_comps[$comps]++;
    $CC_steps[$steps]++;
    $CC_branch += $branch;
    $CCn++;
  }
  elsif ($line =~ /^RR\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+\.\d+)/)
  {
    my ($stack, $ttff, $numsol, $smax, $comps, $steps, $branch) =
      ($1, $2, $3, $4, $5, $6, $7);
    
    $RR_ttff[$ttff]++;
    $RR_numsol[$numsol]++;
    $RR_smax[$smax]++;
    $RR_comps[$comps]++;
    $RR_steps[$steps]++;
    $RR_branch += $branch;
    $RRn++;
  }
}

close $fh;

mdump(\@CC_ttff, "CC ttff");
mdump(\@CC_numsol, "CC numsol");
mdump(\@CC_smax, "CC smax");
mdump(\@CC_comps, "CC comps");
mdump(\@CC_steps, "CC steps");
printf("%-12s%8.2f\n", "branch", $CC_branch / $CCn);
print "\n";

mdump(\@RR_ttff, "RR ttff");
mdump(\@RR_numsol, "RR numsol");
mdump(\@RR_smax, "RR smax");
mdump(\@RR_comps, "RR comps");
mdump(\@RR_steps, "RR steps");
printf("%-12s%8.2f\n", "branch", $RR_branch / $RRn);


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

  printf("%-12s%8.2f\n", $name, $sum / $cnt);
}
