#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#define MAX_POOL_SIZE 1000000
struct MemoryBlock
{
    int start;
    int block_size;
    int allocation_id;
    int allocation_status;
};

unsigned char *memory = NULL;
struct MemoryBlock *memory_map = NULL;
int pool_size, allocation_id = 1, alloc_strategy = -1, block_count = 1, capacity = 1;

void clear_input_buffer()
{
    int character;
    while((character = getchar()) != '\n' && (character != EOF));
}

void initialize_memory_block(int block_index, int alloc_id,int alloc_status, int block_size, int start)
{
    memory_map[block_index].allocation_id = alloc_id;
    memory_map[block_index].allocation_status = alloc_status;
    memory_map[block_index].block_size = block_size;
    memory_map[block_index].start = start;
}

bool check_external_fragmentation(int requested_size)
{
    int total_free = 0;
    int largest_free = 0;
    for(int i = 0; i < block_count; i++)
    {
        if(memory_map[i].allocation_status == 0)
        {
            total_free += memory_map[i].block_size;

            if(memory_map[i].block_size > largest_free)
            {
                largest_free = memory_map[i].block_size;
            }
        }
    }
    if(total_free >= requested_size && largest_free < requested_size)
    {
        printf("\nExternal Fragmentation Detected!\n");
        printf("Requested memory: %d bytes\n", requested_size);
        printf("Total free memory: %d bytes\n", total_free);
        printf("Largest free block: %d bytes\n", largest_free);
        printf("Individual free block sizes:\n");
        for(int i = 0; i < block_count; i++)
        {
            if(memory_map[i].allocation_status == 0)
            {
                printf("%d bytes\n", memory_map[i].block_size);
            }
        }
        return true;
    }
    return false;
}

bool first_fit_allocation(int requested_size)
{
    if(memory_map == NULL) return false;
    int free_block_index = -1;
    for(int current_block = 0; current_block <= block_count - 1; current_block++)
    {
        if((memory_map[current_block].allocation_status == 0) && (memory_map[current_block].block_size >= requested_size))
        {
            free_block_index = current_block;
            break;
        }
    }
    if(free_block_index == -1)
    {
        if(check_external_fragmentation(requested_size))
        {
            printf("Allocation failed due to external fragmentation!\n");
        }
        else
        {
            printf("Insufficient free memory!\n");
        }
    return false;
    }
    int remaining_block_memory = memory_map[free_block_index].block_size - requested_size;
    if((remaining_block_memory != 0) && (block_count == capacity))
    {
        struct MemoryBlock *temp = realloc(memory_map, (capacity * 2) * sizeof *memory_map);
        if(temp == NULL)
        {
            printf("Error: Memory reallocation failed!\n");
            return false;
        }
        memory_map = temp;
        capacity = capacity * 2;
    }
    if(remaining_block_memory != 0)
    {
        for(int block = block_count; block > free_block_index; block--)
        {
            memory_map[block] = memory_map[block - 1];
        }
        int starting_offset = memory_map[free_block_index].start;
        initialize_memory_block(free_block_index, allocation_id++, 1, requested_size, starting_offset);
        initialize_memory_block(free_block_index + 1, -1, 0,remaining_block_memory, starting_offset + requested_size);
        block_count++;
    }

    else{
        initialize_memory_block(free_block_index, allocation_id++, 1, requested_size, memory_map[free_block_index].start);
    }
    printf("Allocation successful. ID: %d, Start: %d, Size: %d\n", memory_map[free_block_index].allocation_id, memory_map[free_block_index].start, memory_map[free_block_index].block_size);
    return true;
}

bool best_fit_allocation(int requested_size)
{
    if(memory_map == NULL) return false;

    int best_index = -1, best_block_size = pool_size + 1;
    for(int current_block = 0; current_block <= block_count - 1; current_block++)
    {
        if((memory_map[current_block].allocation_status == 0) && (memory_map[current_block].block_size >= requested_size) && (memory_map[current_block].block_size < best_block_size))
        {
            best_index = current_block;
            best_block_size = memory_map[current_block].block_size;
        }
    }

    if(best_index == -1)
    {
        if(check_external_fragmentation(requested_size))
        {
            printf("Allocation failed due to external fragmentation!\n");
        }
        else
        {
            printf("Insufficient free memory!\n");
        }
    return false;
    }
    int remaining_block_memory = memory_map[best_index].block_size - requested_size;
    if((remaining_block_memory != 0) && (block_count == capacity))
    {
        struct MemoryBlock *temp = realloc(memory_map, (capacity * 2) * sizeof *memory_map);
        if(temp == NULL)
        {
            printf("Error: Memory reallocation failed!\n");
            return false;
        }
        memory_map = temp;
        capacity = capacity * 2;
    }
    if(remaining_block_memory != 0)
    {
        for(int block = block_count; block > best_index; block--)
        {
            memory_map[block] = memory_map[block - 1];
        }
        int starting_offset = memory_map[best_index].start;
        initialize_memory_block(best_index, allocation_id++, 1, requested_size, starting_offset);
        initialize_memory_block(best_index + 1, -1, 0,remaining_block_memory, starting_offset + requested_size);
        block_count++;
    }

    else{
        initialize_memory_block(best_index, allocation_id++, 1, requested_size, memory_map[best_index].start);
    }
    printf("Allocation successful. ID: %d, Start: %d, Size: %d\n", memory_map[best_index].allocation_id, memory_map[best_index].start, memory_map[best_index].block_size);
    return true;
}

