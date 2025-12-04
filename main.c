#include <stdio.h>
#include <stdlib.h> // system() function
#include <string.h>
#include <ctype.h> // toupper(), tolower(), isAlpha
#include <stdbool.h>
#include <time.h> //localtime function

#define DISCOUNT_MIN_AGE 22
#define DISCOUNT_MAX_AGE 25
#define EXTRA10 0.9f
#define DISCOUNT_20 0.8f
#define MENU_OPTION_CARS_ON_SALE 1
#define MENU_OPTION_BUY_CAR 2
#define MENU_OPTION_SALE_STATS 3
#define MENU_OPTION_EXIT 4
#define MAX_MODEL 100// maximum number of car models and also max stored sales
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

Customer currentCustomer = {0};

typedef struct
{
    char customerName[101];
    unsigned short customerAge;
    char carModel[101];
    unsigned short carYear;
    float finalPrice; // final price after discounts
    float discountAmount;
    char discountType[51]; // "No_discount", "Age", "Membership", "Age_And_Membership"
    time_t saleDate;
    unsigned short customerFeedbackRating; // 1 to 5
    char customerFeedback[201];
} Sale; // represents a single completed sale (used for statistics)


typedef struct
{
    unsigned short carsInStock;
    unsigned short numberOfSales;
    float totalIncome; // total income from all sales
    float totalDiscountGiven;
    Sale sales[MAX_MODEL]; // array storing each completed sale record
} Statistics; // represents all sales

Statistics statistics = {0};


typedef struct
{
    char carModelName[101];
    unsigned short carModelYear;
    float carPrice;
    unsigned short carStock;
} Car;

Car carsOnSale[MAX_MODEL] = {
    {.carModelName = "Ford Fiesta", .carModelYear = 2017, .carPrice = 4800, .carStock = 3},
    {.carModelName = "Toyota Corolla", .carModelYear = 2016, .carPrice = 6000, .carStock = 2},
    {.carModelName = "Volkswagen Golf", .carModelYear = 2015, .carPrice = 5900, .carStock = 1},
    {.carModelName = "Hyundai i30", .carModelYear = 2016, .carPrice = 5000, .carStock = 1},
    {.carModelName = "Kia Ceed", .carModelYear = 2017, .carPrice = 5400, .carStock = 4},
    {.carModelName = "BMW 320d", .carModelYear = 2014, .carPrice = 6900, .carStock = 2},
    {.carModelName = "Mercedes A180", .carModelYear = 2015, .carPrice = 7100, .carStock = 1},
    {.carModelName = "Audi A3", .carModelYear = 2015, .carPrice = 7300, .carStock = 1},
    {.carModelName = "Nissan Qashqai", .carModelYear = 2015, .carPrice = 6200, .carStock = 2},
    {.carModelName = "Honda Civic", .carModelYear = 2016, .carPrice = 6400, .carStock = 3},
    //{.carModel = "Abc", .carYear = 2016, .carPrice = 10000, .carStock = 10}
};

// enam used to give names to constant numbers
typedef enum
{
    SORT_PRICE,
    SORT_YEAR,
    SORT_STOCK
} SortType;

FILE* createFile(char* fileName)
{
    file = fopen(fileName, "w"); // creates the file or resets it if it is already exists

    if (file != NULL) // if the file created, file will not be NULL
    {
        fclose(file);
    }
    return file; // return NULL if creation failed, non NULL if success
}

// Tries to open the file, creates it if needed, then retries opening.
void openFile(char* fileName, char* mode)
{
    file = fopen(fileName, mode); // first try to open as normal

    if (file == NULL) // if file is not open
    {
        if (createFile(fileName) == NULL) // will try to create file and if unsuccesuful == NULL
        {
            printf("There was an error trying to create the file %s.", fileName);
            fileStatus = FILE_ERROR;
        }
        else
        {
            openFile(fileName, mode); // if file created, function calling itself to retry opening (recursive)
        }
    }
    else
    {
        fileStatus = FILE_OPENED;
    }
}

// if the file is open, this function closes it.
void closeFile()
{
    if (fileStatus == FILE_OPENED)
    {
        fclose(file);
        fileStatus = FILE_CLOSED;
    }
}

