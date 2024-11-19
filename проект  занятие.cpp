#include <iostream>
#include <Windows.h>
#include <string>
#include <fstream>
using namespace std;

HANDLE hConsole;

class Player
{
public:
    Player(const string& name = "");
    string GetName() const;
    Player* GetNext() const;
    void SetNext(Player* next);
    void SetScore(int score);
    int GetScore() const;
    int GetUniqueId() const; 
    int GetWins() const { return m_Wins; }
    int GetLosses() const { return m_Losses; }
    int GetErrors() const { return m_Errors; }

private:
    string m_Name;
    Player* m_pNext;
    int m_Score;
    int m_UniqueId; 
    static int s_NextUniqueId;
    int m_Wins;
    int m_Losses;
    int m_Errors;
};

int Player::s_NextUniqueId = 0;

Player::Player(const string& name) :
    m_Name(name),
    m_pNext(NULL),
    m_Score(0),
    m_UniqueId(s_NextUniqueId++)
{}

int Player::GetUniqueId() const {
    return m_UniqueId;
}

string Player::GetName() const {
    return m_Name;
}

Player* Player::GetNext() const {
    return m_pNext;
}

void Player::SetNext(Player* next) {
    m_pNext = next;
}

void Player::SetScore(int score) {
    m_Score = score;
}

int Player::GetScore() const {
    return m_Score;
}

class Lobby
{
    friend ostream& operator<<(ostream& os, const Lobby& aLobby);
public:
    Lobby(const string& playerName);
    ~Lobby();
    void AddPlayer();
    void RemovePlayer();
    void Clear();
    void DisplayPlayers() const;
    void SortPlayersByScore();
    void FindPlayerByName(const string& name) const;
    void AddScoreToPlayer(const string& name, int score);
    void AddPlayerByName(const string& name);
    void SaveData() const;
    void LoadData();
    void Instriction();
    void ShowLobbyStatistics();
    void DeletePlayerByName(const string& name) {
        if (m_pHead == nullptr) {
            cout << "Лобби пустое!\n";
            return;
        }

        if (m_pHead->GetName() == name) {
            Player* temp = m_pHead;
            m_pHead = m_pHead->GetNext();
            delete temp;
            return;
        }

        Player* current = m_pHead;
        Player* previous = nullptr;

        while (current != nullptr && current->GetName() != name) {
            previous = current;
            current = current->GetNext();
        }

        if (current == nullptr) {
            cout << "Игрок не найден!\n";
            return;
        }

        previous->SetNext(current->GetNext());
        delete current;
    }

    void SaveLobbyToFile(const string& fileName) const {
        ofstream file(fileName + ".txt");
        if (file.is_open()) {
            Player* pIter = m_pHead;
            while (pIter != nullptr) {
                file << pIter->GetName() << " " << pIter->GetScore() << endl;
                pIter = pIter->GetNext();
            }
            file.close();
            cout << "Лобби успешно сохранено в файл " << fileName << ".txt.\n";
        }
        else {
            cout << "Ошибка при сохранении лобби в файл.\n";
        }
    }

    void LoadLobbyFromFile(const string& fileName) {
    Clear();
    ifstream file(fileName + ".txt"); // append.txt extension automatically
    if (file.is_open()) {
        string name;
        int score;
        while (file >> name >> score) {
            AddPlayerByName(name);
            Player* pIter = m_pHead;
            while (pIter->GetNext()!= nullptr) {
                pIter = pIter->GetNext();
            }
            pIter->SetScore(score);
        }
        file.close();
        cout << "Лобби успешно загружено из файла " << fileName << ".txt.\n";
    }
    else {
        cout << "Ошибка при загрузке лобби из файла.\n";
    }
}
private:
    Player* m_pHead;
    string m_PlayerName;
    int m_numPlayers; 
    int m_totalScore; 
    int m_errors; 
};


Lobby::Lobby(const string& playerName) :
    m_pHead(0),
    m_PlayerName(playerName),
    m_numPlayers(0),
    m_totalScore(0), 
    m_errors(0) 
{
    try {
        LoadData();
    }
    catch (const exception& e) {
        cerr << "Error: unable to load data." << endl;
    }
}

