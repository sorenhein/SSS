#!perl

use strict;
use warnings;

# Count the cover's that are not stated in words.

my $file = shift;
open my $fh, '<', $file or die $!;

my $TEXT = 0;
my $UNTEXT = 1;
my $SINGULAR = 2;
my $SINGULAR_FULL = 3;

my $TWO_TOPS_EQUAL = 4;
my $TWO_TOPS_LENGTH_EQUAL = 5;
my $TWO_TOPS_LENGTHVAR_EQUAL = 6;
my $ANY_TWO_EQUAL = 7;
my $ANY_TWO_LENGTH_EQUAL = 8;
my $ANY_TWO_LENGTHVAR_EQUAL = 9;
my $TWO_TOPS_EQUAL_ONEVAR = 10;
my $ANY_TWO_EQUAL_ONEVAR = 11;

my $THREE_TOPS_EQUAL = 12;
my $THREE_TOPS_LENGTH_EQUAL = 13;
my $THREE_TOPS_LENGTHVAR_EQUAL = 14;
my $ANY_THREE_EQUAL = 15;
my $ANY_THREE_LENGTH_EQUAL = 16;
my $ANY_THREE_LENGTHVAR_EQUAL = 17;
my $THREE_TOPS_EQUAL_ONEVAR = 18;
my $ANY_THREE_EQUAL_ONEVAR = 19;

my $FOUR_TOPS_EQUAL = 20;
my $FOUR_TOPS_LENGTH_EQUAL = 21;
my $FOUR_TOPS_LENGTHVAR_EQUAL = 22;
my $ANY_FOUR_EQUAL = 23;
my $ANY_FOUR_LENGTH_EQUAL = 24;
my $ANY_FOUR_LENGTHVAR_EQUAL = 25;
my $FOUR_TOPS_EQUAL_ONEVAR = 26;
my $ANY_FOUR_EQUAL_ONEVAR = 27;

my $REST = 28;

my (@numbers, @complexities, @titles);
for my $i (0 .. $REST)
{
  $numbers[$i] = 0;
  $complexities[$i] = 0;
}

$titles[$TEXT] = "Text";
$titles[$UNTEXT] = "Untext";
$titles[$SINGULAR] = "Singular";
$titles[$SINGULAR_FULL] = "Singfull";

$titles[$TWO_TOPS_EQUAL] = "Top2 eq";
$titles[$TWO_TOPS_LENGTH_EQUAL] = "Top2 eqL";
$titles[$TWO_TOPS_LENGTHVAR_EQUAL] = "Top2 eqV";
$titles[$ANY_TWO_EQUAL] = "Any2 eq";
$titles[$ANY_TWO_LENGTH_EQUAL] = "Any2 eqL";
$titles[$ANY_TWO_LENGTHVAR_EQUAL] = "Any2 eqV";
$titles[$TWO_TOPS_EQUAL_ONEVAR] = "Top2 eq+";
$titles[$ANY_TWO_EQUAL_ONEVAR] = "Any2 eq+";

$titles[$THREE_TOPS_EQUAL] = "Top3 eq";
$titles[$THREE_TOPS_LENGTH_EQUAL] = "Top3 eqL";
$titles[$THREE_TOPS_LENGTHVAR_EQUAL] = "Top3 eqV";
$titles[$ANY_THREE_EQUAL] = "Any3 eq";
$titles[$ANY_THREE_LENGTH_EQUAL] = "Any3 eqL";
$titles[$ANY_THREE_LENGTHVAR_EQUAL] = "Any3 eqV";
$titles[$THREE_TOPS_EQUAL_ONEVAR] = "Top3 eq+";
$titles[$ANY_THREE_EQUAL_ONEVAR] = "Any3 eq+";

$titles[$FOUR_TOPS_EQUAL] = "Top4 eq";
$titles[$FOUR_TOPS_LENGTH_EQUAL] = "Top4 eqL";
$titles[$FOUR_TOPS_LENGTHVAR_EQUAL] = "Top4 eqV";
$titles[$ANY_FOUR_EQUAL] = "Any4 eq";
$titles[$ANY_FOUR_LENGTH_EQUAL] = "Any4 eqL";
$titles[$ANY_FOUR_LENGTHVAR_EQUAL] = "Any4 eqV";
$titles[$FOUR_TOPS_EQUAL_ONEVAR] = "Top4 eq+";
$titles[$ANY_FOUR_EQUAL_ONEVAR] = "Any4 eq+";

