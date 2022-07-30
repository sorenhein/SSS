#!perl

use strict;
use warnings;

# Count the cover's that are not stated in words.

my $file = shift;
open my $fh, '<', $file or die $!;

my $numText = 0;
my $numUntext = 0;
my $numSingular = 0;
my $numTwoTops = 0;
my $numAnyTwo = 0;
my $numSingularFull = 0;
my $numRest = 0;
my $lno = 0;

my @oppsMax;

while (my $line = <$fh>)
{
  $lno++;

  if ($line =~ /^Rank North/)
  {
    @oppsMax = ();

    while (my $line2 = <$fh>)
    {
      $lno++;

      chomp $line2;
      $line2 =~ s///g;
      last if $line2 eq "";
      next unless $line2 =~ /Opps\s+(\d+)\s+/;
      unshift @oppsMax, $1;
    }
  }
  elsif ($line =~ /^Strategy \#(\d+) \[c (\d+)\/(\d+), w (\d+)\/(\d+), d (\d+)\/(\w+)\]/)
  {
    while (my $line2 = <$fh>)
    {
      $lno++;

      chomp $line2;
      $line2 =~ s///g;
      $line2 =~ s/^\s*$//g;
      last if $line2 eq "";

      $line2 =~ s/^\* //;
      $line2 =~ s/^\s+//;
      $line2 =~ s/; or$//;

      if ($line2 =~ /unused/)
      {
        $numUntext++;
        my @splits = split /\s+/, $line2;
        my $num = $#splits - 6;
        $num-- if $line2 =~ / sym /;

        my $i = 1;
        my $j = 0;
        my $state = 0;
        my $oper = -1; # == 0, <= 1, >= 2, - 3
        my @opps;

        my $dcount = $splits[0] =~ tr/1//;
        $dcount >>= 1 if $line2 =~ /sym/;

        while ($i <= $num)
        {
          my $f = $splits[$i];
          if ($state == 0)
          {
            # Expecting an operator, a number or a range.
            if ($f =~ /^\d+$/)
            {
              $opps[$j]{oper} = 0;
              $opps[$j]{lower} = $f;
              $opps[$j]{upper} = -1;
  
              $state = 0;
              $oper = -1;
              $i++;
              $j++;
            }
            elsif ($f eq "==")
            {
              $state = 1;
              $oper = 0;
              $i++;
            }
            elsif ($f eq "<=")
            {
              $state = 1;
              $oper = 1;
              $i++;
            }
            elsif ($f eq ">=")
            {
              $state = 1;
              $oper = 2;
              $i++;
            }
            elsif ($f =~ /^(\d+)-(\d+)$/)
            {
              $opps[$j]{oper} = 3;
              $opps[$j]{lower} = $1;
              $opps[$j]{upper} = $2;
  
              $state = 0;
              $oper = -1;
              $i++;
              $j++;
            }
            elsif ($f eq "unused")
            {
              $opps[$j]{oper} = -1;
              $opps[$j]{lower} = -1;
              $opps[$j]{upper} = -1;
  
              $state = 0;
              $oper = -1;
              $i++;
              $j++;
            }
            else
            {
              print "i $i j $j state $state oper $oper\n";
              for my $a (@splits)
              {
                print "  .$a.\n";
              }
              die $line2;
            }
          }
          elsif ($state == 1)
          {
            # Expected a number following an operator.
            die "$f not a number" unless $f =~ /^\d+$/;
            die "No operator" unless $oper >= 0;

            $opps[$j]{oper} = $oper;
            $opps[$j]{lower} = $f;
            $opps[$j]{upper} = -1;

            $state = 0;
            $oper = -1;
            $i++;
            $j++;
          }
          else
          {
            die "Error";
          }
        }

        if (singular(\@opps, \@oppsMax))
        {
          $numSingular++;
          if ($dcount != 1)
          {
            print "COUNT1 $line2\n";
          }
        }
        elsif (singular_full(\@opps, \@oppsMax))
        {
          $numSingularFull++;
          if ($dcount != 1)
          {
            print "COUNT2 $line2\n";
          }
        }
        elsif (two_tops(\@opps))
        {
          $numTwoTops++;
          if ($dcount == 1)
          {
            print "COUNT3 $line2\n";
          }
        }
        elsif (any_two(\@opps))
        {
          $numAnyTwo++;
          if ($dcount == 1)
          {
            print "COUNT4 $line2\n";
          }
        }
        else
        {
          print "$line2\n";
          # print "$lno: $line2\n";
          $numRest++;
          if ($dcount == 1)
          {
            print "COUNT5 $line2\n";
          }
        }
      }
      else
      {
        $numText++;
      }
    }
  }
}

close $fh;

print "\n";
printf("Number text     %7d\n", $numText);
printf("Number untext   %7d\n", $numUntext);
print "\n";

printf("Number singular %7d\n", $numSingular);
printf("Number singfull %7d\n", $numSingularFull);
printf("Number two tops %7d\n", $numTwoTops);
printf("Number two any  %7d\n", $numAnyTwo);
printf("Number rest     %7d\n", $numRest);


sub singular
{
  my ($opps_ref, $oppsMax_ref) = @_;

  if ($#$opps_ref > $#$oppsMax_ref+1)
  {
    dump_opps($opps_ref, $oppsMax_ref);
    die unless $#$opps_ref <= $#$oppsMax_ref+1;
  }

  if ($opps_ref->[0]{oper} != 0)
  {
    # Length must not be unused
    return 0;
  }

  my $len = $opps_ref->[0]{lower};

  my $lenMax = 0;
  $lenMax += $_ for @$oppsMax_ref;

  my $sumWest = 0;
  my $sumEast = 0;
  my $j = $#$oppsMax_ref + 1 - $#$opps_ref;

  for my $i (1 .. $#$opps_ref)
  {
    my $oper = $opps_ref->[$i]{oper};
    if ($oper > 0)
    {
      # Count must be unused or a single number.
      return 0;
    }

    if ($oper == 0)
    {
      # Only look a single numbers.
      my $tops = $opps_ref->[$i]{lower};
      $sumWest += $tops;

      die unless defined $tops;
      if (! defined $oppsMax_ref->[$j])
      {
        print "i $i j $j\n";
        dump_opps($opps_ref, $oppsMax_ref);
        die unless defined $oppsMax_ref->[$j];
      }

      $sumEast += $oppsMax_ref->[$j] - $tops;
    }
    $j++;
  }

  if ($sumWest == $len || $sumEast == $lenMax - $len)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}


sub two_tops
{
  # The highest two tops, no length
  my $opps_ref = pop;

  if ($opps_ref->[0]{oper} != -1)
  {
    # Length must be unset
    return 0;
  }

  my $c = $#$opps_ref;
  for my $i (1 .. $c-2)
  {
    my $oper = $opps_ref->[$i]{oper};
    if ($oper != -1)
    {
      # Low tops must be unset
      return 0;
    }
  }

  for my $i ($c-1 .. $c)
  {
    my $oper = $opps_ref->[$i]{oper};
    if ($oper != 0)
    {
      # Two high tops must be equal-sets
      return 0;
    }
  }

  return 1;
}


sub any_two
{
  # Any two tops, no length
  my $opps_ref = pop;

  if ($opps_ref->[0]{oper} != -1)
  {
    # Length must be unset
    return 0;
  }

  my $c = $#$opps_ref;
  my $used = 0;
  for my $i (1 .. $c)
  {
    my $oper = $opps_ref->[$i]{oper};
    next if $oper == -1;
    return 0 unless $oper == 0;
    $used++;
  }

  if ($used == 2)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}


sub singular_full
{
  # All tops except x exactly; length given exactly
  my ($opps_ref, $oppsMax_ref) = @_;

# dump_opps($opps_ref, $oppsMax_ref);
  if ($opps_ref->[0]{oper} != 0)
  {
    # Length must be set to equal
    return 0;
  }

  my $len = $opps_ref->[0]{lower};

  my $lenMax = 0;
  $lenMax += $_ for @$oppsMax_ref;

  my $sumWest = 0;
  my $sumEast = 0;

  my $j = $#$oppsMax_ref + 1 - $#$opps_ref + 1; # Last +1 for start at 2

# print "len $len opps $lenMax\n";

  my $c = $#$opps_ref;
  for my $i (2 .. $c)
  {
    my $oper = $opps_ref->[$i]{oper};
    return 0 if $oper == -1;

    my $tops = $opps_ref->[$i]{lower};
    $sumWest += $tops;
    $sumEast += $oppsMax_ref->[$j] - $tops;

    $j++;
  }

  if ($#$oppsMax_ref + 1 == $#$opps_ref)
  {
    # The number of x's is implicitly also given.
    return 1;
  }

  # Otherwise not clear.

# print "sums $sumWest $sumEast\n";

  if ($sumWest == $len || $sumEast == $lenMax - $len)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}


sub dump_opps
{
  my ($opps_ref, $oppsMax_ref) = @_;

  for my $a (@$opps_ref)
  {
    print $a->{oper} . " " . $a->{lower} . " " .
      $a->{upper} . "\n";
  }
  print "\n";

  for my $b (@$oppsMax_ref)
  {
    print "$b\n";
  }
  print "\n";
}

