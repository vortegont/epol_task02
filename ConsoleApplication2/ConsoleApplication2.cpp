#include <windows.h>
#include <SQLAPI.h>
#include <conio.h>
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>




const SAString 
                sql_connection  = "DESKTOP-KGGFEM7\\SQLEXPRESS2@epol_tt2", // Пусть к базе данных
                sql_name        = "user01",                                // Имя для входа
                sql_password    = "123456";                                // Пароль для входа

using namespace std;

void list() 
{
    setlocale(LC_ALL, "Russian");
    cout << "Доступные команды:\n\nnew_task - Создать задачу;\ndelete_task - Удалить задачу со всеми связанными элементами;\nupdate_task - Обновить (или изменить) задачу;\nget_all_tasks - Получить список всех задач;\nget_tasks_name - Получить список всех задач с необходимым полем name;\nget_tasks_status - Получить список всех задач с необходимым полем status;\nget_tasks_date - Поиск задач по дате;\n";
    cout << "new_tasks_elem - Создать элемент;\ndelete_task_elem - Удалить элемент;\nupdate_task_elem - Обновить элемент;\nget_all_tasks_elem - Получить список всех элементов задачи;\nget_all_tasks_elem_value - Поиск элементов задачи с нужным значением;\nget_all_tasks_elem_id - Поиск всех элементов задачи с нужным ID задачи;\n";
    cout << "save - Сохранить таблицы в текстовый документ;\n\n";
}

string date_ptr(int date)
{
    int year = 1900; // С этого года идет отсчет
    int year_temp;
    int year_leap;
    int month = 0;
    int day;
    int days_temp;
    int day_remember;
    year_temp = date / 365; 
    year_leap = year_temp / 4; 
    days_temp = date - (365 * year_temp) - year_leap -1 ;
    if (year_temp % 4 == 0) { days_temp++; }
    year = 1900 + year_temp;

        while (days_temp > 0)
        {
            month++;
            day_remember = days_temp;
            day = day_remember;
            if (month == 1) { days_temp = days_temp - 31; if (days_temp == 0) { day = 31; break; } } // 31
            if (month == 2) 
            {
                if (day_remember == 29)
                {
                     month = 2;
                     day = 29; //60
                     break;
                }
                days_temp = days_temp - 28; // 59
            }
            if (month == 3) { days_temp = days_temp - 31; if (days_temp == 0) { day = 31;  break; }} // 90   или 91
            if (month == 4) { days_temp = days_temp - 30; if (days_temp == 0) { day = 30;  break; }} // 120  или 121
            if (month == 5) { days_temp = days_temp - 31; if (days_temp == 0) { day = 31;  break; }} // 151  или 152
            if (month == 6) { days_temp = days_temp - 30; if (days_temp == 0) { day = 30;  break; }} // 181  или 182
            if (month == 7) { days_temp = days_temp - 31; if (days_temp == 0) { day = 31;  break; }} // 212  или 213
            if (month == 8) { days_temp = days_temp - 31; if (days_temp == 0) { day = 31;  break; }} // 243  или 244
            if (month == 9) { days_temp = days_temp - 30; if (days_temp == 0) { day = 30;  break; }} // 273  или 274
            if (month == 10) { days_temp = days_temp - 31; if (days_temp == 0) { day = 31; break; }} // 304  или 305
            if (month == 11) { days_temp = days_temp - 30; if (days_temp == 0) { day = 30; break; }} // 334  или 335
            if (month == 12) { days_temp = days_temp - 31; if (days_temp == 0) { day = 31; break; }} // 365  или 366
    }
        return to_string(year) + "." + to_string(month) + "." + to_string(day);
        //return "";
}

