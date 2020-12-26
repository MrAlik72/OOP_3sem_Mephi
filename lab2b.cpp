#include <iostream>
#include <stdio.h>
/*b)	сама таблица хранится в основной памяти, а информация, относящаяся к элементу таблицы, хранится во внешней памяти 
(используется двоичный файл произвольного доступа), причем она записывается в файл сразу же при выполнении операции включения в таблицу. Имя файла вводится по запросу из программы.
 По завершении работы элементы таблицы записываются во внешнюю память, а в начале нового сеанса таблица строится на основе информации во внешней памяти.*/

using namespace std;

//Просматриваемая таблица
struct Item{
	int key;        //ключ элемента
   int release;    //номер версии элемента
	//char *info;   //указатель на информацию (не используется, информация хранится в файле)
   int InfoIndex;  //смещение информации в файле
   int InfoLength; //длина информации в файле
	Item *next;     //указатель на следующий элемент
};

//Наша таблица
Item *ptab = NULL;

void Insert(char *); //Организационное занесение элемента в таблицу
int Push(int, int, char *, char *); //Непосредственное занесение элемента в таблицу

void Delete(char *); //Организационное удаление элемента из таблицы
int del2(int, int); //Непосредственное удаление элемента из таблицы

void View(char *); //Просмотр таблицы

void Quit(char *); //выход

//Другие функции
int input(); //Проверка ввода данных
int search(int); //Поиск последней версии элемента по ключу
char * delInfo(char *); //Очищение памяти у информации
char * dupl(char *in); //Выделение памяти для информации

void (*func[])(char *) = {Quit, Insert, Delete, View}; //Массив указателей на функции

//Функции по работе с файлами
void WriteInfo(Item * tmp, char * info, char * filename); //Запись строки информации в файл
char * ReadInfo(Item * cur, char * filename); //Чтение строки информации из файла
int PushFromFile(int, int, int, int); //Занесение элемента из файла в таблицу
char * ReadTable(); //Первоначальное чтение из таблицы
void WriteRow(Item * tmp, FILE * file, char * filename); //Запись строки элемента в файл при выходе
void WriteTable(char * filename); //Запись таблицы в файл при выходе

//Пункты меню
char * Mes[] = {"1. Insert element in table\n", "2. Delete element from table\n", "3. Show all table\n", "4. Quit\n"};

int MesCount = sizeof(Mes)/sizeof(Mes[0]); //Количество пунктов

//Первоначальное меню
int Menu(){
  int answer = 1;
  cout << "Main Menu" << endl;

  //----------Начало: выбор действия----------
  do{
    for(int j = 0; j < MesCount; j++)
      cout << Mes[j];
    cout << "Make your choice: ";
    answer = input();
  }while(answer < 1 || answer > MesCount);
  //----------Конец: выбор действия----------

  return (answer % MesCount);
}

int main(){
  //setlocale(LC_ALL, "Russian");
  int answer = 1;
  cout << "Laboratory 2b, alternative 14" << endl;

  //----------Начало: ввод имени файла----------
  char * filename;
  filename = ReadTable();
  //----------Конец: ввод имени файла----------

  do{
    answer = Menu();
    func[answer](filename);
  }while(answer);

  return 0;
}

int Push(int k, int v, char * in, char * filename){
  Item *tmp;
  tmp = new Item;

  //Занесение элемента в таблицу
  tmp->key = k;
  tmp->release = v;
  //tmp->info = dupl(in);
  WriteInfo(tmp, dupl(in), filename); //Запись информации в файл; смещение в файле и длина идёт в таблицу
  tmp->next = ptab;
  ptab = tmp;
  return 1;
}

void Insert(char * filename){
  int key, vers;
  char buf[80];

  //----------Начало: ввод ключа элемента----------
  cout << "Enter key: ";
  key = input();
  vers = search(key) + 1;
  //----------Конец: ввод ключа элемента----------

  cout << "Enter information: " << endl;
  cin.ignore(80, '\n');
  cin.getline(buf, 80);

  if(Push(key, vers, buf, filename))
    cout << "Element successfully inserted!" << endl;
}

