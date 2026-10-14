#include<stdio.h>
#include<string.h>
#include<stdbool.h>
#include<stdlib.h>
#include<ctype.h>
#define MAX_SIZE 100

int no_of_students;

struct Student
{
    int roll_no;
    char name[30];
    int subjects_mark[3];
};

void clear_input_buffer()
{
    int character;
    while((character = getchar()) != '\n' && character != EOF)
    {
    }
}

bool get_student_data(struct Student *student)
{
    printf("Enter students detail in single line eg:Roll_no name mark1 mark2 mark3: ");
    char input[200];
    fgets(input,sizeof(input),stdin);
    input[strcspn(input, "\n")] = '\0';
    char *end;
    student->roll_no = strtol(input, &end, 10);
    while(isspace((unsigned char) *end))
    {
        end++;
    }
    int loop_count = 3;
    while(loop_count > 0)
    {
    char *space_pointer = strrchr(input,' ');
    if(space_pointer == NULL)
    {
        printf("Invalid input format.\n");
        return false;
    }
    char *mark_start = space_pointer + 1;
    char *mark_end;

    long mark = strtol(mark_start, &mark_end, 10);
    if(mark < 0 || mark > 100)
    {
        printf("Mark must be between 0 to 100\n");
        return false;
    }
    student->subjects_mark[loop_count - 1] = (int)mark;
    *space_pointer = '\0';
     loop_count--;
    }   
    
    char *name_start = end;
    while(isspace((unsigned char) *name_start))
    {
        name_start++;
    }

    size_t name_length = strlen(name_start);
    if(name_length == 0 || name_length >= sizeof(student-> name))
    {
        printf("Name cannot be empty or too long\n");
        return false;
    }
    strcpy(student->name, name_start);
    if(student->roll_no < 1)
    {
        printf("Roll number must be greater than 0.\n");
        return false;
    }
    return true;
}

bool check_uniquness_of_students(struct Student students[], int check_student_index)
{
    if(check_student_index == 0)
    {
        return true;
    }
    for(int current_index = 0; current_index < check_student_index; current_index++)
    {
        if(students[current_index].roll_no == students[check_student_index].roll_no)
        {
            printf("Roll number should be unique for each student\n");
            return false;
        }
    }
    return true;
}

void add_students(struct Student students[])
{
    for(int idx = 0; idx < no_of_students; idx++)
    {
        while(true)
        {
            if(!get_student_data(&students[idx]))
                continue;
            if(!check_uniquness_of_students(students, idx))
                continue;
            break;
        }      
    }
    printf("Students detail added successfully.\n");
}

int find_total_mark(struct Student student)
{
    int total_mark = 0;
    for(int mark_index = 0; mark_index < 3; mark_index++)
    {
        total_mark += student.subjects_mark[mark_index];
    }
    return total_mark;
}

float find_average_mark(float total_mark)
{
    return total_mark / 3;
}

char find_grade(float average_mark)
{
    if(average_mark >= 85)
        return 'A';
    else if(average_mark >= 70)
        return 'B';
    else if(average_mark >= 50)
        return 'C';
    else if(average_mark >= 35)
        return 'D';
    return 'F';
}

void print_performance_star(char grade)
{
    switch(grade)
    {
        case 'A' : printf("*****\n");break;
        case 'B' : printf("****\n");break;
        case 'C' : printf("***\n");break;
        case 'D' : printf("**\n");break;
    }
}

void print_students_roll_no(struct Student students[],int no_of_students)
{
    if(no_of_students == 0)
        return;
    print_students_roll_no(students, no_of_students - 1);
    printf("%d ",students[no_of_students - 1].roll_no);
}

void print_students_analysis(struct Student students[], int no_of_students)
{
    for(int current_student_idx = 0; current_student_idx < no_of_students; current_student_idx++)
    {
        int current_student_total = find_total_mark(students[current_student_idx]);
        float current_student_average = find_average_mark((float)current_student_total);
        char current_student_grade = find_grade(current_student_average);

        printf("-----------------------------------------------------------------------\n");
        printf("Roll : %d\n", students[current_student_idx].roll_no);
        printf("Name : %s\n", students[current_student_idx].name);
        printf("Total : %d\n", current_student_total);
        printf("Average : %.2f\n", current_student_average);
        printf("Grade : %c\n", current_student_grade);
        if(current_student_grade == 'F')
        {
            continue;
        }
        printf("Performance: ");
        print_performance_star(current_student_grade);
    }
}

int get_no_of_students()
{
    printf("Enter no of students : ");
    if(scanf("%d", &no_of_students) != 1)
    {
        printf("Invalid Input\n");
        clear_input_buffer();
        return 0;
    }

    if(no_of_students < 1 || no_of_students > MAX_SIZE)
    {
        printf("Invalid number of students\n");
        clear_input_buffer();
        return 0;
    }
    return no_of_students;
}


int main(void)
{
    no_of_students = get_no_of_students();
    while(no_of_students == 0)
    {
        printf("Number of students must be greater than 0 and less than 100\n");
        no_of_students = get_no_of_students();
    }
    clear_input_buffer();
    struct Student students[no_of_students];
    add_students(students);

    print_students_analysis(students, no_of_students);
    printf("List of Roll Numbers (via recursion): ");
    print_students_roll_no(students, no_of_students);

    return 0;
}