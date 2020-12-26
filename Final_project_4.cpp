#include <iostream>
#include <string>
#include <cstdlib>
#include <fstream>
using namespace std;

const char* cType[]={"StdFile","SpFile","Catalog","MainCat"};
//Type=0=>stdFile
//Type=1=>spFile
//Type=2=>Catalog
////Type=3=>MainCat

const char* cAccess[]={"Full","ReadOnly","Disable"};
//Access=0=>Full
//Access=1=>OnlyRead
//Access=2=>Disable

const char* cspType[]={"bytes","blocks"};
//spType=0=>bytes
//spType=1=>blocks

const char* filename="file.inf";

//*********************************************************************************
//Описание классов
//*********************************************************************************
class Obj //основной класс
{
public:
	char *Name;
	int Date;
	int Time;
	int Access;
	int Adr;
	int Type;
	Obj *Owner;
	Obj():Name(NULL),Date(0),Time(0),Access(0),Type(0){};
	Obj (char*, int, int, int, Obj*);
	Obj (const &Obj);
	virtual ostream&put(ostream&)=0;
	virtual istream&get(istream&)=0;
	virtual iAm()=0;
	virtual void NewCur(char*,int)=0;
};

Obj::Obj(char *n, int D=0, int T=0, int A=0, Obj *O=NULL)
{
	Name=new char[strlen(n)+1];
	strcpy(Name,n);
	Date=D;
	Time=T;
	Access=A;
	Owner=O;
}


class StdFile: public Obj //стандартный файл
{
protected:
	ostream& put(ostream&);
    istream& get(istream&);
public:
	void NewCur(char* s,int i){cout<<"Bad command"<<endl;};
	int iAm() {return 0;}
	int Size;
	StdFile(StdFile &);
	StdFile():Obj(),Size(0){};
	StdFile(char *n, int D=0, int T=0, int A=0, int sz=0, Obj *O=NULL):Obj(n,D,T,A,O),Size(sz){};
	StdFile & operator = (const StdFile &);
	~StdFile();
};


class SpFile: public StdFile
{
protected:
	ostream& put(ostream&);
    istream& get(istream&);
public:
	int iAm() {return 1;}
	int spType;
	SpFile():StdFile(),spType(0){};
	SpFile(char *n, int D=0, int T=0, int A=0, int sz=0, int spt=0, Obj *O=NULL):StdFile(n,D,T,A,sz,O),spType(spt){};
	SpFile & SpFile::operator = (const SpFile &);
	~SpFile();
};


class Catalog: public Obj
{
protected:
	ostream& put(ostream&);
    istream& get(istream&);
	void Del();
public:
	void NewCur(char* ,int );
	int iAm() {return 2;}
	Catalog *Cats;
	int NCats;
	StdFile *Files;
	int NFiles;
	void Add(Obj&);
	Catalog & operator = (const Catalog &);
	Catalog (char *n, int D=0, int T=0, int A=0, Catalog *Catss=NULL, int nc=0, StdFile *Filess=NULL,int nf=0, Obj *O=NULL):Obj(n,D,T,A,O),Cats(Catss),NCats(nc),Files(Filess),NFiles(nf){};
	Catalog():Obj(),Cats(NULL),Files(NULL){};
	~Catalog();
};


class MainCat: public Catalog
{
protected:
	ostream& put(ostream&);
    istream& get(istream&);
	int iAm() {return 3;}
public:
	void NewCur(char* ,int );
	SpFile *SpFiles;
	int NSpFiles;
	MainCat():Catalog(),SpFiles(NULL){};
	MainCat (char *n, int D=0, int T=0, int A=0, Catalog *Catss=NULL, int nc=0, StdFile *Filess=NULL, int nf=0, SpFile *SpFiless=NULL, int ns=0):Catalog (n,D,T,A,Catss,nc,Filess,nf), SpFiles(SpFiless),NSpFiles(ns){};
	~MainCat();
};


class Iter;
typedef Obj* Elem;
class Table
{
public:
	Elem *El;
	int NEl;
	Table():El(NULL),NEl(0){};
	void Add(Obj &);
friend Iter;
//	~Table();
friend ostream & operator << (ostream &, Table &);
};


class Iter
{
	Table *main;
	int index;
public:
	Iter(Table *tab=NULL,int ind=0):main(tab),index(ind){};
	Obj* operator()();
	int End(){return (index>=(*main).NEl);};
	void reset(){index=0;};
};



