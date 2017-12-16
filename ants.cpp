#include <iostream>
#include <bits/stdc++.h>
#include <cmath>

#define MAX_CITIES 30
#define MAX_DIST 100
#define MAX_TOUR (MAX_CITIES * MAX_DIST)
#define MAX_ANTS MAX_CITIES

#define ALPHA 1.0
#define BETA 5.0
#define RHO 0.5
#define QVAL 100
#define MAX_TOURS 20
#define MAX_ITER (MAX_TOURS * MAX_CITIES)
#define INIT_PHER (1.0/MAX_CITIES)

using namespace std;

struct City
{
    int x,y;
};

struct Ant
{
    int currCity,nextCity;
    int numcities;
    int path[MAX_CITIES];
    bool visited[MAX_CITIES];
    double tourlen;
};

City cities[MAX_CITIES];
double dist[MAX_CITIES][MAX_CITIES];
double phero[MAX_CITIES][MAX_CITIES];
Ant ants[MAX_ANTS];
double best=(double)MAX_TOUR;
int bestIndex;

void init()
{
    int from,to;
    for(from = 0; from < MAX_CITIES; from++)
    {
        cities[from].x=rand()%MAX_DIST;
        cities[from].y=rand()%MAX_DIST;
        cout <<"City "<<from<<": x="<< cities[from].x << ", y=" << cities[from].y << endl;
        for(to = 0; to < MAX_CITIES; to++)
        {
            dist[from][to] = 0.0;
            phero[from][to] = INIT_PHER;
        }
    }
    for(from = 0; from < MAX_CITIES; from++)
    for(to = 0; to < MAX_CITIES; to++)
    {
        if(to!=from && dist[from][to]==0)
        {
            int xi=pow(cities[from].x-cities[to].x,2);
            int yi=pow(cities[from].y-cities[to].y,2);
            dist[from][to]=sqrt(xi+yi);
            dist[to][from]=dist[from][to];
        }
    }

    to = 0;
    for(int i = 0; i < MAX_ANTS; i++)
    {
        ants[i].currCity = to++;

        for(from = 0; from<MAX_CITIES; from++)
        {
            ants[i].visited[from] = false;
            ants[i].path[from] = -1;
        }

        ants[i].path[0] = ants[i].currCity;
        ants[i].nextCity = -1;
        ants[i].tourlen = 0.0;
        ants[i].visited[ants[i].currCity] = true;
        ants[i].numcities = 1;
    }

}

void restartAnts()
{
    for(int ant = 0; ant < MAX_ANTS; ant++)
    {
        if(ants[ant].tourlen < best)
        {
            best=ants[ant].tourlen;
            bestIndex = ant;
        }
        ants[ant].nextCity = -1;
        ants[ant].tourlen = 0.0;
        for(int i = 0; i < MAX_CITIES; i++)
        {
            ants[ant].visited[i] = 0;
            ants[ant].path[i] = -1;
        }

        ants[ant].currCity = ant;
        ants[ant].path[0] = ant;
        ants[ant].visited[ant] = 1;
        ants[ant].numcities = 1;
    }
}

double antProduct(int from, int to)
{
	return((pow(phero[from][to], ALPHA)*pow((1.0/ dist[from][to]), BETA)));
}

double fRand(double fMin, double fMax)
{
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

int selectNextCity(int ant)
{
    ants[ant].visited[ants[ant].currCity] = 1;
	int from, to;
	vector<double> p;
	vector<int> pcity;
	double denom = 0.0;
	from=ants[ant].currCity;

	for(to = 0; to < MAX_CITIES; to++)
	{
        if(ants[ant].visited[to] == 0)
        {
            double pp = antProduct(from,to);
            denom += pp;
            p.push_back(pp);
            pcity.push_back(to);
        }
	}
	if(denom == 0.0)
	{
        cout<<"Denom je 0"<<endl;
        exit(1);
	}
	double sum = 0.0;
	double x = fRand(0.0,denom);
    for(int i = 0; i < pcity.size(); i++)
    {
        sum += p[i];
        if(x < sum) return pcity[i];
    }
    return pcity[pcity.size()-1];
}

int simulateAnts()
{
    int moving = 0;
    for(int i = 0; i < MAX_ANTS; i++)
    {
        if(ants[i].numcities < MAX_CITIES)
        {
            ants[i].nextCity=selectNextCity(i);
            ants[i].visited[ants[i].nextCity] = 1;
            ants[i].path[ants[i].numcities++] = ants[i].nextCity;
            ants[i].tourlen += dist[ants[i].currCity][ants[i].nextCity];
            if(ants[i].numcities == MAX_CITIES)
            {
                ants[i].tourlen += dist[ants[i].path[MAX_CITIES-1]][ants[i].path[0]];
            }
            ants[i].currCity = ants[i].nextCity;
            moving++;
        }
    }
    return moving;
}

void updateTrails()
{
    int from,to,ant;

    for(from = 0; from < MAX_CITIES; from++)
	{
		for(to = 0; to < MAX_CITIES; to++)
		{
			if(from != to)
			{
				phero[from][to] *= (1.0 - RHO);

				if(phero[from][to] < 0.0)
				{
					phero[from][to] = INIT_PHER;
				}
			}
		}
	}
	for(ant = 0; ant < MAX_ANTS; ant++)
	{
		for(int i = 0; i < MAX_CITIES; i++)
		{
			if(i < MAX_CITIES-1)
			{
				from = ants[ant].path[i];
				to = ants[ant].path[i+1];
			}
			else
			{
				from = ants[ant].path[i];
				to = ants[ant].path[0];
			}

			phero[from][to] += (QVAL/ ants[ant].tourlen);
			phero[to][from] = phero[from][to];

		}
	}

	for (from = 0; from < MAX_CITIES; from++)
	{
		for(to=0; to<MAX_CITIES; to++)
		{
			phero[from][to] *= RHO;
		}
	}
}

int main()
{
    int iterations=0;
    srand(time(nullptr));
    init();
    while(iterations++ < MAX_ITER)
    {
        if( simulateAnts() == 0)
		{
			updateTrails();

			if(iterations != MAX_ITER)
				restartAnts();

			cout<<"Iteration "<<iterations<<" ("<<best<<")"<<endl;

		}
    }
    cout << "Best tour: "<<best<< endl;
    return 0;
}



