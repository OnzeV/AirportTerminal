#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <conio.h> // getch_
#include <limits> //cinignore
#include <cstdlib>
#include <ctime>
#include <set>
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
using namespace std;

string selectedClass = "Economy";
double classPriceMultiplier = 1.0;
int globalSeatsBooked = 0;

struct FlightReceipt // FOR receipt
{
    string flightID;
    int    seats;
};

struct LoginResult // FOR verifybookedID
{
    string name;
    string bookedClass;
    bool approved;
};

struct verifiedid //FOR verifyID
{
    string name;
    bool approved;
    string expiry;
};


void display();

// ────────────────────────────────────────────────
//              Check in FUNCTIONS
// ────────────────────────────────────────────────
LoginResult verifybookedID();
verifiedid verifyID();
void checkLuggage(int numPassengers);
void printBoardingPass(string flightID, string passengerName, string travelClass);

// ────────────────────────────────────────────────
//             SHOW / SEARCH FLIGHTS
// ────────────────────────────────────────────────
void showAllFlights();
void searchFlights();

void playMusic();
// ────────────────────────────────────────────────
//              RECEIPT FUNCTIONS
// ────────────────────────────────────────────────
FlightReceipt getFlightReceipt();

// ────────────────────────────────────────────────
//             STAFF MODE FUNCTIONS
// ────────────────────────────────────────────────
void enterf();
void viewf();
bool pass();
int select();


double getMultiplierForClass(string cls)
{
    if (cls == "Business")        return 5.0;
    if (cls == "First-Class")     return 7.0;
    return 1.0;
}

