#include<iostream>
#include<omp.h>

using namespace std;

#define V 6

int minDistance(int dist[], bool sptSet[]){
	int min = INT_MAX, min_index = -1;
	
	#pragma opm parallel
	{
		int local_min = INT_MAX;
		int local_index = -1;
		
		#pragma omp for nowait
		for(int v=0; v<V; v++){
			if(!sptSet[v] && dist[v] <= local_min){
				local_min = dist[v];
				local_index = v;
			}
		}
		
		#pragma omp critical
		{
			if(local_min < min){
				min = local_min;
				min_index = local_index;
			}
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
		
		#pragma opm parallel for
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

