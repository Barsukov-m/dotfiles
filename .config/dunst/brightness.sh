#!/usr/bin/env bash

dunstify -t 3000 -r 90210 " $(printf %.0f%% $(xbacklight -get))"
