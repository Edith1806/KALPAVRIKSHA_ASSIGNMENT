#include<stdio.h>
#include<string.h>
#include<stdbool.h>
#include<stdlib.h>

#define NAME_SIZE 30
#define DESIGNATION_SIZE 20

enum InputStatus
{
    INPUT_ENDED = -1,
    INPUT_INVALID = 0,
    INPUT_OK = 1
};
struct Employee;
struct Node
{
    struct Employee *employee_pointer;
    struct Node *next;
};

struct Employee
{
    int employee_id;
    char employee_name[NAME_SIZE];
    char emp_designation[DESIGNATION_SIZE];
    struct Node *reports;
    int reporting_manager_id;
};

struct Node *employee_list = NULL;
void clear_input_buffer(void)
{
    int character;
    while((character = getchar()) != '\n' && character != EOF)
    {
    }
}

struct Node *find_employee_node(int employee_id)
{
    struct Node *current_node = employee_list;
    while(current_node != NULL)
    {
        if(current_node->employee_pointer->employee_id == employee_id)
        {
            return current_node;
        }
        current_node = current_node->next;
    }
    return NULL;
}

void free_all_employees(void)
{
    while(employee_list != NULL)
    {
        struct Node *main_node = employee_list;
        employee_list = main_node->next;
        struct Node *report = main_node->employee_pointer->reports;
        while(report != NULL)
        {
            struct Node *next_report = report->next;
            free(report);
            report = next_report;
        }
        free(main_node->employee_pointer);
        free(main_node);
    }
}

bool read_line(char *buffer, size_t size)
{
    if(fgets(buffer, size, stdin) == NULL)
    {
        buffer[0] = '\0';
        return false;
    }
    char *newline = strchr(buffer, '\n');
    if(newline != NULL)
    {
        *newline = '\0';
    }
    else
    {
        clear_input_buffer();
    }
    return true;
}

bool read_positive_int(const char *prompt, int *value)
{
    printf("%s", prompt);
    while(true)
    {
        int result = scanf("%d", value);
        if(result == EOF)
        {
            return false;
        }
        if(result == 1 && *value >= 1)
        {
            clear_input_buffer();
            return true;
        }
        printf("Must enter a valid positive number!\n");
        clear_input_buffer();
        printf("%s", prompt);
    }
}

int get_employee_details(struct Employee *employee_details)
{
    if(!read_positive_int("Enter the employee ID: ", &employee_details->employee_id))
    {
        return INPUT_ENDED;
    }

    if(find_employee_node(employee_details->employee_id) != NULL)
    {
        printf("Employee ID already registered!\n");
        return INPUT_INVALID;
    }
    printf("Enter Name: ");
    if(!read_line(employee_details->employee_name, sizeof(employee_details->employee_name)))
    {
        return INPUT_ENDED;
    }
    if(employee_details->employee_name[0] == '\0')
    {
        printf("Name must not be empty!\n");
        return INPUT_INVALID;
    }
    printf("Enter Designation: ");
    if(!read_line(employee_details->emp_designation, sizeof(employee_details->emp_designation)))
    {
        return INPUT_ENDED;
    }
    if(employee_details->emp_designation[0] == '\0')
    {
        printf("Designation should not be empty!\n");
        return INPUT_INVALID;
    }
    bool has_manager = false;
    while(true)
    {
        char answer[8];
        printf("Do you have a reporting manager [Y/N]: ");
        if(!read_line(answer, sizeof(answer)))
        {
            return INPUT_ENDED;
        }
        if(answer[0] == 'Y' || answer[0] == 'y')
        {
            has_manager = true;
            break;
        }
        if(answer[0] == 'N' || answer[0] == 'n')
        {
            break;
        }
        printf("Please enter Y or N!\n");
    }

    if(has_manager)
    {
        if(!read_positive_int("Enter the reporting manager ID: ", &employee_details->reporting_manager_id))
        {
            return INPUT_ENDED;
        }
        if(find_employee_node(employee_details->reporting_manager_id) == NULL)
        {
            printf("Reporting manager ID must be a valid Employee ID!\n");
            return INPUT_INVALID;
        }
    }
    else
    {
        employee_details->reporting_manager_id = -1;
    }
    return INPUT_OK;
}

