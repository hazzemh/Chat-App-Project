#include<iostream>
#include<map>
#include<vector>
#include<fstream>
#include<stdio.h>
#include<string.h>
#include<string>
#include<list>
#include<sstream>
#include<ctime>
#include<chrono>
#include<queue>
#include<stdlib.h>
#include <cstdio>
#include"mysql_connection.h"
#include<cppconn/driver.h>
#include<cppconn/exception.h>
#include<cppconn/prepared_statement.h>
#include <mysql_driver.h>
using namespace sql;
using namespace std;
sql::mysql::MySQL_Driver* driver;
sql::Connection* con;
sql::Statement* stmt;
sql::Statement* stmt2;
sql::ResultSet* res;
int userid, roomId;
//registeration and login page
void page1();
//menu page
void page2(string);
//-------------------------------------------------------------------------------//
//"tcp://127.0.0.1:3306", "mysql", "Skillerhack1*"//
//map for username and password
map<string, string> user;
//map for description
map<string, pair<string, string>> descrip;
//map for group members
map<string, pair<string, string>> members;
//------------------------------------------------------------------------------//

void LoadGroupMsg(string uname, string grpname)
{
    map<string, pair<string, string>>::iterator im;
    int msgNum = 0, userID = 0, maxMsg = 0;
    res = stmt->executeQuery("select user_id from users where username='" + uname + "';");

    while (res->next())
    {
        userID = res->getInt(1);
    }
    res = stmt->executeQuery("select max(msgno) from groupmsg where groupname='" + grpname + "';");
    while (res->next())
    {
        msgNum = res->getInt(1);
    }
    string msg, uName;
    for (int i = 1; i <= msgNum; i++)
    {
        res = stmt->executeQuery("select username from users where user_id=(select user_id from groupmsg where msgno=" + to_string(i) + " and groupname='" + grpname + "');");
        while (res->next())
        {
            uName = res->getString("username");
        }
        if (uName == uname)
        {
            cout << "Me: ";
        }
        else
        {
            cout << uName << ": ";
        }
        res = stmt->executeQuery("select message from groupmsg where groupname='" + grpname + "' and msgno=" + to_string(i) + ";");
        while (res->next())
        {
            msg = res->getString("message");
        }
        cout << msg;
        cout << endl;
    }

}
void InsertGroupMsg(string uname, string grpname)
{

    string message;
    int msgNum = 1, userID = 0, maxMsg = 0;

    res = stmt->executeQuery("select user_id from users where username='" + uname + "';");
    while (res->next())
    {
        userID = res->getInt(1);
    }
    cout << "\nEnter your message: \n";
    cin.ignore();
    getline(cin, message);
    res = stmt->executeQuery("select max(msgno) from groupmsg where groupname='" + grpname + "';");
    while (res->next())
    {
        maxMsg = res->getInt(1);
    }
    if (maxMsg >= msgNum)
    {
        msgNum = maxMsg + 1;

        stmt->execute("insert into groupmsg (user_id,groupname,message,msgno) values(" + to_string(userID) + ",'" + grpname + "','" + message + "'," + to_string(msgNum) + ");");
    }
    else
    {
        stmt->execute("insert into groupmsg (user_id,groupname,message,msgno) values(" + to_string(userID) + ",'" + grpname + "','" + message + "'," + to_string(msgNum) + ");");
    }
}

void LoadGroupUsers()
{
    string name;
    int id = 0;
    map<string, string>::iterator iu;
    for (iu = user.begin(); iu != user.end(); iu++)
    {
        name = iu->first;
        res = stmt->executeQuery("select user_id from users where username ='" + name + "';");
        while (res->next())
        {
            id = res->getInt(1);
        }
        res = stmt->executeQuery("select groupname from groupchat where user_id='" + to_string(id) + "';");
        while (res->next())
        {
            members[iu->first].first = res->getString("groupname");
        }
    }
}
void LoadUsers(map <string, string>& users)
{

    sql::ResultSet* res1;
    int i, maxid = 0;
    string username, itostring;
    driver = sql::mysql::get_mysql_driver_instance();
    con = driver->connect("tcp://127.0.0.1:3306", "mysql", "Skillerhack1*");
    con->setSchema("firstschema1");
    stmt = con->createStatement();
    stmt2 = con->createStatement();
    res = stmt->executeQuery("select username,password from users where user_id = 1;");
    res1 = stmt2->executeQuery("select max(user_id) from users;");
    while (res1->next())
    {
        maxid = res1->getInt(1);
    }
    while (res->next())
    {
        username = res->getString("username");
        users[username] = res->getString("password");
    }
    for (i = 2; i <= maxid; i++)
    {
        itostring = to_string(i);
        res = stmt->executeQuery("select username, password from users where user_id = " + itostring + ";");
        while (res->next())
        {
            username = res->getString("username");
            users[username] = res->getString("password");
        }
    }

}

