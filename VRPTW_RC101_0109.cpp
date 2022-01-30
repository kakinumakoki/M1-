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
vector<double>now_capacity(max_car,0);

double dist[N][N];

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
bool ok_capacity(int now,int next)
{
    if(now_capacity[now] + demand[next]<=max_capacity) return true;
    return false;
}

int main()
{
    srand(time(NULL));
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
    vector<int>now_point(max_car,0);
    vector<double>wait_counter(max_car,0);
    int used_car;
    arrived[0] = true;
    visited_time[0] = 0;
    int i=0;
    while(i<max_car){//制限時間内の一つの車の行動決めてから、次の車
        if(all_arrived()) break;
        double time=0;
        while(time<=limit_time){
            if(wait_counter[i]<=0){
                int next = -1;
                double min_dist = 5000;
                /*for(int j=1;j<N;j++){//今自分のいる地点から最も近くで回収時間を満たしている地点に行く
                    if(arrived[j] == false && min_dist>dist[now_point[i]][j]&&ok_time(time,now_point[i],j)&&ok_capacity(i,j) && now_point[i]!=j){
                        if(i==7) cout<<j<<endl;
                        min_dist = dist[now_point[i]][j];
                        next = j;
                    }
                }*/
                double max_dist = 0;
                /*for(int j=1;j<N;j++){//今デポにいるなら解種可能な最も遠く、それ以外は自分のいる地点から最も近くで回収時間を満たしている地点に行く
                    if(now_point[i]==0){
                        if(arrived[j] == false && max_dist<dist[now_point[i]][j]&&ok_time(time,now_point[i],j)&&ok_capacity(i,j) && now_point[i]!=j){
                            max_dist = dist[now_point[i]][j];
                            next = j;
                        }
                    }
                    else{
                        if(arrived[j] == false && min_dist>dist[now_point[i]][j]&&ok_time(time,now_point[i],j)&&ok_capacity(i,j) && now_point[i]!=j){
                            min_dist = dist[now_point[i]][j];
                            next = j;
                        }                        
                    }
                }*/
                double min_cost = 1000;
                for(int j=1;j<N;j++){
                    if(arrived[j] == false &&ok_time(time,now_point[i],j)&&ok_capacity(i,j) && now_point[i]!=j){
                        double cost;
                        cost = 0.7 * dist[now_point[i]][j] + 0.3 * (due_time[j] - dist[now_point[i]][j]- time);
                        if(min_cost>cost){
                            min_cost = cost;
                            next = j;
                        }
                    }
                }

                if(next==-1 && now_point[i]!=0){
                    root[i].push_back(0);
                    now_point[i] = 0;
                }
                else if(next != -1){
                    visited_time[next] = time + dist[now_point[i]][next];
                    wait_counter[i] = dist[now_point[i]][next] + service_time[next];
                    now_point[i] = next;
                    now_capacity[i]+= demand[next];
                    arrived[next] = true;
                    root[i].push_back(next);
                }
            }
            else wait_counter[i]-=0.1;
            time+=0.1;
        }
        i++;
    }
    used_car = i;
    rep(i,N){
        if(ready_time[i] > visited_time[i] || due_time[i] < visited_time[i]) cout<<"NG : "<<i<<endl;
    }
    vector<double> result_dist(max_car,0);
    double sum_cost = 0;
    rep(i,max_car){
        for(int j=1;j<root[i].size();j++){
            sum_cost+=dist[root[i][j-1]][root[i][j]];
            result_dist[i]+=dist[root[i][j-1]][root[i][j]];
        }
    }
    cout<<"before sum_cost : "<<sum_cost<<endl;
    rep(i,N){
        if(due_time[i]-visited_time[i]<0) {
            cout<<"NG"<<endl;
            return 0;
        }
    }
//-------------------------------------近傍探索-----------------------------------
    clock_t start = clock(),end = clock();
    while((double)(end-start)/CLOCKS_PER_SEC<3){
        int select_car;
        while(1){
            select_car = rand()%used_car;
            if(root[select_car].size()>4) break;
        }
        int x,y;
        while(1){
            x = rand()%(root[select_car].size()-3)+1,y=rand()%(root[select_car].size()-3)+1;//最初と最後のデポは選びたくない
            if(x!=y) break;
        }
        vector<int> copy_root(root[select_car].size());
        vector<double> v_t(root[select_car].size());
        v_t[0] = 0;
        int flag=0;
        rep(i,copy_root.size()) copy_root[i]=root[select_car][i];
        swap(copy_root[x],copy_root[y]);
        double t = 0,cp = 0,cal_dist = 0;
        for(int i = 1;i<copy_root.size();i++){
            t+=dist[copy_root[i-1]][copy_root[i]];
            v_t[i] = t;
            if(copy_root[i]!=0)cp+=demand[copy_root[i]];
            else cp=0;
            cal_dist+=dist[copy_root[i-1]][copy_root[i]];
            if(ready_time[copy_root[i]]>t||t>due_time[copy_root[i]] && cp>=200){
                flag = 1;
                break;
            }
            else t+=service_time[copy_root[i]];
        }
        if(flag == 0){
            if(result_dist[select_car]>cal_dist){
                cout<<"!!! update !!!"<<endl;
                result_dist[select_car] = cal_dist;
                rep(i,root[select_car].size()) {
                    root[select_car][i] = copy_root[i];
                    visited_time[copy_root[i]] = v_t[i];
                }
            }
        }
        end = clock();
    }
    sum_cost = 0;
    rep(i,max_car){
        for(int j=1;j<root[i].size();j++){
            sum_cost+=dist[root[i][j-1]][root[i][j]];
            result_dist[i]+=dist[root[i][j-1]][root[i][j]];
        }
    }
//--------------------------結果--------------------------------------------------
    rep(i,max_car){
        if(root[i].size()>1){
        printf("%2d:",i);
        rep(j,root[i].size()) cout<<root[i][j]<<" ";
        cout<<endl<<"!RUN_DIST!:"<<result_dist[i]<<endl;
        }
    }
    rep(i,N){
        cout<<i<<": "<<due_time[i] - visited_time[i]<<endl;
    }

    if(all_arrived()) cout<<"OK"<<endl;
    else {
        cout<<"NG"<<endl;
        rep(i,N){
            if(arrived[i]==false) cout<<i<<" ";
        }
        cout<<endl;
    }
    cout<<used_car<<endl;
    cout<<sum_cost<<endl;

    std:: ofstream output_text_data;
    output_text_data.open("RC101_output.csv");
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