int main() {
    playMusic();

    srand(static_cast<unsigned>(time(nullptr))); // claudeai makes it so the printboarding pass will not have the same seat number

    char choice;
    bool running = true;
    bool change = true;
    string b_name, b_passport, b_country, b_dob, b_expiry, b_flightID, b_class;
    string f_name, f_passport, f_country, f_dob, f_expiry;
    string userFlightID = "";
    string userClass = "";
    int userSeats = 0, b_seats;
    bool access = true;

    while (running) {
        system("cls");
        display();
        choice = _getch();

        // ────────────────────────────────────────────────
        //                 Book a Flight
        // ────────────────────────────────────────────────

        if (choice == '1')
        {

            FlightReceipt receipt = getFlightReceipt();

            if (!receipt.flightID.empty())
            {
                int pax = receipt.seats;

                char wantCheckIn;

                cout << "\nWould you like to check in? (Y/N): ";
                cin >> wantCheckIn;

                cin.ignore(1000, '\n');

                if (toupper(wantCheckIn) == 'Y')
                {
                    verifiedid id = verifyID();

                    if (id.approved)
                    {

                        checkLuggage(pax);


                        for (int i = 0; i < pax; ++i)
                        {
                            printBoardingPass(receipt.flightID, id.name, selectedClass);
                            cout << "\nPress any key for next boarding pass...\n";
                            _getch();
                        }
                    }

                    for (int i = 0; i < (id.name).length(); i++)
                    {
                        if ((id.name)[i] == ' ') (id.name)[i] = '-';
                    }
                    ifstream inFile;
                    inFile.open("id.txt");
                    if (!inFile)
                    {
                        cout << "Error. Cannot open id.txt";
                    }
                    else
                    {
                        while (!inFile.eof())
                        {
                            inFile >> f_name >> f_passport >> f_country >> f_dob >> f_expiry;
                            if (inFile.fail()) break;

                            if (id.expiry == f_expiry)
                            {
                                b_passport = f_passport;
                                b_country = f_country;
                                b_dob = f_dob;
                                inFile.close();
                            }

                        }
                    }

                    ofstream outFile;
                    outFile.open("booked.txt", ofstream::app);
                    if (!outFile)
                        cout << "ERROR!\n";
                    else {
                        outFile << id.name << "\t" << b_passport << "\t" << b_country << "\t" << b_dob <<  "\t"  << id.expiry << "\t"
                            << receipt.flightID << "\t" << selectedClass << "\t" << pax << "\n";
                        outFile.close();

                    }
                   
                }
                else
                {
                    cout << "\n" << pax << " seat(s) reserved. Please check in later.\n";
                }
            }

        }

        // ────────────────────────────────────────────────
        //                Check-in
        // ────────────────────────────────────────────────
        else if (choice == '2')
        {
            LoginResult id = verifybookedID();

            if (id.approved)
            {
                cout << "\nRetrieving your booking information...\n";

                ifstream bookFile;
                bookFile.open("booked.txt");

                if (!bookFile) {
                    cout << "ERROR: Cannot open booked.txt\n";

                }
                else
                {
                    while (!bookFile.eof())
                    {
                        bookFile >> b_name >> b_passport >> b_country >> b_dob >> b_expiry >> b_flightID >> b_class >> b_seats;
                        if (bookFile.fail()) break;

                        for (int i = 0; i < b_name.length(); i++)
                        {
                            if (b_name[i] == '-') b_name[i] = ' ';
                            b_name[i] = toupper(b_name[i]);
                        }

                        if (b_name == id.name)
                        {
                            userFlightID = b_flightID; // Retrieve flight ID & class from res
                            userClass = b_class;
                            userSeats = b_seats;
                            break;
                        }
                    }
                }

                bookFile.close();

                if (userFlightID.empty())
                {
                    cout << "\nNo booking found for " << id.name << "\n";
                    cout << "Please book a flight first using option [1].\n";
                }
                else
                {
                    selectedClass = userClass;
                    classPriceMultiplier = getMultiplierForClass(userClass);

                    ifstream flightFile;
                    flightFile.open("flights.txt");

                    if (!flightFile)
                    {
                        cout << "ERROR: Cannot open flights.txt\n";
                        return 0;
                    }
                    else
                    {
                        string flightid, dest, date, remarks;
                        int    seats;
                        double price;
                        bool   flightFound = false;

                        while (!flightFile.eof())
                        {
                            flightFile >> flightid >> dest >> date >> seats >> price >> remarks;
                            if (flightFile.fail()) break;

                            if (flightid == userFlightID) {
                                flightFound = true;

                                double finalPrice = price * classPriceMultiplier * userSeats;

                                cout << "\n" << string(60, '=') << endl;
                                cout << "BOOKING FOUND\n" << string(60, '=') << endl;
                                cout << "Passenger    : " << id.name << endl;
                                cout << "Flight ID    : " << flightid << endl;
                                cout << "Destination  : " << dest << endl;
                                cout << "Date         : " << date << endl;
                                cout << "Class        : " << selectedClass << endl;
                                cout << "Total Price  : $" << fixed << setprecision(2) << finalPrice << endl;
                                cout << "Booked Seats: " << userSeats << " seats" << endl;
                                cout << string(60, '=') << endl;

                                checkLuggage(userSeats);

                                for (int i = 0; i < userSeats; ++i)
                                {
                                    printBoardingPass(flightid, id.name, selectedClass);

                                    if (i < userSeats - 1)
                                    {
                                        cout << "\nPress any key for next Boarding Pass.\n";
                                        _getch();
                                    }
                                }

                                break;
                            }
                        }
                        if (!flightFound)
                            cout << "\n Flight " << userFlightID << " not found in system.\n";
                    }


                    flightFile.close();


                }
            }
        }
        // ────────────────────────────────────────────────
        //             SHOW / SEARCH FLIGHTS
        // ────────────────────────────────────────────────
        else if (choice == '3')
        {
            showAllFlights();
            char wantSearch;
            cout << "\nSearch for a flight? (Y/N): ";
            cin >> wantSearch;

            if (toupper(wantSearch) == 'Y')
                searchFlights();
        }

        // ────────────────────────────────────────────────
        //                      EXIT
        // ────────────────────────────────────────────────
        else if (choice == '4') {
            running = false;
        }
        // ────────────────────────────────────────────────
        //                   STAFF MODE
        // ────────────────────────────────────────────────
        else if (choice == 27)
        {
            cout << "\n";
            access = pass();
            if (access == false)
            {
                break;
            }
            while (change)
            {
                choice = select();
                if (choice <= 3 && choice >= 1)
                {
                    if (choice == 1)
                    {
                        enterf();
                        change = false;
                    }
                    else if (choice == 2)
                    {
                        viewf();
                        change = false;
                    }
                    else if (choice == 3) searchFlights();

                }
                else
                {
                    cout << "Invalid number!" << endl << "Enter again : ";
                    change = true;
                }
            }
            change = true;
        }

        if (running) {
            cout << "\nPress any key to return to main menu...";
            _getch();
        }
    }

    cout << "\nThank you for using Singapore Airlines system.\n";
    return 0;
}

