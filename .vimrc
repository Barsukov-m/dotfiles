syntax on
filetype indent on

set encoding=utf-8
set t_Co=256
set mouse=i                 " mouse access
set tabstop=4 softtabstop=4
set shiftwidth=4
set expandtab
set smartindent
set relativenumber          " lines numbering
set nu                      " current line number
set noerrorbells
" set visualbell
set wrap                    " text wrap
set linebreak               " break line
set incsearch               " search
set smartcase               " match 'T' when searching 't', not vice versa
set nobackup
set noswapfile
set undodir=~/.vim/undo
set undofile
set showcmd
set laststatus=2            " show status
set noshowmode              " custom status
set pastetoggle=<F2>
set scrolloff=5

set termguicolors
set colorcolumn=80
highlight ColorColumn ctermbg=8

let g:cssColorVimDoNotMessMyUpdatetime = 1

nnoremap <silent> ; :NERDTreeToggle<CR>
nnoremap <silent> <C-f> :Files<CR>


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
Plug 'tpope/vim-eunuch'
Plug 'w0rp/ale'
Plug 'mg979/vim-visual-multi'

call plug#end()

colorscheme dracula
