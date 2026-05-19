#pragma once

#include <iostream>
#include "clsScreen.h"
#include "clsBankClient.h"
#include "clsInputValidate.h"

class clsTransferScreen : protected clsScreen
{


private:

    static void _PrintClient(clsBankClient & Client)
    {
        cout << "\nClient Card:";
        cout << "\n___________________";
        cout << "\nFull Name   : " << Client.FullName();
        cout << "\nAcc. Number : " << Client.AccountNumber();
        cout << "\nBalance     : " << Client.GetAccountBalance();
        cout << "\n___________________\n";

    }

    static string _ReadAccountNumber()
    {
        string AccountNumber = "";
        cin >> AccountNumber;
        return AccountNumber;
    }

public:

    static void ShowTransferScreen()
    {
        _DrawScreenHeader("\t    Transfer Screen");

        cout << "\nPlease enter account number to transfer from: ";
        string AcTransferFrom = _ReadAccountNumber();

        while (!clsBankClient::IsClientExist(AcTransferFrom))
            {
                cout << "\nClient with [" << AcTransferFrom << "] does not exist, try another one.\n";
                AcTransferFrom = _ReadAccountNumber();
            }

        clsBankClient Client1 = clsBankClient::Find(AcTransferFrom);
        _PrintClient(Client1);

        cout << "\nPlease enter account number to transfer to: ";
        string AcTransferTo = _ReadAccountNumber();

        while (!clsBankClient::IsClientExist(AcTransferTo) || AcTransferFrom == AcTransferTo)
            {
                cout << "\nYou Can't Transfer to Client with [" << AcTransferTo << "], try another one.\n";
                AcTransferTo = _ReadAccountNumber();
            }

        clsBankClient Client2 = clsBankClient::Find(AcTransferTo);
        _PrintClient(Client2);

        double Amount = 0;
        cout << "\nPlease enter Transfer amount? ";
        Amount = clsInputValidate::ReadDblNumber();

        while(Amount > Client1.GetAccountBalance())
            {
                cout << "\nAmount Exceeds the available Balance, Enter another amount\n";
                Amount = clsInputValidate::ReadDblNumber();
            }

        cout << "\nAre you sure you want to perform this transaction?y/n ";
        char Answer = 'n';
        cin >> Answer;

        if (Answer == 'Y' || Answer == 'y')
            {

                Client1.Transfer(Amount, Client2, CurrentUser.GetUserName());

                Client2 = clsBankClient::Find(AcTransferTo);
                Client1 = clsBankClient::Find(AcTransferFrom);

                cout << "\nAmount Transferd Successfully.\n";
                _PrintClient(Client1);
                _PrintClient(Client2);
            }
        else
            {
                cout << "\nOperation was cancelled.\n";
            }
    }
};



