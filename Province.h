#include <vector>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <set>
#include <limits>
#include <sstream>

#define SPEEDHIGH 90
#define SPEEDLOW 40
#define SPEEDFLIGHT 700
#define DRIVESTART 7
#define DRIVEEND 19
#define SCENESTART 8
#define SCENEEND 18
#define MAXDRIVE 8
#define MAXHALFDRIVE 5
#define MAXPLAYDRIVE 3
#define HOURHALFDAY 4
#define HOURFULLDAY 8
#define HOURCITY 24
#define HOURTWODAY 30
#define MAXDATE 15
#define COSTROOM 300
#define COSTRENT 300
#define HEADING "天数\t时间\t方式\t起始\t里程\t到达\t游玩\t时长\t价格"

using namespace std;

struct City{
	string name;
	int idx;
	int belong;
	City(string Name,int Idx,int Belong):name(Name),idx(Idx),belong(Belong){};
};

struct Scene{
	string name;
	int idx;
	float stay;
	int belong;
	Scene(string Name,int Idx,float Stay,int Belong):name(Name),idx(Idx),stay(Stay),belong(Belong){};
};

class Province{
public:
	string name;
	int cityNum;
	int sceneNum;
	vector<City> cities;
	vector<Scene> scenes;
	vector<vector<int> > sceneCityIdx;
	vector<vector<float> > sceneCityTime;
	int distanceFromStart;
	string startProvince;
	int **DistanceMat; //Distance between Cities
	float **Shortest; //Shortest TIME between Scenes
    float **DriveCost;
    int **DriveLength;
	string *tour;
    int splitStart;
    int splitEnd;
    int parentIdx;
    int timeTrain;
    int timeFlight;
    int timeTrainFlight;
    int timeFlightTrain;
    float costCar;
    float costTrain;
    float costFlight;
    float costTrainFlight;
    float costFlightTrain;
    string stCar;
    string stTrain;
    string stFlight;
    string stTrainFlight;
    string stFlightTrain;
    string stMerge;
    stringstream *sscurrent;

	int minl;//shortest time
    int *mina;//shortest path in index

    Province(string Name):name(Name),cityNum(0),sceneNum(0),splitStart(0),parentIdx(0) {
        cout<<name<<" initialized\n";
        mina=0;
        minl=numeric_limits<int>::max();
        timeTrain=numeric_limits<int>::max();
        timeFlight=numeric_limits<int>::max();
        timeTrainFlight=numeric_limits<int>::max();
        timeFlightTrain=numeric_limits<int>::max();
        costCar=numeric_limits<int>::max();
        costTrain=numeric_limits<int>::max();
        costFlight=numeric_limits<int>::max();
        costTrainFlight=numeric_limits<int>::max();
        costFlightTrain=numeric_limits<int>::max();
        sscurrent=new stringstream;
    }
    
    Province(Province* province,int idx):name(province->name),sceneNum(province->sceneNum),scenes(province->scenes),distanceFromStart(province->distanceFromStart),startProvince(province->startProvince),Shortest(province->Shortest),splitStart(province->splitEnd),mina(province->mina),parentIdx(idx) {
        double time=DRIVESTART,drived=0,remaining=MAXDRIVE;
        int result=1;
        double timeFromStart=static_cast<double>(distanceFromStart)/SPEEDHIGH;
        bool halfplayed=false,fullplayed=false;
        drive(drived,timeFromStart, result,remaining, time,halfplayed,fullplayed,-1,0);
        int current=0;
        for(int i=splitStart;i<sceneNum;i++) {
            int next=mina[i];
            if(current<next) {
                drive(drived,Shortest[current][next],result,remaining,time,halfplayed,fullplayed,current,next);
            }else if (current>next) {
                drive(drived,Shortest[next][current],result,remaining,time,halfplayed,fullplayed,current,next);
            }
            play(drived,scenes[next].stay,result,remaining,time,halfplayed,fullplayed,i);
            current=next;
        }
        if(current!=0) {
            drive(drived,Shortest[0][current],result,remaining,time,halfplayed,fullplayed,current,0);
        }
        drive(drived, timeFromStart, result, remaining, time,halfplayed,fullplayed,0,-1);
        minl=result;
        timeTrain=numeric_limits<int>::max();
        timeFlight=numeric_limits<int>::max();
        timeTrainFlight=numeric_limits<int>::max();
        timeFlightTrain=numeric_limits<int>::max();
        costCar=numeric_limits<int>::max();
        costTrain=numeric_limits<int>::max();
        costFlight=numeric_limits<int>::max();
        costTrainFlight=numeric_limits<int>::max();
        costFlightTrain=numeric_limits<int>::max();
        sscurrent=new stringstream;
    }
    
