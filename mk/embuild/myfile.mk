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
#   1. For terminals it is a list of decimal char codes representing the token.
#      For nonterminals - the result of applying one of rule handlers (it may
#      be used to extract common symbol value postprocessing from its rules).
#
# Return:
#   Converted value. The value is then passed to a rule containing that symbol
#   in its RHS or returned to user in case of the Start Symbol.
#

# Symbol: (EOF)
define $(gold_prefix)_Symbol_EOF
	# TODO Auto-generated token handler stub! Returns empty by default.
endef

# Symbol: (Error)
define $(gold_prefix)_Symbol_Error
	# TODO Auto-generated token handler stub! Returns empty by default.
endef

# Symbol: (Whitespace)
define $(gold_prefix)_Symbol_Whitespace
	# TODO Auto-generated token handler stub! Returns empty by default.
endef

# Symbol: ','
define $(gold_prefix)_Symbol_Comma
	# TODO Auto-generated token handler stub! Returns empty by default.
endef

# Symbol: '.'
define $(gold_prefix)_Symbol_Dot
	# TODO Auto-generated token handler stub! Returns empty by default.
endef

# Symbol: '.*'
define $(gold_prefix)_Symbol_DotTimes
	# TODO Auto-generated token handler stub! Returns empty by default.
endef

# Symbol: ';'
define $(gold_prefix)_Symbol_Semi
	# TODO Auto-generated token handler stub! Returns empty by default.
endef

# Symbol: '{'
define $(gold_prefix)_Symbol_LBrace
	# TODO Auto-generated token handler stub! Returns empty by default.
endef

# Symbol: '}'
define $(gold_prefix)_Symbol_RBrace
	# TODO Auto-generated token handler stub! Returns empty by default.
endef

# Symbol: abstract
define $(gold_prefix)_Symbol_abstract
	# TODO Auto-generated token handler stub! Returns empty by default.
endef

# Symbol: depends
define $(gold_prefix)_Symbol_depends
	# TODO Auto-generated token handler stub! Returns empty by default.
endef

# Symbol: extends
define $(gold_prefix)_Symbol_extends
	# TODO Auto-generated token handler stub! Returns empty by default.
endef

# Symbol: file
define $(gold_prefix)_Symbol_file
	# TODO Auto-generated token handler stub! Returns empty by default.
endef

# Symbol: Identifier
define $(gold_prefix)_Symbol_Identifier
	# TODO Auto-generated token handler stub! Returns empty by default.
endef

# Symbol: import
define $(gold_prefix)_Symbol_import
	# TODO Auto-generated token handler stub! Returns empty by default.
endef

# Symbol: module
define $(gold_prefix)_Symbol_module
	# TODO Auto-generated token handler stub! Returns empty by default.
endef

# Symbol: package
define $(gold_prefix)_Symbol_package
	# TODO Auto-generated token handler stub! Returns empty by default.
endef

# Symbol: source
define $(gold_prefix)_Symbol_source
	# TODO Auto-generated token handler stub! Returns empty by default.
endef

# Symbol: static
define $(gold_prefix)_Symbol_static
	# TODO Auto-generated token handler stub! Returns empty by default.
endef

# Symbol: StringLiteral
define $(gold_prefix)_Symbol_StringLiteral
	# TODO Auto-generated token handler stub! Returns empty by default.
endef

# Symbol: <DependencyDecl>
define $(gold_prefix)_Symbol_DependencyDecl
	# TODO Auto-generated token handler stub! Returns empty by default.
endef

# Symbol: <ImportDecl>
define $(gold_prefix)_Symbol_ImportDecl
	# TODO Auto-generated token handler stub! Returns empty by default.
endef

# Symbol: <ImportDecls>
define $(gold_prefix)_Symbol_ImportDecls
	# TODO Auto-generated token handler stub! Returns empty by default.
endef

# Symbol: <Model>
define $(gold_prefix)_Symbol_Model
	# TODO Auto-generated token handler stub! Returns empty by default.
endef

# Symbol: <ModuleBodyDecl>
define $(gold_prefix)_Symbol_ModuleBodyDecl
	# TODO Auto-generated token handler stub! Returns empty by default.
endef

# Symbol: <ModuleBodyDecls>
define $(gold_prefix)_Symbol_ModuleBodyDecls
	# TODO Auto-generated token handler stub! Returns empty by default.
endef

# Symbol: <ModuleDecl>
define $(gold_prefix)_Symbol_ModuleDecl
	# TODO Auto-generated token handler stub! Returns empty by default.
