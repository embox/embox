ifndef __util_serialize_mk
__util_serialize_mk := 1

include mk/util/graph.mk
include mk/util/escape.mk
# include mk/core/object.mk

# Serialize objects to .dot file for converting it by graphviz.
# If $1 exist then graph from this node will be closed and objects from graph will be
# serialized. Else every object in system will be serialized
#
# [param $1] if exist root node for closed graph
#
define objects_to_dot
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
			$(__object_instance_cnt:%=.obj%)
		), # foreach
		$(\n)	"$o" \
			[label="<.> $o : $($o)\l $(foreach f,$(call field_name,$($($o).fields)),
				| <$f> $f = $(call escape_graphviz,$($o.$f))\l
			)"];
		$(\n)
		$(foreach f, $(call obj_links,$o),
			$(foreach p,$(call field_type,$($o.$f)),
				$(\n)	"$o":$f -> "$p":".";
			)
		)
		$(\n)
	)
	$(\n)}
	$(\n)
endef

# Serialize all objects in closed graph to makefile
# param $1 is a root node of a graph
define objects_to_mk
	$(foreach o,$(call graph_closure,$1,get_referenced_objects),
		$(foreach f,$(call field_name,$($($o).fields)),
			$o:=$(call escape_makefile,$($o))
			$(\n)
			$o.$f:=$(call escape_makefile,$($o.$f))
			$(\n)
		)
	)
	$(\n)
endef

#param $1 is a root node of a graph
define objects_to_export
	$(foreach o,$(call graph_closure,$1,get_referenced_objects),
		$(foreach m,$(instance-of $o,module),
			$o:=$(call escape_makefile,$(call get_qualified_name,$m))
			$(\n)
		)
	)
endef

$(def_all)

endif #__util_serialize_mk
