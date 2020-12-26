#include <iostream>
using namespace std;
//Вариант 14  
//Написать программу для работы с просматриваемой таблицей по запросам оператора. 
//Просматриваемая таблица организована списком, каждый элемент списка имеет следующую структуру:
	//struct Item{
		//int key;	/*ключ элемента	*/
		//int release;	/*номер версии элемента*/
		//char *info;	/*указатель на информацию*/
		//Item *next;	/*указатель на следующий элемент*/
	//};
/*Предусмотреть следующие операции:
	включение нового элемента в таблицу при условии, что в таблице могут находиться несколько элементов с одинаковыми ключами и разными номерами версий (номер версии элемента формируется как порядковый номер элемента в последовательности элементов с одинаковыми ключами, определяемый при включении элемента в таблицу);
	удаление из таблицы всех версий элемента, заданного ключом, или конкретной (заданной) версии элемента, также заданного своим ключом;
	вывод содержимого таблицы на экран.
Разработать два варианта программы:
a)	и сама таблица, и информация, относящаяся к элементу таблицы, хранятся в основной памяти;
Примечания:
1.	Программа должна содержать несколько функций; функция main должна выполнять: вывод меню, ввод и анализ ответа, вызов на исполнение требуемой функции;
2.	В программе нужно предусмотреть проверку правильности ввода данных;
3.	Для варианта b) следует модифицировать структуру, определяющую элемент списка, включив в нее длину информации и ее смещение в файле;
4.	В варианте b) для работы с файлом использовать функции пакета stdio.h; чтение и запись выполнять с помощью fread() и fwrite(), 
в которых должна быть указана реальная длина информации.*/  
//Просматриваемая таблица
struct Item{
	int key;      //ключ элемента
   int release;  //номер версии элемента
	char *info;   //указатель на информацию
	Item *next;   //указатель на следующий элемент
};

//Наша таблица
Item *ptab = NULL;
int input(); //проверка ввода данных

//Пункты меню
char * Mes[] = {"1. Insert element in table\n", "2. Delete element from table\n", "3. Show all table\n", "4. Quit\n"};

int MesCount = sizeof(Mes)/sizeof(Mes[0]);

//Первоначальное меню
int Menu(){
  int answer = 1;
  cout << "Main Menu" << endl;

  ////Начало: выбор действия\\\\
  do{
    for(int j = 0; j < MesCount; j++)
      cout << Mes[j];
    cout << "Make your choice: ";
    answer = input();
  }
    (answer < 1 || answer > MesCount);
  ////Конец: выбор действия\\\\

  return (answer % MesCount);
}

void Insert(); //Организационное занесение элемента в таблицу
int Push(int, int, char *); //Непосредственное занесение элемента в таблицу

void Delete(); //Организационное удаление элемента из таблицы
int del2(int, int); //Непосредственное удаление элемента из таблицы

void View(); //Просмотр таблицы

void Quit(); //выход

//другие функции
int search(int); //поиск последней версии элемента по ключу
char * delInfo(char *); //очищение памяти у информации
char * dupl(char *in); //выделение памяти для информации

void (*func[])() = {Quit, Insert, Delete, View}; //Массив указателей на функции

int main(){
  //setlocale(LC_ALL, "Russian");
  int answer = 1;
  cout << "Laboratory 2a, alternative 14" << endl;
  do{
    answer = Menu();
    func[answer]();
  }while(answer);
  return 0;
}

int Push(int k, int v, char * in){
  Item *tmp;
  tmp = new Item;

  //Занесение элемента в таблицу
  tmp->key = k;
  tmp->release = v;
  tmp->info = dupl(in);
  tmp->next = ptab;
  ptab = tmp;
  return 1;
}

void Insert(){
  int key, vers;
  char buf[80];

  cout << "Enter key: ";
  key = input();
  vers = search(key)+1;
  cin.ignore(80, '\n');

  cout << "Enter information: " << endl;
  cin.getline(buf, 80);

  if(Push(key, vers, buf))
    cout << "Element successfully inserted!" <<endl;
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
        delInfo(cur->info);
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
        delInfo(cur->info);
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

void Delete(){
  int key, vers, kdel;
  char buf[80];

  cout << "Enter key of deleting element: ";
  key = input();

  cout << "Enter version of deleting element (0 - all version): ";
  vers = input();
  //cin.ignore(80, '\n');

  if((kdel = del2(key, vers))>0)
    cout << "Successfully deleted " << kdel << " elements!" << endl;
  else
    cout << "No elements found for you request." << endl;
}

void View(){
  Item *cur = ptab;
  if(!cur){
    cout << "The table is empty!" << endl;
    return;
  }
  cout << " key \t| vers \t|  information\n===================\n";
  for(; cur; cur = cur->next)
    cout << cur-> key << "\t| " << cur->release << "\t|  " << cur->info <<endl;
}

void Quit(){
  Item *tmp;
  //Очищение таблицы по выходу
  while(ptab){
    tmp = ptab;
    delInfo(ptab->info);
    ptab = ptab->next;
    delete tmp;
  }
}

int input(){
  int a = 0;
  while(!(cin >> a)){
    if(cin.eof())
      break;
    cin.clear();
    cin.ignore();
  }
  return a;
}

int search(int k){
  Item *cur;
  for(cur = ptab; cur; cur = cur->next){
    if(cur->key == k)
      return cur->release;
  }
  return 0;
}

char * dupl(char *in)
{
	char *res = new char [strlen(in) + 1];
	strcpy(res, in);
	return res;
}

char * delInfo(char * in){
  delete [] in;
  return NULL;
}