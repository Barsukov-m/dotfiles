" ██    ██ ██ ███    ███ 
" ██    ██ ██ ████  ████ 
" ██    ██ ██ ██ ████ ██ 
"  ██  ██  ██ ██  ██  ██ 
"   ████   ██ ██      ██ 

" VIM-PLUG
call plug#begin('~/.vim/plugged')
  Plug 'jiangmiao/auto-pairs'
  Plug 'mg979/vim-visual-multi'
  Plug 'tpope/vim-commentary'

  Plug 'Yggdroot/indentLine'
  Plug 'iamcco/markdown-preview.nvim', { 'do': 'cd app && yarn install'  }
  Plug 'itchyny/lightline.vim'
  Plug 'mcchrish/nnn.vim'
  Plug 'junegunn/goyo.vim'
  Plug 'junegunn/limelight.vim'
  Plug 'reedes/vim-colors-pencil'
  Plug 'subnut/vim-iawriter'

  " Plug 'airblade/vim-gitgutter'
  Plug 'joshdick/onedark.vim'
  Plug 'nvim-treesitter/nvim-treesitter'
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


" LOOK & FEEL
if !has('gui_running')
  set t_Co=256
endif

if (has("termguicolors"))
 set termguicolors
endif

autocmd FileType css :setlocal sw=4 ts=4 sts=4
autocmd FileType html :setlocal sw=4 ts=4 sts=4
autocmd FileType javascript :setlocal sw=2 ts=2 sts=2
autocmd FileType python :setlocal sw=4 ts=4 sts=4
autocmd FileType xml :setlocal sw=4 ts=4 sts=4

colorscheme gruvbox
highlight link cssBorderProp GruvboxAqua
highlight link cssDimensionProp GruvboxAqua
highlight link cssMediaProp GruvboxAqua
highlight link cssPageProp GruvboxAqua
highlight link cssPositioningProp GruvboxAqua
highlight link cssUIProp GruvboxAqua
let g:gruvbox_italic=1

filetype indent on
set ch=1              " cmdheight
set et                " expandtab
set ic                " ignorecase
set lbr               " linebreak
set ls=2              " laststatus
set mouse=a
set nosmd             " noshowmode
set noswapfile
set nu                " number
set pa+=**            " path
set rnu               " relativenumber
set sc                " showcmd
set scs               " smartcase
set si                " smartindent
set so=5              " scrolloff
set sw=2 ts=2 sts=2   " tab options
set wrap
syntax on


" KEY MAPPINGS
imap <C-v> <esc>P
nnoremap <C-j> gj
nnoremap <C-k> gk
nmap <silent> <space> :noh<cr>
nnoremap tw :set wrap!<cr>
nnoremap д l
nnoremap л k
nnoremap о j
nnoremap р h
vnoremap <C-c> "+y
vnoremap <silent> <F9> :sort<cr>
noremap <silent> <F1> :Iawriter<cr>


" INTEGRATED TERMINAL
au BufEnter * if &buftype == 'terminal' | :startinsert | endif
function! OpenTerminal()
  split term://zsh
  resize 10
endfunction
nnoremap tt :call OpenTerminal()<cr>
set splitbelow
set splitright
tnoremap <Esc> <C-\><C-n>


" TABS & PANELS 
inoremap <silent> <C-H> <C-W>
nnoremap <silent> <C-h> <C-w>h
nnoremap <silent> <C-j> <C-w>j
nnoremap <silent> <C-k> <C-w>k
nnoremap <silent> <C-l> <C-w>l
nnoremap <silent> <M-S-h> :tabmove -1<cr>
nnoremap <silent> <M-S-l> :tabmove +1<cr>
nnoremap <silent> <M-h> :tabprevious<cr>
nnoremap <silent> <M-l> :tabnext<cr>


" LIGTHLINE
let g:lightline = {
  \ 'colorscheme': 'seoul256',
  \ }


" NNN
nnoremap <silent> ; :Np %:p:h<cr>
" let g:nnn#layout = { 'left': '~20%' } " or right, up, down
let g:nnn#layout = { 'window': { 'width': 0.6, 'height': 0.8, 'highlight': 'Debug' } }
let g:nnn#action = {
  \ '<C-t>': 'tab split',
  \ '<C-x>': 'split',
  \ '<C-s>': 'vsplit' }
let g:nnn#session = 'local'
let g:nnn#command = 'nnn -deH'


" FZF
nnoremap <C-p> :FZF<cr>
let g:fzf_action = {
  \ 'ctrl-t': 'tab split',
  \ 'ctrl-x': 'split',
  \ 'ctrl-s': 'vsplit'
  \ } 


" COC        
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

"" Prettier
command! -nargs=0 Prettier :CocCommand prettier.formatFile


" Markdown Preview
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
