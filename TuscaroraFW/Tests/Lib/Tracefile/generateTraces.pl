#!/usr/bin/env perl

# Call this script with the number of nodes and the total simulation
# time. This script outputs a trace file which aligns all the nodes on
# the y-axis at their node-ids, and then every second, sends them
# either to the line x=100 or back to x=0. This movement pattern is
# easy to check. Every second, a node's y coordinate is equal to its
# node id and it's x coordinate is (approximately) either 0 or 100.

#use Modern::Perl; # enforce good programming practice, and define say
                  # (println).

my ($nodes, $duration) = @ARGV;
if (not defined $nodes or not $nodes > 0 or
    not defined $duration or not $duration > 0) {
  die "$0 error: Call with the number of nodes and time."
}
# say "nodes = $nodes, duration = $duration\n";

foreach my $node (0..$nodes-1) {
  # my ($node, $ns, $time) = ('$node', '$ns', '$time');
  my ($x, $y, $z) = (0, $node, 0);
  print "\$node_($node) set X_ $x\n";
  print "\$node_($node) set Y_ $y\n";
  print "\$node_($node) set Z_ $z\n\n";
}
for my $time (0..$duration-1) {
  foreach my $node (0..$nodes-1){
    my $speed = 100;
    my ($x, $y) = (50*(1 + ((-1)**$time)), $node);
    print "\$ns_ at $time \"\$node_($node) setdest $x $y $speed\"\n";
  }
  print "\n";
}