int del2(int k, int v){
  Item *cur, **pptr = &ptab;
  int kolvoud=0;
  if(v)
    //Если удаляем по ключу и версии элемента
    while(*pptr){
		if(((*pptr)->key == k) && ((*pptr)->release == v)){
        cur = *pptr;
        *pptr = cur->next;
        //delInfo(cur->info);
        delete cur;
        return 1;
		}
		pptr = &(*pptr)->next;
    }
  else{
    //Если удаляем все версии элемента по ключу
    while(*pptr){
		if((*pptr)->key == k){
        cur = *pptr;
        *pptr = cur->next;
        //delInfo(cur->info);
		  delete cur;
		  kolvoud = kolvoud + 1;
		}
      else
		  pptr = &(*pptr)->next;
	 }
    return kolvoud;
  }
  return 0;
}

void Delete(char * filename){
  int key, vers, kdel;
  char buf[80];

  //----------Начало: ввод ключа удаляемого элемента----------
  cout << "Enter key of deleting element: ";
  key = input();
  //----------Конец: ввод ключа удаляемого элемента----------

  //----------Начало: ввод версии удаляемого элемента----------
  cout << "Enter version of deleting element (0 - all version): ";
  vers = input();
  //----------Конец: ввод версии удаляемого элемента----------

  if((kdel = del2(key, vers))>0)
    cout << "Successfully deleted " << kdel << " elements!" << endl;
  else
    cout << "No elements found for you request." << endl;
}

void View(char * filename){
  Item *cur = ptab;

  //----------Начало: проверка на пустоту таблицы----------
  if(!cur){
    cout << "The table is empty!" << endl;
    return;
  }
  //----------Конец: проверка на пустоту таблицы----------

  cout << " key \t| vers \t|  information\n===================\n";
  for(; cur; cur = cur->next){
    char *buf = ReadInfo(cur, filename);
    cout << cur-> key << "\t| " << cur->release << "\t|  " << buf << endl;
    delete[] buf;
  }
}

void Quit(char * filename){
  Item *tmp;
  WriteTable(filename); //Запись таблицы в файл
  //Очищение таблицы по выходу
  while(ptab){
    tmp = ptab;
    //delInfo(ptab->info);
    ptab = ptab->next;
    delete tmp;
  }
}

int input(){
  int a = 0;
  while(!(cin >> a)){
    if(cin.eof()){
      cout << "Break!" << endl;
      break;
    }
    cin.clear();
    cin.ignore();
    cout << "Clear and ignore!" << endl;
  }
  return a;
}

int search(int k){
  Item *cur;
  int tmpvers = 0;

  //Поиск последней версии элемента по ключу
  //Если элемента с переданным ключом нет, то возвращаем ноль
  for(cur = ptab; cur; cur = cur->next)
    if((cur->key == k) && (cur->release > tmpvers))
      tmpvers = cur->release;
  return tmpvers;
}

char * dupl(char *in){
  char *res = new char [strlen(in) + 1];
  strcpy(res, in);
  return res;
}

char * delInfo(char * in){
  delete[] in;
  return NULL;
}

void WriteInfo(Item * tmp, char * info, char * filename){

  //Проверка на заполненность поля информации
  if(info == NULL){
    tmp->InfoLength = -1;
    return;
  }

  //Открытие файла на добавление
  FILE * file = fopen(filename, "ab+");

  fseek(file, 0, SEEK_END); //курсор в конец файла
  tmp->InfoIndex = ftell(file); //Записываем положение конца файла
  tmp->InfoLength = strlen(info); //Записываем длину информации

  //Пишем в файл строку
  fwrite(info, sizeof(char), tmp->InfoLength, file);

  fclose(file); // Закрываем файл
}

char * ReadInfo(Item * cur, char * filename){

  //Проверка на заполненность поля индекса
  if(cur->InfoIndex < 0)
    return NULL;

  FILE * file = fopen(filename, "rb"); //Открываем файл в двоичном режиме для чтения

  char buf[80];
  fseek(file, cur->InfoIndex, SEEK_SET); //Переносим курсор на нужную нам строку
  fread(buf, sizeof(char), cur->InfoLength, file); //Читаем строку
  buf[cur->InfoLength] = 0; //Вставляем "конец строки"

  char * info = dupl(buf); //Выделение памяти под строку

  fclose(file); // Закрываем файл
  return info;
}

