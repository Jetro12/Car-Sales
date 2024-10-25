#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#define DISCOUNT_MIN_Age 55
#define DISCOUNT_MAX_Age  90
#define DISCOUNT_PERCENTAGE 0.15f
#define MAX_SALES 210
#define MAX_FEEDBACK_SIZE 100
#define MENU_OPTION_VIEW_CAR_STOCK  'A'
#define MENU_OPTION_BUY_CARS 'B'
#define MENU_OPTION_VIEW_SALES_DATA  'C'
#define MENU_OPTION_VIEW_CUSTOMER_FEEDBACK  'D'
#define MENU_OPTION_EXIT  'X'
#define TRUE  1
#define FALSE 0

unsigned short CarsAvailable = 0, CarsNeeded = 0, user_Age = 0, discount = FALSE, correctInput = FALSE, CarType;
float totalPrice = 0.0f;

unsigned short numberOfSales = 0;
unsigned short CarAmountPerSale[MAX_SALES];
unsigned short typeOfCarPerSale[MAX_SALES];
unsigned short discountGivenPerSale[MAX_SALES];
char customerNames[MAX_SALES][201];
int ratings[MAX_SALES];
char feedback[MAX_SALES][MAX_FEEDBACK_SIZE];

struct Cars {
    char CarModel[99];
    float CarPrices;
    short CarManufacture_Year;
    short CarsInStock;
};
struct Cars CarStocks[MAX_SALES];
struct Sale {
    char modelName[50];
    int numberOfCars;
    float totalPrice;
};

void clearScreen() {
    system("cls");
}

char getCharFromConsole(char *message) {
    char userInput;
    printf("%s", message);
    scanf("\n%c", &userInput);
    return userInput;
}

unsigned short getUnsignedShortFromConsole(char *message) {
    unsigned short userInput;
    printf("%s", message);
    scanf("%hd", &userInput);
    return userInput;
}

void pauseProgram(char userChoice) {
    if (userChoice == MENU_OPTION_EXIT) {
        printf("\n\nPress Enter to Exit Dealership");
    } else {
        printf("\n\n Press Enter to return to Menu.....");
    }
    getchar();
    getchar();
}

void Greeting_Customer() {
    printf("**************************************************************\n");
    printf("*            Welcome to the Aru Dealership!!                 *\n");
    printf("**************************************************************\n\n");

}

void Menu() {
    printf("\n============================================\n");
    printf("            Aru Dealership Menu          \n");
    printf("============================================\n");
    printf("Menu: \n");
    printf("A. View Cars Stock,\n");
    printf("B. Buy Cars \n");
    printf("C. View Sales Data \n");
    printf("D. Customer Feedback.\n");
    printf("X. Exit \n");
    printf("============================================\n\n");
}

void View_Car_Stock() {
    FILE *file = fopen("Car_stock.txt", "r");
    if (file == NULL) {
        printf("Error opening file for reading.\n");
        exit(EXIT_FAILURE);
    }
    fscanf(file, "%*[^\n]");
    CarsAvailable = 0;
    while (CarsAvailable < MAX_SALES && fscanf(file, "%49[^,],%f,%hd,%hd\n",
                                               CarStocks[CarsAvailable].CarModel,
                                               &CarStocks[CarsAvailable].CarPrices,
                                               &CarStocks[CarsAvailable].CarManufacture_Year,
                                               &CarStocks[CarsAvailable].CarsInStock) == 4) {
        CarsAvailable++;
    }
    fclose(file);

    // Sorting algorithm
    int length = CarsAvailable;
    short temp;
    int swapped;
    do {
        swapped = 0;
        for (int i = 1; i < length; i++) {
            if (CarStocks[i - 1].CarManufacture_Year < CarStocks[i].CarManufacture_Year) {
                // Swap years
                temp = CarStocks[i - 1].CarManufacture_Year;
                CarStocks[i - 1].CarManufacture_Year = CarStocks[i].CarManufacture_Year;
                CarStocks[i].CarManufacture_Year = temp;

                // Swap models
                char modelTemp[50];
                strcpy(modelTemp, CarStocks[i - 1].CarModel);
                strcpy(CarStocks[i - 1].CarModel, CarStocks[i].CarModel);
                strcpy(CarStocks[i].CarModel, modelTemp);

                //swap price
                int priceTemp = CarStocks[i - 1].CarPrices;
                CarStocks[i - 1].CarPrices = CarStocks[i].CarPrices;
                CarStocks[i].CarPrices = priceTemp;

                // Swap remaining cars
                temp = CarStocks[i - 1].CarsInStock;
                CarStocks[i - 1].CarsInStock = CarStocks[i].CarsInStock;
                CarStocks[i].CarsInStock = temp;

                swapped = 1;
            }
        }
    } while (swapped);

    // Display car stock in a user-friendly format
    printf("************************************************************\n");
    printf("Cars in Stock (Sorted by Year in Descending Order)\n");
    printf("************************************************************\n");
    printf("%-25s%-20s%-20s\n", "Model", "Manufacture Year", "Remaining in Stock");
    for (int i = 0; i < CarsAvailable; i++) {
        printf("%-25s%-20d%-20d\n", CarStocks[i].CarModel, CarStocks[i].CarManufacture_Year, CarStocks[i].CarsInStock);
    }
    printf("\n");
}

