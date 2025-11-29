#include <stdio.h>
#include <stdlib.h> // system() function
#include <string.h>
#include <ctype.h> // toupper(), tolower(), isAlpha
#include <stdbool.h>
#include <time.h> //localtime function

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
// File
#define CSV_FILE "salesData.csv"
#define FILE_OPENED 0
#define FILE_CLOSED 1
#define FILE_ERROR 2

FILE* file;
unsigned char fileStatus = FILE_CLOSED; // file starts closed because no file is open yet

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
    char discountType[51]; // "No_discount", "Age", "Membership", "Age_And_Membership"
    time_t saleDate;
    unsigned short customerFeedbackRating; // 1 to 5
    char customerFeedback[201];
} Sale; // represents a single completed sale (used for statistics)

Customer currentCustomer = {0};

typedef struct
{
    unsigned short carsInStock;
    unsigned short numberOfSales;
    float totalIncome;
    float totalDiscountGiven;
    Sale sales[MAX_MODEL];
} Statistics;

Statistics statistics = {0};


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

FILE* createFile(char* fileName)
{
    file = fopen(fileName, "w"); // creates the file or resets it if it already exists

    if (file != NULL) // if the file created, file will not be NULL
    {
        fclose(file);
    }
    return file;
}

// Tries to open the file, creates it if needed, then retries opening.
void openFile(char* fileName, char* mode)
{
    file = fopen(fileName, mode); // first try to open as normal

    if (file == NULL) // if file is not open
    {
        if (createFile(fileName) == NULL) // will try to create file and if unsuccesuful == NULL
        {
            fileStatus = FILE_ERROR;
        }
        else
        {
            openFile(fileName, mode); // if file created, function calling itself (recursive)
        }
    }
    else
    {
        fileStatus = FILE_OPENED;
    }
}

// if the file is open, this function closes it. if it is not open, it does nothing.
void closeFile()
{
    if (fileStatus == FILE_OPENED)
    {
        fclose(file);
        fileStatus = FILE_CLOSED;
    }
}

void readDataFromFile(Statistics *stats)
{
    int lineCounter = 0;
    stats->numberOfSales = 0;
    stats->totalIncome = 0.0f;
    stats->totalDiscountGiven = 0.0f;

    while (1)
    {
        if (lineCounter >= MAX_MODEL)
        {
            printf("Maximum number of sales reached. No more sales can be recorded.\n");
            break;
        }

        Sale *sale = &stats->sales[lineCounter];
        long saleDateLong = 0;

        int scanResult = fscanf(
            file,
            " \"%100[^\"]\" %hu \"%100[^\"]\" %hu %f %f %50s %ld %hu \"%200[^\"]\"",
            sale->customerName,
            &sale->customerAge,
            sale->carModel,
            &sale->carYear,
            &sale->finalPrice,
            &sale->discountAmount,
            sale->discountType,
            &saleDateLong,
            &sale->customerFeedbackRating,
            sale->customerFeedback
        );

        if (scanResult == EOF)
        {
            break;
        }

        sale->saleDate = saleDateLong;

        stats->totalIncome += sale->finalPrice;
        stats->totalDiscountGiven += sale->discountAmount;

        lineCounter++;

    }
    stats->numberOfSales = lineCounter;
}

void getDataFromFile()
{
    openFile(CSV_FILE, "r");
    if (fileStatus == FILE_OPENED)
    {
        readDataFromFile(&statistics);
    }
    else if (fileStatus == FILE_ERROR)
    {
        printf("There was an error trying to read from the file %s.", CSV_FILE);
        getchar();
        getchar();
    }
    closeFile();
}

void writeDataToFile()
{
    for (unsigned short i = 0; i < statistics.numberOfSales; i++)
    {
        Sale *sale = &statistics.sales[i];
        fprintf(file, "\"%s\" %hu \"%s\" %hu %.2f %.2f %s %ld %hu \"%s\"\n",
                sale->customerName,
                sale->customerAge,
                sale->carModel,
                sale->carYear,
                sale->finalPrice,
                sale->discountAmount,
                sale->discountType,
                (long)sale->saleDate,
                sale->customerFeedbackRating,
                sale->customerFeedback
        );
    }
}

