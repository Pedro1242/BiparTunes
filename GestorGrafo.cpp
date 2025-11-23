#include "GestorGrafo.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib> // Para rand()
#include <ctime>   // Para time()

using namespace std;

// ==========================================
// GESTIÓN DE MEMORIA Y CONSTRUCTOR
// ==========================================

GestorGrafo::GestorGrafo() {
    listaUsuarios = nullptr;
    listaCanciones = nullptr;
    totalUsuarios = 0;
    totalCanciones = 0;
    totalInteracciones = 0;
}

GestorGrafo::~GestorGrafo() {
    cout << "[SISTEMA] Iniciando limpieza de memoria..." << endl;
    
    // 1. Borrar Usuarios y sus historiales
    Usuario* userActual = listaUsuarios;
    while (userActual != nullptr) {
        Usuario* aBorrar = userActual;
        userActual = userActual->siguiente;
        
        // Limpiar las aristas de este usuario
        Arista* aristaActual = aBorrar->cabezaHistorial;
        while (aristaActual != nullptr) {
            Arista* aristaBorrar = aristaActual;
            aristaActual = aristaActual->sigEnHistorialUsuario;
            delete aristaBorrar; // Adiós Arista
        }
        delete aBorrar; // Adiós Usuario
    }

    // 2. Borrar Canciones (Las aristas ya se borraron arriba)
    Cancion* cancionActual = listaCanciones;
    while (cancionActual != nullptr) {
        Cancion* aBorrar = cancionActual;
        cancionActual = cancionActual->siguiente;
        delete aBorrar; // Adiós Canción
    }
    
    cout << "[SISTEMA] Memoria liberada correctamente." << endl;
}

// ==========================================
// MÉTODOS PRIVADOS
// ==========================================

Usuario* GestorGrafo::buscarUsuarioPorId(int id) {
    Usuario* temp = listaUsuarios;
    while (temp != nullptr) {
        if (temp->id == id) return temp;
        temp = temp->siguiente;
    }
    return nullptr;
}

Cancion* GestorGrafo::buscarCancionPorId(int id) {
    Cancion* temp = listaCanciones;
    while (temp != nullptr) {
        if (temp->id == id) return temp;
        temp = temp->siguiente;
    }
    return nullptr;
}

bool GestorGrafo::haEscuchado(Usuario* u, Cancion* c) {
    Arista* actual = u->cabezaHistorial;
    while (actual != nullptr) {
        if (actual->cancion == c) return true;
        actual = actual->sigEnHistorialUsuario;
    }
    return false;
}

// La función estrella: Conecta la multilista
void GestorGrafo::conectarNodos(Usuario* u, Cancion* c, Arista* nuevaArista) {
    nuevaArista->usuario = u;
    nuevaArista->cancion = c;

    // Inserción en cabeza Usuario (Horizontal)
    nuevaArista->sigEnHistorialUsuario = u->cabezaHistorial;
    u->cabezaHistorial = nuevaArista;

    // Inserción en cabeza Canción (Vertical)
    nuevaArista->sigEnOyentesCancion = c->cabezaOyentes;
    c->cabezaOyentes = nuevaArista;
    
    c->totalReproducciones++;
    totalInteracciones++;
}

// ==========================================
// MÉTODOS PÚBLICOS PRINCIPALES
// ==========================================

void GestorGrafo::registrarInteraccion(int idUsuario, int idCancion, int calificacion, 
                                       bool favorito, string comentario) {
    Usuario* u = buscarUsuarioPorId(idUsuario);
    Cancion* c = buscarCancionPorId(idCancion);

    if (u == nullptr || c == nullptr) return;

    Arista* nueva = new Arista();
    nueva->calificacion = calificacion;
    nueva->esFavorito = favorito;
    nueva->comentario = comentario;
    // Inicializamos vecesEscuchada en 1 por defecto
    nueva->vecesEscuchada = 1; 

    // Cálculo de peso simple (Aquí luego meteremos lo de Vibe)
    float pesoBase = (calificacion * 20.0f);
    if (favorito) pesoBase += 15.0f;
    nueva->pesoFinal = pesoBase;
    
    conectarNodos(u, c, nueva);
}

void GestorGrafo::agregarUsuario(string nombre, string pais) {
    int nuevoId = totalUsuarios + 1;
    Usuario* nuevo = new Usuario(nuevoId, nombre, pais);
    // Inserción O(1)
    nuevo->siguiente = listaUsuarios;
    listaUsuarios = nuevo;
    totalUsuarios++;
}

void GestorGrafo::agregarCancion(string nombre, string artista, Genero genero) {
    int nuevoId = totalCanciones + 1;
    Cancion* nueva = new Cancion(nuevoId, nombre, artista, genero);
    // Inserción O(1)
    nueva->siguiente = listaCanciones;
    listaCanciones = nueva;
    totalCanciones++;
}

// ==========================================
// ALGORITMOS (Popularidad & Recomendación)
// ==========================================

vector<Cancion*> GestorGrafo::obtenerTopCanciones() {
    vector<Cancion*> resultado;
    Cancion* actual = listaCanciones;
    while (actual != nullptr) {
        resultado.push_back(actual);
        actual = actual->siguiente;
    }

    // Bubble Sort por Popularidad (Descendente)
    int n = resultado.size();
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (resultado[j]->totalReproducciones < resultado[j + 1]->totalReproducciones) {
                Cancion* temp = resultado[j];
                resultado[j] = resultado[j + 1];
                resultado[j + 1] = temp;
            }
        }
    }
    return resultado;
}

