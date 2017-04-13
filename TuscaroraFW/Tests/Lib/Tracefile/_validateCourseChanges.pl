#!/usr/bin/env perl

# Nodes have been lined up on the Y axis according to their id. Each
# second, each node sprints from x=0 to x=100 or back to x=0.

#use Modern::Perl;
local $SIG{__WARN__} = sub { die @_ }; # throw errors on invalid files.

my ($lines, $xCount, $vxCount, $yCount, $vyCount) = (0,0,0,0,0);
foreach my $line (<>) {
  # print "$line\n";
  $lines++;
  
  # Check x values, either 0 or +/- 100 more-or-less.
  while ($line =~ /Vx=(-?\d+(\.\d+e-?\d+)?)/g) {
    my $val = abs $1;
    $vxCount++;
    die "Bad line $line. Vx value must be either 0 or 100."
      if not (0 <= $val and $val < .001 or 99.99 < $val and $val < 100.01);
  }

  while ($line =~ / x=(-?\d+(\.\d+e-?\d+)?)/g) {
    my $val = abs $1;
    $xCount++;
    die "Bad line $line. x value must be either 0 or 100."
      if not (0 <= $val and $val < .001 or 99.99 < $val and $val < 100.01);
  }

  # Speed in y-direction is always zero.
  while ($line =~ /Vy=(-?\d+(\.\d+e-?\d+)?)/g) {
    $vyCount++;
    die "Bad line $line. Vy value must be 0."
      if abs $1 > .001;
  }

  # Check y position is always equal to nodeId.
  while ($line =~ m:NodeList/(\d+)/.* y=(-?\d+(\.\d+e-?\d+)?):g) {
    $yCount++;
    die "Bad line $line. y value must equal the node id."
      if $1 != $2;
  }
}

END {
  print "lines=$lines, vxCount=$vxCount, xCount=$xCount, ",
    "vyCount=$vyCount, yCount=$yCount \n";
    if($lines gt 0){
      return 1;
    }else {
      return 0;
    }
}
