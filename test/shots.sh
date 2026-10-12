#!/bin/sh
# 使い方: test/shots.sh 名前 通日 分[,edo] 機種...
# 例: test/shots.sh summer 171 1215 basalt chalk
#     test/shots.sh edo 284 330,edo basalt
name=$1; yday=$2; minute=$3; shift 3
WADOKEI_DEMO="$yday,$minute" pebble build >/dev/null 2>&1 || { echo build failed; exit 1; }
for p in "$@"; do
  out="screenshots/${p}_${name}.png"
  rm -f "$out"
  # 複数のエミュレータが残っていると応答しなくなるので、1台ずつ起動し直す
  pids=$(pgrep -f 'qemu-pebbl[e]'); [ -n "$pids" ] && kill -9 $pids
  pids=$(pgrep -f '[p]ypkjs'); [ -n "$pids" ] && kill -9 $pids
  rm -f /tmp/pb-emulator.json
  for try in 1 2 3; do
    timeout 150 pebble install --emulator "$p" >/dev/null 2>&1 || continue
    sleep 4
    timeout 60 pebble screenshot --emulator "$p" --no-open "$out" >/dev/null 2>&1 && break
  done
  [ -f "$out" ] && echo "$out" || echo "$p: 失敗"
done
