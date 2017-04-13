#!/usr/bin/env bash
# set -x

if (( "$#" != 1 )); then
    echo "Usage: $0 repr"
    exit 1;
fi

TMP=$(mktemp -d)
cfile="${TMP}/repr.cpp"
ofile="${TMP}/repr"

cat > "$cfile" <<EOF
#include <string>
using std::string;
#include <sstream>
using std::stringstream;
#include <iostream>
using std::cout;

// for mmap
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

EOF

cat "$1".repr >> "$cfile"
#cat "$1".repr 1>&2

cat >> "$cfile" <<EOF
void printlines ( mystruct *s, int len ) {
  for ( int i = 0; i < len; i++ ) {
    cout << s[i].str() << "\n";
  }
};
#define handle_error(msg) do { perror(msg); exit(EXIT_FAILURE); } while (0)

int main ( int argc, char *argv[] ) {
  if ( argc !=  2 ) {
    fprintf(stderr, "Usage: %s file\n", argv[0] );
    exit(EXIT_FAILURE);
  }

  int fd = open(argv[1], O_RDONLY);
  if (fd == -1) handle_error("open");

  /* To obtain file size */
  struct stat sb;
  if (fstat(fd, &sb) == -1) handle_error("fstat");
  size_t length = sb.st_size;

  fprintf(stderr, "struct size = %ld\n", sizeof ( mystruct ));
  if ( length != length / sizeof ( mystruct ) * sizeof ( mystruct )) {
    fprintf(stderr, "file is too long: length of file is %ld, but only %ld bytes required.\n", 
            length, length/sizeof ( mystruct ));
  }
  mystruct *addr = (mystruct*) mmap(NULL, length, PROT_READ,
                       MAP_PRIVATE, fd, 0);
  if (addr == MAP_FAILED) handle_error("mmap");

  printlines ( addr, length / sizeof ( mystruct ) );
  exit(EXIT_SUCCESS);
}
EOF

g++ "$cfile" -I ${TUS}/Include -o "$ofile" && "$ofile" "$1"
rm -fr "${TMP}"