Obj *Cur;
MainCat maincat;
Table Tab;

StdFile::StdFile(StdFile &f)
{	
	Name=new char[strlen(f.Name)+1];
	strcpy(Name,f.Name);
	Date=f.Date;
	Time=f.Time;
	Access=f.Access;
	Type=f.Type;
	Size=f.Size;
};

void MainCat::NewCur(char *s,int tp)
{
	int k=0;
	{
		if (tp==0)//ищем среди каталого
		{
			for(int i=0;i<(*(MainCat*)(Cur)).NCats;i++)
			{
				if (!strcmp(s,(*(MainCat*)(Cur)).Cats[i].Name))
				{
					Cur=&(*(MainCat*)(Cur)).Cats[i];
					k=1;break;
				}
			}
		}
		if (tp==1)//ищем среди файлов
		{
			for(int i=0;i<(*(MainCat*)(Cur)).NFiles;i++)
			{
				if (!strcmp(s,(*(MainCat*)(Cur)).Cats[i].Name))
				{
					Cur=&(*(MainCat*)(Cur)).Cats[i];
					k=1;break;
				}
			}
		}
		if (tp==2)//ищем среди спец.файлов
		{
			for(int i=0;i<(*(MainCat*)(Cur)).NSpFiles;i++)
			{
				if (!strcmp(s,(*(MainCat*)(Cur)).SpFiles[i].Name))
				{
					Cur=&(*(MainCat*)(Cur)).SpFiles[i];
					k=1;break;
				}
			}
		}
	}
if(!k)cout<<"Bad command"<<endl;
}

void Catalog::NewCur(char *s,int tp)	
	//т.е. мы указываем на основную дир.
{	int k=0;
	{
		if (tp==0)//ищем среди каталого
		{
			for(int i=0;i<(*(Catalog*)(Cur)).NCats;i++)
			{
				if (!strcmp(s,(*(Catalog*)(Cur)).Cats[i].Name))
				{
					Cur=&(*(Catalog*)(Cur)).Cats[i];
					k=1;break;
				}
			}
		}
		if (tp==1)//ищем среди файлов
		{
			for(int i=0;i<(*(Catalog*)(Cur)).NFiles;i++)
			{
				if (!strcmp(s,(*(Catalog*)(Cur)).Cats[i].Name))
				{
					Cur=&(*(Catalog*)(Cur)).Cats[i];
					k=1;break;
				}
			}
		}
	}
if(!k)cout<<"Bad command"<<endl;
}
		

//*********************************************************************************
//Добавление
//*********************************************************************************
void Catalog::Add(Obj& o)
{
	if (o.iAm()==0) //т.е. StdFile
	{
		StdFile *temp;
		temp=new StdFile[NFiles];
		for (int i=0;i<NFiles;i++)
			temp[i]=Files[i];
		delete []Files;
		Files =new StdFile[NFiles+1];
		for (i=0;i<NFiles;i++)
			Files[i]=temp[i];
		Files[NFiles++]=*(StdFile*)&o;
		delete []temp;
	}
	
	if (o.iAm()==2) //т.е. Catalog
		{
		Catalog *temp;
		temp=new Catalog[NCats];
		for (int i=0;i<NCats;i++)
			temp[i]=Cats[i];
		delete []Cats;
		Cats =new Catalog[NCats+1];
		for (i=0;i<NCats;i++)
			Cats[i]=temp[i];
		Cats[NCats++]=*(Catalog*)&o;
		delete []temp;
	}
	
	if (o.iAm()==1) //т.е. SpFile
	{
		SpFile *temp;
		temp=new SpFile[maincat.NSpFiles];
		for (int i=0;i<maincat.NSpFiles;i++)
			temp[i]=maincat.SpFiles[i];
		delete []maincat.SpFiles;
		maincat.SpFiles =new SpFile[maincat.NSpFiles+1];
		for (i=0;i<maincat.NSpFiles;i++)
			maincat.SpFiles[i]=temp[i];
		maincat.SpFiles[maincat.NSpFiles++]=*(SpFile*)&o;
		delete []temp;
	}
};


//*********************************************************************************
//Ввод
//*********************************************************************************
istream& MainCat::get(istream& is)
{	
	char s[25]; 
	int d,t,a;
	is>>s>>d>>t>>a;
	Owner=this;
	delete []Name;
	Name=new char[strlen(s)+1];
	strcpy(Name,s);
	Date=d;
	Time=t;
	Access=a;
	NFiles=0;
	NCats=0;
	NSpFiles=0;
	Tab.Add(*this);
	return is;
}