vector<Cancion*> GestorGrafo::recomendarParaUsuario(int idUsuario) {
    vector<Cancion*> recomendaciones;
    Usuario* usuarioTarget = buscarUsuarioPorId(idUsuario);
    
    if (usuarioTarget == nullptr) return recomendaciones;

    cout << "\n[ALGORITMO] Analizando red de " << usuarioTarget->nombre << "..." << endl;

    // Recorremos horizontalmente (Mis gustos)
    Arista* miHistorial = usuarioTarget->cabezaHistorial;
    while (miHistorial != nullptr) {
        Cancion* cancionQueMeGusta = miHistorial->cancion;
        
        // Recorremos verticalmente (Vecinos/Otros oyentes)
        Arista* otrosOyentes = cancionQueMeGusta->cabezaOyentes;
        while (otrosOyentes != nullptr) {
            Usuario* vecino = otrosOyentes->usuario;
            
            // Si no soy yo mismo
            if (vecino->id != usuarioTarget->id) {
                
                // Exploramos qué más escucha el vecino
                Arista* historiaVecino = vecino->cabezaHistorial;
                while (historiaVecino != nullptr) {
                    Cancion* candidata = historiaVecino->cancion;
                    
                    // Filtros: No la he escuchado Y no está ya sugerida
                    bool yaLaTengo = haEscuchado(usuarioTarget, candidata);
                    bool yaSugerida = false;
                    for(Cancion* s : recomendaciones) if(s == candidata) yaSugerida = true;

                    if (!yaLaTengo && !yaSugerida) {
                        recomendaciones.push_back(candidata);
                        cout << "  -> (MATCH) Porque a " << vecino->nombre 
                             << " le gusto '" << cancionQueMeGusta->nombre 
                             << "', te sugiere: '" << candidata->nombre << "'" << endl;
                    }
                    historiaVecino = historiaVecino->sigEnHistorialUsuario;
                }
            }
            otrosOyentes = otrosOyentes->sigEnOyentesCancion;
        }
        miHistorial = miHistorial->sigEnHistorialUsuario;
    }
    return recomendaciones;
}

// ==========================================
// FACTOR SORPRESA & GENERADOR
// ==========================================

float GestorGrafo::calcularCompatibilidad(int idUsuarioA, int idUsuarioB) {
    Usuario* userA = buscarUsuarioPorId(idUsuarioA);
    Usuario* userB = buscarUsuarioPorId(idUsuarioB);

    if (!userA || !userB) return 0.0f;

    float interseccion = 0;
    float union_conjuntos = 0;

    // Recorremos A
    Arista* historialA = userA->cabezaHistorial;
    while (historialA != nullptr) {
        union_conjuntos++;
        if (haEscuchado(userB, historialA->cancion)) interseccion++;
        historialA = historialA->sigEnHistorialUsuario;
    }

    // Recorremos B (sumamos solo lo que A no tiene)
    Arista* historialB = userB->cabezaHistorial;
    while (historialB != nullptr) {
        if (!haEscuchado(userA, historialB->cancion)) union_conjuntos++;
        historialB = historialB->sigEnHistorialUsuario;
    }

    if (union_conjuntos == 0) return 0.0f;
    return (interseccion / union_conjuntos) * 100.0f;
}

void GestorGrafo::generarDatosAleatorios(int cantidadAristas) {
    srand(time(0));
    cout << "[GENERADOR] Verificando poblacion minima..." << endl;

    // Rellenar Usuarios si faltan
    string nombres[] = {"Carlos", "Ana", "Luis", "Sofia", "Jorge", "Laura", "Andres", "Valentina", "Nestor", "Camila"};
    string paises[] = {"Colombia", "Mexico", "Peru", "Argentina", "Chile"};
    while(totalUsuarios < 20) { 
        string n = nombres[rand() % 10] + to_string(rand()%100);
        agregarUsuario(n, paises[rand() % 5]);
    }

    // Rellenar Canciones si faltan
    string artistas[] = {"Shakira", "Juanes", "J Balvin", "Niche", "Diomedes", "Queen", "Nirvana"};
    string titulos[] = {"Amor", "Fiesta", "Dolor", "Noche", "Dia", "Sueno", "Vida"};
    while(totalCanciones < 20) { 
        string t = titulos[rand() % 7] + " " + to_string(rand() % 100);
        agregarCancion(t, artistas[rand() % 7], (Genero)(rand() % 7));
    }

    cout << "[GENERADOR] Insertando " << cantidadAristas << " interacciones aleatorias..." << endl;
    for (int i = 0; i < cantidadAristas; i++) {
        int rUser = (rand() % totalUsuarios) + 1; 
        int rSong = (rand() % totalCanciones) + 1;
        
        Usuario* u = buscarUsuarioPorId(rUser);
        Cancion* c = buscarCancionPorId(rSong);
        
        if (u && c && !haEscuchado(u, c)) {
            registrarInteraccion(rUser, rSong, (rand()%5)+1, (rand()%2), "Generado Auto");
        } else {
            i--; 
            if(i > 10000) break; // Safety break
        }
    }
    cout << "[GENERADOR] Finalizado. Total aristas: " << totalInteracciones << endl;
}

void GestorGrafo::cargarDatosDesdeCSV(string archivo) {
    // Pendiente
}