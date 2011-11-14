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

$(gold_prefix)_create-Comma     := ,
$(gold_prefix)_create-Dot       := .
$(gold_prefix)_create-DotTimes  := .*
$(gold_prefix)_create-Colon     := :
$(gold_prefix)_create-Semi      := ;
$(gold_prefix)_create-LBrace    := {
$(gold_prefix)_create-RBrace    := }
$(gold_prefix)_create-abstract  := abstract
$(gold_prefix)_create-ccfags    := ccfags
$(gold_prefix)_create-depends   := depends
$(gold_prefix)_create-extends   := extends
$(gold_prefix)_create-file      := file
$(gold_prefix)_create-import    := import
$(gold_prefix)_create-make      := make
$(gold_prefix)_create-module    := module
$(gold_prefix)_create-package   := package
$(gold_prefix)_create-source    := source
$(gold_prefix)_create-static    := static
$(gold_prefix)_create-target    := target

# Symbol: Identifier
#define $(gold_prefix)_create-Identifier
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: StringLiteral
define $(gold_prefix)_create-StringLiteral
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

# Symbol: <MakeRuleDecl>
#define $(gold_prefix)_create-MakeRuleDecl
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: <MakeRulePrerequisitiesList>
#define $(gold_prefix)_create-MakeRulePrerequisitiesList
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: <MakeRulePrerequisity>
#define $(gold_prefix)_create-MakeRulePrerequisity
#	$(gold_default_create)# TODO Auto-generated stub! Uncomment to override.
#endef

# Symbol: <MakeRuleRecipe>
#define $(gold_prefix)_create-MakeRuleRecipe
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
# description.
#

$(gold_prefix)_name_of-Comma         := ','
$(gold_prefix)_name_of-Dot           := '.'
$(gold_prefix)_name_of-DotTimes      := '.*'
$(gold_prefix)_name_of-Colon         := ':'
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
$(gold_prefix)_name_of-make          := make
$(gold_prefix)_name_of-module        := module
$(gold_prefix)_name_of-package       := package
$(gold_prefix)_name_of-source        := source
$(gold_prefix)_name_of-static        := static
$(gold_prefix)_name_of-StringLiteral := String literal
$(gold_prefix)_name_of-target        := target


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
define $(gold_prefix)_produce-Model
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
define $(gold_prefix)_produce-PackageDecl_package_Semi
	$$_PACKAGE := $2$(\n)
endef

# Rule: <PackageDecl> ::=
# Args: 1..0 - Symbols; 0+1 - Location vector.
define $(gold_prefix)_produce-PackageDecl
	$(call gold_report,$3,
		Warning: using default package
	)
endef

# Rule: <ImportDecls> ::= <ImportDecl> <ImportDecls>
# Args: 1..2 - Symbols; 2+1 - Location vector.
define $(gold_prefix)_produce-ImportDecls
	$(call gold_report,$3,
		Imports are not yet implemented!
	)
endef

# Rule: <ImportDecls> ::=
# Args: 1..0 - Symbols; 0+1 - Location vector.
#define $(gold_prefix)_produce-ImportDecls2
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <ImportDecl> ::= import <QualifiedNameWithWildcard> ';'
# Args: 1..3 - Symbols; 3+1 - Location vector.
#define $(gold_prefix)_produce-ImportDecl_import_Semi
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <ModuleDecls> ::= <ModuleDecl> <ModuleDecls>
# Args: 1..2 - Symbols; 2+1 - Location vector.
#define $(gold_prefix)_produce-ModuleDecls
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <ModuleDecls> ::=
# Args: 1..0 - Symbols; 0+1 - Location vector.
#define $(gold_prefix)_produce-ModuleDecls2
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <ModuleDecl> ::= <ModuleModifiers> module Identifier <SuperModules> '{' <ModuleBodyDecls> '}'
# Args: 1..7 - Symbols; 7+1 - Location vector.
define $(gold_prefix)_produce-ModuleDecl_module_Identifier_LBrace_RBrace
	m := $3$(\n)
	$$_$(if $(filter abstract,$1),APIS,MODS) += $$m$(\n)
	$4
	$6
