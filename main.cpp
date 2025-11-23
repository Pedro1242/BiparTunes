#include <iostream>
#include "GestorGrafo.h"
// Incluimos estructuras para poder navegar manualmente los punteros en el test
#include "estructuras.h" 

using namespace std;

int main() {
    cout << "=== INICIANDO BIPARTUNES TEST V2.0 (CON RECOMENDACIONES) ===\n" << endl;

    // 1. Instanciamos el Motor
    GestorGrafo app;

    // 2. Cargamos Datos Maestros (Nodos)
    cout << "[PASO 1] Creando Usuarios..." << endl;
    app.agregarUsuario("Juan del Llano", "Colombia"); // ID 1
    app.agregarUsuario("Maria Rockera", "Mexico");    // ID 2
    app.agregarUsuario("Pedro Salsero", "Puerto Rico"); // ID 3

    cout << "\n[PASO 2] Creando Canciones..." << endl;
    // Llanera
    app.agregarCancion("Ay Mi Llanura", "Arnulfo Briceno", LLANERA); // ID 1
    // Rock
    app.agregarCancion("Bohemian Rhapsody", "Queen", ROCK); // ID 2
    // Salsa
    app.agregarCancion("El Cantante", "Hector Lavoe", SALSA); // ID 3

    // 3. Creamos las Relaciones (Aquí es donde los punteros se cruzan)
    cout << "\n[PASO 3] Simulando Interacciones (Aristas)..." << endl;

    // Juan (Llanero) escucha su musica
    // ID Usuario 1, ID Cancion 1, 5 estrellas, Favorito=true, Comentario
    app.registrarInteraccion(1, 1, 5, true, "Me recuerda a mi tierra");

    // Maria (Rockera) escucha Queen
    app.registrarInteraccion(2, 2, 5, true, "Clasico de clasicos");

    // AHORA LA PRUEBA DE FUEGO: CRUCE DE GUSTOS
    // Juan tambien escucha Queen (Asi conectamos el grafo)
    app.registrarInteraccion(1, 2, 4, false, "Buena, pero prefiero el arpa");

    // Maria escucha Llanera (Curiosidad)
    app.registrarInteraccion(2, 1, 3, false, "Interesante ritmo");

    // NUEVA INTERACCIÓN PARA PROBAR RECOMENDACIÓN
    // Maria descubre salsa. Juan NO la ha escuchado.
    // Como Juan y Maria coinciden en Queen y Llanera, el sistema debería pensar:
    // "Si a Maria le gustó El Cantante, a Juan quizás también".
    cout << "\n[PASO 3.5] Maria descubre salsa..." << endl;
    app.registrarInteraccion(2, 3, 5, true, "Sabor latino!");

    // 4. Verificación Visual (Output esperado)
    cout << "\n[PASO 4] Probando Algoritmos Basicos..." << endl;
    
    cout << "Top Canciones (Ordenado por popularidad):" << endl;
    vector<Cancion*> top = app.obtenerTopCanciones();
    
    for(Cancion* c : top) {
        cout << " - " << c->nombre << " (" << c->artista << ") [" << c->totalReproducciones << " repros]" << endl;
    }

    cout << "\n[PASO 5] Probando Recomendaciones Inteligentes para Juan..." << endl;
    // Pedimos recomendaciones para Juan (ID 1)
    vector<Cancion*> recs = app.recomendarParaUsuario(1); 
    
    cout << "\nRESULTADO FINAL:" << endl;
    if(recs.empty()) {
        cout << " :( No hay recomendaciones disponibles." << endl;
    } else {
        cout << "Canciones recomendadas para Juan:" << endl;
        for(Cancion* c : recs) {
            cout << " * " << c->nombre << " (Genero: " << c->genero << ")" << endl;
        }
    }

    cout << "\n=== FIN DE PRUEBAS ===" << endl;
    // Al salir del main, se llama el destructor ~GestorGrafo() automaticamente.
    
    return 0;
}