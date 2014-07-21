#include <slot_allocator/slot_allocator.h>
#include <unistd.h>
#include <AIM/aim.h>

#define MY_OBJECT_COUNT 3

struct my_object {
    void *foo;
    char bar[12];
};

static struct slot_allocator *my_object_allocator;
static struct my_object my_objects[MY_OBJECT_COUNT];

/*
 * Example code from slot_allocator.h
 */

static void
my_object_allocator_init(void)
{
    my_object_allocator = slot_allocator_create(MY_OBJECT_COUNT);
}

static struct my_object *
my_object_alloc(void)
{
    uint32_t slot = slot_allocator_alloc(my_object_allocator);
    if (slot == SLOT_INVALID) {
        return NULL;
    } else {
        return &my_objects[slot];
    }
}

static void
my_object_free(struct my_object *obj)
{
    uint32_t slot = obj - my_objects;
    slot_allocator_free(my_object_allocator, slot);
}

/*
 * End example code
 */

void
test_example(void)
{
    struct my_object *a, *b, *c;
    my_object_allocator_init();

    a = my_object_alloc();
    b = my_object_alloc();
    c = my_object_alloc();
    AIM_ASSERT(a && b && c);
    AIM_ASSERT(my_object_alloc() == NULL);

    my_object_free(a);
    my_object_free(b);
    my_object_free(c);
}
