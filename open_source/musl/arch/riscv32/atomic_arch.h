/* This is a stub file containing atomic instructions,
 * only used to build the compiler. User will not use 
 * these interfaces directly, and the atomic instructions
 * were also removed because of the rv32imc architecture. */

#define a_barrier a_barrier
static inline void a_barrier()
{
	__asm__ __volatile__ ("fence rw,rw" : : : "memory");
}

#define a_ll a_ll
static inline int a_ll(volatile int *p)
{
	int v;
	__asm__ __volatile__ ("lw %0, 0(%1)" : "=&r"(v) : "r"(p));
	return v;
}

#define a_sc a_sc
static inline int a_sc(volatile int *p, int v)
{
	int r = 0;
	__asm__ __volatile__ ("sw %1, 0(%0)" : : "r"(p), "r"(v) : "memory");
	return !r;
}

#define a_cas a_cas
static inline int a_cas(volatile int *p, int t, int s)
{
	int old;
	__asm__("1:  lw    %0, %1      \n"
		"    bne     %0, %2, 1f  \n"
		"    sw     %3, %1  \n"
		"1:                      \n"
		: "=&r"(old), "+A"(*p)
		: "r"(t), "r"(s));
	return old;
}
