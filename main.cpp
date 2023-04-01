#include <stdlib.h>
#include <iostream>

#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
using namespace std;

const string server = "tcp://127.0.0.1:3306";
const string username = "root";

int enter_main()
{
    int number = 0;
    while (number == 0)
    {
        cout << "Please enter number to select operation.\n1 - to exit; 2 - to enter patient; 3 - to add new medicine to the database; 4 - to make a report about side effects; \n5 - to enter new appointments; 6 - to determine the number of appointmens on the selected day; \n7 - to determine the number of patients with this disease; 8 - to make a call statement.\n";
        cin >> number;
        if (cin.fail())
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return number;
}

int enter()
{
    int number = 0;
    while (number == 0)
    {
        cin >> number;
        if (cin.fail())
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return number;
}

void enter_patient(sql::Connection* con)
{
    sql::PreparedStatement* pstmt;
    string str;

    pstmt = con->prepareStatement("INSERT INTO patient(name, sex, birthday, adress) VALUES(?,?,?,?)");
    cout << "Enter name: ";
    getline(cin, str);
    pstmt->setString(1, str);
    cout << "Enter sex: ";
    getline(cin, str);
    pstmt->setString(2, str);
    cout << "Enter birthday: ";
    getline(cin, str);
    pstmt->setString(3, str);
    cout << "Enter adress: ";
    getline(cin, str);
    pstmt->setString(4, str);
    pstmt->execute();
    delete pstmt;
}

void enter_medicine(sql::Connection* con)
{
    sql::PreparedStatement* pstmt;
    string str;

    pstmt = con->prepareStatement("INSERT INTO medicine(name, instruction, description, side_effects) VALUES(?,?,?,?)");
    cout << "Enter name: ";
    getline(cin, str);
    pstmt->setString(1, str);
    cout << "Enter instruction: ";
    getline(cin, str);
    pstmt->setString(2, str);
    cout << "Enter description: ";
    getline(cin, str);
    pstmt->setString(3, str);
    cout << "Enter side effects: ";
    getline(cin, str);
    pstmt->setString(4, str);
    pstmt->execute();
    delete pstmt;
}

void side_effect_report(sql::Connection* con)
{
    sql::Statement* stmt;
    sql::ResultSet* res;
    string str, query1, buf;
    int number;
    do {
        cout << "Enter name of medicine: ";
        getline(cin, str);
        query1 = "SELECT COUNT(*) FROM medicine WHERE name ='" + str + "'";
        stmt = con->createStatement();
        res = stmt->executeQuery(query1);
        while (res->next()) {
            buf = res->getString("COUNT(*)");
        }
        number = stoi(buf);
        if (!number)
        {
            cout << "This medicine not in tabel.\n";
            cout << "Quit from task?(1 - No; 2 - Yes)\n";
            number = enter() - 1;
            if (number == 1) return;
        }
    } while(!number);
    query1 = "SELECT side_effects FROM medicine WHERE name='" + str + "'";
    stmt = con->createStatement();

    res = stmt->executeQuery(query1);
    while (res->next()) {
        cout << res->getString("side_effects") << endl;
    }

    delete stmt;
    delete res;
}

void enter_appointmens(sql::Connection* con)
{
    sql::PreparedStatement* pstmt;
    sql::Statement* stmt;
    sql::ResultSet* res;
    string str, query1, buf, medic = "";
    int number;
    pstmt = con->prepareStatement("INSERT INTO appointments(patient_name, doctor_name, date, place, simptoms, diagnose, instructions) VALUES(?,?,?,?,?,?,?)");
    do {
        cout << "Enter patient name: ";
        getline(cin, str);
        query1 = "SELECT COUNT(*) FROM patient WHERE name ='" + str + "'";
        stmt = con->createStatement();
        res = stmt->executeQuery(query1);
        while (res->next()) {
            buf = res->getString("COUNT(*)");
        }
        number = stoi(buf);
        if (number)
        {
            pstmt->setString(1, str);
        }
        else
        {
            if (cin.fail()) 
            {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
            cout << "This patient not in table.\nAdd patien in tabel?(1 - No; 2 - Yes)\n";
            number = enter();
            if (number == 2)
            {
                enter_patient(con);
                pstmt->setString(1, str);
            }
            else
            {
                cout << "Quit from task?(1 - No; 2 - Yes)\n";
                number = enter() - 1;
                if (number == 1) return;
            }
        }
    } while (!number);
    pstmt->setString(1, str);
    cout << "Enter doctor name: ";
    getline(cin, str);
    pstmt->setString(2, str);
    cout << "Enter date: ";
    getline(cin, str);
    pstmt->setString(3, str);
    cout << "Enter place: ";
    getline(cin, str);
    pstmt->setString(4, str);
    cout << "Enter simptoms: ";
    getline(cin, str);
    pstmt->setString(5, str);
    cout << "Enter diagnose: ";
    getline(cin, str);
    pstmt->setString(6, str);
    do {
        cout << "Enter medicine(one by one): ";
        getline(cin, str);
        query1 = "SELECT COUNT(*) FROM medicine WHERE name ='" + str + "'";
        stmt = con->createStatement();
        res = stmt->executeQuery(query1);
        while (res->next()) {
            buf = res->getString("COUNT(*)");
        }
        number = stoi(buf);
        if (number)
        {
            medic += str;
            cout << "This medicine is last?(1 - No; 2 - Yes)\n";
            number = (enter()) - 1;
            if (number == 0)
            {
                medic += ", ";
            }
        }
        else
        {
            if (cin.fail())
            {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
            cout << "This medicine not in table.\nAdd medicine in table?(1 - No; 2 - Yes)\n";
            number = enter();
            if (number == 2)
            {
                enter_medicine(con);
                medic += str;
                cout << "This medicine is last?(1 - No; 2 - Yes)\n";
                number = (enter()) - 1;
                if (number == 0)
                {
                    medic += ", ";
                }
            }
            else
            {
                cout << "Quit from task?(1 - No; 2 - Yes)\n";
                number = enter() - 1;
            }
        }
    } while (!number);

    medic += ".";
    pstmt->setString(7, medic);
    pstmt->execute();
    delete pstmt;
}

void appointments_in_day(sql::Connection* con)
{
    sql::Statement* stmt;
    sql::ResultSet* res;
    string str, query1;

    cout << "Enter date: ";
    getline(cin, str);
    query1 = "SELECT COUNT(*) FROM appointments WHERE date='" + str + "' and place != 'hospital'";
    stmt = con->createStatement();
    res = stmt->executeQuery(query1);
    while (res->next()) {
        cout << "On this day, number of appointments is " << res->getString("COUNT(*)") << "\n\n";
    }

    delete stmt;
    delete res;
}

void number_of_disease(sql::Connection* con)
{
    sql::Statement* stmt;
    sql::ResultSet* res;
    string str, query1;

    cout << "Enter disease: ";
    getline(cin, str);
    query1 = "SELECT COUNT(*) FROM appointments WHERE diagnose='" + str + "'";
    stmt = con->createStatement();
    res = stmt->executeQuery(query1);
    while (res->next()) {
        cout << "Number of patient with this disease is " << res->getString("COUNT(*)") << "\n\n";
    }

    delete stmt;
    delete res;
}

void call_statement(sql::Connection* con)
{
    sql::Statement* stmt;
    sql::ResultSet* res;
    string str, query1;

    query1 = "SELECT doctor_name,  COUNT(doctor_name) FROM appointments where place!='hospital' group by doctor_name";
    stmt = con->createStatement();
    res = stmt->executeQuery(query1);
    while (res->next()) {
        cout << "Doctor " << res->getString("doctor_name") << " had " << res->getString("COUNT(doctor_name)") << " appointments.\n";
    }

    delete stmt;
    delete res;
}

int main()
{
    sql::Driver* driver;
    sql::Connection* con;
    sql::Statement* stmt;
    sql::PreparedStatement* pstmt;
    string password;
    cout << "Enter password: ";
    getline(cin, password);
    try
    {
        driver = get_driver_instance();
        con = driver->connect(server, username, password);
    }
    catch (sql::SQLException e)
    {
        cout << "Could not connect to server. Error message: " << e.what() << endl;
        system("pause");
        exit(1);
    }
    con->setSchema("patient");
    stmt = con->createStatement();
    stmt->execute("CREATE TABLE IF NOT EXISTS patient (id serial PRIMARY KEY, name VARCHAR(50), sex VARCHAR(2), birthday VARCHAR(11), adress VARCHAR(50));");
    stmt->execute("CREATE TABLE IF NOT EXISTS medicine (id serial PRIMARY KEY, name VARCHAR(50), instruction VARCHAR(300), description VARCHAR(300), side_effects VARCHAR(300));");
    stmt->execute("CREATE TABLE IF NOT EXISTS appointments (id serial PRIMARY KEY, patient_name VARCHAR(50), doctor_name VARCHAR(50), date VARCHAR(300), place VARCHAR(300), simptoms VARCHAR(300), diagnose VARCHAR(50), instructions VARCHAR(300));");
    delete stmt;

    int number = enter_main();
    while (number != 1)
    {
        switch (number)
        {
        case(2):
            enter_patient(con);
            break;
        case(3):
            enter_medicine(con);
            break;
        case(4):
            side_effect_report(con);
            break;
        case(5):
            enter_appointmens(con);
            break;
        case(6):
            appointments_in_day(con);
            break;
        case(7):
            number_of_disease(con);
            break;
        case(8):
            call_statement(con);
            break;
        }
        number = enter_main();
    }

    delete con;
    return 0;
}
