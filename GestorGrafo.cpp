#include "GestorGrafo.h"
#include <iostream>
#include <fstream> // Para leer el CSV más adelante
#include <sstream>

using namespace std;

// Constructor: Inicializa el grafo vacío
GestorGrafo::GestorGrafo() {
    listaUsuarios = nullptr;
    listaCanciones = nullptr;
    totalUsuarios = 0;
    totalCanciones = 0;
    totalInteracciones = 0;
}

// Destructor: LA LIMPIEZA MANUAL (Vital para tu nota)
GestorGrafo::~GestorGrafo() {
    // Aquí demostramos que sabemos borrar memoria sin dejar fugas
    // 1. Borrar Usuarios
    Usuario* userActual = listaUsuarios;
    while (userActual != nullptr) {
        Usuario* aBorrar = userActual;
        userActual = userActual->siguiente;
        
        // OJO: Las aristas se comparten. 
        // Estrategia: Borraremos las aristas al recorrer los usuarios, 
        // y cuando borremos canciones solo borramos el nodo canción.
        
        Arista* aristaActual = aBorrar->cabezaHistorial;
        while (aristaActual != nullptr) {
            Arista* aristaBorrar = aristaActual;
            aristaActual = aristaActual->sigEnHistorialUsuario;
            delete aristaBorrar; // Adiós Arista
        }
        delete aBorrar; // Adiós Usuario
    }

    // 2. Borrar Canciones (Las aristas ya murieron arriba)
    Cancion* cancionActual = listaCanciones;
    while (cancionActual != nullptr) {
        Cancion* aBorrar = cancionActual;
        cancionActual = cancionActual->siguiente;
        delete aBorrar; // Adiós Canción
    }
    
    cout << "[SISTEMA] Memoria liberada correctamente." << endl;
}

// --- MÉTODOS DE BÚSQUEDA (Privados) ---

Usuario* GestorGrafo::buscarUsuarioPorId(int id) {
    Usuario* temp = listaUsuarios;
    // Recorrido clásico de lista enlazada
    while (temp != nullptr) {
        if (temp->id == id) return temp;
        temp = temp->siguiente;
    }
    return nullptr; // No existe
}

Cancion* GestorGrafo::buscarCancionPorId(int id) {
    Cancion* temp = listaCanciones;
    while (temp != nullptr) {
        if (temp->id == id) return temp;
        temp = temp->siguiente;
    }
    return nullptr;
}

// --- LA MAGIA: CONECTAR NODOS (TEJIDO DE PUNTEROS) ---

void GestorGrafo::conectarNodos(Usuario* u, Cancion* c, Arista* nuevaArista) {
    // Asignamos los dueños de la arista
    nuevaArista->usuario = u;
    nuevaArista->cancion = c;

    // 1. COSTURA HORIZONTAL (Lado del Usuario)
    // La nueva arista apunta a lo que antes era la primera del usuario
    nuevaArista->sigEnHistorialUsuario = u->cabezaHistorial;
    // El usuario ahora apunta a la nueva arista como su primera
    u->cabezaHistorial = nuevaArista;

    // 2. COSTURA VERTICAL (Lado de la Canción)
    // La nueva arista apunta a la antigua primera interacción de la canción
    nuevaArista->sigEnOyentesCancion = c->cabezaOyentes;
    // La canción actualiza su cabeza para apuntar a esta nueva interacción
    c->cabezaOyentes = nuevaArista;
    
    // Aumentamos contadores para estadísticas rápidas
    c->totalReproducciones++;
    totalInteracciones++;
}

// --- REGISTRAR INTERACCIÓN (El método público) ---

void GestorGrafo::registrarInteraccion(int idUsuario, int idCancion, int calificacion, 
                                       bool favorito, string comentario) {
    
    Usuario* u = buscarUsuarioPorId(idUsuario);
    Cancion* c = buscarCancionPorId(idCancion);

    if (u == nullptr || c == nullptr) {
        cout << "[ERROR] Usuario o Cancion no encontrados." << endl;
        return;
    }

    // Creamos la arista en memoria dinámica (Heap)
    Arista* nueva = new Arista();
    
    // Llenamos datos básicos
    nueva->calificacion = calificacion;
    nueva->esFavorito = favorito;
    nueva->comentario = comentario;

    // CÁLCULO DE PESO "REVOLUCIONARIO"
    // Fórmula: (Stars * 20) + (Favorito extra 15)
    float pesoBase = (calificacion * 20.0f);
    if (favorito) pesoBase += 15.0f;
    
    // Aquí en el futuro agregaremos lógica de Vibe, por ahora peso numérico
    nueva->pesoFinal = pesoBase;
    
    // Conectamos los cables
    conectarNodos(u, c, nueva);
    
    cout << "[EXITO] " << u->nombre << " escucho " << c->nombre << " (Peso: " << nueva->pesoFinal << ")" << endl;
}

// --- AGREGAR NODOS MAESTROS ---

void GestorGrafo::agregarUsuario(string nombre, string pais) {
    // ID Autoincremental simple basado en conteo actual
    int nuevoId = totalUsuarios + 1;
    
    Usuario* nuevo = new Usuario(nuevoId, nombre, pais);
    
    // Inserción en cabeza para la lista maestra de usuarios
    nuevo->siguiente = listaUsuarios;
    listaUsuarios = nuevo;
    
    totalUsuarios++;
}

void GestorGrafo::agregarCancion(string nombre, string artista, Genero genero) {
    int nuevoId = totalCanciones + 1;
    
    Cancion* nueva = new Cancion(nuevoId, nombre, artista, genero);
    
    // Inserción en cabeza para la lista maestra de canciones
    nueva->siguiente = listaCanciones;
    listaCanciones = nueva;
    
    totalCanciones++;
}