bool worst_fit_allocation(int requested_size)
{
    if(memory_map == NULL) return false;

    int worst_index = -1, worst_block_size = -1;
    for(int current_block = 0; current_block <= block_count - 1; current_block++)
    {
        if((memory_map[current_block].allocation_status == 0) && (memory_map[current_block].block_size >= requested_size) && (memory_map[current_block].block_size >= worst_block_size))
        {
            worst_index = current_block;
            worst_block_size = memory_map[current_block].block_size;
        }
    }

    if(worst_index == -1)
    {
        if(check_external_fragmentation(requested_size))
        {
            printf("Allocation failed due to external fragmentation!\n");
        }
        else
        {
            printf("Insufficient free memory!\n");
        }
    return false;
    }
    int remaining_block_memory = memory_map[worst_index].block_size - requested_size;
    if((remaining_block_memory != 0) && (block_count == capacity))
    {
        struct MemoryBlock *temp = realloc(memory_map, (capacity * 2) * sizeof *memory_map);
        if(temp == NULL)
        {
            printf("Error: Memory reallocation failed!\n");
            return false;
        }
        memory_map = temp;
        capacity = capacity * 2;
    }
    if(remaining_block_memory != 0)
    {
        for(int block = block_count; block > worst_index; block--)
        {
            memory_map[block] = memory_map[block - 1];
        }
        int starting_offset = memory_map[worst_index].start;
        initialize_memory_block(worst_index, allocation_id++, 1, requested_size, starting_offset);
        initialize_memory_block(worst_index + 1, -1, 0,remaining_block_memory, starting_offset + requested_size);
        block_count++;
    }

    else{
        initialize_memory_block(worst_index, allocation_id++, 1, requested_size, memory_map[worst_index].start);
    }
    printf("Allocation successful. ID: %d, Start: %d, Size: %d\n", memory_map[worst_index].allocation_id, memory_map[worst_index].start, memory_map[worst_index].block_size);
    return true;
}

void allocate_memory()
{
    int requested_size;
    bool allocation_status;
    printf("Requested size: ");
    while(scanf("%d", &requested_size) != 1 || requested_size < 1 || requested_size > pool_size)
    {
        clear_input_buffer();
        printf("Invalid Requested size!\n");
        printf("Enter Requested size: ");
    }
    switch(alloc_strategy)
    {
        case 1:
            allocation_status = first_fit_allocation(requested_size);
            break;
        case 2:
            allocation_status = best_fit_allocation(requested_size);
            break;
        case 3:
            allocation_status = worst_fit_allocation(requested_size);
            break;
        default:
            allocation_status = false;
    }

    if(!allocation_status){
    printf("Memory allocation unsucessful!\n");
    }
}

void merge_free_blocks()
{
    int block = 0;
    while(block < block_count - 1)
    {
        if(memory_map[block].allocation_status == 0 && memory_map[block + 1].allocation_status == 0)
        {
            memory_map[block].block_size +=memory_map[block + 1].block_size;
            for(int j = block + 1; j < block_count - 1; j++)
            {
                memory_map[j] = memory_map[j + 1];
            }
            block_count--;
        }
        else
        {
            block++;
        }
    }
}


void release_memory()
{
    int release_id;
    printf("Enter allocation ID to release memory: ");
    while(scanf("%d", &release_id) != 1 || release_id < 1)
    {
        clear_input_buffer();
        printf("Invalid ID\n");
        printf("Enter ID: ");
    }
    int release_index = -1;
    for(int id = 0; id < block_count; id++)
    {
        if(memory_map[id].allocation_id == release_id &&
           memory_map[id].allocation_status == 1)
        {
            release_index = id;
            break;
        }
    }
    if(release_index == -1)
    {
        printf("Memory release unsuccessful! Invalid allocation ID.\n");
        return;
    }
    memory_map[release_index].allocation_id = -1;
    memory_map[release_index].allocation_status = 0;
    merge_free_blocks();
    printf("Memory released successfully!\n");
}

