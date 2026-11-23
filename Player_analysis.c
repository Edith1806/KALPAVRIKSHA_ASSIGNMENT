#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "Players_data.h"

#define NO_TEAMS 10
#define MAX_ID 1000
#define MIN_PLAYERS 11
#define MAX_PLAYERS 50
#define NAME_LENGTH 50
#define INPUT_LENGTH 256

typedef enum
{
    BATSMAN,
    BOWLER,
    ALL_ROUNDER
} Role;

typedef struct Player
{
    int player_id;
    char player_name[NAME_LENGTH];
    char team_name[NAME_LENGTH];
    Role role;
    int total_runs;
    float batting_avg;
    float strike_rate;
    int wickets;
    float economy_rate;
    float performance_index;
    struct Player *next;
} MyPlayer;

typedef struct Team
{
    int teamId;
    char team_name[NAME_LENGTH];
    int total_player_count;
    int batting_player_count;
    float batting_sr_sum;
    float avg_bat_strike_rate;
    MyPlayer *batsman_list;
    MyPlayer *bowler_list;
    MyPlayer *all_rounder_list;
} Team;

Team team_list[NO_TEAMS];

typedef struct
{
    MyPlayer *player;
    int team_idx;
} HeapNode;

typedef struct
{
    HeapNode nodes[NO_TEAMS];
    int size;
} MaxHeap;
int read_int_range(const char *prompt, int min_value, int max_value)
{
    char input[INPUT_LENGTH];
    int value;
    char extra;
    while (1)
    {
        printf("%s", prompt);
        if (fgets(input, sizeof(input), stdin) == NULL)
            return -1;

        if (sscanf(input, "%d %c", &value, &extra) == 1 && value >= min_value && value <= max_value)
        {
            return value;
        }

        printf("Invalid input. Enter a value between %d and %d.\n", min_value, max_value);
    }
}
float read_float_range(const char *prompt, float min_value, float max_value)
{
    char input[INPUT_LENGTH];
    float value;
    char extra;
    while (1)
    {
        printf("%s", prompt);
        if (fgets(input, sizeof(input), stdin) == NULL)
            return -1.0f;
        if (sscanf(input, "%f %c", &value, &extra) == 1 &&
            value >= min_value && value <= max_value)
        {
            return value;
        }
        printf("Invalid input. Enter a value between %.2f and %.2f.\n", min_value, max_value);
    }
}

bool read_nonempty_string(const char *prompt, char *out, size_t out_size)
{
    while (1)
    {
        printf("%s", prompt);
        if (fgets(out, (int)out_size, stdin) == NULL)
            return false;

        out[strcspn(out, "\n\r")] = '\0';
        if (out[0] != '\0')
            return true;

        printf("Input cannot be empty. Please try again.\n");
    }
}

void initialize_teams(void)
{
    for (int i = 0; i < NO_TEAMS; i++)
    {
        team_list[i].teamId = i + 1;
        strcpy(team_list[i].team_name, teams[i]);
        team_list[i].batsman_list = NULL;
        team_list[i].bowler_list = NULL;
        team_list[i].all_rounder_list = NULL;
        team_list[i].total_player_count = 0;
        team_list[i].batting_player_count = 0;
        team_list[i].batting_sr_sum = 0.0f;
        team_list[i].avg_bat_strike_rate = 0.0f;
    }
}

bool role_parse(const char *role, Role *role_out)
{
    if (role == NULL || role_out == NULL) return false;
    if (strcmp(role, "Batsman") == 0)
    {
        *role_out = BATSMAN;
        return true;
    }
    if (strcmp(role, "Bowler") == 0)
    {
        *role_out = BOWLER;
        return true;
    }
    if (strcmp(role, "All-rounder") == 0)
    {
        *role_out = ALL_ROUNDER;
        return true;
    }
    return false;
}

