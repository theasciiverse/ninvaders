# Changes from upstream

This is the Asciiverse packaging of nInvaders 0.1.1 (GPL-2.0-or-later), the
ncurses Space Invaders clone by Thomas Dettbarn (dettus), Alexander
Hollinger, Sebastian Gutsfeld and Matthias Thar, with additional code by
Mike Saarna: the last upstream release (2003-05-08,
ninvaders.sourceforge.net). History here starts from the release tarball
(ninvaders-0.1.1.tar.gz from SourceForge, sha256
bfbc5c378704d9cf5e7fed288dac88859149bee5ed0850175759d310b61fd30b, the same
file Debian distributes as ninvaders_0.1.1.orig.tar.gz), tagged `0.1.1`.

## The CVS history (branch `cvs`)

SourceForge's CVS holds the project's whole history, 2003-04-04 to
2003-10-17, including unreleased work after 0.1.1. It is here as branch
`cvs` (tag `cvs-head`), not joined to `master`: 66 commits converted with
cvs-fast-export from SourceForge's code snapshot
(https://sourceforge.net/code-snapshots/cvs/n/ni/ninvaders.zip, sha256
84b415e3f4f4e8916f9da1ee14f042e95c9f5e7f4edf2fff0ab35d436755f020), dated
in German time. CVS users are given the names in the project's own files:
hollinge is Alexander Hollinger and segoh Sebastian Gutsfeld (both from
ChangeLog); senorhoss, who made the initial import and signed the project's
web page, is kept under that SourceForge name, since nothing in the files
says who it is.

After 0.1.1 (2003-05-08) the CVS has:

- a high score table (file.c, then highscore.c and highscore.h, Alexander
  Hollinger and Sebastian Gutsfeld, May to September 2003) and a title
  screen alternating the alien legend with the TOP 10: ported, see below
- an autoconf build (configure, configure.in, Makefile.in, October 2003):
  not taken, the build here is asciiverse/build.sh
- gpl.txt renamed LICENSE: not taken, nothing reads that name
- no other changes to the game

## Debian's patches

From ninvaders 0.1.1-6 (ninvaders_0.1.1-6.debian.tar.xz, sha256
c540c5cab5e0bf327564d208c5ff5f597a1096000a74241a1fb484952efc481e), one
commit each, tagged `debian/0.1.1-6`:

- `debian-changes`: Matthew Palmer's accumulated changes as Debian's
  maintainer since 2002, among them the lowest_ship overflow fix (Matt
  Chapman's report and patch, Debian #597036) and the GCC 10 fixes, which
  define the shared globals once (#957609).
- `gcc-15`: GCC 15 compatibility (Matt Palmer, #1097466).

Debian's package names no packaging repository (no Vcs-Git), so there is
no `debian` remote.

## Ours

- **A game loop instead of a timer signal.** The game ticked from a
  SIGALRM timer whose handler moved and drew everything with curses while
  main waited in getch(). main now waits for a key at most until the next
  tick (50 a second) and runs the tick itself: no curses calls in a signal
  handler, and it runs where there is no setitimer (Windows). Turbo mode
  (a movement key held down) works as before.
- **Centred in the terminal.** The 80x24 battlefield is drawn centred and
  centred again when the terminal is resized; a smaller terminal shows it
  from the top left, as before. After a resize the window is grown back if
  ncurses cut it down, and during a game the status line, bunkers, player
  and aliens are drawn again.
- **Only changes are sent to the terminal.** refreshScreen repainted the
  whole battlefield on every tick, 50 times a second ("needed to display
  graphics properly at startup on some terminals"); the full repaint now
  happens once at startup and after a resize. The 45-second preview went
  from 10.7 MB to about 0.5 MB, and play over a slow link is smoother.
- **-gpl shows gpl.txt.** It read Debian's
  /usr/share/common-licenses/GPL, which is version 3 on current Debian, not
  the game's version 2. It now reads the gpl.txt beside the game first.
- **Windows.** Builds with MinGW against PDCurses: <curses.h> instead of
  <ncurses.h>, <windows.h> for Sleep(), and rand() for BSD random().
- **The high score table, from the CVS, finished.** The 2003 table
  (credited to Alexander Hollinger and Sebastian Gutsfeld in the commit) was
  never finished: entries came only from a debug key, and it was saved at
  an exit the game never reached. Now a score that makes the TOP 10 asks
  for a name after the game over banner (up to 8 characters, ENTER saves,
  an empty name is "player"; keys in the first second are ignored, so keys
  still held from play do not end up in it; quitting with q records
  nothing), and the table is saved at once to the player's data folder:
  `$XDG_DATA_HOME/ninvaders/highscore` (else
  `~/.local/share/ninvaders/highscore`), or `%APPDATA%\ninvaders\highscore`
  on Windows. Skill 0 plays for the expert table, the default 1 for the
  normal one and 2 to 9 for the beginner one, and the title screen shows
  the table being played for. Fixed in the 2003 code: addEntry wrote past
  the end of the table; names were read with no length limit; the table
  was defined in the header (a link error since GCC 10); a damaged or
  edited file is checked line by line and the standard table (the
  authors' own names) used instead; no error messages over the curses
  screen. The title screen also made four curses pads every frame and never
  freed them.
- **Seeded random numbers.** The game never seeded them, so the aliens'
  bombs and the UFO came the same way in every game. They are seeded from
  the clock at startup.

## Packaging (new files)

- `asciiverse/build.sh`, `asciiverse/package.sh`: one compiler call, and
  one archive per target with the binary (ninvaders), its man page,
  README, ChangeLog and gpl.txt.
- `asciiverse/ninvaders.6`: Debian's man page by Matthew Palmer, written
  for Debian "but may be used by others".
- `asciiverse/record.py`, `asciiverse/ninvaders.cast`: the catalog preview
  (100x30, 45 s) and the player that recorded it, reading the screen
  through pyte: it waits on the title until its TOP 10 has shown, then
  aims at the alien columns, dodges bombs and leads its shot at the UFO.
- `.github/workflows/release.yml`: static Linux in Alpine (x86_64,
  aarch64), macOS (x86_64, arm64) and Windows x86_64 (MinGW, PDCurses),
  plus SHA256SUMS, on a tag.
