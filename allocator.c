#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>

#define MANAGING_SIZE 12

char *arena;
u_int32_t arena_size, start_index;

void initialize(uint32_t size)
{
    arena_size = size;
    arena = (char *)calloc(arena_size, sizeof(char));
    start_index = 0;
}

void finalize()
{
    free(arena);
}

void dump()
{
    int i, j;
    for (i = 0; i < arena_size / 16; i++)
    {
        printf("%08X\t", i * 16);
        for (j = 0; j < 16; j++)
        {
            char value = *(arena + i * 16 + j);
            printf("%02X ", value);
            if (j == 7)
                printf(" ");
        }
        printf("\n");
    }

    if (arena_size % 16 != 0)
        printf("%08X\t", arena_size / 16 * 16);
    for (j = 0; j < arena_size % 16; j++)
    {
        char value = *(arena + arena_size - arena_size % 16 + j);
        printf("%02X ", value);
        if (j == 7)
            printf(" ");
    }
    if (arena_size % 16 != 0)
        printf("\n");
}

void alloc(uint32_t size)
{
    int space_found = 0;
    uint32_t space_index = 0;
    uint32_t current_index = start_index;

    // check if no block exists
    if (*(uint32_t *)(arena + current_index) == 0 &&
        *(uint32_t *)(arena + current_index + 4) == 0 &&
        *(uint32_t *)(arena + current_index + 8) == 0)
    {
        space_found = 1;
        space_index = 0;
    }

    else
    {
        while (current_index < arena_size)
        {
            uint32_t next_index = *(uint32_t *)(arena + current_index);
            uint32_t data_size = *(uint32_t *)(arena + current_index + 8);

            // check for space after current block
            uint32_t block_end = current_index + MANAGING_SIZE + data_size;
            uint32_t free_space = (next_index == 0) ? (arena_size - block_end) : (next_index - block_end);

            if (free_space >= size + MANAGING_SIZE)
            {
                space_index = block_end;
                space_found = 1;
                break;
            }

            if (next_index == 0)
                break;
            current_index = next_index;
        }
    }

    if (!space_found)
    {
        printf("0\n");
        return;
    }

    // add new block data
    *(uint32_t *)(arena + space_index) = 0;
    *(uint32_t *)(arena + space_index + 4) = current_index;
    *(uint32_t *)(arena + space_index + 8) = size;

    // update previous block next index
    *(uint32_t *)(arena + current_index) = space_index;

    printf("%d\n", space_index + MANAGING_SIZE);
}

void parse_command(char *cmd)
{
    const char *delims = " \n";

    char *cmd_name = strtok(cmd, delims);
    if (!cmd_name)
        goto invalid_command;

    if (strcmp(cmd_name, "INITIALIZE") == 0)
    {
        char *size_str = strtok(NULL, delims);
        if (!size_str)
            goto invalid_command;
        uint32_t size = atoi(size_str);

        initialize(size);
    }
    else if (strcmp(cmd_name, "FINALIZE") == 0)
    {
        finalize();
    }
    else if (strcmp(cmd_name, "DUMP") == 0)
    {
        dump();
    }
    else if (strcmp(cmd_name, "ALLOC") == 0)
    {
        char *size_str = strtok(NULL, delims);
        if (!size_str)
            goto invalid_command;
        uint32_t size = atoi(size_str);

        alloc(size);
    }
    else if (strcmp(cmd_name, "FREE") == 0)
    {
        char *index_str = strtok(NULL, delims);
        if (!index_str)
            goto invalid_command;
        uint32_t index = atoi(index_str);

        // FREE
    }
    else if (strcmp(cmd_name, "FILL") == 0)
    {
        char *index_str = strtok(NULL, delims);
        if (!index_str)
            goto invalid_command;
        uint32_t index = atoi(index_str);

        char *size_str = strtok(NULL, delims);
        if (!size_str)
            goto invalid_command;
        uint32_t size = atoi(size_str);

        char *value_str = strtok(NULL, delims);
        if (!value_str)
            goto invalid_command;
        uint32_t value = atoi(value_str);

        // FILL
    }
    else if (strcmp(cmd_name, "ALLOCALIGNED") == 0)
    {
        char *size_str = strtok(NULL, delims);
        if (!size_str)
            goto invalid_command;
        uint32_t size = atoi(size_str);

        char *align_str = strtok(NULL, delims);
        if (!align_str)
            goto invalid_command;
        uint32_t align = atoi(align_str);

        // ALLOCALIGNED
    }
    else if (strcmp(cmd_name, "REALLOC") == 0)
    {
        printf("Found cmd REALLOC\n");

        char *index_str = strtok(NULL, delims);
        if (!index_str)
            goto invalid_command;
        uint32_t index = atoi(index_str);

        char *size_str = strtok(NULL, delims);
        if (!size_str)
            goto invalid_command;
        uint32_t size = atoi(size_str);

        // REALLOC
    }
    else
        goto invalid_command;

    return;

invalid_command:
    finalize();
    printf("Invalid command: %s\n", cmd);
    exit(1);
}

int main(void)
{
    ssize_t read;
    char *line = NULL;
    size_t len;

    while ((read = getline(&line, &len, stdin)) != -1)
    {
        printf("%s", line);
        parse_command(line);
    }

    free(line);
    return 0;
}