const char *role_name(Role role)
{
    switch (role)
    {
        case BATSMAN: return "Batsman";
        case BOWLER: return "Bowler";
        case ALL_ROUNDER: return "All-rounder";
        default: return "Unknown";
    }
}

float calculate_performance_index(MyPlayer *player)
{
    if (player == NULL) return 0.0f;
    switch (player->role)
    {
        case BATSMAN:
            return (player->batting_avg * player->strike_rate) / 100.0f;
        case BOWLER:
            return (player->wickets * 2.0f) + (100.0f - player->economy_rate);
        case ALL_ROUNDER:
            return ((player->batting_avg * player->strike_rate) / 100.0f) + player->wickets * 2.0f;
        default:
            return 0.0f;
    }
}

int find_team_name(const char *name)
{
    if (name == NULL) return -1;
    int low = 0, high = NO_TEAMS - 1;
    while (low <= high)
    {
        int mid = low + (high - low) / 2;
        int cmp = strcmp(team_list[mid].team_name, name);
        if (cmp == 0) return mid;
        if (cmp < 0) low = mid + 1;
        else high = mid - 1;
    }
    return -1;
}

int find_team_id(int id)
{
    if (id < 1 || id > NO_TEAMS) return -1;
    return id - 1;
}

MyPlayer *createPlayer(const Player *source)
{
    if (source == NULL) return NULL;
    MyPlayer *new_player = malloc(sizeof(*new_player));
    if (new_player == NULL) return NULL;
    new_player->player_id = source->id;
    new_player->batting_avg = source->battingAverage;
    new_player->economy_rate = source->economyRate;
    snprintf(new_player->player_name, sizeof(new_player->player_name), "%s", source->name);
    new_player->strike_rate = source->strikeRate;
    snprintf(new_player->team_name, sizeof(new_player->team_name), "%s", source->team);
    new_player->total_runs = source->totalRuns;
    new_player->wickets = source->wickets;
    if (!role_parse(source->role, &new_player->role))
    {
        free(new_player);
        return NULL;
    }
    new_player->performance_index = calculate_performance_index(new_player);
    new_player->next = NULL;
    return new_player;
}

void insert_players(MyPlayer **head, MyPlayer *new_player)
{
    if (head == NULL || new_player == NULL) return;
    if (*head == NULL || new_player->performance_index > (*head)->performance_index)
    {
        new_player->next = *head;
        *head = new_player;
        return;
    }
    MyPlayer *current = *head;
    while (current->next != NULL &&
           current->next->performance_index >= new_player->performance_index)
        current = current->next;
    new_player->next = current->next;
    current->next = new_player;
}

MyPlayer **get_role_list_ptr(Team *team, Role role)
{
    if (team == NULL) return NULL;
    switch (role)
    {
        case BATSMAN: return &team->batsman_list;
        case BOWLER: return &team->bowler_list;
        case ALL_ROUNDER: return &team->all_rounder_list;
        default: return NULL;
    }
}

MyPlayer *get_role_list(Team *team, Role role)
{
    MyPlayer **ptr = get_role_list_ptr(team, role);
    return ptr == NULL ? NULL : *ptr;
}

bool build_team(void)
{
    for (int i = 0; i < playerCount; i++)
    {
        int team_idx = find_team_name(players[i].team);
        if (team_idx == -1) return false;
        MyPlayer *p = createPlayer(&players[i]);
        if (p == NULL) return false;
        MyPlayer **role_list = get_role_list_ptr(&team_list[team_idx], p->role);
        if (role_list == NULL)
        {
            free(p);
            return false;
        }
        insert_players(role_list, p);
        if (p->role != BOWLER)
        {
            team_list[team_idx].batting_player_count++;
            team_list[team_idx].batting_sr_sum += p->strike_rate;
            team_list[team_idx].avg_bat_strike_rate =
                team_list[team_idx].batting_sr_sum / team_list[team_idx].batting_player_count;
        }
        team_list[team_idx].total_player_count++;
    }
    return true;
}

