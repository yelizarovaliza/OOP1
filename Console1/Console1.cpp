#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <unordered_map>

using namespace std;

// FlightInfo structure for storing flight details
struct FlightInfo {
    string date;
    string flightNumber;
    int seatsPerRow;
    vector<pair<int, int>> rowPricePairs;
};

// Parser class for reading and parsing flight data from a file
class Parser {
public:
    vector<FlightInfo> parseFromFile(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error opening file: " << filename << endl;
            return {};
        }

        vector<FlightInfo> flights;
        int numberOfRecords;
        file >> numberOfRecords;
        file.ignore(); // Ignore newline character after numberOfRecords

        for (int i = 0; i < numberOfRecords; ++i) {
            FlightInfo flight;
            file >> flight.date >> flight.flightNumber >> flight.seatsPerRow;
            file.ignore(); // Ignore the remaining newline

            string rowPriceData;
            getline(file, rowPriceData);
            cout << "Parsing row price data: " << rowPriceData << endl; // Debug output
            parseRowPrices(rowPriceData, flight);
            flights.push_back(flight);
        }

        file.close();
        return flights;
    }

private:
    void parseRowPrices(const string& data, FlightInfo& flight) {
        stringstream ss(data);
        string rowRange;
        int price;
        char dollarSign;

        while (ss >> rowRange >> price >> dollarSign) {
            if (dollarSign != '$') {
                cerr << "Invalid price format!" << endl;
                continue;
            }

            size_t dashPos = rowRange.find('-');
            if (dashPos != string::npos) {
                int rowStart = stoi(rowRange.substr(0, dashPos));
                int rowEnd = stoi(rowRange.substr(dashPos + 1));

                for (int row = rowStart; row <= rowEnd; ++row) {
                    flight.rowPricePairs.push_back(make_pair(row, price));
                }
            }
            else {
                cerr << "Invalid row range format!" << endl;
            }
        }
    }


};

// Airplane class for handling seat availability, booking, and returning tickets
class Airplane {
private:
    FlightInfo flightInfo;
    vector<vector<string>> seatMap;  // Seat availability map, like "A1", "A2", etc.

public:
    Airplane(const FlightInfo& flight) : flightInfo(flight) {
        initializeSeatMap();
    }

    bool bookSeat(const string& seat, const string& passengerName) {
        // Seat booking logic
        return true; // Dummy return for now
    }

    void checkAvailability() const {
        // Display available seats
    }

    void returnTicket(const string& seat) {
        // Logic to return booked seats
    }

    void initializeSeatMap() {
        int numRows = 50; // Adjust as needed or derive from flight info
        seatMap.resize(numRows);

        for (int i = 0; i < numRows; ++i) {
            seatMap[i].resize(flightInfo.seatsPerRow);
            for (int j = 0; j < flightInfo.seatsPerRow; ++j) {
                seatMap[i][j] = " "; // Initialize all seats as available
            }
        }
    }


    void displayFlightDetails() const {
        // Logic to display flight details
    }

    const string& getFlightNumber() const {
        return flightInfo.flightNumber;
    }

    const string& getDate() const {
        return flightInfo.date;
    }
};

// Ticket class for storing booking information
class Ticket {
private:
    string passengerName;
    string flightNumber;
    string seatNumber;
    bool isBooked;

public:
    Ticket() : passengerName(""), flightNumber(""), seatNumber(""), isBooked(false) {}

    Ticket(const string& passenger, const string& flight, const string& seat)
        : passengerName(passenger), flightNumber(flight), seatNumber(seat), isBooked(true) {}

    void displayTicket() const {
        cout << "Passenger: " << passengerName << ", Flight: " << flightNumber
            << ", Seat: " << seatNumber << (isBooked ? " (Booked)" : " (Available)") << endl;
    }

    void cancelBooking() {
        isBooked = false;
    }

    const string& getPassengerName() const {
        return passengerName;
    }
};


// Class to handle user commands and validate input
class FlightCommandHandler {
private:
    vector<Airplane>& airplanes;
    unordered_map<string, Ticket> tickets;

public:
    FlightCommandHandler(vector<Airplane>& airplanes) : airplanes(airplanes) {}

    void handleCommands() {
        string command;
        while (true) {
            cout << "> ";
            cin >> command;

            if (command == "exit") {
                break;
            }
            else if (command == "check") {
                handleCheckCommand();
            }
            else if (command == "book") {
                handleBookCommand();
            }
            else if (command == "return") {
                handleReturnCommand();
            }
            else if (command == "view") {
                handleViewCommand();
            }
            else {
                cout << "Unknown command!" << endl;
            }
        }
    }

private:
    void handleCheckCommand() {
        string date, flightNo;
        cin >> date >> flightNo;

        cout << "Checking flight with date: " << date << " and flight number: " << flightNo << endl;

        Airplane* airplane = findFlight(date, flightNo);
        if (airplane) {
            airplane->checkAvailability();
        }
        else {
            cout << "Flight not found!" << endl;
        }
    }



    void handleBookCommand() {
        string date, flightNo, seat, passengerName;
        cin >> date >> flightNo >> seat >> passengerName;

        Airplane* airplane = findFlight(date, flightNo);
        if (airplane) {
            bool success = airplane->bookSeat(seat, passengerName);
            if (success) {
                Ticket ticket(passengerName, flightNo, seat);
                tickets[seat] = ticket;
                cout << "Booking confirmed for " << passengerName << endl;
            }
            else {
                cout << "Booking failed. Seat may be unavailable." << endl;
            }
        }
        else {
            cout << "Flight not found!" << endl;
        }
    }

    void handleReturnCommand() {
        string seat;
        cin >> seat;

        auto it = tickets.find(seat);
        if (it != tickets.end()) {
            it->second.cancelBooking();
            tickets.erase(it);
            cout << "Ticket returned successfully." << endl;
        }
        else {
            cout << "Ticket not found!" << endl;
        }
    }

    void handleViewCommand() {
        string param;
        cin >> param;

        if (isdigit(param[0])) {
            // Assuming param is ticket ID or seat number
            viewTicketBySeat(param);
        }
        else {
            viewTicketsByPassenger(param);
        }
    }

    void viewTicketBySeat(const string& seat) {
        auto it = tickets.find(seat);
        if (it != tickets.end()) {
            it->second.displayTicket();
        }
        else {
            cout << "Ticket not found!" << endl;
        }
    }

    void viewTicketsByPassenger(const string& passenger) {
        for (const auto& pair : tickets) {
            if (pair.second.getPassengerName() == passenger) {
                pair.second.displayTicket();
            }
        }
    }

    Airplane* findFlight(const string& date, const string& flightNo) {
        for (auto& airplane : airplanes) {
            if (airplane.getDate() == date && airplane.getFlightNumber() == flightNo) {
                return &airplane;
            }
        }
        return nullptr;
    }

};

// Main application controller class
class FlightBookingApp {
private:
    vector<Airplane> airplanes;
    Parser parser;
    FlightCommandHandler* commandHandler;

public:
    FlightBookingApp() : commandHandler(nullptr) {}

    ~FlightBookingApp() {
        delete commandHandler;
    }

    void run() {
        loadFlights();
        commandHandler = new FlightCommandHandler(airplanes);
        commandHandler->handleCommands();
    }

private:
    void loadFlights() {
        vector<FlightInfo> flights = parser.parseFromFile("C:/Users/User/Documents/GitHub/OOP1/Console1/config.txt");
        for (const auto& flight : flights) {
            airplanes.emplace_back(flight);
        }
    }
};

int main() {
    FlightBookingApp app;
    app.run();
    return 0;
}
