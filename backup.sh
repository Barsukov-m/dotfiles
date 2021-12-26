#!/bin/sh

DOTFILES_DIR=~/.config/dotfiles

# Back up the configuration
mkdir -p $DOTFILES_DIR/{.local/share,.config}
sudo cp -rv ~/{.themes,.vim,.xinitrc,.Xresources,.zshrc} $DOTFILES_DIR
sudo cp -rv ~/.local/share/icons $DOTFILES_DIR/.local/share
sudo cp -rv ~/.config/{alacritty,bspwm,dunst,flameshot,fontconfig,GIMP,mimeapps.list,neofetch,nvim,pcmanfm,picom,polybar,rofi,scripts,sxhkd,xfce4,zathura} $DOTFILES_DIR/.config

# Check git status
gs="$(git status | grep -i modified)"

# In case of any changes
if [[ $gs == *"modified"* ]]; then
  git add --all
  git commit -m "Backup `date +'%Y-%m-%d'_%H-%M`"
  git push origin master
fi

