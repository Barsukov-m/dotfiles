" vimrc

call plug#begin('~/.vim/plugged')

Plug 'jiangmiao/auto-pairs'
Plug 'mg979/vim-visual-multi'
Plug 'tpope/vim-commentary'

Plug 'itchyny/lightline.vim'
Plug 'iamcco/markdown-preview.nvim', { 'do': 'cd app && yarn install'  }
Plug 'mcchrish/nnn.vim'

" Plug 'airblade/vim-gitgutter'
Plug 'arcticicestudio/nord-vim'
Plug 'dracula/vim', { 'as': 'dracula' }
Plug 'lilydjwg/colorizer'
Plug 'morhetz/gruvbox'
Plug 'neoclide/coc.nvim', {'branch': 'release'}
let g:coc_global_extensions = [
  \ 'coc-clangd',
  \ 'coc-css',
  \ 'coc-emmet',
  \ 'coc-eslint',
  \ 'coc-html',
  \ 'coc-json',
  \ 'coc-prettier',
  \ 'coc-python',
  \ 'coc-tsserver'
  \ ]

call plug#end()


syntax on
colorscheme gruvbox

if !has('gui_running')
  set t_Co=256
endif

if (has("termguicolors"))
 set termguicolors
endif

filetype indent on
set mouse=i
set tabstop=4 softtabstop=4
set shiftwidth=4
set expandtab
set smartindent
set relativenumber
set nu
set wrap
set linebreak
set smartcase
set ic
set showcmd
set laststatus=0
" set noshowmode
set scrolloff=5
set noswapfile
set path+=**


"" General key mappings
imap <C-v> <esc>P

nmap <silent> <space> :noh<CR>

inoremap <C-j> gj
inoremap <C-k> gk

nnoremap р h
nnoremap о j
nnoremap л k
nnoremap д l
nnoremap tw :set wrap!<CR>

vnoremap <F9> :sort


""NNN
nnoremap <silent> ; :Np %:p:h<CR>
" let g:nnn#layout = { 'left': '~20%' } " or right, up, down
let g:nnn#layout = { 'window': { 'width': 0.6, 'height': 0.6, 'highlight': 'Debug' } }
let g:nnn#action = {
  \ '<C-t>': 'tab split',
  \ '<C-x>': 'split',
  \ '<C-v>': 'vsplit' }
let g:nnn#session = 'local'
let g:nnn#command = 'nnn -deH'
   
   
"" FZF
nnoremap <C-p> :FZF<CR>
let g:fzf_action = {
  \ 'ctrl-t': 'tab split',
  \ 'ctrl-x': 'split',
  \ 'ctrl-v': 'vsplit'
  \ } 
  
   
"" Integrated terminal
set splitright
set splitbelow
tnoremap <Esc> <C-\><C-n>
au BufEnter * if &buftype == 'terminal' | :startinsert | endif
function! OpenTerminal()
  split term://zsh
  resize 10
endfunction
nnoremap tt :call OpenTerminal()<CR>
   
   
"" Tabs and panels 
nnoremap <C-h> <C-w>h
nnoremap <C-j> <C-w>j
nnoremap <C-k> <C-w>k
nnoremap <C-l> <C-w>l
inoremap <C-H> <C-W>
nnoremap <M-h> :tabprevious<CR>
nnoremap <M-l> :tabnext<CR>
nnoremap <M-S-h> :tabmove -1<CR>
nnoremap <M-S-l> :tabmove +1<CR>
         
        
"" CoC        
set cmdheight=2     
set updatetime=300  
set shortmess+=c    
       
if has("nvim-0.5.0") || has("patch-8.1.1564")
  set signcolumn=number
else 
  set signcolumn=yes
endif

inoremap <silent><expr> <TAB>
  \ pumvisible() ? "\<C-n>" :
  \ <SID>check_back_space() ? "\<TAB>" :
  \ coc#refresh()
inoremap <expr><S-TAB> pumvisible() ? "\<C-p>" : "\<C-h>"
    
function! s:check_back_space() abort
  let col = col('.') - 1
  return !col || getline('.')[col - 1]  =~# '\s'
endfunction
   
if has('nvim')
  inoremap <silent><expr> <c-space> coc#refresh()
else
  inoremap <silent><expr> <c-@> coc#refresh()
endif

""" Prettier
command! -nargs=0 Prettier :CocCommand prettier.formatFile
   
   
"" Markdown Preview
let g:mkdp_auto_start = 1
let g:mkdp_auto_close = 1
let g:mkdp_refresh_slow = 0
let g:mkdp_echo_preview_url = 1
let g:mkdp_preview_options = {
  \ 'mkit': {},
  \ 'katex': {},
  \ 'uml': {},
  \ 'maid': {},
  \ 'disable_sync_scroll': 0,
  \ 'sync_scroll_type': 'middle',
  \ 'hide_yaml_meta': 1,
  \ 'sequence_diagrams': {},
  \ 'flowchart_diagrams': {},
  \ 'content_editable': v:false,
  \ 'disable_filename': 0
  \ }
nmap mp <Plug>MarkdownPreviewToggle
