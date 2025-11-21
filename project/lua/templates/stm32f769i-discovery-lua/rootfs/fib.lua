function fib(n)
    if n < 3 then
        return 1
    else
        return fib(n - 1) + fib(n - 2)
    end
end

print("fib(7) = " .. fib(7))