endef

# Symbol: <ModuleDecls>
define $(gold_prefix)_Symbol_ModuleDecls
	# TODO Auto-generated token handler stub! Returns empty by default.
endef

# Symbol: <ModuleModifier>
define $(gold_prefix)_Symbol_ModuleModifier
	# TODO Auto-generated token handler stub! Returns empty by default.
endef

# Symbol: <ModuleModifiers>
define $(gold_prefix)_Symbol_ModuleModifiers
	# TODO Auto-generated token handler stub! Returns empty by default.
endef

# Symbol: <PackageDecl>
define $(gold_prefix)_Symbol_PackageDecl
	# TODO Auto-generated token handler stub! Returns empty by default.
endef

# Symbol: <QualifiedName>
define $(gold_prefix)_Symbol_QualifiedName
	# TODO Auto-generated token handler stub! Returns empty by default.
endef

# Symbol: <QualifiedNameWithWildcard>
define $(gold_prefix)_Symbol_QualifiedNameWithWildcard
	# TODO Auto-generated token handler stub! Returns empty by default.
endef

# Symbol: <SourceBody>
define $(gold_prefix)_Symbol_SourceBody
	# TODO Auto-generated token handler stub! Returns empty by default.
endef

# Symbol: <SourceDecl>
define $(gold_prefix)_Symbol_SourceDecl
	# TODO Auto-generated token handler stub! Returns empty by default.
endef

# Symbol: <SourceStatement>
define $(gold_prefix)_Symbol_SourceStatement
	# TODO Auto-generated token handler stub! Returns empty by default.
endef

# Symbol: <SourceStatements>
define $(gold_prefix)_Symbol_SourceStatements
	# TODO Auto-generated token handler stub! Returns empty by default.
endef

# Symbol: <SuperModules>
define $(gold_prefix)_Symbol_SuperModules
	# TODO Auto-generated token handler stub! Returns empty by default.
endef

# Symbol: <SuperModulesList>
define $(gold_prefix)_Symbol_SuperModulesList
	# TODO Auto-generated token handler stub! Returns empty by default.
endef


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

# Rule: <QualifiedName> ::= Identifier '.' <QualifiedName>
define $(gold_prefix)_Rule_QualifiedName_Identifier_Dot
	# TODO Auto-generated rule handler stub! Returns empty by default.
endef

# Rule: <QualifiedName> ::= Identifier
define $(gold_prefix)_Rule_QualifiedName_Identifier
	# TODO Auto-generated rule handler stub! Returns empty by default.
endef

# Rule: <QualifiedNameWithWildcard> ::= <QualifiedName> '.*'
define $(gold_prefix)_Rule_QualifiedNameWithWildcard_DotTimes
	# TODO Auto-generated rule handler stub! Returns empty by default.
endef

# Rule: <QualifiedNameWithWildcard> ::= <QualifiedName>
define $(gold_prefix)_Rule_QualifiedNameWithWildcard
	# TODO Auto-generated rule handler stub! Returns empty by default.
endef

# Rule: <Model> ::= <PackageDecl> <ImportDecls> <ModuleDecls>
define $(gold_prefix)_Rule_Model
	# TODO Auto-generated rule handler stub! Returns empty by default.
endef

# Rule: <PackageDecl> ::= package <QualifiedName> ';'
define $(gold_prefix)_Rule_PackageDecl_package_Semi
	# TODO Auto-generated rule handler stub! Returns empty by default.
endef

# Rule: <PackageDecl> ::=
define $(gold_prefix)_Rule_PackageDecl
	# TODO Auto-generated rule handler stub! Returns empty by default.
endef

# Rule: <ImportDecls> ::= <ImportDecl> <ImportDecls>
define $(gold_prefix)_Rule_ImportDecls
	# TODO Auto-generated rule handler stub! Returns empty by default.
endef

# Rule: <ImportDecls> ::=
define $(gold_prefix)_Rule_ImportDecls2
	# TODO Auto-generated rule handler stub! Returns empty by default.
endef

# Rule: <ImportDecl> ::= import <QualifiedNameWithWildcard> ';'
define $(gold_prefix)_Rule_ImportDecl_import_Semi
	# TODO Auto-generated rule handler stub! Returns empty by default.
endef

# Rule: <ModuleDecls> ::= <ModuleDecl> <ModuleDecls>
define $(gold_prefix)_Rule_ModuleDecls
	# TODO Auto-generated rule handler stub! Returns empty by default.
endef

