#include "GestorGrafo.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib> 
#include <ctime>   
#include <algorithm> // Para transformar string a minusculas

using namespace std;

// ==========================================
// GESTIÓN DE MEMORIA
// ==========================================

GestorGrafo::GestorGrafo() {
    listaUsuarios = nullptr;
    listaCanciones = nullptr;
    totalUsuarios = 0;
    totalCanciones = 0;
    totalInteracciones = 0;
}

GestorGrafo::~GestorGrafo() {
    cout << "[SISTEMA] Limpiando memoria..." << endl;
    Usuario* userActual = listaUsuarios;
    while (userActual != nullptr) {
        Usuario* aBorrar = userActual;
        userActual = userActual->siguiente;
        Arista* aristaActual = aBorrar->cabezaHistorial;
        while (aristaActual != nullptr) {
            Arista* aristaBorrar = aristaActual;
            aristaActual = aristaActual->sigEnHistorialUsuario;
            delete aristaBorrar; 
        }
        delete aBorrar; 
    }
    Cancion* cancionActual = listaCanciones;
    while (cancionActual != nullptr) {
        Cancion* aBorrar = cancionActual;
        cancionActual = cancionActual->siguiente;
        delete aBorrar; 
    }
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

void GestorGrafo::conectarNodos(Usuario* u, Cancion* c, Arista* nuevaArista) {
    nuevaArista->usuario = u;
    nuevaArista->cancion = c;
    
    // Inserción en cabeza Usuario
    nuevaArista->sigEnHistorialUsuario = u->cabezaHistorial;
    u->cabezaHistorial = nuevaArista;

    // Inserción en cabeza Canción
    nuevaArista->sigEnOyentesCancion = c->cabezaOyentes;
    c->cabezaOyentes = nuevaArista;
    
    c->totalReproducciones++;
    totalInteracciones++;
}

// Helper para borrar: Quita una arista de la lista horizontal de un usuario
void GestorGrafo::desconectarAristaDeUsuario(Usuario* u, Arista* aBorrar) {
    if (!u || !aBorrar) return;

    if (u->cabezaHistorial == aBorrar) {
        u->cabezaHistorial = aBorrar->sigEnHistorialUsuario;
        return;
    }

    Arista* prev = u->cabezaHistorial;
    while (prev != nullptr && prev->sigEnHistorialUsuario != aBorrar) {
        prev = prev->sigEnHistorialUsuario;
    }

    if (prev != nullptr) {
        prev->sigEnHistorialUsuario = aBorrar->sigEnHistorialUsuario;
    }
}

// ==========================================
// GESTIÓN DE DATOS (ABM & BÚSQUEDA)
// ==========================================

void GestorGrafo::agregarUsuario(string nombre, string pais) {
    int nuevoId = totalUsuarios + 1;
    Usuario* nuevo = new Usuario(nuevoId, nombre, pais);
    nuevo->siguiente = listaUsuarios;
    listaUsuarios = nuevo;
    totalUsuarios++;
}

void GestorGrafo::agregarCancion(string nombre, string artista, Genero genero) {
    int nuevoId = totalCanciones + 1;
    Cancion* nueva = new Cancion(nuevoId, nombre, artista, genero);
    nueva->siguiente = listaCanciones;
    listaCanciones = nueva;
    totalCanciones++;
}

bool GestorGrafo::modificarCancion(int id, string nuevoNombre, string nuevoArtista) {
    Cancion* c = buscarCancionPorId(id);
    if(c == nullptr) return false;
    c->nombre = nuevoNombre;
    c->artista = nuevoArtista;
    return true;
}

bool GestorGrafo::eliminarCancion(int id) {
    // 1. Buscar la canción y su anterior (para re-enlazar la lista maestra)
    Cancion* actual = listaCanciones;
    Cancion* prev = nullptr;
    
    while(actual != nullptr && actual->id != id) {
        prev = actual;
        actual = actual->siguiente;
    }

    if(actual == nullptr) return false; // No encontrada

    // 2. ELIMINAR ARISTAS (Crucial: Desconectar de los usuarios primero)
    Arista* aristaVertical = actual->cabezaOyentes;
    while(aristaVertical != nullptr) {
        Arista* aBorrar = aristaVertical;
        aristaVertical = aristaVertical->sigEnOyentesCancion;
        
        // Vamos al usuario y le quitamos esta arista de su historial
        desconectarAristaDeUsuario(aBorrar->usuario, aBorrar);
        
        delete aBorrar; // Liberar memoria de la arista
        totalInteracciones--;
    }

    // 3. Sacar la canción de la lista maestra
    if(prev == nullptr) {
        listaCanciones = actual->siguiente; // Era la primera
    } else {
        prev->siguiente = actual->siguiente;
    }

    delete actual; // Liberar memoria de la canción
    totalCanciones--;
    return true;
}

vector<Cancion*> GestorGrafo::buscarCanciones(string termino) {
    vector<Cancion*> encontrados;
    // Convertir termino a minusculas para busqueda flexible
    string termLower = termino;
    transform(termLower.begin(), termLower.end(), termLower.begin(), ::tolower);

    Cancion* actual = listaCanciones;
    while(actual != nullptr) {
        string nombreLower = actual->nombre;
        transform(nombreLower.begin(), nombreLower.end(), nombreLower.begin(), ::tolower);
        
        // Si el nombre contiene el termino
        if(nombreLower.find(termLower) != string::npos) {
            encontrados.push_back(actual);
        }
        actual = actual->siguiente;
    }
    return encontrados;
}

string GestorGrafo::obtenerGrafoComoTexto() {
    stringstream ss;
    ss << "=== REPORTE DE CONEXIONES ===\n";
    
    Usuario* u = listaUsuarios;
    while(u != nullptr) {
        if(u->cabezaHistorial != nullptr) { // Solo mostrar si tiene actividad
            ss << "User: " << u->nombre << " (" << u->pais << ")\n";
            Arista* a = u->cabezaHistorial;
            while(a != nullptr) {
                ss << "   -> Escucho: " << a->cancion->nombre 
                   << " [" << a->calificacion << "*]\n";
                a = a->sigEnHistorialUsuario;
            }
            ss << "-----------------------------\n";
        }
        u = u->siguiente;
    }
    return ss.str();
}

// ==========================================
// INTERACCIÓN Y LÓGICA
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
    nueva->vecesEscuchada = 1; 

    float pesoBase = (calificacion * 20.0f);
    if (favorito) pesoBase += 15.0f;
    nueva->pesoFinal = pesoBase;
    
    conectarNodos(u, c, nueva);
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

    Arista* miHistorial = usuarioTarget->cabezaHistorial;
    while (miHistorial != nullptr) {
        Cancion* cancionQueMeGusta = miHistorial->cancion;
        Arista* otrosOyentes = cancionQueMeGusta->cabezaOyentes;
        while (otrosOyentes != nullptr) {
            Usuario* vecino = otrosOyentes->usuario;
            if (vecino->id != usuarioTarget->id) {
                Arista* historiaVecino = vecino->cabezaHistorial;
                while (historiaVecino != nullptr) {
                    Cancion* candidata = historiaVecino->cancion;
                    bool yaLaTengo = haEscuchado(usuarioTarget, candidata);
                    bool yaSugerida = false;
                    for(Cancion* s : recomendaciones) if(s == candidata) yaSugerida = true;

                    if (!yaLaTengo && !yaSugerida) {
                        recomendaciones.push_back(candidata);
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

float GestorGrafo::calcularCompatibilidad(int idUsuarioA, int idUsuarioB) {
    Usuario* userA = buscarUsuarioPorId(idUsuarioA);
    Usuario* userB = buscarUsuarioPorId(idUsuarioB);
    if (!userA || !userB) return 0.0f;

    float interseccion = 0;
    float union_conjuntos = 0;

    Arista* historialA = userA->cabezaHistorial;
    while (historialA != nullptr) {
        union_conjuntos++;
        if (haEscuchado(userB, historialA->cancion)) interseccion++;
        historialA = historialA->sigEnHistorialUsuario;
    }

    Arista* historialB = userB->cabezaHistorial;
    while (historialB != nullptr) {
        if (!haEscuchado(userA, historialB->cancion)) union_conjuntos++;
        historialB = historialB->sigEnHistorialUsuario;
    }

    if (union_conjuntos == 0) return 0.0f;
    return (interseccion / union_conjuntos) * 100.0f;
}

// ==========================================
// GENERADOR DE DATOS
// ==========================================

void GestorGrafo::generarDatosAleatorios(int cantidadAristas) {
    srand(time(0));
    cout << "[GENERADOR] Verificando poblacion minima..." << endl;

    string nombres[] = {"Carlos", "Ana", "Luis", "Sofia", "Jorge", "Laura", "Andres", "Valentina", "Nestor", "Camila"};
    string paises[] = {"Colombia", "Mexico", "Peru", "Argentina", "Chile"};
    // AUMENTADO A 50 PARA EVITAR BUCLE INFINITO
    while(totalUsuarios < 50) { 
        string n = nombres[rand() % 10] + to_string(rand()%100);
        agregarUsuario(n, paises[rand() % 5]);
    }

    string artistas[] = {"Shakira", "Juanes", "J Balvin", "Niche", "Diomedes", "Queen", "Nirvana"};
    string titulos[] = {"Amor", "Fiesta", "Dolor", "Noche", "Dia", "Sueno", "Vida"};
    // AUMENTADO A 50 PARA EVITAR BUCLE INFINITO
    while(totalCanciones < 50) { 
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
            if(i > 20000) break; 
        }
    }
    cout << "[GENERADOR] Finalizado. Total aristas: " << totalInteracciones << endl;
}

void GestorGrafo::cargarDatosDesdeCSV(string archivo) {}