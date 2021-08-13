#!/usr/bin/perl -w 

$| = 1;
$time = $ARGV[0];
$count = $time*60;

 while ($count > 0)  {
     &TimeOutput($count);
     sleep(1);
     $count--;
}

system("notify-send -i ~/.pomo/icon.png \"Take a break\" \"Timer has expired.\"");
system("canberra-gtk-play -i message");

sub TimeOutput {
    $countin = shift;
    $days = int($countin/(24*60*60)); chomp($days);
    $hours = ($countin/(60*60))%24;
    $mins  = ($countin/60)%60;
    $secs  = $countin%60;
    printf "\r%02d:%02d:%02d ", $hours, $mins, $secs;
}
