#pragma once

#include "clsInputValidate.h"
#include "clsPerson.h"
#include "clsString.h"
#include "clsUtil.h"

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>

using namespace std;

class clsBankClient : public clsPerson
{
private:

    enum enMode { EmptyMode = 0, UpdateMode = 1, AddNewMode = 2 };

    enMode _Mode;

    string _AccountNumber;
    string _PinCode;
    float _AccountBalance;
    bool _MarkedForDelete = false;


    static clsBankClient _ConvertLinetoClientObject(string Line, string Seperator = "#//#")
    {
        vector<string> vClientData;
        vClientData = clsString::Split(Line, Seperator);


        if (vClientData.size() < 7)
            {
                return _GetEmptyClientObject();
            }


        return clsBankClient(enMode::UpdateMode,
                             vClientData[0],
                             vClientData[1],
                             vClientData[2],
                             vClientData[3],
                             vClientData[4],
                             clsUtil::DecryptText(vClientData[5]),
                             stod(vClientData[6]));
    }

    static string _ConverClientObjectToLine(clsBankClient Client, string Seperator = "#//#")
    {
        string stClientRecord = "";
        stClientRecord += Client.GetFirstName() + Seperator;
        stClientRecord += Client.GetLastName() + Seperator;
        stClientRecord += Client.GetEmail() + Seperator;
        stClientRecord += Client.GetPhone() + Seperator;
        stClientRecord += Client.AccountNumber() + Seperator;
        stClientRecord += clsUtil::EncryptText(Client.GetPinCode()) + Seperator;
        stClientRecord += to_string(Client.GetAccountBalance());

        return stClientRecord;
    }

    static vector <clsBankClient> _LoadClientsDataFromFile()
    {
        vector <clsBankClient> vClients;
        fstream MyFile;
        MyFile.open("Clients.txt", ios::in);

        if (MyFile.is_open())
            {
                string Line;
                while (getline(MyFile, Line))
                    {
                        clsBankClient Client = _ConvertLinetoClientObject(Line);


                        if (!Client.IsEmpty())
                            {
                                vClients.push_back(Client);
                            }
                    }
                MyFile.close();
            }
        return vClients;
    }

    static void _SaveCleintsDataToFile(vector <clsBankClient> vClients)
    {
        fstream MyFile;
        MyFile.open("Clients.txt", ios::out);

        string DataLine;

        if (MyFile.is_open())
            {
                for (clsBankClient C : vClients)
                    {
                        if(C._MarkedForDelete == false)
                            {
                                DataLine = _ConverClientObjectToLine(C);
                                MyFile << DataLine << endl;
                            }

                    }
                MyFile.close();
            }
    }

    void _Update()
    {
        vector <clsBankClient> _vClients = _LoadClientsDataFromFile();
        for (clsBankClient& C : _vClients)
            {
                if (C.AccountNumber() == AccountNumber())
                    {
                        C = *this;
                        break;
                    }
            }
        _SaveCleintsDataToFile(_vClients);
    }

    static clsBankClient _GetEmptyClientObject()
    {
        return clsBankClient(enMode::EmptyMode, "", "", "", "", "", "", 0);
    }

    void _AddNew()
    {

        _AddDataLineToFile(_ConverClientObjectToLine(*this));
    }

    void _AddDataLineToFile(string  stDataLine)
    {
        fstream MyFile;
        MyFile.open("Clients.txt", ios::out | ios::app);

        if (MyFile.is_open())
            {

                MyFile << stDataLine << endl;

                MyFile.close();
            }

    }


public:

    struct stTransferLogRecord
    {
        string DateTime;
        string SourceAccountNumber;
        string DestinationAccountNumber;
        float Amount;
        float srcBalanceAfter;
        float destBalanceAfter;
        string UserName;
    };

    clsBankClient(enMode Mode, string FirstName, string LastName,
                  string Email, string Phone, string AccountNumber, string PinCode,
                  float AccountBalance) :
        clsPerson(FirstName, LastName, Email, Phone)
    {
        _Mode = Mode;
        _AccountNumber = AccountNumber;
        _PinCode = PinCode;
        _AccountBalance = AccountBalance;
    }

    bool IsEmpty()
    {
        return (_Mode == enMode::EmptyMode);
    }
    string AccountNumber()
    {
        return _AccountNumber;
    }

    void SetPinCode(string PinCode)
    {
        _PinCode = PinCode;
    }
    string GetPinCode()
    {
        return _PinCode;
    }

