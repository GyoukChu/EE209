### --------------------------------------------------------------------
### mydc.s
### Assignment #4
### Edited by Gyouk Chu on 03/11/2021
### Last Updated on 18/11/2021
###
### Desk Calculator (dc)
### --------------------------------------------------------------------
	.equ   ARRAYSIZE, 20
	.equ   EOF, -1
	.equ   TWOOFFSETS, 8
	.equ   ONEOFFSET, 4
	.equ   INT_MIN, -2147483648
.section ".rodata"

scanfFormat:
	.asciz "%s"
printfFormat:
	.asciz "%d\n"
INTMINString:
	.asciz "2147483648" # for ifund
emptystackString:
	.asciz "dc: stack empty\n"
zerodivString:
	.asciz "dc: divide by zero\n"
overflowString:
	.asciz "dc: overflow happens\n"

### --------------------------------------------------------------------

        .section ".data"

### --------------------------------------------------------------------

        .section ".bss"
buffer:
        .skip  ARRAYSIZE

### --------------------------------------------------------------------

	.section ".text"

	## -------------------------------------------------------------
	## int main(void)
	## Runs desk calculator program.  Returns 0.
	## -------------------------------------------------------------

	.globl  main
	.type   main,@function

main:
	pushl   %ebp
	movl    %esp, %ebp

input:
	## dc number stack initialized. %esp = %ebp

	## scanf("%s", buffer)
	pushl	$buffer
	pushl	$scanfFormat
	call	scanf
	addl	$TWOOFFSETS, %esp

	## check if user input EOF
	cmp	$EOF, %eax
	je	quit

	## get the first element of buffer. buffer[0]
	movl	$buffer, %ebx
	movzx	0(%ebx), %esi

	## check whether the first element is digit or not
	pushl	%esi
	call	isdigit
	addl	$ONEOFFSET, %esp
	## isdigit(buffer[0])
	cmp	$0, %eax
	jne	ifdigit
	
	## verify the corresponding non-digit case
	
	## check if user input is 'p'
	cmpb $'p', buffer
	## if then, go to ifp
	je ifp

	## check if user input is 'q'
	cmpb $'q', buffer
	## if then, go to ifq
	je ifq

	## check if user input is '+'
	cmpb $'+', buffer
	## if then, go to ifadd
	je ifadd

	## check if user input is '-'
	cmpb $'-', buffer
	## if then, go to ifsub
	je ifsub

	## check if user input is '*'
	cmpb $'*', buffer
	## if then, go to ifmul
	je ifmul

	## check if user input is '/'
	cmpb $'/', buffer
	## if then, go to ifdiv
	je ifdiv

	## check if user input is '%'
	cmpb $'%', buffer
	## if then, go to ifmod
	je ifmod

	## check if user input is '^'
	cmpb $'^', buffer
	## if then, go to ifpow
	je ifpow

	## check if user input is '_'
	cmpb $'_', buffer
	## if then, go to ifund
	je ifund

	## check if user input is 'f'
	cmpb $'f', buffer
	## if then, go to iff
	je iff

	## check if user input is 'c'
	cmpb $'c', buffer
	## if then, go to ifc
	je ifc

	## check if user input is 'd'
	cmpb $'d', buffer
	## if then, go to ifd
	je ifd

	## check if user input is 'r'
	cmpb $'r', buffer
	## if then, go to ifr
	je ifr

	## if not match anything, then ignore and scan new
	jmp input
## -------------------------------------------------------------

ifp:
	## check whether stack is empty
	cmp %esp, %ebp
	je emptystackerror
	## print the value of top of the stack
	movl (%esp), %esi
	push	%esi
	pushl $printfFormat
	call	printf
	addl	$TWOOFFSETS, %esp
	jmp	input
	
ifq:
	je quit

ifadd:
	## check whether stack has at least two elements
	## 1. check whether stack is empty
	cmp %esp, %ebp
	je emptystackerror
	## 2. check whether stack has only one element
	movl %esp, %esi
	addl $ONEOFFSET, %esi
	cmp %esi, %ebp
	je emptystackerror
	## pop two elements, add them, and push the result
	popl %ebx #top element
	popl %esi #bottom element
	addl %ebx, %esi 
	## check whether overflow occurs
	jo overflowerror
	pushl %esi #stack.push(result)
	jmp	input