Lobby::~Lobby() {
    try {
        SaveData();
    }
    catch (const exception& e) {
        cerr << "Error: unable to save data." << endl;
    }
    Clear();
}

void Lobby::AddPlayer() {
    cout << "Пожалуйста, назовите нового игрока: ";
    string name;
    cin >> name;
    try {
        Player* pNewPlayer = new Player(name);
        if (m_pHead == 0) {
            m_pHead = pNewPlayer;
        }
        else {
            Player* pIter = m_pHead;
            while (pIter->GetNext()!= 0) {
                pIter = pIter->GetNext();
            }
            pIter->SetNext(pNewPlayer);
        }
    } catch (const bad_alloc& e) {
        cerr << "Error: unable to allocate memory for new player." << endl;
    }
}

void Lobby::RemovePlayer() {
    if (m_pHead == 0) {
        cout << "Игровое лобби пустое!\n";
        return;
    }
    try {
        Player* pTemp = m_pHead;
        m_pHead = m_pHead->GetNext();
        delete pTemp;
    }
    catch (const exception& e) {
        cerr << "Error: unable to remove player." << endl;
    }
}

void Lobby::ShowLobbyStatistics() {
    std::ofstream outFile(m_PlayerName + ".txt");
    if (!outFile) {
        std::cerr << "Error: unable to open file for saving statistics." << std::endl;
        return;
    }

    int totalPlayers = 0;
    int totalWins = 0;
    int totalLosses = 0;
    int totalErrors = 0;

    Player* pIter = m_pHead;
    while (pIter != nullptr) {
        totalPlayers++;
        totalWins += pIter->GetWins();
        totalLosses += pIter->GetLosses();
        totalErrors += pIter->GetErrors();
        pIter = pIter->GetNext();
    }

    outFile << totalPlayers << "\n";
    outFile << totalWins << "\n";
    outFile << totalLosses << "\n";
    outFile << totalErrors << "\n";

    outFile.close();

    std::cout << "\nЛобби статистика:\n";
    std::cout << "Количество игроков: " << totalPlayers << "\n";
    std::cout << "Общий балл: " << totalWins * 1000 + totalLosses * 500 + totalErrors * 10 << "\n";
    std::cout << "Количество выигрышей: " << totalWins << "\n";
    std::cout << "Количество проигрышей: " << totalLosses << "\n";
    std::cout << "Количество ошибок: " << totalErrors << "\n";

    std::ifstream inFile(m_PlayerName + ".txt");
    if (!inFile) {
        std::cerr << "Error: unable to open file for loading statistics." << std::endl;
        return;
    }

    int numPlayers, numWins, numLosses, numErrors;
    inFile >> numPlayers;
    inFile >> numWins;
    inFile >> numLosses;
    inFile >> numErrors;

    if (numPlayers < 0 || numWins < 0 || numLosses < 0 || numErrors < 0) {
        std::cerr << "Error: invalid statistics data." << std::endl;
        return;
    }

    m_numPlayers = numPlayers;
    m_totalScore = numWins * 1000 + numLosses * 500 + numErrors * 10;
    m_errors = numErrors;

    inFile.close();
}

void Lobby::Clear() {
    while (m_pHead != 0) {
        try {
            RemovePlayer();
        }
        catch (const exception& e) {
            cerr << "Error: unable to clear lobby." << endl;
        }
    }
}

void Lobby::Instriction() {
    cout << "Данная программа симулирует ожидание игрока в игровом лоббиn\n";
    cout << "user может добавлять игроков в своё лобби, отправлють их в игру или полностью очистить лобби\n admin может добавлять очки игрокам для бонуса в самой игре, находиить игрока в лобби или сортировать игроков по очкам\n";
    cout << "Что бы попасть на ветвь администратора в пароле введите admin\n";
    cout << "Для сохранения ваших данных в конце работы выходите вводя 0, а не просто закрывая так ваши данные не сохраняться\n";
    cout << "Функция сортировки ещё свежая и она может некорректно работать или вовсе не работать\n";
    cout << "Ваши данные сохраняются в файле с вашим ником в игре так что запомните свой ник\n";
    cout << "1 функция добавляет нового игрока снизу, а втроя отправляет игрока в игру самого первого\n";
    cout << "6 функция у админа начисляет игроку очки в формате: 1 ввод ник; 2 ввод количество очков\n";
    cout << "Приятной игры\n";
}

