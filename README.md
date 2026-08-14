# openqore SDK

this is the SDK based on [openpinebuds](https://github.com/pine64/OpenPineBuds) project made to work with soundcore headphones based on bes2300p SoC.<br>
this is a part of my project - [openqore](https://github.com/nnonickreal/openqore)

## Usage

first of all, please read my [flashing guide](https://github.com/nnonickreal/openqore/blob/main/docs/FLASHING.md) and make a [backup](https://github.com/nnonickreal/openqore/blob/main/docs/READING.md)!

after reading (backuping) the flash, put the flash dump image in the "tools" folder and rename it to "firmware.bin"

```bash

./start_dev.sh # this will cause docker to start your working environment; this should take roughly 1-3 minutes depending on your network speed to the GCC hosting server

# now you will be inside of the container, and your prompt will look akin to "root@ec5410d0a265:/usr/src#"

./build.sh # this will run make and build the output program. if you have weird build errors try running clean.sh or rm -rf'ing the out folder first

# now that the firmware has finished building; if there are no errors you can load it to your headphones

# assuming that your serial port is 0, run the following to commands to program the headphones.
bestool write-image out/openqore_alpha/openqore_alpha.bin --port /dev/ttyACM0
```

## button mappings
| button | action | mapped on |
| :--- | :--- | :--- |
| PWR | click | power on |
| PWR | holding | power off |
| PWR | double click | ANC switch |
| volume up | click | vol + |
| volume down | click | vol - |
| play/pause | click | play/pause |
| play/pause | double click | next track |
| play/pause | triple click | prev track |

**note that ANC button is currently unmapped because it works buggy in the firmware**

## ✅ working 

- buttons
- leds, and a software PWM driver has been implemented for them
- BT
- sound (reverse-engineered from q35 firmware) but speakers are clipping, needs to be fixed
- ANC & transparency with stock coefficients
- ADC (ES7243E)

## ❌ WIP (not working for now)

- touch panel
- ANC button

## roadmap
- [x] make firmware & base functions work correctly
- [ ] apply an EQ for correcting the speakers frequency response
- [ ] multipoint
- [ ] audio focusing feature - make headphones focus on specific connected device by clicking some button combination; headphones should be focused even if the other device is trying to take the focus on itself
- [ ] BLE beacon based on [openhaystack](https://github.com/seemoo-lab/openhaystack) project
- [ ] always-on BLE beacon
- [ ] PC & mobile app for setting EQ and other future features with showing real-time frequency response and battery life
- [ ] USB audio


## other things
### custom build flags
currently, there is only a "SOUNDCORE_PATCH" flag. if it's enabled, it runs a script after building. the script extracts the OTA_BOOT partition from your full flash dump, which you should place in the tools/ directory and name firmware.bin. 

this script is intended for use if you plan to flash the firmware [via UART](https://github.com/nnonickreal/openqore/blob/main/docs/FLASHING.md) starting at address 0x3c000000. it's **NOT needed** if you're flashing over-the-air.

it's disabled by default to flash over-the-air.

### custom audio alerts

The default audio alerts are stored in:

`config/_default_cfg_src_/res/en/`

if you want to change the alert to a custom sound, just replace the sound file you'd like to change
(ie `config/_default_cfg_src_/res/en/SOUND_POWER_ON.opus`) with your own audio file with the same base
name (ie `config/_default_cfg_src_/res/en/SOUND_POWER_ON.mp3`) and recompile with `./build.sh`!

### language support

the `AUDIO` environment variable can be set when running the `build.sh` script to load sound files
for languages other than the default English. for example, running `AUDIO=cn ./build.sh` will load sounds files from
`config/_default_cfg_src_/res/cn/` instead of the default `en/` folder.

the current languages supported with sound files are English (`en`) and Chinese (`cn`). other languages
(or other sets of custom sounds) may be added by adding all the correct sound files into a
`config/_default_cfg_src_/res/<custom_sounds>/` directory and building with `AUDIO=<custom_sounds> ./build.sh`.

## license and credits

NOTE: currently, the SDK is not licensed under an 'open source' license. this project is a fork of [openpinebuds](https://github.com/pine64/OpenPineBuds) and all original rights to the base SDK belong to Bestechnic and pine64.