    void SetAccountBalance(float AccountBalance)
    {
        _AccountBalance = AccountBalance;
    }
    float GetAccountBalance()
    {
        return _AccountBalance;
    }

    void Print()
    {
        cout << "\nClient Card:";
        cout << "\n___________________";
        cout << "\nFirstName      : " << GetFirstName();
        cout << "\nLastName       : " << GetLastName();
        cout << "\nFull Name      : " << FullName();
        cout << "\nEmail          : " << GetEmail();
        cout << "\nPhone          : " << GetPhone();
        cout << "\nAcc. Number    : " << _AccountNumber;
        cout << "\nPin Code       : " << _PinCode;
        cout << "\nBalance        : " << _AccountBalance;
        cout << "\n___________________\n";
    }

    static clsBankClient Find(string AccountNumber)
    {
        fstream MyFile;
        MyFile.open("Clients.txt", ios::in);
        if (MyFile.is_open())
            {
                string Line;
                while (getline(MyFile, Line))
                    {
                        clsBankClient Client = _ConvertLinetoClientObject(Line);
                        if (Client.AccountNumber() == AccountNumber)
                            {
                                MyFile.close();
                                return Client;
                            }
                    }
                MyFile.close();
            }
        return _GetEmptyClientObject();
    }

    enum enSaveResults { svFaildEmptyObject = 0, svSucceeded = 1, svFaildAccountNumberExists = 2};

    enSaveResults Save()
    {

        switch (_Mode)
            {
            case enMode::EmptyMode:
            {
                if (IsEmpty())
                    {

                        return enSaveResults::svFaildEmptyObject;

                    }

            }

            case enMode::UpdateMode:
            {


                _Update();

                return enSaveResults::svSucceeded;

                break;
            }

            case enMode::AddNewMode:
            {
                //This will add new record to file or database
                if (clsBankClient::IsClientExist(_AccountNumber))
                    {
                        return enSaveResults::svFaildAccountNumberExists;
                    }
                else
                    {
                        _AddNew();

                        //We need to set the mode to update after add new
                        _Mode = enMode::UpdateMode;
                        return enSaveResults::svSucceeded;
                    }

                break;
            }
            }

    }
    static bool IsClientExist(string AccountNumber)
    {
        clsBankClient Client1 = clsBankClient::Find(AccountNumber);
        return (!Client1.IsEmpty());
    }


    static  void ReadClientInfo(clsBankClient& Client)
    {
        cout << "\nEnter FirstName: ";
        Client.SetFirstName(clsInputValidate::ReadString());

        cout << "\nEnter LastName: ";
        Client.SetLastName(clsInputValidate::ReadString());

        cout << "\nEnter Email: ";
        Client.SetEmail(clsInputValidate::ReadString());

        cout << "\nEnter Phone: ";
        Client.SetPhone(clsInputValidate::ReadString());

        cout << "\nEnter PinCode: ";
        Client.SetPinCode(clsInputValidate::ReadString());

        cout << "\nEnter Account Balance: ";
        Client.SetAccountBalance(clsInputValidate::ReadFloatNumber());
    }

    static void UpdateClient()
    {
        string AccountNumber = "";

        cout << "\nPlease Enter client Account Number: ";
        AccountNumber = clsInputValidate::ReadString();

        while (!clsBankClient::IsClientExist(AccountNumber))
            {
                cout << "\nAccount number is not found, choose another one: ";
                AccountNumber = clsInputValidate::ReadString();
            }

        clsBankClient Client1 = clsBankClient::Find(AccountNumber);
        Client1.Print();

        cout << "\n\nUpdate Client Info:";
        cout << "\n____________________\n";

        ReadClientInfo(Client1);

        clsBankClient::enSaveResults SaveResult;
        SaveResult = Client1.Save();

        switch (SaveResult)
            {
            case clsBankClient::enSaveResults::svSucceeded:
                cout << "\nAccount Updated Successfully :-)\n";
                Client1.Print();
                break;
            case clsBankClient::enSaveResults::svFaildEmptyObject:
                cout << "\nError account was not saved because it's Empty";
                break;
            }
    }


    static clsBankClient GetAddNewClientObject(string AccountNumber)
    {
        return clsBankClient(enMode::AddNewMode, "", "", "", "", AccountNumber, "", 0);
    }






