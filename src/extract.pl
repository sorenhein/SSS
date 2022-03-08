#!perl

use strict;
use warnings;

# TODO
# Store the actual text, not only the sorted and processed one.
# Output in comments the actual text.
# Output the sorted one in perl code.
# Prettify with empty lines as needed.

# Check two generations of verbal strategies against each other.
# Extract agreements and differences.

my $file = shift;
open my $fh, '<', $file or die $!;

my $lno = 0;
my ($cards, $holding, $tag);
my (@strats, @vstrats, @shead, @vhead, @diagram);
my (%stextSame, %vtextSame, %countSame, %headerSame);
my (%stextDiff, %vtextDiff, %countDiff, %headerDiff);

while (my $line = <$fh>)
{
  $lno++;

  if ($line =~ /^Cards\s+(\d+):\s+(\d+)/)
  {
    $cards = $1;
    $holding = $2;
    @strats = ();
    @diagram = ();

    # Get the diagram.
    $line = <$fh>;
    for my $i (0 .. 4)
    {
      $line = <$fh>;
      chomp $line;
      $line =~ s///g;
      push @diagram, $line;
    }
    $lno += 6;

    # Make the tag of the form 6-1 (length 6, of which 1 top).
    my $opps = $diagram[2];
    $opps =~ /(\w+)\s*$/;
    $opps = $1;

    my $honor = substr $opps, 0, 1;
    my $count = () = $opps =~ /\Q$honor/g;

    my $len = length $opps;
    $tag = $len . '-' . $count;
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
      $line2 =~ s/\s*\[.*\]//;
      $line2 =~ s/^YY //;
      $line2 =~ s/^\s+//;
      push @{$strats[$sno]}, $line2;
    }
  }
  elsif ($line =~ /^VStrategy #(\d+): (.*)/)
  {
# print "lno $ lno\n";
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
      $line2 =~ s/^\s+//;
      $line2 =~ s/^\*\s+//;
      $line2 =~ s/\s*\[.*\]//;
      $line2 =~ s/\s+$//;
      push @vstrats, $line2;
    }

    my @ssorted = sort @{$strats[$vno]};
    my @vsorted = sort @vstrats;

    if ($shead[$vno] ne $vhead[$vno])
    {
      if (same(\@ssorted, \@vsorted))
      {
# print "New header: same\n";
        store(\@ssorted, \@vsorted,
          \@{$stextSame{$tag}}, \@{$vtextSame{$tag}}, 
          \@{$countSame{$tag}}, \@{$headerSame{$tag}},
          $cards, $holding, \@diagram);
      }
      else
      {
# print "New header: diff\n";
        store(\@ssorted, \@vsorted,
          \@{$stextDiff{$tag}}, \@{$vtextDiff{$tag}}, 
          \@{$countDiff{$tag}}, \@{$headerDiff{$tag}},
          $cards, $holding, \@diagram);
      }
      next;
    }

    for my $i (reverse 0 .. $#{$strats[$vno]})
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


    my $hit = contained(\@ssorted, \@vsorted,
      \@{$stextDiff{$tag}}, \@{$vtextDiff{$tag}});

    if ($hit >= 0)
    {
# print "Diff match\n";
      $countDiff{$tag}[$hit]++;
      next;
    }

    $hit = contained(\@ssorted, \@vsorted,
      \@{$stextSame{$tag}}, \@{$vtextSame{$tag}});

    if ($hit >= 0)
    {
# print "Same match\n";
      $countSame{$tag}[$hit]++;
      next;
    }

    if (same(\@ssorted, \@vsorted))
    {
# print "New store: same\n";
      store(\@ssorted, \@vsorted,
        \@{$stextSame{$tag}}, \@{$vtextSame{$tag}}, 
        \@{$countSame{$tag}}, \@{$headerSame{$tag}},
        $cards, $holding, \@diagram);
    }
    else
    {
# print "New store: diff\n";
      store(\@ssorted, \@vsorted,
        \@{$stextDiff{$tag}}, \@{$vtextDiff{$tag}}, 
        \@{$countDiff{$tag}}, \@{$headerDiff{$tag}},
        $cards, $holding, \@diagram);
    }
  }
}

close $fh;

dump_file('same.pl', \%stextSame, \%vtextSame, 
  \%countSame, \%headerSame, 1);
dump_file('diff.pl', \%stextDiff, \%vtextDiff, 
  \%countDiff, \%headerDiff, 0);


sub contained
{
  my ($ssorted_ref, $vsorted_ref, $slist_ref, $vlist_ref) = @_;

  my $scount = $#$ssorted_ref;
  my $vcount = $#$vsorted_ref;

  for my $c (0 .. $#$slist_ref)
  {
    next unless $#{$slist_ref->[$c]} == $scount;
    next unless $#{$vlist_ref->[$c]} == $vcount;

    my $match = 1;
    for my $d (0 .. $scount)
    {
      if ($slist_ref->[$c][$d] ne $ssorted_ref->[$d])
      {
        $match = 0;
        last;
      }
    }

    next unless $match;

    for my $d (0 .. $vcount)
    {
      if ($vlist_ref->[$c][$d] ne $vsorted_ref->[$d])
      {
        $match = 0;
        last;
      }
    }

    return $c if $match;
  }

  return -1;
}


sub same
{
  my ($ssorted_ref, $vsorted_ref) = @_;

  return 0 unless $#$ssorted_ref == $#$vsorted_ref;

  for my $i (0 .. $#$ssorted_ref)
  {
    return 0 unless $ssorted_ref->[$i] eq $vsorted_ref->[$i];
  }
  return 1;
}


sub store
{
  my ($strats_ref, $vstrats_ref, 
    $slist_ref, $vlist_ref, 
    $count_ref, $header_ref,
    $cards, $holding, $diag_ref) = @_;

  my $index = 1 + $#$slist_ref;

  my $header = "# Example: $cards / $holding, count ";
  push @{$header_ref->[$index]}, $header;

  for my $d (@$diag_ref)
  {
    push @{$header_ref->[$index]}, "# $d";
  }

  for my $s (@$strats_ref)
  {
    # my $line = 'push @{$stext[' . $index . ']}, \'' . $s . '\';';
    push @{$slist_ref->[$index]}, $s;
  }

  for my $v (@$vstrats_ref)
  {
    # my $line = 'push @{$vtext[' . $index . ']}, \'' . $v . '\';';
    push @{$vlist_ref->[$index]}, $v;
  }

  $count_ref->[$index] = 1;
}


sub dump_file
{
  my ($fname, $stext_ref, $vtext_ref, 
    $count_ref, $header_ref, $same_flag) = @_;

  open my $fh, '>', $fname or die $!;

  for my $key (sort keys %$stext_ref)
  {
    print $fh "#################### $key ####################\n\n";

    for my $i (0 .. $#{$stext_ref->{$key}})
    {
      my $first = 1;
      for my $h (@{$header_ref->{$key}[$i]})
      {
        if ($first)
        {
          print $fh $h, $count_ref->{$key}[$i], "\n";
          $first = 0;
        }
        else
        {
          print $fh $h, "\n";
        }
      }

      print $fh "Entry S $i\n";
      for my $l (@{$stext_ref->{$key}[$i]})
      {
        print $fh $l, "\n";
      }

      next if $same_flag;

      print $fh "Entry V $i\n";
      for my $l (@{$vtext_ref->{$key}[$i]})
      {
        print $fh $l, "\n";
      }
    }
    print $fh "\n";
  }

  close $fh;
}
