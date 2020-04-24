#ifndef SCHED_H_
#define SCHED_H_

#include "queue.h"

#define MAX_TASK_NUM 64
#define MAX_STACK_SIZE 4096

struct task_context {
  /* Callee saved registers */
  uint64_t x19;
  uint64_t x20;
  uint64_t x21;
  uint64_t x22;
  uint64_t x23;
  uint64_t x24;
  uint64_t x25;
  uint64_t x26;
  uint64_t x27;
  uint64_t x28;

  uint64_t fp;
  uint64_t lr;
  uint64_t sp;
};

struct task {
  struct task_context context;
  uint32_t id;
};

struct queue runqueue;

struct task __attribute__((aligned(16))) task_pool[MAX_TASK_NUM];
bool task_inuse[MAX_TASK_NUM];
uint8_t kstack_pool[MAX_TASK_NUM][MAX_STACK_SIZE];

void idle_task_init(void);
void privilege_task_create(void(*func)(void));
void context_switch(struct task *next);
void schedule();

void context_switch_helper(struct task *prev, struct task *next);
struct task *get_current_task(void);

void idle(void);
void foo(void);

#endif // SCHED_H_
