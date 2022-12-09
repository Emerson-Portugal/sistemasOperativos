#include <iostream>
#include <thread>
#include <queue>//ara generar una cola de entrada y salida de los caracteres del alfabeto.
#include <mutex> //para los estados lock o unlocked, esto funciona como un candado para el monitor

using namespace std;

// g++ monitorFinal.cpp -lpthread

#define NUM_HILOS 10

queue<char> cola;
const char abededario[27] ={'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};

int cont=0;

mutex flag;

//Los productores y consumidores se comunican con el monitor. 
//Se creó un método (insertar) mediante el cual el productor activará una operación que es insertar. 
//Con una estructura similar se creó un método (extraer) mediante el cual el consumidor activará una operación que es extraer.
class Monitor {
	public:
    //El método Monitor, muestra el comportamiento de los productores y consumidores.
	void producir(int productor){
		flag.lock();	// LOCK
    
		int j = rand() % 26;
		char letra = abededario[j];
		cola.push(letra);
		cout<<"Productor numero "<<productor<<" ha producido: "<<letra<<"\n";
        //Apertura
		flag.unlock();	// UNLOCK		
	}
	
	void consumir(int consumidor){
        //cierre
	  flag.lock();	// LOCK
    
		int j = rand() % 26;
		char letra = abededario[j];
		cola.pop();
		cout<<"Consumidor numero "<<consumidor<<" ha consumido: "<<letra<<"\n";
    
		flag.unlock();	// UNLOCK		
	}
};
//La clase productor solo puede añadir caracteres al buffer mediante el método insertar.
class Productor {
	private:
		Monitor* monitor;
		thread t;
		int id;
        //El mètodo run_thread(), se invoca a sí mismo,
		void run_thread(){			
			for (int i = 1; true; i++) {
			    monitor->producir(id);		//	RUN
			}
		}
			
	public:
		Productor(Monitor* monitor1, int identificadorP){
			id = identificadorP;
			monitor = monitor1;

			t = thread(&Productor::run_thread, this);
		}
        //mientras el método join_thread() dará inicio a la ejecución.
		void join_thread(){
			t.join();	// START
		}
		
};
//La clase consumidor solo puede extraer caracteres del buffer mediante el método extraer.
class Consumidor {
	private:
		Monitor* monitor;
		thread t;
		int id;
        //El mètodo run_thread(), se invoca a sí mismo, 
		void run_thread(){			
			for (int i = 1; true; i++) {
			    monitor->consumir(id);		//	RUN
			}
		}
			
	public:
		Consumidor(Monitor* monitor2, int identificadorC){
			id = identificadorC;
			monitor = monitor2;
			t = thread(&Consumidor::run_thread, this);
		}
        //mientras el método join_thread() dará inicio a la ejecución.
		void join_thread(){
			t.join();	// START
		}
		
};


int main() {
// damos por iniciado al productor, conumidor y monitor
	Productor* productor[NUM_HILOS];
	Consumidor* consumidor[NUM_HILOS];
	Monitor* monitor;
	
	int i;
	for(i=0; i < NUM_HILOS; i++) {
		productor[i] = new Productor(monitor, i);
		consumidor[i] = new Consumidor(monitor, i);
	}
	
	for(i=0; i < NUM_HILOS; i++) {
		productor[i]->join_thread();	//	START
	  consumidor[i]->join_thread();	//	START
	}

	return 0;
}