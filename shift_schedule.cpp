#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <iomanip>  // For formatting output
#include <algorithm> // For std::shuffle
#include <random>    // For random number generation

class Employee {
public:
    std::string name;
    std::string color;   // Unique color for each employee (used for PDF later)
    int weeklyHours;     // Total contracted weekly hours
    int dailyWorkedHours[7];   // Hours worked per day (0 = Tuesday, 6 = Sunday)
    bool isManager;      // If the employee is a manager (A and B)

    // Constraints
    bool mustWork[7][2];   // Mandatory work hours (startHour, endHour)
    bool mustBeOff[7][2];  // Off-hours constraints (startHour, endHour)
    int maxDailyHours;     // Max shift hours per day
    int minConsecutiveHours; // Minimum shift length (4 hours)

    // Constructor
    Employee(std::string n, std::string c, int h, bool manager, int maxDaily, int minConsecutive)
        : name(n), color(c), weeklyHours(h), isManager(manager), maxDailyHours(maxDaily), minConsecutiveHours(minConsecutive) {
        for (int i = 0; i < 7; i++) {
            dailyWorkedHours[i] = 0;
            mustWork[i][0] = mustWork[i][1] = -1;  // No mandatory work slots by default
            mustBeOff[i][0] = mustBeOff[i][1] = -1; // No mandatory off slots by default
        }
    }
    
    // Check if available for a shift
    bool isAvailable(int day, int startHour, int endHour) {
        if (mustBeOff[day][0] != -1 && startHour >= mustBeOff[day][0] && endHour <= mustBeOff[day][1]) {
            return false; // The employee is required to be off during this time
        }
        return true;
    }

    // Assign shift
    void assignShift(int day, int startHour, int endHour) {
        dailyWorkedHours[day] += (endHour - startHour);
    }

    // Add constraints: mandatory working times and off times
    void addMustWorkConstraint(int day, int startHour, int endHour) {
        mustWork[day][0] = startHour;
        mustWork[day][1] = endHour;
    }

    void addMustBeOffConstraint(int day, int startHour, int endHour) {
        mustBeOff[day][0] = startHour;
        mustBeOff[day][1] = endHour;
    }
};

// Function to generate a random schedule and write it to a text file
void generateSchedule(std::vector<Employee> &employees, std::ofstream &outputFile) {
    const int openTime[6] = {8 * 60, 8 * 60, 8 * 60, 8 * 60, 7 * 60 + 30, 8 * 60};  // Store opening times for each day
    const int closeTime[6] = {20 * 60 + 30, 20 * 60 + 30, 20 * 60 + 30, 20 * 60 + 30, 20 * 60 + 30, 14 * 60};  // Closing times

    // Random number generator
    std::random_device rd;
    std::mt19937 g(rd());

    // Iterate over each day
    for (int day = 0; day < 6; ++day) {
        outputFile << "\n\nDay " << day + 1 << ": (in 30-minute slots)\n";
        outputFile << "     ,A,B,C,D,E,F,G\n"; // Header for each day with 5 spaces at the start

        int hoursWorked[7] = {0}; // Track daily hours for each employee
        
        for (int time = openTime[day]; time <= closeTime[day]; time += 30) {
            // Write the time slot to the file
            int hour = time / 60;
            int minute = time % 60;
            outputFile << std::setw(2) << std::setfill('0') << hour << "h" << std::setw(2) << std::setfill('0') << minute << ",";
            
            std::vector<int> availableEmployees;

            // Find available employees for this time slot
            for (size_t i = 0; i < employees.size(); ++i) {
                if (employees[i].isAvailable(day, time, time + 30)) {
                    availableEmployees.push_back(i);
                }
            }

            // Shuffle available employees
            std::shuffle(availableEmployees.begin(), availableEmployees.end(), g);
            int assignedCount = std::min(4, static_cast<int>(availableEmployees.size()));  // Maximum 4 employees per slot

            // Initialize all slots to 0 (not working)
            int workSlots[7] = {0, 0, 0, 0, 0, 0, 0};

            // Assign shifts
            for (int i = 0; i < assignedCount; ++i) {
                employees[availableEmployees[i]].assignShift(day, time, time + 30);
                hoursWorked[availableEmployees[i]] += 30;  // 30 minutes increment
                workSlots[availableEmployees[i]] = 1; // Mark the employee as working this slot
            }

            // Output the results to the file
            for (int i = 0; i < 7; ++i) {
                outputFile << workSlots[i] << ",";
            }
            outputFile << "\n"; // New line for the next time slot
        }

        // Print the total hours worked by each employee at the end of the day
        outputFile << "\nDaily summary:\n";
        for (size_t i = 0; i < employees.size(); ++i) {
            outputFile << employees[i].name << ": " << hoursWorked[i] / 60 << " hours " << (hoursWorked[i] % 60) << " minutes\n";
        }

        // Skip three lines before the next day
        outputFile << "\n\n\n";
    }
}

// Main function
int main() {
    srand(time(0));  // Seed random number generator for unique outputs on every run

    // Initialize employees with their contract hours and constraints
    std::vector<Employee> employees;
    employees.push_back(Employee("A", "#FF5733", 39, true, 8 * 60 + 30, 4 * 60));
    employees.push_back(Employee("B", "#33FF57", 39, true, 8 * 60 + 30, 4 * 60));
    employees.push_back(Employee("C", "#3375FF", 25, false, 8 * 60, 4 * 60));
    employees.push_back(Employee("D", "#F57F17", 25, false, 8 * 60, 4 * 60));
    employees.push_back(Employee("E", "#9C27B0", 35, false, 8 * 60 + 30, 4 * 60));
    employees.push_back(Employee("F", "#FFEB3B", 35, false, 8 * 60 + 30, 4 * 60));
    employees.push_back(Employee("G", "#FF4081", 15, false, 8 * 60, 4 * 60));

    // Add specific constraints
    employees[0].addMustWorkConstraint(0, 12 * 60 + 30, 15 * 60 + 30);  // A (Tuesday 12:30-15:30)
    employees[0].addMustWorkConstraint(4, 12 * 60 + 30, 15 * 60 + 30);  // A (Friday 12:30-15:30)

    employees[1].addMustWorkConstraint(0, 12 * 60 + 30, 15 * 60 + 30);  // B (Tuesday 12:30-15:30)
    employees[1].addMustWorkConstraint(4, 12 * 60 + 30, 15 * 60 + 30);  // B (Friday 12:30-15:30)
    employees[1].addMustBeOffConstraint(2, 17 * 60, 20 * 60 + 30);      // B (Wednesday after 5pm)

    // Ensure G (Juliette) only works on weekends
    for (int i = 0; i < 4; ++i) {  // Days 0-3 correspond to Tuesday-Friday
        employees[6].addMustBeOffConstraint(i, 0, 24 * 60);  // G off all day from Tuesday to Friday
    }

    // Open file for output
    std::ofstream outputFile("shift_schedule.txt");

    if (!outputFile) {
        std::cerr << "Error opening file for output.\n";
        return 1;
    }

    // Generate a random weekly schedule and output to file
    generateSchedule(employees, outputFile);

    outputFile.close();
    std::cout << "Schedule written to shift_schedule.txt.\n";

    return 0;
}
