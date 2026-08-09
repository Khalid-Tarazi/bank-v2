#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>

using namespace std;

struct stUser {
    string userName;
    string password;
    int permissions;
    bool markForDelete = false;
};

enum enTransactionsMenuOptions {eDeposit = 1, eWithDraw = 2, eTotalBalance = 3, eShowMainMenu = 4};

enum enManageUsersMenuOptions {
    eListUsers = 1, eAddNewUser = 2, eDeleteUser = 3,
    eUpdateUser = 4, eFindUser = 5, eMainMenu = 6
};

enum enMainMenuOptions {
    eListClients = 1, eAddNewClient = 2,
    eDeleteClient = 3, eUpdateClient = 4,
    eFindClient = 5, eTransactions = 6, eManageUsers = 7, eLogout
};

enum enMainMenuPermissions {
    eAll = -1, pListClients = 1, pAddNewClient = 2, pDeleteClient = 4,
    pUpdateClients = 8, pFindClient = 16, pTransactions = 32, pManageUsers = 64
};

const string clientsFileName = "Clients.txt";
const string UsersFileName = "Users.txt";

stUser currentUser; // global variable

void showMainMenu();
void showTransactionsMenu();
void showManageUsersMenu();
bool checkAccessPermission(enMainMenuPermissions Permission);
void login();

struct sClient {
    string AccountNumber;
    string PinCode;
    string Name;
    string Phone;
    double AccountBalance;
    bool MarkForDelete = false;
};

vector<string> splitString(string S1, string Delim) {
    vector<string> vString;
    short pos = 0;
    string sWord;

    // use find() function to get the position of the delimiters  
    while ((pos = S1.find(Delim)) != std::string::npos) {
        sWord = S1.substr(0, pos); // store the word   
        if (sWord != "") {
            vString.push_back(sWord);
        }

        S1.erase(0, pos + Delim.length());  /* erase() until positon and move to next word. */
    }

    if (S1 != "") {
        vString.push_back(S1); // it adds last word of the string.
    }

    return vString;
}

stUser convertUserLineToRecord(string line, string seperator = "#//#") {

    stUser user;
    vector<string> vUserData;

    vUserData = splitString(line, seperator = "#//#");

    user.userName = vUserData[0];
    user.password = vUserData[1];
    user.permissions = stoi(vUserData[2]);
    return user;
}

sClient convertLinetoRecord(string Line, string seperator = "#//#") {
    sClient Client;
    vector<string> vClientData;
    vClientData = splitString(Line, seperator);

    Client.AccountNumber = vClientData[0];
    Client.PinCode = vClientData[1];
    Client.Name = vClientData[2];
    Client.Phone = vClientData[3];
    Client.AccountBalance = stod(vClientData[4]);//cast string to double
    return Client;
}

string convertRecordToLine(sClient Client, string seperator = "#//#") {

    string stClientRecord = "";
    stClientRecord += Client.AccountNumber + seperator;
    stClientRecord += Client.PinCode + seperator;
    stClientRecord += Client.Name + seperator;
    stClientRecord += Client.Phone + seperator;
    stClientRecord += to_string(Client.AccountBalance);
    return stClientRecord;
}

string convertUserRecordToLine(stUser user, string seperator = "#//#") {
    
    string stClientRecord = "";

    stClientRecord += user.userName + seperator;
    stClientRecord += user.password + seperator;
    stClientRecord += to_string(user.permissions);

    return stClientRecord;
}

bool clientExistsByAccountNumber(string AccountNumber, string FileName) {

    vector <sClient> vClients;
    fstream MyFile;
    MyFile.open(FileName, ios::in);//read Mode

    if (MyFile.is_open()) {
        string Line;
        sClient Client;

        while (getline(MyFile, Line)) {
            Client = convertLinetoRecord(Line);
            if (Client.AccountNumber == AccountNumber) {
                MyFile.close();
                return true;
            }
            vClients.push_back(Client);
        }

        MyFile.close();

    }
    return false;
}

bool userExistsByUsername(string username, string fileName) {

    fstream myFile;
    myFile.open(fileName, ios::in); //read mode

    if (myFile.is_open()) {
        string line;
        stUser user;

        while (getline(myFile, line)) {
            user = convertUserLineToRecord(line);
            if (user.userName == username) {
                myFile.close();
                return true;
            }
        }
        myFile.close();
    }
    return false;
}

