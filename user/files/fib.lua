#!/usr/bin/lua

function fib(n)
    a, b = 0, 1
    for i = 1, n do
        a, b = b, a+b
    end
    return a
end

for i = 0, 30 do
    print(fib(i))
end
