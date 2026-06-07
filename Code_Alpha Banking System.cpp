#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <sstream>
using namespace std;

// ===== HELPER: double to string =====
string doubleToStr(double val) {
    ostringstream oss;
    oss << fixed << setprecision(2) << val;
    return oss.str();
}

// ===== HELPER: Split CSV line by | =====
// Format: accountNumber|customerName|password|balance
void parseLine(string line, string &num, string &name, string &pass, double &bal) {
    stringstream ss(line);
    string balStr;
    getline(ss, num,  '|');
    getline(ss, name, '|');
    getline(ss, pass, '|');
    getline(ss, balStr);
    bal = stod(balStr);
}

// ===== ACCOUNT CLASS =====
class Account {
public:
    string accountNumber;
    string customerName;
    string password;
    double balance;

    Account() { balance = 0; }

    void saveToFile() {
        ofstream file("accounts.txt", ios::app);
        // Save as pipe-separated: ACC001|Ahmed Ali|pass123|5000.00
        file << accountNumber << "|" << customerName << "|" << password << "|" << doubleToStr(balance) << "\n";
        file.close();
    }

    void displayInfo() {
        cout << "\n====== Account Info ======" << endl;
        cout << "Account Number : " << accountNumber << endl;
        cout << "Customer Name  : " << customerName << endl;
        cout << "Balance        : Rs. " << fixed << setprecision(2) << balance << endl;
        cout << "==========================" << endl;
    }
};

// ===== HELPER: Account exists? =====
bool accountExists(string accNum) {
    ifstream file("accounts.txt");
    if (!file.is_open()) return false;
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        string num, name, pass;
        double bal;
        parseLine(line, num, name, pass, bal);
        if (num == accNum) { file.close(); return true; }
    }
    file.close();
    return false;
}

// ===== HELPER: Load account =====
Account loadAccount(string accNum) {
    Account acc;
    ifstream file("accounts.txt");
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        string num, name, pass;
        double bal;
        parseLine(line, num, name, pass, bal);
        if (num == accNum) {
            acc.accountNumber = num;
            acc.customerName  = name;
            acc.password      = pass;
            acc.balance       = bal;
            break;
        }
    }
    file.close();
    return acc;
}

// ===== HELPER: Update balance in file =====
void updateBalanceInFile(string accNum, double newBalance) {
    ifstream file("accounts.txt");
    string lines[100];
    int count = 0;
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        string num, name, pass;
        double bal;
        parseLine(line, num, name, pass, bal);
        if (num == accNum)
            lines[count] = num + "|" + name + "|" + pass + "|" + doubleToStr(newBalance);
        else
            lines[count] = num + "|" + name + "|" + pass + "|" + doubleToStr(bal);
        count++;
    }
    file.close();

    ofstream outFile("accounts.txt");
    for (int i = 0; i < count; i++)
        outFile << lines[i] << "\n";
    outFile.close();
}

// ===== HELPER: Save transaction =====
void saveTransaction(string accNum, string type, double amount) {
    ofstream file("transactions.txt", ios::app);
    file << accNum << "|" << type << "|" << doubleToStr(amount) << "\n";
    file.close();
}

// ===== CREATE ACCOUNT =====
void createAccount() {
    Account acc;
    cout << "\n===== Create New Account =====" << endl;

    cout << "Enter your full name: ";
    cin.ignore();
    getline(cin, acc.customerName);   // getline so full name with spaces works

    if (acc.customerName.empty()) {
        cout << "Name cannot be empty!" << endl;
        return;
    }

    cout << "Enter account number (e.g. ACC001): ";
    cin >> acc.accountNumber;

    if (accountExists(acc.accountNumber)) {
        cout << "Account number already exists! Try another." << endl;
        return;
    }

    cout << "Set password (min 6 chars): ";
    cin >> acc.password;

    if (acc.password.length() < 6) {
        cout << "Password too short!" << endl;
        return;
    }

    cout << "Enter initial deposit (Rs.): ";
    cin >> acc.balance;

    if (acc.balance < 0) {
        cout << "Invalid amount!" << endl;
        return;
    }

    acc.saveToFile();
    saveTransaction(acc.accountNumber, "INITIAL_DEPOSIT", acc.balance);
    cout << "\nAccount created successfully!" << endl;
    acc.displayInfo();
}

// ===== LOGIN =====
Account loginAccount() {
    Account acc;
    string accNum, pass;

    cout << "\n===== Login =====" << endl;
    cout << "Enter account number: ";
    cin >> accNum;
    cout << "Enter password: ";
    cin >> pass;

    if (!accountExists(accNum)) {
        cout << "Account not found!" << endl;
        return acc;
    }

    acc = loadAccount(accNum);

    if (acc.password != pass) {
        cout << "Wrong password!" << endl;
        acc.accountNumber = "";
        return acc;
    }

    cout << "Login Successful! Welcome, " << acc.customerName << "!" << endl;
    return acc;
}

