#!/bin/sh

DOTFILES_DIR=~/Documents/dotfiles

# Back up the configuration
mkdir -p $DOTFILES_DIR/{.config,.local/share/fonts}
sudo cp -rv ~/{.fehbg,.vim,.vimrc,.xinitrc,.Xresources,.zshrc} $DOTFILES_DIR
sudo cp -rv ~/.config/{cava,dunst,flameshot,fontconfig,i3,kitty,mimeapps.list,neofetch,nnn,picom,polybar,rofi,xfce4,zathura} $DOTFILES_DIR/.config

# Back up fonts (except Iosevka - it's too big)
cp -r $(find .local/share/fonts -maxdepth 1 \( ! -name "*Iosevka*" \) \( ! -name "*fonts" \)) $DOTFILES_DIR/.local/share/fonts

# Check git status
gs="$(git status | grep -i modified)"

# In case of any changes
if [[ $gs == *"modified"* ]]; then
  git add --all
  git commit -m "Backup `date +'%Y-%m-%d'_%H-%M`"
  git push origin master
fi

