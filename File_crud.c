#include<stdio.h>
#include<string.h>
#include<stdbool.h>

struct user
{
    int id;
    char name[30];
    int age;
};

void clear_input_buffer()
{
    int character;
    while((character = getchar()) != '\n' && character != EOF)
    {
    }
}

bool getUserData(struct user *user)
{
    printf("Enter id:");
    if(scanf("%d",&user->id) != 1)
    {
        printf("Invalid Id\n");
        return false;
    };
    if(user->id < 1 || user->id > 500)
    {
        printf("Invalid ID\n");
        return false;
    }
    clear_input_buffer();
    printf("Enter name:");
    fgets(user->name,sizeof(user->name),stdin);
    user->name[strcspn(user->name, "\n")] = '\0';
    if(user->name[0] == '\0')
    {
        printf("Invalid user name\n");
        return false;
    }
    printf("Enter age: ");
    
    if(scanf("%d",&user->age) != 1)
    {
        printf("Invalid Age\n");
        return false;
    }
    if(user->age < 1 || user->age > 100)
    {
        printf("Invalid age\n");
        return false;
    }
    clear_input_buffer();
    return true;
}

bool is_file_empty(FILE *file_pointer)
{
    if(file_pointer == NULL)
    {
        printf("Unable to access file\n");
        return true;
    }
    rewind(file_pointer);
    fseek(file_pointer,0,SEEK_END);
    long file_size = ftell(file_pointer);
    return (file_size == 0);
}


void create_user(FILE *fp)
{
    struct user user;
    if(!getUserData(&user))
    {
        return;
    }
    char current_line[50];
    rewind(fp);
    while(fgets(current_line,sizeof(current_line),fp) != NULL)
    {
        int current_id;
        sscanf(current_line,"%d",&current_id);
        if(current_id == user.id)
        {
            printf("ID already exists...\n");
            return;
        }
    }
    fprintf(fp,"%d,%s,%d\n",user.id,user.name,user.age);
    printf("Creation successful\n");
}

void read_users(FILE *file)
{
    if(is_file_empty(file))
    {
        printf("No data to read\n");
        return;
    }
    rewind(file);
    char current_line[50];
    while(fgets(current_line,sizeof(current_line),file) != NULL)
    {
        char current_name[30];
        int current_id, current_age;
        sscanf(current_line, "%d,%[^,],%d",&current_id, current_name, &current_age);
        printf("----------------------------------------------------------\n");
        printf("User ID : %d\nUser name : %s\nUser age : %d\n",current_id,current_name,current_age);
    }
}

FILE * update_user_details(FILE *fp)
{
    if(is_file_empty(fp))
    {
        printf("File empty.Nothing to update..\n");
        return NULL;
    }
    rewind(fp);
    struct user user;
    if(!getUserData(&user))
        return NULL;
    FILE *temp_file = fopen("temp.txt","w");
    if(temp_file == NULL)
    {
        printf("Unable to open file");
        return NULL;
    }
    char current_line[50];
    int current_id;
    bool found_user = false;
    while(fgets(current_line,sizeof(current_line),fp) != NULL)
    {
        if(sscanf(current_line,"%d",&current_id) != 1)
        {
            printf("Malformed record\n");
            return NULL;
        }
        if(current_id == user.id)
        {
            fprintf(temp_file,"%d,%s,%d\n",user.id,user.name,user.age);
            printf("Update successful\n");
            found_user = true;
        }
        else
        {
            fprintf(temp_file,"%s",current_line);
        }
    }
    if(!found_user)
    {
        printf("ID not found...\n");
    }
    fclose(fp);
    fclose(temp_file);
    remove("users.txt");
    rename("temp.txt","users.txt");
    FILE *new_file_pointer = fopen("users.txt","a+");
    return new_file_pointer;
}

FILE * delete_user(FILE *fp)
{
    if(is_file_empty(fp))
    {
        printf("File empty.There is no data to delete...\n");
        return NULL;
    }
     rewind(fp); 
    char current_line[50];
    int current_id, delete_id;
    printf("Enter the user id: ");
    if(scanf("%d",&delete_id) != 1)
    {
        printf("Invalid Input\n");
        return NULL;
    }
    clear_input_buffer();
    if(delete_id < 1 || delete_id > 500)
    {
        printf("Invalid ID\n");
        return NULL;
    }
    FILE *temp_file = fopen("temp.txt","w");
    if(temp_file == NULL)
    {
        printf("Unable to open file");
        return NULL;
    }
    bool found_id = false;
    while(fgets(current_line,sizeof(current_line),fp) != NULL)
    {
        if(sscanf(current_line,"%d",&current_id) != 1)
        {
            printf("Malformed record.\n");
            return NULL;
        }
        if(current_id == delete_id)
        {
            found_id = true;
            printf("Deletion successful\n");
            continue;
        }
        fprintf(temp_file,"%s",current_line);
    }
    if(!found_id)
    {
        printf("ID not found to delete...\n");
    }
    fclose(temp_file);
    fclose(fp);
    remove("users.txt");
    rename("temp.txt","users.txt");
    FILE *new_file_pointer = fopen("users.txt","a+");
    return new_file_pointer;
}


void menu()
{
    printf("\n1.Create\n2.Read\n3.Update\n4.Delete\n5.Exit");
    printf("\nEnter your option: ");
}

int get_choice()
{
    int choice;
    menu();
     if(scanf("%d",&choice) != 1)
    {
        printf("Invalid choice\n");
        clear_input_buffer();
        return -1;
    }
    return choice;
}

bool process_choice(FILE **file_pointer, int choice)
{
    FILE *temporary_file = NULL;
    switch(choice)
    {
        case 1 : 
            create_user(*file_pointer);
            break;
        case 2 : 
            read_users(*file_pointer);
            break;
        case 3 :
            temporary_file = update_user_details(*file_pointer);
            if(temporary_file == NULL)
            {
                printf("Update unsucessful!\n");
            }
            else{
                *file_pointer = temporary_file;
            }
            break;
        case 4 :
            temporary_file = delete_user(*file_pointer);
            if(temporary_file == NULL)
            {
                printf("Delete unsucessful!\n");
            }
            else{
                *file_pointer = temporary_file;
            }
            break;
        case 5 :
            printf("Exited\n");
            return true;
        default :
            printf("Invalid Input\n");
    }
    return false;
}
int main()
{
    FILE *file_pointer = fopen("users.txt","a+");
    if(file_pointer == NULL)
    {
        printf("File cannot be opened!");
        return 0;
    }
    int choice;
    while(true)
    {
        choice = get_choice();
        if(choice == -1)
            continue;
        bool should_exit = process_choice(&file_pointer, choice);
        if(should_exit)
            break;
    }
    fclose(file_pointer);
    return 0;
}