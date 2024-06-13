.globl strcmp_optm
.type strcmp_optm, @function
strcmp_optm:
  or a4, a0,a1
  andi a4, a4,0x3
  bnez a4, .Lmisaligned_optm

  lw a2, 0(a0)
  lw a3, 0(a1)
  lui t4, 0x80808
  bne a2, a3, .Lmismatch0
  lw a4, 4(a0)
  lw a5, 4(a1)
  ori t4, t4, 0x80
  bne a4, a5, .Lmismatch4
  lw a6, 8(a0)
  lw a7, 8(a1)
  xori t5, t4, -1
  bne a6, a7, .Lmismatch8
  lw t0, 12(a0)
  lw t1, 12(a1)
  add a3, a2, t5
  bne t1, t0, .Lmismatch12
  lw t2, 16(a0)
  lw t3, 16(a1)
  add a5, a4, t5
  beq t3, t2, .Lmatch16
  add a7, a6, t5
  add t1, t0, t5
  add t6, t2, t5
  and a3, a3, a5
  and a3, a3, a7
  and a3, a3, t1
  and a3, a3, t6
  and a3, a3, t4
  bne a3, t4, .Lmismatch16
  slli a2, t2, 8
  slli a3, t3, 8
  bne a2, a3, .Lmismatchlow24
  sub a0, t2, t3
  ret

.Lmisaligned_optm:
   # misaligned
  lbu a2, 0(a0)
  lbu a3, 0(a1)
  add a0, a0, 1
  add a1, a1, 1
  bne a2, a3, .Lretsub
  bnez a2, .Lmisaligned_optm

.Lretsub:
  sub a0, a2, a3
  ret

.Lmismatch0:
  # words don't match, and a2 maybe have null byte.
  lbu a2, 0(a0)
  lbu a3, 0(a1)
  bne a2, a3, .Lretsub
  beqz a2, .Lret0
  lbu a2, 1(a0)
  lbu a3, 1(a1)
  bne a2, a3, .Lretsub
  beqz a2, .Lret0
  lbu a2, 2(a0)
  lbu a3, 2(a1)
  bne a2, a3, .Lretsub
  beqz a2, .Lret0
  lbu a2, 3(a0)
  lbu a3, 3(a1)
  j .Lretsub
.Lret0:
  li a0, 0
  ret

.Lmismatch4:
  li t4, -1
  li t5, 0x7f7f7f7f
  and t6, a2, t5
  or t1, a2, t5
  add t6, t6, t5
  or t6, t6, t1
  bne t6, t4, .Lret0
  add a0, a0, 4
  add a1, a1, 4
  j .Lmismatch0

.Lmismatch8:
  li t4, -1
  li t5, 0x7f7f7f7f
  and t6, a2, t5
  or t1, a2, t5
  add t6, t6, t5
  or t6, t6, t1
  bne t6, t4, .Lret0
  and t6, a4, t5
  or t1, a4, t5
  add t6, t6, t5
  or t6, t6, t1
  bne t6, t4, .Lret0
  add a0, a0, 8
  add a1, a1, 8
  j .Lmismatch0

.Lmismatch12:
  li t4, -1
  li t5, 0x7f7f7f7f
  and t6, a2, t5
  or t1, a2, t5
  add t6, t6, t5
  or t6, t6, t1
  bne t6, t4, .Lret0
  and t6, a4, t5
  or t1, a4, t5
  add t6, t6, t5
  or t6, t6, t1
  bne t6, t4, .Lret0
  and t6, a6, t5
  or t1, a6, t5
  add t6, t6, t5
  or t6, t6, t1
  bne t6, t4, .Lret0
  add a0, a0, 12
  add a1, a1, 12
  j .Lmismatch0

.Lmismatch16:
  li t4, -1
  li t5, 0x7f7f7f7f
  and t6, a2, t5
  or t1, a2, t5
  add t6, t6, t5
  or t6, t6, t1
  bne t6, t4, .Lret0
  and t6, a4, t5
  or t1, a4, t5
  add t6, t6, t5
  or t6, t6, t1
  bne t6, t4, .Lret0
  and t6, a6, t5
  or t1, a6, t5
  add t6, t6, t5
  or t6, t6, t1
  bne t6, t4, .Lret0
  and t6, t0, t5
  or t1, t0, t5
  add t6, t6, t5
  or t6, t6, t1
  bne t6, t4, .Lret0
  add a0, a0, 16
  add a1, a1, 16
  j .Lmismatch0