endef

# Rule: <ModuleModifiers> ::= <ModuleModifier> <ModuleModifiers>
# Args: 1..2 - Symbols; 2+1 - Location vector.
define $(gold_prefix)_produce-ModuleModifiers
	$(if $(not $(eq $(words $1 $2),$(words $(sort $1 $2)))),
		$(call gold_report,$3,
			Error: Repeated occurrence of '$1' modifier
		)
	)
	$(gold_default_produce)
endef

# Rule: <ModuleModifiers> ::=
# Args: 1..0 - Symbols; 0+1 - Location vector.
#define $(gold_prefix)_produce-ModuleModifiers2
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <ModuleModifier> ::= static
# Args: 1..1 - Symbols; 1+1 - Location vector.
#define $(gold_prefix)_produce-ModuleModifier_static
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <ModuleModifier> ::= abstract
# Args: 1..1 - Symbols; 1+1 - Location vector.
#define $(gold_prefix)_produce-ModuleModifier_abstract
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <SuperModules> ::= extends <SuperModulesList>
# Args: 1..2 - Symbols; 2+1 - Location vector.
define $(gold_prefix)_produce-SuperModules_extends
	$$_PROVIDES-$$m += $2$(\n)
endef

# Rule: <SuperModules> ::=
# Args: 1..0 - Symbols; 0+1 - Location vector.
#define $(gold_prefix)_produce-SuperModules
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <SuperModulesList> ::= <QualifiedName> ',' <SuperModulesList>
# Args: 1..3 - Symbols; 3+1 - Location vector.
define $(gold_prefix)_produce-SuperModulesList_Comma
	$1 $3
endef

# Rule: <SuperModulesList> ::= <QualifiedName>
# Args: 1..1 - Symbols; 1+1 - Location vector.
#define $(gold_prefix)_produce-SuperModulesList
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <ModuleBodyDecls> ::= <ModuleBodyDecl> <ModuleBodyDecls>
# Args: 1..2 - Symbols; 2+1 - Location vector.
#define $(gold_prefix)_produce-ModuleBodyDecls
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <ModuleBodyDecls> ::=
# Args: 1..0 - Symbols; 0+1 - Location vector.
#define $(gold_prefix)_produce-ModuleBodyDecls2
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <ModuleBodyDecl> ::= <SourceDecl>
# Args: 1..1 - Symbols; 1+1 - Location vector.
#define $(gold_prefix)_produce-ModuleBodyDecl
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <ModuleBodyDecl> ::= <CcflagsDecl>
# Args: 1..1 - Symbols; 1+1 - Location vector.
#define $(gold_prefix)_produce-ModuleBodyDecl2
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <ModuleBodyDecl> ::= <DependencyDecl>
# Args: 1..1 - Symbols; 1+1 - Location vector.
#define $(gold_prefix)_produce-ModuleBodyDecl3
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <ModuleBodyDecl> ::= <MakeRuleDecl>
# Args: 1..1 - Symbols; 1+1 - Location vector.
#define $(gold_prefix)_produce-ModuleBodyDecl4
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <SourceDecl> ::= source <SourceBody>
# Args: 1..2 - Symbols; 2+1 - Location vector.
define $(gold_prefix)_produce-SourceDecl_source
	$2
endef

# Rule: <SourceBody> ::= '{' <SourceStatements> '}'
# Args: 1..3 - Symbols; 3+1 - Location vector.
define $(gold_prefix)_produce-SourceBody_LBrace_RBrace
	$2
endef