    static void AddNewClient()
    {
        string AccountNumber = "";

        cout << "\nPlease Enter Account Number: ";

        AccountNumber = clsInputValidate::ReadString();

        while (clsBankClient::IsClientExist(AccountNumber))
            {
                cout << "\nAccount Number Is Already Used, Choose another one: ";
                AccountNumber = clsInputValidate::ReadString();
            }

        clsBankClient NewClient = clsBankClient::GetAddNewClientObject(AccountNumber);


        ReadClientInfo(NewClient);

        cout << endl;

        clsBankClient::enSaveResults SaveResult;

        SaveResult = NewClient.Save();

        switch (SaveResult)
            {
            case  clsBankClient::enSaveResults::svSucceeded:
            {
                cout << "\nAccount Addeded Successfully :-)\n";
                NewClient.Print();
                break;
            }
            case clsBankClient::enSaveResults::svFaildEmptyObject:
            {
                cout << "\nError account was not saved because it's Empty";
                break;

            }
            case clsBankClient::enSaveResults::svFaildAccountNumberExists:
            {
                cout << "\nError account was not saved because account number is used!\n";
                break;

            }
            }
    }



    bool Delete()
    {
        vector <clsBankClient> _vClients;
        _vClients = _LoadClientsDataFromFile();

        for(clsBankClient & c : _vClients)
            {
                if(c.AccountNumber() == _AccountNumber)
                    {
                        c._MarkedForDelete = true;
                        break;
                    }
            }
        _SaveCleintsDataToFile(_vClients);
        *this = _GetEmptyClientObject();
        return true;
    }

    static void DeleteClient()
    {
        string AccountNumber = "";

        cout << "\nPlease Enter Account Number: ";

        AccountNumber = clsInputValidate::ReadString();

        while(!clsBankClient::IsClientExist(AccountNumber))
            {
                cout << "\nAccount number is not found, try another one: ";
                AccountNumber = clsInputValidate::ReadString();
            }

        clsBankClient client = clsBankClient::Find(AccountNumber);
        client.Print();

        cout << "\nAre you sure you want to delete this client y/n?";

        char Answer = 'n';
        cin >> Answer;

        if(Answer == 'y' || Answer == 'Y')
            {
                if(client.Delete())
                    {
                        cout << "\nClient Deleted successfully!\n";
                        client.Print();
                    }
                else
                    {
                        cout << "\nError Client Was Not Deleted\n";
                    }
            }

    }

    static vector <clsBankClient> GetClientsList()
    {
        return _LoadClientsDataFromFile();
    }

//----------------------------------------------------

    static stTransferLogRecord _ConvertLinetoRecordObject(string Line, string Seperator = "#//#")
    {
        stTransferLogRecord Record;
        vector<string> vData = clsString::Split(Line, Seperator);


        Record.DateTime = vData[0];
        Record.SourceAccountNumber = vData[1];
        Record.DestinationAccountNumber = vData[2];
        Record.Amount = stof(vData[3]);
        Record.srcBalanceAfter = stof(vData[4]);
        Record.destBalanceAfter = stof(vData[5]);
        Record.UserName = vData[6];

        return Record;
    }

    static vector <stTransferLogRecord> _LoadTransferLogDataFromFile()
    {
        vector <stTransferLogRecord> vRecords;

        fstream MyFile;
        MyFile.open("TransferLog.txt", ios::in);

        if (MyFile.is_open())
            {
                string Line;
                stTransferLogRecord TransferRecord;

                while (getline(MyFile, Line))
                    {
                        TransferRecord = _ConvertLinetoRecordObject(Line);

                        vRecords.push_back(TransferRecord);

                    }
                MyFile.close();
            }
        return vRecords;
    }



    static vector <stTransferLogRecord> GetTransferLog()
    {
        return _LoadTransferLogDataFromFile();
    }


//----------------------------------------------------

    static void PrintClientRecordLine( clsBankClient Client)
    {
        cout << "| " << left << setw(15) << Client.AccountNumber();
        cout << "| " << left << setw(20) << Client.FullName();
        cout << "| " << left << setw(12) << Client.GetPhone();
        cout << "| " << left << setw(20) << Client.GetEmail();
        cout << "| " << left << setw(10) << Client.GetPinCode();
        cout << "| " << left << setw(12) << Client.GetAccountBalance();
    }

