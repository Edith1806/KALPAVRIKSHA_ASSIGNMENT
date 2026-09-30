#include<stdio.h>
#include<string.h>
#include<stdbool.h>
struct user
{
    int id;
    char name[50];
    int age;
};

struct user create_user()
{
    struct user u1;
    printf("Enter id:");
    scanf("%d%*c",&u1.id);
    printf("Enter updated name:");
    fgets(u1.name,sizeof(u1.name),stdin);
    u1.name[strcspn(u1.name, "\n")] = '\0';
    printf("Enter age: ");
    scanf("%d",&u1.age);
    return u1;
}

bool isEmpty(FILE *fp)
{
    if(fp == NULL)
    {
        printf("File is not created yet....\n");
        return false;
    }
    rewind(fp);
    fseek(fp,0,SEEK_END);
    long sz = ftell(fp);
    return (sz == 0);
}
void create(FILE *fp)
{
    struct user u1 = create_user();
    char line[50];
    while(fgets(line,sizeof(line),fp) != NULL)
    {
        int id;
        sscanf(line,"%d",&id);
        if(id == u1.id)
        {
            printf("ID already exists...\n");
            return;
        }
    }
    fprintf(fp,"%d,%s,%d\n",u1.id,u1.name,u1.age);
    printf("Creation successful\n");
}

void read(FILE *fp)
{
    if(isEmpty(fp))
    {
        printf("No data to read\n");
        return;
    }
    rewind(fp);
    char ch[50];
    while(fgets(ch,sizeof(ch),fp) != NULL)
    {
        printf("%s",ch);
    }
}

FILE * update(FILE *fp)
{
    if(isEmpty(fp))
    {
        printf("File empty.Nothing to update..\n");
        return NULL;
    }
    rewind(fp);
    FILE *t = fopen("temp.txt","w");
    struct user u1 = create_user();
    char line[50];
    int id;
    bool found = false;
    while(fgets(line,sizeof(line),fp) != NULL)
    {
        sscanf(line,"%d",&id);
        if(id == u1.id)
        {
            fprintf(t,"%d,%s,%d\n",u1.id,u1.name,u1.age);
            printf("Update successful\n");
            found = true;
        }
        else
        {
            fprintf(t,"%s",line);
        }
    }
    if(!found)
    {
        printf("ID not found...\n");
    }
    fclose(fp);
    fclose(t);
    remove("user.txt");
    rename("temp.txt","user.txt");
    FILE *f = fopen("user.txt","a+");
    return f;
}

FILE * delete(FILE *fp)
{
    if(isEmpty(fp))
    {
        printf("File empty.There is no data to delete...\n");
        return NULL;
    }
     rewind(fp); 
    char line[50];
    int id,x;

    printf("Enter the user id: ");
    scanf("%d",&x);
    FILE *t = fopen("temp.txt","w");
   bool found = false;
    while(fgets(line,sizeof(line),fp) != NULL)
    {
        sscanf(line,"%d",&id);
        if(id == x)
        {
            found = true;
            printf("Deletion successful\n");
            continue;
        }
        fprintf(t,"%s",line);
    }
    if(!found)
    {
        printf("ID not found to delete...\n");
    }
    fclose(t);
    fclose(fp);
    remove("user.txt");
    rename("temp.txt","user.txt");
    FILE *f = fopen("user.txt","a+");
    return f;
}
void menu()
{
    printf("\n1.Create\n2.Read\n3.Update\n4.Delete\n5.Exit");
    printf("\nEnter your option: ");
}
int main()
{
    FILE *fp = fopen("user.txt","a+");
    int x;
    while(true)
    {
        menu();
        scanf("%d",&x);
        switch(x)
        {
            case 1:create(fp);break;
            case 2:read(fp);break;
            case 3:fp = update(fp);break;
            case 4:fp = delete(fp);break;
            case 5:printf("Exited...\n");return 0;
            default:printf("Invalid input\n");
        }
    }
    fclose(fp);
    return 0;
}