// Clay 設定画面の定義。スマホの言語が日本語なら日本語、それ以外は英語で出す。
// 保存される値（messageKey と value）は両方の言語で共通。

var TEXT = {
  ja: {
    title: '和時計',
    intro: '昼（明け六つ〜暮れ六つ）と夜をそれぞれ六等分する不定時法の時計です。' +
      '一刻の長さは季節と場所で毎日変わります。',
    basisHeading: '明け六つ・暮れ六つ',
    basisLabel: '昼と夜の境目',
    basisSun: '日の出・日の入り',
    basisEdo: '江戸の暦法（日の出の約36分前・日の入りの約36分後）',
    basisNote: '江戸の寛政暦以降は、太陽が地平線の下 7°21′40″ にあり' +
      '「手のひらの筋が見える」明るさの時を明け六つ・暮れ六つとしました。',
    placeHeading: '場所',
    auto: 'スマホの位置情報を使う',
    latitude: '緯度（北緯が正）',
    longitude: '経度（東経が正）',
    placeNote: '位置情報を使わないときは、この緯度・経度で計算します。初期値は東京です。',
    directionHeading: '太陽の向き',
    flipSouth: '南半球では右から左へ動かす',
    directionNote: '北半球では、南の空を見上げたときと同じく、太陽が左から昇って右へ沈む時計回りです。' +
      'オンにすると、南半球（緯度がマイナス）では北の空と同じく右から昇って左へ沈む反時計回りになります。' +
      'オフにすると、どこでも時計回りです。',
    save: '保存'
  },
  en: {
    title: 'Wadokei',
    intro: 'A Japanese temporal-hour clock: daytime (dawn to dusk) and night are each ' +
      'divided into six "koku". The length of a koku changes every day with the season and your location.',
    basisHeading: 'Dawn and dusk (Ake-mutsu / Kure-mutsu)',
    basisLabel: 'Boundary between day and night',
    basisSun: 'Sunrise and sunset',
    basisEdo: 'Edo-period rule (about 36 min before sunrise and after sunset)',
    basisNote: 'From the Kansei calendar onward, Edo-period Japan counted dawn and dusk as the moment ' +
      'the sun is 7°21′40″ below the horizon, when "the lines on your palm become visible".',
    placeHeading: 'Location',
    auto: 'Use phone location',
    latitude: 'Latitude (north is positive)',
    longitude: 'Longitude (east is positive)',
    placeNote: 'When phone location is off, these coordinates are used. The default is Tokyo.',
    directionHeading: 'Direction of the sun',
    flipSouth: 'Right to left in the Southern Hemisphere',
    directionNote: 'In the Northern Hemisphere the sun rises on the left and sets on the right, ' +
      'as it does when you face south, so the dial turns clockwise. When this is on, south of the equator ' +
      '(negative latitude) it rises on the right and sets on the left, as it does when you face north. ' +
      'When off, the dial always turns clockwise.',
    save: 'Save'
  }
};

function language() {
  var lang = (typeof navigator !== 'undefined' && navigator.language) || '';
  return /^ja/i.test(lang) ? 'ja' : 'en';
}

function build(lang) {
  var t = TEXT[lang] || TEXT.en;
  return [
    {
      type: 'heading',
      defaultValue: t.title
    },
    {
      type: 'text',
      defaultValue: t.intro
    },
    {
      type: 'section',
      items: [
        { type: 'heading', defaultValue: t.basisHeading },
        {
          type: 'radiogroup',
          messageKey: 'BASIS',
          label: t.basisLabel,
          defaultValue: '0',
          options: [
            { label: t.basisSun, value: '0' },
            { label: t.basisEdo, value: '1' }
          ]
        },
        {
          type: 'text',
          defaultValue: t.basisNote
        }
      ]
    },
    {
      type: 'section',
      items: [
        { type: 'heading', defaultValue: t.placeHeading },
        {
          type: 'toggle',
          messageKey: 'CFG_AUTO_LOCATION',
          label: t.auto,
          defaultValue: true
        },
        {
          type: 'input',
          messageKey: 'CFG_LATITUDE',
          label: t.latitude,
          defaultValue: '35.6895',
          attributes: { type: 'number', step: 'any' }
        },
        {
          type: 'input',
          messageKey: 'CFG_LONGITUDE',
          label: t.longitude,
          defaultValue: '139.6917',
          attributes: { type: 'number', step: 'any' }
        },
        {
          type: 'text',
          defaultValue: t.placeNote
        }
      ]
    },
    {
      type: 'section',
      items: [
        { type: 'heading', defaultValue: t.directionHeading },
        {
          type: 'toggle',
          messageKey: 'FLIP_SOUTH',
          label: t.flipSouth,
          defaultValue: true
        },
        {
          type: 'text',
          defaultValue: t.directionNote
        }
      ]
    },
    {
      type: 'submit',
      defaultValue: t.save
    }
  ];
}

module.exports = {
  build: build,
  language: language
};
