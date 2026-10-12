var Clay = require('@rebble/clay');
var config = require('./config.js');

var clay = new Clay(config.build(config.language()), null, { autoHandleEvents: false });

function loadSettings() {
  var s = {};
  try { s = JSON.parse(localStorage.getItem('clay-settings')) || {}; } catch (e) {}
  return {
    edo: String(s.BASIS) === '1',
    flipSouth: s.FLIP_SOUTH !== false,
    auto: s.CFG_AUTO_LOCATION !== false,
    latitude: parseFloat(s.CFG_LATITUDE),
    longitude: parseFloat(s.CFG_LONGITUDE)
  };
}

function send(message) {
  Pebble.sendAppMessage(message, function () {}, function (e) {
    console.log('送信に失敗: ' + JSON.stringify(e));
  });
}

function sendPosition(latitude, longitude, s) {
  var message = {
    'BASIS': s.edo ? 1 : 0,
    'FLIP_SOUTH': s.flipSouth ? 1 : 0,
    'TIMEZONE': -new Date().getTimezoneOffset()
  };
  if (isFinite(latitude) && isFinite(longitude) &&
      Math.abs(latitude) <= 90 && Math.abs(longitude) <= 180) {
    message.LATITUDE = Math.round(latitude * 10000);
    message.LONGITUDE = Math.round(longitude * 10000);
  }
  console.log('wadokei: ' + JSON.stringify(message));
  send(message);
}

function update() {
  var s = loadSettings();
  if (!s.auto) {
    sendPosition(s.latitude, s.longitude, s);
    return;
  }
  navigator.geolocation.getCurrentPosition(function (pos) {
    sendPosition(pos.coords.latitude, pos.coords.longitude, s);
  }, function (err) {
    console.log('位置情報を取得できません: ' + err.message);
    // 腕時計は前回の位置で計算を続ける。位置以外の設定だけ送る。
    sendPosition(NaN, NaN, s);
  }, { enableHighAccuracy: false, timeout: 30000, maximumAge: 60 * 60 * 1000 });
}

Pebble.addEventListener('ready', update);

Pebble.addEventListener('showConfiguration', function () {
  Pebble.openURL(clay.generateUrl());
});

Pebble.addEventListener('webviewclosed', function (e) {
  if (!e || !e.response) return;
  clay.getSettings(e.response, false);
  update();
});
