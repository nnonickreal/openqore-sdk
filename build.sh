#!/usr/bin/env sh

if make -j "$(nproc)" T=openqore_alpha DEBUG=1 ANC_APP=0 ANC_FF_ENABLED=0 ANC_FB_ENABLED=0 APP_ANC_KEY=1 SOUNDCORE_PATCH=0 >log.txt 2>&1; then
	echo "build success"
else
	echo "build failed and call log.txt"
	grep "error:" log.txt
fi