sClient readNewClient() {
    sClient Client;
    cout << "Enter Account Number? ";

    // Usage of std::ws will extract all the whitespace character
    getline(cin >> ws, Client.AccountNumber);

    while (clientExistsByAccountNumber(Client.AccountNumber, clientsFileName))
    {
        cout << "\nClient with [" << Client.AccountNumber << "] already exists, Enter another Account Number? ";
        getline(cin >> ws, Client.AccountNumber);
    }

    cout << "Enter PinCode: ";
    getline(cin, Client.PinCode);

    cout << "Enter Name: ";
    getline(cin, Client.Name);

    cout << "Enter Phone Number: ";
    getline(cin, Client.Phone);

    cout << "Enter AccountBalance: ";
    cin >> Client.AccountBalance;

    return Client;
}

int readPermissionsToSet() {
    
    int permissions = 0;
    char answer = 'n';

    cout << "\nDo you want to give full access? y/n? ";
    cin >> answer;
    if (answer == 'Y' || answer == 'y') {
        return -1;
    }

    cout << "\nDo you want to give access to: \n";
    cout << "\nShow client list? y/n? ";
    cin >> answer;
    if (answer == 'Y' || answer == 'y') {
        permissions += enMainMenuPermissions::pListClients;
    }

    cout << "\nAdd new client? y/n? ";
    cin >> answer;
    if (answer == 'Y' || answer == 'y') {
        permissions += enMainMenuPermissions::pAddNewClient;
    }

    cout << "\nDelete client? y/n? ";
    cin >> answer;
    if (answer == 'Y' || answer == 'y') {
        permissions += enMainMenuPermissions::pDeleteClient;
    }

    cout << "\nUpdate client? y/n? ";
    cin >> answer;
    if (answer == 'Y' || answer == 'y') {
        permissions += enMainMenuPermissions::pUpdateClients;
    }

    cout << "\nFind client? y/n? ";
    cin >> answer;
    if (answer == 'Y' || answer == 'y') {
        permissions += enMainMenuPermissions::pFindClient;
    }

    cout << "\nTransactions? y/n? ";
    cin >> answer;
    if (answer == 'Y' || answer == 'y') {
        permissions += enMainMenuPermissions::pTransactions;
    }

    cout << "\nManage users? y/n? ";
    cin >> answer;
    if (answer == 'Y' || answer == 'y') {
        permissions += enMainMenuPermissions::pManageUsers;
    }

    return permissions;
}

stUser readNewUser() {
    
    stUser user;

    getline(cin >> ws, user.userName);

    while (userExistsByUsername(user.userName, UsersFileName)) {
        cout << "\nUser with [" << user.userName << "] already exists, enter another user name. \n";
        getline(cin >> ws, user.userName);
    }

    cout << "Enter Password? ";
    getline(cin, user.password);

    user.permissions = readPermissionsToSet();
    return user;
}

vector <stUser> loadUsersDataFromFile(string fileName) {
    
    vector <stUser> vUsers;

    fstream myFile;
    myFile.open(fileName, ios::in); //read mode

    if (myFile.is_open()) {
        string line;
        stUser user;

        while (getline(myFile, line)) {
            
            user = convertUserLineToRecord(line);
            vUsers.push_back(user);
        }
        myFile.close();
    }

    return vUsers;
}

vector <sClient> loadClientsDataFromFile(string fileName) {
    vector <sClient> vClients;
    fstream myFile;
    myFile.open(fileName, ios::in);//read Mode

    if (myFile.is_open()) {
        string line;
        sClient client;

        while (getline(myFile, line))
        {
            client = convertLinetoRecord(line);
            vClients.push_back(client);
        }
        myFile.close();
    }
    return vClients;
}

void printClientRecordLine(sClient client) {
    cout << "| " << setw(15) << left << client.AccountNumber;
    cout << "| " << setw(10) << left << client.PinCode;
    cout << "| " << setw(40) << left << client.Name;
    cout << "| " << setw(12) << left << client.Phone;
    cout << "| " << setw(12) << left << client.AccountBalance;
}

void printUserRecordLine(stUser user) {
    cout << "| " << setw(15) << left << user.userName;
    cout << "| " << setw(10) << left << user.password;
    cout << "| " << setw(40) << left << user.permissions;
}

