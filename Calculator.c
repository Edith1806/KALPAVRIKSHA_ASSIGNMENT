#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#define  MAX_SIZE 100
int operand_stack[MAX_SIZE];
char operator_stack[MAX_SIZE];
int operand_top = -1;
int operator_top = -1;
bool calculation_successful = true;
void push_operand(int val)
{
    if(operand_top == MAX_SIZE - 1)
    {
        printf("overflow..");
        return;
    }
    operand_stack[++operand_top] = val;
}

int pop_operand()
{
    if(operand_top==-1){
        printf("Underflow");
    return -1;}
    return operand_stack[operand_top--];
}

bool isEmpty_operand_stack()
{
    if(operand_top == -1) return true;
    return false;

}


void push_operator(char val)
{
    if(operator_top == MAX_SIZE - 1)
    {
        printf("overflow..");
        return;
    }
    operator_stack[++operator_top] = val;
}

char pop_operator()
{
    if(operator_top == -1){
        printf("Underflow");
    return -1;}
    return operator_stack[operator_top--];
}

bool isEmpty_operator_stack()
{
    if(operator_top == -1) return true;
    return false;

}

char peek_operator()
{
    if(isEmpty_operator_stack()) return -1;
    return operator_stack[operator_top];
}

int precedence(char operator)
{
    switch(operator)
    {
        case '/': return 2;
        case '*':return 2;
        case '+':return 1;
        case '-':return 1;
    }
    return 0;
}

int calculate(char operator, int left_operand, int right_operand)
{
    switch(operator)
    {
        case '+' : return left_operand + right_operand;
        case '-' : return left_operand - right_operand;
        case '*' : return left_operand * right_operand;
        case '/' : {
            if(right_operand == 0) {
                calculation_successful = false;return 0;}
            return left_operand / right_operand;
        }
    }
    calculation_successful = false;
    return -1;
}

bool calculate_expression()
{
    bool insufficient_operands = false;
    if(isEmpty_operand_stack()) insufficient_operands = true;
    int right_operand = pop_operand();
    if(isEmpty_operand_stack()) insufficient_operands = true;
    int left_operand = pop_operand();
    if(insufficient_operands)
    {
        printf("Invalid expression given...");
        return false;
    }
                
    int answer = calculate(pop_operator(), left_operand, right_operand);
    if(!calculation_successful)
    {
        printf("Division by 0 not possible");
        return false;
    }
    push_operand(answer);
    return true;
}

void validate_expression(char expression[], char cleaned_expression[])
{
    expression[strcspn(expression,"\n")] = '\0';
    int expression_length = strlen(expression);
    int idx = 0;
    for(int i = 0;  i < expression_length; i++)
    {
        if(expression[i] == ' ') {
            continue;
        } else {
            cleaned_expression[idx++] = expression[i];
        }
    }
    cleaned_expression[idx] = '\0';
}

void main_calculator(char expression[])
{
    int expression_size = strlen(expression);
    int number = 0;
    bool expect_number = true;
    for(int i = 0; i < expression_size; i++)
    {
        char character = expression[i];
        if(character >= '0' && character <= '9')
        {
            number = number * 10 + (expression[i] - '0');
            expect_number = false;
        }
        else if(character == '+' || character == '-' || character == '*' || character == '/')
        {
            
            if(expect_number)
            {
                printf("Invalid Expression...");
                return;
            }
            
            push_operand(number);
            number = 0;
            while(!isEmpty_operator_stack() && precedence(peek_operator()) >= precedence(character))
            {
                if(!calculate_expression())
                    return; 
            }
            push_operator(character);
            expect_number = true;
        }
        else
        {
            printf("Error : Invalid Input");
            return;
        }
       
    }
    if(expect_number)
    {
        printf("Invalid Expression...");
        return;
    }
    push_operand(number);
    while(!isEmpty_operator_stack())
    {
        if(!calculate_expression()) return;
    }
    if(operand_top != 0)
    {
        printf("Invalid input...");
    }
    else{
    printf("Result: %d\n",pop_operand());}
}


int main()
{
    char expression[MAX_SIZE];
    char cleaned_expression[MAX_SIZE];
    
    printf("Enter expression: ");
    fgets(expression, sizeof(expression), stdin);
    validate_expression(expression, cleaned_expression);
    main_calculator(cleaned_expression);
    return 0;
}