void Lobby::DisplayPlayers() const {
    Player* pIter = m_pHead;
    cout << "\nСейчас в игровом лобби\n";
    if (pIter == 0) {
        cout << "В лобби никого нет\n";
    }
    else {
        while (pIter != 0) {
            cout << pIter->GetName() << " - " << pIter->GetScore() << endl;
            pIter = pIter->GetNext();
        }
    }
}

void Lobby::SortPlayersByScore() {
    if (m_pHead == 0) return;
    Player* pIter = m_pHead;
    Player* pMin;
    int minScore;
    while (pIter != 0) {
        pMin = pIter;
        minScore = pIter->GetScore();
        Player* pNext = pIter->GetNext();
        while (pNext != 0) {
            if (pNext->GetScore() < minScore) {
                pMin = pNext;
                minScore = pNext->GetScore();
            }
            pNext = pNext->GetNext();
        }
        if (pMin != pIter) {
            int tempScore = pIter->GetScore();
            pIter->SetScore(pMin->GetScore());
            pMin->SetScore(tempScore);
        }
        pIter = pIter->GetNext();
    }
}

void Lobby::FindPlayerByName(const string& name) const {
    Player* pIter = m_pHead;
    while (pIter != 0) {
        if (pIter->GetName() == name) {
            cout << "Игрок " << name << " найден!\n";
            return;
        }
        pIter = pIter->GetNext();
    }
    cout << "Игрок " << name << " не найден\n";
}

void Lobby::AddScoreToPlayer(const string& name, int score) {
    Player* pIter = m_pHead;
    while (pIter != 0) {
        if (pIter->GetName() == name) {
            pIter->SetScore(pIter->GetScore() + score);
            cout << "Очки игрока " << name << " обновлены до " << pIter->GetScore() << endl;
            return;
        }
        pIter = pIter->GetNext();
    }
    cout << "Игрок " << name << " не найден\n";
}

void Lobby::SaveData() const {
    ofstream file(m_PlayerName + ".txt");
    if (file.is_open()) {
        Player* pIter = m_pHead;
        while (pIter != 0) {
            file << pIter->GetName() << " " << pIter->GetScore() << endl;
            pIter = pIter->GetNext();
        }
        file.close();
    }
    else if (!file.is_open()) {
        cerr << "Error: unable to open file for saving data." << endl;
        return;
    }
}

void Lobby::AddPlayerByName(const string& name) {
    Player* pNewPlayer = new Player(name);
    if (m_pHead == 0) {
        m_pHead = pNewPlayer;
    }
    else {
        Player* pIter = m_pHead;
        while (pIter->GetNext() != 0) {
            pIter = pIter->GetNext();
        }
        pIter->SetNext(pNewPlayer);
    }
}

void Lobby::LoadData() {
    ifstream file(m_PlayerName + ".txt");
    if (file.is_open()) {
        string name;
        int score;
        while (file >> name >> score) {
            AddPlayerByName(name);
            Player* pIter = m_pHead;
            while (pIter->GetNext() != 0) {
                pIter = pIter->GetNext();
            }
            pIter->SetScore(score);
        }
        file.close();
    }
    else if (!file.is_open()) {
        cerr << "Ошибка данный пользователь не найден создаем новый аккаунт с таким именем" << endl;
        return;
    }
}

ostream& operator<<(ostream& os, const Lobby& aLobby) {
    aLobby.DisplayPlayers();
    return os;
}

string getAdminPassword() {
    ifstream passwordFile("admin_password.txt");
    string password;
    if (passwordFile.is_open()) {
        getline(passwordFile, password);
        passwordFile.close();
    }
    else {
        password = "admin";
        ofstream newPasswordFile("admin_password.txt");
        newPasswordFile << password;
        newPasswordFile.close();
    }
    return password;
}

void setAdminPassword(const string& newPassword) {
    ofstream passwordFile("admin_password.txt");
    passwordFile << newPassword;
    passwordFile.close();
}