void printClientRecordBalanceLine(sClient client) {
    cout << "| " << setw(15) << left << client.AccountNumber;
    cout << "| " << setw(40) << left << client.Name;
    cout << "| " << setw(12) << left << client.AccountBalance;
}

void showAllClientsScreen() {
    vector <sClient> vClients = loadClientsDataFromFile(clientsFileName);

    cout << "\n\t\t\t\t\tClient List (" << vClients.size() << ") Client(s).";
    cout << "\n_______________________________________________________";
    cout << "_________________________________________\n" << endl;

    cout << "| " << left << setw(15) << "Account Number";
    cout << "| " << left << setw(10) << "Pin Code";
    cout << "| " << left << setw(40) << "Client Name";
    cout << "| " << left << setw(12) << "Phone";
    cout << "| " << left << setw(12) << "Balance";
    cout << "\n_______________________________________________________";
    cout << "_________________________________________\n" << endl;

    if (vClients.size() == 0)
        cout << "\t\t\t\tNo Clients Available In the System!";
    else

        for (sClient client : vClients)
        {

            printClientRecordLine(client);
            cout << endl;
        }

    cout << "\n_______________________________________________________";
    cout << "_________________________________________\n" << endl;
}

void showAllUsersScreen() {

    vector<stUser> vUsers = loadUsersDataFromFile(UsersFileName);

    cout << "\n\t\t\t\t\tUsers List (" << vUsers.size() << ") User(s).";
    cout << "\n_______________________________________________________";
    cout << "_________________________________________\n" << endl;

    cout << "| " << left << setw(15) << "User Name";
    cout << "| " << left << setw(10) << "Password";
    cout << "| " << left << setw(40) << "Permissions";
    cout << "\n_______________________________________________________";
    cout << "_________________________________________\n" << endl;

    if (vUsers.size() == 0)
        cout << "\t\t\t\tNo Users Available In the System!";
    else

        for (stUser User : vUsers) {
            printUserRecordLine(User);
            cout << endl;
        }

    cout << "\n_______________________________________________________";
    cout << "_________________________________________\n" << endl;
}

void showTotalBalances() {
    vector <sClient> vClients = loadClientsDataFromFile(clientsFileName);

    cout << "\n\t\t\t\t\tBalances List (" << vClients.size() << ") Client(s).";
    cout << "\n_______________________________________________________";
    cout << "_________________________________________\n" << endl;

    cout << "| " << left << setw(15) << "Account Number";
    cout << "| " << left << setw(40) << "Client Name";
    cout << "| " << left << setw(12) << "Balance";
    cout << "\n_______________________________________________________";
    cout << "_________________________________________\n" << endl;

    double totalBalances = 0;

    if (vClients.size() == 0)
        cout << "\t\t\t\tNo Clients Available In the System!";
    else

        for (sClient client : vClients)
        {

            printClientRecordBalanceLine(client);
            totalBalances += client.AccountBalance;

            cout << endl;
        }

    cout << "\n_______________________________________________________";
    cout << "_________________________________________\n" << endl;
    cout << "\t\t\t\t\t   Total Balances = " << totalBalances;
}

void printClientCard(sClient client) {
    cout << "\nThe following are the client details:\n";
    cout << "-----------------------------------";
    cout << "\nAccout Number: " << client.AccountNumber;
    cout << "\nPin Code     : " << client.PinCode;
    cout << "\nName         : " << client.Name;
    cout << "\nPhone        : " << client.Phone;
    cout << "\nAccount Balance: " << client.AccountBalance;
    cout << "\n-----------------------------------\n";
}

void printUserCard(stUser user) {
    cout << "\nThe following are the user details:\n";
    cout << "-----------------------------------";
    cout << "\nUsername    : " << user.userName;
    cout << "\nPassword    : " << user.password;
    cout << "\nPermissions : " << user.permissions;
    cout << "\n-----------------------------------\n";
}

bool findClientByAccountNumber(string accountNumber, vector <sClient> vClients, sClient& client) {
    for (sClient C : vClients) {

        if (C.AccountNumber == accountNumber) {
            client = C;
            return true;
        }

    }
    return false;
}

bool findUserByUserName(string username, vector <stUser> vUsers, stUser& user) {

}

bool findUserByUserNameAndPassword(string username, string password, stUser& user) {

    vector <stUser> vUsers = loadUsersDataFromFile(UsersFileName);

    for (stUser U : vUsers) {
        if (U.userName == username && U.password == password) {
            user = U;
            return true;
        }
    }
    return false;
}