//function to store description
void storeDescrip(string name, string descripition, string state)
{

    descrip[name].first = descripition;
    descrip[name].second = state;
    int userID = 0;
    res = stmt->executeQuery("select user_id from users where username='" + name + "';");
    if (res->next())
    {
        userID = res->getInt(1);
    }
    res = stmt->executeQuery("select descrip from descripition where user_id= " + to_string(userID) + ";");
    if (res->next())
    {
        stmt->execute("update descripition set descrip ='" + descripition + "' , visibilty ='" + state + "'where user_id= " + to_string(userID) + ";");

    }
    else
    {
        stmt->execute("insert into descripition (user_id,descrip,visibilty) values(" + to_string(userID) + ",'" + descripition + "','" + state + "');");
    }
}

void loadDescrip(string uname)
{
    string visib, desc;
    int userID = 0;
    res = stmt->executeQuery("select user_id from users where username='" + uname + "';");
    if (res->next())
    {
        userID = res->getInt(1);
        res = stmt->executeQuery("select descrip , visibilty from descripition where user_id=" + to_string(userID) + ";");
        while (res->next())
        {
            descrip[uname].first = res->getString("descrip");
            descrip[uname].second = res->getString("visibilty");
        }
    }
}

//function to print user's description-
void Descript(string name)
{
    map<string, pair<string, string>>::iterator desc;
    desc = descrip.begin();
    for (; desc != descrip.end(); desc++)
    {
        if (desc->first == name && (desc->second).second == "y")
        {
            cout << endl << "Description : " << descrip[name].first;
        }
        else if (desc->first == name && (desc->second).second == "n")
        {
            cout << endl << "Sorry user's information is not visiable\n";
        }

    }
}
//user class
class User
{
    vector<string> users_list;
public:
    void add_contact(string);
    void delete_contact(string);
    void display_contacts(string);
    void create_chatroom(string, string);
    void loadFriends(string uname);
    void create_chatgroup(string, string);
    void showing_chatgroups(string, string);
    void inbox(string, string);
    void enterstory(string, string);
};

