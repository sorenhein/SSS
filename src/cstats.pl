#!perl

use strict;
use warnings;

# Count the cover's that are not stated in words.
# Show them in a big table.
# perl cstats.pl coverall num45_3

my $cfile = shift;
my $nfile = shift;

my @table;

my $TOP_NONE = 0;

my $TOP_EQUAL = 1;
my $TOP_RANGE = 2;
my $TOP_GREATER = 3;
my $TOP_LESS = 4;

my $ANY_EQUAL = 5;
my $ANY_RANGE = 6;
my $ANY_GREATER = 7;
my $ANY_LESS = 8;

my $LEN_NONE = 0;
my $LEN_EQUAL = 1;
my $LEN_RANGE = 2;
my $LEN_GREATER = 3;
my $LEN_LESS = 4;

my $MAX_TOPS = 5;

for my $top (0 .. $ANY_LESS)
{
  for my $len (0 .. $LEN_LESS)
  {
    for my $depth (0 .. 5)
    {
      $table[$top][$len][$depth]{used} = 0;
      $table[$top][$len][$depth]{uses} = 0;
      $table[$top][$len][$depth]{total} = 0;
    }
  }
}

my @top_titles;

$top_titles[$TOP_NONE] = "Lenonly";

$top_titles[$TOP_EQUAL] = "Top==";
$top_titles[$TOP_RANGE] = "TopRg";
$top_titles[$TOP_GREATER] = "Top>=";
$top_titles[$TOP_LESS] = "Top<=";

$top_titles[$ANY_EQUAL] = "Any==";
$top_titles[$ANY_RANGE] = "AnyRg";
$top_titles[$ANY_GREATER] = "Any>=";
$top_titles[$ANY_LESS] = "Any<=";

my @length_titles;

$length_titles[$LEN_NONE] = "Lenno";
$length_titles[$LEN_EQUAL] = "Len==";
$length_titles[$LEN_RANGE] = "LenRg";
$length_titles[$LEN_GREATER] = "Len>=";
$length_titles[$LEN_LESS] = "Len<=";


my %allcovers; # Indexed by sum profile string and then code string
open my $fc, '<', $cfile or die $!;
parse_covers($cfile, \%allcovers, \@table);
close $fc;

open my $fh, '<', $nfile or die $!;

my @oppsMax;
my $profile;
my $lno = 0;

while (my $line = <$fh>)
{
  $lno++;

  if ($line =~ /^Rank North/)
  {
    # In effect, parse out the sum profile (without the leading length).
    parse_sum_profile(\@oppsMax, \$profile, \$lno);
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

      next unless $line2 =~ /unused/;

      $line2 =~ s/\[\d+\/\d+\]\s*$//;

      my @splits = split /\s+/, $line2;
      my $num = $#splits - 6;
      $num-- if $line2 =~ / sym /;

      my $i = 1;
      my $j = 0;
      my $state = 0;
      my @opps;

      while ($i <= $num)
      {
        my $f = $splits[$i];
        parse_term($f, \$state, \$i, \$j, \@opps);
      }

      # Skip the singular ones.
      my $symmflag = ($line2 =~ /sym/ ? 1 : 0);
      my $dcount = $splits[0] =~ tr/1//;
      $dcount >>= 1 if $symmflag;
      next if $dcount == 1;

      my $top_score = get_top_score(\@opps);
      my $len_score = get_length_score(\@opps);
      my $depth = get_depth(\@opps);

=pod
if ($top_score == $TOP_EQUAL &&
    $depth == 0)
{
  print "HIT!\n";
  print "$lno\n";
  print "$line2\n";
}
=cut

      $table[$top_score][$len_score][$depth]{uses}++;

      $line2 =~ /(\d+)\s*$/;
      my $key = $1;

      if ($allcovers{$profile}{$key}[$symmflag]{count} == 0)
      {
        $table[$top_score][$len_score][$depth]{used}++;
      }

      $allcovers{$profile}{$key}[$symmflag]{count}++;
    }
  }
}

close $fh;

print "Total\n\n";
print_table('total');

print "Uses\n\n";
print_table('uses');

