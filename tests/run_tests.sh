#!/usr/bin/env bash
set -uo pipefail

HERE="$(cd "$(dirname "$0")" && pwd)"
BIN="${1:-$HERE/../build/itchparse}"

if [ ! -x "$BIN" ]; then
  echo "binaire introuvable: $BIN"
  echo "usage: $0 [chemin/vers/itchparse]"
  exit 2
fi

python3 "$HERE/make_fixtures.py" >/dev/null

pass=0
fail=0
todo=0

for f in "$HERE"/fixtures/*.itch; do
  name="$(basename "$f" .itch)"
  exp="$HERE/expected/$name.txt"

  got="$("$BIN" -f "$f" 2>/dev/null)"
  rc=$?

  if [ ! -f "$exp" ] || [ "$(cat "$exp")" = "TODO" ]; then
    echo "TODO  $name  (rc=$rc)"
    echo "$got" | sed 's/^/      /'
    todo=$((todo + 1))
    continue
  fi

  if [ "$got" = "$(cat "$exp")" ]; then
    echo "PASS  $name"
    pass=$((pass + 1))
  else
    echo "FAIL  $name"
    diff <(cat "$exp") <(echo "$got") | sed 's/^/      /'
    fail=$((fail + 1))
  fi
done

echo
echo "pass=$pass fail=$fail todo=$todo"
[ "$fail" -eq 0 ]