sClient changeClientRecord(string accountNumber) {
    sClient client;

    client.AccountNumber = accountNumber;

    cout << "\n\nEnter PinCode? ";
    getline(cin >> ws, client.PinCode);

    cout << "Enter Name? ";
    getline(cin, client.Name);

    cout << "Enter Phone? ";
    getline(cin, client.Phone);

    cout << "Enter AccountBalance? ";
    cin >> client.AccountBalance;
    return client;
}

bool markClientForDeleteByAccountNumber(string accountNumber, vector <sClient>& vClients) {

    for (sClient& C : vClients) {

        if (C.AccountNumber == accountNumber) {
            C.MarkForDelete = true;
            return true;
        }
    }

    return false;
}

bool markUserForDeleteByUsername(string username, vector <stUser>& vUsers) {
    
    for (stUser& U : vUsers) {
        if (U.userName == username) {
            U.markForDelete = true;
            return true;
        }
    }
    return false;
}

vector <sClient> saveClientsDataToFile(string fileName, vector <sClient> vClients) {
    fstream myFile;
    myFile.open(fileName, ios::out);//overwrite

    string DataLine;

    if (myFile.is_open()) {
        for (sClient C : vClients) {

            if (C.MarkForDelete == false) {
                //we only write records that are not marked for delete.  
                DataLine = convertRecordToLine(C);
                myFile << DataLine << endl;
            }
        }
        myFile.close();
    }

    return vClients;
}

vector <stUser> saveUsersDataToFile(string fileName, vector <stUser> vUsers) {
    fstream MyFile;
    MyFile.open(fileName, ios::out);//overwrite

    string dataLine;

    if (MyFile.is_open()) {
        for (stUser U : vUsers) {
            if (U.markForDelete == false) {
                //we only write records that are not marked for delete.  
                dataLine = convertUserRecordToLine(U);
                MyFile << dataLine << endl;
            }
        }
        MyFile.close();
    }
    return vUsers;
}

void addDataLineToFile(string fileName, string  stDataLine) {
    fstream MyFile;
    MyFile.open(fileName, ios::out | ios::app);

    if (MyFile.is_open()) {
        MyFile << stDataLine << endl;
        MyFile.close();
    }
}

void addNewClient() {
    sClient Client;
    Client = readNewClient();
    addDataLineToFile(clientsFileName, convertRecordToLine(Client));
}

void addNewUser() {
    stUser user;
    user = readNewUser();
    addDataLineToFile(UsersFileName, convertUserRecordToLine(user));
}

void addNewClients() {
    char AddMore = 'Y';
    do {
        //system("cls");
        cout << "Adding New Client:\n\n";

        addNewClient();
        cout << "\nClient Added Successfully, do you want to add more clients? Y/N? ";
        cin >> AddMore;

    } while (toupper(AddMore) == 'Y');
}

void addNewUsers() {
    char addMore = 'Y';

    do {
        cout << "Adding new user:\n\n";

        addNewUser();
        cout << "User added sucessfully, do you want to add more users? Y/N? ";
        cin >> addMore;
    } while (toupper(addMore) == 'Y');
}

bool deleteClientByAccountNumber(string AccountNumber, vector <sClient>& vClients) {
    sClient Client;
    char Answer = 'n';

    if (findClientByAccountNumber(AccountNumber, vClients, Client)) {

        printClientCard(Client);

        cout << "\n\nAre you sure you want delete this client? y/n ? ";
        cin >> Answer;
        if (Answer == 'y' || Answer == 'Y') {
            markClientForDeleteByAccountNumber(AccountNumber, vClients);
            saveClientsDataToFile(clientsFileName, vClients);

            //Refresh Clients 
            vClients = loadClientsDataFromFile(clientsFileName);

            cout << "\n\nClient Deleted Successfully.";
            return true;
        }
    }
    else {
        cout << "\nClient with Account Number (" << AccountNumber << ") is Not Found!";
        return false;
    }
}

