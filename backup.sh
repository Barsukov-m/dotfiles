#!/bin/sh

DOTFILES_DIR=~/Documents/dotfiles

# Backup the configuration files
sudo cp -r {.fehbg,.vim,.vimrc,.xinitrc,.Xresources,.zshrc} $DOTFILES_DIR
cp -r ~/.config/{alacritty,cava,dunst,flameshot,fontconfig,i3,kitty,mimeapps.list,neofetch,nnn,picom,polybar,rofi,zathura} $DOTFILES_DIR
cp -r ~/.local/share/fonts $DOTFILES_DIR

# Check git status
gs="$(git status | grep -i modified)"

# In case of any changes
if [[ $gs == *"modified"* ]]; then
  git add --all
  git commit -m "Backup `date +'%Y-%m-%d'_%H-%M`"
  git push origin master
fi

