" ██╗   ██╗██╗███╗   ███╗██████╗  ██████╗
" ██║   ██║██║████╗ ████║██╔══██╗██╔════╝
" ██║   ██║██║██╔████╔██║██████╔╝██║     
" ╚██╗ ██╔╝██║██║╚██╔╝██║██╔══██╗██║     
"  ╚████╔╝ ██║██║ ╚═╝ ██║██║  ██║╚██████╗
"   ╚═══╝  ╚═╝╚═╝     ╚═╝╚═╝  ╚═╝ ╚═════╝

call plug#begin('~/.vim/plugged')
  Plug 'jiangmiao/auto-pairs'
  Plug 'mg979/vim-visual-multi'
  Plug 'tpope/vim-commentary'

  Plug 'nvim-treesitter/nvim-treesitter'
  Plug 'lilydjwg/colorizer'
  Plug 'morhetz/gruvbox'

  " Plug 'itchyny/lightline.vim'
  Plug 'Yggdroot/indentLine'
  Plug 'iamcco/markdown-preview.nvim', { 'do': 'cd app && yarn install'  }
  Plug 'kyazdani42/nvim-web-devicons'
  Plug 'nvim-lualine/lualine.nvim'
  Plug 'mcchrish/nnn.vim'
  Plug 'mhinz/vim-startify'
  Plug 'romgrk/barbar.nvim'

  Plug 'neoclide/coc.nvim', {'branch': 'release'}
  let g:coc_global_extensions = [
    \ 'coc-clangd',
    \ 'coc-css',
    \ 'coc-emmet',
    \ 'coc-eslint',
    \ 'coc-html',
    \ 'coc-json',
    \ 'coc-python',
    \ 'coc-tsserver'
    \ ]
call plug#end()


" VIM LOOK
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

packadd termdebug
let g:termdebug_wide=1

filetype indent on
" set rnu               " relativenumber
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
set sc                " showcmd
set scs               " smartcase
set si                " smartindent
set so=5              " scrolloff
set sw=2 ts=2 sts=2   " tab options
set wrap
syntax on


" KEY MAPPINGS
imap <C-v> <esc>P
nmap <silent> <space> :noh<cr>
nnoremap tw :set wrap!<cr>

nnoremap д l
nnoremap л k
nnoremap о j
nnoremap р h

nnoremap в d
nnoremap В D
nnoremap г u
nnoremap Г U
nnoremap з p
nnoremap З P
nnoremap к r
nnoremap К R
nnoremap м V
nnoremap М V
nnoremap н y
nnoremap Н Y
nnoremap с c
nnoremap С C
nnoremap ф a
nnoremap Ф A
nnoremap ц w
nnoremap Ц W
nnoremap ш i
nnoremap Ш I
nnoremap щ o
nnoremap Щ O
vnoremap <C-c> "+y
vnoremap <silent> <F9> :sort<cr>


" INTEGRATED TERMINAL
au BufEnter * if &buftype == 'terminal' | :startinsert | endif
function! OpenTerminal()
  vs term://zsh
endfunction
nnoremap tt :call OpenTerminal()<cr>
set splitbelow
set splitright
tnoremap <esc> <C-\><C-n>


" PANELS & BARBAR (TABS)
inoremap <silent> <c-h> <c-w>
nnoremap <silent> <c-h> <esc><c-w>h
nnoremap <silent> <c-j> <esc><c-w>j
nnoremap <silent> <c-k> <esc><c-w>k
nnoremap <silent> <c-l> <esc><c-w>l
" nnoremap <silent> <M-S-h> :tabmove -1<cr>
" nnoremap <silent> <M-S-l> :tabmove +1<cr>
" nnoremap <silent> <M-h> :tabprevious<cr>
" nnoremap <silent> <M-l> :tabnext<cr>

nnoremap <silent> <m-s-h> :BufferMovePrevious<cr>
nnoremap <silent> <m-s-l> :BufferMoveNext<cr>
nnoremap <silent> <m-h> :BufferPrevious<cr>
nnoremap <silent> <m-l> :BufferNext<cr>
nnoremap <silent> <c-w><cr> :BufferClose<cr>
let bufferline = get(g:, 'bufferline', {})
let bufferline.animation = v:false
let bufferline.auto_hide = v:true
let bufferline.tabpages = v:false


" LIGTHLINE
" let g:lightline = {
"   \ 'colorscheme': 'seoul256',
"   \ }


" NNN
nnoremap <silent> ; :Np %:p:h<cr>
" let g:nnn#layout = { 'left': '~20%' } " or right, up, down
let g:nnn#layout = { 'window': { 'width': 0.6, 'height': 0.8, 'highlight': 'Debug' } }
let g:nnn#action = {
  \ '<C-t>': 'tab split',
  \ '<C-x>': 'split',
  \ '<C-s>': 'vsplit' }
let g:nnn#session = 'local'
let g:nnn#command = 'nnn -de'


" FZF
nnoremap <C-p> :FZF ~/<cr>
let g:fzf_action = {
  \ 'ctrl-t': 'tab split',
  \ 'ctrl-x': 'split',
  \ 'ctrl-s': 'vsplit'
  \ } 


" COC        
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


" STARTIFY
let g:startify_files_number = 5
let g:startify_bookmarks = [
  \ {'d': '~/.config/dotfiles'},
  \ {'u': '~/Documents/university'},
  \ {'f': '~/Documents/front-end'}
  \ ]

" hi StartifyHeader  guifg=#928373
" hi StartifyBracket guifg=
" hi StartifyFile    guifg=
" hi StartifyFooter  guifg=
" hi StartifyNumber  guifg=
" hi StartifyPath    guifg=
" hi StartifySlash   guifg=
" hi StartifySpecial guifg=
