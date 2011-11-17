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
gold_grammar := $(basename $(notdir $(lastword $(MAKEFILE_LIST))))

ifeq ($(call singleword,$(gold_grammar)),)
$(error 'gold_grammar' is empty or not a single word, \
	you have to define it properly in order to continue)
endif

gold_grammar := $(call trim,$(gold_grammar))

ifndef __gold_grammar_$(gold_grammar)_mk
__gold_grammar_$(gold_grammar)_mk := 1

# By default it is assumed that you'll place tables file in the same directory
# with this one and name it with '-tables' prefix.

# Fix the following line to override default behavior.
include $(dir $(lastword $(MAKEFILE_LIST)))$(gold_grammar)-tables.mk

#
# Symbols.
#
# Each symbol is converted by the corresponding constructor (if any has been
# defined). Constructor is a function with the following signature:
#
# Params:
#   1. For terminals: a list of decimal char codes representing the token.
#      For nonterminals: the result of production.
#   2. Start location in form 'line:column'.
#
# Return:
#   Converted value. The value is then passed to a rule containing that symbol
#   in its RHS or returned to user in case of the Start Symbol.
#
# If constructor for some symbol is not defined then the default behavior
# is used:
#   For terminals:
#     Decodes an input by replacing all printable characters with their values
#     and the rest ones with spaces.
#   For nonterminals:
#     Outputs the result of production as is, without modifying it.
#
# Constructor may also use a special 'gold_default_create' function to get
# the default value.
#

$(gold_grammar)_create-Comma     := ,
$(gold_grammar)_create-Dot       := .
$(gold_grammar)_create-DotTimes  := .*
$(gold_grammar)_create-Colon     := :
$(gold_grammar)_create-Semi      := ;
$(gold_grammar)_create-LBrace    := {
$(gold_grammar)_create-RBrace    := }
$(gold_grammar)_create-abstract  := abstract
$(gold_grammar)_create-ccfags    := ccfags
$(gold_grammar)_create-depends   := depends
$(gold_grammar)_create-extends   := extends
$(gold_grammar)_create-file      := file
$(gold_grammar)_create-import    := import
$(gold_grammar)_create-make      := make
$(gold_grammar)_create-module    := module
$(gold_grammar)_create-package   := package
$(gold_grammar)_create-source    := source
$(gold_grammar)_create-static    := static
$(gold_grammar)_create-target    := target