void display_players_list(MyPlayer *list, int limit)
{
    if (list == NULL)
    {
        printf("No players found in this role.\n");
        return;
    }
    MyPlayer *current = list;
    int displayed = 0;
    while (current != NULL && (limit == -1 || displayed < limit))
    {
        printf("%-25s PI: %8.2f | Runs: %-5d Avg: %7.2f SR: %7.2f Wickets: %-3d Econ: %6.2f\n",
               current->player_name, current->performance_index, current->total_runs,
               current->batting_avg, current->strike_rate, current->wickets, current->economy_rate);
        current = current->next;
        displayed++;
    }
}

void display_team_players(const char *teamName)
{
    int team_idx = find_team_name(teamName);
    if (team_idx == -1)
    {
        printf("Team name not found!\n");
        return;
    }
    Team *team = &team_list[team_idx];
    printf("\n===============================================================\n");
    printf("Team Name: %s | Team ID: %d\n", team->team_name, team->teamId);
    printf("Total Players: %d\n", team->total_player_count);
    printf("Average Batting Strike Rate: %.2f\n", team->avg_bat_strike_rate);
    printf("===============================================================\n");
    printf("--- Batsmen ---\n"); display_players_list(team->batsman_list, -1);
    printf("--- Bowlers ---\n"); display_players_list(team->bowler_list, -1);
    printf("--- All-rounders ---\n"); display_players_list(team->all_rounder_list, -1);
}

void display_teams_by_strike_rate(void)
{
    Team sorted[NO_TEAMS];
    memcpy(sorted, team_list, sizeof(team_list));
    for (int i = 0; i < NO_TEAMS - 1; i++)
    {
        int best = i;
        for (int j = i + 1; j < NO_TEAMS; j++)
            if (sorted[j].avg_bat_strike_rate > sorted[best].avg_bat_strike_rate)
                best = j;
        if (best != i)
        {
            Team temp = sorted[i]; sorted[i] = sorted[best]; sorted[best] = temp;
        }
    }
    printf("\nTeams ranked by average batting strike rate (descending)\n");
    printf("-----------------------------------------------------\n");
    for (int i = 0; i < NO_TEAMS; i++)
        printf("%2d. %-18s Avg SR: %.2f | Players: %d\n", i + 1, sorted[i].team_name,
               sorted[i].avg_bat_strike_rate, sorted[i].total_player_count);
}

void display_top_K_players(const char *team, Role role, int k)
{
    if (k < 1)
    {
        printf("K must be positive.\n");
        return;
    }
    int team_idx = find_team_name(team);
    if (team_idx == -1)
    {
        printf("Team not found!\n");
        return;
    }
    MyPlayer *list = get_role_list(&team_list[team_idx], role);
    printf("\nTop %d %s from %s\n", k, role_name(role), team);
    display_players_list(list, k);
}

void swap_heap_nodes(HeapNode *a, HeapNode *b)
{
    HeapNode temp = *a; *a = *b; *b = temp;
}

void heap_insert(MaxHeap *heap, MyPlayer *player, int team_idx)
{
    if (heap == NULL || player == NULL || heap->size >= NO_TEAMS) return;
    int idx = heap->size++;
    heap->nodes[idx].player = player;
    heap->nodes[idx].team_idx = team_idx;
    while (idx > 0)
    {
        int parent = (idx - 1) / 2;
        if (heap->nodes[parent].player->performance_index >=
            heap->nodes[idx].player->performance_index) break;
        swap_heap_nodes(&heap->nodes[parent], &heap->nodes[idx]);
        idx = parent;
    }
}

