#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <memory>

using namespace std;

// FlightInfo structure for storing flight details
struct FlightInfo {
    string date;
    string flightNumber;
    int seatsPerRow;
    vector<pair<int, int>> rowPricePairs;

    int getNumRows() const {
        int maxRow = 0;
        for (const auto& pair : rowPricePairs) {
            if (pair.first > maxRow) {
                maxRow = pair.first;
            }
        }
        return maxRow;
    }
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
            parseRowPrices(rowPriceData, flight);
            flights.push_back(flight);
        }

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
    vector<vector<string>> seatMap;
    unordered_map<string, string> bookedSeats;

public:
    Airplane(const FlightInfo& flight) : flightInfo(flight) {
        initializeSeatMap();
    }

    bool bookSeat(const string& seat, const string& passengerName) {
        if (bookedSeats.find(seat) != bookedSeats.end()) {
            return false;
        }
        bookedSeats[seat] = passengerName;
        return true;
    }

    int getSeatsPerRow() const {
        return flightInfo.seatsPerRow;
    }

    void returnTicket(const string& seat) {
        auto it = bookedSeats.find(seat);
        if (it != bookedSeats.end()) {
            bookedSeats.erase(it);
            cout << "Ticket returned successfully." << endl;
        }
        else {
            cout << "Ticket not found!" << endl;
        }
    }

    void initializeSeatMap() {
        int numRows = flightInfo.getNumRows();
        int seatsPerRow = flightInfo.seatsPerRow;

        seatMap.resize(numRows);

        for (int i = 0; i < numRows; ++i) {
            seatMap[i].resize(seatsPerRow, " ");
        }
    }

    void displayFlightDetails() const {
        cout << "Date: " << flightInfo.date << ", Flight: " << flightInfo.flightNumber
            << ", Seats per row: " << flightInfo.seatsPerRow << endl;
        for (const auto& rowPrice : flightInfo.rowPricePairs) {
            cout << "Rows: 1-" << rowPrice.first << ", Price: " << rowPrice.second << "$" << endl;
        }
    }

    const string& getFlightNumber() const {
        return flightInfo.flightNumber;
    }

    const string& getDate() const {
        return flightInfo.date;
    }

    const unordered_map<string, string>& getBookedSeats() const {
        return bookedSeats;
    }

    int getPriceForRow(int row) const {
        for (const auto& pair : flightInfo.rowPricePairs) {
            if (row <= pair.first) {
                return pair.second;
            }
        }
        return 0; // Default price if row not found
    }

    bool isSeatValid(const string& seat) const {
        if (seat.empty() || seat.size() < 2) return false;

        string rowStr = seat.substr(0, seat.size() - 1);
        char seatChar = seat.back();

        int row;
        try {
            row = stoi(rowStr);
        }
        catch (const invalid_argument&) {
            return false;
        }

        return row > 0 && row <= flightInfo.getNumRows() &&
            seatChar >= 'A' && seatChar < 'A' + flightInfo.seatsPerRow;
    }

};

// Ticket class for storing booking information
class Ticket {
private:
    string id;
    string passengerName;
    string flightDate;
    string flightNumber;
    string seatNumber;
    bool isBooked;
    int price;

public:
    Ticket() : id(""), passengerName(""), flightDate(""), flightNumber(""), seatNumber(""), isBooked(false), price(0) {}

    Ticket(const string& id, const string& passenger, const string& date, const string& flight, const string& seat, int price)
        : id(id), passengerName(passenger), flightDate(date), flightNumber(flight), seatNumber(seat), isBooked(true), price(price) {}

    void displayTicket() const {
        cout << "Flight: " << flightNumber << ", Date: " << flightDate << ", seat: " << seatNumber << ", price: " << price << endl;
    }

    void cancelBooking() {
        isBooked = false;
    }

    const string& getPassengerName() const {
        return passengerName;
    }

    const string& getId() const {
        return id;
    }

    int getPrice() const {
        return price;
    }

    const string& getDate() const {
        return flightDate;
    }

    const string& getFlightNumber() const {
        return flightNumber;
    }

    const string& getSeatNumber() const {
        return seatNumber;
    }
};

// Class to handle user commands and validate input
class FlightCommandHandler {
private:
    vector<Airplane>& airplanes;
    unordered_map<string, shared_ptr<Ticket>> tickets; // Ticket map
    int ticketIdCounter; // Counter for generating unique ticket IDs

public:
    FlightCommandHandler(vector<Airplane>& airplanes) : airplanes(airplanes), ticketIdCounter(0) {}

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
            const auto& seatMap = airplane->getBookedSeats();
            bool availableSeats = false;
            int seatsPerRow = airplane->getSeatsPerRow();

