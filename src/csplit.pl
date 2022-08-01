#!perl

use strict;
use warnings;

# perl csplit.pl coverall num45_3
# coverall has all covers generated.
# num45_3 is a solution file with only numerical strategy output and
# nothing like "queen doubleton".
# Generates, per sumProfile, one list of used and one list of unused
# covers.

use Scalar::Util qw(looks_like_number);

my $cfile = shift;
my $nfile = shift;

my %allcovers; # Indexed by sum profile string and then the code string
parse_covers($cfile, \%allcovers);

my %histo;
parse_solutions($nfile, \%allcovers, \%histo);

my %histo_unused;

my $used = "used.txt";
my $unused = "unused.txt";

open my $fused, '>', $used or die $!;
open my $funused, '>', $unused or die $!;

for my $profile (sort keys %allcovers)
{
  print $fused "$profile\n";
  print $fused '-' x length($profile), "\n";

  print $funused "$profile\n";
  print $funused '-' x length($profile), "\n";

  for my $key (sort keys %{$allcovers{$profile}})
  {
    my $ptr = \@{$allcovers{$profile}{$key}};
    for my $symm (0 .. 1)
    {
      next unless defined $ptr->[$symm]{count};

      if ($ptr->[$symm]{count} == 0)
      {
        print $funused $ptr->[$symm]{text}, "\n";

        my $text = $ptr->[$symm]{text};
        my @a = split /\s+/, $text;
        my $i = $#a - 3;
        $i-- if $text =~ /sym/;
        $i++ if $text =~ /sym\d/;

        my $compl = $a[$i];
        if ($compl =~ /unused/)
        {
          # unused, #dist and complexity are squashed together.
          $compl =~ s/unused//;
          die unless looks_like_number($compl);

          if (length($compl) == 4)
          {
            # Remove first two digits.
            $compl =~ s/^\d\d//;
          }
        }

        if (! looks_like_number($compl))
        {
          # 1-61001 -> 1001
          $compl = $a[$i+1];
        }

        if ($compl > 30)
        {
          print "HERE\n";
        }

        $histo_unused{$profile}[$compl]++;
      }
      else
      {
        print $fused $ptr->[$symm]{text}, ": count ",
          $ptr->[$symm]{count}, "\n";
      }
    }
  }
  
  print $fused "\n";
  print $funused "\n";

  for my $i (0 .. $#{$histo{$profile}})
  {
    if (defined $histo{$profile}[$i])
    {
      printf $fused ("%2d%6d\n", $i, $histo{$profile}[$i]);
    }
    else
    {
      printf $fused ("%2d\n", $i);
    }
  }
  print $fused "\n";

  for my $i (0 .. $#{$histo_unused{$profile}})
  {
    if (defined $histo_unused{$profile}[$i])
    {
      printf $funused ("%2d%6d\n", $i, $histo_unused{$profile}[$i]);
    }
    else
    {
      printf $funused ("%2d\n", $i);
    }
  }
  print $funused "\n";
}

close $used;
close $unused;




sub parse_covers
{
  my ($cfile, $covers_ref) = @_;

  my $lno = 0;
  my $profile;
  open my $fh, '<', $cfile or die $!;

  while (my $line = <$fh>)
  {
    $lno++;

    if ($line =~ /^ Length Top0/)
    {
      $line = <$fh>;
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
      $line = <$fh>;
      $lno++;

      while (my $line2 = <$fh>)
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
      }
    }
  }

  close $fh;
}


sub parse_solutions
{
  my ($nfile, $allcovers_ref, $histo_ref) = @_;

  my $lno = 0;
  my $profile;
  open my $fh, '<', $nfile or die $!;

  while (my $line = <$fh>)
  {
    $lno++;

    if ($line =~ /^Rank North/)
    {
      my @oppsMax = ();
      my $len = 0;

      while (my $line2 = <$fh>)
      {
        $lno++;

        chomp $line2;
        $line2 =~ s///g;
        last if $line2 eq "";
        next unless $line2 =~ /Opps\s+(\d+)\s+/;
        unshift @oppsMax, $1;
        $len += $1;
      }

      $profile = $len . ":" . join(',', @oppsMax);
    }
    elsif ($line =~ /^Strategy \#(\d+) \[c (\d+)\/(\d+), w (\d+)\/(\d+), d (\d+)\/(\w+)\]/)
    {
      while (my $line2 = <$fh>)
      {
        $lno++;

        chomp $line2;
        $line2 =~ s///g;
        $line2 =~ s/^\s*$//g;
        last if $line2 =~ /^\s*$/;

        $line2 =~ s/^\* //;
        $line2 =~ s/^\s+//;
        $line2 =~ s/; or$//;

        if ($line2 =~ /unused/)
        {
          $line2 =~ s/\s+\[(\d+)\/\d+\]\s*$//;
          my $compl = $1;

          $line2 =~ /(\d+)\s*$/;
          my $key = $1;

if (! defined $key)
{
  print "HERE\n";
}

          my $symmflag = ($line2 =~ /sym/ ? 1 : 0);

          $allcovers_ref->{$profile}{$key}[$symmflag]{count}++;

          $histo_ref->{$profile}[$compl]++;
        }
      }
    }
  }

  close $fh;
}

