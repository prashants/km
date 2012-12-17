probe <events> { handler }

events :
begin
end
kernel.function("sys_open")
kernel.function("*@net/socket.c").call/return
syscall.close.return()
module("ext3").statement(0xadsasdasd)
timer.ms(200)
timer.profile
perf.hw.cache_misses
procfs("status").read()
process("a.out").statement("*@main.c:200)

functions :
tid()
pid()
uid()
execname()
cpu()
gettimeofday_s()
get_cycles()
pp()
probefunc()
$$vars
print_backtrace()

$stap -e 'script'
$stap script.stp
$stap -l '<event*>'

