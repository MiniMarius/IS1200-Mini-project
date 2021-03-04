  # labwork.S
  # Written 2015-2017 by F Lundevall
  # Skeleton file for IS1200/IS1500 lab 1.
  # The macros PUSH and POP are in the public domain.
  # Please add your own code at the end of the file.

  #
  # Please keep the two macros PUSH and POP unchanged
  #
.global delay
.global time2string
.global enable_interrupt

.macro  PUSH reg
      addi  $sp,$sp,-4
      sw    \reg,0($sp)
.endm
.macro  POP reg
      lw    \reg,0($sp)
      addi  $sp,$sp,4
.endm
.data
.align 2
.text
 main:
  #
  # Please add your own code below this line
  

hexasc:
	andi    $a0,$a0,0xf             # takes only 4 LSB using bitwise and
	li      $v0,0x30                # set v0 to 0 in ascii by default
    ble     $a0,0x09,hexit    # if a0 is in range 0-9, go to hexit
	nop
    li      $v0,0x37                # set v0 to 0x37 (A - 10 in ascii) (only reached when a0 is > 9)

hexit:
    add     $v0,$v0,$a0             # set v0 to corresponding ascii value
    jr      $ra
    nop

delay:
	PUSH 	$ra
		
	li		$t2, 0	#counter in for loop
	li    	$t1, 4420000	#ms CALIBRATION value
	move    $t0,$a0		#moves argument to t0
	while:
	ble     $t0,$zero,exitDelay		# if ms <= 0, move to exitDelay
	nop
	addi    $t0,$t0,-1      		#decrement ms by 1  
	for:
	beq     $t2,$t1,while		#if t2 equals calibration value, go back to while
	nop
	addi    $t2,$t2,1	#increment counter
	j       for

exitDelay:   
	POP		$ra
		
	jr  	$ra
	nop
	

time2string:
	PUSH    $ra
	PUSH	$s0
	PUSH	$s1
	
	move    $s0,$a0      #contains address of string and is where we store it later
	move    $s1,$a1      #contains the time-info(0x5957)
	
	andi 	$t0,$s1,0xf000	#extracts highest bits 16-12 with and-mask
	srl 	$a0,$t0,12	#moves these to the 4 LSB position so hexasc can use it
	jal 	hexasc
	nop
	sb      $v0,0($s0)     #store the 4 bits in the location a0 points to
	
	andi 	$t1,$s1,0x0f00	#extracts bits 12-8
	srl 	$a0,$t1, 8			#moves these to the 4 LSB position so hexasc can use it
	jal 	hexasc
	nop
	sb      $v0,1($s0)     #stores at location a0 points to
	
	li      $t3,0x3A		#assigns ascii colon to t5
	sb      $t3,2($s0)		#stores colon in a0 position 2
	
	andi 	$t2,$s1,0x00f0 	#extracts bits 8-4
	srl 	$a0, $t2,4			#moves these to the 4 LSB position so hexasc can use it
	jal 	hexasc
	nop
	sb		$v0,3($s0)		# stores at location a0 points to
	
	move	$a0,$s1		#no need to shift so move directly
	jal		hexasc
	nop
	sb		$v0,4($s0)		#stores at location a0 points to
	
	dingDone:
	
	li 		$t4,0x00		#loads NUL and sends it to memory address of a0
	sb		$t4,5($s0)
	
	POP		$s0
	POP		$s1
	POP 	$ra
	jr  	$ra
	nop 
enable_interrupt:
	ei 
	jr		$ra
	nop

