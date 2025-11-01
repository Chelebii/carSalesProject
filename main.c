#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> // toupper(), tolower()

#define CARPRICE 1000
#define DISCOUNT_MIN_AGE 20
#define DISCOUNT_MAX_AGE 25
#define EXTRA10 0.9f
#define DISCOUNT_20 0.8f
#define MENU_OPTION_CARS_ON_SALE 1
#define MENU_OPTION_BUY_CAR 2
#define MENU_OPTION_SALE_STATS 3
#define MENU_OPTION_EXIT 4
#define TRUE 1
#define FALSE 0

int main(void)
{
    unsigned short carsWanted = 0, carsAvailable = 10, userAge = 0, option = 0, totalSold = 0, hasMembership = FALSE;
    float totalIncome = 0.0, totalMadeDiscount = 0.0, totalDiscountGiven = 0.0, totalPrice = 0.0;
    char membership, customerNames[101], firstName[40];


    do // at least will work one time. condition is MENU_OPTION_EXIT
    {
        system("cls"); //clear screen
        printf("\n---------------------------------------");
        printf("\nWelcome to Chelebi's Garage\n");
        printf("---------------------------------------\n");

        printf("Please enter your name: ");
        fgets(customerNames, 101, stdin); // max 100 chars +1 for '\0'
        customerNames[strlen(customerNames) - 1] = '\0'; // removes the newline '\n' added by fgets

        while (strlen(customerNames) == 0) // if the name is empty, ask again
        {
            printf("Please enter your name: ");
            fgets(customerNames, 101, stdin);
            customerNames[strlen(customerNames) - 1] = '\0';
        }


        sscanf(customerNames, "%s", firstName); // read first word from the string, till ' '
        // sscanf = string scanf (reads from a string, not from keyboard)

        customerNames[0] = toupper(customerNames[0]);
        firstName[0] = toupper(firstName[0]); // convert first character to uppercase

        for (int i = 1; i < strlen(firstName); i++) // this for loop makes rest of the letters lower
        {
            firstName[i] = tolower(firstName[i]);
        }

        printf("Hello %s\n", customerNames);

        printf("\n1. Cars on Sale\n");
        printf("2. Buy a Car\n");
        printf("3. Sale Stats\n");
        printf("4. Exit\n");
        printf("\nChoose an option from menu %s: ", firstName);
        scanf("%hu", &option);

        system("cls"); // after input it clears the menu, so it is better visually

        switch (option)
        {
        case MENU_OPTION_CARS_ON_SALE:
            printf("\nWorking on it\n");
            break; // it ends switch

        case MENU_OPTION_BUY_CAR: // Buying option codes are here

            if (carsAvailable == 0)
            {
                printf("Sorry, all cars are sold out\n");
                break; // goes back to menu
            }

            printf("\nThere are %hu cars available, each car costs %d GBP\n", carsAvailable,CARPRICE);
            printf("\nHow many cars would you like to buy?");
            scanf("%hu", &carsWanted);

            //check are there enough cars
            if (carsWanted > carsAvailable)
            {
                printf("There are fewer cars in the Garage than you need\n");
                break;
            }
            else if (carsWanted == 0)
            {
                printf("You need to enter a number greater than 0\n");
                break;
            }


            printf("How old are you %s? Age:", firstName);
            scanf("%hu", &userAge);

            if (userAge < 18)
            {
                printf("Sorry, %s you can't buy a car!!!\n", firstName);
                break;
            }

            totalPrice = carsWanted * CARPRICE;


            printf("Do you have a membership card (Y/N):");
            scanf(" %c", &membership);

            if (membership == 'Y' || membership == 'y')
            {
                hasMembership = TRUE;
            }
            else
            {
                hasMembership = FALSE;
            }

            //check discount and update
            if (userAge >= DISCOUNT_MIN_AGE && userAge <= DISCOUNT_MAX_AGE)
            {
                printf("You are eligible for %%20 age discount\n");
                totalPrice *= DISCOUNT_20; // gives %20 by = *0.8
                if (hasMembership == TRUE)
                {
                    printf("Your membership gives you an extra %%10 discount.\n");
                    totalPrice *= EXTRA10; // gives extra %10 =*0.9
                }
                printf("You need to pay %0.2f GBP\n", totalPrice);

                carsAvailable -= carsWanted;
                printf("There are %hu cars left\n", carsAvailable);

                totalSold += carsWanted; // Total how many cars sold
                totalIncome += totalPrice; // Total generated income
                totalMadeDiscount = (carsWanted * CARPRICE) - (totalPrice);
                totalDiscountGiven += totalMadeDiscount; //

                break; // after buying completed goes back to menu
            }

            else if (hasMembership == TRUE)
            {
                printf("You get an extra %%10 membership discount.\n");
                totalPrice *= EXTRA10;
                printf("You need to pay %0.2f GBP\n", totalPrice);
            }

            else
            {
                totalPrice = carsWanted * CARPRICE;
                printf("You need to pay %0.2f GBP\n", totalPrice);
            }

            carsAvailable -= carsWanted;
            printf("There are %hu cars left\n", carsAvailable);
            totalSold += carsWanted; // total how many cars sold. this code calculates
            totalIncome += totalPrice; // Total generated income
            totalMadeDiscount = (carsWanted * CARPRICE) - (totalPrice);
            totalDiscountGiven += totalMadeDiscount;


            break; // buying options finished


        case MENU_OPTION_SALE_STATS: //stats
            printf("Total %hu cars sold.\n", totalSold);
            printf("Total %0.2f GBP income.\n", totalIncome);
            printf("Total %0.2f GBP discount given\n", totalDiscountGiven);
            break; //ends switch

        case MENU_OPTION_EXIT:
            printf("Thank you for using our service %s. Have a good day!\n", customerNames);
            break; // ends switch

        default:
            printf("Please choose a number between 1-4: ");
        }


        printf(option == MENU_OPTION_EXIT
                   ? "\n\nPress Enter to Exit..."
                   : "\n\nPress Enter to return to the Menu..."); // sonucu printf dondur demek,
        //(condition) ? expression_if_true : expression_if_false;

        getchar(); // takes "\n" from buffer
        getchar(); // waits an entry from user
    } // do ends here
    while (option != MENU_OPTION_EXIT); // loop continues while option is NOT EXIT (4). If user chose EXIT, loop stops.

    system("cls"); // clear screen
    printf("See you next time %s!\n", firstName);
    return 0;

}