print "Used\n\n";
print_table('used');



sub parse_covers
{
  my ($cfile, $covers_ref, $table_ref) = @_;

  my $lno = 0;
  my $profile;
  open my $fh, '<', $cfile or die $!;

  while (my $line = <$fc>)
  {
    $lno++;

    if ($line =~ /^ Length Top0/)
    {
      $line = <$fc>;
      chomp $line;
      $line =~ s///g;
      $lno++;

      # This is the sum profile in a certain format
      $line =~ s/^\s+//;
      $line =~ s/:\s+/:/;
      $line =~ s/\s+$//;
      $line =~ s/\s+/,/g;
      $profile = $line;

      # The header line
      $line = <$fc>;
      $lno++;

      while (my $line2 = <$fc>)
      {
        chomp $line2;
        $line2 =~ s///g;
        $lno++;
        last if $line2 =~ /^\s*$/;

        $line2 =~ /(\d+)\s*$/;
        my $key = $1;

        my $symmflag = ($line2 =~ /sym/ ? 1 : 0);

        $covers_ref->{$profile}{$key}[$symmflag]{text} = $line2;
        $covers_ref->{$profile}{$key}[$symmflag]{count} = 0;

        # Parse the individual terms.
        my @splits = split /\s+/, $line2;
        my $num = $#splits - 5;
        $num-- if $line2 =~ / sym /;

        my $i = 1;
        my $j = 0;
        my $state = 0;
        my @opps;
        
        while ($i <= $num)
        {
          my $f = $splits[$i];
          parse_term($f, \$state, \$i, \$j, \@opps);
        }

        my $top_score = get_top_score(\@opps);
        my $len_score = get_length_score(\@opps);
        my $depth = get_depth(\@opps);

=pod
if (# $top_score == $TOP_EQUAL &&
    # $len_score == $LEN_NONE &&
    $depth == 5)
{
  print "HIT!\n";
  print "$lno\n";
  print "$line2\n";
}
=cut
        $covers_ref->{$profile}{$key}[$symmflag]{top} = $top_score;
        $covers_ref->{$profile}{$key}[$symmflag]{length} = $len_score;
        $covers_ref->{$profile}{$key}[$symmflag]{depth} = $depth;

        $table[$top_score][$len_score][$depth]{total}++;

      }
    }
  }
}

sub parse_sum_profile
{
  my ($oppsMax_ref, $profile_ref, $lno_ref) = @_;

  @$oppsMax_ref = ();
  my $len = 0;

  while (my $line2 = <$fh>)
  {
    $lno++;

    chomp $line2;
    $line2 =~ s///g;
    last if $line2 eq "";
    next unless $line2 =~ /Opps\s+(\d+)\s+/;
    unshift @$oppsMax_ref, $1;
    $len += $1;
  }

  $$profile_ref = $len . ":" . join(',', @$oppsMax_ref);
}


sub parse_term
{
  my ($field, $state_ref, $i_ref, $j_ref, $opps_ref) = @_;

  # We use the length constants, but it applies generally.

  if ($$state_ref == 0)
  {
    if ($field eq "==")
    {
      $opps_ref->[$$j_ref]{oper} = $LEN_EQUAL;

      $$state_ref = 1;
      $$i_ref++;
    }
    elsif ($field eq "<=")
    {
      $opps_ref->[$$j_ref]{oper} = $LEN_LESS;

      $$state_ref = 1;
      $$i_ref++;
    }
    elsif ($field eq ">=")
    {
      $opps_ref->[$$j_ref]{oper} = $LEN_GREATER;

      $$state_ref = 1;
      $$i_ref++;
    }
    elsif ($field =~ /^(\d+)-(\d+)$/)
    {
      $opps_ref->[$$j_ref]{oper} = $LEN_RANGE;
      $opps_ref->[$$j_ref]{lower} = $1;
      $opps_ref->[$$j_ref]{upper} = $2;

      $$state_ref = 0;
      $$i_ref++;
      $$j_ref++;
    }
    elsif ($field eq "unused")
    {
      $opps_ref->[$$j_ref]{oper} = $LEN_NONE;
      $opps_ref->[$$j_ref]{lower} = -1;
      $opps_ref->[$$j_ref]{upper} = -1;

      $$state_ref = 0;
      $$i_ref++;
      $$j_ref++;
    }
    else
    {
      print "field $field\n";
      print "i $$i_ref j $$j_ref state $$state_ref\n";
      die;
    }
  }
  elsif ($$state_ref == 1)
  {
    # Expecting a number following an operator.
    die "$field not a number" unless $field =~ /^\d+$/;

    $opps_ref->[$$j_ref]{lower} = $field;
    $opps_ref->[$$j_ref]{upper} = -1;

    $$state_ref = 0;
    $$i_ref++;
    $$j_ref++;
  }
  else
  {
    print "Field $field\n";
    print "State $$state_ref\n";
    print "i     $$i_ref\n";
    print "j     $$j_ref\n";
    dump_opps($opps_ref);
    die "Error";
  }
}


