#pragma once

#include <iostream>
#include "clsScreen.h"
#include "clsBankClient.h"


class clsShowTransferLogScreen : protected clsScreen
{

private:

    static void PrintRecordBalanceLine(clsBankClient::stTransferLogRecord & Record)
    {

        cout << setw(8) << left << "" << "| " << setw(23) << left << Record.DateTime;
        cout << "| " << setw(8)  << left  << Record.SourceAccountNumber;
        cout << "| " << setw(8)  << left  << Record.DestinationAccountNumber;
        cout << "| " << setw(8)  << left  << Record.Amount;
        cout << "| " << setw(10) << left  << Record.srcBalanceAfter;
        cout << "| " << setw(10) << left  << Record.destBalanceAfter;
        cout << "| " << setw(8)  << left  << Record.UserName;
    }


public:

    static void ShowTransferLogScreen()
    {
        vector <clsBankClient::stTransferLogRecord> vRecord = clsBankClient::GetTransferLog();

        string Title = "\t  Transfer Log Screen";
        string SubTitle = "\t    (" + to_string(vRecord.size()) + ") Record(s).";

        _DrawScreenHeader(Title, SubTitle);

        cout << setw(25) << left << "" << "\n\t\t_______________________________________________________";
        cout << "__________________________\n" << endl;

        cout << setw(8) << left << "" << "| " << left << setw(23) << "Date/Time";
        cout << "| " << left << setw(8) << "s.Acct";
        cout << "| " << left << setw(8) << "d.Acct";
        cout << "| " << left << setw(8) << "Amount";
        cout << "| " << left << setw(10) << "s.Balance";
        cout << "| " << left << setw(10) << "d.Balance";
        cout << "| " << left << setw(8) << "User";
        cout << setw(25) << left << "" << "\t\t_______________________________________________________";
        cout << "__________________________\n" << endl;


        if (vRecord.size() == 0)
            cout << "\t\t\t\tNo Records Available In the System!";
        else

            for (clsBankClient::stTransferLogRecord Record : vRecord)
                {
                    PrintRecordBalanceLine(Record);
                    cout << endl;
                }
    }

};
