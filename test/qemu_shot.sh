#!/bin/sh
# SDK 付属のまっさらなフラッシュで qemu を直接起動し、インストールしてスクリーンショットを撮る。
# 普段使いのエミュレータ（~/.pebble-sdk/<sdk>/<platform>/）には触れない。
# 使い方: test/qemu_shot.sh 機種 出力.png 作業ディレクトリ
set -u
p=$1; out=$2; work=$3
sdk=$HOME/.pebble-sdk/SDKs/current/sdk-core/pebble/$p/qemu
mkdir -p "$work"
bzcat "$sdk/qemu_spi_flash.bin.bz2" > "$work/$p.bin"
case $p in
  basalt)  m="-machine pebble-snowy-bb -cpu cortex-m4 -pflash" ;;
  chalk)   m="-machine pebble-s4-bb -cpu cortex-m4 -pflash" ;;
  emery)   m="-machine pebble-snowy-emery-bb -cpu cortex-m4 -pflash" ;;
  gabbro)  m="-machine pebble-spalding-gabbro-bb -cpu cortex-m4 -pflash" ;;
  diorite|flint) m="-machine pebble-silk-bb -cpu cortex-m4 -mtdblock" ;;
esac
PATH=$HOME/.pebble-sdk/SDKs/current/toolchain/bin:$PATH
port=$((20000 + $$ % 20000))
qemu-pebble -rtc base=localtime -serial null -serial tcp::$port,server,nowait \
  -serial tcp::$((port+1)),server,nowait -pflash "$sdk/qemu_micro_flash.bin" \
  -display none $m "$work/$p.bin" >"$work/$p.qemu.log" 2>&1 &
qpid=$!
ok=0
for i in $(seq 1 30); do
  sleep 3
  timeout 20 pebble emu-bt-connection --connected yes --qemu localhost:$port >/dev/null 2>&1 && { ok=1; break; }
done
if [ $ok = 1 ]; then
  timeout 120 pebble install --qemu localhost:$port >/dev/null 2>&1
  sleep 4
  timeout 60 pebble screenshot --qemu localhost:$port --no-open "$out" >/dev/null 2>&1
fi
kill $qpid 2>/dev/null
[ -f "$out" ] && echo "$out" || echo "$p: 失敗"