istream& Catalog::get(istream& is)
{
	char s[25]; 
	int d,t,a;
	is>>s>>d>>t>>a;
	Owner=Cur;
	delete []Name;
	Name=new char[strlen(s)+1];
	strcpy(Name,s);
	NFiles=0;
	NCats=0;
	Date=d;
	Time=t;
	Access=a;
	Tab.Add(*this);
	(*(Catalog*)Cur).Add(*this);
	return is;
}

istream& SpFile::get(istream& is)
{
//	is<<"<имя> <размер> <дату> <время> <доступ> <тип>"<<endl;
	char s[25]; 
	int si,d,t,a,ty;
	is>>s>>d>>t>>a>>si>>ty;
	Owner=Cur;
	delete []Name;
	Name=new char[strlen(s)+1];
	strcpy(Name,s);
	Date=d;
	Time=t;
	Access=a;
	Size=si;
	spType=ty;
	Tab.Add(*this);
	maincat.Add(*this);
	return is;
}

istream& StdFile::get(istream& is)
{
//	is<<"<имя> <размер> <дату> <время> <доступ>"<<endl;
	char s[25]; 
	int si,d,t,a;
	is>>s>>d>>t>>a>>si;
	Owner=Cur;
	delete []Name;
	Name=new char[strlen(s)+1];
	strcpy(Name,s);
	Date=d;
	Time=t;
	Access=a;
	Size=si;
	Tab.Add(*this);
	(*(Catalog*)Cur).Add(*this);
	return is;
}

istream& operator >> (istream& is, Obj & f)
{
	if (f.iAm()==0)cout<<"File:    <Name> <Date> <Time> <Access> <Size>"<<endl;
	if (f.iAm()==1)cout<<"SpFile:  <Name> <Date> <Time> <Access> <Size> <Type>"<<endl;
	if (f.iAm()==2)cout<<"Catalog: <Name> <Date> <Time> <Access> "<<endl;
	if (f.iAm()==3)cout<<"MainCat: <Name> <Date> <Time> <Access> "<<endl;
	return f.get(is);
}


//*********************************************************************************
//Вывод
//*********************************************************************************
ostream& StdFile::put(ostream& os)
{
	os<<"Type:"<<cType[iAm()]<<"; Name:"<<Name<<"; Date:"<<Date<<"; Time:"<<Time<<"; Access:"<<cAccess[Access]<<"; Size:"<<Size<<endl;
	return os;
}

ostream& SpFile::put(ostream& os)
{
	os<<"Type:"<<cType[iAm()]<<"; Name:"<<Name<<"; Date:"<<Date<<"; Time:"<<Time<<"; Access:"<<cAccess[Access]<<"; Size:"<<Size<<"; spType:"<<cspType[spType]<<endl;
	return os;
}

ostream& MainCat::put(ostream& os)
{
	int i;
	os<<"Type:"<<cType[iAm()]<<"; Name:"<<Name<<"; Date:"<<Date<<"; Time:"<<Time<<"; Access:"<<cAccess[Access]<<endl;
	os<<"Catalogs:"<<endl;
	for (i=0;i<NCats;i++)
		os<<'\t'<<Cats[i].Name<<endl;
	os<<"Files:"<<endl;
	for (i=0;i<NFiles;i++)
		os<<'\t'<<Files[i].Name<<endl;
	os<<"Spec Files:"<<endl;
	for (i=0;i<NSpFiles;i++)
		os<<'\t'<<SpFiles[i].Name<<endl;
	return os;
}

ostream& Catalog::put(ostream& os)
{
	int i;
	os<<"Type:"<<cType[iAm()]<<"; Name:"<<Name<<"; Date:"<<Date<<"; Time:"<<Time<<"; Access:"<<cAccess[Access]<<endl;
	os<<"Catalogs:"<<endl;
	for (i=0;i<NCats;i++)
		os<<'\t'<<Cats[i].Name<<endl;
	os<<"Files:"<<endl;
	for (i=0;i<NFiles;i++)
		os<<'\t'<<Files[i].Name<<endl;
	return os;
}

ostream& operator << (ostream& os, Obj & f)
{
	return f.put(os);
}


