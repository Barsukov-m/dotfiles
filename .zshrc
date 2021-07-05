export ZSH="/home/michael/.config/oh-my-zsh"
export EDITOR="nvim"
export VISUAL="nvim"
export BROWSER="brave"
export FZF_DEFAULT_COMMAND="ag -g '' --hidden"

ZSH_THEME="main"
DISABLE_UPDATE_PROMPT="true"
plugins=(git)

source $ZSH/oh-my-zsh.sh
# source ~/.local/share/icons-in-terminal/icons_bash.sh

mkcd () {
  mkdir -p -- "$1" && cd -P -- "$1"
}

xset b off # disable the sound bell
xset r rate 250 50

## NNN Configuration
export NNN_TRASH=1
export NNN_COLORS="3241"
export NNN_FIFO="/tmp/nnn.fifo"
export NNN_BMS='r:/run/media/michael;d:~/Documents;D:~/Documents/dotfiles;u:~/Documents/university;f:~/Documents/front-end;'
export NNN_PLUG='d:dragdrop;e:suedit;f:fzcd;l:launch;m:nmount;p:preview-tui;'
export NNN_DE_FILE_MANAGER="nautilus"
export NNN_FALLBACK_OPENER="xdg-open"
export NNN_FALLBACK_OPENER="gio open"
export NNN_FALLBACK_OPENER="gvfs-open"


## Aliases
alias l="ls -A"
alias ll="ls -Alhgo"
alias alist="cat ~/.zshrc | grep 'alias '"
alias up="sudo pacman -Syu && flatpak update -y && sudo updatedb"
alias u="cd ~/Documents/university"
alias p="python3"
alias n="nnn -deH"
alias nf="neofetch"
alias nv="nvim"
alias ascii="less ~/.local/share/etc/ascii"
alias inet="ping archlinux.org"
alias conadd="nmcli device connect wlan0"
alias conadd="nmcli device connect $(ls /sys/class/net | grep -o "wl.*")"
alias t="redshift -P -O"
alias t0="redshift -x"
alias trash

## Screensavers
# alias pipes="sh ~/.local/share/etc/pipes.sh"
alias rain="sh ~/.local/share/etc/rain.sh"
alias wtime="tty-clock -cbnC 4"
alias feh="feh -d --scale-down"

## Kitty
alias icat="kitty +kitten icat"

## Pacman
alias S="sudo pacman -S"                    # Install a package
alias S!="sudo pacman -Syu"                 # Full upgrade
alias Syu="sudo pacman -Syu"                # Full upgrade
alias Qu="pacman -Qu"                       # List upgradable
alias Qs="pacman -Qs"                       # Short description
alias Qi="pacman -Qi"                       # Package info
alias Qk="pacman -QK"                       # Number of package files
alias Ql="pacman -Ql"                       # List of package files
alias R="sudo pacman -R"                    # Remove a package
alias R!="sudo pacman -Rcns"                # Remove a package with dependencies
# Remove unneccessary packages
alias Rdt="for i in $(pacman -Qdtq); do sudo pacman -R "$i"; done"
