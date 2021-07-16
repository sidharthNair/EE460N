   .ORIG x3000
      AND R0,R0,x0000 ; R0<--0
      ADD R0,R0,x7 ; R0 <-- x7
      AND R1,R0,x-2 ; R1 <-- x6
      LEA R2, A ;
      LDW R3, R2, #0 ; R3 <-- x4000
      LDW R4, R2, #1 ; R4 <-- xFFFF N=1
      LSHF R4, R4, #8 ; R4 <-- xFF00
      RSHFL R3, R3, #3 ; R3 <-- x800 P = 1
      RSHFA R3, R3, #1 ; R3 <-- x400
      RSHFA R4, R4, #10 ; R4 <-- xFFFF
      ADD R5, R4, R0 ; R5 <--- x6
      XOR R5, R5, R1 ; R5 <--- 0 z=1
      BRz B
D     LEA R0, C ; R0 <-- C  x301a
      LDW R0, R0, #0; R0 <-- x3500
      STW R1, R0, #0 ; x3500 <-- 0010
      STW R1, R0, #-2; x34FC < -- 0010
      STB R1, R0, #2; x3502 <-- x0010
      STB R1, R0, #3; x3502 <-- x1010
      JSR E
      LEA R0, F  ;3028
      JSRR R0

B     LEA R0, C
      LDB R1, R0, #-4 ; R1 <-- x0001
      LDB R1, R0, #-3 ; R1 <-- x0010
      LEA R2, D
      JMP R2   ; jump to D

A     .FILL x4000 ;3036
      .FILL xFFFF ;3038
      .FILL x1001 ; 303a
      .FILL x0000 ; 303c
C     .FILL x3500 ; 303e

E     AND R0, R0, x0000 ;3040
      ADD R0, R0, x07 ;R0 <-- 7
      RET

F     HALT ;3046


      .END
