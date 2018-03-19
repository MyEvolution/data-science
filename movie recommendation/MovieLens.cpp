#include<string>
#include<iostream>
#include<vector>
#include<map>
#include<fstream>
#include<algorithm>
#include<unordered_set> 
#include<sstream>
#include<ctime>
#include<random>
using namespace std;
typedef map<int,unordered_set<int>> datastruct;
int times = 0;
vector<int> movieId;
map<int,string> movieIdToName;
void getMovie(string filename)
{
	ifstream ifs(filename);
	string line;
	//datastruct result;
	while(getline(ifs,line))
	{
		string weNeed[2];
		int now = 0;
	     for(auto i:line)
	     {
	     	if(i != '\t')
	     	weNeed[now]+=i;
	     	else
	     	  now++;
	     	  if(now == 2)
	     	  break;
		 }
		int id = atoi(weNeed[0].c_str());
	//	movieId.push_back(i);
		movieIdToName[id] = weNeed[1];
	}
 } 
void arrangeData(string filename,datastruct &movies,datastruct &users)
{
	ifstream ifs(filename);
	string uid,mid;
	int num1=0,num2=0,num3=0,num4=0,num5=0,num6=6; 
	string score;
	datastruct movie;
	while(ifs>>uid>>mid)
	{
		ifs>>score;
		if(score[0]>='3')
		{
			int u = atoi(uid.c_str());
			int m = atoi(mid.c_str());
		
		//	users[u].insert(m);
			
			movie[m].insert(u);
			
		}
		
		ifs>>score;
	}
	for(auto &i : movie)
	{
       int count = i.second.size();
       if(count < 20)
         ++num1;
        else if(count < 40)
          ++num2;
          else if(count<60)
           ++num3;
           else if(count<80)
           ++num4;
           else if(count<100)
           ++num5;
           else ++num6;
        if(count>=50)
        { 
           movies[i.first]=i.second;
           movieId.push_back(i.first); 
           } 
	}
	cout<<"ѡ������  |��Ӱ��Ŀ"<<endl; 
	cout<<"0-20     :"<<num1<<endl;
		cout<<"20-40     :"<<num2<<endl;
			cout<<"40-60     :"<<num3<<endl;
				cout<<"60-80     :"<<num4<<endl;
					cout<<"80-100     :"<<num5<<endl;
						cout<<">=100:"<<num6<<endl;
	cout<<"�����Եĵ�Ӱ����:"<<movie.size() - movieId.size()<<endl; 
	int edgeCount = 0;
	ifstream iff(filename);
		while(iff>>uid>>mid)
	{
		iff>>score;

		
			int u = atoi(uid.c_str());
			users[u];
			if(score[0]>='3')
		{
			int m = atoi(mid.c_str());
		
		if(movies.find(m)!=movies.end())
		{
		
			users[u].insert(m);
			++edgeCount;
		}
			
		}		
		iff>>score;
		
	}
	cout<<"����"<<edgeCount<<"���ߣ�"<<endl;
	cout<<"���������Ҫ�Ĳ�����:"<<movies.size()<<"*"<<users.size()<<"*"<<movies.size()<<endl;
	
}
void getTrainAndTest(datastruct &movies,datastruct &users,datastruct &trainM,datastruct &trainU,datastruct &testM,datastruct &testU)
{
	default_random_engine d(time(nullptr));
	uniform_int_distribution<unsigned> dis(0, 9);
	int train = 0,test = 0;
	for(auto &i:movies)
	{
		for(auto &j:i.second)
		{
			//cout<<dis(d)<<endl;
			if(dis(d)<9)
			{
			trainU[j].insert(i.first);
			trainM[i.first].insert(j);
			++train;
		 	}
		 	else
		 	{
		 		testU[j].insert(i.first);
		 		testM[i.first].insert(j);
		 		++test;
			 }
		}
	}

	cout<<"����ѵ�����ı��У�"<<train<<"��"<<endl;
	cout<<"������Լ��ı��У�"<<test<<"��"<<endl; 
//	cout<<testM.size()<<" "<<trainM.size()<<endl; 
}

 inline double getAQuota(datastruct &movies,const datastruct &users,int id,int jd)
{
	double result = 0.0;
    
	for(auto &selecteds : users)
	{
	if(selecteds.second.find(id) != selecteds.second.end() && selecteds.second.find(jd) != selecteds.second.end())
		{
		result+=1.0/selecteds.second.size();
		//	cout<<result<<endl;
 		}
 		++times;
	}
	 
	return result/movies[jd].size();
}
void getAllQuotas(datastruct &movies,const datastruct &users,vector<vector<double>> &iNeed)
{
 cout<<"loading.....please wait...."<<endl;
	for(auto &i : movies)
	{
	iNeed.push_back(vector<double>());
	int now = iNeed.size() - 1;
	   for(auto &j : movies)
	   {
	   	double score = getAQuota(movies,users,i.first,j.first);
	     //cout<<score<<endl;
	   	iNeed[now].push_back(score);
	   		  
	   }
      }
}

