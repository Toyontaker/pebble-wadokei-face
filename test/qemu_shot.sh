#!/bin/sh
# SDK 付属のまっさらなフラッシュで qemu を直接起動し、インストールしてスクリーンショットを撮る。
# 普段使いのエミュレータ（~/.pebble-sdk/<sdk>/<platform>/）には触れない。
# 使い方: [QUICKVIEW=1] test/qemu_shot.sh 機種 出力.png 作業ディレクトリ
set -u
p=$1; out=$2; work=$3
sdk=$HOME/.pebble-sdk/SDKs/current/sdk-core/pebble/$p/qemu
mkdir -p "$work"
rm -f "$out"
bzcat "$sdk/qemu_spi_flash.bin.bz2" > "$work/$p.bin"
# SDK 4.33 以降の qemu-pebble（7 以降）に合わせた引数（pebble-tool の sdk/emulator.py と同じ）
spi="-drive if=none,id=spi-flash,file=$work/$p.bin,format=raw"
case $p in
  basalt)  m="-machine pebble-snowy-bb -cpu cortex-m4 $spi" ;;
  chalk)   m="-machine pebble-s4-bb -cpu cortex-m4 $spi" ;;
  diorite) m="-machine pebble-silk-bb -cpu cortex-m4 -mtdblock $work/$p.bin" ;;
  emery)   m="-machine pebble-emery -cpu cortex-m33 -drive if=mtd,format=raw,file=$work/$p.bin -audio driver=none,id=audio0" ;;
  flint)   m="-machine pebble-flint -cpu cortex-m4 -drive if=mtd,format=raw,file=$work/$p.bin -audio driver=none,id=audio0" ;;
  gabbro)  m="-machine pebble-gabbro -cpu cortex-m33 -drive if=mtd,format=raw,file=$work/$p.bin -audio driver=none,id=audio0" ;;
esac
PATH=$HOME/.pebble-sdk/SDKs/current/toolchain/bin:$PATH
port=$((20000 + $$ % 20000))
qemu-pebble -rtc base=localtime -serial null -serial tcp::$port,server=on,wait=off \
  -serial tcp::$((port+1)),server=on,wait=off -kernel "$sdk/qemu_micro_flash.bin" \
  -display none $m >"$work/$p.qemu.log" 2>&1 &
qpid=$!
ok=0
for i in $(seq 1 30); do
  sleep 3
  timeout 20 pebble emu-bt-connection --connected yes --qemu localhost:$port >/dev/null 2>&1 && { ok=1; break; }
done
if [ $ok = 1 ]; then
  timeout 120 pebble install --qemu localhost:$port >/dev/null 2>&1
  sleep 4
  # QUICKVIEW=1 を付けると、画面下に通知（Timeline Quick View）を出した状態で撮る
  if [ -n "${QUICKVIEW:-}" ]; then
    timeout 30 pebble emu-set-timeline-quick-view on --qemu localhost:$port >/dev/null 2>&1
    sleep 3
  fi
  timeout 60 pebble screenshot --qemu localhost:$port --no-open "$out" >/dev/null 2>&1
fi
kill $qpid 2>/dev/null
[ -f "$out" ] && echo "$out" || echo "$p: 失敗"
