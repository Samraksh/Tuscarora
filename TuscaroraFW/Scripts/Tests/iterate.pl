#!/usr/bin/perl
use strict; use warnings;
$_ = shift;
my @lines=split;
for (@ARGV) { # each argv is set to $_
  # if arg is "val=abc", strip off "val=" and save in $v
  my $v = (s/^(\w+=)//) ? $1: "";
  @lines = map { 
    my $a=$_; # save current argument
    map { "$_ $v$a" } @lines; # append arg to each line
  } split; # splits $_ on whitespace
};
#print "$_\n" for @lines;
for my $command (@lines) {
  print `$command`;
}