$titles[$REST] = "Rest";

my @oppsMax;
my $lno = 0;

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
        my @splits = split /\s+/, $line2;
        my $num = $#splits - 6;
        $num-- if $line2 =~ / sym /;

        my $compl = $splits[$#splits];
        $compl =~ /^\[(\d+)/;
        $compl = $1;

        $numbers[$UNTEXT]++;
        $complexities[$UNTEXT] += $compl;

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
          $numbers[$SINGULAR]++;
          $complexities[$SINGULAR] += $compl;
          if ($dcount != 1)
          {
            print "COUNT1 $line2\n";
          }
        }
        elsif (singular_full(\@opps, \@oppsMax))
        {
          $numbers[$SINGULAR_FULL]++;
          $complexities[$SINGULAR_FULL] += $compl;
          if ($dcount != 1)
          {
            print "COUNT2 $line2\n";
          }
        }
        elsif (N_tops_equal(\@opps, 2))
        {
          $numbers[$TWO_TOPS_EQUAL]++;
          $complexities[$TWO_TOPS_EQUAL] += $compl;
          if ($dcount == 1)
          {
            print "COUNT3 $line2\n";
          }
        }
        elsif (any_N_equal(\@opps, 2))
        {
          $numbers[$ANY_TWO_EQUAL]++;
          $complexities[$ANY_TWO_EQUAL] += $compl;
          if ($dcount == 1)
          {
            print "COUNT4 $line2\n";
          }
        }
        elsif (N_tops_length_equal(\@opps, 2))
        {
          $numbers[$TWO_TOPS_LENGTH_EQUAL]++;
          $complexities[$TWO_TOPS_LENGTH_EQUAL] += $compl;
          if ($dcount == 1)
          {
            print "COUNT3 $line2\n";
          }
        }
        elsif (any_N_length_equal(\@opps, 2))
        {
          $numbers[$ANY_TWO_LENGTH_EQUAL]++;
          $complexities[$ANY_TWO_LENGTH_EQUAL] += $compl;
          if ($dcount == 1)
          {
            print "COUNT4 $line2\n";
          }
        }
        elsif (N_tops_lengthvar_equal(\@opps, 2))
        {
          $numbers[$TWO_TOPS_LENGTHVAR_EQUAL]++;
          $complexities[$TWO_TOPS_LENGTHVAR_EQUAL] += $compl;
          if ($dcount == 1)
          {
            print "COUNT3 $line2\n";
          }
        }
        elsif (any_N_lengthvar_equal(\@opps, 2))
        {
          $numbers[$ANY_TWO_LENGTHVAR_EQUAL]++;
          $complexities[$ANY_TWO_LENGTHVAR_EQUAL] += $compl;
          if ($dcount == 1)
          {
            print "COUNT4 $line2\n";
          }
        }
        elsif (N_tops_equal_onevar(\@opps, 2))
        {
          $numbers[$TWO_TOPS_EQUAL_ONEVAR]++;
          $complexities[$TWO_TOPS_EQUAL_ONEVAR] += $compl;
          if ($dcount == 1)
          {
            print "COUNT3 $line2\n";
          }
        }
        elsif (any_N_equal_onevar(\@opps, 2))
        {
          $numbers[$ANY_TWO_EQUAL_ONEVAR]++;
          $complexities[$ANY_TWO_EQUAL_ONEVAR] += $compl;
          if ($dcount == 1)
          {
            print "COUNT4 $line2\n";
          }
        }


        elsif (N_tops_equal(\@opps, 3))
        {
          $numbers[$THREE_TOPS_EQUAL]++;
          $complexities[$THREE_TOPS_EQUAL] += $compl;
          if ($dcount == 1)
          {
            print "COUNT3 $line2\n";
          }
        }
        elsif (any_N_equal(\@opps, 3))
        {
          $numbers[$ANY_THREE_EQUAL]++;
          $complexities[$ANY_THREE_EQUAL] += $compl;
          if ($dcount == 1)
          {
            print "COUNT4 $line2\n";
          }
        }
        elsif (N_tops_length_equal(\@opps, 3))
        {
          $numbers[$THREE_TOPS_LENGTH_EQUAL]++;
          $complexities[$THREE_TOPS_LENGTH_EQUAL] += $compl;
          if ($dcount == 1)
          {
            print "COUNT3 $line2\n";
          }
        }
        elsif (any_N_length_equal(\@opps, 3))
        {
          $numbers[$ANY_THREE_LENGTH_EQUAL]++;
          $complexities[$ANY_THREE_LENGTH_EQUAL] += $compl;
          if ($dcount == 1)
          {
            print "COUNT4 $line2\n";
          }
        }
        elsif (N_tops_lengthvar_equal(\@opps, 3))
        {
          $numbers[$THREE_TOPS_LENGTHVAR_EQUAL]++;
          $complexities[$THREE_TOPS_LENGTHVAR_EQUAL] += $compl;
          if ($dcount == 1)
          {
            print "COUNT3 $line2\n";
          }
        }
        elsif (any_N_lengthvar_equal(\@opps, 3))
        {
          $numbers[$ANY_THREE_LENGTHVAR_EQUAL]++;
          $complexities[$ANY_THREE_LENGTHVAR_EQUAL] += $compl;
          if ($dcount == 1)
          {
            print "COUNT4 $line2\n";
          }
        }
        elsif (N_tops_equal_onevar(\@opps, 3))
        {
          $numbers[$THREE_TOPS_EQUAL_ONEVAR]++;
          $complexities[$THREE_TOPS_EQUAL_ONEVAR] += $compl;
          if ($dcount == 1)
          {
            print "COUNT3 $line2\n";
          }
        }
        elsif (any_N_equal_onevar(\@opps, 3))
        {
          $numbers[$ANY_THREE_EQUAL_ONEVAR]++;
          $complexities[$ANY_THREE_EQUAL_ONEVAR] += $compl;
          if ($dcount == 1)
          {
            print "COUNT4 $line2\n";
          }
        }


        elsif (N_tops_equal(\@opps, 4))
        {
          $numbers[$FOUR_TOPS_EQUAL]++;
          $complexities[$FOUR_TOPS_EQUAL] += $compl;
          if ($dcount == 1)
          {
            print "COUNT3 $line2\n";
          }
        }
        elsif (any_N_equal(\@opps, 4))
        {
          $numbers[$ANY_FOUR_EQUAL]++;
          $complexities[$ANY_FOUR_EQUAL] += $compl;
          if ($dcount == 1)
          {
            print "COUNT4 $line2\n";
          }
        }
        elsif (N_tops_length_equal(\@opps, 4))
        {
          $numbers[$FOUR_TOPS_LENGTH_EQUAL]++;
          $complexities[$FOUR_TOPS_LENGTH_EQUAL] += $compl;
          if ($dcount == 1)
          {
            print "COUNT3 $line2\n";
          }
        }
        elsif (any_N_length_equal(\@opps, 4))
        {
          $numbers[$ANY_FOUR_LENGTH_EQUAL]++;
          $complexities[$ANY_FOUR_LENGTH_EQUAL] += $compl;
          if ($dcount == 1)
          {
            print "COUNT4 $line2\n";
          }
        }
        elsif (N_tops_lengthvar_equal(\@opps, 4))
        {
          $numbers[$FOUR_TOPS_LENGTHVAR_EQUAL]++;
          $complexities[$FOUR_TOPS_LENGTHVAR_EQUAL] += $compl;
          if ($dcount == 1)
          {
            print "COUNT3 $line2\n";
          }
        }
        elsif (any_N_lengthvar_equal(\@opps, 4))
        {
          $numbers[$ANY_FOUR_LENGTHVAR_EQUAL]++;
          $complexities[$ANY_FOUR_LENGTHVAR_EQUAL] += $compl;
          if ($dcount == 1)
          {
            print "COUNT4 $line2\n";
          }
        }
        elsif (N_tops_equal_onevar(\@opps, 4))
        {
          $numbers[$FOUR_TOPS_EQUAL_ONEVAR]++;
          $complexities[$FOUR_TOPS_EQUAL_ONEVAR] += $compl;
          if ($dcount == 1)
          {
            print "COUNT3 $line2\n";
          }
        }
        elsif (any_N_equal_onevar(\@opps, 4))
        {
          $numbers[$ANY_FOUR_EQUAL_ONEVAR]++;
          $complexities[$ANY_FOUR_EQUAL_ONEVAR] += $compl;
          if ($dcount == 1)
          {
            print "COUNT4 $line2\n";
          }
        }

        else
        {
          $numbers[$REST]++;
          $complexities[$REST] += $compl;

          print "$line2\n";
          # print "$lno: $line2\n";
          if ($dcount == 1)
          {
            print "COUNT5 $line2\n";
          }
        }
      }
      else
      {
        $numbers[$TEXT]++;
      }
    }
  }
}