void saveDataToFile() {
    openFile(CSV_FILE, "w");

    if (fileStatus == FILE_OPENED) {
        writeDataToFile();
    }
    else if (fileStatus == FILE_ERROR) {
        printf("There was an error trying to write to the file %s.", CSV_FILE);
        getchar();
        getchar();
    }

    closeFile();
}


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
        strcpy(stats->sales[s].discountType, "No_discount");
    }
    else if (ageDiscountApplied && membershipDiscountApplied)
    {
        strcpy(stats->sales[s].discountType, "Age_And_Membership");
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

int findCarByModelAndYear(Car carsOnsale[], unsigned short capacity, char* model, unsigned short year)
{
    for (unsigned short i = 0; i < capacity; i++)
    {
        if (carsOnsale[i].carModel[0] != '\0' && carsOnsale[i].carYear == year && strcmp(carsOnsale[i].carModel, model)
            == 0)
        {
            return i;
        }
    }
    return -1; // means not found. No array ever has index (-1)
}

void clearScreen(void)
{
    system("cls");
}

void clearBuffer(void)
{
    while (getchar() != '\n');
    // clear the buffer and removes all leftover characters including the Enter key
}

void carsOnSaleList(bool showSoldCars)
{
    printf("\n===================== CARS ON SALE =====================\n\n");
    printf("%-3s  %-20s  %-6s  %-10s  %-10s\n", "ID", "Model", "Year", "Price", "Status");
    printf("-----------------------------------------------------------\n");
    for (unsigned short i = 0; i < MAX_MODEL; i++)
        if (carsOnSale[i].carModel[0] != '\0')
        {
            printf("%-3hu  %-20s  %-6hu  %-10.2f  ",
                   i + 1,
                   carsOnSale[i].carModel,
                   carsOnSale[i].carYear,
                   carsOnSale[i].carPrice);
            if (showSoldCars)
            {
                printf("%-10hu", carsOnSale[i].carStock);
            }
            else
            {
                printf("%-10s", carsOnSale[i].carStock > 0 ? "Available" : "Sold");
            }
            printf("\n");
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
            if (!isalpha(c->currentCustomerName[i]) && c->currentCustomerName[i] != ' ')
            {
                showBanner();
                printf("Please enter a valid name using letters only: ");
                scanf(" %100[^\n]", c->currentCustomerName);
                clearBuffer();
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

unsigned short carIdValidation(Car carsOnSale[], unsigned short capacity, Customer* customer)
{
    unsigned short selectedId = 0;
    bool validChoice = false;
    do
    {
        scanf("%hu", &selectedId);
        clearBuffer();


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
        for (unsigned short j = i + 1; j < capacity; j++)
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
        for (unsigned short j = i + 1; j < capacity; j++)
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
        for (unsigned short j = i + 1; j < capacity; j++)
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
        for (unsigned short j = i + 1; j < capacity; j++)
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
        clearBuffer();
        if (*sortMenuOption < 1 || *sortMenuOption > 4)
        {
            clearScreen();
            showBanner();
            showSortMenu();
            printf("Invalid option. Please choose between 1-4: ");
        }
    }
    while (*sortMenuOption < 1 || *sortMenuOption > 4);
}

void customersFeedbackRating()
{
    printf("\nPlease rate your experience!\n");
    printf("5 *****  (Excellent)\n");
    printf("4 ****   (Good)\n");
    printf("3 ***    (Average)\n");
    printf("2 **     (Poor)\n");
    printf("1 *      (Very Poor)\n");
    printf("Your choice (1-5): ");
}

/*void customerFeedbackRatingValidation()
{
    do
    {
        scanf(" %hu", &customerFeedbackRating);
        clearBuffer();
        if (customerFeedbackRating < 1 || customerFeedbackRating > 5)
        {
            clearScreen();
            printf("Invalid rating. Please choose a rating between 1-5: ");
            customerFeedbackRating();
        }
    }while (customerFeedbackRating < 1 || customerFeedbackRating > 5);
}*/

void getCustomerFeedback(Sale* saleFeedback)
{
    printf("Please write a short comment or press Enter to skip:\n");
    //Reads a full line including spaces and prevents buffer overflow.
    fgets(saleFeedback->customerFeedback, sizeof(saleFeedback->customerFeedback), stdin);

    printf("\nThank you for your feedback!\n");
}

void captureValueAndValidate(unsigned short* value, unsigned short min, unsigned short max)
{
    do
    {
        int capturedValue = scanf("%hu", value);
        clearBuffer();
        if (capturedValue == 1 && *value >= min && *value <= max)
        {
            break;
        }
        printf("Please enter a valid value between %hu and %hu: ", min, max);
    }
    while (1);
}

/*char captureYesOrNo(char *input)
{

    int result = scanf(" %c", input); //
    clearBuffer();

    *input = toupper(*input);

    if (result ==1 && *input == 'Y')
    {
        return 'Y';
    }
}*/




int main(void)
{

    system("color 0E"); // black background, yellow text

    getDataFromFile();

    statistics.carsInStock = calculateTotalStock(carsOnSale, MAX_MODEL);

    do
    {
        clearScreen();
        showBanner();

        printf("Please enter your name: ");
        // reads up to 100 characters what the user types until Enter, but does not take the Enter key
        scanf(" %100[^\n]", currentCustomer.currentCustomerName);
        clearBuffer();

        nameValidation(&currentCustomer);

        nameToUpperCase(currentCustomer.currentCustomerName);

        do
        {
            clearScreen();
            showBanner();
            showMenu();
            printf("\nPlease choose an option between 1-4 %s: ", currentCustomer.currentCustomerName);

            captureValueAndValidate(&menuOption, 1, 4);

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
                    clearScreen();
                    printf("\nSorting by price descending...\n");
                    sortByPriceDescending(carsOnSale, countCarModels(carsOnSale, MAX_MODEL));
                    break;
                case 2:
                    clearScreen();
                    printf("\nSorting by price ascending...\n");
                    sortByPriceAscending(carsOnSale, countCarModels(carsOnSale, MAX_MODEL));
                    break;
                case 3:
                    clearScreen();
                    printf("\nSorting by stock descending...\n");
                    sortByStockDescending(carsOnSale, countCarModels(carsOnSale, MAX_MODEL));
                    break;
                case 4:
                    clearScreen();
                    printf("\nSorting by stock ascending...\n");
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

                printf("\nTotal cars in stock      : %hu\n", statistics.carsInStock);
                printf("Models currently on sale : %hu\n", countCarModels(carsOnSale, MAX_MODEL));

                printf("\nWhich car would you like to buy %s? ", currentCustomer.currentCustomerName);

                unsigned short selectedId = carIdValidation(carsOnSale, MAX_MODEL, &currentCustomer);

                unsigned short index = selectedId - 1; // array index starts from 0, so we subtract 1
                printf("You have selected: %s, %hu\n", carsOnSale[index].carModel, carsOnSale[index].carYear);

                printf("\nHow old are you %s? Age: ", currentCustomer.currentCustomerName);

                captureValueAndValidate(&currentCustomer.currentCustomerAge, 1, 99);

                if (currentCustomer.currentCustomerAge < 18)
                {
                    printf("\nSorry %s, you must be at least 18 years old to buy a car.\n",
                           currentCustomer.currentCustomerName);
                    menuOption = MENU_OPTION_EXIT;// eger exit olmazsa, yasi kucuk kullanici sistemde kalir
                    saveDataToFile();
                    break; // exits switch, goes back to do while
                }


                printf("\nDo you have a Chelebi Garage membership card? (Y/N): ");
                scanf(" %c", &membership);
                clearBuffer();
                clearScreen();

                membership = toupper(membership);
                if (membership == 'Y')
                {
                    currentCustomer.currentCustomerIsMember = true;
                    printf("\nMembership confirmed \n");
                }
                else
                {
                    currentCustomer.currentCustomerIsMember = false;
                }

                //local variables
                float currentTotalPrice;
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
                        currentTotalPrice *= EXTRA10; // gives extra %10 = *0.9
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

                customersFeedbackRating();

                captureValueAndValidate(&statistics.sales[statistics.numberOfSales - 1].customerFeedbackRating, 1, 5);


                getCustomerFeedback(&statistics.sales[statistics.numberOfSales - 1]);

                menuOption = MENU_OPTION_EXIT; // end the menu session for this customer
                saveDataToFile();
                break; // buying options finished


            case MENU_OPTION_SALE_STATS:
                printf("\n=== Sales Summary ===\n");
                printf("Total customers served : %hu\n", statistics.numberOfSales);
                printf("Total income           : %.2f GBP\n", statistics.totalIncome);
                printf("Total discount given   : %.2f GBP\n\n", statistics.totalDiscountGiven);

                if (statistics.numberOfSales == 0)
                {
                    printf("No customers served yet.\n");
                    break;
                }
                printf("%-3s %-15s %-5s %-18s %-6s %-18s %-12s %-20s\n",
                       "No", "Customer", "Age", "Car Model", "Year",
                       "Discount Type", "Disc GBP", "Date / Time");
                printf(
                    "---------------------------------------------------------------------------------------------------------------\n");

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

                    printf(
                        "---------------------------------------------------------------------------------------------------------------\n");
                }

                break;
            }


            printf(menuOption == MENU_OPTION_EXIT
                       ? "\n\nPress Enter ..."
                       : "\n\nPress Enter to return to the Menu...");

            saveDataToFile();
            getchar(); // waits an entry from user
            clearScreen();
        }
        while (menuOption != MENU_OPTION_EXIT);
        // loop continues while option is NOT EXIT (4). If user choose EXIT, loop stops.
        printf("\nDo you wish to quit? (Y/N): ");
        char quitChoice;
        scanf(" %c", &quitChoice);
        clearBuffer();

        quitChoice = toupper(quitChoice);
        if (quitChoice == 'Y')
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
