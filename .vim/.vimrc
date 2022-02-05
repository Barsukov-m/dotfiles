" ██╗   ██╗██╗███╗   ███╗██████╗  ██████╗
" ██║   ██║██║████╗ ████║██╔══██╗██╔════╝
" ██║   ██║██║██╔████╔██║██████╔╝██║     
" ╚██╗ ██╔╝██║██║╚██╔╝██║██╔══██╗██║     
"  ╚████╔╝ ██║██║ ╚═╝ ██║██║  ██║╚██████╗
"   ╚═══╝  ╚═╝╚═╝     ╚═╝╚═╝  ╚═╝ ╚═════╝

call plug#begin('~/.vim/plugged')
  Plug 'Yggdroot/indentLine'
  Plug 'jiangmiao/auto-pairs'
  Plug 'junegunn/fzf', { 'do': { -> fzf#install() } }
  Plug 'junegunn/fzf.vim'
  Plug 'kyazdani42/nvim-web-devicons'
  Plug 'lilydjwg/colorizer'
  Plug 'mbbill/undotree'
  Plug 'mcchrish/nnn.vim'
  Plug 'mg979/vim-visual-multi'
  Plug 'mhinz/vim-startify'
  Plug 'morhetz/gruvbox'
  Plug 'nvim-lualine/lualine.nvim'
  Plug 'nvim-treesitter/nvim-treesitter'
  Plug 'romgrk/barbar.nvim'
  Plug 'tpope/vim-commentary'

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

au FileType css :setlocal sw=2 ts=2 sts=2
au FileType html :setlocal sw=4 ts=4 sts=4
au FileType javascript :setlocal sw=2 ts=2 sts=2
au FileType python :setlocal sw=4 ts=4 sts=4
au FileType cpp :setlocal sw=2 ts=2 sts=2 noet
au FileType xml :setlocal sw=4 ts=4 sts=4

colo gruvbox
hi link cssBorderProp GruvboxAqua
hi link cssDimensionProp GruvboxAqua
hi link cssMediaProp GruvboxAqua
hi link cssPageProp GruvboxAqua
hi link cssPositioningProp GruvboxAqua
hi link cssUIProp GruvboxAqua
" set no bg colour in terminal
hi Normal ctermbg=NONE ctermfg=NONE guifg=NONE guibg=NONE
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
nnoremap <tab> >>
nnoremap <s-tab> <<
vnoremap <tab> >>
vnoremap <s-tab> <<
noremap tw :set wrap!<cr>

noremap д l
noremap л k
noremap о j
noremap р h

noremap в d
noremap В D
noremap г u
noremap Г U
noremap з p
noremap З P
noremap к r
noremap К R
noremap м v
noremap М V
noremap н y
noremap Н Y
noremap с c
noremap С C
noremap ф a
noremap Ф A
noremap ц w
noremap Ц W
noremap ш i
noremap Ш I
noremap щ o
noremap Щ O
noremap Б <
noremap Ю >
vnoremap <C-c> "+y
vnoremap <silent> <F9> :sort<cr>


" Undotree
nnoremap <silent> tu :UndotreeToggle<cr>

let g:undotree_WindowLayout = 3
let g:undotree_SetFocusWhenToggle = 1

if has("persistent_undo")
 let target_path = expand('~/.undodir')

  " create the directory and any parent directories
  " if the location does not exist.
  if !isdirectory(target_path)
    call mkdir(target_path, "p", 0700)
  endif

  let &undodir=target_path
  set udf
endif


" INTEGRATED TERMINAL
au BufEnter * if &buftype == 'terminal' | :startinsert | endif
function! OpenTerminal()
  vs term://zsh
endfunction
nnoremap <silent> tt :call OpenTerminal()<cr>
set splitbelow
set splitright
tnoremap <esc> <C-\><C-n>


" PANELS & BARBAR (TABS)
inoremap <silent> <c-h> <c-w>
nnoremap <silent> <c-h> <esc><c-w>h
nnoremap <silent> <c-j> <esc><c-w>j
nnoremap <silent> <c-k> <esc><c-w>k
nnoremap <silent> <c-l> <esc><c-w>l

" nnoremap <silent> <m-s-h> :tabmove -1<cr>
" nnoremap <silent> <m-s-l> :tabmove +1<cr>
" nnoremap <silent> <m-h> :tabprevious<cr>
" nnoremap <silent> <m-l> :tabnext<cr>
" nnoremap <silent> <c-w><cr> :tabclose<cr>

nnoremap <silent> <m-s-h> :BufferMovePrevious<cr>
nnoremap <silent> <m-s-l> :BufferMoveNext<cr>
nnoremap <silent> <m-h> :BufferPrevious<cr>
nnoremap <silent> <m-l> :BufferNext<cr>
nnoremap <silent> <c-w><cr> :BufferClose<cr>
let bufferline = get(g:, 'bufferline', {})
let bufferline.animation = v:false
let bufferline.auto_hide = v:true
let bufferline.tabpages = v:false


" NNN
nnoremap <silent> ; :Np %:p:h<cr>
" let g:nnn#layout = { 'left': '~20%' } " or right, up, down
let g:nnn#layout = { 'window': { 'width': 0.6, 'height': 0.8, 'highlight': 'Debug' } }
let g:nnn#action = {
  \ '<C-t>': 'tab split',
  \ '<C-x>': 'split',
  \ '<C-s>': 'vsplit' }
let g:nnn#session = 'local'
let g:nnn#command = 'nnn -e'



" FZF
nnoremap <c-f> :Files ~/<cr>
" nnoremap <c-f> :FZF ~/<cr>
" let g:fzf_action = {
"   \ 'ctrl-t': 'tab split',
"   \ 'ctrl-x': 'split',
"   \ 'ctrl-s': 'vsplit'
"   \ } 


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