vector<pair<double,int>> getRecommend(int id,vector<vector<double>> &iNeed,datastruct &users)
{
	
	vector<int> help;
	auto user = users[id];
	for(auto &m : movieId)
	{
		help.push_back(user.find(m) != user.end());
	}
	
	vector<pair<double,int>> the_f;
//	cout<<"������:"<<endl;
	for(int i = 0;i!=iNeed.size();++i)
	{
		if(user.find(movieId[i]) == user.end())
		{
		
		the_f.push_back(make_pair(0.0,movieId[i]));
		auto &score = the_f[the_f.size()-1];
		for(int j = 0;j!=iNeed.size();++j)
		  score.first+=iNeed[i][j]*help[j];
    	}
  //  	else cout<<movieId[i]<<endl;
	}
	sort(the_f.begin(),the_f.end(),[](pair<double,int> a,pair<double,int> b){return a.first>b.first;}
	);
	return the_f;
}
double getR(int id,vector<vector<double>> &iNeed,datastruct &users,datastruct &testU)
{
   vector<pair<double,int>> res = getRecommend(id,iNeed,users);
   map<int,int> list ;
   for(int i = 0;i!=res.size();++i)
   {
   	list[res[i].second] = i+1;
	   }
	   double r = 0.0;
	for(auto i:testU[id])
	{
	   r+=list[i]/list.size();
	}
	return r/list.size();
	
}
double getR(int id,vector<pair<double,int>> &res,datastruct &testU,double threshold,double &tp,double &fp)
{
	   map<int,int> list ;
   for(int i = 0;i!=res.size();++i)
   {
   	list[res[i].second] = i+1;
	   }
	   double r = 0.0;
	 //  cout<<"û������"<<endl;
	 	 int TP = 0,FP = 0;
	 	 double now;
	for(auto i:testU[id])
	{
	//cout<<i<<" "<<list[i]<<endl;
	   now=(0.0+list[i])/list.size();
	   r+=now;
	   if(now<threshold)
	    ++TP;
	}
	FP = int(list.size()*threshold)- TP;

	if(testU[id].size() == 0)
	tp = 1;
	else 
       tp = (TP+0.0)/testU[id].size();
       fp = (FP+0.0)/(list.size() - testU[id].size());
    //cout<<tp<<" "<<fp<<endl;
	return r/list.size();
}
int main()
{
	string filename1 = "C:\\Users\\����ʱ����������\\Desktop\\movies.txt";
	string filename2 = "C:\\Users\\����ʱ����������\\Desktop\\ratings.txt";
	datastruct testU,testM,trainU,trainM;
	puts("���ڻ�õ�Ӱ��Ϣ...");
	datastruct movies;// 
	getMovie(filename1);
	cout<<"����"<<movieIdToName.size()<<"����Ӱ"<<endl; 
	puts("���ڻ���û���Ϣ...");
	datastruct users ;
	puts("���������ȡ��Ϣ...");
	arrangeData(filename2,movies,users);
	vector<vector<double>> wt;
	puts("��������ѵ�����Ͳ��Լ�...");
	getTrainAndTest(movies,users,trainM,trainU,testM,testU);
	puts("���ڻ��W����...");

	getAllQuotas(trainM,trainU,wt);
	cout<<"�������:"<<times<<endl;
		puts("���������Ƽ�����...");
	int userId;
//	cout<<"�������û�id����ѯΪ�����ɵ��Ƽ��б�..."<<endl; 
//	ofstream theLists("./lists.txt");
    ofstream ROC("./roc_0.025.txt");
	double R = 0.0; 
	//while(cin>>userId)
	vector<vector<pair<double,int>>> ress;
	for(auto &id:users)
	{
		userId = id.first;
		//cout<<"�û�id��"<<userId<<endl; 
		//theLists<<"�û�id��"<<userId<<endl; 
      ress.push_back( getRecommend(userId,wt,trainU));
    }
    
    ROC<<0<<"\t"<<0<<endl;
    for(double  i = 0;i<=100.0;i+=2.5)
    {
    	double tp = 0.0,fp = 0.0;
    	for(int j = 1;j<=ress.size();++j)
    	{
		double t,f;
    	getR(j,ress[j-1],testU,i/100,t,f);
    	tp+=t;
    	fp+=f;
        }
        //cout<<"threshold:"<<i/10<<endl;
        //cout<<"TP:"<<tp/ress.size()<<" "<<"FP:"<<fp/ress.size()<<endl;
        ROC<<fp/ress.size()<<"\t"<<tp/ress.size()<<endl;
	}
	ROC<<1<<"\t"<<1<<endl;
	//cout<<"����|�Ƽ���ӰID|�Ƽ�����|�Ƽ�ָ��"<<endl;
/*	for(auto i : res)
	{
	 cout<<rank++<<"|"<<i.second<<"|"<<movieIdToName[i.second]<<"|"<<i.first<<endl;
	 //theLists<<rank++<<"|"<<i.second<<"|"<<movieIdToName[i.second]<<"|"<<i.first<<endl;
	}*/

    //theLists<<"<r>��"<<r<<endl;
    
    //R+=r;
    //	cout<<"--------------------------------------------------"<<endl; 
    //	theLists<<"--------------------------------------------------"<<endl; 
    //	cout<<"�������û�id����ѯΪ�����ɵ��Ƽ��б�..."<<endl; 
   //}
   //theLists<<"�����û���<r>ƽ��ֵ:"<<R/users.size()<<endl; 
	return 0;
}