sub dump_opps
{
  my $opps_ref = shift;
  for my $i (0 .. $#$opps_ref)
  {
    printf("%2d oper %d lower %d upper %d\n",
      $i,
      $opps_ref->[$i]{oper},
      $opps_ref->[$i]{lower},
      $opps_ref->[$i]{upper});
  }
}


sub get_top_score
{
  my $opps_ref = shift;

  my $c = $#$opps_ref;
  my $last_unset = 0;
  for my $i (1 .. $c)
  {
    if ($opps_ref->[$i]{oper} == $LEN_NONE)
    {
      $last_unset = $i;
    }
  }

  my $first_unset = 999;
  my $non_equal = $LEN_EQUAL;
  for my $i (reverse 1 .. $c)
  {
    my $oper = $opps_ref->[$i]{oper};
    if ($oper == $LEN_NONE)
    {
      $first_unset = $i;
    }
    elsif ($oper != $LEN_EQUAL)
    {
      # There is only one such.
      $non_equal = $oper;
    }
  }

  if (($first_unset == 999 && $last_unset == 0) ||
      ($c == 1 && $first_unset == 1 && $last_unset == 1))
  {
    return $TOP_NONE;
  }
  elsif ($first_unset == $last_unset)
  {
    # We have a top situation.
    return $TOP_EQUAL if $non_equal == $LEN_EQUAL;
    return $TOP_RANGE if $non_equal == $LEN_RANGE;
    return $TOP_GREATER if $non_equal == $LEN_GREATER;
    return $TOP_LESS if $non_equal == $LEN_LESS;
    die "No result?";
  }
  else
  {
    # We have a top situation.
    return $ANY_EQUAL if $non_equal == $LEN_EQUAL;
    return $ANY_RANGE if $non_equal == $LEN_RANGE;
    return $ANY_GREATER if $non_equal == $LEN_GREATER;
    return $ANY_LESS if $non_equal == $LEN_LESS;
    die "No result?";
  }
}


sub get_length_score
{
  my $opps_ref = shift;
  return $opps_ref->[0]{oper};
}


sub get_depth
{
  # Count the non-zero top entries.

  my $opps_ref = shift;

  my $c = $#$opps_ref;
  my $depth = 0;
  for my $i (1 .. $c)
  {
    my $oper = $opps_ref->[$i]{oper};
    $depth++ unless $oper == $LEN_NONE;
  }
  return $depth;
}


sub print_table
{
  my $field = shift;

  printf("%-16s", "");
  for my $depth (0 .. 5)
  {
    printf("%6d", $depth);
  }
  print "\n";
  print '-' x 52, "\n";

  for my $top (0 .. $ANY_LESS)
  {
    for my $len (0 .. $LEN_LESS)
    {

      my $sum = 0;
      $sum += $table[$top][$len][$_]{$field} for (0 .. 5);
      next if $sum == 0;

      printf("%-16s", $top_titles[$top] . ' ' . $length_titles[$len]);
      printf("%6d", $table[$top][$len][$_]{$field}) for (0 .. 5);
      print "\n";
    }
    print "\n";
  }
  print "\n";
}