// reads all sales data from the already opened global FILE *file into Statistics and recalculates
void readDataFromFile(Statistics* stats)
{
    int lineCounter = 0; // counts how many sales have been read from the file
    stats->numberOfSales = 0; // reset count so we rebuild it from the file data
    stats->totalIncome = 0.0f;
    stats->totalDiscountGiven = 0.0f;

    while (1)
    {
        if (lineCounter >= MAX_MODEL) // Prevents reading more lines
        {
            printf("Maximum number of sales reached. No more sales can be recorded.\n");
            break;
        }

        // Pointer to the next Sale slot inside the Statistics struct
        Sale* sale = &stats->sales[lineCounter];

        // used long long to read timestamp correctly.
        long long saleDate = 0;

        //  "%100[^\"]" read up to 100 characters until a quote, %50s read up to 50 chars without space, '^' exclude
        int scanResult = fscanf(
            file,
            " \"%100[^\"]\" %hu \"%100[^\"]\" %hu %f %f %50s %lld %hu \"%200[^\"]\"",
            sale->customerName,
            &sale->customerAge,
            sale->carModel,
            &sale->carYear,
            &sale->finalPrice,
            &sale->discountAmount,
            sale->discountType,
            &saleDate,
            &sale->customerFeedbackRating,
            sale->customerFeedback
        );
        // If reached end of file or there was an error it stops reading
        if (scanResult == EOF)
        {
            break;
        }

        // Store the read timestamp into the struct as time_t
        sale->saleDate = saleDate;

        // Update totals using the sale just read
        stats->totalIncome += sale->finalPrice;
        stats->totalDiscountGiven += sale->discountAmount;

        /*int carIndex = findCarByModelAndYear(carsOnSale, MAX_MODEL, sale->carModel, sale->carYear);
        if (carIndex >= 0 && carsOnSale[carIndex]carStock > 0)
        {
            carsOnSale[carIndex].carStock--;
            stats->carsInStock--;
        }*/

        lineCounter++;// Move to the next sale
    }
    // After reading all lines, store how many sales successfully read
    stats->numberOfSales = lineCounter;
}

// Opens the CSV file in read mode and loads sales data into statistics
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
    }
    closeFile();
}

// Writes all sales from the 'statistics' variable into the FILE
void writeDataToFile()
{
    for (unsigned short i = 0; i < statistics.numberOfSales; i++)
    {
        Sale* sale = &statistics.sales[i];
        fprintf(file, "\"%s\" %hu \"%s\" %hu %.2f %.2f %s %lld %hu \"%s\"\n",
                sale->customerName,
                sale->customerAge,
                sale->carModel,
                sale->carYear,
                sale->finalPrice,
                sale->discountAmount,
                sale->discountType,
                (long long)sale->saleDate,// time_t on this system is implemented as long long.
                sale->customerFeedbackRating,
                sale->customerFeedback
        );
    }
}

// Opens file in write mode and saves current statistics to memory
void saveDataToFile()
{
    openFile(CSV_FILE, "w");

    if (fileStatus == FILE_OPENED)
    {
        writeDataToFile();
    }
    else if (fileStatus == FILE_ERROR)
    {
        printf("There was an error trying to write to the file %s.", CSV_FILE);
        getchar();
        getchar();
    }
    closeFile();
}

// Counts how many car models are actually in use
unsigned short countCarModels(Car carsOnSale[], unsigned short capacity)
{
    unsigned short count = 0;
    for (unsigned short i = 0; i < capacity; i++)
    {
        if (carsOnSale[i].carModelName[0] != '\0') // if the first character is null. there is no carModel exists
        {
            count++;
        }
    }
    return count;
}

// sum of all carStock values
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
// after successful sale updates statistics
void updateStatisticsAfterSale(Statistics* stats, Car carsOnSale[], unsigned short index, Customer* c,
                               float currentTotalPrice, bool ageDiscountApplied, bool membershipDiscountApplied)
{
    stats->carsInStock -= CAR_WANTED;
    carsOnSale[index].carStock--; // decrease stock for the specific car model

    stats->totalIncome += currentTotalPrice; // add this sale's income to total income

    float discountThisSale = carsOnSale[index].carPrice - currentTotalPrice; //  given discount just for this sale
    stats->totalDiscountGiven += discountThisSale; // add this sale's discount to total discount

    unsigned short s = stats->numberOfSales; // index where for storing the new sale

    // copy temporary customer information into the new sale entry(permanent for record)
    strcpy(stats->sales[s].customerName, c->currentCustomerName);
    stats->sales[s].customerAge = c->currentCustomerAge;

    // copy the selected car's model name into this sale record (which car was sold)
    strcpy(stats->sales[s].carModel, carsOnSale[index].carModelName);
    stats->sales[s].carYear = carsOnSale[index].carModelYear;
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

    // store the current system time into saleDate for this Sale (when the sale happened)
    time(&stats->sales[s].saleDate);


    stats->numberOfSales++; // increase number of sales after each successful sale
}

