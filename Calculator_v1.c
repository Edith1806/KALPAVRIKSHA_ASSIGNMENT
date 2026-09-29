#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#define max 100
int var[max];
char opr[max];
int top_var = -1;
int top_op = -1;
bool crt = true;
void push_st1(int val)
{
    if(top_var == max - 1)
    {
        printf("overflow..");
        return;
    }
    var[++top_var] = val;
}

int pop_st1()
{
    if(top_var==-1){
        printf("Underflow");
    return -1;}
    return var[top_var--];
}

bool isFull_st1()
{
    if(top_var == max - 1) return true;
    return false;
}
bool isEmpty_st1()
{
    if(top_var == -1) return true;
    return false;

}

int peek_st1()
{
    if(isEmpty_st1()) return -1;
    return var[top_var];
}

void push_st2(char val)
{
    if(top_op == max - 1)
    {
        printf("overflow..");
        return;
    }
    opr[++top_op] = val;
}

char pop_st2()
{
    if(top_op==-1){
        printf("Underflow");
    return -1;}
    return opr[top_op--];
}

bool isFull_st2()
{
    if(top_op == max - 1) return true;
    return false;
}
bool isEmpty_st2()
{
    if(top_op == -1) return true;
    return false;

}

char peek_st2()
{
    if(isEmpty_st2()) return -1;
    return opr[top_op];
}

int precedence(char ch)
{
    switch(ch)
    {
        case '/': return 2;
        case '*':return 2;
        case '+':return 1;
        case '-':return 1;
        default:return 0;
    }
}

int calculate(char op, int l, int r)
{
    switch(op)
    {
        case '+' : return l + r;
        case '-' : return l - r;
        case '*' : return l * r;
        case '/' : {
            if(r == 0) {
                crt = false;return 0;}
            return l / r;
        }
    }
}

bool calc_opr()
{
    bool isemp = false;
    if(isEmpty_st1()) isemp = true;
    int r = pop_st1();
    if(isEmpty_st1()) isemp = true;
    int l = pop_st1();
    if(isemp)
    {
        printf("Invalid expression given...");
        return false;
    }
                
    int ans = calculate(pop_st2(), l, r);
    if(!crt)
    {
        printf("Division by 0 not possible");
        return false;
    }
    push_st1(ans);
    return true;
}


int main()
{
    char exp[100];
    char act[100];
    
    printf("Enter expression: ");
    fgets(exp, sizeof(exp), stdin);
    
    exp[strcspn(exp,"\n")] = '\0';
    int n = strlen(exp);
    int k = 0;
    for(int i = 0;  i < n; i++)
    {
        if(exp[i] == ' ') {
            continue;
        } else {
            act[k++] = exp[i];
        }
    }
    act[k] = '\0';

    int n1 = strlen(act);
    int num = 0;
    bool expect_num= true;
    for(int i = 0; i < n1; i++)
    {
        char op = act[i];
        if(op >= '0' && op <= '9')
        {
            num = num * 10 + (act[i] - '0');
            expect_num = false;
        }
        else if(op == '+' || op == '-' || op == '*' || op == '/')
        {
            
            if(expect_num)
            {
                printf("Invalid Expression...");
                return 0;
            }
            
            push_st1(num);
            num = 0;
            while(!isEmpty_st2() && precedence(peek_st2()) >= precedence(op))
            {
                if(!calc_opr())
                    return 0; 
            }
            push_st2(op);
            expect_num = true;
        }
        else
        {
            printf("Error : Invalid Input");
            return 0;
        }
       
    }
    if(expect_num)
    {
        printf("Invalid Expression...");
        return 0;
    }
    push_st1(num);
    while(!isEmpty_st2())
    {
        if(!calc_opr()) return 0;
    }
    if(top_var != 0)
    {
        printf("Invalid input...");
    }
    else{
    printf("Result: %d\n",pop_st1());}
    return 0;
}
