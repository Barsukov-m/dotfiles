syntax on 

set noerrorbells 
set tabstop=4 softtabstop=4
set shiftwidth=4
set expandtab
set smartindent
set nu
set nowrap
set smartcase
set noswapfile
set nobackup
set undodir=~/.vim/undodir
set undofile
set incsearch
set laststatus=2
set noshowmode
set pastetoggle=<F2>

set colorcolumn=80
highlight ColorColumn ctermbg=8

let g:cssColorVimDoNotMessMyUpdatetime = 1
map ; :NERDTreeToggle

call plug#begin('~/.vim/plugged')

Plug 'dracula/vim', { 'as': 'dracula' }
Plug 'airblade/vim-gitgutter'
Plug 'editorconfig/editorconfig-vim'
Plug 'itchyny/lightline.vim'
Plug 'junegunn/fzf'
Plug 'junegunn/fzf.vim'
Plug 'mattn/emmet-vim'
Plug 'lilydjwg/colorizer'
Plug 'scrooloose/nerdtree'
Plug 'terryma/vim-multiple-cursors'
Plug 'tpope/vim-eunuch'
Plug 'w0rp/ale'
Plug 'mg979/vim-visual-multi'

call plug#end()

colorscheme peachpuff

