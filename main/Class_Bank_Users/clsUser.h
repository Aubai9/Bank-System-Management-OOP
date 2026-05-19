#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "clsPerson.h"
#include "clsString.h"
#include "clsDate.h"
#include "clsUtil.h"

using namespace std;

class clsUser : public clsPerson
{
private:
    enum enMode { EmptyMode = 0, UpdateMode = 1, AddNewMode = 2 };
    enMode _Mode;
    string _UserName;
    string _Password;
    int _Permissions;
    bool _MarkedForDelete = false;

    static clsUser _ConvertLinetoUserObject(string Line, string Seperator = "#//#")
    {
        vector<string> vUserData = clsString::Split(Line, Seperator);
        if (vUserData.size() < 7) return _GetEmptyUserObject();

        return clsUser(enMode::UpdateMode, vUserData[0], vUserData[1], vUserData[2], vUserData[3],
                       vUserData[4], clsUtil::DecryptText(vUserData[5]), stoi(vUserData[6]));
    }

    static string _ConverUserObjectToLine(clsUser User, string Seperator = "#//#")
    {
        string UserRecord = "";
        UserRecord += User.GetFirstName() + Seperator;
        UserRecord += User.GetLastName() + Seperator;
        UserRecord += User.GetEmail() + Seperator;
        UserRecord += User.GetPhone() + Seperator;
        UserRecord += User.GetUserName() + Seperator;
        UserRecord += clsUtil::EncryptText(User.GetPassword()) + Seperator;
        UserRecord += to_string(User.GetPermissions());
        return UserRecord;
    }

    static vector <clsUser> _LoadUsersDataFromFile()
    {
        vector <clsUser> vUsers;
        fstream MyFile;
        MyFile.open("Users.txt", ios::in);
        if (MyFile.is_open())
            {
                string Line;
                while (getline(MyFile, Line))
                    {
                        if (Line == "") continue;

                        clsUser User = _ConvertLinetoUserObject(Line);
                        if (!User.IsEmpty())
                            {
                                vUsers.push_back(User);
                            }
                    }
                MyFile.close();
            }
        return vUsers;
    }
    static void _SaveUsersDataToFile(vector<clsUser> vUsers)
    {
        fstream MyFile("Users.txt", ios::out);
        if (MyFile.is_open())
            {
                for (clsUser& U : vUsers)
                    {
                        if (!U._MarkedForDelete)
                            {
                                MyFile << _ConverUserObjectToLine(U) << endl;
                            }
                    }
                MyFile.close();
            }
    }

    void _Update()
    {
        vector<clsUser> _vUsers = _LoadUsersDataFromFile();
        for (clsUser& U : _vUsers)
            {
                if (U.GetUserName() == _UserName)
                    {
                        U = *this;
                        break;
                    }
            }
        _SaveUsersDataToFile(_vUsers);
    }

    void _AddNew()
    {
        _AddDataLineToFile(_ConverUserObjectToLine(*this));
    }

    void _AddDataLineToFile(string stDataLine)
    {
        fstream MyFile("Users.txt", ios::out | ios::app);
        if (MyFile.is_open())
            {
                MyFile << stDataLine << endl;
                MyFile.close();
            }
    }

    static clsUser _GetEmptyUserObject()
    {
        return clsUser(enMode::EmptyMode, "", "", "", "", "", "", 0);
    }

public:

    struct stLoginRegisterRecord
    {
        string DateTime;
        string UserName;
        string Password;
        int Permissions;
    };

    enum enPermissions { eAll = -1, pListClients = 1, pAddNewClient = 2, pDeleteClient = 4, pUpdateClients = 8, pFindClient = 16, pTranactions = 32, pManageUsers = 64, pShowLoginRegister = 128 };

    clsUser(enMode Mode, string FirstName, string LastName, string Email, string Phone, string UserName, string Password, int Permissions)
        : clsPerson(FirstName, LastName, Email, Phone)
    {
        _Mode = Mode;
        _UserName = UserName;
        _Password = Password;
        _Permissions = Permissions;
    }