void save_to_excel()
{
    SAString SA_param = "SELECT * FROM tasks";
    ofstream ExcelSaver;
    ExcelSaver.open("excel.txt", ios::app);
    SAConnection con;
    SACommand cmd(&con, _TSA(SA_param));
    con.Connect(_TSA(sql_connection), _TSA(sql_name), _TSA(sql_password), SA_SQLServer_Client);
    cmd.Execute();
    ExcelSaver << "ID Задачи" << '\t' << "Название" << '\t' << "Описание" << '\t' << "Дата начала" << '\t' << "Дата окончания" << '\t' << "Статус" << endl;
    while (cmd.FetchNext()) {
        long Id = cmd[1].asLong();
        SAString Name = cmd[2].asString();
        SAString Discription = cmd[3].asString();
        int Start_date = cmd[4].asDateTime();
        int End_date = cmd[5].asDateTime();
        string Start_date_str = date_ptr(Start_date);
        string End_date_str = date_ptr(End_date);
        SAString SD_SASTRING = Start_date_str.c_str();
        SAString ED_SASTRING = End_date_str.c_str();
        long Status = cmd[6].asLong();
        ExcelSaver << to_string(Id) << '\t' << Name.GetMultiByteChars() << '\t' << Discription.GetMultiByteChars() << '\t' << SD_SASTRING.GetMultiByteChars() << '\t' << ED_SASTRING.GetMultiByteChars() << '\t' << Status << endl;
    }
    con.Disconnect();
    ExcelSaver << endl;
    SAString SA_param2 = "SELECT * FROM tasks_elem";
    SAConnection con2;
    SACommand cmd2(&con2, _TSA(SA_param2));
    con2.Connect(_TSA(sql_connection), _TSA(sql_name), _TSA(sql_password), SA_SQLServer_Client);
    cmd2.Execute();
    ExcelSaver << "ID Элемента задачи" << '\t' << "Название" << '\t' << "Описание" << '\t' <<  "Значение" << endl;
    while (cmd2.FetchNext()) {
        long Id2 = cmd2[1].asLong();
        SAString Name2 = cmd2[2].asString();
        SAString Discription2 = cmd2[3].asString();
        SAString Value = cmd2[4].asString();
        ExcelSaver << to_string(Id2) << '\t' << Name2.GetMultiByteChars() << '\t' << Discription2.GetMultiByteChars() << '\t' << Value.GetMultiByteChars() << endl;
    }
    con2.Disconnect();

}

void sql_param(string param, int type)
{   
    SAString SA_param = param.c_str();
    if (param.find("SELECT") == 0) 
    {
        SAConnection con;
        SACommand cmd(&con,_TSA(SA_param));
        try {
            con.Connect(_TSA(sql_connection), _TSA(sql_name), _TSA(sql_password), SA_SQLServer_Client);
            cmd.Execute();

            if (type == 1) //type 1 = Список всех задач (+ поиск по полю name, status, date)
            {
                string temp = "Задачи не найдены";
                printf(" ID Задачи  |  Название   |  Описание  |  Дата начала  |  Дата окончания  | Статус |\n");
                while (cmd.FetchNext()) {
                    temp = "";
                    long Id = cmd[1].asLong();
                    SAString Name = cmd[2].asString();
                    SAString Discription = cmd[3].asString();
                    int Start_date = cmd[4].asDateTime();
                    int End_date = cmd[5].asDateTime();
                    string Start_date_str   = date_ptr(Start_date);
                    string End_date_str     = date_ptr(End_date);
                    SAString SD_SASTRING = Start_date_str.c_str();
                    SAString ED_SASTRING = End_date_str.c_str();
                    long Status = cmd[6].asLong();
                    printf("------------------------------------------------------------------------------------\n");
                    printf("  %d         |  %s      |  %s     |  %s    |  %s       |  %d     |\n", Id, Name, Discription, SD_SASTRING, ED_SASTRING, Status);
                    printf("------------------------------------------------------------------------------------\n");
                    }
                if (temp != "")
                {
                    system("cls");
                    cout << temp;
                }}

            if (type == 2) //type 2 = Список всех элементов задач (+ поиск по полю Value и ID)
            {
                string temp = "Задачи не найдены";
                printf(" ID Элемента задачи  |  Название   |  Описание  |  Значение |\n");
                while (cmd.FetchNext()) {
                    temp = "";
                    long Id = cmd[1].asLong();
                    SAString Name = cmd[2].asString();
                    SAString Discription = cmd[3].asString();
                    SAString Value = cmd[4].asString();
                    printf("------------------------------------------------------------------------------------\n");
                    printf("  %d         |  %s      |  %s     |  %s    |  %s   \n", Id, Name, Discription, Value);
                    printf("------------------------------------------------------------------------------------\n");
                }
                if (temp != "")
                {
                    system("cls");
                    cout << temp;
                }}
            }
        catch (SAException& x) {
            try
            {
                con.Rollback();
            }
            catch (SAException&) {}
            printf("%s\n", x.ErrText().GetMultiByteChars());
        }
    }
    else 
    {
        SAConnection con;
        SACommand cmd;
            try {
                con.Connect(_TSA(sql_connection), _TSA(sql_name), _TSA(sql_password), SA_SQLServer_Client);
                cmd.setConnection(&con);
                cmd.setCommandText(_TSA(SA_param));
                cmd.Execute();
                con.Commit();
                con.Disconnect();
            }
            catch (SAException& x) {
                try
                {
                    con.Rollback();
                }
                catch (SAException&){}
                printf( "%s\n", x.ErrText().GetMultiByteChars() );
        }
    }
}