//function to create chatroom
void User::create_chatroom(string uname, string reciepient)
{
    int flag = 0;
    map<string, string>::iterator iuser;
    string msg;
    char choice;
    int activeID = 0, recID = 0, maxRoomid = 0, msgNum = 1, msgMax = 0, curRoomid = 0;
    for (iuser = user.begin(); iuser != user.end(); iuser++)
    {
        if (iuser->first == reciepient)
        {
            flag++;
        }
    }
    if (!flag)
    {
        cout << "\nSorry!! No user found\a\a\n";
        page2(uname);
    }
    res = stmt->executeQuery("select user_id from users where username ='" + uname + "';");
    while (res->next())
    {
        activeID = res->getInt(1);
    }
    res = stmt->executeQuery("select user_id from users where username='" + reciepient + "';");
    while (res->next())
    {
        recID = res->getInt(1);
    }
    res = stmt->executeQuery("select distinct chatroomid from chatrooms where chatroomid in (select chatroomid from chatrooms where user_id=" + to_string(recID) + " and recpid=" + to_string(activeID) + ");");
    if (res->next())
    {
        auto start = chrono::system_clock::now();
        auto legacyStart = chrono::system_clock::to_time_t(start);
        char tmBuff[30];
        ctime_s(tmBuff, sizeof(tmBuff), &legacyStart);
        string s(tmBuff);
        maxRoomid = res->getInt(1);
        cout << "\nEnter your Message: \n";
        cin.ignore();
        getline(cin, msg);
        res = stmt->executeQuery("select max(msgnuum) from chatrooms where chatroomid='" + to_string(maxRoomid) + "';");
        while (res->next())
        {
            msgMax = res->getInt(1);
        }
        if (msgMax >= msgNum)
        {
            msgNum = msgMax + 1;

            stmt->execute("insert into chatrooms (user_id,chatroomid,message,msgnuum,recpid,datetime) values(" + to_string(activeID) + ",'" + to_string(maxRoomid) + "','" + msg + "'," + to_string(msgNum) + "," + to_string(recID) + ",'" + s + "');");
        }
        else
        {
            stmt->execute("insert into chatrooms (user_id,chatroomid,message,msgnuum,recpid,datetime) values(" + to_string(activeID) + ",'" + to_string(maxRoomid) + "','" + msg + "'," + to_string(msgNum) + "," + to_string(recID) + ",'" + s + "');");
        }
        cout << "\nDo you want to undo this messege?(y/n)" << endl;
        cin >> choice;
        if (choice == 'y')
        {
            stmt->execute("delete from chatrooms where user_id = " + to_string(activeID) + " and chatroomid = " + to_string(maxRoomid) + " and message ='" + msg + "' and msgnuum= " + to_string(msgNum) + " and recpid= " + to_string(recID) + ";");
        }
    }
    else
    {
        auto start = chrono::system_clock::now();
        auto legacyStart = chrono::system_clock::to_time_t(start);
        char tmBuff[30];
        ctime_s(tmBuff, sizeof(tmBuff), &legacyStart);
        string s(tmBuff);
        cout << "\nEnter your Messages: \n";
        cin.ignore();
        getline(cin, msg);
        res = stmt->executeQuery("select max(chatroomid) from chatrooms;");
        if (res->next())
        {
            maxRoomid = res->getInt(1);
            maxRoomid++;
        }
        else
        {
            maxRoomid = 1;
        }
        res = stmt->executeQuery("select max(msgnuum) from chatrooms where chatroomid='" + to_string(maxRoomid) + "';");
        while (res->next())
        {
            msgMax = res->getInt(1);
        }
        if (msgMax >= msgNum)
        {
            msgNum = msgMax + 1;

            stmt->execute("insert into chatrooms (user_id,chatroomid,message,msgnuum,recpid,datetime) values(" + to_string(activeID) + ",'" + to_string(maxRoomid) + "','" + msg + "'," + to_string(msgNum) + "," + to_string(recID) + ",'" + s + "');");
        }
        else
        {
            stmt->execute("insert into chatrooms (user_id,chatroomid,message,msgnuum,recpid,datetime) values(" + to_string(activeID) + ",'" + to_string(maxRoomid) + "','" + msg + "'," + to_string(msgNum) + "," + to_string(recID) + ",'" + s + "');");
        }
        cout << "\nDo you want to undo this messege?(y/n)" << endl;
        cin >> choice;
        if (choice == 'y')
        {
            stmt->execute("delete from chatrooms where user_id = " + to_string(activeID) + " and chatroomid = " + to_string(maxRoomid) + " and message ='" + msg + "' and msgnuum= " + to_string(msgNum) + " and recpid= " + to_string(recID) + ";");
        }

    }

}

//function to create group chat
void User::create_chatgroup(string nameofgrp, string uname)
{
    int activeID = 0;
    int flag = 0;
    string recip;
    map<string, string> ::iterator iuser;
    res = stmt->executeQuery("select user_id from users where username ='" + uname + "';");
    if (res->next())
    {
        activeID = res->getInt(1);
    }
    stmt->execute("insert into groupchat (user_id,groupname) values(" + to_string(activeID) + ",'" + nameofgrp + "');");
Group_recip:
    cout << "\nEnter name of recipients:\n";
    cin >> recip;
    for (iuser = user.begin(); iuser != user.end(); iuser++)
    {

        if (iuser->first == recip)
        {
            flag++;
            break;
        }

    }
    if (!flag)
    {
        cout << "\nSorry!! No such user exist\a\a\n";
        goto Group_recip;
    }
    res = stmt->executeQuery("select user_id from users where username ='" + recip + "';");
    while (res->next())
    {
        activeID = res->getInt(1);
    }
    stmt->execute("insert into groupchat (user_id,groupname) values(" + to_string(activeID) + ",'" + nameofgrp + "');");
    cout << "\nDo you want to add more recipients?(y/n)\n";
    char cho;
    cin >> cho;
    if (cho == 'y')
        goto Group_recip;
}

