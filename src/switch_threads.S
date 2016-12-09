.code64
.text
.global switch_threads
.global switch_new_thread
.extern thread_origin

switch_threads:
pushq %rbx
pushq %rbp
pushq %r12
pushq %r13
pushq %r14
pushq %r15
pushfq

movq %rsp, (%rdi)
movq %rsi, %rsp

popfq
popq %r15
popq %r14
popq %r13
popq %r12
popq %rbp
popq %rbx

ret


switch_new_thread:
pushq %rbx
pushq %rbp
pushq %r12
pushq %r13
pushq %r14
pushq %r15
pushfq

movq %rsp, (%rdi)
movq %rsi, %rsp

popfq
popq %r15
popq %r14
popq %r13
popq %r12
popq %rbp
popq %rbx

movq %rsp, %rax
addq $8, %rax

movq (%rax), %rdi

cld
callq *(%rsp)

popq %rax
popq %rax

movq %rsp, %rax
addq $8, %rax

movq (%rax), %rdi

ret
