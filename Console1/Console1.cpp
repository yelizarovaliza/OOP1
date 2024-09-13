#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdio>

using namespace std;

// FlightInfo structure for storing flight details
struct FlightInfo {
    string date;
    string flightNumber;
    int seatsPerRow;
    vector<pair<int, int>> rowPricePairs;

    void display() const {
        cout << "Date: " << date << ", Flight: " << flightNumber
            << ", Seats per row: " << seatsPerRow << endl;
        for (const auto& rowPrice : rowPricePairs) {
            cout << "Rows: 1-" << rowPrice.first << ", Price: " << rowPrice.second << "$" << endl;
        }
    }
};

// Parser class for reading and parsing flight data from a file
class Parser {
public:
    vector<FlightInfo> parseFromFile(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error opening file: " << filename << endl;
            std::perror("Error");
            return {};
        }

        vector<FlightInfo> flights;
        int numberOfRecords;
        file >> numberOfRecords;
        if (file.fail()) {
            cerr << "Error reading the number of records!" << endl;
            file.close();
            return {};
        }
        file.ignore();  // Ignore the remaining newline after numberOfRecords

        for (int i = 0; i < numberOfRecords; ++i) {
            FlightInfo flight;
            file >> flight.date >> flight.flightNumber >> flight.seatsPerRow;
            if (file.fail()) {
                cerr << "Error reading flight information for record " << i + 1 << endl;
                file.close();
                return {};
            }
            file.ignore();  // Ignore the space or newline before row price data

            string rowPriceData;
            getline(file, rowPriceData);
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
            size_t dashPos = rowRange.find('-');
            if (dashPos != string::npos) {
                int rowStart = stoi(rowRange.substr(0, dashPos));
                int rowEnd = stoi(rowRange.substr(dashPos + 1));

                // Add price for each row in the range
                for (int row = rowStart; row <= rowEnd; ++row) {
                    flight.rowPricePairs.push_back(make_pair(row, price));
                }
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
        // Initialize seat map based on flightInfo and seatsPerRow
    }

    void displayFlightDetails() const {
        flightInfo.display();
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
    Ticket(const string& passenger, const string& flight, const string& seat)
        : passengerName(passenger), flightNumber(flight), seatNumber(seat), isBooked(true) {}

    void displayTicket() const {
        cout << "Passenger: " << passengerName << ", Flight: " << flightNumber
            << ", Seat: " << seatNumber << (isBooked ? " (Booked)" : " (Available)") << endl;
    }

    void cancelBooking() {
        isBooked = false;
    }
};

// Main application controller class
class FlightBookingApp {
private:
    vector<Airplane> airplanes;
    Parser parser;

public:
    void run() {
        loadFlights();
        handleUserCommands();
    }

private:
    void loadFlights() {
        vector<FlightInfo> flights = parser.parseFromFile("C:/Users/User/Documents/GitHub/OOP1/Console1/config.txt");
        for (const auto& flight : flights) {
            airplanes.emplace_back(flight);
        }
    }

    void handleUserCommands() {
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
            else if (command == "exit") {
                break;
            }
            else {
                cout << "Unknown command!" << endl;
            }
        }
    }

    void handleCheckCommand() {
        // Check available seats logic
    }

    void handleBookCommand() {
        // Booking logic
    }

    void handleReturnCommand() {
        // Ticket return logic
    }

    void handleViewCommand() {
        // View booked tickets logic
    }
};

int main() {
    FlightBookingApp app;
    app.run();
    return 0;
}
