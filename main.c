#include <stdio.h>
#include <stdlib.h> // system() function
#include <string.h>
#include <ctype.h> // toupper(), tolower()
#include <stdbool.h>
#include <time.h> //localtime fonksiyonu burada tanimli

#define DISCOUNT_MIN_AGE 20
#define DISCOUNT_MAX_AGE 25
#define EXTRA10 0.9f
#define DISCOUNT_20 0.8f
#define MENU_OPTION_CARS_ON_SALE 1
#define MENU_OPTION_BUY_CAR 2
#define MENU_OPTION_SALE_STATS 3
#define MENU_OPTION_EXIT 4
#define MAX_MODEL 100
#define CAR_WANTED 1

unsigned short menuOption = 0, selectedId = 0;
char membership;
bool validName;

typedef struct
{
    char currentCustomerName[101];
    unsigned short currentCustomerAge;
    bool currentCustomerIsMember;
} Customer; // represents current active user

typedef struct
{
    char customerName[101];
    unsigned short customerAge;
    char carModel[101];
    unsigned short carYear;
    float finalPrice;
    float discountAmount;
    char discountType[51]; // "No discount", "Age", "Membership", "Age + Membership"
    time_t saleDate;
    unsigned short customerFeedbackRating; // 1 to 5
    char customerFeedback[201];
} Sale; // represents a single completed sale (used for statistics)

typedef struct
{
    unsigned short carsInStock;
    unsigned short numberOfSales;
    float totalIncome;
    float totalDiscountGiven;
    Sale sales[MAX_MODEL];
} Statistics;


typedef struct
{
    char carModel[101];
    unsigned short carYear;
    float carPrice;
    unsigned short carStock;
} Car;

Car carsOnSale[MAX_MODEL] = {
    {.carModel = "Ford Fiesta", .carYear = 2017, .carPrice = 4800, .carStock = 3},
    {.carModel = "Toyota Corolla", .carYear = 2016, .carPrice = 6000, .carStock = 2},
    {.carModel = "Volkswagen Golf", .carYear = 2015, .carPrice = 5900, .carStock = 1},
    {.carModel = "Hyundai i30", .carYear = 2016, .carPrice = 5000, .carStock = 1},
    {.carModel = "Kia Ceed", .carYear = 2017, .carPrice = 5400, .carStock = 4},
    {.carModel = "BMW 320d", .carYear = 2014, .carPrice = 6900, .carStock = 2},
    {.carModel = "Mercedes A180", .carYear = 2015, .carPrice = 7100, .carStock = 1},
    {.carModel = "Audi A3", .carYear = 2015, .carPrice = 7300, .carStock = 1},
    {.carModel = "Nissan Qashqai", .carYear = 2015, .carPrice = 6200, .carStock = 2},
    {.carModel = "Honda Civic", .carYear = 2016, .carPrice = 6400, .carStock = 3},
    //{.carModel = "Abc", .carYear = 2016, .carPrice = 10000, .carStock = 10}
};

unsigned short countCarModels(Car carsOnSale[], unsigned short capacity)
{
    unsigned short count = 0;
    for (unsigned short i = 0; i < capacity; i++)
    {
        if (carsOnSale[i].carModel[0] != '\0')
        {
            count++;
        }
    }
    return count;
}

unsigned short calculateTotalStock(Car carsOnSale[], unsigned short capacity)
{
    unsigned short totalStock = 0;
    for (unsigned short i = 0; i < capacity; i++)
    {
        totalStock += carsOnSale[i].carStock;
    }
    printf("     Total stock: %31hu\n", totalStock);
    return totalStock;
}