bool deleteUserByUserName(string username, vector<stUser>& vUsers) {
    
    if (username == "Admin") {
        cout << "\n\nYou cannot delete this user.";
        return false;
    }

    stUser user;
    char answer = 'n';

    if (findUserByUserName(username, vUsers, user)) {
        printUserCard(user);

        cout << "\n\nAre you sure you want delete this User? y/n ? ";
        cin >> answer;
        if (answer == 'y' || answer == 'Y') {
            markUserForDeleteByUsername(username, vUsers);
            saveUsersDataToFile(UsersFileName, vUsers);

            //refresh clients
            vUsers = loadUsersDataFromFile(UsersFileName);

            cout << "\n\nUser deleted successfully.";
            return true;
        }
    }
    else {
        cout << "\nUser with Username (" << username << ") is Not Found!";
        return false;
    }
}

bool updateClientByAccountNumber(string AccountNumber, vector <sClient>& vClients) {

    sClient Client;
    char Answer = 'n';

    if (findClientByAccountNumber(AccountNumber, vClients, Client)) {

        printClientCard(Client);
        cout << "\n\nAre you sure you want update this client? y/n ? ";
        cin >> Answer;
        if (Answer == 'y' || Answer == 'Y') {
            for (sClient& C : vClients) {
                if (C.AccountNumber == AccountNumber) {
                    C = changeClientRecord(AccountNumber);
                    break;
                }
            }

            saveClientsDataToFile(clientsFileName, vClients);

            cout << "\n\nClient Updated Successfully.";
            return true;
        }

    }
    else {
        cout << "\nClient with Account Number (" << AccountNumber << ") is Not Found!";
        return false;
    }
}

bool depositBalanceToClientByAccountNumber(string accountNumber, double amount, vector<sClient>& vClients) {

    char answer = 'n';

    cout << "\n\nAre you sure you want to perform this transaction? y/n? ";
    cin >> answer;
    if (answer == 'y' || answer == 'Y') {
        for (sClient& c : vClients) {
            if (c.AccountNumber == accountNumber) {
                c.AccountBalance += amount;
                saveClientsDataToFile(clientsFileName, vClients);
                cout << "\n\nDone Succcesfully, new balance is: " << c.AccountBalance;

                return true;
            }
        }
    }

    return false;
}

string readClientAccountNumber() {
    string AccountNumber = "";

    cout << "\nPlease enter AccountNumber? ";
    cin >> AccountNumber;
    return AccountNumber;
}

string readUserName() {
    
    string username = "";
    
    cout << "\nPlease enter username? ";
    cin >> username;
    return username;
}

void showListUsersScreen() {
    showAllUsersScreen();
}

void showAddNewUserScreen() {
    cout << "\n-----------------------------------\n";
    cout << "\tAdd New User Screen";
    cout << "\n-----------------------------------\n";

    addNewUsers();
}

void showDeleteUserScreen() {
    cout << "\n-----------------------------------\n";
    cout << "\tDelete Users Screen";
    cout << "\n-----------------------------------\n";

    vector<stUser> vUsers = loadUsersDataFromFile(UsersFileName);

    string username = readUserName();
    deleteUserByUserName(username, vUsers);
}

void showDeleteClientScreen() {
    cout << "\n-----------------------------------\n";
    cout << "\tDelete Client Screen";
    cout << "\n-----------------------------------\n";

    vector <sClient> vClients = loadClientsDataFromFile(clientsFileName);
    string AccountNumber = readClientAccountNumber();
    deleteClientByAccountNumber(AccountNumber, vClients);
}

void showUpdateClientScreen() {
    cout << "\n-----------------------------------\n";
    cout << "\tUpdate Client Info Screen";
    cout << "\n-----------------------------------\n";

    vector <sClient> vClients = loadClientsDataFromFile(clientsFileName);
    string AccountNumber = readClientAccountNumber();
    updateClientByAccountNumber(AccountNumber, vClients);
}

void showAddNewClientsScreen() {
    cout << "\n-----------------------------------\n";
    cout << "\tAdd New Clients Screen";
    cout << "\n-----------------------------------\n";

    addNewClients();
}

void showFindClientScreen() {
    cout << "\n-----------------------------------\n";
    cout << "\tFind Client Screen";
    cout << "\n-----------------------------------\n";

    vector <sClient> vClients = loadClientsDataFromFile(clientsFileName);
    sClient Client;
    string AccountNumber = readClientAccountNumber();
    if (findClientByAccountNumber(AccountNumber, vClients, Client))
        printClientCard(Client);
    else
        cout << "\nClient with Account Number[" << AccountNumber << "] is not found!";
}

