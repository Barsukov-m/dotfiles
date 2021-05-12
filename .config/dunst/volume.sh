#!/bin/bash

msgID="2345"
volume="$(pamixer --get-volume)"

if [ "$volume" -gt "65" ]; then
	dunstify  "  $volume" -t 800 -r $msgID
elif [ "$volume" == "0" ]; then
	dunstify  "  Muted " -t 800 -r $msgID
else
	dunstify  "  $volume" -t 800 -r $msgID
fi