    void readFile(string prefix,int idx) {
        stringstream ss;
        ss<<"/Users/gyz/Workspaces/xcode/math/math/data/"<<prefix<<idx<<".txt";
        string s;
        ss>>s;
        fstream fs(s);
        fs>>minl;
        mina=new int[sceneNum];
        for(int i=0;i<sceneNum;i++) {
            fs>>mina[i];
        }
    }

	void addCity(string cityName, int cityIdx){
		City city(cityName,cityIdx,0);
		cities.push_back(city);
		cityNum += 1;
		//cout<<"City:"<<cityName<<" added\n";
	}

	void addScene(string sceneName, int sceneIdx, float stay){
		Scene scene(sceneName,sceneIdx,stay,0);
		scenes.push_back(scene);
		sceneNum += 1;
		//cout<<"Scene:"<<sceneName<<" added\n";
	}

	void initCity(int idx){
		//openfile
		char filename[1024];
		sprintf(filename,"/Users/gyz/Workspaces/xcode/math/math/data/SceneDistance%d.csv",idx);
		cout<<filename<<endl;
		ifstream infile(filename);
		//params
		char buffer[1024];
		int sceneidx,cityidx,distance;
		float time;
		bool inflag;
		int lastsceneidx = idx;
		int scenecount = 0;
		vector<int> scenecityidx;
		vector<float> scenecitytime;
		//add cities and update scenecities
		//add self
		addCity(name,idx);
		scenecityidx.push_back(idx);
		scenecitytime.push_back(0);
		while(infile>>buffer){
			sscanf(buffer,"%d;%d;%d;%f",&sceneidx,&cityidx,&distance,&time);
			inflag = false;
			for(int i=0;i<cities.size();i++){
				if(cities[i].idx==cityidx)
					inflag = true;
			}
			if(!inflag&&cityidx>=1000){
				char cityname[1024];
				sprintf(cityname,"City%d",cityidx);
				addCity(cityname,cityidx);
			}
			//update scenecity
			if(sceneidx!=lastsceneidx){	//start a new scene
				if(scenecityidx.size()>=0){
					sceneCityIdx.push_back(scenecityidx);
					sceneCityTime.push_back(scenecitytime);
				}
				scenecityidx.clear();
				scenecitytime.clear();
				lastsceneidx = sceneidx;
				scenecount += 1;
			}
			scenecityidx.push_back(cityidx);
			scenecitytime.push_back(time);
		}
		sceneCityIdx.push_back(scenecityidx);//update last scene
		sceneCityTime.push_back(scenecitytime);
		infile.close();
		cout<<"show all cities"<<endl;
		for(int i=0;i<cities.size();i++){
			cout<<cities[i].idx<<" "<<cities[i].name<<endl;
		}
		//*
		cout<<"show all scenecity"<<endl;
		for(int i=0;i<sceneCityIdx.size();i++){
			cout<<"Scene:"<<scenes[i].name<<endl;
			for(int j=0;j<sceneCityIdx[i].size();j++){
				cout<<sceneCityIdx[i][j]<<"("<<sceneCityTime[i][j]<<")\t";
			}
			cout<<endl;
		}
		//*/
		cout<<"cityNum:"<<cityNum<<endl;
		//system("PAUSE");
	}

	void initCityDistance(int idx){
		//here calc distance/time/cost between all cities not scenes
		DistanceMat = new int*[cityNum];
		for(int i=0;i<cityNum;i++){
			DistanceMat[i] = new int[cityNum];
		}
		char filename[1024];
		sprintf(filename,"/Users/gyz/Workspaces/xcode/math/math/data/CityDistance%d.csv",idx);
		cout<<filename<<endl;
		ifstream infile(filename);
		char buffer[1024];
		int startcityidx,endcityidx,distance;
		float time;
		int startidx,endidx;
		int allin;
		while(infile>>buffer){
			sscanf(buffer,"%d;%d;%d;%f",&startcityidx,&endcityidx,&distance,&time);
			allin = 0;
			for(int i=0;i<cityNum;i++){
				if(cities[i].idx==startcityidx){
					startidx = i;
					allin += 1;
				}
				if(cities[i].idx==endcityidx){
					endidx = i;
					allin += 1;
				}
			}
			if(allin==2)
				DistanceMat[startidx][endidx] = DistanceMat[endidx][startidx] = distance;
			else{
				cout<<"Error here"<<endl;
				//system("PAUSE");
			}
		}
		for(int i=0;i<cityNum;i++)
			DistanceMat[i][i] = 0;
		
		infile.close();
		//printDistanceMat();
		//system("PAUSE");
	}