int findCarByModelAndYear(Car carsOnsale[], unsigned short capacity, char* model, unsigned short year)
{
    for (unsigned short i = 0; i < capacity; i++)
    {
        if (carsOnsale[i].carModelName[0] != '\0' && carsOnsale[i].carModelYear == year && strcmp(carsOnsale[i].carModelName, model)== 0)
        {
            return i; // return index of matching car
        }
    }
    return -1; // means not found. No array ever has index (-1)
}

void clearScreen(void)
{
    system("cls"); // for Windows
}

void clearBuffer(void)
{
    // clear the buffer and removes all leftover characters including the enter key
    while (getchar() != '\n');
}

void carsOnSaleList(bool showSoldCars)
{
    printf("\n===================== CARS ON SALE =====================\n\n");
    printf("%-3s  %-20s  %-6s  %-10s  %-10s\n", "ID", "Model", "Year", "Price", "Status");
    printf("-----------------------------------------------------------\n");
    for (unsigned short i = 0; i < MAX_MODEL; i++)
        if (carsOnSale[i].carModelName[0] != '\0')
        {
            printf("%-3hu  %-20s  %-6hu  %-10.2f  ",
                   i + 1,
                   carsOnSale[i].carModelName,
                   carsOnSale[i].carModelYear,
                   carsOnSale[i].carPrice);
            if (showSoldCars)
            {
                // show how many units are left for this model
                printf("%-10hu", carsOnSale[i].carStock);
            }
            else
            {
                // show whether the car is still available or sold out
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
        validName = true; // assume name is valid initially
        for (int i = 0; c->currentCustomerName[i]; i++)
        {
            // if this character is not a letter and not a space, then it is invalid.
            if (isalpha(c->currentCustomerName[i]) == 0 && c->currentCustomerName[i] != ' ')
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

// first character uppercase, the rest lowercase.
void nameToUpperCase(char* name)
{
    name[0] = toupper(name[0]);
    for (int i = 1; name[i]; i++)
    {
        name[i] = tolower(name[i]);
    }
}

// Validates selected car ID by checking range and availability
unsigned short carIdValidation(Car carsOnSale[], Customer* customer)
{
    unsigned short selectedId = 0;
    bool validChoice = false;
    do
    {
        scanf("%hu", &selectedId);
        clearBuffer();

        // check if ID is within valid range
        if (selectedId < 1 || selectedId > countCarModels(carsOnSale, MAX_MODEL))
        {
            clearScreen();
            carsOnSaleList(false);
            printf("\nInvalid car ID. Please choose an available car ID: ");
            continue; // goes to the start of the loop, skips rest
        }

        //Check if that car is already sold
        if (carsOnSale[selectedId - 1].carStock == 0)
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

void sortAscending(Car carsOnSale[], unsigned short capacity, SortType type)
{
    clearScreen();
    for (int i = 0; i < capacity - 1; i++)
    {
        for (int j = i + 1; j < capacity; j++)
        {
            float leftValue; // float because of the price
            float rightValue;

            if (type == SORT_PRICE)
            {
                leftValue = carsOnSale[i].carPrice;
                rightValue = carsOnSale[j].carPrice;
            }
            else if (type == SORT_STOCK)
            {
                leftValue = carsOnSale[i].carStock;
                rightValue = carsOnSale[j].carStock;
            }
            else if (type == SORT_YEAR)
            {
                leftValue = carsOnSale[i].carModelYear;
                rightValue = carsOnSale[j].carModelYear;
            }
            if (leftValue > rightValue)
            {
                Car temp = carsOnSale[i];
                carsOnSale[i] = carsOnSale[j];
                carsOnSale[j] = temp;
            }
        }
    }
}

void sortDescending(Car carsOnSale[], unsigned short capacity, SortType type)
{
    clearScreen();
    for (int i = 0; i < capacity - 1; i++)
    {
        for (int j = i + 1; j < capacity; j++)
        {
            float leftValue; // float because of the price
            float rightValue;

            if (type == SORT_PRICE)
            {
                leftValue = carsOnSale[i].carPrice;
                rightValue = carsOnSale[j].carPrice;
            }
            else if (type == SORT_STOCK)
            {
                leftValue = carsOnSale[i].carStock;
                rightValue = carsOnSale[j].carStock;
            }
            else if (type == SORT_YEAR)
            {
                leftValue = carsOnSale[i].carModelYear;
                rightValue = carsOnSale[j].carModelYear;
            }
            if (leftValue < rightValue)
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
    printf("5. Year descending\n");
    printf("6. Year ascending\n");
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
            break;// valid value, exit loop
        }
        printf("Please enter a valid value between %hu and %hu: ", min, max);
    }
    while (1);
}


int main(void)
{
    system("color 0E"); // black background, yellow text for Windows

    getDataFromFile(); // load previous sales data from file into 'statistics'

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
                captureValueAndValidate(&sortMenuOption, 1, 6);

                switch (sortMenuOption)
                {
                case 1:
                    sortDescending(carsOnSale, countCarModels(carsOnSale, MAX_MODEL), SORT_PRICE);
                    break;
                case 2:
                    sortAscending(carsOnSale, countCarModels(carsOnSale, MAX_MODEL), SORT_PRICE);
                    break;
                case 3:
                    sortDescending(carsOnSale, countCarModels(carsOnSale, MAX_MODEL), SORT_STOCK);
                    break;
                case 4:
                    sortAscending(carsOnSale, countCarModels(carsOnSale, MAX_MODEL), SORT_STOCK);
                    break;
                case 5:
                    sortDescending(carsOnSale, countCarModels(carsOnSale, MAX_MODEL), SORT_YEAR);
                    break;
                case 6:
                    sortAscending(carsOnSale, countCarModels(carsOnSale, MAX_MODEL), SORT_YEAR);
                    break;
                }

                carsOnSaleList(true); // show full list with stock numbers
                calculateTotalStock(carsOnSale, MAX_MODEL); // recalculate and display total stock

                break;

            case MENU_OPTION_BUY_CAR:

                // If there are no cars in stock, show message and go back to menu
                if (statistics.carsInStock == 0)
                {
                    printf("\nSorry, %s all cars are sold out at the moment!\n", currentCustomer.currentCustomerName);
                    printf("Please check back later when we restock.\n");
                    break; // goes back to menu
                }
                // most expensive appears first
                sortDescending(carsOnSale, countCarModels(carsOnSale, MAX_MODEL), SORT_PRICE);

                carsOnSaleList(false); // show available/sold status

                printf("\nTotal cars in stock      : %hu\n", statistics.carsInStock);
                printf("Models currently on sale : %hu\n", countCarModels(carsOnSale, MAX_MODEL));

                printf("\nWhich car would you like to buy %s? ", currentCustomer.currentCustomerName);

                unsigned short selectedId = carIdValidation(carsOnSale, &currentCustomer);

                unsigned short index = selectedId - 1; // array index starts from 0, so subtract 1
                printf("You have selected: %s, %hu\n", carsOnSale[index].carModelName, carsOnSale[index].carModelYear);

                printf("\nHow old are you %s? Age: ", currentCustomer.currentCustomerName);

                // validate age
                captureValueAndValidate(&currentCustomer.currentCustomerAge, 1, 99);

                if (currentCustomer.currentCustomerAge < 18)
                {
                    printf("\nSorry %s, you must be at least 18 years old to buy a car.\n",
                           currentCustomer.currentCustomerName);
                    // if we do not exit the menu, underage user would remain in the system
                    menuOption = MENU_OPTION_EXIT;
                    saveDataToFile();
                    break; // exit this case and go back to menu loop
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

                //local variables for discount
                float currentTotalPrice;
                bool ageDiscountApplied = false;
                bool membershipDiscountApplied = currentCustomer.currentCustomerIsMember;

                currentTotalPrice = carsOnSale[index].carPrice; // start with original car price

                // check age discount and update statistics
                if (currentCustomer.currentCustomerAge >= DISCOUNT_MIN_AGE && currentCustomer.currentCustomerAge <=
                    DISCOUNT_MAX_AGE)
                {
                    ageDiscountApplied = true;
                    printf("Great news %s, You get a 20%% age discount.\n", currentCustomer.currentCustomerName);
                    currentTotalPrice *= DISCOUNT_20;
                    if (currentCustomer.currentCustomerIsMember == true)
                    {
                        printf("Plus, your Chelebi Garage membership gives you an extra 10%% discount.\n");
                        currentTotalPrice *= EXTRA10;
                    }
                    printf("Final price after discounts: %.2f GBP\n", currentTotalPrice);
                }

                else if (currentCustomer.currentCustomerIsMember == true)
                {
                    printf("Thank you for being a Chelebi Garage member, %s.\n", currentCustomer.currentCustomerName);
                    printf("\nYou get a 10%% membership discount on your purchase.\n");
                    currentTotalPrice *= EXTRA10; // membership discount only
                    printf("\nFinal price after membership discount: %.2f GBP\n", currentTotalPrice);
                }

                else
                {
                    printf("No discount applied for this purchase.\n");
                    printf("Total price: %.2f GBP\n", currentTotalPrice);
                }

                // apply sale to statistics and save the sale details
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
                printf("%-3s %-15s %-5s %-18s %-6s %-18s %-12s %-12s %-20s\n",
                       "No", "Customer", "Age", "Car Model", "Year",
                       "Discount Type", "Disc GBP", "Final GBP", "Date / Time");
                printf(
                    "---------------------------------------------------------------------------------------------------------------\n");


                for (unsigned short i = 0; i < statistics.numberOfSales; i++)
                {
                    Sale s = statistics.sales[i];

                    char timeOnSale[20];
                    // convert timestamp to local time structure
                    struct tm* localTime = localtime(&s.saleDate);
                    // format date/time
                    strftime(timeOnSale, sizeof(timeOnSale), "%d/%m/%Y %H:%M:%S", localTime);

                    printf("%-3hu %-15s %-5hu %-18s %-6hu %-18s %-12.2f %-12.2f %-20s\n",
                           i + 1,
                           s.customerName,
                           s.customerAge,
                           s.carModel,
                           s.carYear,
                           s.discountType,
                           s.discountAmount,
                           s.finalPrice,
                           timeOnSale);
                    printf("    Rating : %hu/5\n", s.customerFeedbackRating);
                    if (strlen(s.customerFeedback) > 1) // checks if there is a comment other than just newline
                        printf("    Comment: %s", s.customerFeedback);
                    else
                        printf("    Comment: (no comment)\n");

                    printf(
                        "---------------------------------------------------------------------------------------------------------------\n");

                }

                printf("\n=== Total Sales Per Model ===\n");
                printf("%-20s %-6s %-12s %-12s\n", "Model", "Year", "Units", "Total GBP");
                printf("-------------------------------------------------------------\n");

                unsigned short modelCount = countCarModels(carsOnSale, MAX_MODEL);

                unsigned short units[MAX_MODEL] = {0};
                float revenue[MAX_MODEL] = {0.0f};

                // count how many units sold and revenue for each model/year
                for (unsigned short i = 0; i < modelCount; i++)
                {
                    for (unsigned short s = 0; s < statistics.numberOfSales; s++)
                    {
                        Sale* sale = &statistics.sales[s];

                        // if model and year match, add this sale to that model's totals
                        if (strcmp(sale->carModel, carsOnSale[i].carModelName) == 0 &&
                            sale->carYear == carsOnSale[i].carModelYear)
                        {
                            units[i]++;
                            revenue[i] += sale->finalPrice;
                        }
                    }
                }

                    // Sort models by revenue in descending order
                    for (unsigned short i = 0; i < modelCount -1; i++)
                    {
                        for (unsigned short j = i + 1; j < modelCount; j++)
                        {
                            if (revenue[i] < revenue[j])
                            {
                                // revenue swap to keep highest revenue first
                                float tempR = revenue[i];
                                revenue[i] = revenue[j];
                                revenue[j] = tempR;

                                // units swap to stay in sync with revenue and carsOnSale
                                unsigned short tempU = units[i];
                                units[i] = units[j];
                                units[j] = tempU;

                                // carsOnSale swap so model/year stays aligned with units and revenue
                                Car tempC = carsOnSale[i];
                                carsOnSale[i] = carsOnSale[j];
                                carsOnSale[j] = tempC;
                            }
                        }
                    }

                    // Print only models that have at least one unit sold
                    for (unsigned short i = 0; i < modelCount; i++)
                    {
                        if (units[i] == 0)
                            continue;

                        printf("%-20s %-6hu %-12hu %-12.2f\n",
                           carsOnSale[i].carModelName,
                           carsOnSale[i].carModelYear,
                           units[i],
                           revenue[i]);
                    }
                printf("-------------------------------------------------------------\n");
                break;
            }

            printf(menuOption == MENU_OPTION_EXIT
                       ? "\n\nPress Enter ..."
                       : "\n\nPress Enter to return to the Menu...");

            saveDataToFile(); // save current statistics to file after each main action
            getchar(); // wait for user to press Enter
            clearScreen();
        }
        // loop continues while option is NOT EXIT (4). If user choose EXIT, loop stops.
        while (menuOption != MENU_OPTION_EXIT);

        printf("\nDo you wish to quit? (Y/N): ");
        char quitChoice;
        scanf(" %c", &quitChoice);
        clearBuffer();

        quitChoice = toupper(quitChoice);
        if (quitChoice == 'Y')
        {
            break; // exit outer loop and end program
        }

        clearScreen();
    }
    while (1);
    clearScreen();
    printf("See you next time %s!\n", currentCustomer.currentCustomerName);
    return 0;
}
