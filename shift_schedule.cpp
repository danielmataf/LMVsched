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
    std::string color;
    int weeklyHours;
    int dailyWorkedHours[7];   // Hours worked per day (0 = Tuesday, 6 = Sunday)
    bool isManager;

    // Constraints
    bool mustWork[7][2];   // Mandatory work hours (startHour, endHour)
    bool mustBeOff[7][2];  // Off-hours constraints (startHour, endHour)
    int maxDailyHours;
    int minConsecutiveHours;

    // Constructor
    Employee(std::string n, std::string c, int h, bool manager, int maxDaily, int minConsecutive)
        : name(n), color(c), weeklyHours(h), isManager(manager), maxDailyHours(maxDaily), minConsecutiveHours(minConsecutive) {
        for (int i = 0; i < 7; i++) {
            dailyWorkedHours[i] = 0;
            mustWork[i][0] = mustWork[i][1] = -1;
            mustBeOff[i][0] = mustBeOff[i][1] = -1;
        }
    }

    // Check if available for a shift
    bool isAvailable(int day, int startHour, int endHour) {
        if (mustBeOff[day][0] != -1 && startHour >= mustBeOff[day][0] && endHour <= mustBeOff[day][1]) {
            return false;
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

// Class to represent each day
class Day {
public:
    int dayType;
    std::string dayName;
    int openTime;
    int closeTime;
    std::vector<std::vector<int>> schedule;  // Schedule matrix: employees x time slots

    Day(int type, const std::string& name) : dayType(type), dayName(name) {
        switch (dayType) {
            case 1:
                openTime = 8 * 60;
                closeTime = 20 * 60 + 30;
                break;
            case 2:
                openTime = 7 * 60 + 30;
                closeTime = 20 * 60 + 30;
                break;
            case 3:
                openTime = 8 * 60;
                closeTime = 14 * 60;
                break;
            default:
                openTime = 0;
                closeTime = 0;
        }
        int numSlots = (closeTime - openTime) / 30; // Calculate the number of slots
        schedule = std::vector<std::vector<int>>(7, std::vector<int>(numSlots, 0)); // Initialize schedule
    }

    // Function to add employee shifts
    void addEmployeeShift(Employee& employee, int day, std::mt19937& g) {
        int current_minutes = 0;
        std::vector<int> availableSlots;

        for (int i = 0; i < schedule[0].size(); ++i) {
            int startHour = openTime + i * 30;
            int endHour = startHour + 30;
            if (employee.isAvailable(day, startHour, endHour)) {
                availableSlots.push_back(i);
            }
        }

        // Shuffle available slots to randomize assignments
        std::shuffle(availableSlots.begin(), availableSlots.end(), g);

        for (int slot : availableSlots) {
            if (current_minutes >= employee.maxDailyHours) break;

            int startHour = openTime + slot * 30;
            int endHour = startHour + 30;

            if (employee.isAvailable(day, startHour, endHour) && slot < schedule[0].size()) {
                schedule[employee.name[0] - 'A'][slot] = 1;  // Corrected indexing to avoid out-of-bounds errors
                employee.assignShift(day, startHour, endHour);
                current_minutes += 30;
            }
        }
    }
};

// Function to generate the schedule for the entire week
void generateWeeklySchedule(std::vector<Employee>& employees, std::vector<Day>& week, std::ofstream& outputFile) {
    std::random_device rd;
    std::mt19937 g(rd());

    for (int dayIndex = 0; dayIndex < week.size(); ++dayIndex) {
        outputFile << "\n\n" << week[dayIndex].dayName << ": (in 30-minute slots)\n";
        outputFile << "     ,A,B,C,D,E,F,G\n";

        // Shuffle the employees to vary assignment
        std::shuffle(employees.begin(), employees.end(), g);

        for (auto& employee : employees) {
            if (!(employee.name == "G" && week[dayIndex].dayType == 1)) {
                week[dayIndex].addEmployeeShift(employee, dayIndex, g);
            }
        }

        // Print the schedule
        for (int i = 0; i < week[dayIndex].schedule[0].size(); ++i) {
            int hour = week[dayIndex].openTime / 60 + (i / 2);
            int minute = (i % 2 == 0) ? 0 : 30;
            outputFile << std::setw(2) << std::setfill('0') << hour << "h" << std::setw(2) << std::setfill('0') << minute << ",";

            for (int j = 0; j < 7; ++j) {
                outputFile << week[dayIndex].schedule[j][i] << ",";
            }
            outputFile << "\n";
        }

        outputFile << "\nDaily summary:\n";
        for (auto& employee : employees) {
            outputFile << employee.name << ": " << employee.dailyWorkedHours[dayIndex] / 60 << " hours " << (employee.dailyWorkedHours[dayIndex] % 60) << " minutes\n";
        }

        outputFile << "\n\n\n";
    }
}

// Main function
int main() {
    srand(time(0));

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
    employees[0].addMustWorkConstraint(0, 12 * 60 + 30, 15 * 60 + 30);  
    employees[0].addMustWorkConstraint(4, 12 * 60 + 30, 15 * 60 + 30);

    employees[1].addMustWorkConstraint(0, 12 * 60 + 30, 15 * 60 + 30);  
    employees[1].addMustWorkConstraint(4, 12 * 60 + 30, 15 * 60 + 30);
    employees[1].addMustBeOffConstraint(2, 17 * 60, 20 * 60 + 30); 

    for (int i = 0; i < 4; ++i) {  
        employees[6].addMustBeOffConstraint(i, 0, 24 * 60);  
    }

    std::vector<Day> week = {
        Day(1, "Mardi"), Day(1, "Mercredi"), Day(1, "Jeudi"), Day(1, "Vendredi"),
        Day(2, "Samedi"), Day(3, "Dimanche")
    };

    std::ofstream outputFile("shift_schedule.txt");

    if (!outputFile) {
        std::cerr << "Error opening file for output.\n";
        return 1;
    }

    generateWeeklySchedule(employees, week, outputFile);

    outputFile.close();
    std::cout << "Schedule written to shift_schedule.txt.\n";

    return 0;
}