	void printDistanceMat(){
		for(int i=0;i<cityNum;i++){
			for(int j=0;j<cityNum;j++){
				cout<<DistanceMat[i][j]<<'\t';
			}
			cout<<endl;
		}
		cout<<endl;
	}

	void initSceneShortest(){
		float shortest,thistime;
		int startidx,endidx;
		char shortestpath[2048];
		string buf;
		Shortest = new float*[sceneNum];
		for(int i=0;i<sceneNum;i++){
			Shortest[i] = new float[sceneNum];
		}
		for(int i=0;i<sceneNum;i++){
			Shortest[i][i] = 0;
			for(int j=i+1;j<sceneNum;j++){
				shortest = 100000;
				for(int m=0;m<sceneCityIdx[i].size();m++){
					for(int n=0;n<sceneCityIdx[j].size();n++){
						for(int c=0;c<cityNum;c++){
							if(cities[c].idx==sceneCityIdx[i][m])
								startidx = c;
							if(cities[c].idx==sceneCityIdx[j][n])
								endidx = c;
						}
						thistime = sceneCityTime[i][m] + float(DistanceMat[startidx][endidx])/SPEEDHIGH + sceneCityTime[j][n];
						if(shortest>=thistime){
							shortest = thistime;
							//sprintf(shortestpath,"%d->%d->%d->%d",scenes[i].idx,cities[startidx].idx,cities[endidx].idx,scenes[j].idx);
							//cout<<shortestpath<<endl;
							//system("PAUSE");
						}
					}
				}
				Shortest[i][j] = Shortest[j][i] = shortest;
			}
		}
		printShortest();
		//system("PAUSE");
	}

	void printShortest(){
		cout<<"Shortest Mat:"<<endl;
		for(int i=0;i<sceneNum;i++){
			for(int j=0;j<sceneNum;j++){
				cout<<Shortest[i][j]<<"\t";
			}
			cout<<endl;
		}
		cout<<endl;
	}

	vector<int> generateOnePath(){
		//int *path = new int[sceneNum];
		cout<<"test path:\n";
		vector<int> path;
		for(int i=0;i<sceneNum;i++){
			path.push_back(i);
			cout<<scenes[i].name<<"-->";
		}
		cout<<endl;
		return path;
	}

	string tourGenerator(vector<int> path){
		string tour="";
		int day=0;
		float hour=7;
		int startsceneidx,endsceneidx;
		char buffer[2048];

		float backtime = float(distanceFromStart)/SPEEDHIGH;
		day += backtime/8;
		hour = hour + (backtime - day*8);
		cout<<startProvince<<"-->"<<cities[0].name<<endl;
		cout<<distanceFromStart<<" km"<<endl;
		cout<<"Day:"<<day<<" Clock:"<<hour<<endl;
		//system("PAUSE");
		//sprintf(buffer,"City:%s-%d km-City:%s(Day:%d,Time:%f)",startProvince,distanceFromStart,cities[0].name,day,hour);
		for(int i=0;i<path.size();i++){
			if(i==path.size()-1){	//tour done and go back
				cout<<"Go back"<<endl;
				float remainingDayDriveTime = getRemainingDayDriveTime();
				if(backtime <= remainingDayDriveTime){
					hour+=backtime;
				}
				else{
					backtime -= remainingDayDriveTime;
					day += 1;
					hour = 7;
					day += backtime/8;
					hour = hour + (backtime - int(backtime/8)*8);
				}
				cout<<"Day:"<<day<<" Clock:"<<hour<<endl;
				break;
			}
			//get path from path[i] to path[i+1]
			
		}

		//tour += string(buffer);
		return tour;
	}

	bool checkBack(int sceneidx, int day, float remainingDayDriveTime){
		float tmptime = float(distanceFromStart)/SPEEDHIGH;
		if(tmptime>remainingDayDriveTime){
			tmptime-=remainingDayDriveTime;
			if((day+1+tmptime/8)>15)
				return false;
		}
		return true;
	}
	