void showEndScreen() {
    cout << "\n-----------------------------------\n";
    cout << "\tProgram Ends.";
    cout << "\n-----------------------------------\n";
}

void showDepositScreen() {
    cout << "\n-----------------------------------\n";
    cout << "\tDeposit Screen";
    cout << "\n-----------------------------------\n";

    sClient client;

    vector <sClient> vClients = loadClientsDataFromFile(clientsFileName);
    string accountNumber = readClientAccountNumber();

    while (!findClientByAccountNumber(accountNumber, vClients, client)) {
        cout << "\nClient with[" << accountNumber << "] does not exist.\n";
        accountNumber = readClientAccountNumber();
    }

    printClientCard(client);

    double amount = 0;
    cout << "\nPlease enter deposit amount: ";
    cin >> amount;

    depositBalanceToClientByAccountNumber(accountNumber, amount, vClients);
}

void showWithdrawScreen() {
    cout << "\n-----------------------------------\n";
    cout << "\tWithdraw Screen";
    cout << "\n-----------------------------------\n";

    sClient client;

    vector <sClient> vClients = loadClientsDataFromFile(clientsFileName);
    string accountNumber = readClientAccountNumber();

    while (!findClientByAccountNumber(accountNumber, vClients, client)) {
        cout << "\nClient with[" << accountNumber << "] does not exist.\n";
        accountNumber = readClientAccountNumber();
    }

    printClientCard(client);

    double amount = 0;
    cout << "\nPlease enter withdraw amount: ";
    cin >> amount;

    //validate amount does not exceeds balance
    while (amount > client.AccountBalance) {
        ;
        cout << "\nAmount Exceeds the balance, you can withdraw up to :" << client.AccountBalance << endl;
        cout << "Please enter another amount : ";
        cin >> amount;
    }

    depositBalanceToClientByAccountNumber(accountNumber, amount * -1, vClients); // deposit in minus
}

void showTotalBalancesScreen() {
    showTotalBalances();
}

void goBackToMainMenu() {
    cout << "\n\nPress any key to go back to Main Menu...";
    system("pause>0");
    showMainMenu();
}

void goBackToTransactionsMenu() {
    cout << "\n\nPress any key to go back to Transactions Menu...";
    system("pause>0");
    showTransactionsMenu();
}

short readMainMenuOption() {
    cout << "Choose what do you want to do? [1 to 8]? ";
    short Choice = 0;
    cin >> Choice;

    return Choice;
}

void performManageUsersMenuOption(enManageUsersMenuOptions manageUsersMenuOption) {
    switch (manageUsersMenuOption) {
    
    case enManageUsersMenuOptions::eListUsers: {
        system("cls");
        showListUsersScreen();
        goBackToManageUsersMenu();
        break;
    }
    
    case enManageUsersMenuOptions::eAddNewUser: {
        system("cls");
        showAddNewUserScreen();
        goBackToManageUsersMenu();
        break;
    }

    case enManageUsersMenuOptions::eDeleteUser: {
        system("cls");
        showDeleteUserScreen();
        goBackToManageUsersMenu();
        break;
    }

    case enManageUsersMenuOptions::eUpdateUser: {
        system("cls");
        showUpdateUserScreen();
        goBackToManageUsersMenu();
        break;
    }

    case enManageUsersMenuOptions::eFindUser: {
        system("cls");
        showFindUserScreen();
        goBackToManageUsersMenu();
        break;
    }

    case enManageUsersMenuOptions::eMainMenu: {
        showMainMenu();
    }
    }
}

void goBackToManageUsersMenu() {
    cout << "\n\nPress any key to go back to Transactions Menue...";
    system("pause>0");
    showManageUsersMenu();
}

short readTransactionsMenuOption() {
    cout << "Choose what do you want to do? [1 to 4]? ";
    short Choice = 0;
    cin >> Choice;

    return Choice;
}

short readManageUsersOption() {
    cout << "Choose what do you want to do? [1 to 6]? ";
    short Choice = 0;
    cin >> Choice;

    return Choice;
}

