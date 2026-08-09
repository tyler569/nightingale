#ifndef _NIGHTINGALE_STDATOMIC_H
#define _NIGHTINGALE_STDATOMIC_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Standard Memory Orderings */
typedef enum memory_order {
	memory_order_relaxed = __ATOMIC_RELAXED,
	memory_order_consume = __ATOMIC_CONSUME,
	memory_order_acquire = __ATOMIC_ACQUIRE,
	memory_order_release = __ATOMIC_RELEASE,
	memory_order_acq_rel = __ATOMIC_ACQ_REL,
	memory_order_seq_cst = __ATOMIC_SEQ_CST
} memory_order;

/* C11 Atomic Types */
typedef _Atomic(bool) atomic_bool;
typedef _Atomic(char) atomic_char;
typedef _Atomic(int) atomic_int;
typedef _Atomic(unsigned int) atomic_uint;
typedef _Atomic(long) atomic_long;
typedef _Atomic(unsigned long) atomic_ulong;
typedef _Atomic(size_t) atomic_size_t;
typedef _Atomic(uintptr_t) atomic_uintptr_t;

/* ========================================================================= */
/* Compiler Built-in Abstraction Layer                                       */
/* ========================================================================= */

#if defined(__clang__) \
	|| (defined(__has_builtin) && __has_builtin(__c11_atomic_fetch_add))

/* --- Clang / C11 Built-in Family --- */

#define atomic_init(obj, value) __c11_atomic_init(obj, value)

#define atomic_store_explicit(object, desired, order) \
	__c11_atomic_store(object, desired, order)

#define atomic_load_explicit(object, order) __c11_atomic_load(object, order)

#define atomic_exchange_explicit(object, desired, order) \
	__c11_atomic_exchange(object, desired, order)

#define atomic_compare_exchange_strong_explicit( \
	object, expected, desired, success, failure) \
	__c11_atomic_compare_exchange_strong( \
		object, expected, desired, success, failure)

#define atomic_compare_exchange_weak_explicit( \
	object, expected, desired, success, failure) \
	__c11_atomic_compare_exchange_weak( \
		object, expected, desired, success, failure)

#define atomic_fetch_add_explicit(object, operand, order) \
	__c11_atomic_fetch_add(object, operand, order)

#define atomic_fetch_sub_explicit(object, operand, order) \
	__c11_atomic_fetch_sub(object, operand, order)

#define atomic_fetch_or_explicit(object, operand, order) \
	__c11_atomic_fetch_or(object, operand, order)

#define atomic_fetch_and_explicit(object, operand, order) \
	__c11_atomic_fetch_and(object, operand, order)

#else

/* --- GCC / GNU __atomic_* Built-in Family --- */

#define atomic_init(obj, value) \
	do { \
		*(obj) = (value); \
	} while (0)

#define atomic_store_explicit(object, desired, order) \
	__atomic_store_n(object, desired, order)

#define atomic_load_explicit(object, order) __atomic_load_n(object, order)

#define atomic_exchange_explicit(object, desired, order) \
	__atomic_exchange_n(object, desired, order)

#define atomic_compare_exchange_strong_explicit( \
	object, expected, desired, success, failure) \
	__atomic_compare_exchange_n(object, expected, desired, 0, success, failure)

#define atomic_compare_exchange_weak_explicit( \
	object, expected, desired, success, failure) \
	__atomic_compare_exchange_n(object, expected, desired, 1, success, failure)

#define atomic_fetch_add_explicit(object, operand, order) \
	__atomic_fetch_add(object, operand, order)

#define atomic_fetch_sub_explicit(object, operand, order) \
	__atomic_fetch_sub(object, operand, order)

#define atomic_fetch_or_explicit(object, operand, order) \
	__atomic_fetch_or(object, operand, order)

#define atomic_fetch_and_explicit(object, operand, order) \
	__atomic_fetch_and(object, operand, order)

#endif

/* ========================================================================= */
/* Implicit Sequentially-Consistent (seq_cst) Convenience Macros             */
/* ========================================================================= */

#define atomic_store(object, desired) \
	atomic_store_explicit(object, desired, memory_order_seq_cst)

#define atomic_load(object) atomic_load_explicit(object, memory_order_seq_cst)

#define atomic_exchange(object, desired) \
	atomic_exchange_explicit(object, desired, memory_order_seq_cst)

#define atomic_compare_exchange_strong(object, expected, desired) \
	atomic_compare_exchange_strong_explicit( \
		object, expected, desired, memory_order_seq_cst, memory_order_seq_cst)

#define atomic_compare_exchange_weak(object, expected, desired) \
	atomic_compare_exchange_weak_explicit( \
		object, expected, desired, memory_order_seq_cst, memory_order_seq_cst)

#define atomic_fetch_add(object, operand) \
	atomic_fetch_add_explicit(object, operand, memory_order_seq_cst)

#define atomic_fetch_sub(object, operand) \
	atomic_fetch_sub_explicit(object, operand, memory_order_seq_cst)

#define atomic_fetch_or(object, operand) \
	atomic_fetch_or_explicit(object, operand, memory_order_seq_cst)

#define atomic_fetch_and(object, operand) \
	atomic_fetch_and_explicit(object, operand, memory_order_seq_cst)

#endif /* _NIGHTINGALE_STDATOMIC_H */