	float getRemainingDayDriveTime(){
		return 0;
	}

	//GU
	void ShortestPath(){
        vector<int> remainings;
        for(int i=0;i<sceneNum;i++) {
            remainings.push_back(i);
        }
        int *a=new int[sceneNum];
        ShortestPathHelper(0,remainings,a);
        delete [] a;
        splitEnd=sceneNum;
	}
    
    void ShortestPathHelper(int index,vector<int>& remainings,int a[]) {
        if(index<sceneNum) {
        for(int i=0;i<remainings.size();i++) {
            vector<int> newremainings;
            for(int j=0;j<i;j++) {
                newremainings.push_back(remainings[j]);
            }
            for(int j=i+1;j<remainings.size();j++) {
                newremainings.push_back(remainings[j]);
            }
            int *b=new int[sceneNum];
            memcpy(b,a,sizeof(int)*index);
            b[index]=remainings[i];
            ShortestPathHelper(index+1,newremainings,b);
            delete [] b;
        }
        } else {
            int current=timeCalc(a);
            if(current<minl) {
                minl=current;
                if(mina!=0) {
                    delete[] mina;
                }
                mina=new int[sceneNum];
                memcpy(mina, a, sizeof(int)*sceneNum);
            }
        }
    }
    
    int timeCalc(int a[]) {
        double time=DRIVESTART,drived=0,remaining=MAXDRIVE;
        int result=1;
        bool halfplayed=false,fullplayed=false;
        double timeFromStart=static_cast<double>(distanceFromStart)/SPEEDHIGH;
        drive(drived,timeFromStart, result,remaining, time,halfplayed,fullplayed,-1,0);
        int current=0;
        for(int i=0;i<sceneNum;i++) {
            int next=a[i];
            if(current<next) {
                drive(drived,Shortest[current][next],result,remaining,time,halfplayed,fullplayed,current,next);
            }else if (current>next) {
                drive(drived,Shortest[next][current],result,remaining,time,halfplayed,fullplayed,current,next);
            }
            play(drived,scenes[next].stay,result,remaining,time,halfplayed,fullplayed,i);
            current=next;
        }
        if(current!=0) {
            drive(drived,Shortest[0][current],result,remaining,time,halfplayed,fullplayed,current,0);
        }
        drive(drived, timeFromStart, result, remaining, time,halfplayed,fullplayed,0,-1);
        return result;
    }
    
    void drive(double& drived,double length,int& result,double& remaining,double& time,bool& halfplayed,bool& fullplayed,int start,int end,bool rent=false) {
        int mileage=0;
        float costd=0;
        if(start>=0&&end>=0) {
            mileage=DriveLength[start][end];
            costd=DriveCost[start][end];
        }else {
            mileage=length*60;
            costd=mileage;
        }
        while(length>remaining) {
            *sscurrent<<result<<"\t"<<time<<"\tdrive\t"<<(start<0?start:scenes[start].idx)<<"\t"<<mileage<<"\t"<<(end<0?end:scenes[end].idx)<<"\t\t"<<remaining<<"\t"<<costd<<endl;
            *sscurrent<<result<<"\t\troom\t\t\t\t\t\t"<<COSTROOM<<endl;
            if(rent) {
                *sscurrent<<result<<"\t\trent\t\t\t\t\t\t"<<COSTRENT<<endl;
            }
            length-=remaining;
            time=DRIVESTART;
            result++;
            remaining=MAXDRIVE;
            halfplayed=false;
            fullplayed=false;
        }
        *sscurrent<<result<<"\t"<<time<<"\tdrive\t"<<(start<0?start:scenes[start].idx)<<"\t"<<mileage<<"\t"<<(end<0?end:scenes[end].idx)<<"\t\t"<<length<<"\t"<<costd<<endl;
        time+=length;
        drived=length;
        remaining-=length;
    }
    