void showManageUsersMenu() {
    
    if (!checkAccessPermission(enMainMenuPermissions::pManageUsers)) {
        showAccessDeniedMessage();
        goBackToMainMenu();
        return;
    }

    system("cls");
    cout << "===========================================\n";
    cout << "\t\tManage Users Menue Screen\n";
    cout << "===========================================\n";
    cout << "\t[1] List Users.\n";
    cout << "\t[2] Add New User.\n";
    cout << "\t[3] Delete User.\n";
    cout << "\t[4] Update User.\n";
    cout << "\t[5] Find User.\n";
    cout << "\t[6] Main Menu.\n";
    cout << "===========================================\n";

    performManageUsersMenuOption((enManageUsersMenuOptions)readManageUsersOption());
}

void performTransactionMenuOptions(enTransactionsMenuOptions transactionsMenuOption) {
    cout << "\n-----------------------------------\n";
    cout << "\tTransactions Menu Screen";
    cout << "\n-----------------------------------\n";

    switch (transactionsMenuOption) {

    case enTransactionsMenuOptions::eDeposit:
        system("cls");
        showDepositScreen();
        goBackToTransactionsMenu();
        break;

    case enTransactionsMenuOptions::eWithDraw:
        system("cls");
        showWithdrawScreen();
        goBackToTransactionsMenu();
        break;

    case enTransactionsMenuOptions::eTotalBalance:
        system("cls");
        showTotalBalancesScreen();
        goBackToTransactionsMenu();
        break;

    case enTransactionsMenuOptions::eShowMainMenu:
        showMainMenu();
    }
}

void performMainMenueOption(enMainMenuOptions MainMenuOption) {
    switch (MainMenuOption) {

    case enMainMenuOptions::eListClients:

        system("cls");
        showAllClientsScreen();
        goBackToMainMenu();
        break;

    case enMainMenuOptions::eAddNewClient:
        system("cls");
        showAddNewClientsScreen();
        goBackToMainMenu();
        break;

    case enMainMenuOptions::eDeleteClient:
        system("cls");
        showDeleteClientScreen();
        goBackToMainMenu();
        break;

    case enMainMenuOptions::eUpdateClient:
        system("cls");
        showUpdateClientScreen();
        goBackToMainMenu();
        break;

    case enMainMenuOptions::eFindClient:
        system("cls");
        showFindClientScreen();
        goBackToMainMenu();
        break;

    case enMainMenuOptions::eTransactions:
        system("cls");
        showTransactionsMenu();
        break;

    case enMainMenuOptions::eManageUsers:
        system("cls");
        showManageUsersMenu();
        break;

    case enMainMenuOptions::eLogout:
        system("cls");
        //showEndScreen();
        login();
        break;
    }
}

void showTransactionsMenu() {
    system("cls");
    cout << "===========================================\n";
    cout << "\tTransactions Menu Screen\n";
    cout << "===========================================\n";
    cout << "\t[1] Deposit.\n";
    cout << "\t[2] Withdraw.\n";
    cout << "\t[3] Total Balances.\n";
    cout << "\t[4] Main Menu.\n";
    cout << "===========================================\n";
    performTransactionMenuOptions((enTransactionsMenuOptions)readTransactionsMenuOption());
}

void showMainMenu() {
    system("cls");
    cout << "===========================================\n";
    cout << "\t\tMain Menu Screen\n";
    cout << "===========================================\n";
    cout << "\t[1] Show Client List.\n";
    cout << "\t[2] Add New Client.\n";
    cout << "\t[3] Delete Client.\n";
    cout << "\t[4] Update Client Info.\n";
    cout << "\t[5] Find Client.\n";
    cout << "\t[6] Transactions.\n";
    cout << "\t[7] Manage Users.\n";
    cout << "\t[8] Logout.\n";
    cout << "===========================================\n";
    performMainMenueOption((enMainMenuOptions)readMainMenuOption());
}

bool loadUserInfo(string username, string password) {
    if (findUserByUserNameAndPassword(username, password, currentUser)) {
        return true;
    }
    else {
        return false;
    }
}

void login() {

    bool loginFailed = false;

    string username, password;

    do {
    
        system("cls");

        cout << "\n---------------------------------\n";
        cout << "\tLogin Screen";
        cout << "\n---------------------------------\n";

        if (loginFailed) {
            cout << "Invalid Username/Password!\n";
        }

        cout << "Enter username: ";
        cin >> username;

        cout << "Enter password: ";
        cin >> password;

        loginFailed = !loadUserInfo(username, password);
    
    } while (loginFailed);

    showMainMenu();
}

int main() {
    
    login();
    system("pause>0");
    return 0;
}