#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

constexpr size_t PK_SIZE = 2048;
constexpr size_t PK_HEADROOM = 128;

struct pk {
        uint16_t head, tail;
        uint16_t l2, l3, l4;
        struct net_if *origin_if;
        struct pk *queue_next;
        uint8_t data[];
};
constexpr size_t PK_DATA_SIZE = PK_SIZE - sizeof(struct pk);

struct pk *pk_alloc(void);

static inline uint8_t *pk_data(struct pk *pk) { return pk->data + pk->head; }
static inline size_t pk_len(struct pk *pk) { return pk->tail - pk->head; }
static inline size_t pk_headroom(struct pk *pk) { return pk->head; }
static inline size_t pk_tailroom(struct pk *pk) { return PK_DATA_SIZE - pk->tail; }

void *pk_put(struct pk *pk, size_t n); // grow tail, return ptr to new space (payload)
void *pk_push(struct pk *pk, size_t n); // grow head backward, return new head (headers)

void *pk_peek(struct pk *pk, size_t n); // ptr to head if len >= n, else NULL
void *pk_pull(struct pk *pk, size_t n); // like peek, then advance head past it
bool pk_trim(struct pk *pk, size_t n); // shrink len to n (strip eth padding)

static inline void pk_mark_l2(struct pk *pk) { pk->l2 = pk->head; }
static inline void pk_mark_l3(struct pk *pk) { pk->l3 = pk->head; }
static inline void pk_mark_l4(struct pk *pk) { pk->l4 = pk->head; }
static inline void *pk_l2(struct pk *pk) { return (void *)(pk->data + pk->l2); }
static inline void *pk_l3(struct pk *pk) { return (void *)(pk->data + pk->l3); }
static inline void *pk_l4(struct pk *pk) { return (void *)(pk->data + pk->l4); }

#define PK_PUSH(pk, T) ((T *)pk_push(pk, sizeof(T)))
#define PK_PULL(pk, T) ((T *)pk_pull(pk, sizeof(T)))
#define PK_PEEK(pk, T) ((T *)pk_peek(pk, sizeof(T)))