    void flightOrTrain(double& drived,float length,int& result,double& remaining, double& time,bool& halfplayed,bool& fullplayed,int start,int end,int costf,bool flight=true) {
        if(fullplayed||length>DRIVEEND-time) {
            *sscurrent<<result<<"\t\troom\t\t\t\t\t\t"<<COSTROOM<<endl;
            *sscurrent<<result<<"\t\trent\t\t\t\t\t\t"<<COSTRENT<<endl;
            result++;
            time=DRIVESTART;
            fullplayed=false;
            drived=0;
        }
        *sscurrent<<result<<"\t"<<time<<"\t"<<(flight?"flight":"train")<<"\t"<<(start<0?start:scenes[start].idx)<<"\t\t"<<(end<0?end:scenes[end].idx)<<"\t\t"<<length<<"\t"<<costf<<endl;
        halfplayed=true;
        time+=length;
        remaining=DRIVEEND-time;
        if(remaining>MAXHALFDRIVE) {
            remaining=MAXHALFDRIVE;
        }
    }
    
    void play(double& drived,int length,int& result,double& remaining, double& time,bool& halfplayed,bool& fullplayed,int idx,bool rent=false) {
        switch (length) {
            case HOURHALFDAY:
                if(halfplayed&&drived<=MAXPLAYDRIVE&&time+length<=SCENEEND) {
                    *sscurrent<<result<<"\t"<<time<<"\t\t\t\t\t"<<scenes[idx].idx<<"\t0.5 DAY\t"<<endl;
                    time+=length;
                    if(remaining>DRIVEEND-time) {
                        remaining=DRIVEEND-time;
                    }
                    if(remaining>MAXPLAYDRIVE-drived) {
                        remaining=MAXPLAYDRIVE-drived;
                    }
                    fullplayed=true;
                }
                else if(!halfplayed && drived<=MAXHALFDRIVE && time + length<=SCENEEND) {
                    *sscurrent<<result<<"\t"<<time<<"\t\t\t\t\t"<<scenes[idx].idx<<"\t0.5 DAY\t"<<endl;
                    time+=length;
                    halfplayed=true;
                    if(remaining>DRIVEEND-time) {
                        remaining=DRIVEEND-time;
                    }
                    if(remaining>MAXHALFDRIVE-drived) {
                        remaining=MAXHALFDRIVE-drived;
                    }
                    fullplayed=false;
                } else {
                    *sscurrent<<result<<"\t\troom\t\t\t\t\t\t"<<COSTROOM<<endl;
                    if(rent) {
                        *sscurrent<<result<<"\t\trent\t\t\t\t\t\t"<<COSTRENT<<endl;
                    }
                    result++;
                    *sscurrent<<result<<"\t"<<SCENESTART<<"\t\t\t\t\t"<<scenes[idx].idx<<"\t0.5 DAY\t"<<endl;
                    time=SCENESTART+length;
                    drived=0;
                    remaining=MAXHALFDRIVE;
                    halfplayed=true;
                    fullplayed=false;
                }
                break;
            case HOURFULLDAY:
                if(drived<=MAXPLAYDRIVE&&time+length<=SCENEEND) {
                    *sscurrent<<result<<"\t"<<time<<"\t\t\t\t\t"<<scenes[idx].idx<<"\t1 DAY\t"<<endl;
                    time+=length;
                    if(remaining>DRIVEEND-time) {
                        remaining=DRIVEEND-time;
                    }
                    if(remaining>MAXPLAYDRIVE-drived) {
                        remaining=MAXPLAYDRIVE-drived;
                    }
                }else {
                    *sscurrent<<result<<"\t\troom\t\t\t\t\t\t"<<COSTROOM<<endl;
                    if(rent) {
                        *sscurrent<<result<<"\t\trent\t\t\t\t\t\t"<<COSTRENT<<endl;
                    }
                    result++;
                    *sscurrent<<result<<"\t"<<SCENESTART<<"\t\t\t\t\t"<<scenes[idx].idx<<"\t1 DAY\t"<<endl;
                    time=SCENESTART+length;
                    drived=0;
                    remaining=MAXPLAYDRIVE;
                }
                fullplayed=true;
                break;
            case HOURCITY:
                *sscurrent<<result<<"\t"<<time<<"\t\t\t\t\t"<<scenes[idx].idx<<"\t24 HOURS\t"<<endl;
                *sscurrent<<result<<"\t\troom\t\t\t\t\t\t"<<COSTROOM<<endl;
                if(rent) {
                    *sscurrent<<result<<"\t\trent\t\t\t\t\t\t"<<COSTRENT<<endl;
                }
                result++;
                drived=0;
                remaining=DRIVEEND-time;
                if(remaining>MAXDRIVE) {
                    remaining=MAXDRIVE;
                }
                fullplayed=false;
                break;
            case HOURTWODAY:
                if(time+HOURFULLDAY<=SCENEEND&&drived<=MAXPLAYDRIVE) {
                    *sscurrent<<result<<"\t"<<time<<"\t\t\t\t\t"<<scenes[idx].idx<<"\t2 DAYS\t"<<endl;
                    *sscurrent<<result<<"\t\troom\t\t\t\t\t\t"<<COSTROOM<<endl;
                    if(rent) {
                        *sscurrent<<result<<"\t\trent\t\t\t\t\t\t"<<COSTRENT<<endl;
                    }
                    result++;
                }else {
                    *sscurrent<<result+1<<"\t"<<SCENESTART<<"\t\t\t\t\t"<<scenes[idx].idx<<"\t2 DAYS\t"<<endl;
                    *sscurrent<<result+1<<"\t\troom\t\t\t\t\t\t"<<COSTROOM<<endl;
                    if(rent) {
                        *sscurrent<<result+1<<"\t\trent\t\t\t\t\t\t"<<COSTRENT<<endl;
                    }
                    result+=2;
                }
                time=SCENESTART+HOURFULLDAY;
                drived=0;
                remaining=MAXPLAYDRIVE;
                fullplayed=true;
                break;
        }
    }
    
