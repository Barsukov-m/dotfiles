export ZSH='/home/michael/.config/oh-my-zsh'
export EDITOR='nvim'
export VISUAL='nvim'
export BROWSER='brave'
export TERM='alacritty'
export TERMINAL='alacritty'
export FZF_DEFAULT_COMMAND='ag -g "" --hidden'

ZSH_THEME='main'
DISABLE_UPDATE_PROMPT='true'
DISABLE_AUTO_UPDATE=true

plugins=(git)

source $ZSH/oh-my-zsh.sh
# source ~/.local/share/icons-in-terminal/icons_bash.sh

# xset b off
# xset r rate 250 50

if [ -z "${DISPLAY}" ] && [ "${XDG_VTNR}" -eq 1 ]; then
  exec startx
fi

remove_packets() {
  for i in $(pacman -Qdtq); do
    sudo pacman -R $i;
  done
}

gpp() {
  if (($# == 2)); then # if 2 args
    g++ -g -Wall -Wextra -pedantic -std=c++11 $1 -o $2 && ./$2
  else
    g++ -g -Wall -Wextra -pedantic -std=c++11 $1 && ./a.out
  fi
}

debug() {
  g++ -g -Wall -Wextra -pedantic -std=c++11 $1 && gdb a.out
}


## NNN Configuration
export NNN_BMS='r:/run/media/;d:~/.config/dotfiles;u:~/Documents/university;f:~/Documents/front-end;p:~/Pictures;'
export NNN_COLORS='3241'
export NNN_DE_FILE_MANAGER='nautilus'
export NNN_FALLBACK_OPENER='gio open'
export NNN_FALLBACK_OPENER='gvfs-open'
export NNN_FALLBACK_OPENER='xdg-open'
export NNN_FIFO='/tmp/nnn.fifo'
export NNN_PLUG='d:dragdrop;e:suedit;p:preview-tui;t:imgview;x:xdgdefault;'
export NNN_TRASH=1

## Aliases
alias HDD='sudo mkdir -p /run/media/michael/HDD && sudo mount /dev/sdb1 /run/media/michael/HDD'
alias N='sudo nnn -de'
alias agh='ag --hidden -g'
alias ascii='less ~/.local/share/etc/ascii'
alias btc='bluetoothctl power on && bluetoothctl'
alias conadd='nmcli device connect $(ls /sys/class/net | grep -o "wl.*")'
alias inet='ping archlinux.org'
alias l='ls -A'
alias ll='ls -Alhgo'
alias n='nnn -de'
alias nf='neofetch'
alias nv='nvim'
alias p='python3'
alias t0='redshift -x'
alias t='redshift -P -O'
alias up='sudo pacman -Syu && sudo updatedb'

## Screensavers
alias rain='sh ~/.local/share/etc/rain.sh'
alias wtime='tty-clock -cbnC 4'
alias feh='feh -d --scale-down'

## Pacman
alias S='sudo pacman -S'                    # Install a package
alias Syu='sudo pacman -Syu'                # Full upgrade
alias Qu='pacman -Qu'                       # List upgradable
alias Qs='pacman -Qs'                       # Short description
alias Qi='pacman -Qi'                       # Package info
alias Qk='pacman -QK'                       # Number of package files
alias Ql='pacman -Ql'                       # List of package files
alias R='sudo pacman -R'                    # Remove a package
alias R!='sudo pacman -Rcns'                # Remove a package with dependencies
# Remove unneccessary packages
alias Rdt='remove_packets'
