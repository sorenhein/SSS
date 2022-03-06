#!perl

use strict;
use warnings;

# Check two generations of verbal strategies against each other.

my $file = shift;
open my $fh, '<', $file or die $!;

my $lno = 0;
my ($cards, $holding);
my (@strats, @vstrats);
my (@shead, @vhead);

my $countHeadText = 0;
my $countNum = 0;
my $countText = 0;
my $countSame = 0;

while (my $line = <$fh>)
{
  $lno++;

  if ($lno == 4896)
  {
    # print "HERE\n";
  }

  if ($line =~ /^Cards\s+(\d+):\s+(\d+)/)
  {
    $cards = $1;
    $holding = $2;
    @strats = ();
  }
  elsif ($line =~ /^Strategy #(\d+): (.*)/)
  {
    my $sno = $1;
    my $rest = $2;
    $rest =~ s///;
    $shead[$sno] = $rest;

    while (my $line2 = <$fh>)
    {
      chomp $line2;
      $line2 =~ s///;
      $lno++;
      last if ($line2 =~ /^\s*$/);
      $line2 =~ s/^\*\s+//;
      $line2 =~ s/^\s*-\s+//;
      $line2 =~ s/\[.*\]//;
      push @{$strats[$sno]}, $line2;
    }
  }
  elsif ($line =~ /^VStrategy #(\d+): (.*)/)
  {
    my $vno = $1;
    my $rest = $2;
    $rest =~ s///;
    $vhead[$vno] = $rest;
    @vstrats = ();

    while (my $line2 = <$fh>)
    {
      chomp $line2;
      $line2 =~ s///;
      $lno++;
      last if ($line2 =~ /^\s*$/);
      $line2 =~ s/^\*\s+//;
      $line2 =~ s/\[.*\]//;
      push @vstrats, $line2;
    }

    my @vsorted = sort @vstrats;

    if ($shead[$vno] ne $vhead[$vno])
    {
      $countHeadText++;
      next;
    }

    for my $i (0 .. $#{$strats[$vno]})
    {
      my $t = $strats[$vno][$i];
      if ($t =~ /^Either opponent/)
      {
        my $t1 = $t;
        $t1 =~ s/^Either opponent/West/;
        push @{$strats[$vno]}, $t1;

        $t1 = $t;
        $t1 =~ s/^Either opponent/East/;
        push @{$strats[$vno]}, $t1;

        splice(@{$strats[$vno]}, $i, 1);
      }
    }

    my @ssorted = sort @{$strats[$vno]};

    if ($#ssorted != $#vsorted)
    {
print "$lno C\n";
      $countNum++;
    }
    else
    {
      my $sameFlag = 1;
      for my $i (0 .. $#ssorted)
      {
        if ($ssorted[$i] ne $vsorted[$i])
        {
          $sameFlag = 0;
          last;
        }
      }

      if ($sameFlag)
      {
        $countSame++;
      }
      else
      {
print "$lno T\n";
        $countText++;
      }
    }
  }
}

close $fh;

print "Head text       ", $countHeadText, "\n";
print "Number of lines ", $countNum, "\n";
print "Text content    ", $countText, "\n";
print "Same            ", $countSame, "\n";
