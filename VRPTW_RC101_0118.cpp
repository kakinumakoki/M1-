#include<iostream>
#include<fstream> //std::ifstreamを使用
#include<algorithm>
#include<map>
#include<queue>
#include<stack>
#include<set>
#include<bitset>
#include<vector>
#include<string>
#include <iomanip>
#include <deque>
#include<list>
#include<cmath>
#include<random>
#include <windows.h>
#include<time.h>

#define rep(i,n) for(int i=0;i<(n);i++)
using namespace std;
using ll=long long;
using P=pair<int,int>;
using P_S=pair<int,string>;
using P_D=pair<double,int>;
using T=tuple<int,int,char,ll,string>;

#define N 101
#define max_car 25
#define limit_time 1236
double max_capacity = 200;
vector<double> point_x(N),point_y(N),demand(N),ready_time(N),due_time(N),service_time(N);
vector<bool> arrived(N,false);

double dist[N][N];

struct Point{
    int x;
    int y;
};

bool all_arrived(){
    rep(i,N){
        if(arrived[i]==false) return false;
    }
    return true;
}

bool ok_time(double time,int now,int next){
    if(time + dist[now][next]>=ready_time[next] && time + dist[now][next]<=due_time[next]) return true;
    return false;
}
bool ok_capacity(double now_capacity,int next)
{
    if(now_capacity + demand[next]<=max_capacity) return true;
    return false;
}
double min_0(double a){
    if(a<0) return 0;
    return a;
}
double max_num(double a,double b)
{
    if(a<b) return b;
    return a;
}
bool cross(Point a,Point b,Point c,Point d){
    double s, t;
    s = (a.x - b.x) * (c.y - a.y) - (a.y - b.y) * (c.x - a.x);
    t = (a.x - b.x) * (d.y - a.y) - (a.y - b.y) * (d.x - a.x);
    if (s * t > 0)
        return false;

    s = (c.x - d.x) * (a.y - c.y) - (c.y - d.y) * (a.x - c.x);
    t = (c.x - d.x) * (b.y - c.y) - (c.y - d.y) * (b.x - c.x);
    if (s * t > 0)
        return false;
    return true;    
}

bool new_root_is_ok_ver1(vector<int>new_root,double& new_dist,vector<double>&v_t)
{
    double t = 0;
    new_dist = 0;
    for(int i=1;i<new_root.size();i++){
        int now = new_root[i-1],nex = new_root[i];
        if(t+dist[now][nex]>due_time[nex]){
            return false;
        }
        new_dist+=dist[now][nex];
        v_t[i]=t+dist[now][nex];
        if(t+dist[now][nex]<=ready_time[nex]) t=ready_time[nex] + service_time[nex];
        else t+=dist[now][nex] + service_time[nex];
    }
    return true;
}

bool new_root_is_ok_ver2(vector<int>new_root,double& new_dist,vector<double>&v_t)
{
    double t=0;
    new_dist = 0;
    double cp=0;
    for(int i=1;i<new_root.size();i++){
        int now = new_root[i-1],nex = new_root[i];
        if(t+dist[now][nex]>due_time[nex]||cp+demand[i]>max_capacity){
            return false;
        }
        new_dist+=dist[now][nex];
        v_t[i]=t+dist[now][nex];
        if(t+dist[now][nex]<=ready_time[nex]) t=ready_time[nex] + service_time[nex];
        else t+=dist[now][nex] + service_time[nex];
    }
    return true;
}

