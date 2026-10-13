#!/bin/sh
# Packs the build into dist/ninvaders-<target>.tar.gz (.zip for Windows)
# for av: the binary at the top beside its man page, README, ChangeLog and
# gpl.txt, which -gpl shows.
#
#   asciiverse/package.sh x86_64-unknown-linux-musl
set -eu
cd "$(dirname "$0")/.."
target="$1"
case "$target" in *windows*) exe=.exe ;; *) exe= ;; esac
stage=stage/ninvaders
rm -rf "$stage" && mkdir -p "$stage" dist
cp "out/ninvaders$exe" asciiverse/ninvaders.6 README ChangeLog gpl.txt "$stage/"
if [ -n "$exe" ]; then
  (cd "$stage" && zip -qr "../../dist/ninvaders-$target.zip" .)
else
  tar -czf "dist/ninvaders-$target.tar.gz" -C "$stage" .
fi
ls dist
