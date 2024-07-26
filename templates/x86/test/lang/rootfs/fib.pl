sub fib {
   my ($n) = @_;
   if ($n < 2) {
      return $n;
   }
   return fib($n-1) + fib($n-2);
}

print "fib(7)=", fib(7), "\n";