void updateStatisticsAfterSale(Statistics* stats, Car carsOnSale[], unsigned short index, Customer* c,
                               float currentTotalPrice, bool ageDiscountApplied, bool membershipDiscountApplied)
{
    stats->carsInStock -= CAR_WANTED;
    carsOnSale[index].carStock--; // mark the car as sold

    stats->totalIncome += currentTotalPrice; // Total generated income

    float discountThisSale = carsOnSale[index].carPrice - currentTotalPrice; //  given discount just for this sale
    stats->totalDiscountGiven += discountThisSale; // add this sale's discount to total discount

    unsigned short s = stats->numberOfSales;

    strcpy(stats->sales[s].customerName, c->currentCustomerName);
    stats->sales[s].customerAge = c->currentCustomerAge;

    strcpy(stats->sales[s].carModel, carsOnSale[index].carModel);
    stats->sales[s].carYear = carsOnSale[index].carYear;
    stats->sales[s].finalPrice = currentTotalPrice;
    stats->sales[s].discountAmount = discountThisSale;

    if (discountThisSale == 0.0f)
    {
        strcpy(stats->sales[s].discountType, "No discount");
    }
    else if (ageDiscountApplied && membershipDiscountApplied)
    {
        strcpy(stats->sales[s].discountType, "Age + Membership");
    }
    else if (ageDiscountApplied)
    {
        strcpy(stats->sales[s].discountType, "Age");
    }
    else if (membershipDiscountApplied)
    {
        strcpy(stats->sales[s].discountType, "Membership");
    }
    time(&stats->sales[s].saleDate);


    stats->numberOfSales++; // increase number of sales after each successful sale
}

void clearScreen(void)
{
    system("cls");
}

void carsOnSaleList(bool showSoldCars)
{
    printf("\n===================== CARS ON SALE =====================\n\n");
    if (showSoldCars)
    {
        printf("%-3s  %-20s  %-6s  %-10s  %-10s\n", "ID", "Model", "Year", "Price", "Stock");
        // Header row (-) left aligns the text
        printf("-----------------------------------------------------------\n");

        for (unsigned short i = 0; i < MAX_MODEL; i++)
            if (carsOnSale[i].carModel[0] != '\0')
            {
                printf("%-3hu  %-20s  %-6hu  %-10.2f  %-10hu\n",
                       i + 1,
                       carsOnSale[i].carModel,
                       carsOnSale[i].carYear,
                       carsOnSale[i].carPrice,
                       carsOnSale[i].carStock);
            }
        printf("-----------------------------------------------------------\n");
    }
    else
    {
        printf("%-3s  %-20s  %-6s  %-10s  %-10s\n", "ID", "Model", "Year", "Price", "Status");
        printf("-----------------------------------------------------------\n");
        for (unsigned short i = 0; i < MAX_MODEL; i++)
            if (carsOnSale[i].carModel[0] != '\0')
            {
                printf("%-3hu  %-20s  %-6hu  %-10.2f  %-10s\n",
                       i + 1,
                       carsOnSale[i].carModel,
                       carsOnSale[i].carYear,
                       carsOnSale[i].carPrice,
                       carsOnSale[i].carStock > 0 ? "Available" : "Sold");
            }
    }
}

void showBanner(void)
{
    printf("========================================\n");
    printf("     WELCOME TO CHELEBI'S GARAGE  \n");
    printf("========================================\n");
    printf("      Quality Cars. Fair Deals.\n");
    printf("========================================\n");
}

void showMenu(void)
{
    printf("\n1. View Cars on Sale\n");
    printf("2. Buy a Car\n");
    printf("3. View Sales Statistics\n");
    printf("4. Log out\n");
}

void nameValidation(Customer* c)
{
    do
    {
        clearScreen();
        validName = true; // ilk seferde isim dogru olursa, dongu cikis sarti tamamlanir
        for (int i = 0; c->currentCustomerName[i]; i++)
        {
            // isalpha() requires unsigned char; prevents undefined behavior
            if (!isalpha((unsigned char)c->currentCustomerName[i]) && c->currentCustomerName[i] != ' ')
            {
                showBanner();
                printf("Please enter a valid name using letters only: ");
                scanf(" %100[^\n]", c->currentCustomerName);
                while (getchar() != '\n');
                validName = false;
                break;
            }
        }
    }
    while (validName == false);
}

void nameToUpperCase(char* name)
{
    name[0] = toupper(name[0]);
    for (int i = 1; name[i]; i++)
    {
        name[i] = tolower(name[i]);
    }
}

unsigned short carIdValidation(Car carsOnSale[], unsigned short capacity, Customer *customer)
{
    unsigned short selectedId = 0;
    bool validChoice = false;
    do
    {
        scanf("%hu", &selectedId);
        while (getchar() != '\n');


        if (selectedId < 1 || selectedId > countCarModels(carsOnSale, MAX_MODEL))
        {
            clearScreen();
            carsOnSaleList(false);
            printf("\nInvalid car ID. Please choose an available car ID: ");
            continue; // goes to the start of the loop, skips rest
        }
        if (carsOnSale[selectedId - 1].carStock == 0) //Check if that car is already sold
        {
            printf("\nSorry, %s, this car is already sold!\n", customer->currentCustomerName);
            printf("Please choose another car: ");
            continue; // goes to the start of the loop, skips rest
        }
        validChoice = true;
    }
    // if the user enters an invalid ID, it will ask again until a valid ID is entered
    while (validChoice == false);
    return selectedId;
}

