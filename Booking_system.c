#include<stdio.h>
#include<string.h>
#include<stdbool.h>
#include<ctype.h>
#define MAX_NAME_LENGTH 30
#define MIN_TICKETS 1
#define MAX_TICKETS  20
#define DEFAULT_TICKET_PRIZE 0
#define DISCOUNT_MIN_TICKETS 5
#define DISCOUNT_RATE 0.10
#define DEFAULT_DISCOUNT_PRIZE 0.0


const int standard_ticket_price = 150;
const int premium_ticket_price = 250;
const int recliner_ticket_price = 400;

enum TicketCategory
{
    STANDARD = 1,
    PREMIUM,
    RECLINER
};

struct Booking
{
    char name[MAX_NAME_LENGTH];
    int ticket_count;
    enum TicketCategory ticket_category;
    float subtotal;
    float discount_amount;
    float final_bill;
};

void clear_input_buffer()
{
    int ch;
    while((ch = getchar()) != '\n' && ch != EOF)
    {
    }
}

void booking_counter(struct Booking *booking_details)
{
    while(1)
    {
    printf("Enter Name: ");
    if(fgets(booking_details->name, sizeof(booking_details->name),stdin) == NULL)
    {
        printf("Error reading name\n");
        continue;
    }
    size_t name_length = strlen(booking_details->name);
    if(name_length > 0 && booking_details->name[name_length - 1] =='\n')
    {
        booking_details->name[name_length - 1] = '\0';
    }
    else
    {
        int ch = getchar();
        if(ch != '\n' && ch != EOF){
        clear_input_buffer();
        printf("Name is too long. Maximum %d characters allowed\n", (MAX_NAME_LENGTH - 1));
        continue;
        }
    }
    if(booking_details->name[0] == '\0')
    {
        printf("Name must not be empty!\n");
        continue;
    }
    bool has_space = false;
    for(int idx = 0; booking_details->name[idx] != '\0'; idx++)
    {
        if(isspace((unsigned char)booking_details->name[idx]))
        {
            printf("Name must be a single word without spaces.\n");
            has_space = true;
            break;
        }
    }
    if(has_space) continue;
    break;
    }
    while(1)
    {
        printf("Enter the number of tickets: ");
        if(scanf("%d",&booking_details->ticket_count) != 1)
        {
        printf("Ticket count must be a valid number\n");
        clear_input_buffer();
        continue;
        }
        clear_input_buffer(); 
        if(booking_details->ticket_count < MIN_TICKETS || booking_details->ticket_count > MAX_TICKETS)
        {
        printf("Error: Ticket count must be between 1 to 20\n");
        continue;
        }
        break;
    }
    int category_choice;
    
    while(1)
    {
    printf("Enter the ticket category: ");
    if(scanf("%d",&category_choice) != 1)
    {
        printf("Ticket category must be a valid number\n");
        clear_input_buffer();
        continue;
    }
    clear_input_buffer();
    
    if(category_choice < STANDARD || category_choice > RECLINER)
    {
        printf("Error: You can only select from 1, 2 or 3 category\n");
        continue;
    }
    booking_details->ticket_category = (enum TicketCategory)category_choice;
    break;
    }
}

void calculate_subtotal(struct Booking *booking_details)
{
    float ticket_price;
    switch(booking_details->ticket_category)
    {
        case STANDARD:
            ticket_price = standard_ticket_price;
            break;
        case PREMIUM:
            ticket_price = premium_ticket_price;
            break;
        case RECLINER:
            ticket_price = recliner_ticket_price;
            break;
        default:
            ticket_price = DEFAULT_TICKET_PRIZE;
    }
    booking_details->subtotal = ticket_price * booking_details->ticket_count;
}

void calculate_discount(struct Booking* booking_details)
{
    
    if(booking_details->ticket_count >= DISCOUNT_MIN_TICKETS)
    {
        booking_details->discount_amount = booking_details->subtotal * DISCOUNT_RATE;
    }
    else
    {
        booking_details->discount_amount = DEFAULT_DISCOUNT_PRIZE;
    }
}

void calculate_final_price(struct Booking *booking_details)
{
    booking_details->final_bill = booking_details->subtotal - booking_details->discount_amount;
}

const char* get_ticket_category(struct Booking *booking_details)
{

    switch(booking_details->ticket_category)
    {
        case STANDARD :
            return "Standard";
        case PREMIUM:
            return "Premium";
        case RECLINER:
            return "Recliner";
    }
    return "Invalid category";
}

void print_booking_details(struct Booking *booking_details)
{
    const char* ticket_category = get_ticket_category(booking_details);
    printf("----------------------------------------------------------------\n");
    printf("Customer: %s\n",booking_details->name);
    printf("Ticket Category: %s\n", ticket_category);
    printf("Tickets: %d\n",booking_details->ticket_count);
    printf("Subtotal: %.2f\n", booking_details->subtotal);
    printf("Discount: %0.2f\n", booking_details->discount_amount);
    printf("Final Bill: %.2f\n",booking_details->final_bill);
    printf("----------------------------------------------------------------\n");
}

int main(void)
{
   struct Booking booking_details;
   booking_counter(&booking_details);
   calculate_subtotal(&booking_details);
   calculate_discount(&booking_details);
   calculate_final_price(&booking_details);
   print_booking_details(&booking_details);
   return 0;
}