#!/usr/bin/perl

my $DEBUG = 0;
my $PATH_OUTPUT = 0;
our $VERSION = 1.00;

our $CLASS = 1;

use strict;
use warnings;
use Getopt::Long;
use Pod::Usage;
use Time::HiRes qw(time);
use Fatal qw(open close);
use IO::Handle;

our $RETURN_ERROR = 0;
our $RETURN_OK    = 1;
our $RETURN_DONE  = 2;

our $ON_FLAG;

our $NS_RANK;
our $EW_RANK;
our $safety_level;
our %inputs;
our %run_stats;
our $ERROR = 0;
our @fnc_list;
our $hand_global_ref;
our $dist_global_ref;
our @tvec;

my %FILES;
our $CURRENT_FILE;

our $NEUTRAL = 0;
our $GOOD    = 1;
our $BAD     = 2;
our $SAME    = 3;

our $EPS = 1.e-9;
our $EMPTY = q{};
our $SPACE = q{ };

use constant {
  NEXT   => 0,
  PROB   => 1,
  EVAL   => 2,
  SAFE   => 3,
  RESULT => 4,
  PVAR   => 5,    # For finesse-type endings
  ENDING => 6,    # For selecting the choice of ending
  DIST   => 1,
  NUMD   => 2,
  ONED   => 3,    # The number of the single distribution (if applicable),
                  # otherwise not meaningful (but equals some distribution).
  UNIQ   => 4,
  SIDE   => 1,    # For pre-setting a strategy
  TYPE   => 2,    # Can be CASH, PLAY, DUCK, LDTO, RUN, HOOK
  VAR    => 3,
  WIN    => 4,
  CASH   => 1,
  PLAY   => 2,
  DUCK   => 3,
  RUN    => 4,
  LDTO   => 5,    # Lead to
  HOOK   => 6,    # Finesse
};

require './constants.pl';
require './manpage.pl';
require './math.pl';
require './inputs.pl';
require './arrays.pl';
require './recterm.pl';
require './recarrays.pl';
require './ew.pl';
require './ns.pl';
require './printfnc.pl';
require './diag.pl';
require './strats.pl';
require './dist_list.pl';
# require 'dist_list2.pl';
require './fillstrat.pl';
require './dominant.pl';
require './prematch.pl';
require './match.pl';
require './verbal.pl';
require './common.pl';

require './rec_cst.pl';
require './rec_generic.pl';
require './rec_ewlen3.pl';
require './rec_nslen3_30.pl';
require './rec_nslen4_31.pl';
require './rec_nslen4_22.pl';
require './rec_nslen4_40.pl';
require './rec_nslen5_41.pl';


constants();
make_key_constants();
ns_set_play_book();
set_pre_matches();

($#ARGV == 0) ? solve_batch() : solve(0);
exit;


sub solve_main
{
  my (%hand, @dist, $halted);

  make_arrays(\%inputs, \%hand, \@dist);
  print_header(\%inputs, \%hand);

  my %dvec;
  for my $d (0 .. $#dist)
  {
    $dvec{$d} = 1;
  }

  # Get the study_rank results.
  ns_recursion_done(\%hand, \@dist, \%dvec, 0, 0, 2);

  my $branch_tag = get_branch_tag();


  if ((! defined $branch_tag) || $branch_tag eq 'OK')
  {
    return $RETURN_OK;
  }

  my $letter = substr($branch_tag, 0, 1);

  if ($letter eq 'a' || $letter eq 'f')
  {
    $ON_FLAG = 0;
  }
  else
  {
    $ON_FLAG = 1;
  }

  # if ($letter ge 'h' && $letter le 'x')
  if ($letter ge 'h' && $letter le 'x' && $letter ne 'j' && $letter ne 't')
  {
    return $RETURN_OK;
  }

  if ($letter lt 'a' || $letter gt 'z')
  {
    return $RETURN_OK;
  }


  open_output_file($branch_tag);

  print_dist(\@dist, \%hand, 'short');
  my $str = print_combo();


  my (%pvec, @dist_struct);
  init_help_arrays(\@dist, \%dvec, \%pvec, \@dist_struct);

  my (@ns_history, @node_list);
  ns_init_nodes(\%hand, \@dist, \@dist_struct, 
    \@ns_history, 0, \@node_list, 0);

  # This restores the play constants.
  # Unnecessary?  Try removing later.
  ns_recursion_done(\%hand, \@dist, \%dvec, 0, 0, 2);

  print_reminder($branch_tag);

  # TEMP
  my (@strategy_list, @result_list);
  my $flag = build_strategies(\%hand, \@dist, \%dvec,
    \@strategy_list, \@result_list, \@node_list);
  if ($flag == $RETURN_ERROR)
  {
    return $RETURN_ERROR;
  }

if(1)
{
  dump_dist(\@dist, \%dvec);
  exit;
}


if(0)
{
  printf "Hand %s-%s: %d strategies\n",
        $hand{N}{CARDS},  $hand{S}{CARDS}, $#strategy_list + 1;
}

if(0)
{
  dump_result_vectors_big(\@result_list);
  exit;
}

if(0)
{
  dump_result_vectors_compact(\@result_list);
  exit;
}


  print $CURRENT_FILE $str;

  my $base = find_base2(\@result_list);

  my $s = 0;

  $hand_global_ref = \%hand;
  $dist_global_ref = \@dist;

  $str = print_base($base);
  print $CURRENT_FILE $str;

  my $ns_strat_ref = ns_init_strat_with_nodes(\%hand, \@dist, 
    \@dist_struct, \@ns_history, 0, 1, \@node_list);
  if ($ns_strat_ref== -1)
  {
    print "solve_main: ns_init_strat_with_nodes error.\n";
    return $RETURN_ERROR;
  }

  # Get the study_rank results.
  ns_recursion_done(\%hand, \@dist, \%dvec, 0, 0, 2);

  # Get the component vectors for each function.
  $#fnc_list = -1;
  init_dist_functions(\@fnc_list);
  run_dist_functions(\@fnc_list);

  my @pre_matches;
  get_pre_matches($branch_tag, \@pre_matches, \@fnc_list);

  for my $i (0 .. $#result_list)
  {
    my @match;
if ($i == 1)
{
  my $a;
}
    my $m = match_functions(\@{$result_list[$i]}, 
      \@{$pre_matches[$i]}, \@match, $base, \@fnc_list, $i); 
    if ($m == $RETURN_ERROR)
    {
      # return $RETURN_ERROR;
    }

    # print_verbal_comment(\@strategy_list, $ns_strat_ref, 
      # \@dist_struct, \%dvec, \%pvec, \%hand, \@dist, 
      # \@node_list, $i);

    print_match_functions(\@match, $i);
  }

  flush $CURRENT_FILE;

  HALTED:
  print "\n" if (0);
}
