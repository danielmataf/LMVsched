import pdfkit
from jinja2 import Template
import random

# Define the employee data with constraints
employees = [
    {"name": "Jesus", "color": "#FF5733", "weekly_hours": 39, "daily_hours": [0, 0, 0, 0, 0, 0]},
    {"name": "Sarah", "color": "#33FF57", "weekly_hours": 39, "daily_hours": [0, 0, 0, 0, 0, 0]},
    {"name": "Sasa", "color": "#3375FF", "weekly_hours": 25, "daily_hours": [0, 0, 0, 0, 0, 0]},
    {"name": "Anna", "color": "#F57F17", "weekly_hours": 25, "daily_hours": [0, 0, 0, 0, 0, 0]},
    {"name": "Yael", "color": "#9C27B0", "weekly_hours": 35, "daily_hours": [0, 0, 0, 0, 0, 0]},
    {"name": "Nae", "color": "#FFEB3B", "weekly_hours": 35, "daily_hours": [0, 0, 0, 0, 0, 0]},
    {"name": "Juliette", "color": "#FF4081", "weekly_hours": 15, "daily_hours": [0, 0, 0, 0, 0, 0]}
]

# Define the HTML template for the schedule
html_template = """
<!DOCTYPE html>
<html>
<head>
    <style>
        body { font-family: Arial, sans-serif; }
        table { border-collapse: collapse; width: 100%; margin-bottom: 20px; }
        th, td { border: 1px solid black; padding: 5px; text-align: center; }
        .employee { background-color: {{ color }}; }
        .break { background-color: #FFFFFF; }
    </style>
</head>
<body>
    <h1>Weekly Shift Schedule</h1>
    {% for day in days %}
    <h2>{{ day.name }}</h2>
    <table>
        <tr>
            <th>Time</th>
            {% for employee in employees %}
            <th>{{ employee.name }}</th>
            {% endfor %}
        </tr>
        {% for time in day.times %}
        <tr>
            <td>{{ time }}</td>
            {% for employee in employees %}
            <td style="background-color: {{ employee.color if time in employee.schedule[loop.index0] else '#FFFFFF' }}">
            </td>
            {% endfor %}
        </tr>
        {% endfor %}
    </table>
    {% endfor %}
</body>
</html>
"""

# Define the schedule for each day
days = [
    {"name": "Mardi", "times": ["8h00", "8h30", "9h00", "9h30", "10h00", "10h30", "11h00", "11h30", "12h00", "12h30", "13h00", "13h30", "14h00", "14h30", "15h00", "15h30", "16h00", "16h30", "17h00", "17h30", "18h00", "18h30", "19h00", "19h30", "20h00", "20h30"]},
    {"name": "Mercredi", "times": ["8h00", "8h30", "9h00", "9h30", "10h00", "10h30", "11h00", "11h30", "12h00", "12h30", "13h00", "13h30", "14h00", "14h30", "15h00", "15h30", "16h00", "16h30", "17h00", "17h30", "18h00", "18h30", "19h00", "19h30", "20h00", "20h30"]},
    {"name": "Jeudi", "times": ["8h00", "8h30", "9h00", "9h30", "10h00", "10h30", "11h00", "11h30", "12h00", "12h30", "13h00", "13h30", "14h00", "14h30", "15h00", "15h30", "16h00", "16h30", "17h00", "17h30", "18h00", "18h30", "19h00", "19h30", "20h00", "20h30"]},
    {"name": "Vendredi", "times": ["8h00", "8h30", "9h00", "9h30", "10h00", "10h30", "11h00", "11h30", "12h00", "12h30", "13h00", "13h30", "14h00", "14h30", "15h00", "15h30", "16h00", "16h30", "17h00", "17h30", "18h00", "18h30", "19h00", "19h30", "20h00", "20h30"]},
    {"name": "Samedi", "times": ["7h30", "8h00", "8h30", "9h00", "9h30", "10h00", "10h30", "11h00", "11h30", "12h00", "12h30", "13h00", "13h30", "14h00", "14h30", "15h00", "15h30", "16h00", "16h30", "17h00", "17h30", "18h00", "18h30", "19h00", "19h30", "20h00", "20h30"]},
    {"name": "Dimanche", "times": ["8h00", "8h30", "9h00", "9h30", "10h00", "10h30", "11h00", "11h30", "12h00", "12h30", "13h00", "13h30", "14h00"]}
]

# Schedule generation logic considering constraints
for employee in employees:
    employee["schedule"] = [[] for _ in range(6)]  # Initialize empty schedule slots for each day
    employee["breaks"] = [[] for _ in range(6)]  # Initialize empty breaks slots for each day

for day_index, day in enumerate(days):
    for employee in employees:
        shift_hours = employee["weekly_hours"] * 60 // 6  # Calculate daily shift duration
        current_minutes = 0
        start_time_index = 0  # Reset start time index for each employee each day
        
        # Skip unnecessary loop iterations based on constraints
        if employee["name"] == "Juliette" and day_index < 4:  # Juliette works only weekends
            continue
        if employee["name"] == "Sarah" and day_index == 1:  # Sarah's Wednesday constraint
            end_index = day["times"].index("17h00")
        else:
            end_index = len(day["times"])

        while current_minutes < shift_hours and start_time_index < end_index:
            # Assign shift slots dynamically
            for time_index in range(start_time_index, end_index):
                if current_minutes >= shift_hours:
                    break
                # Apply breaks after 4 consecutive hours
                if len(employee["schedule"][day_index]) > 0 and len(employee["schedule"][day_index]) % 4 == 0:
                    employee["breaks"][day_index].append(day["times"][time_index])
                    current_minutes += 60  # Add 1-hour break
                    start_time_index = time_index + 2  # Skip break slots
                    break

                # Randomly decide to assign a shift or not
                if random.choice([True, False]):
                    employee["schedule"][day_index].append(day["times"][time_index])
                    current_minutes += 30  # Increment by 30 minutes
                    start_time_index = time_index + 1  # Move to the next slot

# Render HTML using Jinja2
template = Template(html_template)
html_out = template.render(employees=employees, days=days)

# Write the output to an HTML file and convert it to PDF
with open("schedule.html", "w") as file:
    file.write(html_out)

# Generate PDF using pdfkit
pdfkit.from_file("schedule.html", "shift_schedule.pdf")
