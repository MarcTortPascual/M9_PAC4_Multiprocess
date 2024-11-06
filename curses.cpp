#include <atomic>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>

#include <iterator>
#include <memory>
#include <ostream>
#include <string>
#include <thread>
#include <random>
#include <chrono>
#include <vector>
#include <mutex>

#define meta 80
//mutex per blocar el acces al guanyador y al array dels caracters
std::mutex block;
std::mutex block_ganador;
//arrays per posicionar els animals
std::vector<std::string> one;
std::vector<std::string> two;
std::vector<std::vector<std::string>> pista =  {one,two};

typedef struct animal{
    int pos;
    int line;
    std::string simbol;
}Animal_t;
//de moment guanyador no existeix
Animal_t * ganador = NULL;
//funció per dibuxar els animals
void draw(){
    while (!ganador){
        block.lock();
       
        system("clear");
        //printem el array de strings a la terminal
        for (int y = 0; y <= 1 ; y++){
            for (int x = 0; x <= meta ; x++){
                std::cout << pista[y][x];
            }
            std::cout << std::endl;
        }
        //reiniciem el array
          for (int y = 0; y <= 1 ; y++){
            for (int x = 0; x <= meta ; x++){
                pista[y][x] = " ";
            }
        }
        block.unlock();
        std::chrono::seconds dura =  std::chrono::seconds(1);
        std::this_thread::sleep_for(dura);
    }

}
//funció per moure els animals
void race (Animal_t* animal){
    //metres no hagi guanyador && la posició del animal es menor que la meta
    //restem 5 a la meta perque la función rand tingi un rang per calcular 
    //el utim pas y així no provocar un bucle infinit
    while (animal->pos <= (meta-5) && !ganador) {
    
        std::chrono::seconds dura =  std::chrono::seconds(1);
        std::this_thread::sleep_for(dura);
        block.lock();
        //fem que el pas tingi una longitud maxima de 5 
        // just el valor que el restat anteriorment per no caure en un bucle infinit
        animal->pos = animal->pos + ((rand()%5)+1);
        pista[animal->line][animal->pos] = animal->simbol;
        block.unlock();
    }
    //si el animal ariba la meta y no hi ha guanyador proclamem aquest com a guanyador
    if (!ganador){
        block_ganador.lock();
        ganador = animal;
        block_ganador.unlock();
    }
   
}
int main (int argc , char ** argv){
    //iniciem la llavor per la funció rand
    srand(time(NULL));
    //inizializem el array
    for (int y = 0; y <= 1 ; y++){
        for (int x = 0; x <= meta ; x++){
            pista[y].push_back(" ");
        }
    }
    //invoquen un fill per cada animal
    Animal_t one = {0,0,"🐍"};
    Animal_t two = {0,1,"🐘"};
    std::thread t1 = std::thread(race, &one);
    std::thread t2 = std::thread(race, &two);
    //un altre per dibujar els animals
    std::thread t3 = std::thread(draw);
    //esperem a  que acabin els tres fils
    t1.join();
    t2.join();
    t3.join();
    //indiquem al guanyador
    if (ganador){
        std::cout << "ganador: " << ganador->simbol << std::endl;
    }

    
}