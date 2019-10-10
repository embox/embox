" Vim syntax file
" Language:	Mybuild
" Maintainer:	Denis Deryugin <deryugin.denis@gmail.com>
" Last Change:	2019 June 03
"
" Put this file to ~/.vim/syntax/
"
" Also add following command to ~/.vimrc 
"
" au BufRead,BufNewFile *.my,Mybuild,mods.config set filetype=mybuild

syn keyword	myDepends	depends
syn keyword	myInclude	include
syn keyword	mySource	source
syn keyword	myStatic	static
syn keyword	myAbstract	abstract
syn keyword	myModule	module
syn keyword	myOption	option
syn keyword	myPackage	package
syn keyword	myConfiguration	configuration
syn match	myFlag		'@[a-zA-Z0-9]*'

syn match	myComment	'//.*'
syn region	myMultilineComment	start='/\*' end='\*/'


syn region	myString	start='"' end='"'
syn region	myTripleString	start="'''" end="'''"

syn keyword	myPreFlagOption	help man name stage path chmod target_name xattr chown script cppflags
syn region	myPreFlagInner	start='(' end=')' contains=myPreFlagOption,myString,myTripleString

syn keyword	myType		boolean string number


hi def link myDepends		keyword
hi def link mySource		keyword
hi def link myInclude		statement

hi def link myConfiguration	structure
hi def link myPackage		structure
hi def link myModule		structure

hi def link myFlag		include
hi def link myString		string
hi def link myTripleString	string
hi def link myComment		comment
hi def link myMultilineComment	comment

hi def link myType		type
hi def link myOption		structure
hi def link myStatic		storageclass
hi def link myAbstract		storageclass

hi def link myPreFlagOption	type
