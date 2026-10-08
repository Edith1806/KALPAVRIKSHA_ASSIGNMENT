#include<stdio.h>
#include<string.h>
#include<stdbool.h>
#include<math.h>

#define MAX_SIZE 100

//add_students()
//getstudentdata()
//calculate_tot()
//calc_avg()
//assign_grade()
//print_pattern()
//print_output()
//recusive_roll_no()
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
    printf("Enter roll no: ");
    if(scanf("%d", &(student->roll_no)) != 1)
    {
        printf("Invalid Input\n");
        clear_input_buffer();
        return false;
    }
    clear_input_buffer();
    if(student -> roll_no < 1 || student -> roll_no > 100)
    {
        printf("Invalid Roll no\n");
        clear_input_buffer();
        return false;
    }
    printf("Enter the name : ");
    fgets(student -> name, sizeof(student -> name), stdin);
    student -> name[strcspn(student -> name, "\n")] = '\0';
    if(student -> name[0] == '\0')
    {
        printf("Name cannot be empty.");
        clear_input_buffer();
        return false;
    }
    printf("Enter 3 subject marks with spaces (e.g: mark1 mark2 mark3): ");
    if(scanf("%d %d %d", &(student -> subjects_mark[0]), &(student -> subjects_mark[1]), &(student -> subjects_mark[2])) != 3)
    {
        printf("invalid no of marks/ Invalid mark");
        clear_input_buffer();
        return false;
    }
    for(int idx = 0; idx < 3; idx++)
    {
        if(student->subjects_mark[idx] < 0 || student->subjects_mark[idx] > 100)
        {
            printf("Invalid mark!");
            clear_input_buffer();
            return false;
        }
    }
    return true;
}

bool add_students(struct Student students[], int no_of_students)
{
    for(int current_roll_no = 1; current_roll_no <= no_of_students; current_roll_no++)
    {
        if(!get_student_data(&students[current_roll_no - 1]))
        {
            return false;
        }
        if(students[current_roll_no - 1] . roll_no != current_roll_no)
        {
            printf("Roll number mismatch!");
            return false;
        }
    }
    return true;
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

char find_grade(int average_mark)
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

void get_performance_star(char grade)
{
    switch(grade)
    {
        case 'A' : printf("*****\n");break;
        case 'B' : printf("****\n");break;
        case 'C' : printf("***\n");break;
        case 'D' : printf("**\n");break;
    }
}

void get_students_roll_no(struct Student students[],int no_of_students)
{
    if(no_of_students == 0)
        return;
    get_students_roll_no(students, no_of_students - 1);
    printf("%d ",students[no_of_students - 1].roll_no);
}

void get_students_analysis(struct Student students[], int no_of_students)
{
    for(int current_student_idx = 0; current_student_idx < no_of_students; current_student_idx++)
    {
        int currect_student_total = find_total_mark(students[current_student_idx]);
        float current_student_average = find_average_mark((float)currect_student_total);
        char current_student_grade = find_grade(round(current_student_average));

        printf("-----------------------------------------------------------------------\n");
        printf("Roll : %d\n", students[current_student_idx].roll_no);
        printf("Name : %s\n", students[current_student_idx].name);
        printf("Total : %d\n", currect_student_total);
        printf("Average : %.2f\n", current_student_average);
        printf("Grade : %c\n", current_student_grade);
        if(current_student_grade == 'F')
        {
            continue;
        }
        printf("Performance: ");
        get_performance_star(current_student_grade);
    }
}

int get_no_of_students()
{
    int no_of_students;
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


int main()
{
    int no_of_students = get_no_of_students();
    if(no_of_students == 0)
    {
        return 0;
    }
    struct Student students[no_of_students];
    if(!add_students(students, no_of_students))
    {
        printf("Add student unsuccessful!");
        return 0;
    }

    get_students_analysis(students, no_of_students);
    printf("List of Roll Numbers (via recursion): ");
    get_students_roll_no(students, no_of_students);
}