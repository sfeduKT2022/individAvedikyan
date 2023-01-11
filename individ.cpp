/*
	Для заданного номера телефона
	сформировать список тех предоставленных услуг,
	суммарная стоимость
	которых не меньше заданной,
	в течение 3-х последних дней
	(от момента запуска программы).
*/

#include <fstream>
#include <ctime>

#define CLIENTS_FILE "clients.txt"
#define SERVICES_FILE "services.txt"
#define USEDSERVICES_FILE "usedservices.txt"

using namespace std;

struct DateTime {
	int day = 0;
	int month = 0;
	int year = 0;
	int hour = 0;
	int minute = 0;
	int second = 0;
};

struct Client {
	char fullname[300];
	unsigned long long int number;
	DateTime begin;
	DateTime end;
	int duty;
	int credit;
};

struct Service {
	char name[300];
	int code;
	double price;
	char interval[100];
};

struct UsedService {
	unsigned long long int number;
	int code;
	DateTime date;
	int count;
};

// Список клиентов
Client clients[50];
int clients_count = 0;
// Список услуг
Service services[50];
int services_count = 0;
// Список использованных услуг
UsedService usedservices[50];
int usedservices_count = 0;


void load();
void report();

int main() {
	load();
	report();
	return 0;
}
void readDate(std::ifstream& s, DateTime& d, char sep) {
	// День 
	char buffer[20];
	s.getline(buffer, 20, '.');
	d.day = atoi(buffer); // atoi - получить int из строки
	// Месяц
	s.getline(buffer, 20, '.');
	d.month = atoi(buffer);
	// Год
	s.getline(buffer, 20, sep);
	d.year = atoi(buffer);
}

void load() {
	ifstream fin;
	fin.open(CLIENTS_FILE);
	char buffer[20];

	// Загрузка клиентов
	for (clients_count = 0; fin.good(); clients_count++) {
		// ФИО
		fin.getline(clients[clients_count].fullname, 300, ',');
		// Номер телефона
		fin.getline(buffer, 20, ',');
		clients[clients_count].number = atoll(buffer); // atoll - получить long long int из строки
		// Дата заключения договора
		readDate(fin, clients[clients_count].begin, ',');
		// Дата окончания договора
		readDate(fin, clients[clients_count].end, ',');
		// Сумма задолжности
		fin.getline(buffer, 20, ',');
		clients[clients_count].duty = atoi(buffer);
		// Допустимый кредит
		fin.getline(buffer, 20);
		clients[clients_count].credit = atoi(buffer);
	}
	fin.close();
	fin.open(SERVICES_FILE);
	// Загрузка услуг
	for (services_count = 0; fin.good(); services_count++) {
		// Название
		fin.getline(services[services_count].name, 300, ',');
		// Код услуги
		fin.getline(buffer, 20, ',');
		services[services_count].code = atoi(buffer);
		// Цена
		fin.getline(buffer, 20, ',');
		services[services_count].price = atof(buffer); // atof - получить double из строки
		// Интервал
		fin.getline(services[services_count].interval, 100);
	}
	fin.close();
	fin.open(USEDSERVICES_FILE);
	// Загрузка использованных услуг
	for (usedservices_count = 0; fin.good(); usedservices_count++) {
		// Номер телефона
		fin.getline(buffer, 20, ',');
		usedservices[usedservices_count].number = atoll(buffer);
		// Код услуги
		fin.getline(buffer, 20, ',');
		usedservices[usedservices_count].code = atoi(buffer);
		// Дата использования
		// День 
		fin.getline(buffer, 20, '.');
		usedservices[usedservices_count].date.day = atoi(buffer);
		// Месяц
		fin.getline(buffer, 20, '.');
		usedservices[usedservices_count].date.month = atoi(buffer);
		// Год
		fin.getline(buffer, 20, ' ');
		usedservices[usedservices_count].date.year = atoi(buffer);
		// Часы
		fin.getline(buffer, 20, ':');
		usedservices[usedservices_count].date.hour = atoi(buffer);
		// Минуты
		fin.getline(buffer, 20, ':');
		usedservices[usedservices_count].date.minute = atoi(buffer);
		// Секунды
		fin.getline(buffer, 20, ',');
		usedservices[usedservices_count].date.second = atoi(buffer);
		// Количество использований
		fin.getline(buffer, 20);
		usedservices[usedservices_count].count = atoi(buffer);
	}
	fin.close();
}
/*
	Суммарная стоимость
	номер телефона
*/
void report() {
	std::ifstream fin;
	fin.open("Param.txt");
	double sum;
	long long int number;
	// Чтение параметров

	fin >> sum >> number;
	std::ofstream fout;
	fout.open("Report.txt");
	// Текущее время в unix time
	time_t unixnow = time(0);
	// Время в unix time 3 дня назад
	time_t unixbg = unixnow - 60 * 60 * 24 * 3;
	time_t unixtemp;
	tm temp;
	// Фикс времени 3 дня назад
	localtime_s(&temp, &unixbg);
	unixbg -= 3600 * temp.tm_hour + 60 * temp.tm_min + temp.tm_sec;
	// Прохождение по списку услуг
	for (int i = 0; i < services_count; i++) {
		double tsum = 0;
		// Прохождение по списку использованных услуг
		for (int j = 0; j < usedservices_count; j++) {
			if (services[i].code == usedservices[j].code && number == usedservices[j].number){
				// Загрузка времени использования услуги в unix time
				temp.tm_year = usedservices[j].date.year - 1900;
				temp.tm_mon = usedservices[j].date.month - 1;
				temp.tm_mday = usedservices[j].date.day;
				temp.tm_hour = usedservices[j].date.hour;
				temp.tm_min = usedservices[j].date.minute;
				temp.tm_sec = usedservices[j].date.second;
				unixtemp = mktime(&temp);
				// Проверка на временной диапазон
				if (unixbg <= unixtemp && unixtemp <= unixnow) {
					tsum += services[i].price * (double)usedservices[j].count;
				}
			}
		}
		// Если суммма потраченная на услугу больше
		// чем минимальная
		if (tsum >= sum) {
			fout << services[i].name << '\n';
		}
	}
}