ifsub:
	## check whether stack has at least two elements
	## 1. check whether stack is empty
	cmp %esp, %ebp
	je emptystackerror
	## 2. check whether stack has only one element
	movl %esp, %esi
	addl $ONEOFFSET, %esi
	cmp %esi, %ebp
	je emptystackerror
	## pop two elements, sub them, and push the result
	popl %ebx #top element
	popl %esi #bottom element
	subl %ebx, %esi
	## check whether overflow occurs
	jo overflowerror
	pushl %esi #stack.push(result)
	jmp	input

ifmul:
	## check whether stack has at least two elements
	## 1. check whether stack is empty
	cmp %esp, %ebp
	je emptystackerror
	## 2. check whether stack has only one element
	movl %esp, %esi
	addl $ONEOFFSET, %esi
	cmp %esi, %ebp
	je emptystackerror
	## pop two elements, mul them, and push the result
	popl %ebx #top element
	popl %esi #bottom element
	imull %ebx, %esi
	## check whether overflow occurs
	jo overflowerror
	pushl %esi #stack.push(result)
	jmp	input

ifdiv:
	## check whether stack has at least two elements
	## 1. check whether stack is empty
	cmp %esp, %ebp
	je emptystackerror
	## 2. check whether stack has only one element
	movl %esp, %esi
	addl $ONEOFFSET, %esi
	cmp %esi, %ebp
	je emptystackerror
	## pop two elements, div them, and push the result
	popl %ebx #top element, denominator
	popl %esi #bottom element, numerator
	## check whether denominator is zero
	cmp $0, %ebx
	je zerodiverror
	## handle overflow case, namely INT_MIN/(-1)
	## compare numerator & INT_MIN
	cmp $INT_MIN, %esi
	jne	ifsafediv
	## may not be safe. compare denominator & (-1)
	cmp $-1, %ebx
	je	overflowerror # overflow occurs
ifsafediv:
	movl %esi, %eax
	cdq
	idivl %ebx
	pushl %eax #quotient is in %eax
	jmp	input

ifmod:
	## check whether stack has at least two elements
	## 1. check whether stack is empty
	cmp %esp, %ebp
	je emptystackerror
	## 2. check whether stack has only one element
	movl %esp, %esi
	addl $ONEOFFSET, %esi
	cmp %esi, %ebp
	je emptystackerror
	## pop two elements, div them, and push the result
	popl %ebx #top element, denominator
	popl %esi #bottom element, numerator
	## check whether denominator is zero
	cmp $0, %ebx
	je zerodiverror
	## handle exceptional case, INT_MIN/(-1)
	## compare numerator & INT_MIN
	cmp $INT_MIN, %esi
	jne	ifsafemod
	## may not be safe. compare denominator & (-1)
	cmp $-1, %ebx
	jne	ifsafemod
	## for such case, result is 0
	movl $0, %eax
	pushl %eax
	jmp	input
ifsafemod:
	movl %esi, %eax
	cdq
	idivl %ebx
	pushl %edx #remainder is in %edx
	jmp	input

ifpow:
	## check whether stack has at least two elements
	## 1. check whether stack is empty
	cmp %esp, %ebp
	je emptystackerror
	## 2. check whether stack has only one element
	movl %esp, %esi
	addl $ONEOFFSET, %esi
	cmp %esi, %ebp
	je emptystackerror
	## pop two elements
	popl %ebx #top element, exponent
	popl %esi #bottom element, base
	## call my powerfunc
	call mypowerfunc 
  pushl %eax #stack.push(result)
	jmp	input

mypowerfunc:
  movl $1, %eax
  movl %ebx, %ecx #exponent, using as index
  jmp mulloop
## multiply base(%esi) to 1(%eax) exponent(%ebx) many times
mulloop:
  cmpl $0, %ecx
  jle endloop
  imull %esi
  ## check whether overflow occurs
  jo overflowerror
  decl %ecx
  jmp mulloop
endloop:
  ret

ifund:
	## make string as a digit without '_'
	movl  $buffer, %ebx
	addl  $1, %ebx
	## handle INT_MIN case. with strcmp()
	pushl %ebx
	pushl	$INTMINString
	call	strcmp
	addl  $TWOOFFSETS, %esp
	## strcmp() returns 0 if they are same
	cmpl 	$0, %eax
	je		ifintmin
	## if not, then change into int with atoi()
	pushl  %ebx
	call   atoi
	addl   $ONEOFFSET, %esp
	## multiply -1 after, and then push
	imull $-1, %eax
	pushl %eax
	jmp	input
ifintmin:
	## directly push INT_MIN to stack
	movl	$INT_MIN, %ebx
	pushl %ebx
	jmp input

