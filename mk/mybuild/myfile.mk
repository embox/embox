# Application for 'MyFile' grammar (version 0.1).
#    MyBuild file
# Author: Eldar Abusalimov

#
# All API functions and internals of the parser are defined in their own
# namespace which defaults to the basename of the definition script.
#
# This name is primarily used to refer the grammar when calling
# 'gold_parse_file' and 'gold_parse_stream' functions.
#

# Override this variable to define everything in a different namespace.
# Must be a single word.
gold_prefix := $(basename $(notdir $(lastword $(MAKEFILE_LIST))))

ifeq ($(call singleword,$(gold_prefix)),)
$(error 'gold_prefix' is empty or not a single word, \
	you have to define it properly in order to continue)
endif

gold_prefix := $(call trim,$(gold_prefix))

ifndef __gold_grammar_$(gold_prefix)_mk
__gold_grammar_$(gold_prefix)_mk := 1

# By default it is assumed that you'll place tables file in the same directory
# with this one and name it with '-tables' prefix.

# Fix the following line to override default behavior.
include $(dir $(lastword $(MAKEFILE_LIST)))$(gold_prefix)-tables.mk

#
# Symbols.
#
# Each symbol is converted by the corresponding handler (if any has been
# defined). Handler is a function with the following signature:
#
# Params:
#   For terminals:
#     1. List of decimal char codes representing the token.
#     2. Location of the first character of this token: 'line:column'.
#   For nonterminals:
#     1. The result of applying one of rule handlers (it may be used
#        to extract common symbol value postprocessing from its rules).
#
# Return:
#   Converted value. The value is then passed to a rule containing that symbol
#   in its RHS or returned to user in case of the Start Symbol.
#

# Symbol: ','
#define $(gold_prefix)_create-Comma
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: '.'
#define $(gold_prefix)_create-Dot
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: '.*'
#define $(gold_prefix)_create-DotTimes
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: ';'
#define $(gold_prefix)_create-Semi
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: '{'
#define $(gold_prefix)_create-LBrace
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: '}'
#define $(gold_prefix)_create-RBrace
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: abstract
#define $(gold_prefix)_create-abstract
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: ccfags
#define $(gold_prefix)_create-ccfags
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: depends
#define $(gold_prefix)_create-depends
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: extends
#define $(gold_prefix)_create-extends
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: file
#define $(gold_prefix)_create-file
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: Identifier
#define $(gold_prefix)_create-Identifier
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: import
#define $(gold_prefix)_create-import
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: module
#define $(gold_prefix)_create-module
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: package
#define $(gold_prefix)_create-package
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: source
#define $(gold_prefix)_create-source
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: static
#define $(gold_prefix)_create-static
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: StringLiteral
define $(gold_prefix)_create-StringLiteral
	$(subst \\ ,\,# Unescape '\' sequences.
		$(subst \",",$(subst \',',$(subst \t,$(\t),$(subst \n,$(\n),$(subst \r,,
			$(subst \\,\\ ,
				$(call gold_default_create,
					# Remove '"' at the ends.
					$(wordlist 3,$(words $1),x $1)
				)
			)
		)))))
	)
endef

# Symbol: <CcflagsDecl>
#define $(gold_prefix)_create-CcflagsDecl
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: <DependencyDecl>
#define $(gold_prefix)_create-DependencyDecl
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: <ImportDecl>
#define $(gold_prefix)_create-ImportDecl
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: <ImportDecls>
#define $(gold_prefix)_create-ImportDecls
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: <Model>
#define $(gold_prefix)_create-Model
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: <ModuleBodyDecl>
#define $(gold_prefix)_create-ModuleBodyDecl
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: <ModuleBodyDecls>
#define $(gold_prefix)_create-ModuleBodyDecls
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: <ModuleDecl>
#define $(gold_prefix)_create-ModuleDecl
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: <ModuleDecls>
#define $(gold_prefix)_create-ModuleDecls
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: <ModuleModifier>
#define $(gold_prefix)_create-ModuleModifier
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: <ModuleModifiers>
#define $(gold_prefix)_create-ModuleModifiers
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: <PackageDecl>
#define $(gold_prefix)_create-PackageDecl
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: <QualifiedName>
#define $(gold_prefix)_create-QualifiedName
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: <QualifiedNameWithWildcard>
#define $(gold_prefix)_create-QualifiedNameWithWildcard
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: <SourceBody>
#define $(gold_prefix)_create-SourceBody
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: <SourceDecl>
#define $(gold_prefix)_create-SourceDecl
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: <SourceStatement>
#define $(gold_prefix)_create-SourceStatement
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: <SourceStatements>
#define $(gold_prefix)_create-SourceStatements
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: <SuperModules>
#define $(gold_prefix)_create-SuperModules
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: <SuperModulesList>
#define $(gold_prefix)_create-SuperModulesList
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef


#
# For each regular terminal we also define a constant with a human-readable
# description. Due to current limitations of template generator a definition
# is generated for all existing symbols, although variables for nonterminals
# are never actually used.
#

