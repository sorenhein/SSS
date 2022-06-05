#!perl

use strict;
use warnings;

# Count some cover statistics.
# TODO Add histogram of primary complexity
# TODO Add #unexplained

my (%CC_ttff, %CC_numsol, %CCn);
my (%CC_smax, %CC_comps, %CC_steps, %CC_branch, %CC_worst_stack);
my (%CC_stratsum, %CC_rowsum, %CCs, %CC_unex);

my (%RR_ttff, %RR_numsol, %RRn);
my (%RR_smax, %RR_comps, %RR_steps, %RR_branch);
my (%RR_stratsum, %RR_rowsum, %RRs, %RR_unex);

for my $file (@ARGV)
{
  open my $fh, '<', $file or die $!;
  my $lno = 0;

  $CC_worst_stack{$file} = 0;
  while (my $line = <$fh>)
  {
    $lno++;

    if ($line =~ /^CC\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+\.\d+)/)
    {
      my ($stack, $ttff, $numsol, $smax, $comps, $steps, $branch) =
        ($1, $2, $3, $4, $5, $6, $7);
    
      $CC_ttff{$file}[$ttff]++;
      $CC_numsol{$file}[$numsol]++;
      $CC_smax{$file} += $smax;
      $CC_comps{$file} += $comps;
      $CC_steps{$file} += $steps;
      $CC_branch{$file} += $branch;
      $CCn{$file}++;

      if ($smax > $CC_worst_stack{$file})
      {
        $CC_worst_stack{$file} = $smax;
      }
    }
    elsif ($line =~ /^RR\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+\.\d+)/)
    {
      my ($stack, $ttff, $numsol, $smax, $comps, $steps, $branch) =
        ($1, $2, $3, $4, $5, $6, $7);
    
      $RR_ttff{$file}[$ttff]++;
      $RR_numsol{$file}[$numsol]++;
      $RR_smax{$file} += $smax;
      $RR_comps{$file} += $comps;
      $RR_steps{$file} += $steps;
      $RR_branch{$file} += $branch;
      $RRn{$file}++;
    }
    elsif ($line =~ /^Strategy \#(\d+) \[c (\d+)\/(\d+), w (\d+)\/(\d+)\]/)
    {
      my ($c, $r, $w1, $w2) = ($1, $2, $3, $4);

      $CC_stratsum{$file} += $c;
      $CC_rowsum{$file} += $r;
      $CCs{$file}++;
      $CC_unex{$file}++ if ($line =~ /Unexplained/);
    }
    elsif ($line =~ /^Manual strategy \#(\d+) \[c (\d+)\/(\d+), w (\d+)\/(\d+)\]/)
    {
      my ($c, $r, $w1, $w2) = ($1, $2, $3, $4);

      $RR_stratsum{$file} += $c;
      $RR_rowsum{$file} += $r;
      $RRs{$file}++;
      $RR_unex{$file}++ if ($line =~ /Unexplained/);
    }
  }

  close $fh;
}

header_dump(\%CC_ttff);

mdump(\%CC_ttff, "CC ttff");
mdump(\%CC_numsol, "CC numsol");
rdump(\%CC_smax, \%CCn, "CC smax");
rdump(\%CC_comps, \%CCn, "CC comps");
rdump(\%CC_steps, \%CCn, "CC steps");
rdump(\%CC_branch, \%CCn, "branch");
onedump(\%CC_worst_stack, "wc stack");
print "\n";

mdump(\%RR_ttff, "RR ttff");
mdump(\%RR_numsol, "RR numsol");
rdump(\%RR_smax, \%RRn, "RR smax");
rdump(\%RR_comps, \%RRn, "RR comps");
rdump(\%RR_steps, \%RRn, "RR steps");
rdump(\%RR_branch, \%RRn, "branch");
print "\n";

hdump(\%CC_numsol, "CC numsol");
hdump(\%RR_numsol, "RR numsol");
print "\n";

rdump(\%CC_stratsum, \%CCs, "CC compl");
rdump(\%CC_rowsum, \%CCs, "CC rowc");
onedump(\%CCs, "CC strats");
onedump(\%CC_unex, "CC unex");
print "\n";

rdump(\%RR_stratsum, \%RRs, "RR compl");
rdump(\%RR_rowsum, \%RRs, "RR rowc");
onedump(\%RRs, "RR strats");
onedump(\%RR_unex, "RR unex");
print "\n";



sub rdump
{
  my ($numref, $denomref, $text) = @_;
  printf("%-12s", $text);
  for my $file (sort keys %$numref)
  {
    if ($denomref->{$file})
    {
      printf("%12.2f", $numref->{$file} / $denomref->{$file});
    }
    else
    {
      printf("%12s", "-");
    }
  }
  print("\n");
}


sub onedump
{
  my ($numref, $text) = @_;
  printf("%-12s", $text);
  for my $file (sort keys %$numref)
  {
    printf("%12.2f", $numref->{$file});
  }
  print("\n");
}


sub header_dump
{
  my $cref = shift;
  printf("%-12s", "param");
  my $i = 0;
  for my $file (sort keys %$cref)
  {
    printf("%12s", $file);
    $i++;
  }
  printf("\n");
  my $ul = "-" x (12 + 12 * $i);
  print $ul, "\n";
}


sub mdump
{
  my ($href, $name) = @_;
  my (%sum, %cnt);
  for my $file (sort keys %$href)
  {
    $sum{$file} = 0;
    $cnt{$file} = 0;

    for my $i (0 .. $#{$href->{$file}})
    {
      next unless defined $href->{$file}[$i];
      $sum{$file} += $href->{$file}[$i] * $i;
      $cnt{$file} += $href->{$file}[$i];
    }
  }

  printf("%-12s", $name);
  for my $file (sort keys %$href)
  {
    if ($cnt{$file} == 0)
    {
      printf("%12s", "-\n");
    }
    else
    {
      printf("%12.2f", $sum{$file} / $cnt{$file});
    }
  }
  print("\n");
}


sub hdump
{
  my ($href, $name) = @_;
  my (%sum, %cnt);
  my $max = 0;
  for my $file (sort keys %$href)
  {
    $sum{$file} = 0;
    $cnt{$file} = 0;
    if ($#{$href->{$file}} > $max)
    {
      $max = $#{$href->{$file}};
    }
  }

  print "$name\n";
  for my $i (0 .. $max)
  {
    printf("%-12d", $i);
    for my $file (sort keys %$href)
    {
      if (defined $href->{$file}[$i])
      {
        printf("%12d", $href->{$file}[$i]);
      }
      else
      {
        printf("%12s", "-");
      }
    }
    print "\n";
  }

  print "\n";
}
