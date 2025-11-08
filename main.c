#include <stdio.h>
#include <stdlib.h> // system() function
#include <string.h>
#include <ctype.h> // toupper(), tolower()
#include <stdbool.h>

#define CARPRICE 1000
#define DISCOUNT_MIN_AGE 20
#define DISCOUNT_MAX_AGE 25
#define EXTRA10 0.9f
#define DISCOUNT_20 0.8f
#define MENU_OPTION_CARS_ON_SALE 1
#define MENU_OPTION_BUY_CAR 2
#define MENU_OPTION_SALE_STATS 3
#define MENU_OPTION_EXIT 4
#define MAX_SALES 10

typedef struct
{
    char currentCustomerName[101];
    unsigned short currentCustomerAge;
    bool currentCustomerIsMember;
} Customer;

typedef struct
{
    unsigned short carsInStock;
    unsigned short totalCarsSold;
    unsigned short numberOfSales;
    float totalIncome;
    float totalDiscountGiven;
    char customerNames[MAX_SALES][101]; // 2D array: stores up to MAX_SALES names, each up to 100 chars (+1 for '\0')
} Statistics;

typedef struct
{
    char carModel[101];
    unsigned short carYear;
    float carPrice;
    bool carIsSold;
} Car;


int main(void)
{
    unsigned short carsWanted = 1, menuOption = 0, selectedId = 0;
    float discountThisSale = 0.0, currentTotalPrice = 0.0;
    char membership;
    bool validName;

    Customer currentCustomer = {0};
    Statistics statistics = {.carsInStock = 10};
    Car carsOnSale[MAX_SALES] = {
        {.carModel = "Ford Fiesta", .carYear = 2017, .carPrice = 4800, .carIsSold = false},
        {.carModel = "Toyota Corolla", .carYear = 2016, .carPrice = 6000, .carIsSold = false},
        {.carModel = "Volkswagen Golf", .carYear = 2015, .carPrice = 5900, .carIsSold = false},
        {.carModel = "Hyundai i30", .carYear = 2016, .carPrice = 5000, .carIsSold = false},
        {.carModel = "Kia Ceed", .carYear = 2017, .carPrice = 5400, .carIsSold = false},
        {.carModel = "BMW 320d", .carYear = 2014, .carPrice = 6900, .carIsSold = false},
        {.carModel = "Mercedes A180", .carYear = 2015, .carPrice = 7100, .carIsSold = false},
        {.carModel = "Audi A3", .carYear = 2015, .carPrice = 7300, .carIsSold = false},
        {.carModel = "Nissan Qashqai", .carYear = 2015, .carPrice = 6200, .carIsSold = false},
        {.carModel = "Honda Civic", .carYear = 2016, .carPrice = 6400, .carIsSold = false}
    };

    system("color 0E"); // black background, yellow text

    do
    {
        printf("\n========================================\n");
        printf("     WELCOME TO CHELEBI'S GARAGE  \n");
        printf("========================================\n");
        printf("      Quality Cars. Fair Deals.\n");
        printf("========================================\n");

        printf("Please enter your name: ");
        scanf(" %100[^\n]", currentCustomer.currentCustomerName);
        // reads up to 100 characters what the user types until Enter, but does not take the Enter key
        while (getchar() != '\n'); // clear the buffer and removes all leftover characters including the Enter key

        do
        {
            validName = true; // ilk seferde isim dogru olursa, dongu cikis sarti tamamlanir
            for (int i = 0; currentCustomer.currentCustomerName[i]; i++)
            {
                if (!isalpha(currentCustomer.currentCustomerName[i]))
                {
                    printf("Please enter a valid name using letters only:\n");
                    scanf(" %100[^\n]", currentCustomer.currentCustomerName);
                    while (getchar() != '\n');
                    validName = false;
                    break;
                }
            }
        }
        while (validName == false);


        // Convert name to have first letter uppercase and rest lowercase
        currentCustomer.currentCustomerName[0] = toupper(currentCustomer.currentCustomerName[0]);
        for (unsigned short i = 1; i < strlen(currentCustomer.currentCustomerName); i++)
        {
            currentCustomer.currentCustomerName[i] = tolower(currentCustomer.currentCustomerName[i]);
        }

        do
        {
            system("cls");

            printf("1. View Cars on Sale\n");
            printf("2. Buy a Car\n");
            printf("3. View Sales Statistics\n");
            printf("4. Log out\n");
            printf("\nChoose an option from menu %s: ", currentCustomer.currentCustomerName);
            scanf("%hu", &menuOption);
            while (getchar() != '\n'); // clear the buffer and removes all leftover characters including the Enter key

            system("cls"); // after input it clears the menu, so it is better visually

            switch (menuOption)
            {
            case MENU_OPTION_CARS_ON_SALE:
                for (unsigned short i = 0; i < MAX_SALES; i++)
                    printf("%hu. %s, %hu, %.2f GBP, %s\n", i + 1, carsOnSale[i].carModel, carsOnSale[i].carYear,
                           carsOnSale[i].carPrice, carsOnSale[i].carIsSold ? "SOLD" : "AVAILABLE");
                printf("\nCurrently, we have %hu cars available.\n", statistics.carsInStock);
                break; // it ends switch

            case MENU_OPTION_BUY_CAR: // Buying option codes are here

                if (statistics.carsInStock == 0)
                {
                    printf("\nSorry, %s all cars are sold out at the moment!\n", currentCustomer.currentCustomerName);
                    printf("Please check back later when we restock.\n");
                    break; // goes back to menu
                }

                for (unsigned short i = 0; i < MAX_SALES; i++)
                    if (carsOnSale[i].carIsSold == false)
                    {
                        printf("%hu. %s, %hu, %.2f GBP\n", i + 1, carsOnSale[i].carModel, carsOnSale[i].carYear,
                               carsOnSale[i].carPrice);
                    }
                printf("\nCurrently, we have %hu cars available.\n", statistics.carsInStock);


                printf("\nWhich car would you like to buy %s?", currentCustomer.currentCustomerName);

                bool validChoice = false;

                do
                {
                    scanf("%hu", &selectedId);
                    while (getchar() != '\n');

                    if (selectedId < 1 || selectedId > MAX_SALES)
                    {
                        printf("\nInvalid car ID. Please choose between 1 and 10: \n");
                        continue;// goes to the start of the loop, skips rest
                    }
                    do
                    {
                        if (carsOnSale[selectedId - 1].carIsSold == true) //Check if that car is already sold
                        {
                            printf("\nSorry, %s, this car is sold out!\n", currentCustomer.currentCustomerName);
                            printf("Please choose another car ID: \n");
                            scanf("%hu", &selectedId);
                            while (getchar() != '\n');
                        }
                    }
                    while (carsOnSale[selectedId - 1].carIsSold == true);
                    validChoice = true;
                }
                while (validChoice == false);// if the user enters an invalid ID, it will ask again until a valid ID is entered

                unsigned short index = selectedId - 1; // array index starts from 0, so we subtract 1
                printf("\nYou have selected: %s, %hu\n", carsOnSale[index].carModel, carsOnSale[index].carYear);


                printf("How old are you %s? Age:", currentCustomer.currentCustomerName);
                scanf("%hu", &currentCustomer.currentCustomerAge);
                while (getchar() != '\n');
                // clear the buffer and removes all leftover characters including the Enter key

                if (currentCustomer.currentCustomerAge < 18)
                {
                    printf("\nSorry %s, you must be at least 18 years old to buy a car.\n",
                           currentCustomer.currentCustomerName);
                    menuOption = MENU_OPTION_EXIT;
                    break;
                }

                currentTotalPrice = carsOnSale[index].carPrice;


                printf("\nDo you have a Chelebi Garage membership card? (Y/N): ");
                scanf(" %c", &membership);
                while (getchar() != '\n');
                // clear the buffer and removes all leftover characters including the Enter key
                system("cls");

                if (membership == 'Y' || membership == 'y')
                {
                    currentCustomer.currentCustomerIsMember = true;
                    printf("\nMembership confirmed \n");
                }
                else
                {
                    currentCustomer.currentCustomerIsMember = false;
                }

                //check discount and update and sale happens here
                if (currentCustomer.currentCustomerAge >= DISCOUNT_MIN_AGE && currentCustomer.currentCustomerAge <=
                    DISCOUNT_MAX_AGE)
                {
                    printf("Great news %s, You get a 20%% age discount.\n", currentCustomer.currentCustomerName);
                    currentTotalPrice *= DISCOUNT_20; // gives %20 by = *0.8
                    if (currentCustomer.currentCustomerIsMember == true)
                    {
                        printf("Plus, your Chelebi Garage membership gives you an extra 10%% discount.\n");
                        currentTotalPrice *= EXTRA10; // gives extra %10 =*0.9
                    }
                    printf("Final price after discounts: %.2f GBP\n", currentTotalPrice);

                    statistics.carsInStock -= carsWanted;
                    printf("Cars left in stock: %hu\n", statistics.carsInStock);

                    statistics.totalCarsSold += carsWanted; // Total how many cars sold
                    statistics.totalIncome += currentTotalPrice; // Total generated income
                    discountThisSale = carsOnSale[index].carPrice - currentTotalPrice;
                    //  given discount just for this sale
                    statistics.totalDiscountGiven += discountThisSale; // add this sale's discount to total discount

                    strcpy(statistics.customerNames[statistics.numberOfSales], currentCustomer.currentCustomerName);
                    // copy currentCustomer text into the 2D array
                    statistics.numberOfSales++; // increase number of sales after each successful sale
                    carsOnSale[index].carIsSold = true; // mark the car as sold

                    menuOption = MENU_OPTION_EXIT; // end the menu session for this customer
                    break; // ends switch
                }

                else if (currentCustomer.currentCustomerIsMember == true)
                {
                    printf("Thank you for being a Chelebi Garage member, %s.\n", currentCustomer.currentCustomerName);
                    printf("\nYou get a 10%% membership discount on your purchase.\n");
                    currentTotalPrice = carsOnSale[index].carPrice;
                    currentTotalPrice *= EXTRA10;
                    printf("\nFinal price after membership discount: %.2f GBP\n", currentTotalPrice);
                }

                else
                {
                    currentTotalPrice = carsOnSale[index].carPrice;
                    printf("No discount applied for this purchase.\n");
                    printf("Total price: %.2f GBP\n", currentTotalPrice);
                }

                statistics.carsInStock -= carsWanted;
                printf("Cars left in stock: %hu\n", statistics.carsInStock);
                statistics.totalCarsSold += carsWanted; // Total how many cars sold.
                statistics.totalIncome += currentTotalPrice; // Total generated income
                discountThisSale = carsOnSale[index].carPrice - currentTotalPrice;
                statistics.totalDiscountGiven += discountThisSale;
                carsOnSale[index].carIsSold = true; // mark the car as sold

                strcpy(statistics.customerNames[statistics.numberOfSales], currentCustomer.currentCustomerName);
                statistics.numberOfSales++;

                menuOption = MENU_OPTION_EXIT; // end the menu session for this customer
                break; // buying options finished


            case MENU_OPTION_SALE_STATS:
                printf("\n=== Sales Summary ===\n");
                printf("Customers served      : %hu\n", statistics.numberOfSales);
                printf("Cars sold             : %hu\n", statistics.totalCarsSold);
                printf("Total income          : %.2f GBP\n", statistics.totalIncome);
                printf("Total discount given  : %.2f GBP\n", statistics.totalDiscountGiven);

                if (statistics.totalCarsSold > 0) // Prevent division by zero when there are no sales
                {
                    printf("Average discount per car  : %.2f GBP\n",
                           statistics.totalDiscountGiven / statistics.totalCarsSold);
                    printf("Average income  per car   : %.2f GBP\n", statistics.totalIncome / statistics.totalCarsSold);
                }
                else
                {
                    printf("Average discount per car  : N/A (no cars sold)\n");
                    printf("Average income  per car   : N/A (no cars sold)\n");
                }

                if (statistics.numberOfSales == 0)
                {
                    printf("No customers served\n");
                }
                else
                {
                    for (unsigned short i = 0; i < statistics.numberOfSales; i++) // (initialization; condition; update)
                    {
                        printf("Customer %hu: %s\n", i + 1, statistics.customerNames[i]); // prints customer names
                    }
                }
                break; //ends switch

            case MENU_OPTION_EXIT:
                printf("Thank you for using our service %s.\n", currentCustomer.currentCustomerName);
                break; // ends switch

            default:
                printf("Please choose a number between 1-4: ");
            }


            printf(menuOption == MENU_OPTION_EXIT
                       ? "\n\nPress Enter ..."
                       : "\n\nPress Enter to return to the Menu..."); // sonucu printf dondur demek,
            //(condition) ? expression_if_true : expression_if_false;


            //getchar(); // takes "\n" from buffer
            getchar(); // waits an entry from user
            system("cls");
        }
        while (menuOption != MENU_OPTION_EXIT);
        // loop continues while option is NOT EXIT (4). If user chose EXIT, loop stops.
        printf("\nDo you wish to quit? (Y/N): ");
        char quitChoice;
        scanf(" %c", &quitChoice);
        while (getchar() != '\n');

        if (quitChoice == 'Y' || quitChoice == 'y')
        {
            break;
        }

        system("cls");
    }
    while (1);
    system("cls"); // clear screen
    printf("See you next time %s!\n", currentCustomer.currentCustomerName);
    return 0;
}
