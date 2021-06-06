syntax on
filetype indent on
set t_Co=256
set mouse=i                 " mouse access
set tabstop=4 softtabstop=4
set shiftwidth=4
set expandtab
set smartindent
set relativenumber          " lines numbering
set nu                      " current line number
set wrap                    " text wrap
set linebreak               " break line
set smartcase               " match 'T' when searching 't', not vice versa
set ic                      " ignore case
set showcmd
set laststatus=2            " show status
set noshowmode              " custom status
set scrolloff=5

set termguicolors
set colorcolumn=80
highlight ColorColumn ctermbg=8

let g:cssColorVimDoNotMessMyUpdatetime = 1


" NNN
nnoremap <silent> ; :Np %:p:h<CR>
let g:nnn#layout = { 'window': { 'width': 0.9, 'height': 0.6, 'highlight': 'Debug' } }
let g:nnn#action = {
      \ '<c-t>': 'tab split',
      \ '<c-x>': 'split',
      \ '<c-v>': 'vsplit' }
let g:nnn#session = 'local'
let g:nnn#command = 'nnn -de'


" FZF
nnoremap <silent> <C-f> :Files<CR>
imap <C-v> <esc>P


" Plugins
call plug#begin('~/.vim/plugged')

Plug 'dracula/vim', { 'as': 'dracula' }
Plug 'mcchrish/nnn.vim'
Plug 'airblade/vim-gitgutter'
Plug 'editorconfig/editorconfig-vim'
Plug 'itchyny/lightline.vim'
Plug 'junegunn/fzf'
Plug 'junegunn/fzf.vim'
Plug 'mattn/emmet-vim'
Plug 'lilydjwg/colorizer'
Plug 'tpope/vim-eunuch'
Plug 'w0rp/ale'
Plug 'mg979/vim-visual-multi'
Plug 'neoclide/coc.nvim', {'branch': 'release'}
Plug 'preservim/nerdcommenter'

call plug#end()

colorscheme dracula
