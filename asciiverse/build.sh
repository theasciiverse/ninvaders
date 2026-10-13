#!/bin/sh
# Builds nInvaders into out/. The compiler and curses library come from the
# environment, so one script serves every target (see
# .github/workflows/release.yml):
#
#   asciiverse/build.sh                                        # Linux
#   LIBS=-lncursesw LDFLAGS="-static -s" asciiverse/build.sh   # Alpine, static
#   LIBS=-lncurses asciiverse/build.sh                         # macOS
#   CC=x86_64-w64-mingw32-gcc CFLAGS="-O2 -I$PDC" \
#     LIBS="$PDC/wincon/pdcurses.a" LDFLAGS="-static -s" EXE=.exe asciiverse/build.sh
set -eu
cd "$(dirname "$0")/.."
CC="${CC:-cc}"
CFLAGS="${CFLAGS:--O2}"
LDFLAGS="${LDFLAGS:-}"
LIBS="${LIBS:--lncursesw}"
EXE="${EXE:-}"
mkdir -p out
$CC $CFLAGS -Wall -Wno-unused-result -I. globals.c view.c aliens.c ufo.c player.c highscore.c nInvaders.c $LDFLAGS $LIBS -o "out/ninvaders$EXE"
echo "built ninvaders$EXE"
