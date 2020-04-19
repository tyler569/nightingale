#include  <basic.h>

#if X86_64

.section .init
   /* gcc will nicely put the contents of crtend.o's .init section here. */
   popq %rbp
   ret

.section .fini
   /* gcc will nicely put the contents of crtend.o's .fini section here. */
   popq %rbp
   ret

#elif I686

.section .init
   /* gcc will nicely put the contents of crtend.o's .init section here. */
   popl %ebp
   ret

.section .fini
   /* gcc will nicely put the contents of crtend.o's .fini section here. */
   popl %ebp
   ret

#endif