int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    SetConsoleTextAttribute(hConsole, 2);

    cout << "Введите свое игровое имя: ";
    string playerName;
    cin >> playerName;

    Lobby myLobby(playerName);
    int choice;
    string name;
    string fileName;
    string newPassword;
    int score;
    cout << "Приветствую тебя " << playerName << " в игре  //Лобби ожидания// \n";
    string adminPassword = getAdminPassword();
    cout << "Введите пароль администратора: ";
    string inputPassword;
    cin >> inputPassword;
    bool isAdmin = (inputPassword == adminPassword);
    do {
        cout << myLobby;
        cout << "\nЛобби ожидания\n";
        cout << "-1 - Краткая Инструкция\n";
        cout << "0 - Выйти из программы\n";
        cout << "1 - Добавить игрока в очередь\n";
        cout << "2 - Отправить игрока в игру\n";
        cout << "3 - Полностью очистить лобби\n";
        cout << "4 - Вывести статистику лобби\n";
        if (isAdmin) {
            cout << "5 - Сортировка игроков по очкам\n";
            cout << "6 - Найти игрока по имени\n";
            cout << "7 - Начислить очки игроку\n";
            cout << "8 - Удалить игрока по имени\n";
            cout << "9 - Сохранить лобби в файл\n";
            cout << "10 - Загрузить лобби из файла\n";
            cout << "11 - Изменить пароль администратора\n";
        }
        cout << endl << "Выберите: ";
        cin >> choice;
        switch (choice) {
        case -1: myLobby.Instriction(); break;
        case 0: cout << "До новых встреч\n"; break;
        case 1: myLobby.AddPlayer(); break;
        case 2: myLobby.RemovePlayer(); break;
        case 3: myLobby.Clear(); break;
        case 5: myLobby.SortPlayersByScore(); break;
        case 4:
            myLobby.ShowLobbyStatistics();
            break;
        case 6:
            cout << "Введите имя игрока: ";
            cin >> name;
            myLobby.FindPlayerByName(name);
            break;
        case 7:
            cout << "Введите имя игрока и количество очков: ";
            cin >> name >> score;
            myLobby.AddScoreToPlayer(name, score);
            break;
        case 8:
            cout << "Введите имя игрока: ";
            cin >> name;
            myLobby.DeletePlayerByName(name);
            break;
        case 9:
            cout << "Введите имя файла для сохранения лобби: ";
            cin >> fileName;
            myLobby.SaveLobbyToFile(fileName);
            break;
        case 10:
            cout << "Введите имя файла для загрузки лобби: ";
            cin >> fileName;
            myLobby.LoadLobbyFromFile(fileName);
            break;
        case 11:
            cout << "Введите новый пароль администратора: ";
            cin >> newPassword;
            setAdminPassword(newPassword);
            cout << "Пароль администратора изменен\n";
            break;
        default: cout << "Ошибка. Такого номера нет\n";
        }
    } while (choice != 0);
    return 0;
}


//2. Статистика и рейтинг
//
//Вы можете добавить в Playerкласс переменные - члены для хранения статистики, такой как победы, поражения и рейтинг.Также можно добавить метод расчета рейтинга на основе статистики.
//
//
//4. Режимы игры
//
//Вы можете добавить дополнительные игровые режимы, такие как режим турнира, командный режим и рейтинговый режим.Каждый режим может иметь свой набор правил и логику.
//
//5. Пользовательский интерфейс
//
//Вы можете рассмотреть возможность использования библиотеки графического интерфейса, такой как Qt или wxWidgets, для создания более удобного интерфейса.
//
//6. Безопасность
//
//Вы можете добавить дополнительные функции безопасности, такие как хеширование и добавление паролей, а также реализацию механизмов аутентификации и авторизации.
//
//8. Ведение журнала и аналитика
//
//Вы можете добавить дополнительные функции ведения журнала и аналитики для отслеживания поведения игроков и игровой статистики.
//
//9. Масштабируемость
//
//Вы можете спроектировать архитектуру более масштабируемой, используя шаблоны проектирования, такие как шаблон наблюдателя и шаблон фабрики.
//
//10. Тестирование и отладка
//
//Вы можете добавить дополнительные механизмы тестирования и отладки, такие как модульное тестирование, интеграционное тестирование и инструменты отладки.