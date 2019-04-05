	MODULE ?mutex

#define LOCKED   1
#define UNLOCKED 0

#define TRUE   1
#define FALSE  0

/* try to lock mutex */
	SECTION .mutex_try_lock:CODE:NOROOT(2)
	PUBLIC  mutex_try_lock
mutex_try_lock:
	mov     r1, #LOCKED
	mov     r3, r0
	mov     r0, #FALSE
	ldrex   r2, [r3]
	cmp     r2, r1        /* Test if mutex is locked or unlocked */
	bxeq    lr            /* mutex already locked, exit and return failure */
	strex   r2, r1, [r3]  /* Not locked, attempt to lock it */
	cmp     r2, #1        /* Check if Store-Exclusive failed */
	bxeq    lr            /* Store-Exclusive failed, return failure */
	dmb                   /* Required before accessing protected resource */
	mov     r0, #TRUE
	bx      lr

/* unlock mutex */
	SECTION .mutex_unlock:CODE:NOROOT(2)
	PUBLIC  mutex_unlock
mutex_unlock:
	mov     r1, #UNLOCKED
	dmb                   /* Required before releasing protected resource */
	str     r1, [r0]      /* Unlock mutex */
	bx      lr

/* Check if mutex is locked or not */
	SECTION .mutex_is_locked:CODE:NOROOT(2)
	PUBLIC  mutex_is_locked
mutex_is_locked:
	mov     r1, #LOCKED
	mov     r3, r0
	mov     r0, #FALSE
	ldrex   r2, [r3]
	cmp     r2, r1        /* Test if mutex is locked or unlocked */
	moveq   r0, #TRUE
	bx      lr
	
	END