    Province split(int idx) {
        double time=DRIVESTART,drived=0,remaining=MAXDRIVE;
        int result=1,i=splitStart;
        double timeFromStart=static_cast<double>(distanceFromStart)/SPEEDHIGH;
        bool halfplayed=false,fullplayed=false;
        drive(drived,timeFromStart, result,remaining, time,halfplayed,fullplayed,-1,0);
        int current=0,lastminl=0;
        for(;i<sceneNum;i++) {
            int next=mina[i];
            if(current<next) {
                drive(drived,Shortest[current][next],result,remaining,time,halfplayed,fullplayed,current,next);
            }else if (current>next) {
                drive(drived,Shortest[next][current],result,remaining,time,halfplayed,fullplayed,current,next);
            }
            play(drived,scenes[next].stay,result,remaining,time,halfplayed,fullplayed,i);
            double timeTemp=time,drivedTemp=drived,remainingTemp=remaining;
            int resultTemp=result;
            if(next!=0) {
                drive(drivedTemp,Shortest[0][next],resultTemp,remainingTemp,timeTemp,halfplayed,fullplayed,next,0);
            }
            drive(drivedTemp,timeFromStart,resultTemp,remainingTemp,timeTemp,halfplayed,fullplayed,0,-1);
            if(resultTemp>MAXDATE) {
                break;
            }
            current=next;
            lastminl=resultTemp;
        }
        splitEnd=i;
        minl=lastminl;
        
        return Province(this,idx);
    }
    
    int merge(Province p,int distance) {
        sscurrent->str(HEADING);
        p.sscurrent->str("");
        double time=DRIVESTART,drived=0,remaining=MAXDRIVE;
        int result=1;
        bool halfplayed=false,fullplayed=false;
        drive(drived,static_cast<double>(distanceFromStart)/SPEEDHIGH, result,remaining, time,halfplayed,fullplayed,-1,0);
        int current=0;
        for(int i=splitStart;i<splitEnd;i++) {
            int next=mina[i];
            if(current<next) {
                drive(drived,Shortest[current][next],result,remaining,time,halfplayed,fullplayed,current,next);
            }else if (current>next) {
                drive(drived,Shortest[next][current],result,remaining,time,halfplayed,fullplayed,current,next);
            }
            play(drived,scenes[next].stay,result,remaining,time,halfplayed,fullplayed,i);
            current=next;
        }
        if(current!=0) {
            drive(drived,Shortest[0][current],result,remaining,time,halfplayed,fullplayed,current,0);
        }
        drive(drived, static_cast<double>(distance)/SPEEDHIGH, result, remaining, time,halfplayed,fullplayed,0,-1);
        current=0;
        for(int i=p.splitStart;i<p.splitEnd;i++) {
            int next=p.mina[i];
            if(current<next) {
                p.drive(drived,p.Shortest[current][next],result,remaining,time,halfplayed,fullplayed,current,next);
            }else if (current>next) {
                p.drive(drived,p.Shortest[next][current],result,remaining,time,halfplayed,fullplayed,current,next);
            }
            p.play(drived,p.scenes[next].stay,result,remaining,time,halfplayed,fullplayed,i);
            current=next;
        }
        if(current!=0) {
            p.drive(drived,p.Shortest[0][current],result,remaining,time,halfplayed,fullplayed,current,0);
        }
        p.drive(drived,static_cast<double>(p.distanceFromStart)/SPEEDHIGH,result,remaining,time,halfplayed,fullplayed,0,-1);
        stMerge=sscurrent->str()+p.sscurrent->str();
        return result;
    }
    