// ===== DEPOSIT =====
void deposit(Account &acc) {
    double amount;
    cout << "\n===== Deposit =====" << endl;
    cout << "Enter amount to deposit (Rs.): ";
    cin >> amount;

    if (amount <= 0) { cout << "Invalid amount!" << endl; return; }

    acc.balance += amount;
    updateBalanceInFile(acc.accountNumber, acc.balance);
    saveTransaction(acc.accountNumber, "DEPOSIT", amount);

    cout << "Rs. " << fixed << setprecision(2) << amount << " deposited successfully!" << endl;
    cout << "New Balance: Rs. " << acc.balance << endl;
}

// ===== WITHDRAW =====
void withdraw(Account &acc) {
    double amount;
    cout << "\n===== Withdraw =====" << endl;
    cout << "Enter amount to withdraw (Rs.): ";
    cin >> amount;

    if (amount <= 0) { cout << "Invalid amount!" << endl; return; }

    if (amount > acc.balance) {
        cout << "Insufficient balance! Your balance: Rs. " << acc.balance << endl;
        return;
    }

    acc.balance -= amount;
    updateBalanceInFile(acc.accountNumber, acc.balance);
    saveTransaction(acc.accountNumber, "WITHDRAWAL", amount);

    cout << "Rs. " << fixed << setprecision(2) << amount << " withdrawn successfully!" << endl;
    cout << "New Balance: Rs. " << acc.balance << endl;
}

// ===== FUND TRANSFER =====
void transfer(Account &acc) {
    string targetAcc;
    double amount;

    cout << "\n===== Fund Transfer =====" << endl;
    cout << "Enter target account number: ";
    cin >> targetAcc;

    if (!accountExists(targetAcc)) { cout << "Target account not found!" << endl; return; }
    if (targetAcc == acc.accountNumber) { cout << "Cannot transfer to your own account!" << endl; return; }

    cout << "Enter amount to transfer (Rs.): ";
    cin >> amount;

    if (amount <= 0) { cout << "Invalid amount!" << endl; return; }
    if (amount > acc.balance) {
        cout << "Insufficient balance! Your balance: Rs. " << acc.balance << endl;
        return;
    }

    acc.balance -= amount;
    updateBalanceInFile(acc.accountNumber, acc.balance);
    saveTransaction(acc.accountNumber, "TRANSFER_OUT", amount);

    Account receiver = loadAccount(targetAcc);
    receiver.balance += amount;
    updateBalanceInFile(targetAcc, receiver.balance);
    saveTransaction(targetAcc, "TRANSFER_IN", amount);

    cout << "Rs. " << fixed << setprecision(2) << amount << " transferred to " << targetAcc << " successfully!" << endl;
    cout << "Your New Balance: Rs. " << acc.balance << endl;
}

// ===== VIEW TRANSACTIONS =====
void viewTransactions(Account &acc) {
    cout << "\n===== Transaction History [" << acc.accountNumber << "] =====" << endl;

    ifstream file("transactions.txt");
    if (!file.is_open()) { cout << "No transactions found." << endl; return; }

    string line;
    bool found = false;

    cout << left << setw(15) << "Account" << setw(20) << "Type" << "Amount (Rs.)" << endl;
    cout << "-------------------------------------------" << endl;

    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string accNum, type, amtStr;
        getline(ss, accNum, '|');
        getline(ss, type,   '|');
        getline(ss, amtStr);

        if (accNum == acc.accountNumber) {
            cout << left << setw(15) << accNum << setw(20) << type << amtStr << endl;
            found = true;
        }
    }
    file.close();

    if (!found) cout << "No transactions yet." << endl;
}

// ===== ACCOUNT MENU =====
void accountMenu(Account &acc) {
    int choice;
    do {
        cout << "\n====== Account Menu ======" << endl;
        cout << "Welcome, " << acc.customerName << endl;
        cout << "Balance: Rs. " << fixed << setprecision(2) << acc.balance << endl;
        cout << "--------------------------" << endl;
        cout << "1. Deposit" << endl;
        cout << "2. Withdraw" << endl;
        cout << "3. Fund Transfer" << endl;
        cout << "4. View Transactions" << endl;
        cout << "5. Account Info" << endl;
        cout << "6. Logout" << endl;
        cout << "Enter choice: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Invalid input!" << endl;
            continue;
        }

        switch (choice) {
            case 1: deposit(acc);           break;
            case 2: withdraw(acc);          break;
            case 3: transfer(acc);          break;
            case 4: viewTransactions(acc);  break;
            case 5: acc.displayInfo();      break;
            case 6: cout << "Logged out successfully!" << endl; break;
            default: cout << "Invalid choice!" << endl;
        }
    } while (choice != 6);
}

// ===== MAIN =====
int main() {
    int choice;

    cout << "==============================" << endl;
    cout << "     Simple Banking System    " << endl;
    cout << "==============================" << endl;

    do {
        cout << "\n--- Main Menu ---" << endl;
        cout << "1. Create Account" << endl;
        cout << "2. Login" << endl;
        cout << "3. Exit" << endl;
        cout << "Enter choice: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Please enter a valid number!" << endl;
            continue;
        }

        switch (choice) {
            case 1: createAccount(); break;
            case 2: {
                Account acc = loginAccount();
                if (!acc.accountNumber.empty()) accountMenu(acc);
                break;
            }
            case 3: cout << "Thank you for using our banking system!" << endl; break;
            default: cout << "Invalid choice! Enter 1, 2, or 3." << endl;
        }
    } while (choice != 3);

    return 0;
}