//*********************************************************************************
//Присваевание
//*********************************************************************************
SpFile & SpFile::operator = (const SpFile &c)
{
	delete []Name;
	Name=new char[strlen(c.Name)+1];
	strcpy(Name,c.Name);
	Date=c.Date;
	Time=c.Time;
	Access=c.Access;
	Size=c.Size;
	spType=c.spType;
	Owner=c.Owner;
	return *this;
}

StdFile & StdFile::operator = (const StdFile &c)
{
	delete []Name;
	Name=new char[strlen(c.Name)+1];
	strcpy(Name,c.Name);
	Date=c.Date;
	Time=c.Time;
	Access=c.Access;
	Size=c.Size;
	Owner=c.Owner;
	return *this;
}

Catalog & Catalog::operator = (const Catalog &c)
{
	int i;
	delete []Name;
	Name=new char[strlen(c.Name)+1];
	strcpy(Name,c.Name);
	Date=c.Date;
	Time=c.Time;
	Access=c.Access;
	Cats=new Catalog[NCats=c.NCats];
	for(i=0;i<NCats;i++)
		Cats[i]=c.Cats[i];
	Files=new StdFile[NFiles=c.NFiles];
	for(i=0;i<NFiles;i++)
		Files[i]=c.Files[i];
	Owner=c.Owner;
	return *this;
}


//*********************************************************************************
//Delete
//*********************************************************************************
void Del(Obj &o)
{
	Cur=o.Owner;
	int j=0;
	if(o.iAm()==0)//т.е. это файл
	{
		StdFile *temp;
		temp=new StdFile[(*(Catalog*)(Cur)).NFiles-1];
		for(int i=0;i<(*(Catalog*)(Cur)).NFiles;i++)
		{
			if((&(*(Catalog*)(Cur)).Files[i])!=&o)
				temp[j++]=(*(Catalog*)(Cur)).Files[i];
		}
		delete [](*(Catalog*)(Cur)).Files;
		(*(Catalog*)(Cur)).Files=new StdFile[--(*(Catalog*)(Cur)).NFiles];
		for(i=0;i<(*(Catalog*)(Cur)).NFiles;i++)
		{
			(*(Catalog*)(Cur)).Files[i]=temp[i];
		}
		delete []temp;
		return;
	}
	
	
	if(o.iAm()==1)//т.е. это спец. файл
	{
		SpFile *temp;
		temp=new SpFile[(*(MainCat*)(Cur)).NSpFiles-1];
		for(int i=0;i<(*(MainCat*)(Cur)).NSpFiles;i++)
		{
			if((&(*(MainCat*)(Cur)).SpFiles[i])!=&o)
				temp[j++]=(*(MainCat*)(Cur)).SpFiles[i];
		};
		delete []((*(MainCat*)(Cur)).SpFiles);
		(*(MainCat*)(Cur)).SpFiles=new SpFile[--(*(MainCat*)(Cur)).NSpFiles];
		for(i=0;i<(*(MainCat*)(Cur)).NSpFiles;i++)
		{
			(*(MainCat*)(Cur)).SpFiles[i]=temp[i];
		};
		delete []temp;
		return;
	}


	if(o.iAm()==2)//т.е. это каталог
	{
		Catalog *temp;
		temp=new Catalog[(*(Catalog*)(Cur)).NCats-1];
		for(int i=0;i<(*(Catalog*)(Cur)).NCats;i++)
		{
			if((&(*(Catalog*)(Cur)).Cats[i])!=&o)
				temp[j++]=(*(Catalog*)(Cur)).Cats[i];
		};
		delete [](*(Catalog*)(Cur)).Cats;
		(*(Catalog*)(Cur)).Cats=new Catalog[--(*(Catalog*)(Cur)).NCats];
		for(i=0;i<(*(Catalog*)(Cur)).NCats;i++)
		{
			(*(Catalog*)(Cur)).Cats[i]=temp[i];
		};
		delete []temp;
		return;
	}
}


//*********************************************************************************
//Destructors
//*********************************************************************************
StdFile::~StdFile()
{
	Date=0;
	Time=0;
	Type=0;
	Size=0;
	Access=0;
	Owner=NULL;
}

SpFile::~SpFile()
{
	Date=0;
	Time=0;
	Type=0;
	Size=0;
	Access=0;
	Owner=NULL;
}

Catalog::~Catalog()
{
	Date=0;
	Time=0;
	Type=0;
	Access=0;
	Owner=NULL;
	delete []Cats;
	delete []Files;
}

