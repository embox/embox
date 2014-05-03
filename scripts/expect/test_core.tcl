
set setup_proc ""
set teardown_proc ""

proc TEST_SETUP {setup} {
	global setup_proc
	set setup_proc $setup
}

proc TEST_TEARDOWN {teardown} {
	global teardown_proc
	set teardown_proc $teardown
}

proc TEST_CASE {test_name test_body} {
	global setup_proc
	global teardown_proc

	catch {eval $setup_proc}
	puts $test_name
	eval $test_body
	catch {eval $teardown_proc}
}

