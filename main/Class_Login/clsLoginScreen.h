#pragma once
#include <iomanip>
#include <fstream>
#include "clsMainScreen.h"
#include "Global.h"
#include "clsBankClient.h"

class clsLoginScreen : protected clsScreen
{

private:
    static bool _login()
    {
        short FalidLoginCount = 0;
        bool LoginFaild = false;
        string UserName,Password;

        do
            {
                if(LoginFaild)
                    {
                        FalidLoginCount++;
                        cout << "\nInvalid UserName/Password\n";
                        cout << "You have " << (3 - FalidLoginCount) << " Trial(s) to login\n\n";

                    }

                if(FalidLoginCount == 3)
                    {
                        cout << "You are Locked after 3 faild trials\n";
                        return false;
                    }

                cout << "Enter User Name? ";
                cin >> UserName;

                cout << "Enter Password? ";
                cin >> Password;

                CurrentUser = clsUser::Find(UserName,Password);

                LoginFaild = CurrentUser.IsEmpty();
            }
        while(LoginFaild);
        CurrentUser.RegisterLogIn();
        clsMainScreen::ShowMainMenue();
        return true;
    }

public:
    static bool ShowLoginScreen()
    {
        system("cls");
        _DrawScreenHeader("\t Login Screen");
        return _login();
    }
};
