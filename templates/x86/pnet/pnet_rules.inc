"GRAPH-graph",

	"TYPE-matcher:my_matcher",
	"TYPE-devs entry:devs",
	"TYPE-linux gate:lin_gate",
	"TYPE-info printer:info",

	"LINK_SRC-devs", "LINK_DST-my_matcher",
	"LINK_SRC-my_matcher", "LINK_DST-lin_gate",
	"LINK_SRC-info", "LINK_DST-lin_gate",

	"RULES-my_matcher",
					"AA:BB:CC:DD:EE:01", "IP", "10.0.2.15", "ICMP", "*", "info"
