#include<iostream>
#include<omp.h>
#include<queue>

using namespace std;
#define V 6
void BFS(int adj[V][V]){
	bool visited[V] = {0};
	
	queue<int>que;
	visited[0] = true;
	que.push(0);
	
	while(!que.empty()){
		int node = que.front();
		que.pop();
		
		cout<<node<<" ";
		
		for(int i=0; i<V; i++){
			if(adj[node][i] && !visited[i]){
				visited[i] = true;
				que.push(i);
			}
		}
	}
	
	return;
}

int main(){
	int adj[V][V] = {
		{0,1,0,0,1,0},
		{1,0,1,0,1,0},
		{0,1,0,1,0,0},
		{0,0,1,0,1,1},
		{1,1,0,1,0,0},
		{0,0,0,1,0,0}
	};
	
	BFS(adj);
	return 0;
}