bool increase_capacity()
{
    struct MemoryBlock *temp = realloc(memory_map, capacity * 2 * sizeof *memory_map);
    if(temp == NULL)
    {
        printf("Metadata reallocation failed!\n");
        return false;
    }
    memory_map = temp;
    capacity *= 2;
    return true;
}


void resize_allocation()
{
    int alloc_id;
    printf("Enter allocation ID to resize memory: ");
    while(scanf("%d", &alloc_id) != 1 || alloc_id < 1)
    {
        clear_input_buffer();
        printf("Invalid ID\n");
        printf("Enter ID: ");
    }
    int block_idx = -1;
    for(int id = 0; id < block_count; id++)
    {
        if(memory_map[id].allocation_id == alloc_id && memory_map[id].allocation_status == 1)
        {
            block_idx = id;
            break;
        }
    } 
    if(block_idx == -1)
    {
        printf("Allocation ID not found!\nResize failed\n");
        return;
    }
    int new_size;
    printf("Enter New size: ");
    while(scanf("%d",&new_size) != 1 || new_size < 1 || new_size > pool_size)
    {
        clear_input_buffer();
        printf("Invalid size!\n");
        printf("Enter new size: ");
    }

    int old_size = memory_map[block_idx].block_size;
    int old_start = memory_map[block_idx].start;
    if(new_size == old_size)
    {
        printf("Allocation size is already the requested size.\n");
        return;
    }
    if(new_size < old_size)
    {
        if(block_count == capacity)
        {
        if(!increase_capacity())
            return;
        }

        int remaining = old_size - new_size;
        for(int j = block_count; j > block_idx + 1; j--)
        {
            memory_map[j] = memory_map[j - 1];
        }
        memory_map[block_idx].block_size = new_size;
        initialize_memory_block(block_idx + 1, -1, 0, remaining, old_start + new_size);

        block_count++;
        merge_free_blocks();
        printf("Allocation resized successfully!\n");
        return;
    }
    int extra_memory = new_size - old_size;

    if(block_idx + 1 < block_count && memory_map[block_idx + 1].allocation_status == 0 && memory_map[block_idx + 1].block_size >= extra_memory)
    {
        memory_map[block_idx].block_size = new_size;
        if(memory_map[block_idx + 1].block_size == extra_memory)
        {
            for(int j = block_idx + 1; j < block_count - 1; j++)
            {
                memory_map[j] = memory_map[j + 1];
            }

            block_count--;
        }
        else
        {
            memory_map[block_idx + 1].start += extra_memory;
            memory_map[block_idx + 1].block_size -= extra_memory;
        }

        printf("Allocation expanded successfully!\n");
        return;
    }
    int destination_idx = -1;
    int best_size = pool_size + 1;
    int worst_size = -1;

    for(int i = 0; i < block_count; i++)
    {
        if(memory_map[i].allocation_status == 0 &&
           memory_map[i].block_size >= new_size)
        {
            if(alloc_strategy == 1)
            {
                destination_idx = i;
                break;
            }
            else if(alloc_strategy == 2)
            {
                if(destination_idx == -1 || memory_map[i].block_size < best_size)
                {
                    destination_idx = i;
                    best_size = memory_map[i].block_size;
                }
            }
            else if(alloc_strategy == 3)
            {
                if(memory_map[i].block_size > worst_size)
                {
                    destination_idx = i;
                    worst_size = memory_map[i].block_size;
                }
            }
        }
    }
    if(destination_idx == -1)
    {
        printf("Resize failed! No suitable free block available.\n");
        return;
    }
    int destination_start = memory_map[destination_idx].start;
    int destination_remaining =
        memory_map[destination_idx].block_size - new_size;
    if(destination_remaining > 0 && block_count == capacity)
    {
        if(!increase_capacity())
            return;
    }
    memcpy(memory + destination_start, memory + old_start, old_size);
    if(destination_remaining > 0)
    {
        for(int j = block_count; j > destination_idx; j--)
        {
            memory_map[j] = memory_map[j - 1];
        }
        initialize_memory_block(destination_idx, alloc_id, 1, new_size, destination_start);
        initialize_memory_block(destination_idx + 1, -1, 0, destination_remaining, destination_start + new_size);
        block_count++;
    }
    else
    {
        initialize_memory_block(destination_idx, alloc_id, 1, new_size, destination_start);
    }
    for(int i = 0; i < block_count; i++)
    {
        if(memory_map[i].allocation_id == alloc_id &&
           memory_map[i].start == old_start)
        {
            memory_map[i].allocation_id = -1;
            memory_map[i].allocation_status = 0;
            break;
        }
    }
    merge_free_blocks();
    printf("Allocation moved and resized successfully!\n");
}
void display_memory_map()
{
    printf("\n========= MEMORY MAP ========\n");
    printf("%s %s %s %s\n","Start", "Size", "Allocation ID", "Status");
    for(int i = 0; i < block_count; i++)
    {
        printf("%d  %d  %d  %s\n", memory_map[i].start, memory_map[i].block_size,memory_map[i].allocation_id, memory_map[i].allocation_status == 1 ?"Allocated" : "Free");
    }
    printf("================================\n");
}