void DisplayCarTypes(struct Cars *carModel, struct Cars *carPrices) {
    printf("\n===============================================");
    printf("\n           Available Car Types");
    printf("\n===============================================\n");
    printf("%-5s%-25s%-15s%-15s\n", "Index", "Car Model", "Price (GBP)", "Remaining Stock");
    printf("-----------------------------------------------------\n");

    for (int i = 0; i < CarsAvailable; i++) {
        printf("%-5d%-25s%-15.2f%-15d\n", i, carModel[i].CarModel, carPrices[i].CarPrices, carModel[i].CarsInStock);
    }

    printf("\n");
}

void Buy_Cars(struct Sale *sales, int *numSales, struct Cars *carPrices, struct Cars *carModel, struct Cars *carStocks) {

    int isValidName;
    do {
        // Read the name
        printf("What is your name? Your name: ");
        scanf("\n%[^\n]", customerNames[numberOfSales]);

        // Check if the name contains only alphabetic characters
        isValidName = 1;
        for (int i = 0; customerNames[numberOfSales][i] != '\0'; i++) {
            if (!isalpha(customerNames[numberOfSales][i])) {
                isValidName = 0;
                break;
            }
        }
        // Display an error message if the name is not valid
        if (!isValidName) {
            printf("Invalid input. Please enter a name containing only alphabetic characters.\n");
        }
        while (getchar() != '\n');
    } while (!isValidName);

    short min_pur = 1, max_pur = 15;
    correctInput = 0;
    do {
        CarsNeeded = getUnsignedShortFromConsole("\nHow many cars do you want to purchase? Amount: ");
        if (CarsNeeded == 0) {
            printf("\nPlease enter the amount of cars you want to buy only.\n\n");
        } else {
            if (CarsNeeded >= min_pur && CarsNeeded <= max_pur) {
                correctInput = 1;
            } else {
                printf("\nPlease enter the amount of Cars you want to purchase. There are %hd Cars Available",
                       CarsAvailable);
            }
        }
        while (getchar() != '\n');
    } while (correctInput == 0);

    DisplayCarTypes(carModel, carPrices);

    int maxValue = CarsAvailable - 1;
    correctInput = 0;
    do {
        CarType = getUnsignedShortFromConsole("Select the index of the Car Model you want to purchase: ");
        if (CarType >= 0 && CarType <= maxValue) {
            correctInput = 1;
        } else {
            printf("Please enter a valid index from the choices for the Car Model you want to purchase.\n\n");
        }
        while (getchar() != '\n');
    } while (correctInput == 0);


    typeOfCarPerSale[numberOfSales] = CarType;
    printf("\nThe selected car is: %s\n", carModel[CarType].CarModel);

    totalPrice = carPrices[CarType].CarPrices * CarsNeeded;

    // fix the allocated parts
    if (CarType >= 0 && CarType < CarsAvailable) {
        if (carStocks[CarType].CarsInStock >= CarsNeeded) {
            carStocks[CarType].CarsInStock -= CarsNeeded;
            printf("\nCars successfully allocated. Remaining stock for CarType %d: %d\n", CarType,
                   carStocks[CarType].CarsInStock);

            // Update the stock in the file
            FILE *file = fopen("Car_stock.txt", "w");
            if (file == NULL) {
                printf("Error opening file for writing.\n");
                exit(EXIT_FAILURE);
            }
            // Write the header without a newline character
            fprintf(file, "Model,Price,Year,Stock");
            for (int i = 0; i < CarsAvailable; i++) {
                fprintf(file, "\n%s,%.2f,%d,%d", carStocks[i].CarModel, carPrices[i].CarPrices,
                        carStocks[i].CarManufacture_Year, carStocks[i].CarsInStock);
            }
            fprintf(file, "\n");
            fclose(file);
        } else {
            printf("Not enough cars in stock for CarType %d.\n", CarType);
            // If not enough stock, set CarsNeeded to the available stock
            CarsNeeded = carStocks[CarType].CarsInStock;
        }
    }

    CarAmountPerSale[numberOfSales] = CarsNeeded;
    DisplayCarTypes(carModel, carPrices);

    int max_age = 99, min_age = 18;
    correctInput = 0;
    do {
        user_Age = getUnsignedShortFromConsole("\nWhat is your Age? :  ");
        if (user_Age < 18) {
            printf("\nPlease enter a valid age to purchase a car");
        } else {
            if (user_Age >= min_age && user_Age <= max_age) {
                correctInput = 1;
            } else {
                printf("\nPlease enter age correctly between %d and %d as a whole number.\n\n", min_age, max_age);
            }
        }
        while (getchar() != '\n');
    } while (correctInput == 0);

    discount = FALSE;
    if (user_Age >= DISCOUNT_MIN_Age && user_Age <= DISCOUNT_MAX_Age) {
        discount = TRUE;
    } else {
        correctInput = 0;
        do {
            char bought_car_before = getCharFromConsole(
                    "\nHave you bought cars from this dealership before? 'y' or 'n':\n");
            if (bought_car_before == 'y' || bought_car_before == 'Y') {
                discount = TRUE;
                break;
            } else if (bought_car_before == 'n' || bought_car_before == 'N') {
                break;
            } else {
                printf("Please only enter 'y' OR 'n'\n");
            }
            while (getchar() != '\n');
        } while (correctInput == 0);
    }

    discountGivenPerSale[numberOfSales] = discount;
    switch (discount) {
        case TRUE:
            totalPrice *= (1 - DISCOUNT_PERCENTAGE);
            printf("\n\nYou get a discount of 15 percent!\n");
            break;
        case FALSE:
            printf("\n\nNo discount is given.\n");
            break;
    }
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    // got the time function from" https://stackoverflow.com/questions/1442116/how-to-get-the-date-and-time-values-in-a-c-program"
    printf("The time of purchase is: %02d:%02d:%02d  %d-%02d-%02d\n", tm.tm_hour, tm.tm_min, tm.tm_sec, tm.tm_mday,
           tm.tm_mon + 1, tm.tm_year + 1900);
    printf("You have bought %hd %s.\n", CarsNeeded, carModel[CarType].CarModel);
    printf("Your total cost is %.2f GBP.\n ", totalPrice);

    numberOfSales++;

    correctInput = 0;
    do {
        char leave_Feedback = getCharFromConsole(
                "\n\nWould you like to leave feed back on the car model you have purchased? 'y' or 'n' Answer: ");
        if (leave_Feedback == 'y' || leave_Feedback == 'Y') {
            int min_rate = 0, max_rate = 5;
            correctInput = 0;
            do {
                printf("Enter your feedback for the purchase (rating out of 5): ");
                scanf("%d", &ratings[*numSales]);
                if (ratings[*numSales] == 0) {
                    printf("\nPlease enter a valid rating for the purchase.\n");
                } else {
                    if (ratings[*numSales] >= min_rate && ratings[*numSales] <= max_rate) {
                        correctInput = 1;
                    } else {
                        printf("\nPlease enter an ratings between %d and %d.\n\n", min_rate, max_rate);
                    }
                }
                while (getchar() != '\n');
            } while (correctInput == 0);

            printf("Enter a short comment (up to %d characters): ", MAX_FEEDBACK_SIZE - 1);
            scanf(" %[^\n]", feedback[*numSales]);
            printf("Thank you for your feedback!\n");
        } else if (leave_Feedback == 'n' || leave_Feedback == 'N') {
            break;
        } else {
            printf("Please only enter 'y' OR 'n'\n");
        }
        while (getchar() != '\n');
    } while (correctInput == 0);

    if (*numSales >= 0 && *numSales < MAX_SALES) {
        strcpy(sales[*numSales].modelName, carModel[CarType].CarModel);
        sales[*numSales].numberOfCars = CarsNeeded;
        sales[*numSales].totalPrice = totalPrice;
        (*numSales)++;
    } else {
        printf("Error: Invalid sale index.\n");
    }
}