    static void ShowClientList()
    {
        vector <clsBankClient> vClients = clsBankClient::GetClientsList();

        cout << "\n\t\t\t\t\tClients List (" << vClients.size() << ") Clients\n";
        cout << "\n--------------------------------------------------------------------------------------------------------------------\n" << endl;
        cout << "| " << left << setw(15) << "Account Number"  ;
        cout << "| " << left << setw(20) << "Client Name"     ;
        cout << "| " << left << setw(12) << "Phone"           ;
        cout << "| " << left << setw(20) << "Email"           ;
        cout << "| " << left << setw(10) << "Pin Code"        ;
        cout << "| " << left << setw(12) << "Balance"         ;
        cout << "\n--------------------------------------------------------------------------------------------------------------------\n" << endl;

        if(vClients.empty())
            {
                cout << "\t\t\t\t\tNo Clients Exist";
            }
        else
            {

                for(const clsBankClient client : vClients)
                    {
                        PrintClientRecordLine(client);
                        cout << endl;
                    }
            }
        cout << "\n--------------------------------------------------------------------------------------------------------------------\n" << endl;
    }

    static double GetTotalBalances()
    {
        vector <clsBankClient> vClients = clsBankClient::GetClientsList();

        double TotalBalances = 0;

        for(clsBankClient Client : vClients)
            {
                TotalBalances += Client.GetAccountBalance();

            }
        return TotalBalances;
    }


    static void PrintClientRecordBalanceLine( clsBankClient Client)
    {
        cout << "| " << left << setw(15) << Client.AccountNumber();
        cout << "| " << left << setw(20) << Client.FullName();
        cout << "| " << left << setw(12) << Client.GetAccountBalance();
    }



    static void ShowTotalBalances()
    {
        vector <clsBankClient> vClients = clsBankClient::GetClientsList();


        cout << "\n\t\t\t\t\tBalances List (" << vClients.size() << ") Client(s)\n";
        cout << "\n--------------------------------------------------------------------------------------------------------------------\n" << endl;
        cout << "| " << left << setw(15) << "Account Number";
        cout << "| " << left << setw(20) << "Client Name";
        cout << "| " << left << setw(12) << "Balance";
        cout << "\n--------------------------------------------------------------------------------------------------------------------\n" << endl;

        double TotalBalances = clsBankClient::GetTotalBalances();

        if (vClients.size() == 0)
            {
                cout << "\t\t\t\t No Clients Exists\n";
            }
        else
            {
                for (const clsBankClient  Client : vClients)
                    {
                        PrintClientRecordBalanceLine(Client);
                        cout << endl;
                    }
            }
        cout << "\n--------------------------------------------------------------------------------------------------------------------\n" << endl;
        cout << "\t\t\t\t\t Total Balance = " << TotalBalances << endl;
        cout << "\t\t\t\t\t (" << clsUtil::NumberToText(TotalBalances) << ")" << endl;
    }

    void Deposit(double Amount)
    {
        _AccountBalance += Amount;
        Save();
    }

    void Withdraw(double Amount)
    {
        if(Amount > _AccountBalance)
            {
                return;
            }
        else
            {
                _AccountBalance -= Amount;
                Save();
            }

    }


    bool Transfer(float Amount, clsBankClient &DestinationClient, string UserName)
    {
        if (Amount > GetAccountBalance())
            return false;

        Withdraw(Amount);
        DestinationClient.Deposit(Amount);


        _RegisterTransferLog(Amount, DestinationClient, UserName);
        return true;
    }


    string _PrepareTransferLogRecord(float Amount, clsBankClient& DestinationClient, string UserName, string Seperator = "#//#")
    {
        string Record = "";
        Record += clsDate::GetSystemDateTimeString() + Seperator;
        Record += AccountNumber() + Seperator;
        Record += DestinationClient.AccountNumber() + Seperator;
        Record += to_string(Amount) + Seperator;
        Record += to_string(GetAccountBalance()) + Seperator;
        Record += to_string(DestinationClient.GetAccountBalance()) + Seperator;
        Record += UserName;
        return Record;
    }


    void _RegisterTransferLog(float Amount, clsBankClient& DestinationClient, string UserName)
    {
        string stDataLine = _PrepareTransferLogRecord(Amount, DestinationClient, UserName);
        fstream MyFile;
        MyFile.open("TransferLog.txt", ios::out | ios::app);
        if (MyFile.is_open())
            {
                MyFile << stDataLine << endl;
                MyFile.close();
            }
    }
};