void display_memory_statistics()
{
    int total_allocated = 0, total_free = 0, allocated_blocks = 0;
    int free_blocks = 0, largest_free_block = 0;
    for(int i = 0; i < block_count; i++)
    {
        if(memory_map[i].allocation_status == 1)
        {
            total_allocated += memory_map[i].block_size;
            allocated_blocks++;
        }
        else
        {
            total_free += memory_map[i].block_size;
            free_blocks++;
            if(memory_map[i].block_size > largest_free_block)
            {
                largest_free_block = memory_map[i].block_size;
            }
        }
    }
    printf("\n======= MEMORY STAISTICS =======\n");
    printf("Total Pool Size       : %d bytes\n", pool_size);
    printf("Total Allocated       : %d bytes\n", total_allocated);
    printf("Total Free            : %d bytes\n", total_free);
    printf("Allocated Blocks      : %d\n", allocated_blocks);
    printf("Free Blocks           : %d\n", free_blocks);
    printf("Largest Free Block    : %d bytes\n", largest_free_block);
    printf("=================================\n");
}

void change_allocation_strategy()
{
    int new_strategy;
    printf("\nSelect Allocation Strategy:\n");
    printf("1. First Fit\n");
    printf("2. Best Fit\n");
    printf("3. Worst Fit\n");
    printf("Enter choice: ");
    while(scanf("%d", &new_strategy) != 1 || new_strategy < 1 || new_strategy > 3)
    {
        clear_input_buffer();
        printf("Invalid strategy!\n");
        printf("Enter choice: ");
    }
    alloc_strategy = new_strategy;
    printf("Allocation strategy changed successfully!\n");
    switch(alloc_strategy)
    {
        case 1:
            printf("Current Strategy: First Fit\n");
            break;

        case 2:
            printf("Current Strategy: Best Fit\n");
            break;

        case 3:
            printf("Current Strategy: Worst Fit\n");
            break;
    }
}

bool process_choice()
{
    int choice;
    while(scanf("%d", &choice) != 1 || choice < 1 || choice > 7)
    {
        clear_input_buffer();
        printf("Invalid Choice!\n");
        printf("Enter the choice: ");
    }

    switch(choice)
    {
        case 1: 
            allocate_memory();
            break;
        case 2:
            release_memory();
            break;
        case 3:
            resize_allocation();
            break;
        case 4:
            display_memory_map();
            break;
        case 5:
            display_memory_statistics();
            break;
        case 6:
            change_allocation_strategy();
            break;
        default:
            return false;
    }
    return true;
}


void show_menu()
{
    printf("1.Allocate Memory\n");
    printf("2.Release Memory\n");
    printf("3.Resize Allocation\n");
    printf("4.Display memory allocation\n");
    printf("5.Display memory statistics\n");
    printf("6.Change allocation strategy\n");
    printf("7.Exit\n\n");

    printf("Enter your choice: ");
}

bool get_primary_input()
{
    printf("Enter total pool size: ");
    while(scanf("%d", &pool_size) != 1 || pool_size < 1 || pool_size > MAX_POOL_SIZE)
    {
        clear_input_buffer();
        printf("Invalid size.\n");
        printf("Enter the pool size: ");
    }

    printf("\n1.First Fit\n2.Best Fit\n3.Worst Fit\nEnter allocation strategy\n");
    while(scanf("%d",&alloc_strategy) != 1 || (alloc_strategy < 1 || alloc_strategy > 3))
    {
        clear_input_buffer();
        printf("Error: Invalid allocation strategy!\n");
        printf("Enter allcoation strategy: ");
    }
    memory = malloc(pool_size);
    if(memory == NULL)
    {
        return false;
    }
    memory_map = malloc(sizeof *memory_map);
    if(memory_map == NULL)
    {
       free(memory);
       memory = NULL;
       return false;
    }
    initialize_memory_block(0, -1, 0, pool_size, 0);
    return true;
}

int main(void)
{
    if(!get_primary_input())
    {
        return 1;
    }
    while(true)
    {
        show_menu();
        if(!process_choice())
            break;
    }
    free(memory);
    free(memory_map);
    return 0;
}
