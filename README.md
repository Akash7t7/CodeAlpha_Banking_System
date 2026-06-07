# Banking System — CodeAlpha Internship Task 3

A simple C++ console application that simulates a basic banking system with file-based storage.

## Features
- Create new bank account
- Secure login with account number and password
- Deposit money
- Withdraw money
- Fund transfer between accounts
- View transaction history
- Account information display
- Data stored in local files (accounts.txt, transactions.txt)

## How to Compile & Run

g++ banking_system.cpp -o banking_system
./banking_system

## Example Output

==============================
     Simple Banking System
==============================

--- Main Menu ---
1. Create Account
2. Login
3. Exit
Enter choice: 1

===== Create New Account =====
Enter your full name: Ahmed Ali
Enter account number (e.g. ACC001): ACC001
Set password (min 6 chars): pass123
Enter initial deposit (Rs.): 50000

Account created successfully!

====== Account Info ======
Account Number : ACC001
Customer Name  : Ahmed Ali
Balance        : Rs. 50000.00
==========================

## Language
C++

## Author
Developed as part of CodeAlpha C++ Programming Internship
