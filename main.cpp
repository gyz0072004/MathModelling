#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>
#include "Country.h"

using namespace std;

void prepareScene(vector<Scene>,vector<City>);
void prepareCity(vector<City>);

int main(){
	//init
	Country china;
	vector<Scene> scenelist = china.initScene();//init provinces(SceneData.csv)/ init scenes in provinces
	china.initProvinceDistance();//init distance between province(citycity.csv)
    cout<<"Provinces: "<<china.provinces.size()<<endl;
    vector<City> citylist = china.initCitylist();//init city list for ref(CityData.csv)
	//prepareScene(scenelist,citylist);
	//prepareCity(citylist);
	
	//system("PAUSE");
	china.initCity();//for all province:init cities(SceneDistance*.csv)/init sceneCity* Mat/init distance between cities(CityDistance.csv)/init shortest drive time between scenes
	
	cout<<"Choose Start Province(default:26.XiAn):";
	int start = 1;
	//cin>>start;
	china.setStartProvince(start);
    for(int i=1;i<=31;i++) {
        if(i!=10) {
            china.timeCalcTest(i);
            cout<<"******************"<<endl;
            china.shortestPathTest(i);
            cout<<"******************"<<endl;;
        }
    }
	//system("PAUSE");
	return 0;
}

void prepareScene(vector<Scene> scenelist,vector<City> citylist){
	cout<<"Start Scene data preparing..."<<endl;
	int provinceNum = 31;
	vector<vector<string> > provinces;
	for(int i=0;i<provinceNum;i++){
		vector<string> tmp;
		provinces.push_back(tmp);
	}
	char buffer[1024];
	int sceneidx,cityidx;
	int belong;
	ifstream infile("/Users/gyz/Workspaces/xcode/math/math/data/SceneDistance.csv");
	while(infile>>buffer){
		sscanf(buffer,"%d;%d;",&sceneidx,&cityidx);
		for(int i=0;i<scenelist.size();i++){
			if(scenelist[i].idx==sceneidx){
				belong = scenelist[i].belong;
				//cout<<"scene:"<<sceneidx<<" belong:"<<belong<<endl;
				break;
			}
		}
		for(int i=0;i<citylist.size();i++){
			if(citylist[i].idx==cityidx){
				if(belong==citylist[i].belong){
					provinces[belong-1].push_back(string(buffer));
				}
				break;
			}
		}
	}
	infile.close();
	//write
	ofstream outfile;
	for(int i=0;i<provinceNum;i++){
		char filename[1024];
		sprintf(filename,"/Users/gyz/Workspaces/xcode/math/math/data/SceneDistance%d.csv",i+1);
		cout<<filename<<endl;
		outfile.open(filename);
		for(int j=0;j<provinces[i].size();j++){
			cout<<provinces[i][j]<<endl;
			outfile<<provinces[i][j]<<'\n';
		}
		outfile.close();
	}
	//system("PAUSE");
}

void prepareCity(vector<City> citylist){
	cout<<"Start City data preparing..."<<endl;
	int provinceNum = 31;
	vector<vector<string> > provinces;
	for(int i=0;i<provinceNum;i++){
		vector<string> tmp;
		provinces.push_back(tmp);
	}
	char buffer[1024];
	int startcityidx,endcityidx;
	int belong;
	ifstream infile("/Users/gyz/Workspaces/xcode/math/math/data/CityDistance.csv");
	while(infile>>buffer){
		sscanf(buffer,"%d;%d;",&startcityidx,&endcityidx);
        if(startcityidx==31) {
            //system("PAUSE");
        }
		for(int i=0;i<citylist.size();i++){
			//cout<<i<<endl;
			if(citylist[i].idx==startcityidx){
				belong = citylist[i].belong;
				break;
			}
		}
		for(int i=0;i<citylist.size();i++){
			if(citylist[i].idx==endcityidx){
				if(belong==citylist[i].belong){
					provinces[belong-1].push_back(string(buffer));
					cout<<startcityidx<<"->"<<endcityidx<<":"<<belong<<endl;
				}
				break;
			}
		}
	}
	infile.close();
	cout<<provinces.size()<<endl;
	cout<<provinces[0].size()<<endl;
	//write
	for(int i=0;i<provinceNum;i++){
		char filename[1024];
		sprintf(filename,"/Users/gyz/Workspaces/xcode/math/math/data/CityDistance%d.csv",i+1);
		cout<<filename<<endl;
		ofstream outfile(filename);
		for(int j=0;j<provinces[i].size();j++){
			outfile<<provinces[i][j]<<'\n';
		}
		outfile.close();
	}
	//system("PAUSE");
}