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
#define MAX_SALES 10

int main(void)
{
    unsigned short carsWanted = 0, carsAvailable = 10, userAge = 0, menuOption = 0, totalSold = 0, hasMembership = FALSE
                   , numberOfSales = 0;
    float totalIncome = 0.0, discountThisSale = 0.0, totalDiscountGiven = 0.0, totalPrice = 0.0;
    char membership, currentCustomer[101];
    char customerNames[MAX_SALES][101]; // 2D array: stores up to MAX_SALES names, each up to 100 chars (+1 for '\0')


    do // at least will work one time. condition is MENU_OPTION_EXIT
    {
        system("cls"); //clear screen
        printf("\n---------------------------------------");
        printf("\nWelcome to Chelebi's Garage\n");
        printf("---------------------------------------\n");

        printf("Please enter your name: ");
        scanf("  %100[^\n]", currentCustomer); // reads up to 100 characters what the user types until Enter, but does not take the Enter key
        while (getchar() != '\n'); // clear the buffer and removes all leftover characters including the Enter key


        // Convert name to have first letter uppercase and rest lowercase
        currentCustomer[0] = toupper(currentCustomer[0]);
        for (unsigned short i = 1; i < strlen(currentCustomer); i++)
        {
            currentCustomer[i] = tolower(currentCustomer[i]);
        }

        system("cls");

        printf("\n1. Cars on Sale\n");
        printf("2. Buy a Car\n");
        printf("3. Sale Stats\n");
        printf("4. Exit\n");
        printf("\nChoose an option from menu %s: ", currentCustomer);
        scanf("%hu", &menuOption);
        while (getchar() != '\n');// clear the buffer and removes all leftover characters including the Enter key

        system("cls"); // after input it clears the menu, so it is better visually

        switch (menuOption)
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
            printf("\nHow many cars would you like to buy %s?", currentCustomer);
            scanf("%hu", &carsWanted);
            while (getchar() != '\n');// clear the buffer and removes all leftover characters including the Enter key

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

            printf("How old are you %s? Age:", currentCustomer);
            scanf("%hu", &userAge);
            while (getchar() != '\n');// clear the buffer and removes all leftover characters including the Enter key

            if (userAge < 18)
            {
                printf("Sorry, %s you can't buy a car!!!\n", currentCustomer);
                break;
            }

            totalPrice = carsWanted * CARPRICE;


            printf("Do you have a membership card (Y/N):");
            scanf(" %c", &membership);
            while (getchar() != '\n');// clear the buffer and removes all leftover characters including the Enter key

            if (membership == 'Y' || membership == 'y')
            {
                hasMembership = TRUE;
            }
            else
            {
                hasMembership = FALSE;
            }

            system("cls");

            //check discount and update and sale happens here
            if (userAge >= DISCOUNT_MIN_AGE && userAge <= DISCOUNT_MAX_AGE)
            {
                printf("You are eligible for %%20 age discount\n");
                totalPrice *= DISCOUNT_20; // gives %20 by = *0.8
                if (hasMembership == TRUE)
                {
                    printf("Your membership gives you an extra %%10 discount.\n");
                    totalPrice *= EXTRA10; // gives extra %10 =*0.9
                }
                printf("%s, You need to pay %.2f GBP\n", currentCustomer, totalPrice);

                carsAvailable -= carsWanted;
                printf("There are %hu cars left\n", carsAvailable);

                totalSold += carsWanted; // Total how many cars sold
                totalIncome += totalPrice; // Total generated income
                discountThisSale = (carsWanted * CARPRICE) - (totalPrice); //  given discount just for this sale
                totalDiscountGiven += discountThisSale; // add this sale's discount to total discount

                strcpy(customerNames[numberOfSales], currentCustomer); // copy currentCustomer text into the 2D array
                numberOfSales++; // increase number of sales after each successful sale

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
            discountThisSale = (carsWanted * CARPRICE) - (totalPrice);
            totalDiscountGiven += discountThisSale;

            strcpy(customerNames[numberOfSales], currentCustomer);
            numberOfSales++;


            break; // buying options finished


        case MENU_OPTION_SALE_STATS: //stats
            printf("\n=== Sales Summary ===\n");
            printf("Customers served      : %hu\n", numberOfSales);
            printf("Cars sold             : %hu\n", totalSold);
            printf("Total income          : %.2f GBP\n", totalIncome);
            printf("Total discount given  : %.2f GBP\n", totalDiscountGiven);

            if (totalSold > 0) // Prevent division by zero when there are no sales
            {
                printf("Average discount per car  : %.2f GBP\n", totalDiscountGiven / totalSold);
                printf("Average income  per car   : %.2f GBP\n", totalIncome / totalSold);
            }
            else
            {
                printf("Average discount per car  : N/A (no cars sold)\n");
                printf("Average income  per car   : N/A (no cars sold)\n");
            }

            if (numberOfSales == 0)
            {
                printf("No customers served\n");
            }
            else
            {
                for (unsigned short i = 0; i < numberOfSales; i++) // (initialization; condition; update)
                {
                    printf("Customer %hu: %s\n", i + 1, customerNames[i]);// prints customer names
                }
            }
            break; //ends switch

        case MENU_OPTION_EXIT:
            printf("Thank you for using our service %s. Have a good day!\n", currentCustomer);
            break; // ends switch

        default:
            printf("Please choose a number between 1-4: ");
        }


        printf(menuOption == MENU_OPTION_EXIT
                   ? "\n\nPress Enter to Exit..."
                   : "\n\nPress Enter to return to the Menu..."); // sonucu printf dondur demek,
        //(condition) ? expression_if_true : expression_if_false;

        getchar(); // takes "\n" from buffer
        getchar(); // waits an entry from user
    } // do ends here
    while (menuOption != MENU_OPTION_EXIT);
    // loop continues while option is NOT EXIT (4). If user chose EXIT, loop stops.

    system("cls"); // clear screen
    printf("See you next time %s!\n", currentCustomer);
    return 0;
}