int main()
{
    srand((unsigned)time(NULL));
    vector<double> visited_time(N);
//-----------------------------------input--------------------------
    std::ifstream input_text_data;
    input_text_data.open("RC101.txt");
    while(1){
        if(!input_text_data.eof())
        {
            int number;
            double x;
            double y;
            double capacity;
            double start;
            double end;
            double service;
            input_text_data>>number;
            input_text_data>>x;
            input_text_data>>y;
            input_text_data>>capacity;
            input_text_data>>start;
            input_text_data>>end;
            input_text_data>>service;
            number--;
            point_x[number] = x;
            point_y[number] = y;
            demand[number] = capacity;
            ready_time[number] = start;
            due_time[number] = end;
            service_time[number] = service;
        }
        else break;
    }
    input_text_data.close();

    /*rep(i,N){
        cout<<point_x[i]<<" "<<point_y[i]<<" "<<demand[i]<<" "<<ready_time[i]<<" "<<due_time[i]<<" "<<service_time[i]<<endl;
    }*/
//-------------------準備---------------------------------------------------------
    rep(i,N){
        rep(j,N){
            if(i==j){
                dist[i][j] = 0;
            }
            else{
                double a,b;
                a = point_x[i] - point_x[j];
                b = point_y[i] - point_y[j];
                dist[i][j] = sqrt(a*a+b*b);
            }
        }
    }
//------------------------------------初期解-------------------------------------
    vector<vector<int>> root(max_car);
    rep(i,max_car) root[i].push_back(0);
    int used_car;
    arrived[0] = true;
    visited_time[0] = 0;
    int i=0;
    while(i<max_car){//制限時間内の一つの車の行動決めてから、次の車
        if(all_arrived()) break;
        double t = 0,capacity = 0;;
        int now_point = 0,next;
        while(t<limit_time){
            double min_cost = 10000,cost;
            next=-1;
            for(int j=1;j<N;j++){
                if(arrived[j] == false&&ok_capacity(capacity,j)&&t+dist[now_point][j]<=due_time[j]&&now_point!=j){
                    cost = 0.3*dist[now_point][j]+0.1* (due_time[j] - dist[now_point][j]-t)+ 0.6*min_0(ready_time[j]-t);
                    if(min_cost>cost){
                        min_cost = cost;
                        next = j;
                    }
                }
            }
            if(t+dist[now_point][0]>=limit_time){
                break;
            }
            else if(next==-1){
                if(now_point!=0){
                    t+=dist[now_point][0];
                    root[i].push_back(0);
                    now_point=0;
                    capacity = 0;
                }
                else t++;
            }
            else{
                /*if(t-ready_time[next]<0) {
                    if(t+dist[now_point][0]+dist[0][next]<=due_time[next]&&now_point!=0&&capacity+demand[next]>170) 
                    {
                        t+=dist[now_point][0];
                        root[i].push_back(0);
                        now_point = 0;
                        capacity = 0;
                    }
                }*/
                capacity+=demand[next];
                root[i].push_back(next);
                arrived[next] = true;
                visited_time[next] = t+dist[now_point][next];
                if(ready_time[next]<t+dist[now_point][next]) t+=dist[now_point][next] + service_time[next];
                else t+=ready_time[next] + service_time[next];
                now_point=next;
            }
        }
        i++;
    }
    used_car = i;
    vector<double> result_dist(max_car,0);
    double sum_cost = 0;
    rep(i,max_car){
        for(int j=1;j<root[i].size();j++){
            sum_cost+=dist[root[i][j-1]][root[i][j]];
            result_dist[i]+=dist[root[i][j-1]][root[i][j]];
        }
    }
    /*rep(i,N){//------------------------いつ到着したか----------------------------
        cout<<ready_time[i]<<": "<<visited_time[i]<<" : "<<due_time[i]<<endl;
    }*/
    vector<vector<int>> pre_root(max_car);
    rep(i,max_car){
        if(root[i].size()>1){
        rep(j,root[i].size()) pre_root[i].push_back(root[i][j]);
        }
    }
    double pre_sun_cost=sum_cost;
//-----------------------------------------------最適化-----------------------------
    clock_t start = clock(),end = clock();
    int pattern = 3,in_or_out;
    vector<int> reduce_dist(pattern,0);
    int reduce_dist_out = 0,reduce_dist_cross=0;
    while((double)(end-start)/CLOCKS_PER_SEC<5){
        in_or_out = rand()%2;
        if(in_or_out==0){//-------------------------経路内--------------------
            int change = rand()%pattern;
            int select_car;
            while(1){
                select_car = rand()%used_car;
                if(root[select_car].size()>3) break;//デポ以外の2地点以上めぐる車選択
            }
            vector<int> new_root;
            //cout<<change<<endl;
            if(change == 0){//経路内交換
                int x = (rand()%root[select_car].size())-1,y = (rand()%root[select_car].size())-1;
                if(x<=0) x=1;
                if(y<=0) y=1;
                rep(i,root[select_car].size()) new_root.push_back(root[select_car][i]);
                swap(new_root[x],new_root[y]);
            }
            else if(change == 1 ){//経路内再配置
                int x = (rand()%root[select_car].size())-1,y = (rand()%root[select_car].size())-1;
                if(x<=0) x=1;
                if(y<0) y=0;
                for(int i=0;i<root[select_car].size();i++) {
                    if(i==y) {
                        new_root.push_back(root[select_car][i]);
                        new_root.push_back(root[select_car][x]);
                    }
                    else new_root.push_back(root[select_car][i]);
                }
            }
            else if(change == 2){//2-opt
                int x = (rand()%root[select_car].size())-1,y = (rand()%root[select_car].size())-1;
                if(x<=0) x=1;
                if(y<=0) y=1;
                if(x>y) swap(x,y);
                rep(i,root[select_car].size()) new_root.push_back(root[select_car][i]);
                reverse(new_root.begin()+x,new_root.begin()+y);
            }
            vector<double>v_t(new_root.size());
            v_t[0] = 0;
            int flag = 0;
            double t = 0;
            double new_dist = 0;
            if(!new_root_is_ok_ver1(new_root,new_dist,v_t)) flag = 1;
            if(flag==0&&new_dist<result_dist[select_car]){
                reduce_dist[change]++;
                cout<<"!!update!!"<<endl;
                result_dist[select_car]=new_dist;
                rep(i,root[select_car].size()){
                    root[select_car][i]=new_root[i];
                    visited_time[new_root[i]]=v_t[i];
                }
            }
        }
        else{//---------------------------------経路外----------------------
            int pattern_out = 2;
            int change=rand()%pattern_out;
            if(change==0){//-----------------------経路間再配置----------------------
                int select_1,select_2;
                select_1 = rand()%used_car;
                while(1){
                    select_2 = rand()%used_car;
                    if(select_1!=select_2) break;
                }
                int x = (rand()%root[select_1].size())-1,y = (rand()%root[select_2].size())-1;
                if(x<=0) x=1;
                if(y<0) y=0;
                vector<int>new_root1,new_root2;
                rep(i,root[select_1].size()){
                    if(i!=x) new_root1.push_back(root[select_1][i]);
                }
                rep(i,root[select_2].size()){
                    new_root2.push_back(root[select_2][i]);
                    if(i==y) new_root2.push_back(root[select_1][x]);
                }
                vector<double>v_t1(new_root1.size()),v_t2(new_root2.size());
                v_t1[0] = 0;
                v_t2[0] = 0;
                int flag = 0;
                double t = 0,cp = 0;
                double new_dist1 = 0,new_dist2 = 0;
                if(!new_root_is_ok_ver2(new_root1,new_dist1,v_t1)) flag=1;
                if(flag == 0){
                    if(!new_root_is_ok_ver2(new_root2,new_dist2,v_t2)) flag=1;
                }
                if(flag == 0 &&result_dist[select_1]+result_dist[select_2]>new_dist1+new_dist2){
                    cout<<"!!update!!"<<endl;
                    reduce_dist_out++;
                    result_dist[select_1]=new_dist1;
                    rep(i,new_root1.size()){
                        root[select_1][i]=new_root1[i];
                        visited_time[new_root1[i]]=v_t1[i];
                    }
                    root[select_1].pop_back();

                    result_dist[select_2]=new_dist2;
                    rep(i,root[select_2].size()){
                        root[select_2][i]=new_root2[i];
                        visited_time[new_root2[i]]=v_t2[i];
                    }
                    root[select_2].push_back(0);
                }
            }
            else{//---------------------経路間交差----------------------------------
                int select_1;
                bool finish = false;
                while(1){
                    select_1 = rand()%used_car;
                    if(root[select_1].size()>2) break;
                }
                int x = (rand()%root[select_1].size())-2;
                if(x<0) x=0;
                Point a,b,c,d;
                vector<int> new_root1;
                rep(i,root[select_1].size()) new_root1.push_back(root[select_1][i]);
                a.x=root[select_1][x];
                a.y=root[select_1][x];
                b.x=root[select_1][x+1];
                b.y=root[select_1][x+1];
                rep(i,used_car){
                    if(i!=select_1&&root[i].size()>2){
                        rep(j,root[i].size()-2){
                            c.x=root[i][j];
                            c.y=root[i][j];
                            d.x=root[i][j+1];
                            d.y=root[i][j+1];
                            if(cross(a,b,c,d)){
                                vector<int> new_root2;
                                rep(k,root[i].size()) new_root2.push_back(root[i][k]);
                                swap(new_root1[x+1],new_root2[j+1]);
                                vector<double>v_t1(new_root1.size()),v_t2(new_root2.size());
                                v_t1[0] = 0;
                                v_t2[0] = 0;
                                int flag = 0;
                                double new_dist1 = 0,new_dist2 = 0;
                                if(!new_root_is_ok_ver2(new_root1,new_dist1,v_t1)) flag=1;
                                if(flag == 0){
                                    if(!new_root_is_ok_ver2(new_root2,new_dist2,v_t2)) flag=1;
                                }
                                if(flag == 0&&result_dist[select_1]+result_dist[i]>new_dist1+new_dist2){
                                    finish = true;
                                    cout<<"!!cross_update!!"<<endl;
                                    reduce_dist_cross++;
                                    result_dist[select_1]=new_dist1;
                                    /*rep(k,root[select_1].size()) cout<<root[select_1][k]<<" ";
                                    cout<<endl;*/
                                    rep(k,new_root1.size()){
                                        root[select_1][k]=new_root1[k];
                                        visited_time[new_root1[k]]=v_t1[k];
                                    }
                                    /*rep(k,root[select_1].size()) cout<<root[select_1][k]<<" ";
                                    cout<<endl;
                                    rep(k,root[i].size()) cout<<root[i][k]<<" ";
                                    cout<<endl;*/
                                    result_dist[i]=new_dist2;
                                    rep(k,new_root2.size()){
                                        root[i][k]=new_root2[k];
                                        visited_time[new_root2[k]]=v_t2[k];
                                    }
                                    /*rep(k,root[i].size()) cout<<root[i][k]<<" ";
                                    cout<<endl;*/
                                    //return 0;
                                }
                                else swap(new_root1[x+1],new_root2[j+1]);
                            }
                        }
                    }
                    if(finish) break;
                }
            }
        }
        end = clock();
    }


    /*rep(i,N){//時間内に訪れてるか確認
        if(due_time[i] < visited_time[i]) cout<<"NG : "<<i<<endl;
        cout<< ready_time[i] <<" "<<visited_time[i]<<" "<<due_time[i]<<endl;
    }*/
    rep(i,N){
        if(due_time[i]-visited_time[i]<0) {
            cout<<i<<" : "<<visited_time[i]<<" : "<<due_time[i];
            cout<<"NG"<<endl;
            return 0;
        }
    }

//--------------------------結果--------------------------------------------------
    sum_cost = 0;
    rep(i,max_car){//------------------------------------総距離計算-------------------
        for(int j=1;j<root[i].size();j++){
            sum_cost+=dist[root[i][j-1]][root[i][j]];
        }
    }
    rep(i,max_car){//---------------------------root表示------------------------
        if(root[i].size()>1){
            printf("%2d:",i);
            rep(j,root[i].size()) cout<<root[i][j]<<" ";
            cout<<" = "<<result_dist[i]<<endl;
        }
    }
    rep(i,N){//------------------------いつ到着したか----------------------------
        cout<<ready_time[i]<<": "<<visited_time[i]<<" : "<<due_time[i]<<endl;
    }
    //---------------------全拠点訪れてるか-----------------------------
    rep(i,N) arrived[i] = false;
    rep(i,used_car){
        rep(j,root[i].size()) arrived[root[i][j]] = true;
    } 
    rep(i,N){
        if(arrived[i]==false) {
            cout<<i<<endl;
            cout<<"NG"<<endl;
            return 0;
        }
    }
    cout<<"OK"<<endl;
    //-------------------------------------なんの操作で距離減少したか-----------------------------
    int non=0;
    rep(i,used_car){
        if(result_dist[i]==0) non++;
    }
    cout<<"pre_used_car: "<<used_car<<endl;   
    cout<<"used_car: "<<used_car - non<<endl;
    cout<<"pre_sum_cost: "<<pre_sun_cost<<endl;
    cout<<"sum_cost: "<<sum_cost<<endl;
    rep(i,pattern){
        string s;
        if(i==0) s="change";
        else if(i==1) s="re_set";
        else if(i==2)s="2-opt";
        cout<<s<<" : "<<reduce_dist[i]<<endl;
    }
    cout<<"out"<<" : "<<reduce_dist_out<<endl;
    cout<<"cross"<<" : "<<reduce_dist_cross<<endl;
//---------------------------------書き出し------------------------------
    std:: ofstream output_text_data;
    output_text_data.open("RC101_c6_output.csv");
    rep(i,max_car){
        if(pre_root[i].size()>1){
            rep(j,pre_root[i].size()){
                output_text_data<<point_x[pre_root[i][j]]<<","<<point_y[pre_root[i][j]]<<"\n";
            }
            output_text_data<<"\n";
        }
    }
    output_text_data<<"\n!!!!!!";
    output_text_data<<"\n";
    rep(i,max_car){
        if(root[i].size()>1){
            rep(j,root[i].size()){
                output_text_data<<point_x[root[i][j]]<<","<<point_y[root[i][j]]<<"\n";
            }
            output_text_data<<"\n";
        }
    }
    
    output_text_data.close();
}