            for (int row = 1; row <= 50; ++row) { // Assuming 50 rows; adjust if necessary
                for (char seat = 'A'; seat < 'A' + seatsPerRow; ++seat) {
                    string seatID = to_string(row) + seat;
                    if (airplane->isSeatValid(seatID) && seatMap.find(seatID) == seatMap.end()) { // If seat is valid and not booked
                        int price = airplane->getPriceForRow(row);
                        cout << seatID << " " << price << "$, ";
                        availableSeats = true;
                    }
                }
            }

            if (!availableSeats) {
                cout << "No available seats." << endl;
            }
            else {
                cout << endl; // To finalize the line after the list of seats
            }
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
            if (airplane->isSeatValid(seat)) {
                bool success = airplane->bookSeat(seat, passengerName);
                if (success) {
                    // Generate a unique ticket ID
                    string ticketId = to_string(++ticketIdCounter);
                    int price = getSeatPrice(seat, *airplane); // Get price based on seat
                    auto ticket = make_shared<Ticket>(ticketId, passengerName, date, flightNo, seat, price);
                    tickets[ticketId] = ticket;
                    cout << "Booking confirmed with ID " << ticketId << endl;
                }
                else {
                    cout << "Booking failed. Seat may be unavailable." << endl;
                }
            }
            else {
                cout << "Invalid seat number!" << endl;
            }
        }
        else {
            cout << "Flight not found!" << endl;
        }
    }

    void handleReturnCommand() {
        string ticketId;
        cin >> ticketId;

        auto it = tickets.find(ticketId);
        if (it != tickets.end()) {
            auto ticket = it->second;
            if (ticket) { // Ensure ticket is not null
                ticket->cancelBooking();
                tickets.erase(it);
                cout << "Ticket returned successfully. Refund of " << ticket->getPrice() << "$ for " << ticket->getPassengerName() << endl;
            }
            else {
                cout << "Ticket is invalid!" << endl;
            }
        }
        else {
            cout << "Ticket not found!" << endl;
        }
    }

    void handleViewCommand() {
        string paramType;
        string flightDate, flightNumber;

        cin >> paramType;

        if (paramType == "ID") {
            string ticketId;
            cin >> ticketId;
            viewTicketById(ticketId);
        }
        else if (paramType == "username") {
            string passengerName;
            cin >> passengerName;
            viewTicketsByPassenger(passengerName);
        }
        else if (paramType == "flight") {
            cin >> flightDate >> flightNumber;
            viewTicketsByFlight(flightDate, flightNumber);
        }
        else {
            cout << "Unknown view command parameter!" << endl;
        }
    }

    void viewTicketById(const string& id) {
        auto it = tickets.find(id);
        if (it != tickets.end()) {
            it->second->displayTicket();
        }
        else {
            cout << "Ticket not found!" << endl;
        }
    }

    void viewTicketsByPassenger(const string& passenger) {
        bool found = false;
        for (const auto& pair : tickets) {
            if (pair.second->getPassengerName() == passenger) {
                pair.second->displayTicket();
                found = true;
            }
        }
        if (!found) {
            cout << "No tickets found for passenger: " << passenger << endl;
        }
    }

    void viewTicketsByFlight(const string& flightDate, const string& flightNumber) {
        bool found = false;

        for (const auto& pair : tickets) {
            const auto& ticket = pair.second;

            if (ticket->getDate() == flightDate && ticket->getFlightNumber() == flightNumber) {
                ticket->displayTicket();
                found = true;
            }
        }

        if (!found) {
            cout << "No tickets found for flight " << flightNumber << " on date " << flightDate << endl;
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

    int getSeatPrice(const string& seat, const Airplane& airplane) {
        int row = stoi(seat.substr(0, seat.size() - 1));
        return airplane.getPriceForRow(row);
    }
};

// Main application controller class
class FlightBookingApp {
private:
    vector<Airplane> airplanes;
    Parser parser;
    unique_ptr<FlightCommandHandler> commandHandler;

    void loadFlights() {
        vector<FlightInfo> flights = parser.parseFromFile("C:/Users/User/Documents/GitHub/OOP1/Console1/config.txt");
        for (const auto& flight : flights) {
            airplanes.emplace_back(flight);
        }
    }

public:
    FlightBookingApp() {}

    void run() {
        loadFlights();
        commandHandler = make_unique<FlightCommandHandler>(airplanes);
        commandHandler->handleCommands();
    }
};

int main() {
    FlightBookingApp app;
    app.run();
    return 0;
}
