#!/usr/bin/env bash
set -uo pipefail

HERE="$(cd "$(dirname "$0")" && pwd)"
BIN="${1:-$HERE/../build/itchparse}"

if [ -t 1 ]; then
  GREEN=$'\033[32m'
  RED=$'\033[31m'
  YELLOW=$'\033[33m'
  BOLD=$'\033[1m'
  RESET=$'\033[0m'
else
  GREEN=''
  RED=''
  YELLOW=''
  BOLD=''
  RESET=''
fi

if [ ! -x "$BIN" ]; then
  echo "${RED}binaire introuvable: $BIN${RESET}"
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
    echo "${YELLOW}TODO${RESET}  $name  (rc=$rc)"
    echo "$got" | sed 's/^/      /'
    todo=$((todo + 1))
    continue
  fi

  if [ "$got" = "$(cat "$exp")" ]; then
    echo "${GREEN}PASS${RESET}  $name"
    pass=$((pass + 1))
  else
    echo "${RED}FAIL${RESET}  ${BOLD}$name${RESET}"
    diff <(cat "$exp") <(echo "$got") | sed 's/^/      /'
    fail=$((fail + 1))
  fi
done

echo
if [ "$fail" -eq 0 ] && [ "$todo" -eq 0 ]; then
  echo "${GREEN}${BOLD}all green${RESET}  pass=$pass"
else
  echo "${BOLD}pass=${GREEN}$pass${RESET}${BOLD} fail=${RED}$fail${RESET}${BOLD} todo=${YELLOW}$todo${RESET}"
fi

[ "$fail" -eq 0 ]
