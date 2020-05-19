#include "schedule.h"
#include "uart0.h"
#include "demo.h"
#include "exception.h"
#include "sysregs.h"
#include "mm.h"

struct task_t task_pool[TASK_POOL_SIZE];
char kstack_pool[TASK_POOL_SIZE][KSTACK_SIZE];
char ustack_pool[TASK_POOL_SIZE][USTACK_SIZE];
struct task_queue_elmt_t runqueue_elmt_pool[TASK_POOL_SIZE];
struct task_queue_t runqueue;

/* Task Queue */

void task_queue_init(struct task_queue_t* q) {
    q->front = NULL;
    q->rear = NULL;
}

void task_queue_elmt_init(struct task_queue_elmt_t* elmt, struct task_t *task) {
    elmt->task = task;
    elmt->next = NULL;
    elmt->prev = NULL;
}

void task_queue_push(struct task_queue_t* q, struct task_queue_elmt_t* elmt) {
    if (q->front == NULL) {
        q->front = elmt;
        q->rear = elmt;
    }
    // task priority is largest
    else if (elmt->task->priority > q->front->task->priority) {
        q->front->next = elmt;
        elmt->prev = q->front;
        q->front = elmt;
    }
    // task priority is smallest
    else if (elmt->task->priority <= q->rear->task->priority) {
        q->rear->prev = elmt;
        elmt->next = q->rear;
        q->rear = elmt;
    }
    // q->front->priority >= elmt->task->priority > q->last->priority
    else {
        // find appropriate place to insert
        struct task_queue_elmt_t *ptr = q->rear;
        while (ptr->next != NULL && elmt->task->priority > ptr->task->priority) {
            ptr = ptr->next;
        }
        // push elmt to back of ptr
        elmt->next = ptr;
        elmt->prev = ptr->prev;
        // relink before and after element
        ptr->prev->next = elmt;
        ptr->prev = elmt;
    }
}

struct task_t* task_queue_pop(struct task_queue_t* q) {
    if (q->front == NULL) {
        return &task_pool[0];
    }
    else if (q->front == q->rear) {
        struct task_queue_elmt_t* pop_elmt = q->front;
        struct task_t* pop_task = pop_elmt->task;
        pop_elmt->next = NULL;
        pop_elmt->prev = NULL;
        q->front = NULL;
        q->rear = NULL;
        return pop_task;
    }
    else {
        struct task_queue_elmt_t* pop_elmt = q->front;
        struct task_t* pop_task = pop_elmt->task;
        q->front = pop_elmt->prev;
        q->front->next = NULL;
        pop_elmt->next = NULL;
        pop_elmt->prev = NULL;
        return pop_task;
    }
}

void task_queue_print(struct task_queue_t* q) {
    struct task_queue_elmt_t* ptr = q->front;
    while (ptr != q->rear->prev) {
        uart_printf("%d ", ptr->task->id);
        ptr = ptr->prev;
    }
    uart_printf("\n");
}

/* ----- */

void runqueue_init() {
    for (int i = 0; i < TASK_POOL_SIZE; i++) {
        task_queue_elmt_init(&runqueue_elmt_pool[i], &task_pool[i]);
    }
    task_queue_init(&runqueue);
}

struct task_queue_elmt_t* get_runqueue_elmt(struct task_t* task) {
    return &runqueue_elmt_pool[task->id];
}

/* scheduler */

void mm_struct_init(struct mm_struct *m) {
    m->pgd = 0;
    m->user_pages_count = 0;
    m->kernel_pages_count = 0;
}

void task_init() {
    for (int i = 0; i < TASK_POOL_SIZE; i++) {
        task_pool[i].id = i;
        task_pool[i].state = EXIT;
        task_pool[i].need_resched = 0;
        mm_struct_init(&task_pool[i].mm);
    }
    // idle task
    task_pool[0].state = RUNNING;
    task_pool[0].priority = 0;
    update_current_task(&task_pool[0]);
}

void zombie_reaper() {
    while (1) {
        for (int i = 0; i < TASK_POOL_SIZE; i++) {
            if (task_pool[i].state == ZOMBIE) {
                uart_printf("reaper %d!\n", i);
                task_pool[i].state = EXIT;
                // WARNING: release kernel stack if dynamic allocation
            }
        }
        schedule();
    }
}

void schedule_init() {
    runqueue_init();
    // privilege_task_create(zombie_reaper, 10);

    privilege_task_create(demo_lab5_req3, 10);
    privilege_task_create(demo_lab5_req3, 10);
    privilege_task_create(demo_lab5_req3, 10);

    arm_core_timer_enable();
    schedule();
}

int privilege_task_create(void (*func)(), int priority) {
    struct task_t *new_task;
    for (int i = 0; i < TASK_POOL_SIZE; i++) {
        if (task_pool[i].state == EXIT) {
            new_task = &task_pool[i];
            break;
        }
    }

    new_task->state = RUNNING;
    new_task->priority = priority;
    new_task->counter = TASK_EPOCH;
    new_task->need_resched = 0;
    mm_struct_init(&new_task->mm);
    new_task->cpu_context.lr = (uint64_t)func;
    new_task->cpu_context.fp = (uint64_t)(&kstack_pool[new_task->id][KSTACK_TOP_IDX]);
    new_task->cpu_context.sp = (uint64_t)(&kstack_pool[new_task->id][KSTACK_TOP_IDX]);

    task_queue_push(&runqueue, get_runqueue_elmt(new_task));

    return new_task->id;
}

void context_switch(struct task_t* next) {
    struct task_t* prev = get_current_task();
    if (prev->state == RUNNING) {
        task_queue_push(&runqueue, get_runqueue_elmt(prev));
    }
    update_current_task(next);
    update_pgd(next->mm.pgd);
    switch_to(&prev->cpu_context, &next->cpu_context);
}

void schedule() {
    struct task_t* next = task_queue_pop(&runqueue);
    context_switch(next);
}

int do_exec(uint64_t start, uint64_t size, uint64_t pc) {
    uint64_t sp = 0x0000ffffffffe000 - 8; // stack grow down

    struct task_t *current = get_current_task();
    void* code_page = map_addr_user(pc);
    void* stack_page = map_addr_user(sp);
    if (!code_page || !stack_page) return -1;

    // copy code to pc
    uint8_t* pc_ptr = (uint8_t*)code_page;
    uint8_t* code_ptr = (uint8_t*)start;
    for (uint64_t i = 0; i < size; i++) {
        *(pc_ptr + i) = *(code_ptr + i);
    }

    asm volatile("msr sp_el0, %0" : : "r"(sp));
    asm volatile("msr elr_el1, %0": : "r"(pc));
    asm volatile("msr spsr_el1, %0" : : "r"(SPSR_EL1_VALUE));

    update_pgd(current->mm.pgd);

    asm volatile("eret");

    return 0;
}

void do_exit(int status) {
    struct task_t* current = get_current_task();
    current->state = ZOMBIE;
    current->exit_status = status;

    // reclaim 

    schedule();
}
