.data
.global a
a:
    .quad 128

.text
.global main
main:
pushq %rbp
movq %rsp, %rbp
subq $0, %rsp
pushq %rbx
pushq %r12
pushq %r13
pushq %r14
pushq %r15
# a
movq a, %rbx
# a
movq a, %r10
# 1
movq $1, %r11
# a - 1
subq %r11, %r10
# a = a - 1
movq %r10, a
# a
movq a, %rbx
movq %rbx, %rax
jmp .main_epilogue
.main_epilogue:
popq %r15
popq %r14
popq %r13
popq %r12
popq %rbx
movq %rbp, %rsp
popq %rbp
ret

