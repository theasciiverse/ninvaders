"""Records nInvaders' catalog preview (hub/RECORDING.md): 100x30, about 45 s.

Runs the game under `asciinema rec` in a 100x30 pseudo-terminal, reads the
screen through pyte and plays: it aims at the alien columns, dodges the
bombs falling towards it, and leads its shot when the UFO crosses. Usage,
from the repo root after asciiverse/build.sh (the binary in out/), with
pyte installed:

    python3 asciiverse/record.py

The game runs by name from out/ on PATH, so the cast's header holds no local
path. The cast goes to asciiverse/ninvaders.cast.
"""
import fcntl, os, pty, select, shutil, struct, sys, tempfile, termios, time

import pyte

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
COLS, ROWS = 100, 30
FIELD_W, FIELD_H = 80, 24
PLAYER_ROW = FIELD_H - 2          # PLAYERPOSY in view.h
LENGTH = 46.0                     # seconds of play before quitting

# ncurses repeats a character with REP (CSI n b), which pyte does not know
_draw = pyte.Screen.draw


def _draw_remembering(self, data):
    if data:
        self.last_char = data[-1]
    _draw(self, data)


def _repeat_last(self, n=1, *rest, **kw):
    if getattr(self, "last_char", None):
        _draw(self, self.last_char * max(1, n))


pyte.Screen.draw = _draw_remembering
pyte.Screen.repeat_last = _repeat_last
pyte.Stream.csi = dict(pyte.Stream.csi, b="repeat_last")


class Rec:
    def __init__(self):
        self.home = tempfile.mkdtemp(prefix="ninv-rec-")
        env = {
            "PATH": f"{os.path.join(ROOT, 'out')}:{os.environ['PATH']}",
            "HOME": self.home,
            "TERM": "xterm-256color",
            "COLORTERM": "truecolor",
            "LANG": "C.UTF-8",
        }
        self.cast = os.path.join(ROOT, "asciiverse", "ninvaders.cast")
        self.screen = pyte.Screen(COLS, ROWS)
        self.stream = pyte.ByteStream(self.screen)
        self.pid, self.fd = pty.fork()
        if self.pid == 0:
            os.chdir(self.home)
            os.execvpe("asciinema", ["asciinema", "rec", "--overwrite", "-q", "--idle-time-limit", "2",
                "--window-size", f"{COLS}x{ROWS}", "--capture-env", "TERM,COLORTERM,LANG",
                "--title", "nInvaders", "--command", "ninvaders", self.cast], env)
        fcntl.ioctl(self.fd, termios.TIOCSWINSZ, struct.pack("HHHH", ROWS, COLS, 0, 0))
        self.start = time.time()

    def pump(self, t):
        end = time.time() + t
        while True:
            left = end - time.time()
            r, _, _ = select.select([self.fd], [], [], max(0.0, min(0.01, left)))
            if r:
                try:
                    self.stream.feed(os.read(self.fd, 65536))
                except OSError:
                    return False
            if left <= 0:
                return True

    def key(self, k, wait=0.0):
        os.write(self.fd, k.encode() if isinstance(k, str) else k)
        self.pump(wait)

    def finish(self):
        self.key("q", 0.5)
        end = time.time() + 5
        while time.time() < end:
            if not self.pump(0.1):
                break
            done, _ = os.waitpid(self.pid, os.WNOHANG)
            if done:
                break
        else:
            os.kill(self.pid, 9)    # SIGKILL: nothing more is written to the cast
        try:
            os.waitpid(self.pid, 0)
        except ChildProcessError:
            pass
        shutil.rmtree(self.home, ignore_errors=True)
        print(f"{self.cast}: {os.path.getsize(self.cast) // 1024} KB, {time.time() - self.start:.0f} s",
              file=sys.stderr)


def field(rec):
    """The battlefield's rows, cut out of the screen where the game centred it."""
    oy, ox = (ROWS - FIELD_H) // 2, (COLS - FIELD_W) // 2
    return [rec.screen.display[oy + y][ox:ox + FIELD_W] for y in range(FIELD_H)]


def look(rows):
    """Player x (left edge), alien columns, bombs, the UFO and our missile."""
    player = rows[PLAYER_ROW].find("/-^-\\")
    bombs = [(x, y) for y in range(FIELD_H - 1) for x, c in enumerate(rows[y]) if c == ":"]
    missile = any("!" in rows[y] for y in range(FIELD_H - 2))
    cols = set()
    for y in range(1, FIELD_H - 9):
        for x, c in enumerate(rows[y]):
            if c not in " :!":
                cols.add(x)
    ufo = rows[0].find("<")
    return player, sorted(cols), bombs, missile, ufo


def danger(x, bombs):
    """A bomb about to land on a player whose left edge is at x."""
    return any(x - 1 <= bx <= x + 5 and PLAYER_ROW - 7 <= by < PLAYER_ROW for bx, by in bombs)


def main():
    rec = Rec()
    rec.pump(3.0)                   # the title screen, animated
    rec.key(" ", 0.6)               # start
    end = rec.start + LENGTH
    while time.time() < end:
        rows = field(rec)
        player, cols, bombs, missile, ufo = look(rows)
        if player < 0:              # exploding, or between levels
            rec.pump(0.06)
            continue
        gun = player + 2
        if danger(player, bombs):
            left_ok = player > 6 and not danger(player - 3, bombs)
            rec.key("h" if left_ok else "l", 0.04)
            continue
        if ufo >= 0:
            target = ufo + 2 - 11   # the missile needs about 0.84 s, the UFO moves left
        elif cols:
            target = min(cols, key=lambda c: abs(c - gun))
        else:
            target = gun
        if abs(target - gun) <= 0 and not missile:
            rec.key(" ", 0.05)
        elif target < gun:
            rec.key("h", 0.04)
        elif target > gun:
            rec.key("l", 0.04)
        else:
            rec.pump(0.04)
    rec.finish()


if __name__ == "__main__":
    main()
