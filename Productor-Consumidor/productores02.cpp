#include<stdio.h>
#include<thread>
#include<stdlib.h>

using namespace std;

// g++ productores02.cpp -lpthread

#define NUM_PRODUCTORES 2

const char letras[26]={'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};

//void productor (int k);
class Productor {
	private:
		thread t;
		int k;
		void run_thread() {
			int aleatorio;
			for (int i=1; true; i++) {
				aleatorio = rand() % 26;
				printf ("(%d) Productor-%d estoy produciendo : %c \n", i, k, letras[aleatorio]);
			}
		}
	public:
		Productor(int id) {
			k = id;
			t = thread(&Productor::run_thread, this);
			//t.join();
		}
		void join_thread(){
			t.join();
		}
};

int main(void)
{
	Productor* productores[NUM_PRODUCTORES];
	int identificadores[NUM_PRODUCTORES];

	int i;

	for (i=0; i<NUM_PRODUCTORES; i++)	{		
		productores[i] = new Productor(i); 
	}

	
	for (i=0; i<NUM_PRODUCTORES; i++)	{
		productores[i]->join_thread();
	}

	return 0;
}