//function to print group chats
void User::showing_chatgroups(string uname, string nameofgrp)
{
    int id = 0;
    string g, x;
    bool groupchk = true;
    res = stmt->executeQuery("select user_id from users where username='" + uname + "';");
    while (res->next())
    {
        id = res->getInt(1);
    }
    res = stmt->executeQuery("select groupname from groupchat where user_id=" + to_string(id) + ";");
    while (res->next())
    {
        x = res->getString("groupname");
        if (nameofgrp == x)
        {
            groupchk = false;
            break;
        }
        else
        {
            continue;
        }
    }
    if (!groupchk)
    {
        map<string, pair<string, string>> ::iterator gp;
        cout << "\nGroup Name: " << nameofgrp << "\n";
        cout << endl << endl;
        cout << "Group Members: ";
        for (gp = members.begin(); gp != members.end(); gp++)
        {
            if ((gp->second).first == nameofgrp && gp->first != uname)
            {
                cout << gp->first << " ";
            }
        }
        char choi;
        cout << "\nDo You Want to Send a Message?(y/n)" << endl;
        cin >> choi;
        if (choi == 'y')
            InsertGroupMsg(uname, nameofgrp);

        cout << "\nDo you want to see the group chat?(y/n)" << endl;
        cin >> choi;
        if (choi == 'y')
            LoadGroupMsg(uname, nameofgrp);

    }
    else
    {
        cout << "\nSorry you aren't member of this group!!\n";
    }
}

//function to show inbox
void User::inbox(string uname, string recepient)
{
    int flag = 0;
    map<string, string>::iterator iuser;
    string msg;
    int activeID = 0, recID = 0, chatroomID = 0, maxMsg;
    for (iuser = user.begin(); iuser != user.end(); iuser++)
    {
        if (iuser->first == recepient)
        {
            flag++;
        }
    }
    if (!flag)
    {
        cout << "\nSorry!! No user found\a\a\n";
        page2(uname);
    }
    res = stmt->executeQuery("select user_id from users where username ='" + uname + "';");
    while (res->next())
    {
        activeID = res->getInt(1);
    }
    res = stmt->executeQuery("select user_id from users where username='" + recepient + "';");
    while (res->next())
    {
        recID = res->getInt(1);
    }
    res = stmt->executeQuery("select distinct chatroomid from chatrooms where chatroomid in (select chatroomid from chatrooms where user_id=" + to_string(recID) + " and recpid=" + to_string(activeID) + ");");
    if (res->next())
    {
        chatroomID = res->getInt("chatroomid");
    }
    else
    {

        res = stmt->executeQuery("select distinct chatroomid from chatrooms where chatroomid in (select chatroomid from chatrooms where user_id=" + to_string(activeID) + " and recpid=" + to_string(recID) + ");");
        if (res->next())
        {
            chatroomID = res->getInt("chatroomid");
        }

    }
    res = stmt->executeQuery("select max(msgnuum) from chatrooms where chatroomid =" + to_string(chatroomID) + ";");
    while (res->next())
    {
        maxMsg = res->getInt(1);
    }
    int rotID = 0;
    for (int i = 1; i <= maxMsg; i++)
    {
        res = stmt->executeQuery("select user_id from chatrooms where chatroomid= " + to_string(chatroomID) + " and msgnuum = " + to_string(i) + ";");
        while (res->next())
        {
            rotID = res->getInt(1);
        }
        if (rotID == activeID)
        {
            cout << "Me : ";
        }
        else
            cout << recepient << " : ";
        res = stmt->executeQuery("select message from chatrooms where chatroomid = " + to_string(chatroomID) + " and msgnuum = " + to_string(i) + ";");
        if (res->next())
        {
            msg = res->getString("message");
        }
        cout << msg;
        cout << endl;
    }

}

