#!/bin/bash
TMPBG=/tmp/screen.png
RES=1366x768
# RES=1920x1080

rm $TMPBG

ffmpeg -f x11grab -video_size $RES -i $DISPLAY -filter_complex "boxblur=8:8" -vframes 1 $TMPBG

B='#272727'  # blank
C='#ebdbb211'  # clear
D='#ebdbb2cc'  # default
T='#ebdbb2'  # text
V='#ebdbb2'  # verification
W='#989719'  # wrong

i3lock -i $TMPBG -e --keylayout 1\
    --time-font=Lato\
    --date-font=Lato\
    --layout-font=Lato\
    --verif-font=Lato\
    --wrong-font=Lato\
    --greeter-font=Lato\
    --insidever-color=$C\
    --ringver-color=$V\
    --insidewrong-color=$C\
    --ringwrong-color=$W\
    --inside-color=$B\
    --ring-color=$D\
    --line-color=$B\
    --separator-color=$D\
    --verif-color=$T\
    --wrong-color=$T\
    --time-color=$T\
    --date-color=$T\
    --layout-color=$T\
    --keyhl-color=$W\
    --bshl-color=$W\
    --time-str="%H:%M"\
    --date-str="%A, %m %Y"\
    --verif-text="..."\
    --wrong-text="Try again"\
    --lock-text=""\
    --lockfailed-text=""\
    --radius=100\
    --ring-width=5