    bool IsEmpty()
    {
        return (_Mode == enMode::EmptyMode);
    }
    bool MarkedForDeleted()
    {
        return _MarkedForDelete;
    }
    string GetUserName()
    {
        return _UserName;
    }
    void SetUserName(string UserName)
    {
        _UserName = UserName;
    }
    void SetPassword(string Password)
    {
        _Password = Password;
    }
    string GetPassword()
    {
        return _Password;
    }
    void SetPermissions(int Permissions)
    {
        _Permissions = Permissions;
    }
    int GetPermissions()
    {
        return _Permissions;
    }


    static clsUser Find(string UserName)
    {
        fstream MyFile("Users.txt", ios::in);
        if (MyFile.is_open())
            {
                string Line;
                while (getline(MyFile, Line))
                    {
                        clsUser User = _ConvertLinetoUserObject(Line);
                        if (User.GetUserName() == UserName) return User;
                    }
            }
        return _GetEmptyUserObject();
    }


    static clsUser Find(string UserName, string Password)
    {
        fstream MyFile("Users.txt", ios::in);
        if (MyFile.is_open())
            {
                string Line;
                while (getline(MyFile, Line))
                    {
                        clsUser User = _ConvertLinetoUserObject(Line);
                        if (User.GetUserName() == UserName && User.GetPassword() == Password) return User;
                    }
            }
        return _GetEmptyUserObject();
    }

    static bool IsUserExist(string UserName)
    {
        return !Find(UserName).IsEmpty();
    }

    static vector<clsUser> GetUsersList()
    {
        return _LoadUsersDataFromFile();
    }

    static clsUser GetAddNewUserObject(string UserName)
    {
        return clsUser(enMode::AddNewMode, "", "", "", "", UserName, "", 0);
    }

    enum enSaveResults { svFaildEmptyObject = 0, svSucceeded = 1, svFaildUserExists = 2 };

    enSaveResults Save()
    {
        switch (_Mode)
            {
            case enMode::EmptyMode:
                return svFaildEmptyObject;
            case enMode::UpdateMode:
                _Update();
                return svSucceeded;
            case enMode::AddNewMode:
                if (IsUserExist(_UserName)) return svFaildUserExists;
                _AddNew();
                _Mode = enMode::UpdateMode;
                return svSucceeded;
            }
        return svFaildEmptyObject;
    }

    bool Delete()
    {
        vector<clsUser> _vUsers = _LoadUsersDataFromFile();
        for (clsUser& U : _vUsers)
            {
                if (U.GetUserName() == _UserName)
                    {
                        U._MarkedForDelete = true;
                        break;
                    }
            }
        _SaveUsersDataToFile(_vUsers);
        *this = _GetEmptyUserObject();
        return true;
    }

    bool CheckAccessPermission(enPermissions Permission)
    {
        if (this->_Permissions == enPermissions::eAll) return true;
        return ((Permission & this->_Permissions) == Permission);
    }

    void RegisterLogIn()
    {

        string Line = clsDate::GetSystemDateTimeString() + "#//#" + _UserName + "#//#" + clsUtil::EncryptText(_Password) + "#//#" + to_string(_Permissions);
        fstream MyFile("LoginRegister.txt", ios::out | ios::app);
        if (MyFile.is_open())
            {
                MyFile << Line << endl;
                MyFile.close();
            }
    }
public:
    struct stLoginRegisterRecord;
    static stLoginRegisterRecord _ConvertLoginRegisterLineToRecord(string Line, string Seperator = "#//#")
    {
        stLoginRegisterRecord Record;
        vector<string> vData = clsString::Split(Line, Seperator);
        if (vData.size() < 4) return Record;

        Record.DateTime = vData[0];
        Record.UserName = vData[1];
        Record.Password = clsUtil::DecryptText(vData[2]);
        Record.Permissions = stoi(vData[3]);
        return Record;
    }
    static vector<stLoginRegisterRecord> GetLoginRegisterList()
    {
        vector<stLoginRegisterRecord> vLoginRegisterRecord;
        fstream MyFile("LoginRegister.txt", ios::in);

        if (MyFile.is_open())
            {
                string Line;
                while (getline(MyFile, Line))
                    {
                        if (Line == "") continue;

                        stLoginRegisterRecord Record = _ConvertLoginRegisterLineToRecord(Line);

                        if (Record.UserName != "")
                            {
                                vLoginRegisterRecord.push_back(Record);
                            }
                    }
                MyFile.close();
            }
        return vLoginRegisterRecord;
    }


};
