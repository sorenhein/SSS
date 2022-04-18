#!perl

use strict;
use warnings;

# Check two generations of verbal strategies against each other.
# Extract agreements and differences.

my $file = shift;
open my $fh, '<', $file or die $!;

my $lno = 0;
my ($cards, $holding, $tag);
my (@strats, @vstrats, @sostrats, @vostrats, @shead, @vhead, @diagram);
my (%stextSame, %vtextSame, %countSame, %headerSame);
my (%stextSameOrig, %vtextSameOrig, %countSameOrig, %headerSameOrig);
my (%stextDiff, %vtextDiff, %countDiff, %headerDiff);
my (%stextDiffOrig, %vtextDiffOrig, %countDiffOrig, %headerDiffOrig);

while (my $line = <$fh>)
{
  $lno++;

  if ($line =~ /^Cards\s+(\d+):\s+(\d+)/)
  {
    $cards = $1;
    $holding = $2;
    @strats = ();
    @sostrats = ();
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
      push @{$sostrats[$sno]}, $line2;

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
    @vostrats = ();

    while (my $line2 = <$fh>)
    {
      chomp $line2;
      $line2 =~ s///;
      $lno++;
      last if ($line2 =~ /^\s*$/);
      push @vostrats, $line2;

      $line2 =~ s/^\s+//;
      $line2 =~ s/^\*\s+//;
      $line2 =~ s/\s*\[.*\]//;
      $line2 =~ s/\s+$//;
      push @vstrats, $line2;
    }

=pod
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
=cut

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

        store(\@{$sostrats[$vno]}, \@vostrats,
          \@{$stextSameOrig{$tag}}, \@{$vtextSameOrig{$tag}}, 
          \@{$countSameOrig{$tag}}, \@{$headerSameOrig{$tag}},
          $cards, $holding, \@diagram);
      }
      else
      {
# print "New header: diff\n";
        store(\@ssorted, \@vsorted,
          \@{$stextDiff{$tag}}, \@{$vtextDiff{$tag}}, 
          \@{$countDiff{$tag}}, \@{$headerDiff{$tag}},
          $cards, $holding, \@diagram);

        store(\@{$sostrats[$vno]}, \@vostrats,
          \@{$stextDiffOrig{$tag}}, \@{$vtextDiffOrig{$tag}}, 
          \@{$countDiffOrig{$tag}}, \@{$headerDiffOrig{$tag}},
          $cards, $holding, \@diagram);
      }
      next;
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

      store(\@{$sostrats[$vno]}, \@vostrats,
        \@{$stextSameOrig{$tag}}, \@{$vtextSameOrig{$tag}}, 
        \@{$countSameOrig{$tag}}, \@{$headerSameOrig{$tag}},
        $cards, $holding, \@diagram);
    }
    else
    {
# print "New store: diff\n";
      store(\@ssorted, \@vsorted,
        \@{$stextDiff{$tag}}, \@{$vtextDiff{$tag}}, 
        \@{$countDiff{$tag}}, \@{$headerDiff{$tag}},
        $cards, $holding, \@diagram);

      store(\@{$sostrats[$vno]}, \@vostrats,
        \@{$stextDiffOrig{$tag}}, \@{$vtextDiffOrig{$tag}}, 
        \@{$countDiffOrig{$tag}}, \@{$headerDiffOrig{$tag}},
        $cards, $holding, \@diagram);
    }
    next;
  }
}

close $fh;

print_stats(\%stextSame, \%countSame, \%vtextDiff, \%countDiff);

dump_file('same.pl', 
  \%stextSame, \%vtextSame, 
  \%stextSameOrig, \%vtextSameOrig, 
  \%countSame, \%headerSame, 1);
dump_file('diff.pl', 
  \%stextDiff, \%vtextDiff, 
  \%stextDiffOrig, \%vtextDiffOrig, 
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


sub print_stats
{
  my ($stext_ref, $countSame_ref, $vtext_ref, $countDiff_ref) = @_;

  my ($sum_sc, $sum_dc, $sum_sn, $sum_dn);

  printf("%-6s%6s%6s%10s%6s\n", "Case", "Same", "Diff", "Same", "Diff");
  for my $key (sort keys %$stext_ref)
  {
    my $sc = 1 + $#{$stext_ref->{$key}};
    my $dc = 1 + $#{$vtext_ref->{$key}};

    my $sn = 0;
    for my $i (0 .. $#{$countSame_ref->{$key}})
    {
      $sn += $countSame_ref->{$key}[$i];
    }

    my $dn = 0;
    for my $i (0 .. $#{$countDiff_ref->{$key}})
    {
      $dn += $countDiff_ref->{$key}[$i];
    }

    if ($dc == 0)
    {
      printf("%-6s%6d%6s%10d%6s\n", $key, $sc, "-", $sn, "-");
    }
    else
    {
      printf("%-6s%6d%6d%10d%6d\n", $key, $sc, $dc, $sn, $dn);
    }

    $sum_sc += $sc;
    $sum_dc += $dc;
    $sum_sn += $sn;
    $sum_dn += $dn;
  }

  print "-" x 34, "\n";

  printf("%-6s%6d%6d%10d%6d\n", "Sum", $sum_sc, $sum_dc, $sum_sn, $sum_dn);
}


sub dump_file
{
  my ($fname, 
    $stext_ref, $vtext_ref, 
    $stextOrig_ref, $vtextOrig_ref, 
    $count_ref, $header_ref, $same_flag) = @_;

  open my $fh, '>', $fname or die $!;

  for my $key (sort keys %$stext_ref)
  {
    next if ($#{$stext_ref->{$key}} == -1);

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
      print $fh "\n";

      for my $l (@{$stextOrig_ref->{$key}[$i]})
      {
        print $fh "# $l\n";
      }
      print $fh "\n";

      for my $l (@{$stext_ref->{$key}[$i]})
      {
        printf $fh "push \@\{\$stext\{'$tag'\}\[$i\]\}, '%s';\n", $l;
      }
      print $fh "\n";

      next if $same_flag;

      for my $l (@{$vtextOrig_ref->{$key}[$i]})
      {
        print $fh "# $l\n";
      }
      print $fh "\n";

      for my $l (@{$vtext_ref->{$key}[$i]})
      {
        printf $fh "push \@\{\$vtext\{'$tag'\}\[$i\]\}, '%s';\n", $l;
      }
      print $fh "\n\n";
    }
    print $fh "\n";
  }

  close $fh;
}
