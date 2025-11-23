#include "GestorGrafo.h"
#include <iostream>
#include <fstream>
#include <sstream>

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
    
    // 1. Borrar Usuarios y sus Aristas (Historial)
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

    // 2. Borrar Canciones (Las aristas ya murieron arriba)
    Cancion* cancionActual = listaCanciones;
    while (cancionActual != nullptr) {
        Cancion* aBorrar = cancionActual;
        cancionActual = cancionActual->siguiente;
        delete aBorrar; // Adiós Canción
    }
    
    cout << "[SISTEMA] Memoria liberada correctamente." << endl;
}

// ==========================================
// MÉTODOS PRIVADOS (Auxiliares)
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

// Verifica si un usuario YA tiene esa canción en su historial
bool GestorGrafo::haEscuchado(Usuario* u, Cancion* c) {
    Arista* actual = u->cabezaHistorial;
    while (actual != nullptr) {
        if (actual->cancion == c) return true; // Ya la conoce
        actual = actual->sigEnHistorialUsuario;
    }
    return false;
}

// ==========================================
// LÓGICA DEL GRAFO (Punteros)
// ==========================================

void GestorGrafo::conectarNodos(Usuario* u, Cancion* c, Arista* nuevaArista) {
    // Asignamos los dueños
    nuevaArista->usuario = u;
    nuevaArista->cancion = c;

    // 1. COSTURA HORIZONTAL (Usuario -> Historial)
    nuevaArista->sigEnHistorialUsuario = u->cabezaHistorial;
    u->cabezaHistorial = nuevaArista;

    // 2. COSTURA VERTICAL (Canción -> Oyentes)
    nuevaArista->sigEnOyentesCancion = c->cabezaOyentes;
    c->cabezaOyentes = nuevaArista;
    
    // Estadísticas
    c->totalReproducciones++;
    totalInteracciones++;
}

void GestorGrafo::registrarInteraccion(int idUsuario, int idCancion, int calificacion, 
                                       bool favorito, string comentario) {
    
    Usuario* u = buscarUsuarioPorId(idUsuario);
    Cancion* c = buscarCancionPorId(idCancion);

    if (u == nullptr || c == nullptr) {
        cout << "[ERROR] Usuario o Cancion no encontrados." << endl;
        return;
    }

    Arista* nueva = new Arista();
    nueva->calificacion = calificacion;
    nueva->esFavorito = favorito;
    nueva->comentario = comentario;

    // Cálculo de peso
    float pesoBase = (calificacion * 20.0f);
    if (favorito) pesoBase += 15.0f;
    nueva->pesoFinal = pesoBase;
    
    conectarNodos(u, c, nueva);
    
    cout << "[EXITO] " << u->nombre << " escucho " << c->nombre << " (Peso: " << nueva->pesoFinal << ")" << endl;
}

// ==========================================
// GESTIÓN DE NODOS (ABM)
// ==========================================

void GestorGrafo::agregarUsuario(string nombre, string pais) {
    int nuevoId = totalUsuarios + 1;
    Usuario* nuevo = new Usuario(nuevoId, nombre, pais);
    // Inserción en cabeza (O(1))
    nuevo->siguiente = listaUsuarios;
    listaUsuarios = nuevo;
    totalUsuarios++;
}

void GestorGrafo::agregarCancion(string nombre, string artista, Genero genero) {
    int nuevoId = totalCanciones + 1;
    
    // CORRECCIÓN AQUÍ: Usamos 'nueva' en todo el bloque
    Cancion* nueva = new Cancion(nuevoId, nombre, artista, genero);
    
    // Inserción en cabeza (O(1))
    nueva->siguiente = listaCanciones; // Antes decía 'nuevo'
    listaCanciones = nueva;            // Antes decía 'nuevo'
    
    totalCanciones++;
}

// ==========================================
// ALGORITMOS DE RECOMENDACIÓN
// ==========================================

// 1. TOP CANCIONES (Bubble Sort por Popularidad)
vector<Cancion*> GestorGrafo::obtenerTopCanciones() {
    vector<Cancion*> resultado;
    Cancion* actual = listaCanciones;
    
    // Copiar a vector temporal
    while (actual != nullptr) {
        resultado.push_back(actual);
        actual = actual->siguiente;
    }

    // Ordenamiento Manual (Bubble Sort) de Mayor a Menor
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

// 2. RECOMENDACIÓN COLABORATIVA ("Vecinos")
vector<Cancion*> GestorGrafo::recomendarParaUsuario(int idUsuario) {
    vector<Cancion*> recomendaciones;
    Usuario* usuarioTarget = buscarUsuarioPorId(idUsuario);
    
    if (usuarioTarget == nullptr) return recomendaciones;

    cout << "\n[ALGORITMO] Analizando red de " << usuarioTarget->nombre << "..." << endl;

    // Recorrer historial del usuario
    Arista* miHistorial = usuarioTarget->cabezaHistorial;
    
    while (miHistorial != nullptr) {
        Cancion* cancionQueMeGusta = miHistorial->cancion;
        
        // Ver vecinos (Vertical)
        Arista* otrosOyentes = cancionQueMeGusta->cabezaOyentes;
        while (otrosOyentes != nullptr) {
            Usuario* vecino = otrosOyentes->usuario;
            
            // Si el vecino no soy yo mismo
            if (vecino->id != usuarioTarget->id) {
                
                // Ver historial del vecino
                Arista* historiaVecino = vecino->cabezaHistorial;
                while (historiaVecino != nullptr) {
                    Cancion* candidata = historiaVecino->cancion;
                    
                    // Filtros: Que yo no la tenga Y que no esté ya sugerida
                    bool yaLaTengo = haEscuchado(usuarioTarget, candidata);
                    bool yaSugerida = false;
                    for(Cancion* s : recomendaciones) {
                        if(s == candidata) yaSugerida = true;
                    }

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
// STUBS (Pendientes para futuras fases)
// ==========================================

float GestorGrafo::calcularCompatibilidad(int idUsuarioA, int idUsuarioB) {
    return 0.0f; 
}

void GestorGrafo::cargarDatosDesdeCSV(string archivo) {}
void GestorGrafo::generarDatosAleatorios(int cantidad) {}