close $fh;

print "\n";
for my $i ($TEXT .. $UNTEXT)
{
  printf("%-12s %7d %5.2f\n", 
    $titles[$i], $numbers[$i], $complexities[$i] / $numbers[$i]);
}
print "\n";

for my $i ($SINGULAR .. $REST)
{
  if ($numbers[$i] == 0)
  {
    printf("%-12s %7d\n", $titles[$i], $numbers[$i]);
  }
  else
  {
    printf("%-12s %7d %5.2f\n", 
      $titles[$i], $numbers[$i], $complexities[$i] / $numbers[$i]);
  }
}


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


sub N_tops_equal
{
  # The highest N tops, no length
  my ($opps_ref, $N) = @_;

  if ($opps_ref->[0]{oper} != -1)
  {
    # Length must be unset
    return 0;
  }

  my $c = $#$opps_ref;
  for my $i (1 .. $c-$N)
  {
    my $oper = $opps_ref->[$i]{oper};
    if ($oper != -1)
    {
      # Low tops must be unset
      return 0;
    }
  }

  for my $i ($c-$N+1 .. $c)
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


sub N_tops_equal_onevar
{
  # The highest N tops, no length
  my ($opps_ref, $N) = @_;

  if ($opps_ref->[0]{oper} != -1)
  {
    # Length must be unset
    return 0;
  }

  my $c = $#$opps_ref;
  for my $i (1 .. $c-$N)
  {
    my $oper = $opps_ref->[$i]{oper};
    if ($oper != -1)
    {
      # Low tops must be unset
      return 0;
    }
  }

  my $numvar = 0;
  for my $i ($c-$N+1 .. $c)
  {
    my $oper = $opps_ref->[$i]{oper};
    if ($oper != 0)
    {
      # Two high tops must be equal-sets
      $numvar++;
      return 0 if $numvar > 1;
    }
  }

  if ($numvar == 0)
  {
    return 0;
  }
  else
  {
    return 1;
  }
}


sub N_tops_length_equal
{
  # The highest N tops, no length
  my ($opps_ref, $N) = @_;

  if ($opps_ref->[0]{oper} != 0)
  {
    # Length must be set
    return 0;
  }

  my $c = $#$opps_ref;
  for my $i (1 .. $c-$N)
  {
    my $oper = $opps_ref->[$i]{oper};
    if ($oper != -1)
    {
      # Low tops must be unset
      return 0;
    }
  }

  for my $i ($c-$N+1 .. $c)
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


sub N_tops_lengthvar_equal
{
  # The highest N tops, length given but not a single value
  my ($opps_ref, $N) = @_;

  if ($opps_ref->[0]{oper} < 1)
  {
    # Length must be a range
    return 0;
  }

  my $c = $#$opps_ref;
  for my $i (1 .. $c-$N)
  {
    my $oper = $opps_ref->[$i]{oper};
    if ($oper != -1)
    {
      # Low tops must be unset
      return 0;
    }
  }

  for my $i ($c-$N+1 .. $c)
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


sub any_N_equal
{
  # Any N tops, no length
  my ($opps_ref, $N) = @_;

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

  if ($used == $N)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}


sub any_N_equal_onevar
{
  # Any N tops, no length
  my ($opps_ref, $N) = @_;

  if ($opps_ref->[0]{oper} != -1)
  {
    # Length must be unset
    return 0;
  }

  my $c = $#$opps_ref;
  my $used = 0;
  my $numvar = 0;
  for my $i (1 .. $c)
  {
    my $oper = $opps_ref->[$i]{oper};
    next if $oper == -1;
    if ($oper > 0)
    {
      $numvar++;
      return 0 if $numvar > 1;
    }
    $used++;
  }

  if ($used == $N && $numvar == 1)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}


sub any_N_length_equal
{
  # Any N tops, length single value
  my ($opps_ref, $N) = @_;

  if ($opps_ref->[0]{oper} != 0)
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

  if ($used == $N)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}


sub any_N_lengthvar_equal
{
  # Any N tops, length range
  my ($opps_ref, $N) = @_;

  if ($opps_ref->[0]{oper} < 1)
  {
    # Length must be a range
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

  if ($used == $N)
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

