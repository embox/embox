proc tcl::mathfunc::fib {n} {
    if {$n <= 2} {
        return 1
    } else {
        return [expr fib([expr {$n - 1}]) + fib([expr {$n - 2}])]
    }
}

puts [expr fib(7)]