MainCat::~MainCat()
{
	Date=0;
	Time=0;
	Type=0;
	Access=0;
	Owner=NULL;
	delete []Cats;
	delete []Files;
	delete []SpFiles;
}


//*********************************************************************************
//Table
//*********************************************************************************
void Table::Add(Obj &o)
{
	int w=1;
	int t;
	Elem *temp;
	temp=new Elem[NEl];
	for (int i=0; i<NEl;i++)
		temp[i]=El[i];
	if(El) delete []El;
	El=new Elem[NEl+1];
	int j=0;
	if ((NEl++)>0)
	{
		for (i=0; i<NEl; i++)
		{	
			if(w&&(j<(NEl-1)))
			{
				t=strcmp((*temp[j]).Name,o.Name);
				if (t==-1)//(*temp[j]).Name<o.Name)
				{
					El[i]=temp[j++];
				}
				else
				{
					El[i]=&o;
					w=0;
				}
			}
			else
			{
				if(!w)
				{
					El[i]=temp[j++];
				}
				else
				{
					El[i]=&o;
				}
			}
		}
	}
	else
	{
		El[0]=&o;
	}
	if(temp)delete []temp;
};

ostream & operator << (ostream & os, Table & t)
{
	Iter I(&Tab,0);
	I.reset();
	while (!I.End())
		cout<<*I();
	return os;
}


