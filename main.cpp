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
        cout << "Пожалуйста введите число для выбора операции.\n1 - чтобы выйти; 2 - чтобы добавить лекарство; 3 - чтобы сделать отчет о побочных эффектах; 4 - чтобы добавить пациента;\n5 - чтобы добавить новый прием; 6 - чтобы определить колличество больных выбранной болезнью;\n7 - чтобы определить число вызовов в выбранный день; 8 - чтобы сделать справку о вызовах.\n";
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
    cout << "Введите имя: ";
    getline(cin, str);
    pstmt->setString(1, str);
    cout << "Введите пол: ";
    getline(cin, str);
    pstmt->setString(2, str);
    cout << "Введите день рождения: ";
    getline(cin, str);
    pstmt->setString(3, str);
    cout << "Введите адрес: ";
    getline(cin, str);
    pstmt->setString(4, str);
    pstmt->execute();
    delete pstmt;
}

void enter_medicine(sql::Connection* con)
{
    sql::PreparedStatement* pstmt;
    string str, s;

    pstmt = con->prepareStatement("INSERT INTO medicine(name, instruction, description, side_effects) VALUES(?,?,?,?)");
    cout << "Введите название: ";
    getline(cin, str);
    pstmt->setString(1, str);
    cout << "Введите способ применения: ";
    getline(cin, str);
    pstmt->setString(2, str);
    cout << "Введите описание: ";
    getline(cin, str);
    pstmt->setString(3, str);
    cout << "Введите побочные эффекты: ";
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
        cout << "Введите название препарата: ";
        getline(cin, str);
        query1 = "SELECT COUNT(*) FROM medicine WHERE name =N'" + str + "'";
        stmt = con->createStatement();
        res = stmt->executeQuery(query1);
        while (res->next()) {
            buf = res->getString("COUNT(*)");
        }
        number = stoi(buf);
        if (!number)
        {
            cout << "Препарат отсутствует в базе данных.\n";
            cout << "Закончить выполение функции?(1 - Нет; 2 - Да)\n";
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
        cout << "Введите имя пациента: ";
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
            cout << "Такого пациента нет в базе данный.\nДобавить пациента в базу данных?(1 - Нет; 2 - Да)\n";
            number = enter();
            if (number == 2)
            {
                enter_patient(con);
                pstmt->setString(1, str);
            }
            else
            {
                cout << "Завершить выполнение функции?(1 - Нет; 2 - Да)\n";
                number = enter() - 1;
                if (number == 1) return;
            }
        }
    } while (!number);
    pstmt->setString(1, str);
    cout << "Введите имя доктора: ";
    getline(cin, str);
    pstmt->setString(2, str);
    cout << "Введите дату: ";
    getline(cin, str);
    pstmt->setString(3, str);
    cout << "Введите место: ";
    getline(cin, str);
    pstmt->setString(4, str);
    cout << "Введите симптомы: ";
    getline(cin, str);
    pstmt->setString(5, str);
    cout << "Введите диагноз: ";
    getline(cin, str);
    pstmt->setString(6, str);
    do {
        cout << "Введите препараты(один за другим): ";
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
            cout << "Этот препарат последний?(1 - Нет; 2 - Да)\n";
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
            cout << "Этот препарат отсутствует в базе данный.\nДобавить его в базу данных?(1 - Нет; 2 - Да)\n";
            number = enter();
            if (number == 2)
            {
                enter_medicine(con);
                medic += str;
                cout << "Этот препарат последний?(1 - Нет; 2 - Да)\n";
                number = (enter()) - 1;
                if (number == 0)
                {
                    medic += ", ";
                }
            }
            else
            {
                cout << "Завершить выполнение функции?(1 - Нет; 2 - Да)\n";
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

    cout << "Введите  дату: ";
    getline(cin, str);
    query1 = "SELECT COUNT(*) FROM appointments WHERE date='" + str + "' and place != 'hospital'";
    stmt = con->createStatement();
    res = stmt->executeQuery(query1);
    while (res->next()) {
        cout << "В этот день было совершено " << res->getString("COUNT(*)") << " вызовов.\n\n";
    }

    delete stmt;
    delete res;
}

void number_of_disease(sql::Connection* con)
{
    sql::Statement* stmt;
    sql::ResultSet* res;
    string str, query1;

    cout << "Введите болезнь: ";
    getline(cin, str);
    query1 = "SELECT COUNT(*) FROM appointments WHERE diagnose='" + str + "'";
    stmt = con->createStatement();
    res = stmt->executeQuery(query1);
    while (res->next()) {
        cout << "Число пациентов с этой болезнью равно " << res->getString("COUNT(*)") << "\n\n";
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
        cout << "Доктор " << res->getString("doctor_name") << " был на " << res->getString("COUNT(doctor_name)") << " вызовах.\n";
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
    setlocale(LC_ALL, "Russian");
    cout << "Введите пароль: ";
    getline(cin, password);
    try
    {
        driver = get_driver_instance();
        con = driver->connect(server, username, password);
    }
    catch (sql::SQLException e)
    {
        cout << "Нет подклычения к серверу. Ошибка: " << e.what() << endl;
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
            enter_medicine(con);
            break;
        case(3):
            side_effect_report(con);
            break;
        case(4):
            enter_patient(con);
            break;
        case(5):
            enter_appointmens(con);
            break;
        case(6):
            number_of_disease(con);
            break;
        case(7):
            appointments_in_day(con);
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