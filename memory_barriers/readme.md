Links:
- https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/Documentation/memory-barriers.txt?id=HEAD

Consider the following sequence of events:

	CPU 1		CPU 2
	===============	===============
	{ A == 1; B == 2 }
	A = 3;		x = B;
	B = 4;		y = A;

The set of accesses as seen by the memory system in the middle can be arranged
in 24 different combinations:

	STORE A=3,	STORE B=4,	y=LOAD A->3,	x=LOAD B->4
	STORE A=3,	STORE B=4,	x=LOAD B->4,	y=LOAD A->3
	STORE A=3,	y=LOAD A->3,	STORE B=4,	x=LOAD B->4
	STORE A=3,	y=LOAD A->3,	x=LOAD B->2,	STORE B=4
	STORE A=3,	x=LOAD B->2,	STORE B=4,	y=LOAD A->3
	STORE A=3,	x=LOAD B->2,	y=LOAD A->3,	STORE B=4
	STORE B=4,	STORE A=3,	y=LOAD A->3,	x=LOAD B->4
	STORE B=4, ...
	...

and can thus result in four different combinations of values:

	x == 2, y == 1
	x == 2, y == 3
	x == 4, y == 1
	x == 4, y == 3


GUARANTEES
----------

There are some minimal guarantees that may be expected of a CPU:

 (*) On any given CPU, dependent memory accesses will be issued in order, with
     respect to itself.  This means that for:

	Q = READ_ONCE(P); D = READ_ONCE(*Q);

     the CPU will issue the following memory operations:

	Q = LOAD P, D = LOAD *Q

     and always in that order. 
     
 (*) Overlapping loads and stores within a particular CPU will appear to be
     ordered within that CPU.  This means that for:

	a = READ_ONCE(*X); WRITE_ONCE(*X, b);

     the CPU will only issue the following sequence of memory operations:

	a = LOAD *X, STORE *X = b

     And for:

	WRITE_ONCE(*X, c); d = READ_ONCE(*X);

     the CPU will only issue:

	STORE *X = c, d = LOAD *X

     (Loads and stores overlap if they are targeted at overlapping pieces of
     memory).
     
VARIETIES OF MEMORY BARRIER
---------------------------

Memory barriers come in four basic varieties:

(1) Write (or store) memory barriers.

     A write memory barrier gives a guarantee that all the STORE operations
     specified before the barrier will appear to happen before all the STORE
     operations specified after the barrier with respect to the other
     components of the system.
     
(2) Data dependency barriers.

     A data dependency barrier is a weaker form of read barrier.  In the case
     where two loads are performed such that the second depends on the result
     of the first (eg: the first load retrieves the address to which the second
     load will be directed), a data dependency barrier would be required to
     make sure that the target of the second load is updated after the address
     obtained by the first load is accessed.
    
(3) Read (or load) memory barriers.

     A read barrier is a data dependency barrier plus a guarantee that all the
     LOAD operations specified before the barrier will appear to happen before
     all the LOAD operations specified after the barrier with respect to the
     other components of the system.

     A read barrier is a partial ordering on loads only; it is not required to
     have any effect on stores.

     Read memory barriers imply data dependency barriers, and so can substitute
     for them.
     
(4) General memory barriers.

     A general memory barrier gives a guarantee that all the LOAD and STORE
     operations specified before the barrier will appear to happen before all
     the LOAD and STORE operations specified after the barrier with respect to
     the other components of the system.

     A general memory barrier is a partial ordering over both loads and stores.

     General memory barriers imply both read and write memory barriers, and so
     can substitute for either.
     
**And a couple of implicit varieties:**

(5) ACQUIRE operations.

     This acts as a one-way permeable barrier.  It guarantees that all memory
     operations after the ACQUIRE operation will appear to happen after the
     ACQUIRE operation with respect to the other components of the system.
     ACQUIRE operations include LOCK operations and both smp_load_acquire()
     and smp_cond_load_acquire() operations.

     Memory operations that occur before an ACQUIRE operation may appear to
     happen after it completes.

     An ACQUIRE operation should almost always be paired with a RELEASE
     operation.
   
 (6) RELEASE operations.

     This also acts as a one-way permeable barrier.  It guarantees that all
     memory operations before the RELEASE operation will appear to happen
     before the RELEASE operation with respect to the other components of the
     system. RELEASE operations include UNLOCK operations and
     smp_store_release() operations.

     Memory operations that occur after a RELEASE operation may appear to
     happen before it completes.

     The use of ACQUIRE and RELEASE operations generally precludes the need
     for other sorts of memory barrier.  In addition, a RELEASE+ACQUIRE pair is
     -not- guaranteed to act as a full memory barrier.  However, after an
     ACQUIRE on a given variable, all memory accesses preceding any prior
     RELEASE on that same variable are guaranteed to be visible.  In other
     words, within a given variable's critical section, all accesses of all
     previous critical sections for that variable are guaranteed to have
     completed.

     This means that ACQUIRE acts as a minimal "acquire" operation and
     RELEASE acts as a minimal "release" operation.
     

CONTROL DEPENDENCIES
--------------------
Control dependencies can be a bit tricky because current compilers do
not understand them.  The purpose of this section is to help you prevent
the compiler's ignorance from breaking your code.

A load-load control dependency requires a full read memory barrier, not
simply a data dependency barrier to make it work correctly.

	q = READ_ONCE(a);
	if (q) {
		<data dependency barrier>  /* BUG: No data dependency!!! */
		p = READ_ONCE(b);
	}
  
In such a case what's actually required is:

	q = READ_ONCE(a);
	if (q) {
		<read barrier>
		p = READ_ONCE(b);
	}