//*********************************************************************************
//Iterator
//*********************************************************************************
Obj* Iter::operator ()()
{
	if (index<(*main).NEl)
		return (*main).El[index++];
	else 
		return (*main).El[index=0];

};
//*********************************************************************************
//InterFace
//*********************************************************************************
void take(char s[])
{	
//	char s[80];
	char s1[25];
	int type;
//	cin>>s;
	int key=0;
	int nkey=0;
	if (!strcmp(s,"dir"))   key=1;
	if (!strcmp(s,"md"))    key=2;
	if (!strcmp(s,"/?"))    key=3;
	if (!strcmp(s,"cd"))    key=4;
	if (!strcmp(s,"mf"))    key=5;
	if (!strcmp(s,"msf"))   key=6;
	if (!strcmp(s,"edit"))  key=7;
	if (!strcmp(s,"delete"))key=8;
	if (!strcmp(s,"full"))  key=9;
	switch(key){
	case 1:
		cout<<*Cur<<endl;
		break;
	case 9:
		cout<<Tab;
		break;
	case 3:
		if (((*Cur).iAm()==0)||((*Cur).iAm()==1))//т.е. файл
			cout<<"dir              - information"<<endl<<"cd               - movings (see. cd /?)"<<endl<<"edit             - changes (see. edit /?)"<<endl<<"delete           - delete file"<<endl<<"full             - show full information"<<endl;
		if ((*Cur).iAm()==2)//т.е. каталог
			cout<<"dir              - contents"<<endl<<"cd               - movings (see. cd /?)"<<endl<<"md               - made new Catalog"<<endl<<"mf               - made new file"<<endl<<"msf              - made new spec. file"<<endl<<"edit             - changes (see. edit /?)"<<endl<<"delete           - delete file"<<endl<<"full             - show full information"<<endl;   
		if ((*Cur).iAm()==3)
			cout<<"dir              - contents"<<endl<<"cd               - movings (see. cd /?)"<<endl<<"md               - made new Catalog"<<endl<<"mf               - made new file"<<endl<<"msf              - made new spec. file"<<endl<<"edit             - changes (see. edit /?)"<<endl<<"full             - show full information"<<endl;   
		break;
	case 2:
		if(!(*Cur).Access)
		{			
			if((*Cur).iAm()!=0 && (*Cur).iAm()!=1)
			{
				Catalog *cat;
				cat=new Catalog;
				cin>>*cat;
				break;
			}
			else
			{
				cout<<"Bad command"<<endl;
				break;
			}
		}
		else
		{
			cout<<"Error!-check up the rights of access!"<<endl;		
			break;
		}
	case 4: 
		cin>>s1;
		if(!strcmp(s1,"/?"))
		{
			cout<<"cd ..            - go upwards"<<endl;
			if((*Cur).iAm()==0 || (*Cur).iAm()==1)break;
			if((*Cur).iAm()==2)
			{
				cout<<"cd <Name> <Type> - go to Catalog(Type=0)/File(Type=1) <Name>"<<endl;
				break;
			}
			if((*Cur).iAm()==3)
			{
				cout<<"cd <Name> <Type> - go to Catalog(Type=0)/File(Type=1)/SpFile(Type=2) <Name>"<<endl;
				break;
			}
		}

		if(!strcmp(s1,".."))
		{
			Cur=(*Cur).Owner;
			break;
		}
		cin>>type;
		(*Cur).NewCur(s1,type);
		break;						
	case 5:
		if(!(*Cur).Access)
		{
			if((*Cur).iAm()!=0 && (*Cur).iAm()!=1)
			{
				StdFile *file;
				file=new StdFile;
				cin>>*file;
				break;
			}
			else
			{
				cout<<"Bad command"<<endl;
				break;
			}
		}
		else
		{
			cout<<"Error!-check up the rights of access!"<<endl;		
			break;
		}
	case 6:
		if(!(*Cur).Access)
		{
			if((*Cur).iAm()!=0 && (*Cur).iAm()!=1)
			{
				SpFile *sfile;
				sfile=new SpFile;
				cin>>*sfile;
				break;
			}
			else
			{
				cout<<"Bad command"<<endl;
				break;
			}
		}
		else
		{
			cout<<"Error!-check up the rights of access!"<<endl;		
			break;
		}
	case 7: 
		cin>>s1;//команда Name:/Date:/Time:/Size:/Type:/Access:
		if(!strcmp(s1,"name"))  nkey=1;
		if(!strcmp(s1,"date"))  nkey=2;
		if(!strcmp(s1,"time"))  nkey=3;
		if(!strcmp(s1,"size"))  nkey=4;
		if(!strcmp(s1,"type"))  nkey=5;
		if(!strcmp(s1,"access"))nkey=6;
		if(!strcmp(s1,"/?"))    nkey=7;
		
		switch(nkey){
		case 1:
			if(!(*Cur).Access)
			{
				cin>>s1;
				delete [](*Cur).Name;
				(*Cur).Name=new char[strlen(s1)+1];
				strcpy((*Cur).Name,s1);
			}
			else
				cout<<"Error!-check up the rights of access!"<<endl;
			break;
		case 2:
			if(!(*Cur).Access)
				cin>>(*Cur).Date;
			else
				cout<<"Error!-check up the rights of access!"<<endl;
			break;
		case 3:
			if(!(*Cur).Access)
				cin>>(*Cur).Time;
			else
				cout<<"Error!-check up the rights of access!"<<endl;
			break;
		case 6:
			cin>>(*Cur).Access;
			break;
		case 4:
			if((*Cur).iAm()==0 || (*Cur).iAm()==1)
			{
				cin>>(*(StdFile*)(Cur)).Size;
				break;
			}
			else
			{
				cout<<"Bad command"<<endl;
				break;
			}
		case 5:
			if((*Cur).iAm()==1)
			{
				cin>>(*(SpFile*)(Cur)).spType;
				break;
			}
			else
			{
				cout<<"Bad command"<<endl;
				break;
			}
		case 7:
			cout<<"edit name/date/time/access";
			if(((*Cur).iAm()!=1)&&((*Cur).iAm()!=0))cout<<" <meaning>"<<endl;
			if((*Cur).iAm()==1)cout<<"/size/type <meaning>"<<endl;
			if((*Cur).iAm()==0)cout<<"/size <meaning>"<<endl;
			break;
		
		default:
			cout<<"Bad command"<<endl;
			break;
		}
		break;
	case 8:
		if((*Cur).iAm()!=3)
			{
				Del(*Cur);
				cout<<"The removal is successfully completed"<<endl;
				break;
			}
			else 
			{
				cout<<"You can not remove the Main catalogue"<<endl;
				break;
			}
					
	default:
		cout<<"Bad command"<<endl;
		break;
	}
}



//*********************************************************************************
//Save
//*********************************************************************************
void SaveFile()
{
	ofstream f;
	static openmode;
	f.open(filename,ios::binary);
	f<<maincat;
	f.close();
}


//*********************************************************************************
//Load
//*********************************************************************************
void LoadFile()
{
	ifstream f;
	static openmode;
	f.open(filename,ios::binary);
	f>>maincat;
//	f.get(maincat,'\n');
	f.close();
}



//*********************************************************************************
//Main
//*********************************************************************************
void main()
{
	char s[255];
    cin>>maincat;
	Cur=&maincat;
//	SaveFile();
//	LoadFile();
	while(cin>>s)
		take(s);
};