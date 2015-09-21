#include <vector>
#include <string>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include "Province.h"

using namespace std;
class SortProvince {
public:
    int i;
    int j;
    int opt;
    SortProvince(int a,int b,int o):i(a),j(b),opt(o){}
    bool operator<(const SortProvince& s) const {
        return opt<s.opt;
    }
    bool has(int a,int b) {
        return i==a||j==a||i==b||j==b;
    }
};
class Country{
public:
	int provinceNum;
	vector<Province> provinces;
	vector<City> citylist;
	vector<Scene> scenelist;
	int StartIdx;
	int **DistanceMat;
	string *tour;
	vector<int> left;
    vector<vector<int>> mergedCar;
    vector<SortProvince> realMergedCar;
    float **timeTrain;
    float **costTrain;
    float **costFlight;

	Country(){provinceNum=0;}

	vector<Scene> initScene(){
		ifstream infile("/Users/gyz/Workspaces/xcode/math/math/data/SceneData.csv");
		char buffer[1024];
		int idx,type,belong,stay;
		char name[256];
		
		while(infile>>buffer){
			sscanf(buffer,"%d;%[^;];%d;%d;%d;",&idx,name,&type,&belong,&stay);
			if(type==1){ //This is a province
				provinceNum += 1;
				cout<<idx<<':';
				Province province(name);
				provinces.push_back(province);
			}
			provinces[belong-1].addScene(name,idx,stay);
			scenelist.push_back(Scene(name,idx,stay,belong));
		}
		cout<<"Total Number:"<<provinceNum<<endl;
		for(int i=0;i<provinces.size();i++){
			cout<<provinces[i].name<<" scenes:"<<provinces[i].sceneNum<<endl;
		}
		infile.close();
		return scenelist;
	}

	vector<City> initCitylist(){
		cout<<"Init City list..."<<endl;
		ifstream infile("/Users/gyz/Workspaces/xcode/math/math/data/CityData.csv");
		char buffer[1024];
		int idx,type,belong;
		char name[256];
		while(infile>>buffer){
			sscanf(buffer,"%d;%[^;];%d;%d",&idx,name,&type,&belong);
			citylist.push_back(City(name,idx,belong));
		}
		cout<<"CityNum in citylist:"<<citylist.size()<<endl;
		infile.close();
		return citylist;
	}

	void initCity(){
		cout<<"Initializing Cities in Provinces..."<<endl;
		for(int i=0;i<provinceNum;i++){
		//for(int i=10;i<11;i++){//test JiangSu(10)
			cout<<"Init "<<provinces[i].name<<endl;
			provinces[i].initCity(i+1);
			provinces[i].initCityDistance(i+1);
			provinces[i].initSceneShortest();
		}
		cout<<"Init Cities Done."<<endl;
		//system("PAUSE");
	}

	void initProvinceDistance(){
		cout<<"Initializing Distance Between Provinces..."<<endl;
		//init mat
		DistanceMat = new int*[provinceNum];
		for(int i=0;i<provinceNum;i++){
			DistanceMat[i] = new int[provinceNum];
		}

		//load province data
		ifstream infile("/Users/gyz/Workspaces/xcode/math/math/data/citycity.csv");
		string buffer;
		for(int i=0;i<provinceNum;i++){
			infile>>buffer;
			int start=0,count=0,index=0;
			//cout<<buffer<<endl;
			while(count<provinceNum-1){
				index = buffer.find_first_of(';',start);
				DistanceMat[count][i] = DistanceMat[i][count] = atoi(buffer.substr(start,index-start).c_str());
				count+=1;
				start = index+1;
			}
			DistanceMat[i][i] = 0;
		}
		infile.close();
		//printDistanceMat();
		//system("PAUSE");
	}

	void printDistanceMat(){
		cout<<"Distance Mat"<<endl;
		for(int i=0;i<provinceNum;i++){
			for(int j=0;j<provinceNum;j++){
				cout<<DistanceMat[i][j]<<'\t';
			}
			cout<<endl;
		}
		cout<<endl;
	}

	void setStartProvince(int idx){
		idx -= 1;
		cout<<"Set Start Province:"<<provinces[idx].name<<endl;
		for(int i=0;i<provinceNum;i++){
			provinces[i].distanceFromStart = DistanceMat[idx][i];
			provinces[i].startProvince = provinces[idx].name;
		}
        StartIdx=idx;
	}

	void timeCalcTest(int idx){
		cout<<"time:"<<provinces[idx-1].tourGenerator(provinces[idx-1].generateOnePath())<<endl;
        provinces[idx-1].readFile("order", idx);
	}
    
    void shortestPathTest(int idx) {
        stringstream ss;
        ss<<"/Users/gyz/Workspaces/xcode/math/math/data/beijing"<<idx<<".txt"<<endl;
        string s;
        ss>>s;
        cout<<s;
        fstream fs(s);
        provinces[idx-1].ShortestPath();
        fs<<provinces[idx-1].minl<<endl;
        for(int i=0;i<provinces[idx-1].sceneNum;i++) {
            fs<<provinces[idx-1].mina[i]<<endl;
        }
        fs.close();
    }
	
