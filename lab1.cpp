#include <iostream>
using namespace std;
struct Line{
  int n;
  int *a;
};
/*Вариант 16
Из входного потока вводится непрямоугольная матрица целых чисел [aij], i=1,…,m, j=1,…,n. Значения m и n заранее не известны и вводятся из входного потока. 
  Сформировать вектор {bi}, i=1,…,m, i-ый элемент которого равен сумме тех элементов i-ой строки матрицы, 
  которые превышают соответствующий элемент предыдущей (i-1)-ой строки.
    Для первой строки матрицы в качестве предыдущей использовать последнюю строку матрицы. 
        Исходную матрицу и полученный вектор вывести в выходной поток с необходимыми комментариями.*/

int input(Line *&a); //массив структур, возвращает количество строк матрицы
void erase(Line *&a, int m); //очищение памяти матрицы
void form_new(Line *lines, int m, int *&s); //формирование вектора
int sum(int a[], int n, int b[], int m);
void output(Line *lines, int m, char mes_m[], int *s, char mes_v[]); //вывод

int main(){
  Line *arr=NULL;
  int *s=NULL;
  int m;
  //Пока количество строк больше нуля
  while ((m = input(arr))){
    form_new(arr,m,s);
    output(arr, m, "Source matrix: ", s, "Result vector: ");
    erase(arr, m);
    delete []s;
  }
  cin >> m;
  return 0;
}

int input(Line *&lines){
  const char *pr="";
  int m; //количество строк
  //Начало: ввод количества строк
  do{
    cout << pr <<  endl;
    cout << "Enter number of lines: ";
    pr = "You are wrong; repeat, please!";
    cin >> m;
  }while(m<0);
  //Конец: ввод количества строк
  lines = new Line[m];
  for(int i = 0; i<m; i++){
    pr = "";
    //Начало: ввод количества столбцов для строки i
    do{
      cout << pr <<  endl;
      cout << "Enter number of dems in line " << (i+1) << ": ";
      pr = "You are wrong; repeat, please!";
      cin >> lines[i].n;
    }while(lines[i].n<1);
    //Конец: ввод количества столбцов для строки i
    lines[i].a = new int[lines[i].n];
    cout << "Enter " << lines[i].n << " numbers: ";
    //Начало: ввод n чисел для строки i
    for(int j = 0; j<lines[i].n; j++)
      cin >> lines[i].a[j];
    //Конец: ввод n чисел для строки i
  }
  return m;
}

void erase(Line *&lines, int m){
  for(int i = 0; i<m; i++)
    delete []lines[i].a;
  delete []lines;
  lines = NULL;
}

void form_new(Line *lines, int m, int *&s){
  s = new int[m];
  //Начало: обработка первой и последней строки отдельно
  s[0] = sum(lines[0].a, lines[0].n, lines[m-1].a, lines[m-1].n);
  //Конец: обработка первой и последней строки отдельно
  //Начало: обработка второй и последующих строк
  for(int i = 1; i<m; i++)
    s[i] = sum(lines[i].a, lines[i].n, lines[i-1].a, lines[m-1].n);
  //Конец: обработка второй и последующих строк
}

int sum(int a[], int n, int b[], int m){
  int s=0;
  //Начало: сложение чисел в строке
  for(int i = 0; i<n; i++)
    //Если есть с чем сравнивать
    if (i<m){
      if(a[i]>b[i])
        s = s+a[i];
    }
    else
      s = s+a[i];
  //Конец: сложение чисел в строке
  return s;
}

void output(Line *lines, int m, char mes_m[], int *s, char mes_v[]){
  //Начало: вывод матрицы
  cout << mes_m << endl;
  for(int j = 0; j<m; j++){
    cout << j << '\t';
    for(int k = 0; k<lines[j].n; k++)
      cout << lines[j].a[k] << ' ';
    cout << endl;
  }
  //Конец: вывод матрицы
  //Начало: вывод вектора
  cout << mes_v << endl;
  for(int i = 0; i<m; i++)
    cout << s[i] << ' ';
  //Конец: вывод вектора
}
