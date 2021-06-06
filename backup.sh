#!/bin/sh

DOTFILES_DIR=~/Documents/dotfiles

# Back up the configuration
sudo cp -rv ~/{.vim,.vimrc,.xinitrc,.Xresources,.zshrc} $DOTFILES_DIR
sudo cp -rv ~/.config/{cava,dunst,flameshot,fontconfig,i3,kitty,mimeapps.list,neofetch,nnn,nvim,picom,polybar,rofi,xfce4,zathura} $DOTFILES_DIR/.config

# Check git status
gs="$(git status | grep -i modified)"

# In case of any changes
if [[ $gs == *"modified"* ]]; then
  git add --all
  git commit -m "Backup `date +'%Y-%m-%d'_%H-%M`"
  git push origin master
fi