void User::loadFriends(string uname)
{
    string recipN;
    sql::ResultSet* res2;
    bool chk = false;
    int userID = 0, rID = 0;
    vector <string> ::iterator ix;
    res = stmt->executeQuery("select user_id from users where username='" + uname + "';");
    if (res->next())
    {
        userID = res->getInt(1);
    }
    res2 = stmt->executeQuery("select recipid from friendlist where user_id=" + to_string(userID) + ";");
    while (res2->next())
    {
        rID = res2->getInt("recipid");
        res = stmt->executeQuery("select username from users where user_id= " + to_string(rID) + ";");
        if (res->next())
        {
            recipN = res->getString("username");
            if (!users_list.empty())
            {
                for (ix = users_list.begin(); ix != users_list.end(); ix++)
                {
                    if (*ix == recipN)
                    {
                        chk = true;
                        break;
                    }
                }
                if (!chk)
                {
                    users_list.push_back(recipN);
                }
            }
            else
            {
                users_list.push_back(recipN);
            }
        }

    }
}
//function to add contact
void User::add_contact(string uname)
{
    vector<string>::iterator iv;
    map<string, string>::iterator iuser;
    int uflag = 0, flag = 0, flag3 = 0;
    string addname;
    cout << "\nEnter the name which you want to add in your friend list" << endl;
    cin >> addname;
    cout << "\nPlease wait! checking for  " << addname << "......\n";
    // waiting time for new mwssage
    for (int i = 0; i < 99999910;)
    {
        i++;
    }
    for (iuser = user.begin(); iuser != user.end(); iuser++)
    {
        for (iv = users_list.begin(); iv != users_list.end(); iv++)
        {
            if (*iv == addname)
                flag3++;
        }
        if (addname == uname)
        {

            uflag++;
            break;
        }

        else if (iuser->first == addname)
            flag++;

    }

    //error for trying to add yourself as your friend
    int userID, recpID = 0;
    if (uflag == 1 && flag == 0 && flag3 == 0)
        cout << "\nSorry! Can't add yourself to your friend list\n";
    if (flag == 1 && uflag == 0 && flag3 == 0)
    {
        cout << "\nFound\t" << addname << endl;
        users_list.push_back(addname);
        cout << "\nCongratulations! " << addname << " has been added to your friend list\n" << endl;
        res = stmt->executeQuery("select user_id from users where username='" + uname + "';");
        if (res->next())
        {
            userID = res->getInt(1);
        }
        res = stmt->executeQuery("select user_id from users where username='" + addname + "';");
        if (res->next())
        {
            recpID = res->getInt(1);
        }
        stmt->execute("insert into friendlist (user_id,recipid) values(" + to_string(userID) + "," + to_string(recpID) + ");");
    }
    else if (uflag == 0 && flag == 0)
    {
        cout << "\nSorry!! this username does not exist\n";
    }
    else if (flag3)
    {
        cout << "\n" << addname << " already exists in your friend list\n";
    }

}

//function to display contacts 
void User::display_contacts(string uname)
{
    int c = 0, maxstory = 0, userID = 0;
    vector<string>::iterator i;
    i = users_list.begin();
    string name, storytext;

    cout << "\nWelcome " << uname << " Your Friends are:" << endl;
    for (; i != users_list.end(); i++)
    {
        cout << "\n" << *i;
        name = *i;
        loadDescrip(name);
        Descript(name);
        res = stmt->executeQuery("select user_id from users where username ='" + name + "';");
        if (res->next())
        {
            userID = res->getInt(1);
        }
        res = stmt->executeQuery("select max(storyno) from story where user_id = '" + to_string(userID) + "';");
        if (res->next())
        {
            maxstory = res->getInt(1);
        }
        for (int i = 1; i <= maxstory; i++)
        {
            cout << "\nStory:";
            res = stmt->executeQuery("select story from story where user_id = '" + to_string(userID) + "' and storyno = " + to_string(i) + ";");

            if (res->next())
            {
                storytext = res->getString("story");
            }
            cout << storytext << endl;
        }
        c++;
    }
    cout << endl << endl << "\nTotal friends: " << c;
}

//function to create story
void User::enterstory(string uname, string story)
{
    int userID = 0, story_number = 1, story_max = 0;
    res = stmt->executeQuery("select user_id from users where username='" + uname + "';");
    if (res->next())
    {
        userID = res->getInt(1);
    }
    res = stmt->executeQuery("select max(storyno) from story where user_id='" + to_string(userID) + "';");
    while (res->next())
    {
        story_max = res->getInt(1);
    }

    if (story_max >= story_number)
    {
        story_number = story_max + 1;

        stmt->execute("insert into story (user_id , story, storyno) values (" + to_string(userID) + " , '" + story + "','" + to_string(story_number) + "');");
    }
    else
    {
        stmt->execute("insert into story (user_id , story, storyno) values (" + to_string(userID) + " , '" + story + "','" + to_string(story_number) + "');");
    }

}