int PushFromFile(int k, int v, int InfoLength, int InfoIndex){
  Item *tmp;
  tmp = new Item;

  //Занесение элемента в таблицу
  tmp->key = k;
  tmp->release = v;
  tmp->InfoLength = InfoLength;
  tmp->InfoIndex = InfoIndex;
  tmp->next = ptab;

  ptab = tmp;
  return 1;
}

char * ReadTable(){

  FILE * file;
  char filename[80];
  char * nFilename;

  do{
    int mode;

    //----------Начало: выбор режима работы с файлом----------
    do{
      cout << "What do you want to do?" << endl;
      cout << "0 - read from file to table" << endl;
      cout << "1 - create new file" << endl;
      cout << "Choose mode: " << endl;
      mode = input();
    }while(mode < 0 || mode > 1);
    //----------Конец: выбор режима работы с файлом----------

    cin.ignore(80, '\n');

    //----------Начало: ввод имени файла----------
    cout << "Enter filename: ";
    cin.getline(filename, 80);
    nFilename = dupl(filename);
    //----------Конец: ввод имени файла----------

    cout << "Filename: " << nFilename << endl;

    //----------Начало: на основании выбора чтение/создание файла----------
    if (!mode)
      file = fopen(nFilename, "rb+"); //Попытаемся открыть файл
    else
      file = fopen(nFilename, "wb+");  //Попытается записать файл
    //----------Конец: на основании выбора чтение/создание файла----------

    //----------Начало: проверка, что файл открыт для чтения/записи----------
    if (file == NULL)
      perror(nFilename);
    else
      if (mode){
        cout << "New file " << nFilename << " successfully created!" << endl;
        fclose(file);

        //Если файл создался, то читать нечего, возвращаемся в главную процедуру
        return nFilename;
      }

    //----------Конец: проверка, что файл открыт для чтения/записи----------

  }while(file == NULL);
  //----------Конец: проверка, что файл открыт для чтения/записи----------

  cout << "File opened!" << endl;
  fseek(file, 0, SEEK_END); //Курсор в конец файла
  int maxIndex = ftell(file); //Получаем положение конца файла

  rewind(file); //Возвращение к началу файла

  while(maxIndex > ftell(file)){

    int bufKey;
    fread(&bufKey, sizeof(int), 1, file); //Чтение ключа элемента

    int bufRelease;
    fread(&bufRelease, sizeof(int), 1, file); //Чтение версии элемента

    int bufLength;
    fread(&bufLength, sizeof(int), 1, file); //Чтение длины элемента

    int bufInfoIndex;
    bufInfoIndex = ftell(file);

    char bufInfo[80];
    fread(bufInfo, sizeof(char), bufLength, file); //Чтение информации

    //bufInfo[bufLength] = 0;
    PushFromFile(bufKey, bufRelease, bufLength, bufInfoIndex); //Помещение этого всего в файл

  }

  cout << "Table successfully created!" << endl;
  fclose(file);
  return nFilename;

}

void WriteRow(Item * tmp, FILE * file, char * filename){

  fwrite(&(tmp->key), sizeof(int), 1, file); //Запись ключа элемента
  fwrite(&(tmp->release), sizeof(int), 1, file); //Запись версии элемента
  fwrite(&(tmp->InfoLength), sizeof(int), 1, file); //Запись длины информации

  char * buf = ReadInfo(tmp, filename); //Получение информации
  tmp->InfoLength = strlen(buf);
  fwrite(buf, sizeof(char), tmp->InfoLength, file); //Запись информации

  delete[] buf;
}

void WriteTable(char * filename){
  char * tmpfilename;
  Item * tmp = ptab;

  if(strcmp(filename, "Table.bin") == 0)
    tmpfilename = "TableSystem.bin";
  else
    tmpfilename = "Table.bin";

  FILE * file = fopen(tmpfilename, "wb+");

  for(; tmp; tmp = tmp->next)
    WriteRow(tmp, file, filename);

  fclose(file); // Закрываем файл
  remove(filename); //Удаляем предыдущий файл
  rename(tmpfilename, filename); //Переименовываем и получаем готовый файл с таблицей
  delete[] filename;
}