void sortByPriceDescending(Car carsOnSale[], unsigned short capacity)
{
    for (unsigned short i = 0; i < capacity - 1; i++)
    {
        for (unsigned short j = i+1; j < capacity; j++)
        {
            if (carsOnSale[i].carPrice < carsOnSale[j].carPrice)
            {
                Car temp = carsOnSale[i];
                carsOnSale[i] = carsOnSale[j];
                carsOnSale[j] = temp;
            }
        }
    }
}

void sortByPriceAscending(Car carsOnSale[], unsigned short capacity)
{
    for (unsigned short i = 0; i < capacity - 1; i++)
    {
        for (unsigned short j = i+1; j < capacity; j++)
        {
            if (carsOnSale[i].carPrice > carsOnSale[j].carPrice)
            {
                Car temp = carsOnSale[i];
                carsOnSale[i] = carsOnSale[j];
                carsOnSale[j] = temp;
            }
        }
    }
}

void sortByStockDescending(Car carsOnSale[], unsigned short capacity)
{
    for (unsigned short i = 0; i < capacity - 1; i++)
    {
        for (unsigned short j = i+1; j < capacity; j++)
        {
            if (carsOnSale[i].carStock < carsOnSale[j].carStock)
            {
                Car temp = carsOnSale[i];
                carsOnSale[i] = carsOnSale[j];
                carsOnSale[j] = temp;
            }
        }
    }
}

void sortByStockAscending(Car carsOnSale[], unsigned short capacity)
{
    for (unsigned short i = 0; i < capacity - 1; i++)
    {
        for (unsigned short j = i+1; j < capacity; j++)
        {
            if (carsOnSale[i].carStock > carsOnSale[j].carStock)
            {
                Car temp = carsOnSale[i];
                carsOnSale[i] = carsOnSale[j];
                carsOnSale[j] = temp;
            }
        }
    }
}

void showSortMenu(void)
{
    printf("1. Price descending\n");
    printf("2. Price ascending\n");
    printf("3. Stock descending\n");
    printf("4. Stock ascending\n");
}
void sortMenuOptionValidation(unsigned short* sortMenuOption)
{
    do
    {
        scanf(" %hu", sortMenuOption);
        while (getchar() != '\n');
        if (*sortMenuOption < 1 || *sortMenuOption > 4)
        {
            clearScreen();
            showBanner();
            showSortMenu();
            printf("Invalid option. Please choose between 1-4: ");
        }
    }while (*sortMenuOption < 1 || *sortMenuOption > 4);
}

void customerFeedback(Sale *saleFeedback)
{
    printf("\nPlease rate your experience!\n");
    printf("5 *****  (Excellent)\n");
    printf("4 ****   (Good)\n");
    printf("3 ***    (Average)\n");
    printf("2 **     (Poor)\n");
    printf("1 *      (Very Poor)\n");
    printf("Your choice (1-5): ");

    scanf("%hu", &saleFeedback->customerFeedbackRating);
    while (getchar() != '\n');

    printf("Write a short comment or press Enter to skip:\n");
    //Reads a full line including spaces and prevents buffer overflow.
    fgets(saleFeedback->customerFeedback, sizeof(saleFeedback->customerFeedback), stdin);

    printf("\nThank you for your feedback!\n");
}