However, stores are not speculated.  This means that ordering -is- provided
for load-store control dependencies, as in the following example:

	q = READ_ONCE(a);
	if (q) {
		WRITE_ONCE(b, 1);
	}
  
Control dependencies pair normally with other types of barriers.
That said, please note that neither READ_ONCE() nor WRITE_ONCE()
are optional! Without the READ_ONCE(), the compiler might combine the
load from 'a' with other loads from 'a'.  Without the WRITE_ONCE(),
the compiler might combine the store to 'b' with other stores to 'b'.
Either can result in highly counterintuitive effects on ordering.

Worse yet, if the compiler is able to prove (say) that the value of
variable 'a' is always non-zero, it would be well within its rights
to optimize the original example by eliminating the "if" statement
as follows:

	q = a;
	b = 1;  /* BUG: Compiler and CPU can both reorder!!! */

So don't leave out the READ_ONCE().

It is tempting to try to enforce ordering on identical stores on both
branches of the "if" statement as follows:

	q = READ_ONCE(a);
	if (q) {
		barrier();
		WRITE_ONCE(b, 1);
		do_something();
	} else {
		barrier();
		WRITE_ONCE(b, 1);
		do_something_else();
	}

Unfortunately, current compilers will transform this as follows at high
optimization levels:

	q = READ_ONCE(a);
	barrier();
	WRITE_ONCE(b, 1);  /* BUG: No ordering vs. load from a!!! */
	if (q) {
		/* WRITE_ONCE(b, 1); -- moved up, BUG!!! */
		do_something();
	} else {
		/* WRITE_ONCE(b, 1); -- moved up, BUG!!! */
		do_something_else();
	}

Now there is no conditional between the load from 'a' and the store to
'b', which means that the CPU is within its rights to reorder them:
The conditional is absolutely required, and must be present in the
assembly code even after all compiler optimizations have been applied.
Therefore, if you need ordering in this example, you need explicit
memory barriers, for example, smp_store_release():

	q = READ_ONCE(a);
	if (q) {
		smp_store_release(&b, 1);
		do_something();
	} else {
		smp_store_release(&b, 1);
		do_something_else();
	}
  
In contrast, without explicit memory barriers, two-legged-if control
ordering is guaranteed only when the stores differ, for example:

	q = READ_ONCE(a);
	if (q) {
		WRITE_ONCE(b, 1);
		do_something();
	} else {
		WRITE_ONCE(b, 2);
		do_something_else();
	}
  
  In addition, you need to be careful what you do with the local variable 'q',
otherwise the compiler might be able to guess the value and again remove
the needed conditional.  For example:

	q = READ_ONCE(a);
	if (q % MAX) {
		WRITE_ONCE(b, 1);
		do_something();
	} else {
		WRITE_ONCE(b, 2);
		do_something_else();
	}

If MAX is defined to be 1, then the compiler knows that (q % MAX) is
equal to zero, in which case the compiler is within its rights to
transform the above code into the following:

	q = READ_ONCE(a);
	WRITE_ONCE(b, 2);
	do_something_else();

Given this transformation, the CPU is not required to respect the ordering
between the load from variable 'a' and the store to variable 'b'.  It is
tempting to add a barrier(), but this does not help.  The conditional
is gone, and the barrier won't bring it back.  Therefore, if you are
relying on this ordering, you should make sure that MAX is greater than
one, perhaps as follows:

	q = READ_ONCE(a);
	BUILD_BUG_ON(MAX <= 1); /* Order load from a with store to b. */
	if (q % MAX) {
		WRITE_ONCE(b, 1);
		do_something();
	} else {
		WRITE_ONCE(b, 2);
		do_something_else();
	}

Please note once again that the stores to 'b' differ.  If they were
identical, as noted earlier, the compiler could pull this store outside
of the 'if' statement.

You must also be careful not to rely too much on boolean short-circuit
evaluation.  Consider this example:

	q = READ_ONCE(a);
	if (q || 1 > 0)
		WRITE_ONCE(b, 1);

Because the first condition cannot fault and the second condition is
always true, the compiler can transform this example as following,
defeating control dependency:

	q = READ_ONCE(a);
	WRITE_ONCE(b, 1);
  
  This example underscores the need to ensure that the compiler cannot
out-guess your code.  More generally, although READ_ONCE() does force
the compiler to actually emit code for a given load, it does not force
the compiler to use the results.

In addition, control dependencies apply only to the then-clause and
else-clause of the if-statement in question.  In particular, it does
not necessarily apply to code following the if-statement:

	q = READ_ONCE(a);
	if (q) {
		WRITE_ONCE(b, 1);
	} else {
		WRITE_ONCE(b, 2);
	}
	WRITE_ONCE(c, 1);  /* BUG: No ordering against the read from 'a'. */
  
It is tempting to argue that there in fact is ordering because the
compiler cannot reorder volatile accesses and also cannot reorder
the writes to 'b' with the condition.  Unfortunately for this line
of reasoning, the compiler might compile the two writes to 'b' as
conditional-move instructions, as in this fanciful pseudo-assembly
language:

	ld r1,a
	cmp r1,$0
	cmov,ne r4,$1
	cmov,eq r4,$2
	st r4,b
	st $1,c

A weakly ordered CPU would have no dependency of any sort between the load
from 'a' and the store to 'c'.  The control dependencies would extend
only to the pair of cmov instructions and the store depending on them.
In short, control dependencies apply only to the stores in the then-clause
and else-clause of the if-statement in question (including functions
invoked by those two clauses), not to code following that if-statement.


Note well that the ordering provided by a control dependency is local
to the CPU containing it.  See the section on "Multicopy atomicity"
for more information.

SMP BARRIER PAIRING
-------------------
........
........
