function fib(n)
    if n < 3 then
        return 1
    else
        return fib(n - 1) + fib(n - 2)
    end
end

for i = 0,7 do
    print("Fib(" .. i .. ") = " .. fib(i))
end