void playMusic()
{
    PlaySound(TEXT("charliekirk.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
}

void display() {
    cout << string(78, '=') << endl;
    cout << "||                                                                        A1||\n";
    cout << "||                         W E L C O M E  T O                               ||\n";
    cout << "||                                                                          ||\n";
    cout << "||                \033[31mS I N G A P O R E\033[0m  \033[34mA I R L I N E S\033[0m                        ||\n";
    cout << "||                                                                          ||\n";
    cout << "||                                                                          ||\n";
    cout << "||                          [1] Book A Flight                               ||\n";
    cout << "||                          [2] Check In                                    ||\n";
    cout << "||                          [3] Flight Schedule                             ||\n";
    cout << "||                          [4] Exit                                        ||\n";
    cout << "||                                                                          ||\n";
    cout << "||                                                                          ||\n";
    cout << string(78, '=') << endl;
}

// ────────────────────────────────────────────────
//             Verify ID FUNCTIONS
// ────────────────────────────────────────────────

LoginResult verifybookedID()
{
    string name, passport, country, dob, expiry;
    string f_name, f_pass, f_country, f_dob, f_expiry, f_flight, f_class;
    int f_seats;

    cout << "\nID Verification\n" << string(60, '=') << endl;

    cout << "Full Name          : "; getline(cin, name);
    cout << "Passport No.       : "; getline(cin, passport);
    cout << "Country            : "; getline(cin, country);
    cout << "Date of Birth (dd/mm/yyyy): "; getline(cin, dob);
    cout << "Expiry Date (dd/mm/yyyy)  : "; getline(cin, expiry);


    for (int i = 0; i < name.length(); i++)
    {
        name[i] = toupper(name[i]);
    }

    passport[0] = toupper(passport[0]);
    passport[passport.length() - 1] = toupper(passport[passport.length() - 1]);

    ifstream file;
    file.open("booked.txt");
    if (!file)
    {
        cout << "ERROR: Cannot open booked.txt\n";
        return { "", "", false};
    }
    else
    {
        while (!file.eof())
        {
            file >> f_name >> f_pass >> f_country >> f_dob >> f_expiry >> f_flight >> f_class >> f_seats;
            if (file.fail()) break;

            for (int i = 0; i < f_name.length(); i++)
            {
                if (f_name[i] == '-') f_name[i] = ' ';
                f_name[i] = toupper(f_name[i]);
            }

            if (f_name == name && f_pass == passport &&
                f_country == country && f_dob == dob && f_expiry == expiry)
            {
                file.close();

                cout << string(60, '=') << "\nCHECK-IN APPROVED\n" << string(60, '=') << endl;

                return { name, f_class, true};
            }
        }

    }

    file.close();

    cout << string(60, '=') << "\nCHECK-IN DENIED\n" << string(60, '=') << endl;

    return { "", "", false };
}

verifiedid verifyID()
{
    string name, passport, country, dob, expiry;
    string f_name, f_pass, f_country, f_dob, f_expiry;
    int dash;

    cout << "\nID Verification\n" << string(60, '=') << endl;
    cout << "Full Name          : "; getline(cin, name);
    cout << "Passport No.       : "; getline(cin, passport);
    cout << "Country            : "; getline(cin, country);
    cout << "Date of Birth (dd/mm/yyyy): "; getline(cin, dob);
    cout << "Expiry Date (dd/mm/yyyy)  : "; getline(cin, expiry);

    for (int i = 0; i < name.length(); i++)
    {
        name[i] = toupper(name[i]);
    }

    passport[0] = toupper(passport[0]);
    passport[passport.length() - 1] = toupper(passport[passport.length() - 1]);

    ifstream file;
    file.open("id.txt");

    if (!file)
    {
        cout << "ERROR: Cannot open booked.txt\n";
        return { "", false, ""};
    }
    else
    {
        while (!file.eof())
        {
            file >> f_name >> f_pass >> f_country >> f_dob >> f_expiry;
            if (file.fail()) break;
            for (int i = 0; i < f_name.length(); i++)
            {
                if (f_name[i] == '-') f_name[i] = ' ';
                f_name[i] = toupper(f_name[i]);
            }


            if (f_name == name && f_pass == passport &&
                f_country == country && f_dob == dob && f_expiry == expiry)
            {
                file.close();

                cout << string(60, '=') << "\nCHECK-IN APPROVED\n" << string(60, '=') << endl;

                return { name,  true, expiry};
            }
        }

    }

    file.close();

    cout << string(60, '=') << "\nCHECK-IN DENIED\n" << string(60, '=') << endl;

    return { "", false };
}

// ────────────────────────────────────────────────
//             Check Luggage FUNCTION
// ────────────────────────────────────────────────

void checkLuggage(int num)
{

    double* weights = new double[num](); // chatgpt
    double total = 0;
    double w;

    cout << "\nLuggage Check-In\n" << string(60, '=') << "\n\n";

    for (int i = 0; i < num; ++i) {
        cout << "Weight of luggage " << (i + 1) << " (kg): ";

        while (!(cin >> w) || w <= 0) {
            cout << "Enter positive number: ";
            cin.clear();
            cin.ignore(1000, '\n');
        }
        while (w > 32.0) {
            cout << "Exceeded 32 kg limit!\n";
            cout << "New weight for luggage " << (i + 1) << ": ";
            cin >> w;
        }
        weights[i] = w;
        total += w;
    }

    cout << string(60, '=') << endl;

    for (int i = 0; i < num; ++i)
    {
        cout << "Luggage " << (i + 1) << " : " << weights[i] << " kg\n";
    }

    cout << string(60, '=') << "\nTotal weight = " << total << " kg\n";

    delete[] weights; // chatgpt
}

void printBoardingPass(string flightID, string passengerName, string travelClass)
{
    string fid, dest, date, rem;
    int    seat;
    double price;
    string destination = "UNKNOWN";
    string flightDate = "N/A";

    ifstream f;
    f.open("flights.txt");

    if (!f)
    {
        cout << "Cannot open flights.txt\n"; return;
    }
    else
    {
        while (f >> fid >> dest >> date >> seat >> price >> rem)
        {
            if (fid == flightID)
            {
                destination = dest; flightDate = date; break;
            }
        }
    }

    f.close();

    string destCode = destination.substr(0, 3);
    int    gatenum = rand() % 9 + 1;
    int    eNumber = rand() % 90000000 + 10000000;

    int    seatnum;
    char   seatLetter;
    string seatNumber;

    seatnum = rand() % 60 + 1;
    seatLetter = 'A' + (rand() % 6);
    seatNumber = to_string(seatnum) + seatLetter;



    cout << endl;
    cout << left;
    cout << 'o'; for (int i = 0; i < 65; i++) cout << "="; cout << 'o';
    cout << "\n        S I N G A P O R E       " << travelClass << "        |" << endl;
    cout << "           A I R L I N E S       " << "  Class" << "       | From Singapore" << endl;
    cout << "      |"; for (int i = 0; i < 40; i++) cout << "-"; cout << "| To " << destination;
    cout << setw(10) << "\n      | " << "Flight " << flightID << "   From SIN   TO " << destCode << "      |" << endl;
    cout << "      |"; for (int i = 0; i < 40; i++) cout << "-"; cout << "| Seat ";
    if (seatnum < 10) cout << "0"; cout << seatnum << seatLetter;
    cout << setw(10) << "\n      |" << "Terminal A1  " << "  Gate A0" << gatenum << "   Seat ";
    if (seatnum < 10) cout << "0"; cout << seatnum << seatLetter << "    |" << endl;
    cout << "      |"; for (int i = 0; i < 40; i++) cout << "-"; cout << "| Flight " << flightID;
    cout << setw(25) << "\nGATE CLOSES 10 MINUTES BEFORE DEPARTURE" << "        | Date: " << flightDate << endl;
    cout << "ENo. " << eNumber;
    for (int i = 0; i < 16; i++) { int t = rand() % 9 + 1; cout << t; }
    cout << "                  |" << endl;
    cout << 'o'; for (int i = 0; i < 65; i++) cout << "="; cout << 'o' << endl;
}

// ────────────────────────────────────────────────
//             SHOW / SEARCH FLIGHTS
// ────────────────────────────────────────────────

void showAllFlights() {
    ifstream file("flights.txt");
    if (!file) { cout << "Cannot open flights.txt\n"; return; }

    cout << "\nFLIGHT INFORMATION\n" << string(70, '=') << endl;
    cout << left
        << setw(12) << "Flight ID"
        << setw(18) << "Destination"
        << setw(14) << "Date"
        << setw(8) << "Seats"
        << setw(10) << "Price"
        << "Remarks\n" << string(70, '-') << endl;

    string id, dest, date, remarks;
    int    seats;
    double price;

    while (file >> id >> dest >> date >> seats >> price >> remarks) {
        cout << left
            << setw(12) << id
            << setw(18) << dest
            << setw(14) << date
            << setw(8) << seats
            << fixed << setprecision(2) << setw(10) << price
            << remarks << endl;
    }
    cout << string(70, '=') << endl;
    file.close();
}

void searchFlights() {
    ifstream inFile;
    string sflightnum, slocation, sdate;
    string flightnum, location, date, remarks;
    char   avail;
    int    seatsremaining;
    double price;
    bool   found = false;

    inFile.open("flights.txt");
    if (!inFile) {
        cout << "!Fatal Error Flight Information Not Found!\n";
        return;
    }

    cout << left << setw(30) << " " << "Search Bar\n";
    cout << "Date (enter '-' to skip): ";    cin >> sdate;
    cout << "Location (enter '-' to skip): "; cin >> slocation;
    cout << "Flight ID (enter '-' to skip): "; cin >> sflightnum;
    cout << "Show only available flights? (Y/N): "; cin >> avail;

    bool filterAvailable = (avail == 'Y' || avail == 'y');

    cout << left
        << setw(10) << "FlightID"
        << setw(15) << "Location"
        << setw(12) << "Date"
        << setw(6) << "Seats"
        << setw(8) << "Price"
        << setw(10) << "Remarks" << endl;

    while (inFile >> flightnum >> location >> date >> seatsremaining >> price >> remarks) {
        if ((sdate == "-" || sdate == date) &&
            (slocation == "-" || slocation == location) &&
            (sflightnum == "-" || sflightnum == flightnum))
        {
            if (filterAvailable && remarks == "FULL") continue;
            found = true;
            cout << setw(10) << flightnum
                << setw(15) << location
                << setw(12) << date
                << setw(6) << seatsremaining
                << setw(8) << fixed << setprecision(2) << price
                << setw(10) << remarks << endl;
        }
    }

    if (!found) cout << "Flight not found!\n";
    inFile.close();
}

// ────────────────────────────────────────────────
//                 FliGHT RECEIPT
// ────────────────────────────────────────────────

FlightReceipt getFlightReceipt() {
    const int SIZE = 30;
    ifstream  inFile;

    string flightID[SIZE], Destination[SIZE], date[SIZE], remarks[SIZE];
    double price[SIZE];
    string username, flightcode, memberornot, nothing, name;
    bool   found = false;
    bool   ismember = false;
    double totalprice;
    int    remaindingseats[SIZE], count = 0, i = 0;


    int seats = -1;


    cin.clear();
    if (cin.rdbuf()->in_avail() > 0) cin.ignore(1000, '\n'); // claudeai

    cout << string(60, '=') << endl;
    cout << "FLIGHT BOOKING SYSTEM" << endl;
    cout << string(60, '=') << endl;
    cout << "Please enter username (space sensitive): ";
    cin >> username;

    // Check membership
    inFile.open("members.txt");
    if (!inFile) {
        cout << "Error: Could not find members.txt file!" << endl;
    }
    else
    {
        while (inFile >> name >> nothing >> nothing >> nothing >> nothing >> memberornot) {
            if (username == name) {
                ismember = (memberornot == "Y");
                break;
            }
        }
        inFile.close();
    }

    // Load flights
    inFile.open("flights.txt");
    if (!inFile) {
        cout << "Error cannot open text file" << endl;
        return { "", 0 };
    }
    while (count < SIZE &&
        (inFile >> flightID[count] >> Destination[count] >> date[count]
            >> remaindingseats[count] >> price[count] >> remarks[count]))
    {
        count++;
    }
    inFile.close();

    //Flight selection loop
    while (!found)
    {
        cout << string(60, '=') << endl;
        cout << "Please enter flight ID (ALL CAPS): ";
        cin >> flightcode;

        for (i = 0; i < count; i++) {
            if (flightcode == flightID[i]) {
                found = true;
                cout << "Flight found!\n";
                cout << "DESTINATION\tPRICE ($)\tSEATS LEFT\n";
                cout << "------------------------------------------\n";
                cout << Destination[i] << "\t\t"
                    << fixed << setprecision(2) << price[i] << "\t\t"
                    << remaindingseats[i] << "\n";
                cout << "------------------------------------------\n";

                if (remaindingseats[i] == 0) {
                    cout << "\nError: No more seats left for this flight.\n";
                    cout << "Returning to main menu...\n";
                    return { "", 0 };
                }

                // Prompt for seat count
                seats = 0;
                while (seats < 1 || seats > remaindingseats[i]) {
                    cout << "How many seats? (1-" << remaindingseats[i] << "): ";
                    if (!(cin >> seats) || cin.fail()) {
                        cout << "Invalid input. Please enter a number.\n";
                        cin.clear();
                        cin.ignore(1000, '\n');
                        seats = 0;
                        continue;
                    }
                    if (seats < 1)
                        cout << "Invalid number, please try again.\n";
                    else if (seats > remaindingseats[i])
                        cout << "Sorry, only " << remaindingseats[i] << " seats left.\n";
                }


                break;
            }
        }

        if (!found)
            cout << "Flight not found. Please try again.\n";
    }

    //Class selection
    char   classChoice;
    double multiplier;
    string classDescription;

    cout << "\n" << string(60, '=') << "\n";
    cout << "SELECT YOUR CLASS\n" << string(60, '=') << "\n";
    cout << "[1] Economy      - $" << fixed << setprecision(2) << price[i] << "\n";
    cout << "[2] Business     - $" << fixed << setprecision(2) << (price[i] * 5.0) << "\n";
    cout << "[3] First Class  - $" << fixed << setprecision(2) << (price[i] * 7.0) << "\n";
    cout << string(60, '-') << "\n";
    cout << "Enter choice (1-3): ";
    cin >> classChoice;
    cin.ignore(1000, '\n');

    if (classChoice == '2')
    {
        multiplier = 2.5; classDescription = "BUSINESS CLASS"; selectedClass = "Business";
    }
    else if (classChoice == '3')
    {
        multiplier = 4.0; classDescription = "FIRST CLASS";    selectedClass = "First";
    }
    else
    {
        if (classChoice != '1') cout << "Invalid choice. Defaulting to Economy.\n";
        multiplier = 1.0;
        classDescription = "ECONOMY CLASS";
        selectedClass = "Economy";
    }
    classPriceMultiplier = multiplier;

    // Purchase confirmation
    string ans;
    while (true) {
        cout << "Would you like to purchase the ticket(s)? [Yes/No]: ";
        cin >> ans;

        if (ans == "Yes" || ans == "yes") {
            double priceperseat = (ismember ? 0.9 : 1.0) * price[i] * multiplier;
            totalprice = seats * priceperseat;
            string MEMBER = ismember ? "YES" : "NO";

            string remarkyesorno, remark;
            bool   gotRemark = false;
            while (!gotRemark) {
                cout << "Do you have any food allergies or anything we should know? [Yes/No]: ";
                cin >> remarkyesorno;
                if (remarkyesorno == "No" || remarkyesorno == "no") {
                    remark = "NIL";
                    gotRemark = true;
                }
                else if (remarkyesorno == "Yes" || remarkyesorno == "yes") {
                    cin.ignore(1000, '\n');
                    cout << "What should we look out for: ";
                    getline(cin, remark);
                    gotRemark = true;
                }
            }

            // Receipt printout
            cout << "\n========================================\n";
            cout << "            FLIGHT RECEIPT               \n";
            cout << "========================================\n";
            cout << left << setw(18) << "Username:" << username << "\n";
            cout << left << setw(18) << "Flight ID:" << flightID[i] << "\n";
            cout << left << setw(18) << "Destination:" << Destination[i] << "\n";
            cout << left << setw(18) << "Date:" << date[i] << "\n";
            cout << left << setw(18) << "Seats booked:" << seats << "\n";
            cout << left << setw(18) << "Member price:" << MEMBER << "\n";
            cout << left << setw(18) << "Remarks:" << remark << "\n";
            cout << left << setw(18) << "Flight Class:" << classDescription << "\n";
            cout << "----------------------------------------\n";
            cout << left << setw(18) << "Price per seat:" << "$"
                << fixed << setprecision(2) << priceperseat << "\n";
            cout << left << setw(18) << "Total price:" << "$"
                << fixed << setprecision(2) << totalprice << "\n";
            cout << "========================================\n";
            cout << "   Thank you for booking with us!\n";
            cout << "========================================\n\n";

            // Update flights.txt
            remaindingseats[i] -= seats;
            ofstream outFile("flights.txt");
            if (!outFile) {
                cout << "Error opening file for writing\n";
            }
            else {
                for (int j = 0; j < count; j++) {
                    outFile << flightID[j] << "\t"
                        << Destination[j] << "\t"
                        << date[j] << "\t"
                        << remaindingseats[j] << "\t"
                        << fixed << setprecision(2) << price[j] << "\t"
                        << remarks[j] << "\n";
                }
                outFile.close();
            }

            globalSeatsBooked = seats;
            return { flightID[i], seats };   // ← return flight ID and seat count
        }
        else if (ans == "No" || ans == "no") {
            return { "", 0 };                // user cancelled
        }
        else {
            cout << "Please enter Yes or No.\n";
        }
    }
}

// ────────────────────────────────────────────────
//               STAFF MODE
// ────────────────────────────────────────────────

bool pass()
{
    string pass = "1";//chg int to string
    string correct = "12345678";//chg int to string
    bool access = true;
    cout << "Enter staff password : ";
    cin >> pass;
    int i = 2;

    while (i > 0 && pass != correct)
    {
        while (pass != correct && i >= 1)
        {
            cout << "WRONG PASSWORD!!! " << i << " chance remaining" << endl << "Enter again : ";
            cin >> pass;
            i--;
            if (i == 0)
            {
                cout << "Access denied!!!!!";//add a return to main menu
                access = false;
            }
               

        }
    }
    return access;
}

int select()
{
    cout << "\n========================================\n";
    cout << "              Staff Mode                 \n";
    cout << "========================================\n";

    int choice = 9;
    cout << "1. Add flight" << endl
        << "2. View flight" << endl
        << "3. Search flight" << endl;
    cout << "Enter 1-3 : ";
    cin >> choice;
    system("cls");
    return choice;
}

void enterf()
{
    ofstream outFile;
    string id, location, date, remarks;
    bool choice = true;
    double price;
    char ch;
    int seat;

    while (choice)
    {
        cout << "Enter Flight\n";
        cout << "Please Enter flight details: " << endl;
        cout << left << setw(30) << "Flight ID";
        cin >> id;
        cout << setw(30) << "Location";
        cin >> location;
        cout << setw(30) << "Date (DD-MM-YYYY)";
        cin >> date;
        cout << setw(30) << "Available Seat";
        cin >> seat;
        cout << setw(30) << "Price";
        cin >> price;

        system("cls");

        if (seat > 0) remarks = "Available";
        else remarks = "Full";

        cout << "Creating New Flight :" << endl << "\n";
        cout << left << setw(20) << "Flight ID";
        cout << setw(20) << "Location";
        cout << setw(20) << "Date";
        cout << setw(20) << "Available Seat";
        cout << setw(20) << "Price";
        cout << setw(20) << "Remarks" << endl;

        cout << left << setw(20) << id;
        cout << setw(20) << location;
        cout << setw(20) << date;
        cout << setw(20) << seat;
        cout << fixed << setprecision(2) << setw(20) << price;
        cout << setw(20) << remarks << endl;

        cout << "\n[Enter] CONFIRM  |  [ESC] EDIT DETAILS\n";
        ch = _getch();



        if (ch == 13)
        {
            outFile.open("flights.txt", ofstream::app);
            if (!outFile)
                cout << "ERROR!" << endl;
            else
            {
                outFile << id << "\t" << location << "\t" << date << "\t" << seat << "\t" << price << "\t" << remarks << endl;
                outFile.close();
                cout << "\nFlight comfirmed and saved!\n" << endl;
                choice = false;
            }
        }
        else if (ch == 27)
        {
            cout << "\nRe-entering data...";
            choice = true;
        }


    }

}

void viewf()
{
    system("cls");
    string id, location, date, remarks;
    int seat;
    double price;
    ifstream inFile;
    inFile.open("flights.txt");

    if (!inFile)
    {
        cout << "Error: Could not open the file!" << endl;
    }
    else
    {
        cout << endl;
        cout << left << fixed << setprecision(2)
            << setw(20) << "ID"
            << setw(20) << "Location"
            << setw(20) << "Date"
            << setw(20) << "Seat"
            << setw(20) << "Price"
            << setw(20) << "Remarks" << endl << endl;
        while (!inFile.eof())
        {
            inFile >> id >> location >> date >> seat >> price >> remarks;
            if (inFile.fail())
                break;

            cout << left << fixed << setprecision(2)
                << setw(20) << id
                << setw(20) << location
                << setw(20) << date
                << setw(20) << seat
                << setw(20) << price
                << setw(20) << remarks << endl;
        }
    }
    inFile.close();
}