bool heap_extract_max(MaxHeap *heap, HeapNode *out)
{
    if (heap == NULL || out == NULL || heap->size == 0) return false;
    *out = heap->nodes[0];
    heap->size--;
    if (heap->size == 0) return true;
    heap->nodes[0] = heap->nodes[heap->size];
    int idx = 0;
    for (;;)
    {
        int left = 2 * idx + 1, right = 2 * idx + 2, largest = idx;
        if (left < heap->size && heap->nodes[left].player->performance_index >
            heap->nodes[largest].player->performance_index) largest = left;
        if (right < heap->size && heap->nodes[right].player->performance_index >
            heap->nodes[largest].player->performance_index) largest = right;
        if (largest == idx) break;
        swap_heap_nodes(&heap->nodes[idx], &heap->nodes[largest]);
        idx = largest;
    }
    return true;
}

void display_players_by_role(Role role)
{
    if (role != BATSMAN && role != BOWLER && role != ALL_ROUNDER)
    {
        printf("Invalid role.\n"); return;
    }
    MaxHeap heap = { .size = 0 };
    for (int t = 0; t < NO_TEAMS; t++)
    {
        MyPlayer *head = get_role_list(&team_list[t], role);
        if (head != NULL) heap_insert(&heap, head, t);
    }
    printf("\nAll %s players across teams, sorted by PI descending\n", role_name(role));
    printf("=======================================================\n");
    HeapNode item;
    while (heap_extract_max(&heap, &item))
    {
        MyPlayer *p = item.player;
        printf("%-25s %-18s PI: %8.2f\n", p->player_name, p->team_name, p->performance_index);
        MyPlayer *next = p->next;
        if (next != NULL) heap_insert(&heap, next, item.team_idx);
    }
}

bool player_id_exists(int id)
{
    for (int t = 0; t < NO_TEAMS; t++)
    {
        MyPlayer *lists[] = { team_list[t].batsman_list, team_list[t].bowler_list, team_list[t].all_rounder_list };
        for (int r = 0; r < 3; r++)
            for (MyPlayer *p = lists[r]; p != NULL; p = p->next)
                if (p->player_id == id) return true;
    }
    return false;
}

bool add_player_to_team(void)
{
    printf("\n--- Add Player ---\n");
    int team_id = read_int_range("Enter Team ID (1-10): ", 1, NO_TEAMS);
    if (team_id < 1) return false;
    int team_idx = find_team_id(team_id);
    Team *team = &team_list[team_idx];
    if (team->total_player_count >= MAX_PLAYERS)
    {
        printf("This team has reached its maximum player limit (%d).\n", MAX_PLAYERS);
        return true;
    }

    int id;
    do
    {
        id = read_int_range("Enter new Player ID (1-1000): ", 1, MAX_ID);
        if (id < 1) return false;
        if (player_id_exists(id)) printf("That Player ID already exists. Try another ID.\n");
    } while (player_id_exists(id));

    MyPlayer *p = calloc(1, sizeof(*p));
    if (p == NULL)
    {
        printf("Memory allocation failed. Player not added.\n");
        return false;
    }
    p->player_id = id;
    if (!read_nonempty_string("Enter player name: ", p->player_name, sizeof(p->player_name)))
    {
        free(p); return false;
    }
    snprintf(p->team_name, sizeof(p->team_name), "%s", team->team_name);

    printf("Select Role: 1. Batsman  2. Bowler  3. All-rounder\n");
    int role_choice = read_int_range("Enter role (1-3): ", 1, 3);
    if (role_choice < 1)
    {
        free(p);
        return false;
    }
    p->role = (Role)(role_choice - 1);

    if (p->role != BOWLER)
    {
        p->total_runs = read_int_range("Total runs (0-100000): ", 0, 100000);
        if (p->total_runs < 0)
        {
            free(p);
            return false;
        }
        p->batting_avg = read_float_range("Batting average (0-500): ", 0.0f, 500.0f);
        if (p->batting_avg < 0)
        {
            free(p);
            return false;
        }
        p->strike_rate = read_float_range("Strike rate (0-1000): ", 0.0f, 1000.0f);
        if (p->strike_rate < 0)
        {
            free(p);
            return false;
        }
    }
    if (p->role != BATSMAN)
    {
        p->wickets = read_int_range("Wickets (0-1000): ", 0, 1000);
        if (p->wickets < 0)
        {
            free(p);
            return false;
        }
        p->economy_rate = read_float_range("Economy rate (0-100): ", 0.0f, 100.0f);
        if (p->economy_rate < 0)
        {
            free(p);
            return false;
        }
    }
    p->performance_index = calculate_performance_index(p);
    p->next = NULL;
    MyPlayer **list = get_role_list_ptr(team, p->role);
    if (list == NULL)
    {
        free(p);
        printf("Invalid role; player not added.\n");
        return false;
    }
    insert_players(list, p);
    if (p->role != BOWLER)
    {
        team->batting_player_count++;
        team->batting_sr_sum += p->strike_rate;
        team->avg_bat_strike_rate = team->batting_sr_sum / team->batting_player_count;
    }
    team->total_player_count++;
    printf("Player added successfully! PI: %.2f\n", p->performance_index);
    return true;
}