# Rule: <SourceBody> ::= <SourceStatement>
# Args: 1..1 - Symbols; 1+1 - Location vector.
#define $(gold_prefix)_produce-SourceBody
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <SourceStatements> ::= <SourceStatement> <SourceStatements>
# Args: 1..2 - Symbols; 2+1 - Location vector.
#define $(gold_prefix)_produce-SourceStatements
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <SourceStatements> ::=
# Args: 1..0 - Symbols; 0+1 - Location vector.
#define $(gold_prefix)_produce-SourceStatements2
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <SourceStatement> ::= file StringLiteral ';'
# Args: 1..3 - Symbols; 3+1 - Location vector.
define $(gold_prefix)_produce-SourceStatement_file_StringLiteral_Semi
	$$_SRCS-$$m += $2$(\n)
endef

# Rule: <CcflagsDecl> ::= ccfags StringLiteral ';'
# Args: 1..3 - Symbols; 3+1 - Location vector.
define $(gold_prefix)_produce-CcflagsDecl_ccfags_StringLiteral_Semi
	$$_CFLAGS-$$m += $2$(\n)
endef

# Rule: <DependencyDecl> ::= depends <QualifiedName> ';'
# Args: 1..3 - Symbols; 3+1 - Location vector.
define $(gold_prefix)_produce-DependencyDecl_depends_Semi
	$$_DEPS-$$m += $2$(\n)
endef

# Rule: <MakeRuleDecl> ::= target StringLiteral <MakeRulePrerequisity> <MakeRuleRecipe> ';'
# Args: 1..5 - Symbols; 5+1 - Location vector.
#define $(gold_prefix)_produce-MakeRuleDecl_target_StringLiteral_Semi
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <MakeRulePrerequisity> ::= ':' <MakeRulePrerequisitiesList>
# Args: 1..2 - Symbols; 2+1 - Location vector.
#define $(gold_prefix)_produce-MakeRulePrerequisity_Colon
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <MakeRulePrerequisity> ::=
# Args: 1..0 - Symbols; 0+1 - Location vector.
#define $(gold_prefix)_produce-MakeRulePrerequisity
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <MakeRulePrerequisitiesList> ::= StringLiteral ',' <MakeRulePrerequisitiesList>
# Args: 1..3 - Symbols; 3+1 - Location vector.
#define $(gold_prefix)_produce-MakeRulePrerequisitiesList_StringLiteral_Comma
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <MakeRulePrerequisitiesList> ::=
# Args: 1..0 - Symbols; 0+1 - Location vector.
#define $(gold_prefix)_produce-MakeRulePrerequisitiesList
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <MakeRuleRecipe> ::= make StringLiteral
# Args: 1..2 - Symbols; 2+1 - Location vector.
#define $(gold_prefix)_produce-MakeRuleRecipe_make_StringLiteral
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <MakeRuleRecipe> ::=
# Args: 1..0 - Symbols; 0+1 - Location vector.
#define $(gold_prefix)_produce-MakeRuleRecipe
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <QualifiedName> ::= Identifier '.' <QualifiedName>
# Args: 1..3 - Symbols; 3+1 - Location vector.
define $(gold_prefix)_produce-QualifiedName_Identifier_Dot
	$1.$3
endef

# Rule: <QualifiedName> ::= Identifier
# Args: 1..1 - Symbols; 1+1 - Location vector.
#define $(gold_prefix)_produce-QualifiedName_Identifier
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

# Rule: <QualifiedNameWithWildcard> ::= <QualifiedName> '.*'
# Args: 1..2 - Symbols; 2+1 - Location vector.
define $(gold_prefix)_produce-QualifiedNameWithWildcard_DotTimes
	$1.*
endef

# Rule: <QualifiedNameWithWildcard> ::= <QualifiedName>
# Args: 1..1 - Symbols; 1+1 - Location vector.
#define $(gold_prefix)_produce-QualifiedNameWithWildcard
#	$(gold_default_produce)# TODO Auto-generated stub! Uncomment to override.
#endef

$(def_all)

gold_prefix :=# Undefine.

endif # __gold_grammar_$(gold_prefix)_mk

