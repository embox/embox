(define (fib n)
  (if (< n 3)
    1
    (+ (fib (- n 1))
       (fib (- n 2)))))

(display "fib(7) = ")
(display (fib 7))
(newline)
