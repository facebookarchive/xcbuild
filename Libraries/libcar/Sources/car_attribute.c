/* Copyright 2013-present Facebook. All Rights Reserved. */

#include <car/car.h>

#include <assert.h>
#include <string.h>
#include <stdio.h>

/* This is purely a utility; these structures don't correspond to anything inside a car file. */

struct car_attribute_entry {
    uint16_t identifier;
    uint16_t value;
};

struct car_attribute_list {
    size_t count;
    struct car_attribute_entry *entries;
};

static struct car_attribute_list *
_car_attribute_list_alloc(void)
{
    struct car_attribute_list *attributes = malloc(sizeof(struct car_attribute_list));
    if (attributes == NULL) {
        return NULL;
    }

    attributes->count = 0;
    attributes->entries = NULL;

    return attributes;
}

struct car_attribute_list *
car_attribute_alloc_values(size_t count, enum car_attribute_identifier *identifiers, uint16_t *values)
{
    struct car_attribute_list *attributes = _car_attribute_list_alloc();
    if (attributes == NULL) {
        return NULL;
    }

    attributes->count = count;
    attributes->entries = malloc(sizeof(struct car_attribute_entry) * attributes->count);
    if (attributes->entries == NULL) {
        car_attribute_free(attributes);
        return NULL;
    }

    for (size_t i = 0; i < attributes->count; i++) {
        struct car_attribute_entry *entry = &attributes->entries[i];
        entry->identifier = identifiers[i];
        entry->value = values[i];
    }

    return attributes;
}

struct car_attribute_list *
car_attribute_alloc_copy(struct car_attribute_list *attributes)
{
    struct car_attribute_list *list = _car_attribute_list_alloc();
    if (list == NULL) {
        return NULL;
    }

    list->count = attributes->count;
    list->entries = malloc(sizeof(struct car_attribute_entry) * list->count);
    if (list->entries == NULL) {
        car_attribute_free(list);
        return NULL;
    }
    memcpy(list->entries, attributes->entries, sizeof(struct car_attribute_entry) * list->count);

    return list;
}

uint16_t
car_attribute_get(struct car_attribute_list *attributes, enum car_attribute_identifier identifier)
{
    assert(attributes != NULL);

    /* Find existing entry. */
    for (size_t i = 0; i < attributes->count; i++) {
        struct car_attribute_entry *entry = &attributes->entries[i];
        if (entry->identifier == identifier) {
            return entry->value;
        }
    }

    /* Not found. */
    return UINT16_MAX;
}

void
car_attribute_set(struct car_attribute_list *attributes, enum car_attribute_identifier identifier, uint16_t value)
{
    assert(attributes != NULL);

    /* Update existing entry, if it exists. */
    for (size_t i = 0; i < attributes->count; i++) {
        struct car_attribute_entry *entry = &attributes->entries[i];
        if (entry->identifier == identifier) {
            entry->value = value;
            return;
        }
    }

    /* Not found; append to the end. */
    attributes->count++;
    attributes->entries = realloc(attributes->entries, sizeof(struct car_attribute_entry) * attributes->count);

    /* Configure newly added entry. */
    struct car_attribute_entry *entry = &attributes->entries[attributes->count - 1];
    entry->identifier = identifier;
    entry->value = value;
}

bool
car_attribute_equal(struct car_attribute_list *attributes1, struct car_attribute_list *attributes2)
{
    assert(attributes1 != NULL);
    assert(attributes2 != NULL);

    for (size_t i1 = 0; i1 < attributes1->count; i1++) {
        struct car_attribute_entry *entry1 = &attributes1->entries[i1];

        bool found_identifier = false;
        for (size_t i2 = 0; i2 < attributes2->count; i2++) {
            struct car_attribute_entry *entry2 = &attributes2->entries[i2];
            if (entry1->identifier == entry2->identifier) {
                found_identifier = true;
                if (entry1->value != entry2->value) {
                    return false;
                }
            }
        }
        if (!found_identifier) {
            return false;
        }
    }

    return true;
}

size_t
car_attribute_count(struct car_attribute_list *attributes)
{
    assert(attributes != NULL);

    return attributes->count;
}

void
car_attribute_iterate(struct car_attribute_list *attributes, car_attribute_iterator iterator, void *ctx)
{
    assert(attributes != NULL);

    for (size_t i = 0; i < attributes->count; i++) {
        struct car_attribute_entry *entry = &attributes->entries[i];
        iterator(attributes, i, entry->identifier, entry->value, ctx);
    }
}

static void
_car_attribute_iterator_dump(struct car_attribute_list *attributes, int index, enum car_attribute_identifier identifier, uint16_t value, void *ctx)
{
    if (identifier < sizeof(car_attribute_identifier_names) / sizeof(*car_attribute_identifier_names)) {
        printf("[%02d] %-24s = %-6d | %-4x\n", identifier, car_attribute_identifier_names[identifier] ?: "(unknown)", value, value);
    } else {
        printf("[%02d] %-24s = %-6d | %-4x\n", identifier, "(unknown)", value, value);
    }
}

void
car_attribute_dump(struct car_attribute_list *attributes)
{
    car_attribute_iterate(attributes, _car_attribute_iterator_dump, NULL);
}

void
car_attribute_free(struct car_attribute_list *attributes)
{
    if (attributes == NULL) {
        return;
    }

    free(attributes->entries);
    free(attributes);
}

