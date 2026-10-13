#
# This file is the default set of rules to compile a Pebble application.
#
# Feel free to customize this to your needs.
#
import os.path

top = '.'
out = 'build'


def options(ctx):
    ctx.load('pebble_sdk')


def configure(ctx):
    """
    This method is used to configure your build. ctx.load(`pebble_sdk`) automatically configures
    a build for each valid platform in `targetPlatforms`. Platform-specific configuration: add your
    change after calling ctx.load('pebble_sdk') and make sure to set the correct environment first.
    Universal configuration: add your change prior to calling ctx.load('pebble_sdk').
    """
    ctx.load('pebble_sdk')


def build(ctx):
    ctx.load('pebble_sdk')

    build_worker = os.path.exists('worker_src')
    binaries = []

    cached_env = ctx.env
    for platform in ctx.env.TARGET_PLATFORMS:
        ctx.env = ctx.all_envs[platform]
        ctx.set_group(ctx.env.PLATFORM_NAME)
        # スクリーンショット用: WADOKEI_DEMO="通日,分[,edo][,eto][,q]" で日付と時刻を固定する
        demo = os.environ.get('WADOKEI_DEMO')
        if demo:
            parts = demo.split(',')
            ctx.env.append_value('DEFINES', ['DEMO_YDAY=' + parts[0], 'DEMO_MINUTE=' + parts[1]])
            # 3 つ目以降: edo（江戸の暦法）、eto（十二支で呼ぶ）、q（四つに分ける）
            flags = {'edo': 'DEMO_EDO=1', 'eto': 'DEMO_ZODIAC=1', 'q': 'DEMO_QUARTERS=1'}
            for flag in parts[2:]:
                ctx.env.append_value('DEFINES', [flags[flag]])
        # WADOKEI_PLACE="緯度,経度,時差" で場所を固定する（緯度経度は 1e-4 度、時差は分）
        place = os.environ.get('WADOKEI_PLACE')
        if place:
            lat, lon, tz = place.split(',')
            ctx.env.append_value('DEFINES', ['DEMO_LATITUDE=(' + lat + ')', 'DEMO_LONGITUDE=(' + lon + ')',
                                             'DEMO_TIMEZONE=(' + tz + ')'])
        app_elf = '{}/pebble-app.elf'.format(ctx.env.BUILD_DIR)
        ctx.pbl_build(source=ctx.path.ant_glob('src/c/**/*.c'), target=app_elf, bin_type='app')

        if build_worker:
            worker_elf = '{}/pebble-worker.elf'.format(ctx.env.BUILD_DIR)
            binaries.append({'platform': platform, 'app_elf': app_elf, 'worker_elf': worker_elf})
            ctx.pbl_build(source=ctx.path.ant_glob('worker_src/c/**/*.c'),
                          target=worker_elf,
                          bin_type='worker')
        else:
            binaries.append({'platform': platform, 'app_elf': app_elf})
    ctx.env = cached_env

    ctx.set_group('bundle')
    ctx.pbl_bundle(binaries=binaries,
                   js=ctx.path.ant_glob(['src/pkjs/**/*.js',
                                         'src/pkjs/**/*.json',
                                         'src/common/**/*.js']),
                   js_entry_file='src/pkjs/index.js')
