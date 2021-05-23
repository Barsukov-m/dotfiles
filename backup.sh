#!/bin/sh

DOTFILES_DIR=~/Documents/dotfiles

mkdir -p $DOTFILES_DIR/{.config,.local/share}

# Backup the configuration files
sudo cp -rv ~/{.fehbg,.oh-my-zsh,.vim,.vimrc,.xinitrc,.Xresources,.zshrc} $DOTFILES_DIR
cp -rv ~/.config/{alacritty,cava,dunst,flameshot,fontconfig,i3,kitty,mimeapps.list,neofetch,nnn,picom,polybar,rofi,zathura} $DOTFILES_DIR/.config
cp -r ~/.local/share/fonts $DOTFILES_DIR/.local/share

# Check git status
gs="$(git status | grep -i modified)"

# In case of any changes
if [[ $gs == *"modified"* ]]; then
  git add --all
  git commit -m "Backup `date +'%Y-%m-%d'_%H-%M`"
  git push origin master
fi

