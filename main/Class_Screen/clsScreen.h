#pragma once
#include <iostream>
#include "clsUser.h"
#include "Global.h"


using namespace std;

class clsScreen
{
protected :
    static void _DrawScreenHeader(string Title,string SubTitle = "")
    {
        cout << "\t\t\t\t\t______________________________________";
        cout << "\n\n\t\t\t\t\t  " << Title;
        if (SubTitle != "")
            {
                cout << "\n\t\t\t\t\t  " << SubTitle;
            }
        cout << "\n\t\t\t\t\t______________________________________\n\n";
        cout << "\t\t\t\t\tUser: " << CurrentUser.GetUserName() << endl;
        cout << "\t\t\t\t\tDate: " << date.DateToString() << endl;
    }

    static bool CheckAccessRights(clsUser::enPermissions Permissions)
    {
        if(!CurrentUser.CheckAccessPermission(Permissions))
            {
                cout << "\n\t\t\t\t--------------------------------------------\n";
                cout << "\n\t\t\t\t\t\t Access Denied!\n";
                cout << "\n\t\t\t\t--------------------------------------------\n";
                return false;
            }
        else
            {
                return true;
            }
    }
};

