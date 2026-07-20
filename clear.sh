#!/usr/bin/env sh

make -j "$(nproc)" T=openqore_alpha DEBUG=1 clean