void append_report(struct Employee *manager, struct Node *report_node)
{
    report_node->next = NULL;
    if(manager->reports == NULL)
    {
        manager->reports = report_node;
        return;
    }
    struct Node *tail = manager->reports;
    while(tail->next != NULL)
    {
        tail = tail->next;
    }
    tail->next = report_node;
}

bool add_employee_record(struct Employee details)
{
    struct Employee *new_employee = (struct Employee *) malloc(sizeof(struct Employee));
    if(new_employee == NULL)
    {
        printf("System error: Memory allocation failed!\n");
        return false;
    }
    *new_employee = details;
    new_employee->reports = NULL;
    struct Node *new_node = (struct Node *) malloc(sizeof(struct Node));
    if(new_node == NULL)
    {
        printf("System error: Memory allocation failed for node structure.\n");
        free(new_employee);
        return false;
    }
    struct Node *new_report = NULL;
    struct Node *manager_node = NULL;
    if(details.reporting_manager_id != -1)
    {
        manager_node = find_employee_node(details.reporting_manager_id);
        if(manager_node == NULL)
        {
            printf("Reporting manager not found in the main list!\n");
            free(new_node);
            free(new_employee);
            return false;
        }
        new_report = (struct Node *) malloc(sizeof(struct Node));
        if(new_report == NULL)
        {
            printf("System error: Memory allocation failed for report structure.\n");
            free(new_node);
            free(new_employee);
            return false;
        }
    }
    new_node->employee_pointer = new_employee;
    new_node->next = employee_list;
    employee_list = new_node;

    if(new_report != NULL)
    {
        new_report->employee_pointer = new_employee;
        append_report(manager_node->employee_pointer, new_report);
    }
    return true;
}

bool add_employee(void)
{
    struct Employee employee_details;
    int status = get_employee_details(&employee_details);
    while(status == INPUT_INVALID)
    {
        printf("Must enter valid employee details only\n");
        status = get_employee_details(&employee_details);
    }
    if(status == INPUT_ENDED)
    {
        return false;
    }
    if(add_employee_record(employee_details))
    {
        printf("Employee added successfully!\n");
    }
    else
    {
        printf("Failed to add employee!\n");
    }
    return true;
}

void unlink_from_reports(struct Employee *manager, struct Employee *target)
{
    struct Node *previous = NULL;
    struct Node *current = manager->reports;
    while(current != NULL)
    {
        if(current->employee_pointer == target)
        {
            if(previous == NULL)
            {
                manager->reports = current->next;
            }
            else
            {
                previous->next = current->next;
            }
            free(current);
            return;
        }
        previous = current;
        current = current->next;
    }
}

int count_direct_reports_recursive(struct Node *report)
{
    if(report == NULL)
    {
        return 0;
    }
    return 1 + count_direct_reports_recursive(report->next);
}

bool remove_employee(void)
{
    int employee_id;
    if(!read_positive_int("Enter the employee ID to remove: ", &employee_id))
    {
        return false;
    }
    struct Node *employee_node = find_employee_node(employee_id);
    if(employee_node == NULL)
    {
        printf("Employee not found!\n");
        return true;
    }
    struct Employee *removed = employee_node->employee_pointer;
    if(removed->reporting_manager_id == -1)
    {
        printf("The CEO cannot be removed!\n");
        return true;
    }

    struct Node *manager_node = find_employee_node(removed->reporting_manager_id);
    if(manager_node == NULL)
    {
        printf("Report manager not found in the main list.\n");
        return true;
    }
    struct Employee *manager = manager_node->employee_pointer;
    unlink_from_reports(manager, removed);
    struct Employee *receiver = manager;
    bool receiver_is_sibling = false;
    int fewest_reports = 0;
    struct Node *sibling = manager->reports;
    while(sibling != NULL)
    {
        int sibling_reports = count_direct_reports_recursive(sibling->employee_pointer->reports);
        if(!receiver_is_sibling || sibling_reports < fewest_reports)
        {
            receiver = sibling->employee_pointer;
            receiver_is_sibling = true;
            fewest_reports = sibling_reports;
        }
        sibling = sibling->next;
    }

    int moved_reports = 0;
    struct Node *report = removed->reports;
    while(report != NULL)
    {
    struct Node *next_report = report->next;
    report->employee_pointer->reporting_manager_id = receiver->employee_id;
    append_report(receiver, report);
    moved_reports++;
    report = next_report;
    }
    removed->reports = NULL;

    struct Node *previous_main = NULL;
    struct Node *current_main = employee_list;
    while(current_main != employee_node)
    {
        previous_main = current_main;
        current_main = current_main->next;
    }
    if(previous_main == NULL)
    {
        employee_list = employee_node->next;
    }
    else
    {
        previous_main->next = employee_node->next;
    }
    free(removed);
    free(employee_node);
    printf("Employee removed successfully!\n");
    if(moved_reports > 0)
    {
        if(receiver_is_sibling)
        {
            printf("%d direct reports transferred to employee %d (sibling with the fewest direct reports).\n", moved_reports, receiver->employee_id);
        }
        else
        {
            printf("%d direct reports transferred to their superior, employee %d.\n", moved_reports, receiver->employee_id);
        }
    }
    return true;
}