$(gold_prefix)_name_of-Comma         := ','
$(gold_prefix)_name_of-Dot           := '.'
$(gold_prefix)_name_of-DotTimes      := '.*'
$(gold_prefix)_name_of-Semi          := ';'
$(gold_prefix)_name_of-LBrace        := '{'
$(gold_prefix)_name_of-RBrace        := '}'
$(gold_prefix)_name_of-abstract      := abstract
$(gold_prefix)_name_of-ccfags        := ccfags
$(gold_prefix)_name_of-depends       := depends
$(gold_prefix)_name_of-extends       := extends
$(gold_prefix)_name_of-file          := file
$(gold_prefix)_name_of-Identifier    := Identifier
$(gold_prefix)_name_of-import        := import
$(gold_prefix)_name_of-module        := module
$(gold_prefix)_name_of-package       := package
$(gold_prefix)_name_of-source        := source
$(gold_prefix)_name_of-static        := static
$(gold_prefix)_name_of-StringLiteral := String literal


#
# Rules.
#
# As for symbols each rule can have a handler that is used to create an
# application-specific representation if the rule data.
# The signature of handlers is the following:
#
# Params:
#  ... Each argument contains a value of the corresponding symbol
#      in the rule's RHS.
#
# Return:
#   Converted value that is passed to a symbol handler corresponding to
#   the rule's LHS (if any has been defined).
#

# Rule: <Model> ::= <PackageDecl> <ImportDecls> <ModuleDecls>
#define $(gold_prefix)_produce-Model
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <PackageDecl> ::= package <QualifiedName> ';'
#define $(gold_prefix)_produce-PackageDecl_package_Semi
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <PackageDecl> ::=
#define $(gold_prefix)_produce-PackageDecl
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <ImportDecls> ::= <ImportDecl> <ImportDecls>
#define $(gold_prefix)_produce-ImportDecls
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <ImportDecls> ::=
#define $(gold_prefix)_produce-ImportDecls2
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <ImportDecl> ::= import <QualifiedNameWithWildcard> ';'
#define $(gold_prefix)_produce-ImportDecl_import_Semi
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <ModuleDecls> ::= <ModuleDecl> <ModuleDecls>
#define $(gold_prefix)_produce-ModuleDecls
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <ModuleDecls> ::=
#define $(gold_prefix)_produce-ModuleDecls2
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <ModuleDecl> ::= <ModuleModifiers> module Identifier <SuperModules> '{' <ModuleBodyDecls> '}'
#define $(gold_prefix)_produce-ModuleDecl_module_Identifier_LBrace_RBrace
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <ModuleModifiers> ::= <ModuleModifier> <ModuleModifiers>
#define $(gold_prefix)_produce-ModuleModifiers
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <ModuleModifiers> ::=
#define $(gold_prefix)_produce-ModuleModifiers2
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <ModuleModifier> ::= static
#define $(gold_prefix)_produce-ModuleModifier_static
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <ModuleModifier> ::= abstract
#define $(gold_prefix)_produce-ModuleModifier_abstract
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <SuperModules> ::= extends <SuperModulesList>
#define $(gold_prefix)_produce-SuperModules_extends
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <SuperModules> ::=
#define $(gold_prefix)_produce-SuperModules
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <SuperModulesList> ::= <QualifiedName> ',' <SuperModulesList>
#define $(gold_prefix)_produce-SuperModulesList_Comma
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <SuperModulesList> ::= <QualifiedName>
#define $(gold_prefix)_produce-SuperModulesList
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <ModuleBodyDecls> ::= <ModuleBodyDecl> <ModuleBodyDecls>
#define $(gold_prefix)_produce-ModuleBodyDecls
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <ModuleBodyDecls> ::=
#define $(gold_prefix)_produce-ModuleBodyDecls2
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <ModuleBodyDecl> ::= <SourceDecl>
#define $(gold_prefix)_produce-ModuleBodyDecl
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <ModuleBodyDecl> ::= <CcflagsDecl>
#define $(gold_prefix)_produce-ModuleBodyDecl2
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <ModuleBodyDecl> ::= <DependencyDecl>
#define $(gold_prefix)_produce-ModuleBodyDecl3
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <SourceDecl> ::= source <SourceBody>
#define $(gold_prefix)_produce-SourceDecl_source
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <SourceBody> ::= '{' <SourceStatements> '}'
#define $(gold_prefix)_produce-SourceBody_LBrace_RBrace
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <SourceBody> ::= <SourceStatement>
#define $(gold_prefix)_produce-SourceBody
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <SourceStatements> ::= <SourceStatement> <SourceStatements>
#define $(gold_prefix)_produce-SourceStatements
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <SourceStatements> ::=
#define $(gold_prefix)_produce-SourceStatements2
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <SourceStatement> ::= file StringLiteral ';'
#define $(gold_prefix)_produce-SourceStatement_file_StringLiteral_Semi
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <CcflagsDecl> ::= ccfags StringLiteral ';'
#define $(gold_prefix)_produce-CcflagsDecl_ccfags_StringLiteral_Semi
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <DependencyDecl> ::= depends <QualifiedName> ';'
#define $(gold_prefix)_produce-DependencyDecl_depends_Semi
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <QualifiedName> ::= Identifier '.' <QualifiedName>
#define $(gold_prefix)_produce-QualifiedName_Identifier_Dot
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <QualifiedName> ::= Identifier
#define $(gold_prefix)_produce-QualifiedName_Identifier
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <QualifiedNameWithWildcard> ::= <QualifiedName> '.*'
#define $(gold_prefix)_produce-QualifiedNameWithWildcard_DotTimes
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <QualifiedNameWithWildcard> ::= <QualifiedName>
#define $(gold_prefix)_produce-QualifiedNameWithWildcard
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef


$(def_all)

gold_prefix :=# Undefine.

endif # __gold_grammar_$(gold_prefix)_mk

