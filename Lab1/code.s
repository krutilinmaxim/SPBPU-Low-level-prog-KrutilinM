start:
    li t1, 0xFF0000        # красный
    li t2, 0x00FF00        # зелёный
    li s0, 0xf0000000     
    li s1, 0xf0000190      

    li s2, 0               
    li s3, 10             

next_row:
    beq s2, s3, end

       li t5, 0              
wait_click:
    lw a0, 8(s1)           # LEFT
    lw a1, 12(s1)          # RIGHT

    li t4, 1
    beq a0, t4, left_click
    beq a1, t4, right_click


    bnez t5, start_filling
    j wait_click

left_click:
    li t5, 2              
    j wait_click

right_click:
    li t5, 1               
    j wait_click

start_filling:
    li t4, 1
    beq t5, t4, right_dir
    j left_dir

right_dir:
    li t5, 0               
right_loop:
    li t0, 10
    beq t5, t0, row_done

    slli t6, s2, 5
    slli t3, s2, 3
    add t6, t6, t3
    slli t3, t5, 2
    add t6, t6, t3
    add t6, s0, t6

    sw t1, 0(t6)

    li t0, 1
1:  addi t0, t0, -1
    bnez t0, 1b

    addi t5, t5, 1
    j right_loop

left_dir:
    li t5, 9              
left_loop:
    li t0, -1
    beq t5, t0, row_done

    slli t6, s2, 5
    slli t3, s2, 3
    add t6, t6, t3
    slli t3, t5, 2
    add t6, t6, t3
    add t6, s0, t6

    sw t2, 0(t6)

    li t0, 1
2:  addi t0, t0, -1
    bnez t0, 2b

    addi t5, t5, -1
    j left_loop

row_done:
    addi s2, s2, 1
    j next_row

end:
    j end