    int split(int idx) {
        int parts=1;
        if(provinces[idx-1].minl>15) {
            Province p=provinces[idx-1].split(idx);
            provinces.push_back(p);
            for(;p.minl>15;parts++) {
                p=p.split(p.parentIdx);
                provinces.push_back(p);
            }
        }
        return parts;
    }
    
    void realMergeCar() {
        for(int i=0;i<provinces.size();i++) {
            vector<int> v;
            for(int j=0;j<provinces.size();j++) {
                int parenti=i<31?i:provinces[i].parentIdx-1,parentj=j<31?j:provinces[j].parentIdx-1;
                v.push_back(i==j?0:provinces[i].merge(provinces[j],DistanceMat[parenti][parentj]));
            }
            mergedCar.push_back(v);
        }
        vector<SortProvince> vsp;
        for(int i=0;i<provinces.size();i++) {
            for(int j=0;j<provinces.size();j++) {
                if(i!=j) {
                    int current=provinces[i].minl+provinces[j].minl,temp=mergedCar[i][j];
                    if(temp<=15&&temp<current&&provinces[i].choice()==0&&provinces[j].choice()==0) {
                        vsp.push_back(SortProvince(i,j,current-temp));
                    }
                }
            }
        }
        while(!vsp.empty()) {
            sort(vsp.begin(),vsp.end());
            SortProvince sp=vsp[0];
            realMergedCar.push_back(sp);
            int parenti=sp.i<31?sp.i:provinces[sp.i].parentIdx-1,parentj=sp.j<31?sp.j:provinces[sp.j].parentIdx-1;
            provinces[sp.i].merge(provinces[sp.j],DistanceMat[parenti][parentj]);
            for(vector<SortProvince>::iterator iter=vsp.begin();iter!=vsp.end();) {
                if(iter->has(sp.i,sp.j)) {
                    iter=vsp.erase(iter);
                }else {
                    iter++;
                }
            }
        }
    }
    
    void compareTrainOrFlight() {
        for(int i=0;i<31;i++) {
            if(i!=StartIdx) {
                if(costTrain[StartIdx][i]>0&&timeTrain[StartIdx][i]<=6&&timeTrain[StartIdx][i]>0) {
                    provinces[i].calcTrain(timeTrain[StartIdx][i],costTrain[StartIdx][i]);
                    if(costFlight[StartIdx][i]>0) {
                        provinces[i].calcTrainFlight(timeTrain[StartIdx][i], costTrain[StartIdx][i], costFlight[StartIdx][i]);
                        provinces[i].calcFlightTrain(timeTrain[StartIdx][i], costTrain[StartIdx][i], costFlight[StartIdx][i]);
                    }
                }
                if(costFlight[StartIdx][i]>0) {
                    provinces[i].calcFlight(costFlight[StartIdx][i]);
                }
                if(provinces[i].minl>15) {
                    int choiceTemp=provinces[i].choice();
                    if (choiceTemp==0) {
                        split(i+1);
                    }
                    if(costTrain[StartIdx][i]>0&&timeTrain[StartIdx][i]<=6&&timeTrain[StartIdx][i]>0) {
                        provinces[i].calcTrain(timeTrain[StartIdx][i],costTrain[StartIdx][i]);
                        if(costFlight[StartIdx][i]>0) {
                            provinces[i].calcTrainFlight(timeTrain[StartIdx][i], costTrain[StartIdx][i], costFlight[StartIdx][i]);
                            provinces[i].calcFlightTrain(timeTrain[StartIdx][i], costTrain[StartIdx][i], costFlight[StartIdx][i]);
                        }
                    }
                    if(costFlight[StartIdx][i]>0) {
                        provinces[i].calcFlight(costFlight[StartIdx][i]);
                    }
                } else {
                    provinces[i].calcCar();
                }
            }else {
                provinces[i].calcCar();
            }
        }
        for(int i=31;i<provinces.size();i++) {
            provinces[i].calcCar();
            int parentIdx=provinces[i].parentIdx-1;
            if(parentIdx!=StartIdx) {
                if(costTrain[StartIdx][parentIdx]>0&&timeTrain[StartIdx][parentIdx]<=6&&timeTrain[StartIdx][parentIdx]>0) {
                    provinces[i].calcTrain(timeTrain[StartIdx][parentIdx],costTrain[StartIdx][parentIdx]);
                    if(costFlight[StartIdx][parentIdx]>0) {
                        provinces[i].calcTrainFlight(timeTrain[StartIdx][parentIdx], costTrain[StartIdx][parentIdx], costFlight[StartIdx][parentIdx]);
                        provinces[i].calcFlightTrain(timeTrain[StartIdx][parentIdx], costTrain[StartIdx][parentIdx], costFlight[StartIdx][parentIdx]);
                    }
                }
                if(costFlight[StartIdx][parentIdx]>0) {
                    provinces[i].calcFlight(costFlight[StartIdx][parentIdx]);
                }
            }
        }
        realMergeCar();
    }
    
};