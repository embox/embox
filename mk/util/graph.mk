ifndef __util_graph_mk
__util_graph_mk := 1


#param 1 node
define get_referenced_objects
	$(foreach f,$(subst [],,$(subst .,,$(basename $($($1).fields:%=.%)))),
		$(foreach p,$(suffix $($1.$f)),$p)
	)
endef

#param $1 current object
#param $2 current marked object list
define graph_closure
	$(foreach f,$(or $(singleword $2),$(error invalid argument in $0: '$2')),
		$(sort $(with $1,,
				$(if $(filter $1,$2),$2,
					$(foreach o,$(call $f,$1),
						$(call $0,$o,$2 $1)
					)
				)
			)
		)
	)
endef

$(def_all)

endif #__util_graph_mk