    void calcCar() {
        sscurrent->str(HEADING);
        double time=DRIVESTART,drived=0,remaining=MAXDRIVE;
        int result=1;
        float c=0;
        bool halfplayed=false,fullplayed=false;
        double timeFromStart=static_cast<double>(distanceFromStart)/SPEEDHIGH;
        drive(drived,timeFromStart, result,remaining, time,halfplayed,fullplayed,-1,0);
        c+=distanceFromStart;
        int current=0;
        for(int i=splitStart;i<splitEnd;i++) {
            int next=mina[i];
            if(current<next) {
                drive(drived,Shortest[current][next],result,remaining,time,halfplayed,fullplayed,current,next);
                c+=DriveCost[current][next];
            }else if (current>next) {
                drive(drived,Shortest[next][current],result,remaining,time,halfplayed,fullplayed,current,next);
                c+=DriveCost[next][current];
            }
            play(drived,scenes[next].stay,result,remaining,time,halfplayed,fullplayed,i);
            current=next;
        }
        if(current!=0) {
            drive(drived,Shortest[0][current],result,remaining,time,halfplayed,fullplayed,current,0);
            c+=DriveCost[0][current];
        }
        drive(drived, timeFromStart, result, remaining, time,halfplayed,fullplayed,0,-1);
        c+=distanceFromStart;
        c+=COSTROOM*(result-1);
        costCar=c;
        stCar=sscurrent->str();
    }
    
    void calcTrain(float tt,float tc) {
        sscurrent->str(HEADING);
        double time=DRIVESTART,drived=0,remaining=0;
        int result=1;
        float c=0;
        bool halfplayed=false,fullplayed=false;
        flightOrTrain(drived,tt, result,remaining, time, halfplayed, fullplayed,-1,0,3*tc,false);
        c+=3*tc;
        int current=0;
        if(remaining>MAXHALFDRIVE) {
            remaining=MAXHALFDRIVE;
        }
        for(int i=splitStart;i<splitEnd;i++) {
            int next=mina[i];
            if(current<next) {
                drive(drived,Shortest[current][next],result,remaining,time,halfplayed,fullplayed,current,next,true);
                c+=DriveCost[current][next];
            }else if (current>next) {
                drive(drived,Shortest[next][current],result,remaining,time,halfplayed,fullplayed,current,next,true);
                c+=DriveCost[next][current];
            }
            play(drived,scenes[next].stay,result,remaining,time,halfplayed,fullplayed,i,true);
            current=next;
        }
        if(current!=0) {
            drive(drived,Shortest[0][current],result,remaining,time,halfplayed,fullplayed,current,0,true);
            c+=DriveCost[0][current];
        }
        flightOrTrain(drived,tt, result,remaining, time, halfplayed, fullplayed,0,-1,3*tc,false);
        c+=3*tc;
        timeTrain=result;
        if(result<=15) {
            c+=(COSTRENT+COSTROOM)*(result-1);
            costTrain=c;
        }
        stTrain=sscurrent->str();
    }
    
    void calcFlight(float fc) {
        sscurrent->str(HEADING);
        double time=DRIVESTART,drived=0,remaining=0;
        int result=1;
        float c=0;
        float timeFromStart=static_cast<float>(distanceFromStart)/SPEEDFLIGHT;
        bool halfplayed=false,fullplayed=false;
        flightOrTrain(drived,timeFromStart, result,remaining, time, halfplayed, fullplayed,-1,0,3*fc);
        c+=3*fc;
        int current=0;
        if(remaining>MAXHALFDRIVE) {
            remaining=MAXHALFDRIVE;
        }
        for(int i=splitStart;i<splitEnd;i++) {
            int next=mina[i];
            if(current<next) {
                drive(drived,Shortest[current][next],result,remaining,time,halfplayed,fullplayed,current,next,true);
                c+=DriveCost[current][next];
            }else if (current>next) {
                drive(drived,Shortest[next][current],result,remaining,time,halfplayed,fullplayed,current,next,true);
                c+=DriveCost[next][current];
            }
            play(drived,scenes[next].stay,result,remaining,time,halfplayed,fullplayed,i,true);
            current=next;
        }
        if(current!=0) {
            drive(drived,Shortest[0][current],result,remaining,time,halfplayed,fullplayed,current,0,true);
            c+=DriveCost[0][current];
        }
        flightOrTrain(drived,timeFromStart, result,remaining, time, halfplayed, fullplayed,0,-1,3*fc);
        c+=3*fc;
        timeFlight=result;
        if(result<=15) {
            c+=(COSTRENT+COSTROOM)*(result-1);
            costFlight=c;
        }
        stFlight=sscurrent->str();
    }
    