Role get_role_from_menu(void)
{
    printf("\nSelect role:\n1. Batsman\n2. Bowler\n3. All-rounder\n");
    int choice = read_int_range("Enter choice (1-3): ", 1, 3);
    if (choice < 1) return (Role)-1;
    return (Role)(choice - 1);
}

void menu(void)
{
    while(true)
    {
        printf("\n========== ICC ODI PLAYER PERFORMANCE ANALYZER =========\n");
        printf("1. Add player to a team\n");
        printf("2. Display all players of a team\n");
        printf("3. Display teams ranked by average batting strike rate\n");
        printf("4. Top K players in a team by role\n");
        printf("5. Display all players across teams by role (PI descending)\n");
        printf("6. Exit\n");
        int choice = read_int_range("Enter menu choice (1-6): ", 1, 6);
        if (choice < 1) return;

        switch (choice)
        {
            case 1:
                add_player_to_team();
                break;
            case 2:
            {
                int team_id = read_int_range("Enter Team ID (1-10): ", 1, NO_TEAMS);
                if (team_id >= 1) display_team_players(team_list[team_id - 1].team_name);
                break;
            }
            case 3:
                display_teams_by_strike_rate();
                break;
            case 4:
            {
                int team_id = read_int_range("Enter Team ID (1-10): ", 1, NO_TEAMS);
                if (team_id < 1) break;
                Role role = get_role_from_menu();
                if ((int)role < 0) break;
                int k = read_int_range("Enter K (1-50): ", 1, MAX_PLAYERS);
                if (k >= 1) display_top_K_players(team_list[team_id - 1].team_name, role, k);
                break;
            }
            case 5:
            {
                Role role = get_role_from_menu();
                if ((int)role >= 0) display_players_by_role(role);
                break;
            }
            case 6:
                printf("Exiting analyzer.\n");
                return;
        }
    }
}

void free_player_list(MyPlayer *head)
{
    while (head != NULL)
    {
        MyPlayer *next = head->next;
        free(head);
        head = next;
    }
}
void free_all_players(void)
{
    for (int t = 0; t < NO_TEAMS; t++)
    {
        free_player_list(team_list[t].batsman_list);
        free_player_list(team_list[t].bowler_list);
        free_player_list(team_list[t].all_rounder_list);
        team_list[t].batsman_list = NULL;
        team_list[t].bowler_list = NULL;
        team_list[t].all_rounder_list = NULL;
    }
}
int main(void)
{
    initialize_teams();
    printf("Teams initialized.\n");
    if (!build_team())
    {
        printf("Initial team construction failed.\n");
        free_all_players();
        return 1;
    }
    printf("Initial teams built successfully.\n");
    menu();
    free_all_players();
    return 0;
}