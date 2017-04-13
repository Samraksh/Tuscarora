#!/usr/bin/perl -l

my $patchset = shift;
my $src = shift;
die "Source dir $src is not a dir" if ( not -d $src );
my $dst = shift;
die "Destination dir $dst is not a dir" if ( not -d $dst );

foreach my $srcpatch (@ARGV) {
  (my $dstpatch = $srcpatch) =~ s/$src/$dst/;
  my $rule = << "ENDRULE";
$dstpatch : $srcpatch
	\@echo applying patch $srcpatch to $dstpatch
	\@if [[ -f "$dstpatch" ]] && cd `dirname "$dstpatch"` &&      \\
	   patch -R --dry-run < "$dstpatch"; then                     \\
	  if diff "$dstpatch" "$srcpatch"; then                       \\
	    echo "patch already applied.";                            \\
	  else                                                        \\
	    patch -R < "$dstpatch" &&                                 \\
	    ./safepatch.sh "$dstpatch" "$srcpatch" &&                 \\
	    cp "$srcpatch" "$dstpatch";                               \\
	  fi;                                                         \\
	else                                                          \\
	  ./safepatch.sh "$dstpatch" "$srcpatch";                     \\
	  cp $srcpatch $dstpatch; fi
	[[ -f "$dstpatch" ]]
	\@echo applied patch $srcpatch to $dstpatch

$patchset : $dstpatch $srcpatch

ENDRULE
    
    print $rule;
}