void print_employee_line(struct Employee *employee, int depth)
{
    printf("%*s%d - %s - %s\n", depth * 4, "", employee->employee_id, employee->employee_name, employee->emp_designation);
}

void print_reports_recursive(struct Node *report, int depth)
{
    if(report == NULL)
    {
        return;
    }
    print_employee_line(report->employee_pointer, depth);
    print_reports_recursive(report->employee_pointer->reports, depth + 1);
    print_reports_recursive(report->next, depth);
}

int count_reports_recursive(struct Node *report)
{
    if(report == NULL)
    {
        return 0;
    }
    return 1 + count_reports_recursive(report->employee_pointer->reports) + count_reports_recursive(report->next);
}

bool employee_hierarchy(void)
{
    int employee_id;
    if(!read_positive_int("Enter the employee ID: ", &employee_id))
    {
        return false;
    }
    struct Node *employee_node = find_employee_node(employee_id);
    if(employee_node == NULL)
    {
        printf("Employee not found!\n");
        return true;
    }
    struct Employee *employee = employee_node->employee_pointer;
    printf("\n");
    print_employee_line(employee, 0);
    print_reports_recursive(employee->reports, 1);
    printf("\nTotal Employees Under %d: %d\n\n", employee_id, count_reports_recursive(employee->reports));
    return true;
}

void print_top_level_recursive(struct Node *main_node)
{
    if(main_node == NULL)
    {
        return;
    }
    print_top_level_recursive(main_node->next);
    struct Employee *employee = main_node->employee_pointer;
    if(employee->reporting_manager_id == -1)
    {
        print_employee_line(employee, 0);
        print_reports_recursive(employee->reports, 1);
    }
}

int count_all_recursive(struct Node *main_node)
{
    if(main_node == NULL)
    {
    return 0;
    }
    return 1 + count_all_recursive(main_node->next);
}

void complete_hierarchy(void)
{
    if(employee_list == NULL)
    {
        printf("No employees in the organization yet.\n\n");
        return;
    }
    printf("\n===== ORGANIZATION HIERARCHY =====\n\n");
    print_top_level_recursive(employee_list);
    printf("\nTotal Employees: %d\n\n", count_all_recursive(employee_list));
}

void menu(void)
{
    printf("===== EMPLOYEE HIERARCHY MANAGEMENT =====\n");
    printf("1. Add Employee\n");
    printf("2. Remove Employee\n");
    printf("3. Display Employee Hierarchy\n");
    printf("4. Display Complete Hierarchy\n");
    printf("5. Exit\n\n");
    printf("Enter your choice: ");
}

bool process_choice(int choice)
{
    bool input_available = true;
    switch(choice)
    {
        case 1:
            input_available = add_employee();break;
        case 2:
            input_available = remove_employee();break;
        case 3:
            input_available = employee_hierarchy();break;
        case 4:
            complete_hierarchy();break;
        case 5:
            printf("Exiting....\n");
            return false;
        default:
            printf("Must enter valid choice!\n\n");
    }
    if(!input_available)
    {
        printf("\nInput closed. Exiting....\n");
        return false;
    }
    return true;
}

int main(void)
{
    int choice;
    while(true)
    {
        menu();
        int result = scanf("%d", &choice);
        if(result == EOF)
        {
            printf("\nInput closed. Exiting....\n");
            break;
        }
        clear_input_buffer();
        if(result != 1)
        {
            printf("Must enter valid choice!\n\n");
            continue;
        }

        if(!process_choice(choice))
        {
            break;
        }
    }
    free_all_employees();
    return 0;
}