int main()
{
    setlocale(LC_ALL, "russian");
    SAConnection con;

    try {
        con.Connect(_TSA(sql_connection), _TSA(sql_name), _TSA(sql_password), SA_SQLServer_Client);
        con.Disconnect();
    }
    catch (SAException& x) {
        con.Rollback();
        cout << "%s\n", x.ErrText().GetMultiByteChars();
    }

    string user_command, u_sql_text;
    cout << "Введите команду для работы с программой: "; 
    cin >> user_command;

    string id, name, description, start_date, end_date, status, SD_search, ED_search, value;

    while (user_command != "exit") 
    {
        if (user_command == "help") 
        {
            system("cls");
            list();
            user_command = "";
            cout << "\n\nВведите команду для работы с программой: ";
            cin >> user_command;
            user_command = "";
        }

        if (user_command == "new_task")
        {
            system("cls");
            cout << ("Введите через запятую:\n - Имя, \n - Описание задачи,\n - Дата начала (ФОРМАТ ГГГГ-ММ-ДД),\n - Дата окончания (ФОРМАТ ГГГГ-ММ-ДД),\n - Статус задачи (1 или 0):\n\n\n");
            cout << ("Имя: ");              cin >> name;        cout << ("\n");
            cout << ("Описание задачи: ");  cin >> description; cout << ("\n");
            cout << ("Дата начала: ");      cin >> start_date;  cout << ("\n");
            cout << ("Дата окончания: ");   cin >> end_date;    cout << ("\n");
            cout << ("Статус: ");           cin >> status;      cout << ("\n");

            sql_param("INSERT INTO tasks VALUES ('"+name+"', '"+description+"', CAST('" + start_date + "' AS DATE), CAST('" + end_date + "' AS DATE), " + status + ")", NULL);
            user_command = "";
        }

        if (user_command == "delete_task")
        {
            system("cls");
            cout << ("Выберите ID задачи для удаления:\n\n\n");
            cout << ("ID: "); cin >> id;
            system("cls");
            sql_param("DELETE FROM tasks_elem WHERE id =" + id, NULL);
            sql_param("DELETE FROM tasks WHERE id =" + id, NULL);
            user_command = "";
        }

        if (user_command == "update_task")
        {
            system("cls");
            cout << ("Выберите ID задачи для обновления данных:\n\n\n");
            cout << ("ID: "); cin >> id;
            system("cls");
            cout << ("Введите через запятую новые:\n - Имя, \n - Описание задачи,\n - Дата начала (ФОРМАТ ГГГГ-ММ-ДД),\n - Дата окончания (ФОРМАТ ГГГГ-ММ-ДД),\n - Статус задачи (1 или 0):\n\n\n");
            cout << ("Новое имя: ");              cin >> name;        cout << ("\n");
            cout << ("Новое описание задачи: ");  cin >> description; cout << ("\n");
            cout << ("Новая дата начала: ");      cin >> start_date;  cout << ("\n");
            cout << ("Новая дата окончания: ");   cin >> end_date;    cout << ("\n");
            cout << ("Новый статус: ");           cin >> status;      cout << ("\n");

            sql_param("UPDATE tasks SET name = '" + name + "', description = '" + description + "', start_date = CAST('" + start_date + "' AS DATE), end_date = CAST('" + end_date + "' AS DATE), status = " + status + "WHERE id = "+id, NULL);
            user_command = "";
        }

        if (user_command == "get_all_tasks")
        {
            system("cls");
            sql_param("SELECT * FROM tasks", 1);
            user_command = "";
        }

        if (user_command == "get_tasks_name")
        {
            system("cls");
            cout << "Введите значение поля ИМЯ(Название) для поиска необходимых строк:";
            cin >> name;
            cout << endl;
            sql_param("SELECT * FROM tasks WHERE name = '"+name+"'", 1);
            user_command = "";
        }
        if (user_command == "get_tasks_status")
        {
            system("cls");
            cout << "Введите значение поля СТАТУС для поиска необходимых строк:";
            cin >> status;
            cout << endl;
            sql_param("SELECT * FROM tasks WHERE status = '" + status + "'", 1);
            user_command = "";
        }

        if (user_command == "get_tasks_date")
        {
            system("cls");
            cout << "Введите дату, которая будет больше или равна необходимой Даты Начала (ФОРМАТ ГГГГ-ММ-ДД):";
            cin >> SD_search;
            cout << "\n ----------\n";
            cout << "Введите дату, которая будет больше или равна необходимой Даты Окончания (ФОРМАТ ГГГГ-ММ-ДД):";
            cin >> ED_search;
            system("cls");
            sql_param("SELECT * from tasks WHERE start_date >= '" + SD_search + "' and end_date <= '"+ED_search+"'", 1);
            user_command = "";
        }

        if (user_command == "new_tasks_elem")
        {
            system("cls");
            cout << ("Введите через запятую:\n - ID,\n - Имя,\n - Описание элемента задачи,\n - Значение:\n\n\n");
            cout << ("ID: ");               cin >> id;          cout << ("\n");
            cout << ("Имя: ");              cin >> name;        cout << ("\n");
            cout << ("Описание задачи: ");  cin >> description; cout << ("\n");
            cout << ("Дата начала: ");      cin >> value;       cout << ("\n");

            sql_param("INSERT INTO tasks_elem VALUES ("+id+",'" + name + "', '" + description +"'," + value + ")", NULL);
            user_command = "";
        }

        if (user_command == "delete_tasks_elem")
        {
            system("cls");
            cout << ("Выберите ID элемента задачи для удаления:\n\n\n");
            cout << ("ID: "); cin >> id;
            system("cls");
            sql_param("DELETE FROM tasks_elem WHERE id =" + id, NULL);
            user_command = "";
        }


        if (user_command == "update_tasks_elem")
        {
            system("cls");
            cout << ("Выберите ID элемента задачи для обновления данных:\n\n\n");
            cout << ("ID: "); cin >> id;
            system("cls");
            cout << ("Введите через запятую новые:\n - ID,\n - Имя,\n - Описание элемента задачи,\n - Значение:\n\n\n");
            cout << ("Новое ID: ");                             cin >> id;          cout << ("\n");
            cout << ("Новое имя: ");                            cin >> name;        cout << ("\n");
            cout << ("Новое описание элемента задачи: ");       cin >> description; cout << ("\n");
            cout << ("Новое значение: ");                       cin >> value;       cout << ("\n");

            sql_param("UPDATE tasks_elem SET id = '"+id+"', name = '" + name + "', description = '" + description + "', value = '"+value+"'", NULL);
            user_command = "";
        }

        if (user_command == "get_all_tasks_elem")
        {
            system("cls");
            sql_param("SELECT * FROM tasks_elem", 2);
            user_command = "";
        }

        if (user_command == "get_all_tasks_elem_value")
        {
            system("cls");
            cout << "Введите значение поля ЗНАЧЕНИЕ: ";
            cin >> value;
            system("cls");
            sql_param("SELECT * FROM tasks_elem WHERE value ='"+value+"'", 2);
            user_command = "";
        }

        if (user_command == "get_all_tasks_id_id")
        {
            system("cls");
            cout << "Введите значение поля ID задачи: ";
            cin >> id;
            system("cls");
            sql_param("SELECT * FROM tasks_elem WHERE id ='" + id + "'", 2);
            user_command = "";
        }

        if (user_command == "save") 
        {
            system("cls");
            save_to_excel();
            user_command = "";
        }

        else 
        {
            if (user_command == "") 
            {
                cout << "\n\nВведите команду для работы с программой: ";
                cin >> user_command;
            }
            else 
            {
                system("cls");
                cout << "Неправильная команда: "+ user_command +";\n\nВведите команду для работы с программой: ";
                cin >> user_command;
            }
        }
    }
    return 0;
}

