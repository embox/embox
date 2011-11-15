"GRAPH-graph",

	"TYPE-matcher:my_matcher",
	"TYPE-devs entry:devs",
	"TYPE-linux gate:lin_gate",
	"TYPE-info printer:info",

	"LINK-devs", "my_matcher",
	"LINK-my_matcher", "lin_gate",
	"LINK-info", "lin_gate",

	"RULES-my_matcher",
					"AA:BB:CC:DD:EE:01", "IP", "10.0.2.15", "ICMP", "*", "info"
