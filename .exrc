if &cp | set nocp | endif
let s:cpo_save=&cpo
set cpo&vim
nmap r <Plug>SetTmuxVars
nmap  vip
vmap  "ry :call Send_to_Tmux(@r)
map Q gq
nmap <silent> \w\t <Plug>VimwikiTabMakeDiaryNote
nmap <silent> \w\w <Plug>VimwikiMakeDiaryNote
nmap <silent> \w\i <Plug>VimwikiDiaryGenerateLinks
nmap <silent> \wi <Plug>VimwikiDiaryIndex
nmap <silent> \ws <Plug>VimwikiUISelect
nmap <silent> \wt <Plug>VimwikiTabIndex
nmap <silent> \ww <Plug>VimwikiIndex
nmap \tl :exe "tabn ".g:lasttab
nmap gx <Plug>NetrwBrowseX
nnoremap <silent> <Plug>NetrwBrowseX :call netrw#NetrwBrowseX(expand("<cWORD>"),0)
map <F6> :up  :!sh render_2.sh 
map <F7> :up  :!sh render.sh
map <F8> :up  :!csound % 
noremap <F1> 
inoremap  u
inoremap kj 
let &cpo=s:cpo_save
unlet s:cpo_save
set background=dark
set backspace=indent,eol,start
set backupskip=/tmp/*,/var/spool/cron/*
set expandtab
set helplang=en
set hlsearch
set incsearch
set mouse=a
set nrformats=alpha
set ruler
set runtimepath=~/.vim,/usr/share/vim/vimfiles,/usr/share/vim/vim73,/usr/share/vim/vimfiles/after,~/.vim/after,/usr/local/share/lilypond/vim,~/.vim/after/syntax,~/.vim/eclim,~/.vim/eclim/after
set shiftwidth=4
set tabstop=4
set nowritebackup
" vim: set ft=vim :