# Symbol: Identifier
#define $(gold_grammar)_create-Identifier
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: StringLiteral
define $(gold_grammar)_create-StringLiteral
	$(subst \\ ,\,# Unescape '\' sequences.
		$(subst \',',
			$(if $(eq ",$(call gold_default_create,$(firstword $1))),
				# Regular "..." string.
				$(subst \",",$(subst \t,$(\t),$(subst \n,$(\n),$(subst \r,,
					$(subst \\,\\ ,
						$(call gold_default_create,
							# Remove '"' at the ends.
							$(wordlist 3,$(words $1),x $1)
						)
					)
				)))),
				# Verbatim '''...''' string.
				$(subst \\,\\ ,
					$(call gold_default_create,
						# Remove ''' at the ends.
						$(wordlist 7,$(words $1),x x x $1)
					)
				)
			)
		)
	)
endef

# Symbol: <CcflagsDecl>
#define $(gold_grammar)_create-CcflagsDecl
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: <DependencyDecl>
#define $(gold_grammar)_create-DependencyDecl
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: <ImportDecl>
#define $(gold_grammar)_create-ImportDecl
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: <ImportDecls>
#define $(gold_grammar)_create-ImportDecls
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: <MakeRuleDecl>
#define $(gold_grammar)_create-MakeRuleDecl
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: <MakeRulePrerequisitiesList>
#define $(gold_grammar)_create-MakeRulePrerequisitiesList
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: <MakeRulePrerequisity>
#define $(gold_grammar)_create-MakeRulePrerequisity
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: <MakeRuleRecipe>
#define $(gold_grammar)_create-MakeRuleRecipe
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: <Model>
#define $(gold_grammar)_create-Model
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: <ModuleBodyDecl>
#define $(gold_grammar)_create-ModuleBodyDecl
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: <ModuleBodyDecls>
#define $(gold_grammar)_create-ModuleBodyDecls
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: <ModuleDecl>
#define $(gold_grammar)_create-ModuleDecl
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: <ModuleDecls>
#define $(gold_grammar)_create-ModuleDecls
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: <ModuleModifier>
#define $(gold_grammar)_create-ModuleModifier
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: <ModuleModifiers>
#define $(gold_grammar)_create-ModuleModifiers
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: <PackageDecl>
#define $(gold_grammar)_create-PackageDecl
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: <QualifiedName>
#define $(gold_grammar)_create-QualifiedName
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: <QualifiedNameWithWildcard>
#define $(gold_grammar)_create-QualifiedNameWithWildcard
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: <SourceBody>
#define $(gold_grammar)_create-SourceBody
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: <SourceDecl>
#define $(gold_grammar)_create-SourceDecl
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: <SourceStatement>
#define $(gold_grammar)_create-SourceStatement
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: <SourceStatements>
#define $(gold_grammar)_create-SourceStatements
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: <SuperModules>
#define $(gold_grammar)_create-SuperModules
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: <SuperModulesList>
#define $(gold_grammar)_create-SuperModulesList
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef


#
# For each regular terminal we also define a constant with a human-readable
# description.
#

$(gold_grammar)_name_of-Comma         := ','
$(gold_grammar)_name_of-Dot           := '.'
$(gold_grammar)_name_of-DotTimes      := '.*'
$(gold_grammar)_name_of-Colon         := ':'
$(gold_grammar)_name_of-Semi          := ';'
$(gold_grammar)_name_of-LBrace        := '{'
$(gold_grammar)_name_of-RBrace        := '}'
$(gold_grammar)_name_of-abstract      := abstract
$(gold_grammar)_name_of-ccfags        := ccfags
$(gold_grammar)_name_of-depends       := depends
$(gold_grammar)_name_of-extends       := extends
$(gold_grammar)_name_of-file          := file
$(gold_grammar)_name_of-Identifier    := Identifier
$(gold_grammar)_name_of-import        := import
$(gold_grammar)_name_of-make          := make
$(gold_grammar)_name_of-module        := module
$(gold_grammar)_name_of-package       := package
$(gold_grammar)_name_of-source        := source
$(gold_grammar)_name_of-static        := static
$(gold_grammar)_name_of-StringLiteral := String literal
$(gold_grammar)_name_of-target        := target


#
# Rules.
#
# As for symbols each rule can have a constructor that is used to produce an
# application-specific representation of the rule data.
# The signature of production function is the following:
#
# Params:
#  1..N: Each argument contains a value of the corresponding symbol
#        in the rule's RHS.
#  N+1:  Location vector with 'line:column' words each of which is a start of
#        the corresponding symbol.
#
# Return:
#   Converted value that is passed to a symbol handler corresponding to
#   the rule's LHS (if any has been defined).
#
# If production function is not defined then the rule is produced by
# concatenating the RHS through a space. To reuse this default value one can
# call 'gold_default_produce' function.
#

# Rule: <Model> ::= <PackageDecl> <ImportDecls> <ModuleDecls>
# Args: 1..3 - Symbols; 3+1 - Location vector.
define $(gold_grammar)_produce-Model
	$(\h) Generated from $(gold_file). Do not edit!$(\n)$(\n)
	$(if $1,$1$(\n))
	$(if $2,$2$(\n))
	$(if $3,
		$(if $(findstring $$_APIS,$3),$$_APIS :=$(\n))
		$(if $(findstring $$_MODS,$3),$$_MODS :=$(\n))
		$(\n)$3$(\n)
	)
endef

# Rule: <PackageDecl> ::= package <QualifiedName> ';'
# Args: 1..3 - Symbols; 3+1 - Location vector.
define $(gold_grammar)_produce-PackageDecl_package_Semi
	$$_PACKAGE := $2$(\n)
endef

# Rule: <PackageDecl> ::=
# Args: 1..0 - Symbols; 0+1 - Location vector.
define $(gold_grammar)_produce-PackageDecl
	$(call gold_report,$3,
		Warning: using default package
	)
endef

# Rule: <ImportDecls> ::= <ImportDecl> <ImportDecls>
# Args: 1..2 - Symbols; 2+1 - Location vector.
define $(gold_grammar)_produce-ImportDecls
	$(call gold_report,$3,
		Imports are not yet implemented!
	)
endef

# Rule: <ImportDecls> ::=
# Args: 1..0 - Symbols; 0+1 - Location vector.
#define $(gold_grammar)_produce-ImportDecls2
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <ImportDecl> ::= import <QualifiedNameWithWildcard> ';'
# Args: 1..3 - Symbols; 3+1 - Location vector.
#define $(gold_grammar)_produce-ImportDecl_import_Semi
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <ModuleDecls> ::= <ModuleDecl> <ModuleDecls>
# Args: 1..2 - Symbols; 2+1 - Location vector.
define $(gold_grammar)_produce-ModuleDecls
	$1$2
endef

# Rule: <ModuleDecls> ::=
# Args: 1..0 - Symbols; 0+1 - Location vector.
#define $(gold_grammar)_produce-ModuleDecls2
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <ModuleDecl> ::= <ModuleModifiers> module Identifier <SuperModules> '{' <ModuleBodyDecls> '}'
# Args: 1..7 - Symbols; 7+1 - Location vector.
define $(gold_grammar)_produce-ModuleDecl_module_Identifier_LBrace_RBrace
	m := $3$(\n)
	$$_$(if $(filter abstract,$1),APIS,MODS) += $$m$(\n)
	$4
	$6$(\n)
endef

# Rule: <ModuleModifiers> ::= <ModuleModifier> <ModuleModifiers>
# Args: 1..2 - Symbols; 2+1 - Location vector.
define $(gold_grammar)_produce-ModuleModifiers
	$(if $(not $(eq $(words $1 $2),$(words $(sort $1 $2)))),
		$(call gold_report,$3,
			Error: Repeated occurrence of '$1' modifier
		)
	)
	$(gold_default_produce)
endef

# Rule: <ModuleModifiers> ::=
# Args: 1..0 - Symbols; 0+1 - Location vector.
#define $(gold_grammar)_produce-ModuleModifiers2
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <ModuleModifier> ::= static
# Args: 1..1 - Symbols; 1+1 - Location vector.
#define $(gold_grammar)_produce-ModuleModifier_static
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <ModuleModifier> ::= abstract
# Args: 1..1 - Symbols; 1+1 - Location vector.
#define $(gold_grammar)_produce-ModuleModifier_abstract
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <SuperModules> ::= extends <SuperModulesList>
# Args: 1..2 - Symbols; 2+1 - Location vector.
define $(gold_grammar)_produce-SuperModules_extends
	$$_PROVIDES-$$m += $2$(\n)
endef

# Rule: <SuperModules> ::=
# Args: 1..0 - Symbols; 0+1 - Location vector.
#define $(gold_grammar)_produce-SuperModules
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <SuperModulesList> ::= <QualifiedName> ',' <SuperModulesList>
# Args: 1..3 - Symbols; 3+1 - Location vector.
define $(gold_grammar)_produce-SuperModulesList_Comma
	$1 $3
endef

# Rule: <SuperModulesList> ::= <QualifiedName>
# Args: 1..1 - Symbols; 1+1 - Location vector.
#define $(gold_grammar)_produce-SuperModulesList
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <ModuleBodyDecls> ::= <ModuleBodyDecl> <ModuleBodyDecls>
# Args: 1..2 - Symbols; 2+1 - Location vector.
define $(gold_grammar)_produce-ModuleBodyDecls
	$1$2
endef

# Rule: <ModuleBodyDecls> ::=
# Args: 1..0 - Symbols; 0+1 - Location vector.
#define $(gold_grammar)_produce-ModuleBodyDecls2
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <ModuleBodyDecl> ::= <SourceDecl>
# Args: 1..1 - Symbols; 1+1 - Location vector.
#define $(gold_grammar)_produce-ModuleBodyDecl
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <ModuleBodyDecl> ::= <CcflagsDecl>
# Args: 1..1 - Symbols; 1+1 - Location vector.
#define $(gold_grammar)_produce-ModuleBodyDecl2
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <ModuleBodyDecl> ::= <DependencyDecl>
# Args: 1..1 - Symbols; 1+1 - Location vector.
#define $(gold_grammar)_produce-ModuleBodyDecl3
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <ModuleBodyDecl> ::= <MakeRuleDecl>
# Args: 1..1 - Symbols; 1+1 - Location vector.
#define $(gold_grammar)_produce-ModuleBodyDecl4
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <SourceDecl> ::= source <SourceBody>
# Args: 1..2 - Symbols; 2+1 - Location vector.
define $(gold_grammar)_produce-SourceDecl_source
	$2
endef

# Rule: <SourceBody> ::= '{' <SourceStatements> '}'
# Args: 1..3 - Symbols; 3+1 - Location vector.
define $(gold_grammar)_produce-SourceBody_LBrace_RBrace
	$2
endef

# Rule: <SourceBody> ::= <SourceStatement>
# Args: 1..1 - Symbols; 1+1 - Location vector.
#define $(gold_grammar)_produce-SourceBody
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <SourceStatements> ::= <SourceStatement> <SourceStatements>
# Args: 1..2 - Symbols; 2+1 - Location vector.
#define $(gold_grammar)_produce-SourceStatements
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <SourceStatements> ::=
# Args: 1..0 - Symbols; 0+1 - Location vector.
#define $(gold_grammar)_produce-SourceStatements2
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <SourceStatement> ::= file StringLiteral ';'
# Args: 1..3 - Symbols; 3+1 - Location vector.
define $(gold_grammar)_produce-SourceStatement_file_StringLiteral_Semi
	$$_SRCS-$$m += $2$(\n)
endef

# Rule: <CcflagsDecl> ::= ccfags StringLiteral ';'
# Args: 1..3 - Symbols; 3+1 - Location vector.
define $(gold_grammar)_produce-CcflagsDecl_ccfags_StringLiteral_Semi
	$$_CFLAGS-$$m += $2$(\n)
endef

# Rule: <DependencyDecl> ::= depends <QualifiedName> ';'
# Args: 1..3 - Symbols; 3+1 - Location vector.
define $(gold_grammar)_produce-DependencyDecl_depends_Semi
	$$_DEPS-$$m += $2$(\n)
endef

# Rule: <MakeRuleDecl> ::= target StringLiteral <MakeRulePrerequisity> <MakeRuleRecipe> ';'
# Args: 1..5 - Symbols; 5+1 - Location vector.
#define $(gold_grammar)_produce-MakeRuleDecl_target_StringLiteral_Semi
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <MakeRulePrerequisity> ::= ':' <MakeRulePrerequisitiesList>
# Args: 1..2 - Symbols; 2+1 - Location vector.
#define $(gold_grammar)_produce-MakeRulePrerequisity_Colon
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <MakeRulePrerequisity> ::=
# Args: 1..0 - Symbols; 0+1 - Location vector.
#define $(gold_grammar)_produce-MakeRulePrerequisity
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <MakeRulePrerequisitiesList> ::= StringLiteral ',' <MakeRulePrerequisitiesList>
# Args: 1..3 - Symbols; 3+1 - Location vector.
#define $(gold_grammar)_produce-MakeRulePrerequisitiesList_StringLiteral_Comma
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <MakeRulePrerequisitiesList> ::=
# Args: 1..0 - Symbols; 0+1 - Location vector.
#define $(gold_grammar)_produce-MakeRulePrerequisitiesList
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <MakeRuleRecipe> ::= make StringLiteral
# Args: 1..2 - Symbols; 2+1 - Location vector.
#define $(gold_grammar)_produce-MakeRuleRecipe_make_StringLiteral
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <MakeRuleRecipe> ::=
# Args: 1..0 - Symbols; 0+1 - Location vector.
#define $(gold_grammar)_produce-MakeRuleRecipe
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <QualifiedName> ::= Identifier '.' <QualifiedName>
# Args: 1..3 - Symbols; 3+1 - Location vector.
define $(gold_grammar)_produce-QualifiedName_Identifier_Dot
	$1.$3
endef

# Rule: <QualifiedName> ::= Identifier
# Args: 1..1 - Symbols; 1+1 - Location vector.
#define $(gold_grammar)_produce-QualifiedName_Identifier
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <QualifiedNameWithWildcard> ::= <QualifiedName> '.*'
# Args: 1..2 - Symbols; 2+1 - Location vector.
define $(gold_grammar)_produce-QualifiedNameWithWildcard_DotTimes
	$1.*
endef

# Rule: <QualifiedNameWithWildcard> ::= <QualifiedName>
# Args: 1..1 - Symbols; 1+1 - Location vector.
#define $(gold_grammar)_produce-QualifiedNameWithWildcard
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

$(def_all)

gold_grammar :=# Undefine.

endif # __gold_grammar_$(gold_grammar)_mk

