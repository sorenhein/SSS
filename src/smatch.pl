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

my $countHead = 0;
my $countHeadText = 0;
my $countNum = 0;
my $countText = 0;
my $countSame = 0;

my $stiffHonor = 0;
my $doubleHonor = 0;

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

    # Skip the header
    # my $line2 = <$fh>;
    # $lno++;

    while (my $line2 = <$fh>)
    {
      chomp $line2;
      $line2 =~ s///;
      $lno++;
      last if ($line2 =~ /^\s*$/);
      $line2 =~ s/^\*\s+//;
      push @vstrats, $line2;
    }

    if ($shead[$vno] ne $vhead[$vno])
    {
      $countHeadText++;
    }
    elsif ($#{$strats[$vno]} != $#vstrats)
    {
      if ($#{$strats[$vno]} == 0 &&
        $#vstrats == 1 &&
        $strats[$vno][0] =~ /^Either opponent has the singleton honor/ &&
        $vstrats[0] =~ /^West has the singleton honor/ &&
        $vstrats[1] =~ /^East has the singleton honor/)
      {
        $stiffHonor++;
      }
      elsif ($#{$strats[$vno]} == 1 &&
        $#vstrats == 2 &&
        $strats[$vno][0] eq $vstrats[0] &&
        $strats[$vno][1] =~ /^Either opponent has the singleton honor/ &&
        $vstrats[1] =~ /^West has the singleton honor/ &&
        $vstrats[2] =~ /^East has the singleton honor/)
      {
        $stiffHonor++;
      }
      elsif ($#{$strats[$vno]} == 0 &&
        $#vstrats == 1 &&
        $strats[$vno][0] =~ /^Either opponent has the top at most doubleton/ &&
        $vstrats[0] =~ /^West has the top at most doubleton/ &&
        $vstrats[1] =~ /^East has the top at most doubleton/)
      {
        $doubleHonor++;
      }
      elsif ($#{$strats[$vno]} == 1 &&
        $#vstrats == 2 &&
        $strats[$vno][0] eq $vstrats[0] &&
        $strats[$vno][1] =~ /^Either opponent has the top at most doubleton/ &&
        $vstrats[1] =~ /^West has the top at most doubleton/ &&
        $vstrats[2] =~ /^East has the top at most doubleton/)
      {
        $doubleHonor++;
      }
      elsif ($#{$strats[$vno]} == 0 &&
        $#vstrats == 1 &&
        $strats[$vno][0] =~ /^Either opponent has doubleton honors/ &&
        $vstrats[0] =~ /^West has doubleton honors/ &&
        $vstrats[1] =~ /^East has doubleton honors/)
      {
        $doubleHonor++;
      }
      elsif ($#{$strats[$vno]} == 1 &&
        $#vstrats == 2 &&
        $strats[$vno][0] eq $vstrats[0] &&
        $strats[$vno][1] =~ /^Either opponent has doubleton honors/ &&
        $vstrats[1] =~ /^West has doubleton honors/ &&
        $vstrats[2] =~ /^East has doubleton honors/)
      {
        $doubleHonor++;
      }
      else
      {
print "$lno C\n";
        $countNum++;
      }
    }
    else
    {
      my $flagDiff = 0;
      for my $i (0 .. $#vstrats)
      {
        if ($strats[$vno][$i] ne $vstrats[$i])
        {
print "$lno T\n";
          $countText++;
          $flagDiff = 1;
          last;
        }
      }

      if (! $flagDiff)
      {
        $countSame++;
      }
    }
  }
}

close $fh;

print "Head count      ", $countHead, "\n";
print "Head text       ", $countHeadText, "\n";
print "Number of lines ", $countNum, "\n";
print "  Stiff honor   ", $stiffHonor, "\n";
print "  Double honor  ", $doubleHonor, "\n";
print "Text content    ", $countText, "\n";
print "Same            ", $countSame, "\n";
