#include<iostream>

using namespace std;

#define V 6

int minDistance(int dist[], bool sptSet[]){
	int min = INT_MAX, min_index = -1;
	
	for(int v=0; v<V; v++){
		if(!sptSet[v] && dist[v] <= min){
			min = dist[v];
			min_index = v;
		}
	}
	
	return min_index;
}


void printsolution(int dist[]){
	cout<<"Vertex \t Distance from source\n";
	for(int i=0; i<V; i++){
		cout<<"  "<<i<<" \t\t "<<dist[i]<<endl;
	}
}

void dijkstra(int graph[V][V],int src){
	int dist[V];
	bool sptSet[V];
	
	for(int i=0; i<V; i++){
		dist[i] = INT_MAX;
		sptSet[i] = false;
	}
	
	dist[src] = 0;
	
	for(int count=0; count<V-1; count++){
		int u = minDistance(dist,sptSet);
		if(u==-1) break;
		
		sptSet[u] = true;
		
		for(int v=0; v<V; v++){
			if(!sptSet[v] && graph[u][v] && dist[u] != INT_MAX && dist[u] + graph[u][v] < dist[v]){
				dist[v] = dist[u] + graph[u][v];
			}
		}
	}
	
	printsolution(dist);
}

int main(){
	int graph[V][V] = {
	{0,4,4,0,0,0},
	{4,0,2,0,1,0},
	{4,2,0,1,0,0},
	{0,0,1,0,0,2},
	{0,1,0,0,0,4},
	{0,0,0,2,4,0}
	};
	
	dijkstra(graph,0);
}

