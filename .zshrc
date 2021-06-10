export ZSH="/home/michael/.oh-my-zsh"
export EDITOR="nvim"
export VISUAL="nvim"
export BROWSER="brave"
export FZF_DEFAULT_COMMAND="ag -g ''"

ZSH_THEME="main"
DISABLE_UPDATE_PROMPT="true"
plugins=(git)

source $ZSH/oh-my-zsh.sh
# source ~/.local/share/icons-in-terminal/icons_bash.sh

mkcd () {
  mkdir -p -- "$1" && cd -P -- "$1"
}

xset b off # disable the beep

## NNN Configuration
export NNN_TRASH=1
export NNN_COLORS="2136"
export NNN_FIFO="/tmp/nnn.fifo"
export NNN_BMS='r:/;d:~/Documents;D:~/Documents/dotfiles;u:~/Documents/university;'
export NNN_PLUG='d:dragdrop;e:suedit;f:fzcd;l:launch;m:nmount;p:preview-tui;'
export NNN_DE_FILE_MANAGER="nautilus"
export NNN_FALLBACK_OPENER="xdg-open"
export NNN_FALLBACK_OPENER="gio open"
export NNN_FALLBACK_OPENER="gvfs-open"


## Aliases
alias l="ls -A"
alias ll="ls -Alh"
alias lls="ls -Alhgo"
alias alist="cat ~/.zshrc | grep 'alias '"
alias up="sudo pacman -Syu && flatpak update -y && sudo updatedb"
alias u="cd ~/Documents/university"
alias p="python3"
alias n="nnn -de"
alias nf="neofetch"
alias nv="nvim"
alias ascii="less ~/.local/share/etc/ascii"
alias inet="ping archlinux.org"
alias conadd="nmcli device connect wlan0"
alias conup="nmcli connection up Barsik_2"
alias t="redshift -P -O"
alias t0="redshift -x"
alias trash

## Screensavers
alias pipes="bash ~/.local/share/etc/pipes.sh"
alias rain="sh ~/.local/share/etc/rain.sh"
alias wtime="tty-clock -cbnC 4"
alias feh="feh -d --scale-down"

## Kitty
alias icat="kitty +kitten icat"
alias kdiff="kitty +kitten diff"
alias unicode="kitty +kitten unicode_input"

## Pacman
alias S="sudo pacman -S"                    # Install a package
alias Syu="sudo pacman -Syu"                # Full upgrade
alias Qu="sudo pacman -Qu"                  # List upgradable
alias Qs="sudo pacman -Qs"                  # Short description
alias Qi="sudo pacman -Qi"                  # Package info
alias Qk="sudo pacman -QK"                  # Number of package files
alias Ql="sudo pacman -Ql"                  # List of package files
alias R="sudo pacman -R"                    # Remove a package
alias R!="sudo pacman -Rcns"                # Remove a package with dependencies
# Remove unneccessary packages
alias Rdt="for i in $(pacman -Qdtq); do sudo pacman -R "$i"; done"
