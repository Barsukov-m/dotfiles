# export PATH=$HOME/bin:/usr/local/bin:$PATH
export ZSH="/home/michael/.oh-my-zsh"
export VISUAL=vim

ZSH_THEME="robbyrussell"

DISABLE_UPDATE_PROMPT="true"

# Uncomment the following line to change how often to auto-update (in days).
# export UPDATE_ZSH_DAYS=13

# Uncomment the following line if pasting URLs and other text is messed up.
# DISABLE_MAGIC_FUNCTIONS="true"

# Uncomment the following line to enable command auto-correction.
# ENABLE_CORRECTION="true"

# Uncomment the following line if you want to disable marking untracked files
# under VCS as dirty. This makes repository status check for large repositories
# much, much faster.
# DISABLE_UNTRACKED_FILES_DIRTY="true"

# Standard plugins can be found in $ZSH/plugins/
# Custom plugins may be added to $ZSH_CUSTOM/plugins/
# Example format: plugins=(rails git textmate ruby lighthouse)
plugins=(git)

source $ZSH/oh-my-zsh.sh
source ~/.local/share/icons-in-terminal/icons_bash.sh

# Preferred editor for local and remote sessions
# if [[ -n $SSH_CONNECTION ]]; then
#   export EDITOR='vim'
# else
#   export EDITOR='mvim'
# fi

# ALIAS
alias alist="cat ~/.zshrc | grep 'alias '"
alias up="sudo pacman -Syu"
alias fup="sudo pacman -Syu && flatpak update -y && sudo updatedb"
alias u="cd ~/Documents/university"
alias p="python3"
alias nf="neofetch"
alias ascii="less ~/.local/share/misc/ascii"
alias inet="ping archlinux.org"
alias ra="ranger"
alias t="redshift -P -O"
alias t0="redshift -x"
# Screensavers
alias pipes="bash ~/.local/share/misc/pipes.sh"
alias ufetch="sh ~/.local/share/misc/ufetch-manjaro"
alias rain="sh ~/.local/share/misc/rain.sh"
alias wtime="tty-clock -cbnC 4"
alias feh="feh --scale-down"