# Rule: <ModuleDecls> ::=
define $(gold_prefix)_Rule_ModuleDecls2
	# TODO Auto-generated rule handler stub! Returns empty by default.
endef

# Rule: <ModuleDecl> ::= <ModuleModifiers> module Identifier <SuperModules> '{' <ModuleBodyDecls> '}'
define $(gold_prefix)_Rule_ModuleDecl_module_Identifier_LBrace_RBrace
	# TODO Auto-generated rule handler stub! Returns empty by default.
endef

# Rule: <ModuleModifiers> ::= <ModuleModifier> <ModuleModifiers>
define $(gold_prefix)_Rule_ModuleModifiers
	# TODO Auto-generated rule handler stub! Returns empty by default.
endef

# Rule: <ModuleModifiers> ::=
define $(gold_prefix)_Rule_ModuleModifiers2
	# TODO Auto-generated rule handler stub! Returns empty by default.
endef

# Rule: <ModuleModifier> ::= static
define $(gold_prefix)_Rule_ModuleModifier_static
	# TODO Auto-generated rule handler stub! Returns empty by default.
endef

# Rule: <ModuleModifier> ::= abstract
define $(gold_prefix)_Rule_ModuleModifier_abstract
	# TODO Auto-generated rule handler stub! Returns empty by default.
endef

# Rule: <SuperModules> ::= extends <SuperModulesList>
define $(gold_prefix)_Rule_SuperModules_extends
	# TODO Auto-generated rule handler stub! Returns empty by default.
endef

# Rule: <SuperModules> ::=
define $(gold_prefix)_Rule_SuperModules
	# TODO Auto-generated rule handler stub! Returns empty by default.
endef

# Rule: <SuperModulesList> ::= <QualifiedName> ',' <SuperModulesList>
define $(gold_prefix)_Rule_SuperModulesList_Comma
	# TODO Auto-generated rule handler stub! Returns empty by default.
endef

# Rule: <SuperModulesList> ::= <QualifiedName>
define $(gold_prefix)_Rule_SuperModulesList
	# TODO Auto-generated rule handler stub! Returns empty by default.
endef

# Rule: <ModuleBodyDecls> ::= <ModuleBodyDecl> <ModuleBodyDecls>
define $(gold_prefix)_Rule_ModuleBodyDecls
	# TODO Auto-generated rule handler stub! Returns empty by default.
endef

# Rule: <ModuleBodyDecls> ::=
define $(gold_prefix)_Rule_ModuleBodyDecls2
	# TODO Auto-generated rule handler stub! Returns empty by default.
endef

# Rule: <ModuleBodyDecl> ::= <SourceDecl>
define $(gold_prefix)_Rule_ModuleBodyDecl
	# TODO Auto-generated rule handler stub! Returns empty by default.
endef

# Rule: <ModuleBodyDecl> ::= <DependencyDecl>
define $(gold_prefix)_Rule_ModuleBodyDecl2
	# TODO Auto-generated rule handler stub! Returns empty by default.
endef

# Rule: <SourceDecl> ::= source <SourceBody>
define $(gold_prefix)_Rule_SourceDecl_source
	# TODO Auto-generated rule handler stub! Returns empty by default.
endef

# Rule: <SourceBody> ::= '{' <SourceStatements> '}'
define $(gold_prefix)_Rule_SourceBody_LBrace_RBrace
	# TODO Auto-generated rule handler stub! Returns empty by default.
endef

# Rule: <SourceBody> ::= <SourceStatement>
define $(gold_prefix)_Rule_SourceBody
	# TODO Auto-generated rule handler stub! Returns empty by default.
endef

# Rule: <SourceStatements> ::= <SourceStatement> <SourceStatements>
define $(gold_prefix)_Rule_SourceStatements
	# TODO Auto-generated rule handler stub! Returns empty by default.
endef

# Rule: <SourceStatements> ::=
define $(gold_prefix)_Rule_SourceStatements2
	# TODO Auto-generated rule handler stub! Returns empty by default.
endef

# Rule: <SourceStatement> ::= file StringLiteral ';'
define $(gold_prefix)_Rule_SourceStatement_file_StringLiteral_Semi
	# TODO Auto-generated rule handler stub! Returns empty by default.
endef

# Rule: <DependencyDecl> ::= depends
define $(gold_prefix)_Rule_DependencyDecl_depends
	# TODO Auto-generated rule handler stub! Returns empty by default.
endef


$(def_all)

gold_prefix :=# Undefine.

endif # __gold_grammar_$(gold_prefix)_mk