.Lmatch16:
  li t4, -1
  li t5, 0x7f7f7f7f
  and t6, a2, t5
  or t1, a2, t5
  add t6, t6, t5
  or t6, t6, t1
  bne t6, t4, .Lret0
  and t6, a4, t5
  or t1, a4, t5
  add t6, t6, t5
  or t6, t6, t1
  bne t6, t4, .Lret0
  and t6, a6, t5
  or t1, a6, t5
  add t6, t6, t5
  or t6, t6, t1
  bne t6, t4, .Lret0
  and t6, t0, t5
  or t1, t0, t5
  add t6, t6, t5
  or t6, t6, t1
  bne t6, t4, .Lret0
  and t6, t2, t5
  or t1, t2, t5
  add t6, t6, t5
  or t6, t6, t1
  bne t6, t4, .Lret0
  add a0, a0, 20
  add a1, a1, 20
  j .Loldstrcmp

.Lmismatchlow24:
  slli a4, t2, 16
  slli a5, t3, 16
  bne a4, a5, .Lmismatchlow16
  sub a0, a2, a3
  ret

.Lmismatchlow16:
  andi a2, t2, 0xff
  andi a3, t3, 0xff
  bne a2, a3, .Lmismatchlow8
  srli a2, t2, 8
  srli a3, t3, 8
  andi a2, a2, 0xff
  andi a3, a3, 0xff
  sub a0, a2, a3
  ret

.Lmismatchlow8:
  sub a0, a2, a3
  ret

.Lmismatchup8:
  srli a0, a1, 8
  ret

.Loldstrcmp:
  lw a2, 0(a0)
  lw a3, 0(a1)
  and t0, a2, t5
  or t1, a2, t5
  add t0, t0, t5
  or t0, t0, t1
  bne t0, t4, .Lnull0_optm
  bne a2, a3, .Lmismatch_optm
  lw a2, 4(a0)
  lw a3, 4(a1)
  and t0, a2, t5
  or t1, a2, t5
  add t0, t0, t5
  or t0, t0, t1
  bne t0, t4, .Lnull1_optm
  bne a2, a3, .Lmismatch_optm
  lw a2, 8(a0)
  lw a3, 8(a1)
  and t0, a2, t5
  or t1, a2, t5
  add t0, t0, t5
  or t0, t0, t1
  bne t0, t4, .Lnull2_optm
  bne a2, a3, .Lmismatch_optm
  lw a2, 12(a0)
  lw a3, 12(a1)
  and t0, a2, t5
  or t1, a2, t5
  add t0, t0, t5
  or t0, t0, t1
  bne t0, t4, .Lnull3_optm
  bne a2, a3, .Lmismatch_optm
  lw a2, 16(a0)
  lw a3, 16(a1)
  and t0, a2, t5
  or t1, a2, t5
  add t0, t0, t5
  or t0, t0, t1
  bne t0, t4, .Lnull4_optm
  addi a0, a0, 20
  addi a1, a1, 20
  beq a2, a3, .Loldstrcmp

.Lmismatch_optm:
  slli a4, a2, 0x10
  slli a5, a3, 0x10
  bne a4, a5, .Lmismatch_upoptm
  srli a4, a2, 0x10
  srli a5, a3, 0x10
  sub a0, a4, a5
  andi a1, a0, 255
  bnez a1, .Lmismatch_lowoptm
  ret

.Lmismatch_upoptm:
  srli a4, a4, 0x10
  srli a5, a5, 0x10
  sub a0, a4, a5
  andi a1, a0, 255
  bnez a1, .Lmismatch_lowoptm
  ret

.Lmismatch_lowoptm:
  andi a4, a4, 255
  andi a5, a5, 255
  sub a0, a4, a5
  ret

.Lnull1_optm:
  addi a0, a0, 4
  addi a1, a1, 4

.Lnull0_optm:
  bne a2, a3, .Lmisaligned_optm
  li a0, 0
  ret

.Lnull2_optm:
  addi a0, a0, 8
  addi a1, a1, 8
  bne a2, a3, .Lmisaligned_optm
  li a0, 0
  ret

.Lnull3_optm:
  addi a0, a0, 12
  addi a1, a1, 12
  bne a2, a3, .Lmisaligned_optm
  li a0, 0
  ret

.Lnull4_optm:
  addi a0, a0, 12
  addi a1, a1, 12
  bne a2, a3, .Lmisaligned_optm
  li a0, 0
  ret

.size strcmp_optm, .-strcmp_optm