void saveSalesData(const char *filename, struct Sale sales[], int numSales) {
    FILE *file = fopen(filename, "a");
    if (file != NULL) {
        // Check if there is an empty line, and add below if the line is full
        fseek(file, 0, SEEK_END);
        long size = ftell(file);
        if (size > 0) {
            fprintf(file, "\n"); // Add a new line if the file is not empty
        }
        // Write the sales data to the file
        for (int i = 0; i < numSales; i++) {
            fprintf(file, "%s,%d,%.2f\n", sales[i].modelName, sales[i].numberOfCars, sales[i].totalPrice);
        }

        fclose(file);
    } else {
        printf("Error opening file for writing.\n");
    }
}

void loadSalesData(const char *filename, struct Sale sales[], int *numSales) {
    FILE *file = fopen(filename, "r");

    if (file != NULL) {
        // Skip the header line
        fscanf(file, "%*[^\n]\n");
        *numSales = 0;
        // Read sales data until the end of the file or MAX_SALES
        while (*numSales < MAX_SALES &&
               fscanf(file, "%49[^,],%d,%f\n",
                      sales[*numSales].modelName,
                      &sales[*numSales].numberOfCars,
                      &sales[*numSales].totalPrice) == 3) {
            (*numSales)++;
        }
        fclose(file);
    } else {
        printf("File not found. Starting with an empty sales record.\n");
        *numSales = 0;
    }
}

