ifndef __util_serialize_mk
__util_serialize_mk := 1

include mk/util/graph.mk
include mk/util/escape.mk
include mk/mybuild/resource.mk
include mk/core/object.mk

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
		$(\n)
		__object_instance_cnt += $(o)$(\n)
		$o:=$(call escape_makefile,$($o))
		$(\n)
		$(foreach f,$(call field_name,$($($o).fields)),
			$o.$f:=$(call escape_makefile,$($o.$f))
			$(\n)
		)
	)
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

#param $1 'my_file' object
define model_to_resource
	$(if $(instance-of $1,my_file),
		$(call set_exports,$(new resource),$1)
		$(call resolve_internal,$1)
		$1$(\n)
		$(foreach o,$(call get_modules,$1),
			$(\n)$(call get_qualified_name,$o)
		)
	)
endef

$(def_all)

endif #__util_serialize_mk
