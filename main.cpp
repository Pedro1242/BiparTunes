#include <iostream>
#include "GestorGrafo.h"
// Incluimos estructuras para poder navegar manualmente los punteros en el test
#include "estructuras.h" 

using namespace std;

// Función auxiliar para imprimir la lista de canciones (Solo para debug)
void mostrarEstadoMemoria(GestorGrafo& app) {
    cout << "\n--- ESTADO INTERNO DEL GRAFO (MEMORIA) ---\n";
    // Aquí no puedo acceder a listaUsuarios porque es privada en GestorGrafo
    // Pero confiaremos en los logs de consola por ahora.
    // En la version final haremos metodos 'get' o 'friend'.
}

int main() {
    cout << "=== INICIANDO BIPARTUNES TEST V1.0 ===\n" << endl;

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

    // 4. Verificación Visual (Output esperado)
    cout << "\n[PASO 4] Probando Algoritmos Basicos..." << endl;
    
    cout << "Top Canciones (Deberia salir Queen o Ay Mi Llanura primero por actividad):" << endl;
    vector<Cancion*> top = app.obtenerTopCanciones();
    
    // Nota: Como aun no implementamos la logica de ordenamiento en obtenerTopCanciones, 
    // esto probablemente imprimira en el orden que quedaron en la lista.
    // Pero nos sirve para ver que NO explota el programa.
    for(Cancion* c : top) {
        cout << " - " << c->nombre << " (" << c->artista << ")" << endl;
    }

    cout << "\n=== FIN DE PRUEBAS ===" << endl;
    // Al salir del main, se llama el destructor ~GestorGrafo() automaticamente.
    // Si ves "Memoria liberada correctamente", ¡TRIUNFASTE!
    
    return 0;
}