export ZSH="/home/michael/.oh-my-zsh"
export VISUAL=vim

ZSH_THEME="robbyrussell"
DISABLE_UPDATE_PROMPT="true"
# export UPDATE_ZSH_DAYS=13
# ENABLE_CORRECTION="true"
# DISABLE_UNTRACKED_FILES_DIRTY="true"

# Standard plugins:     $ZSH/plugins/
# Custom plugins        $ZSH_CUSTOM/plugins/
# Example:              plugins=(rails git textmate ruby lighthouse)
plugins=(git)

source $ZSH/oh-my-zsh.sh
source ~/.local/share/icons-in-terminal/icons_bash.sh

mkcd () {
  mkdir -p -- "$1" && cd -P -- "$1"
}

trash () {
  mv "$1" ~/.local/share/Trash/files
}


## NNN Configuration
export NNN_BMS='r:/;d:~/Downloads;D:~/Documents;u:~/Documents/university;'
export NNN_PLUG='d:dragdrop;l:launch;e:suedit;'



## Aliases
alias alist="cat ~/.zshrc | grep 'alias '"
alias up="sudo pacman -Syu && flatpak update -y && sudo updatedb"
alias u="cd ~/Documents/university"
alias p="python3"
alias nf="neofetch"
alias ascii="less ~/.local/share/misc/ascii"
alias inet="ping archlinux.org"
alias ra="ranger"
alias n="nnn"
alias t="redshift -P -O"
alias t0="redshift -x"
alias trash
## Screensavers
alias pipes="bash ~/.local/share/misc/pipes.sh"
alias ufetch="sh ~/.local/share/misc/ufetch-manjaro"
alias rain="sh ~/.local/share/misc/rain.sh"
alias wtime="tty-clock -cbnC 4"
alias feh="feh --scale-down"
# Kitty
alias icat="kitty +kitten icat"
alias kdiff="kitty +kitten diff"
alias unicode="kitty +kitten unicode_input"
## Pacman
# Install a package
alias S="sudo pacman -S"
# Full upgrade
alias Syu="sudo pacman -Syu"
# List upgradable
alias Qu="sudo pacman -Qu"
# Short description
alias Qs="sudo pacman -Qs"
# Package info
alias Qi="sudo pacman -Qi"
# Number of package files
alias Qk="sudo pacman -QK"
# List of package files
alias Ql="sudo pacman -Ql"
# Remove a package
alias R="sudo pacman -R"
# Remove unneccessary packages
alias Rdt="sudo pacman -R $(pacman -Qdtq)"
