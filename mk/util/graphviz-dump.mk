ifndef __util_serialize_mk
__util_serialize_mk := 1

include mk/core/object.mk

include mk/util/graph.mk

# Serialize objects to .dot file for converting it by graphviz.
# If $1 exist then graph from this node will be closed and objects from graph will be
# serialized. Else every object in system will be serialized
#
# [param $1] if exist root node for closed graph
#
define graphviz_dump_objects
	$(\n)digraph "Make Objects Dump"
	$(\n){
	$(\n)	graph[rankdir="LR"];
	$(\n)	node[shape="record"];
	$(\n)
	$(foreach o,
		$(if $(call value,1),
			# if root is known then closure graph
			$(call graph_closure,$1,get_referenced_objects)
			, # else get all obj% in system
			$(call get-instances)
		), # foreach
		$(\n)	"$o" \
			[label="<.> $o : $($o)\l $(foreach f,$(call graphviz_field_name,
				$($($o).fields)),
				| <$f> $f = \l$(patsubst %,%\r,$(call graphviz_escape,$($o.$f)))
			)"];
		$(\n)
		$(foreach f, $(call graphviz_obj_links,$o),
			$(foreach p,$(call graphviz_field_type,$($o.$f)),
				$(\n)	"$o":$f -> "$p":".";
			)
		)
		$(\n)
	)
	$(\n)}
	$(\n)
endef

# The function escapes following sequence of symbols:
# ",\"
# It's used during serialization to dot format
define graphviz_escape
	$(subst ",\",$(subst |,\|,$1))
endef

define graphviz_field_name
	$(subst [],,$(basename $1))
endef

define graphviz_field_type
	$(suffix $1)
endef

define graphviz_obj_links
	$(subst [],,$(subst .,,$(basename $($($1).fields:%=.%))))
endef

__mk_objects_dump_ps := objects_dump.ps

.PHONY : mk_objects_dump
.PHONY : $(__mk_objects_dump_ps:.ps=.dot) # Assume it volatile.

mk_objects_dump : $(__mk_objects_dump_ps)

$(__mk_objects_dump_ps:.ps=.dot) :
	@printf '%b' '$(call escape_printf,$(call graphviz_dump_objects))' > $@

$(__mk_objects_dump_ps) : %.ps : %.dot
	@dot -Tps $< -o $@

$(def_all)

endif #__util_serialize_mk