void View_Sales_Data(struct Sale *sales, int numSales, struct Cars *CarPrices, struct Cars *CarModel) {
    printf("All Sales Data:\n\n");

    float totalSalesValue = 0;
    unsigned int CarsSold = 0;

    loadSalesData("sales_data.csv", sales, &numSales);
// Print each sale entry
    printf("\nSales Data:\n");
    printf("%-10s%-20s%-20s%-15s%-15s%-15s%-15s\n", "Sale Index", "Customer Name", "Type of Car", "Units Sold",
           "Car Price", "Total Amount", "Discount Given");

// Print each sale entry
    for (int i = 0; i < numberOfSales; i++) {
        int typeOfCars = typeOfCarPerSale[i];
        float TotalPrice = CarAmountPerSale[i] * CarPrices[typeOfCars].CarPrices;
        char discountGivenText[4];
        if (discountGivenPerSale[i] == TRUE) {
            strcpy(discountGivenText, "Yes");
            TotalPrice *= (1 - DISCOUNT_PERCENTAGE);
        } else {
            strcpy(discountGivenText, "No");
        }

        printf("%-10d%-20s%-20s%-15hd%-15.2f%-15.2f%-15s\n",
               i, customerNames[i], CarModel[typeOfCars].CarModel, CarAmountPerSale[i],
               CarPrices[typeOfCars].CarPrices, TotalPrice, discountGivenText);


        // Fix totalSalesValue calculation
        totalSalesValue += TotalPrice;
        CarsSold += CarAmountPerSale[i];
    }
    for (int i = 0; i < numberOfSales; i++) {
        strcpy(sales[i].modelName, CarModel[typeOfCarPerSale[i]].CarModel);
        sales[i].numberOfCars = CarAmountPerSale[i];
        sales[i].totalPrice = CarAmountPerSale[i] * CarPrices[typeOfCarPerSale[i]].CarPrices;
    }

// Sorting sales by total price in descending order
    for (int i = 0; i < numberOfSales - 1; i++) {
        for (int j = 0; j < numberOfSales - i - 1; j++) {
            if (sales[j].totalPrice < sales[j + 1].totalPrice) {
                // Swap sales data
                struct Sale temp = sales[j];
                sales[j] = sales[j + 1];
                sales[j + 1] = temp;

                // Swap associated data in typeOfCarPerSale
                int tempCarType = typeOfCarPerSale[j];
                typeOfCarPerSale[j] = typeOfCarPerSale[j + 1];
                typeOfCarPerSale[j + 1] = tempCarType;
            }
        }
    }

// Print the total sales by model
    printf("\nTotal Sales by Model:\n");
    printf("%-20s%-15s%-15s%-10s\n", "Model", "Total Units", "Total Price", "Discounted Total");

    for (int i = 0; i < numberOfSales; i++) {
        int typeOfCars = typeOfCarPerSale[i];
        float TotalPrice = CarAmountPerSale[i] * CarPrices[typeOfCars].CarPrices;
        char discountGivenText[4];

        if (discountGivenPerSale[i] == TRUE) {
            strcpy(discountGivenText, "Yes");
            TotalPrice *= (1 - DISCOUNT_PERCENTAGE);
        } else {
            strcpy(discountGivenText, "No");
        }

        printf("%-20s%-15d%-15.2f%-10s\n", CarModel[typeOfCars].CarModel, CarAmountPerSale[i], TotalPrice,
               discountGivenText);
    }

// Save sales data to file
    saveSalesData("sales_data.csv", sales, numberOfSales);

}