ifdigit:
	## make string as a digit, %eax becomes such digit.
	pushl $buffer
	call	atoi
	addl  $ONEOFFSET, %esp
	## push new digit
	pushl %eax
	jmp	input

iff:
	movl %esp, %esi ## moving pointer
printloop:
	## check whether it is bottom of stack or not
	cmp %esi, %ebp
	je input
	movl (%esi), %edi
	push	%edi
	pushl $printfFormat
	call	printf
	addl	$TWOOFFSETS, %esp
	## move to get next contents in stack
	addl	$ONEOFFSET, %esi
	jmp printloop

ifc:
	cmp %esp, %ebp
	je input
	popl %eax
	jmp ifc
	
ifd:
	## check whether stack is empty
	cmp %esp, %ebp
	je emptystackerror
	movl (%esp), %esi
	pushl %esi
	jmp input

ifr:
	## check whether stack has at least two elements
	## 1. check whether stack is empty
	cmp %esp, %ebp
	je emptystackerror
	## 2. check whether stack has only one element
	movl %esp, %esi
	addl $ONEOFFSET, %esi
	cmp %esi, %ebp
	je emptystackerror
	popl %ebx #top element
	popl %esi #bottom element
	pushl %ebx #top element
	pushl %esi #bottom element
	jmp input

emptystackerror:
	pushl	$emptystackString
	pushl stderr
	call	fprintf
	addl	$TWOOFFSETS, %esp
	## not terminated. should continue from the beginning
	jmp input

zerodiverror:
	pushl	$zerodivString
	pushl stderr
	call	fprintf
	addl	$TWOOFFSETS, %esp
	## should be terminated
	jmp quit

overflowerror:
	pushl	$overflowString
	pushl stderr
	call	fprintf
	addl	$TWOOFFSETS, %esp
	## should be terminated
	jmp quit

## PSEUDO-CODE in C
	## 
	##1 while (1) {
	##2	if (scanf("%s", buffer) == EOF)
	##3		return 0;
	##4 if (!isdigit(buffer[0])) {
	##5		if (buffer[0] == 'p') {
	##6			if (stack.peek() == NULL) { /* check %esp == %ebp */
	##7				fprintf(stderr, "dc: stack empty\n");
	##8			} else {
	##9				printf("%d\n", (int)stack.top()); /* value is already in the stack */
	##10			}
	##11		} else if (buffer[0] == 'q') {
	##12			goto quit;
	##13		} else if (buffer[0] == '+') {
	##14			int a, b;
	##15			if (stack.peek() == NULL) { /* check whether no element */
	##16				fprintf(stderr, "dc: stack empty\n");
	##17				continue;
	##18			}
	##19			a = (int)stack.pop(); //not actually pop in this case
	##20			if (stack.peek() == NULL) { /* check whether only 1 element */
	##21				fprintf(stderr, "dc: stack empty\n");
	##22				stack.push(a); /* pushl some register value */
	##23				continue;
	##24			}
	##25			b = (int)stack.pop(); /* popl to some register */
	##26			b += a; /* with addl , need overflow check */
	##27 			stack.push(b);
	##28		} else if (buffer[0] == '-') {
	##29			/* similar way. b -= a; stack.push(b); with subl */
	##30			/* need overflow check */
	##30		} else if (buffer[0] == '*') {
	##31			/* similar way. b *= a; stack.push(b); with imull */
	##32			/* need overflow check */
	##33		} else if (buffer[0] == '/') {
	##34			/* similar way. b /= a; stack.push(b); with idivl */
	##35			/* need zero-division check, INT_MIN/(-1) case is exceptional */
	##36		} else if (buffer[0] == '%') {
	##37			/* similar way. b %= a; stack.push(b); with idivl */
	##38			/* need zero-division check, INT_MIN/(-1) case is exceptional */
	##39		} else if (buffer[0] == '^') {
	##40			int top = (int)stack.pop();
	##41			int bot = (int)stack.pop();
	##42			int res = 1;
	##43			for(int i=0; i<top; i++) {
	##44				res *= bot; /* need overflow check */
	##45			}
	##46			stack.push(res);
	##47		} else if (buffer[0] == '_') { /* this is also no. */
	##48				int no = atoi(buffer+1) * (-1); // INT_MIN case is exceptional
	##49 		}
	##50 	}
	##51	else { /* the first no. is a digit */
	##52		int no = atoi(buffer);
	##53		stack.push(no);	/* pushl some register value */
	##54	}
	##55 }

quit:
	## return 0
	movl    $0, %eax
	movl    %ebp, %esp
	popl    %ebp
	ret