//function to delete contact
void User::delete_contact(string uname)
{
    if (users_list.empty())
    {
        cout << "\nFriend list empty..!!\a\n";
    }
    else {
        int c = 0, pos = 0, flag = 0;
        string name;
        cout << "\nenter the friend name you want to unfriend \n";
        cin >> name;
        vector<string>::iterator i;
        i = users_list.begin();

        for (; i != users_list.end(); i++)
        {
            c++;
            if (*i == name)
            {
                flag++;
                pos = c - 1;
                break;
            }
        }

        i = users_list.begin();
        if (pos || flag)
        {
            cout << "\n Removing " << name << " from your friend list..\a\a\a\a\n";
            users_list.erase(i + pos);
            cout << name << " has been successfully removed from your friend list\n";
            int userID = 0, recID = 0;
            res = stmt->executeQuery("select user_id from users where username='" + uname + "';");
            if (res->next())
            {
                userID = res->getInt(1);
            }
            res = stmt->executeQuery("select user_id from users where username='" + name + "';");
            if (res->next())
            {
                recID = res->getInt(1);
            }
            stmt->execute("delete from friendlist where user_id= " + to_string(userID) + " and recipid = " + to_string(recID) + " ;");
        }
        if (flag == 0 && pos == 0)
        {
            cout << "\nNo such friend exists in your friend list\n";
        }
    }
}


void page1()
{
    system("cls");

    int flag = 0, choice;

    cout << "\n\t\t\tWELCOME TO THE Chat Application\n";

    while (1)
    {

        cout << "\nPlease Enter your choice\n  1-Sign_up\n  2-Sign_in\n  3-Exit \n \n" << endl;
        cin >> choice;

        switch (choice)
        {
        case 1:
        {
            string username, pwd;
            cout << "\nchoose a username\n";
            cin >> username;
            cout << "\nchoose a password\n";
            cin >> pwd;
            map<string, string>::iterator iuser;
            driver = sql::mysql::get_driver_instance();
            con = driver->connect("tcp://127.0.0.1:3306", "mysql", "Skillerhack1*");
            con->setSchema("firstschema1");
            stmt = con->createStatement();
            res = stmt->executeQuery("select max(user_id) from users;");
            if (res->next())
            {
                userid = res->getInt(1);
                userid++;
            }
            else
            {
                userid = 1;
            }
            if (user.empty())
            {

                string userid2 = to_string(userid);
                stmt->execute("insert into users(username,password,user_id) values('" + username + "','" + pwd + "'," + userid2 + ");");
                delete con;
                delete stmt;
                delete driver;
                user[username] = pwd;

            }
            else
            {
                bool chk = true;
                for (int i = 0; i < 1; i++)
                {
                    for (iuser = user.begin(); iuser != user.end(); iuser++)
                    {
                        if (iuser->first == username)
                        {
                            cout << "\nSorry! username not available..please try with some other name\n";
                            chk = false;
                            break;
                        }
                    }
                    if (chk)
                    {
                        string userid2 = to_string(userid);
                        stmt->execute("insert into users(username,password,user_id) values('" + username + "','" + pwd + "'," + userid2 + ");");
                        user[username] = pwd;

                    }
                }

            }
            break;

        }


        case 2:
        {

            string lusername, lpwd;
            cout << "\nenter your username\n";
            cin >> lusername;
            cout << "\nenter your password\n";
            cin >> lpwd;
            map<string, string>::iterator iuser;
            for (iuser = user.begin(); iuser != user.end(); iuser++)
            {
                if (iuser->first == lusername && iuser->second == lpwd)
                {
                    cout << "\nWELCOME " << lusername << "!!  You have successfully logged in \n";
                    flag = 1;
                    loadDescrip(lusername);
                    page2(lusername);
                    break;

                }
            }


            if (flag == 0)
            {
                cout << "\nusername or password incorrect\n\t!!Please try again\n";
                break;
            }
            break;
        }


        case 3:
        {
            //exit(0);
            cout << "\ndisplay\n";
            map<string, string>::iterator iuser;
            cout << "\nall users\n";
            for (iuser = user.begin(); iuser != user.end(); iuser++)
            {
                cout << iuser->first << "\t" << iuser->second;
            }
            break;
        }
        default:
        {
            cout << "\n ERRORR!! \n";
            system("pause");
            page1();
        }
        }
        if (flag == 1)
        {
            break;
        }

    }
}