    void calcTrainFlight(float tt,float tc,float fc) {
        sscurrent->str(HEADING);
        double time=DRIVESTART,drived=0,remaining=0;
        int result=1;
        float c=0;
        float timeFromStart=static_cast<float>(distanceFromStart)/SPEEDFLIGHT;
        bool halfplayed=false,fullplayed=false;
        flightOrTrain(drived,tt, result,remaining, time, halfplayed, fullplayed,-1,0,3*tc,false);
        c+=3*tc;
        int current=0;
        if(remaining>MAXHALFDRIVE) {
            remaining=MAXHALFDRIVE;
        }
        for(int i=splitStart;i<splitEnd;i++) {
            int next=mina[i];
            if(current<next) {
                drive(drived,Shortest[current][next],result,remaining,time,halfplayed,fullplayed,current,next,true);
                c+=DriveCost[current][next];
            }else if (current>next) {
                drive(drived,Shortest[next][current],result,remaining,time,halfplayed,fullplayed,current,next,true);
                c+=DriveCost[next][current];
            }
            play(drived,scenes[next].stay,result,remaining,time,halfplayed,fullplayed,i,true);
            current=next;
        }
        if(current!=0) {
            drive(drived,Shortest[0][current],result,remaining,time,halfplayed,fullplayed,current,0,true);
            c+=DriveCost[0][current];
        }
        flightOrTrain(drived,timeFromStart, result,remaining, time, halfplayed, fullplayed,0,-1,3*fc);
        c+=3*fc;
        timeTrainFlight=result;
        if(result<=15) {
            c+=(COSTRENT+COSTROOM)*(result-1);
            costTrainFlight=c;
        }
        stTrainFlight=sscurrent->str();
    }
    
    void calcFlightTrain(float tt,float tc,float fc) {
        sscurrent->str(HEADING);
        double time=DRIVESTART,drived=0,remaining=0;
        int result=1;
        float c=0;
        float timeFromStart=static_cast<float>(distanceFromStart)/SPEEDFLIGHT;
        bool halfplayed=false,fullplayed=false;
        flightOrTrain(drived,timeFromStart, result,remaining, time, halfplayed, fullplayed,-1,0,3*fc);
        c+=3*fc;
        int current=0;
        if(remaining>MAXHALFDRIVE) {
            remaining=MAXHALFDRIVE;
        }
        for(int i=splitStart;i<splitEnd;i++) {
            int next=mina[i];
            if(current<next) {
                drive(drived,Shortest[current][next],result,remaining,time,halfplayed,fullplayed,current,next,true);
                c+=DriveCost[current][next];
            }else if (current>next) {
                drive(drived,Shortest[next][current],result,remaining,time,halfplayed,fullplayed,current,next,true);
                c+=DriveCost[next][current];
            }
            play(drived,scenes[next].stay,result,remaining,time,halfplayed,fullplayed,i,true);
            current=next;
        }
        if(current!=0) {
            drive(drived,Shortest[0][current],result,remaining,time,halfplayed,fullplayed,current,0,true);
            c+=DriveCost[0][current];
        }
        flightOrTrain(drived,tt, result,remaining, time, halfplayed, fullplayed,0,-1,3*tc,false);
        c+=3*tc;
        timeFlightTrain=result;
        if(result<=15) {
            c+=(COSTRENT+COSTROOM)*(result-1);
            costFlightTrain=c;
        }
        stFlightTrain=sscurrent->str();
    }
    
    int choice() {
        int minimum=costCar,result=0;
        if(costTrain<minimum) {
            result=1;
        }if(costFlight<minimum) {
            result=2;
        }if(costTrainFlight<minimum) {
            result=3;
        }if(costFlightTrain<minimum) {
            result=4;
        }
        return result;
    }
};