int main(void)
{
    Customer currentCustomer = {0};
    Statistics statistics = {.carsInStock = calculateTotalStock(carsOnSale, MAX_MODEL)};


    system("color 0E"); // black background, yellow text

    do
    {
        clearScreen();
        showBanner();

        printf("Please enter your name: ");
        scanf(" %100[^\n]", currentCustomer.currentCustomerName);
        // reads up to 100 characters what the user types until Enter, but does not take the Enter key
        while (getchar() != '\n'); // clear the buffer and removes all leftover characters including the Enter key


        nameValidation(&currentCustomer);

        nameToUpperCase(currentCustomer.currentCustomerName);

        do
        {
            clearScreen();
            showBanner();
            showMenu();

            printf("\nChoose an option between 1-4 %s: ", currentCustomer.currentCustomerName);
            scanf("%hu", &menuOption);
            while (getchar() != '\n'); // clear the buffer and removes all leftover characters including the Enter key

            clearScreen();

            switch (menuOption)
            {
            case MENU_OPTION_CARS_ON_SALE:

                unsigned short sortMenuOption = 0;
                clearScreen();
                showBanner();
                showSortMenu();
                printf("How would you like to sort the cars on sale: ");
                sortMenuOptionValidation(&sortMenuOption);

                switch (sortMenuOption)
                {
                case 1:
                    sortByPriceDescending(carsOnSale, countCarModels(carsOnSale, MAX_MODEL));
                    break;
                case 2:
                    sortByPriceAscending(carsOnSale, countCarModels(carsOnSale, MAX_MODEL));
                    break;
                case 3:
                    sortByStockDescending(carsOnSale, countCarModels(carsOnSale, MAX_MODEL));
                    break;
                case 4:
                    sortByStockAscending(carsOnSale, countCarModels(carsOnSale, MAX_MODEL));
                    break;
                }

                carsOnSaleList(true);
                calculateTotalStock(carsOnSale, MAX_MODEL);

                break;

            case MENU_OPTION_BUY_CAR:

                if (statistics.carsInStock == 0)
                {
                    printf("\nSorry, %s all cars are sold out at the moment!\n", currentCustomer.currentCustomerName);
                    printf("Please check back later when we restock.\n");
                    break; // goes back to menu
                }
                sortByPriceDescending(carsOnSale, countCarModels(carsOnSale, MAX_MODEL));
                carsOnSaleList(false);

                printf("\nCurrently, we have %hu cars in stock.\n", statistics.carsInStock);
                printf("And %hu different car models available for sale.\n", countCarModels(carsOnSale, MAX_MODEL));

                printf("\nWhich car would you like to buy %s? ", currentCustomer.currentCustomerName);

                unsigned short selectedId = carIdValidation(carsOnSale, MAX_MODEL, &currentCustomer);

                unsigned short index = selectedId - 1; // array index starts from 0, so we subtract 1
                printf("You have selected: %s, %hu\n", carsOnSale[index].carModel, carsOnSale[index].carYear);

                do
                {
                    printf("\nHow old are you %s? Age: ", currentCustomer.currentCustomerName);
                    scanf("%hu", &currentCustomer.currentCustomerAge);
                    while (getchar() != '\n');
                    if (currentCustomer.currentCustomerAge < 1 || currentCustomer.currentCustomerAge > 120)
                    {
                        clearScreen();
                        printf("Invalid age. Please enter a valid age between 1 and 120: ");
                    }
                }
                while (currentCustomer.currentCustomerAge < 1 || currentCustomer.currentCustomerAge > 120);

                if (currentCustomer.currentCustomerAge < 18)
                {
                    printf("\nSorry %s, you must be at least 18 years old to buy a car.\n",
                           currentCustomer.currentCustomerName);
                    menuOption = MENU_OPTION_EXIT; // eger exit olmazsa, yasi kucuk kullanici sistemde kalir
                    break; // exits switch, goes back to do while
                }


                printf("\nDo you have a Chelebi Garage membership card? (Y/N): ");
                scanf(" %c", &membership);
                while (getchar() != '\n');
                // clear the buffer and removes all leftover characters including the Enter key
                clearScreen();


                if (membership == 'Y' || membership == 'y')
                {
                    currentCustomer.currentCustomerIsMember = true;
                    printf("\nMembership confirmed \n");
                }
                else
                {
                    currentCustomer.currentCustomerIsMember = false;
                }

                //local variables
                float currentTotalPrice = 0.0;
                bool ageDiscountApplied = false;
                bool membershipDiscountApplied = currentCustomer.currentCustomerIsMember;

                currentTotalPrice = carsOnSale[index].carPrice; // original car price

                //check discount and update and sale statistics
                if (currentCustomer.currentCustomerAge >= DISCOUNT_MIN_AGE && currentCustomer.currentCustomerAge <=
                    DISCOUNT_MAX_AGE)
                {
                    ageDiscountApplied = true;
                    printf("Great news %s, You get a 20%% age discount.\n", currentCustomer.currentCustomerName);
                    currentTotalPrice *= DISCOUNT_20; // gives %20 by = *0.8
                    if (currentCustomer.currentCustomerIsMember == true)
                    {
                        printf("Plus, your Chelebi Garage membership gives you an extra 10%% discount.\n");
                        currentTotalPrice *= EXTRA10; // gives extra %10 =*0.9
                    }
                    printf("Final price after discounts: %.2f GBP\n", currentTotalPrice);
                }

                else if (currentCustomer.currentCustomerIsMember == true)
                {
                    printf("Thank you for being a Chelebi Garage member, %s.\n", currentCustomer.currentCustomerName);
                    printf("\nYou get a 10%% membership discount on your purchase.\n");
                    currentTotalPrice *= EXTRA10;
                    printf("\nFinal price after membership discount: %.2f GBP\n", currentTotalPrice);
                }

                else
                {
                    printf("No discount applied for this purchase.\n");
                    printf("Total price: %.2f GBP\n", currentTotalPrice);
                }

                updateStatisticsAfterSale(&statistics, carsOnSale, index, &currentCustomer, currentTotalPrice,
                                          ageDiscountApplied, membershipDiscountApplied);

                printf("Cars left in stock: %hu\n", statistics.carsInStock);

                customerFeedback(&statistics.sales[statistics.numberOfSales - 1]);

                menuOption = MENU_OPTION_EXIT; // end the menu session for this customer
                break; // buying options finished


            case MENU_OPTION_SALE_STATS:
                printf("\n=== Sales Summary ===\n");
                printf("Total customers served : %hu\n", statistics.numberOfSales);
                printf("Total income           : %.2f GBP\n", statistics.totalIncome);
                printf("Total discount given   : %.2f GBP\n\n", statistics.totalDiscountGiven);

                if (statistics.numberOfSales == 0)
                {
                    printf("No customers served yet.\n");
                }
                else
                {
                    printf("%-3s %-15s %-5s %-18s %-6s %-18s %-12s %-20s\n",
           "No", "Customer", "Age", "Car Model", "Year",
           "Discount Type", "Disc GBP", "Date / Time");
                    printf("---------------------------------------------------------------------------------------------------------------\n");

                    for (unsigned short i = 0; i < statistics.numberOfSales; i++)
                    {
                        Sale s = statistics.sales[i];

                        char timeOnSale[20];
                        struct tm* localTime = localtime(&s.saleDate);
                        strftime(timeOnSale, sizeof(timeOnSale), "%d/%m/%Y %H:%M:%S", localTime);

                        printf("%-3hu %-15s %-5hu %-18s %-6hu %-18s %-12.2f %-20s\n",
                               i + 1,
                               s.customerName,
                               s.customerAge,
                               s.carModel,
                               s.carYear,
                               s.discountType,
                               s.discountAmount,
                               timeOnSale);
                        printf("    Rating : %hu/5\n", s.customerFeedbackRating);
                        if (strlen(s.customerFeedback) > 1) // checks if there is a comment other than just newline
                            printf("    Comment: %s\n", s.customerFeedback);
                        else
                            printf("    Comment: (no comment)\n");

                        printf("\n");
                    }
                }
                break;

            default:
                do
                {
                    showBanner();
                    showMenu();
                    printf("\nInvalid option.");
                    printf("\nPlease choose an option between 1-4 %s: ", currentCustomer.currentCustomerName);
                    scanf("%hu", &menuOption);
                    while (getchar() != '\n');
                    // clear the buffer and removes all leftover characters including the Enter key
                    clearScreen();
                }
                while (menuOption < 1 || menuOption > 4);
            }


            printf(menuOption == MENU_OPTION_EXIT
                       ? "\n\nPress Enter ..."
                       : "\n\nPress Enter to return to the Menu...");

            getchar(); // waits an entry from user
            clearScreen();
        }
        while (menuOption != MENU_OPTION_EXIT);
        // loop continues while option is NOT EXIT (4). If user choose EXIT, loop stops.
        printf("\nDo you wish to quit? (Y/N): ");
        char quitChoice;
        scanf(" %c", &quitChoice);
        while (getchar() != '\n');

        if (quitChoice == 'Y' || quitChoice == 'y')
        {
            break;
        }

        clearScreen();
    }
    while (1);
    clearScreen();
    printf("See you next time %s!\n", currentCustomer.currentCustomerName);
    return 0;
}