map<string, User> f;
void page2(string uname)
{
    system("cls");
    int choice2, choice3;
    f[uname].loadFriends(uname);
main_menu:
    cout << endl << endl << endl << endl << endl << endl << endl << endl << endl << endl << endl << endl << endl;
    cout << "\t\t\t     WELCOME    " << uname << endl << endl;

    cout << "\n\t    Choose from following menu*\n";
    cout << "\n1-Inbox\n2-Chat\n3-Friend list\n4-Add friend\n5-Unfriend\n6-Group chat\n7-Add or Edit your description\n8-Add to my story\n9-Show chatgroups\n10-Log Out\n";
    cin >> choice2;
    switch (choice2)
    {
    case 1:
    {
        system("cls");
        cout << "\nWelcome " << uname << " to your Chatrooms\n\n";
        cout << "Enter the name of you want to show the chat with : ";
        string selectname;
        cin >> selectname;
        if (selectname == uname)
            cout << "Error!";
        else
        {
            f[uname].inbox(uname, selectname);
        }
        cout << "\npress any number except '0' to go back to main menu\n";
        cin >> choice3;
        if (choice3)
            goto main_menu;

        break;
    }
    case 2:
    {
        system("cls");
        string reciepient;
        cout << "\nPlease enter the reciepient's name:\n";
        cin >> reciepient;
        f[uname].create_chatroom(uname, reciepient);
        cout << "\npress any number except '0' to go back to main menu\n";
        cin >> choice3;
        if (choice3)
            goto main_menu;

        break;

    }
    case 3:
    {
        system("cls");
        f[uname].display_contacts(uname);
        cout << "\npress any number except '0' to go back to main menu\n";
        cin >> choice3;
        if (choice3)
            goto main_menu;
        break;
    }
    case 4:
    {
        system("cls");
        f[uname].add_contact(uname);
        cout << "\npress any number except '0' to go back to main menu\n";
        cin >> choice3;
        if (choice3)
            goto main_menu;
        break;
    }
    case 5:
    {
        system("cls");
        f[uname].delete_contact(uname);
        cout << "\npress any number except '0' to go back to main menu\n";
        cin >> choice3;
        if (choice3)
            goto main_menu;
        break;

    }

    case 6:
    {
        system("cls");
        string groupn;
        cout << "\nEnter Group Name:\n";
        cin >> groupn;
        f[uname].create_chatgroup(groupn, uname);
        cout << "\npress any number except '0' to go back to main menu\n";
        cin >> choice3;
        if (choice3)
            goto main_menu;
        break;
    }

    case 7:
    {
        int choice4;
        string visiability;
        string description;
        f[uname].loadFriends(uname);
        cout << "\nEnter your Description:\n";
        cin.ignore();
        getline(cin, description);
        cout << "\nDo you want your information to be visiable ?(y/n)\n";
        cin >> visiability;
        storeDescrip(uname, description, visiability);
        cout << "\npress any number except '0' to go back to main menu\n";
        cin >> choice4;
        if (choice4)
            goto main_menu;
        break;
    }
    case 8:
    {
        char cho;
        string text;
        system("cls");
        cout << "Enter your Story Text : " << endl;
        cin.ignore();
        getline(cin, text);
        f[uname].enterstory(uname, text);
        cout << "\nYour Story is added!\n";
        cout << "\npress any number except '0' to go back to main menu\n";
        cin >> choice3;
        if (choice3)
            goto main_menu;
        break;
    }
    case 9:
    {
        system("cls");
        string gname;
        cout << "\nEnter the name of the group : \n";
        cin >> gname;
        f[uname].showing_chatgroups(uname, gname);
        cout << "\npress any number except '0' to go back to main menu\n";
        cin >> choice3;
        if (choice3)
            goto main_menu;
        break;
    }
    case 10:
    {
        system("cls");
        cout << "\nyou have been successfully logged out\n";

        page1();
        break;

    }

    }
}
int main()
{
    LoadUsers(user);
    LoadGroupUsers();
    page1();
    delete con;
    delete stmt;
    delete driver;
    return 0;
}