void View_Customer_Feedback(struct Sale *sales, int numSales, const char *feedbackFileName) {
    // Sort the feedback based on ratings in descending order
    for (int i = 0; i < numSales - 1; i++) {
        for (int j = 0; j < numSales - i - 1; j++) {
            if (ratings[j] < ratings[j + 1]) {
                // Swap sales data
                struct Sale tempSale = sales[j];
                sales[j] = sales[j + 1];
                sales[j + 1] = tempSale;

                // Swap associated data in ratings array
                int tempRating = ratings[j];
                ratings[j] = ratings[j + 1];
                ratings[j + 1] = tempRating;

                // Swap associated data in feedback array
                char tempFeedback[100]; // Update the size based on your actual requirement
                strcpy(tempFeedback, feedback[j]);
                strcpy(feedback[j], feedback[j + 1]);
                strcpy(feedback[j + 1], tempFeedback);
            }
        }
    }

    // Print sorted feedback
    printf("Customer Feedback:\n");

    FILE *feedbackFile = fopen(feedbackFileName, "a");
    if (feedbackFile == NULL) {
        printf("Error opening feedback file for writing.\n");
        return;
    }

    for (int i = 0; i < numSales; i++) {
        printf("Sale Index: %d | Car Model: %s | Rating: %d | Comment: %s\n",
               i, sales[i].modelName, ratings[i], feedback[i]);
        // Write feedback data to the CSV file
        fprintf(feedbackFile, "%d,%s,%d,%s\n", i, sales[i].modelName, ratings[i], feedback[i]);
    }

    fclose(feedbackFile);
}

void Exit() {
    printf("\n\nThank you for using this Dealership Program. Bye!!!!!!");
}

int main() {
    struct Sale sales[MAX_SALES];

    int numSales = 0;
    char userChoice;
    do {
        clearScreen();
        Greeting_Customer();
        Menu();
        correctInput = FALSE;
        do {
            userChoice = getCharFromConsole("Please choose one: ");
            userChoice = toupper(userChoice);
            if (userChoice == MENU_OPTION_VIEW_CAR_STOCK || userChoice == MENU_OPTION_BUY_CARS
                || userChoice == MENU_OPTION_VIEW_SALES_DATA || userChoice == MENU_OPTION_VIEW_CUSTOMER_FEEDBACK
                || userChoice == MENU_OPTION_EXIT) {
                correctInput = TRUE;

            } else {
                printf("\nSorry this is an incorrect choice. Please try again \n");
            }
            while (getchar() != '\n');
        } while (correctInput != TRUE);

        clearScreen();
        switch (userChoice) {
            case MENU_OPTION_VIEW_CAR_STOCK:
                View_Car_Stock();
                break;
            case MENU_OPTION_BUY_CARS:
                Buy_Cars(sales, &numSales, CarStocks, CarStocks, CarStocks);
                break;
            case MENU_OPTION_VIEW_SALES_DATA:
                View_Sales_Data(sales, numSales, CarStocks, CarStocks);
                break;
            case MENU_OPTION_VIEW_CUSTOMER_FEEDBACK:
                View_Customer_Feedback(sales, numSales, "customer_feedback.csv");
                break;
            case MENU_OPTION_EXIT:
                Exit();
                break;
        }
        pauseProgram(userChoice);

    } while (userChoice != MENU_OPTION_EXIT);
    clearScreen();
    printf("\n\nHave a good day!!!\n\n");
    return 0;
}