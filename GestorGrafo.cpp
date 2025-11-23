#include "GestorGrafo.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib> 
#include <ctime>   
#include <algorithm> // Para transformar string a minusculas
#include <unordered_set>
#include <unordered_map>
#include <limits>

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
    nextUsuarioId = 1;
    nextCancionId = 1;
}

GestorGrafo::~GestorGrafo() {
    cout << "[SISTEMA] Limpiando memoria..." << endl;
    // Borrar todas las aristas de forma segura recorriendo usuarios
    Usuario* userActual = listaUsuarios;
    while (userActual != nullptr) {
        Arista* aristaActual = userActual->cabezaHistorial;
        while (aristaActual != nullptr) {
            Arista* aristaBorrar = aristaActual;
            aristaActual = aristaActual->sigEnHistorialUsuario;
            // Al borrar desde usuario, también debemos asegurarnos de no
            // intentar borrarla de la lista de la canción dos veces.
            // Aquí, como vamos a borrar canciones después, simplemente delete arista.
            delete aristaBorrar;
        }
        Usuario* tmpUser = userActual;
        userActual = userActual->siguiente;
        delete tmpUser;
    }
    // Ahora borrar canciones
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
    if (!u || !c) return false;
    Arista* actual = u->cabezaHistorial;
    while (actual != nullptr) {
        if (actual->cancion == c) return true;
        actual = actual->sigEnHistorialUsuario;
    }
    return false;
}

Arista* GestorGrafo::buscarAristaEnUsuario(Usuario* u, Cancion* c) {
    if (!u || !c) return nullptr;
    Arista* a = u->cabezaHistorial;
    while (a) {
        if (a->cancion == c) return a;
        a = a->sigEnHistorialUsuario;
    }
    return nullptr;
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

// Quita una arista de la lista horizontal de un usuario (no borra)
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

// Quita una arista de la lista vertical de una canción (no borra)
void GestorGrafo::desconectarAristaDeCancion(Cancion* c, Arista* aBorrar) {
    if (!c || !aBorrar) return;

    if (c->cabezaOyentes == aBorrar) {
        c->cabezaOyentes = aBorrar->sigEnOyentesCancion;
        return;
    }

    Arista* prev = c->cabezaOyentes;
    while (prev != nullptr && prev->sigEnOyentesCancion != aBorrar) {
        prev = prev->sigEnOyentesCancion;
    }

    if (prev != nullptr) {
        prev->sigEnOyentesCancion = aBorrar->sigEnOyentesCancion;
    }
}

// Borra la arista quitándola de ambas listas y actualizando contadores
void GestorGrafo::borrarArista(Arista* aBorrar) {
    if (!aBorrar) return;
    Usuario* u = aBorrar->usuario;
    Cancion* c = aBorrar->cancion;
    if (u) desconectarAristaDeUsuario(u, aBorrar);
    if (c) {
        desconectarAristaDeCancion(c, aBorrar);
        if (c->totalReproducciones > 0) c->totalReproducciones--;
    }
    if (totalInteracciones > 0) totalInteracciones--;
    delete aBorrar;
}

// ==========================================
// GESTIÓN DE DATOS (ABM & BÚSQUEDA)
// ==========================================

void GestorGrafo::agregarUsuario(string nombre, string pais) {
    int nuevoId = nextUsuarioId++;
    Usuario* nuevo = new Usuario(nuevoId, nombre, pais);
    nuevo->siguiente = listaUsuarios;
    listaUsuarios = nuevo;
    totalUsuarios++;
}

void GestorGrafo::agregarCancion(string nombre, string artista, Genero genero) {
    int nuevoId = nextCancionId++;
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

    // 2. ELIMINAR ARISTAS (Desconectar desde la lista de oyentes de la canción)
    Arista* aristaVertical = actual->cabezaOyentes;
    while(aristaVertical != nullptr) {
        Arista* aBorrar = aristaVertical;
        aristaVertical = aristaVertical->sigEnOyentesCancion;
        
        // Vamos al usuario y le quitamos esta arista de su historial y la borramos
        borrarArista(aBorrar);
    }

    // 3. Sacar la canción de la lista maestra
    if(prev == nullptr) {
        listaCanciones = actual->siguiente; // Era la primera
    } else {
        prev->siguiente = actual->siguiente;
    }

    delete actual; // Liberar memoria de la canción
    if (totalCanciones > 0) totalCanciones--;
    return true;
}

string toLowerSafe(const string &s) {
    string out = s;
    for (size_t i = 0; i < out.size(); ++i) {
        out[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(out[i])));
    }
    return out;
}

vector<Cancion*> GestorGrafo::buscarCanciones(string termino) {
    vector<Cancion*> encontrados;
    string termLower = toLowerSafe(termino);

    Cancion* actual = listaCanciones;
    while(actual != nullptr) {
        string nombreLower = toLowerSafe(actual->nombre);
        string artistaLower = toLowerSafe(actual->artista);
        
        if(nombreLower.find(termLower) != string::npos || artistaLower.find(termLower) != string::npos) {
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

    // Evitar duplicados: si ya existe arista entre u y c, actualizarla
    Arista* existente = buscarAristaEnUsuario(u, c);
    if (existente) {
        existente->vecesEscuchada++;
        existente->calificacion = calificacion; // podrías promediar si prefieres
        existente->esFavorito = favorito;
        existente->comentario = comentario;
        float pesoBase = (calificacion * 20.0f);
        if (favorito) pesoBase += 15.0f;
        existente->pesoFinal = pesoBase;
        // actualizar contador de reproducciones porque no creamos nueva arista
        return;
    }

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

    // Usa std::sort para mejor rendimiento
    sort(resultado.begin(), resultado.end(), [](Cancion* a, Cancion* b){
        return a->totalReproducciones > b->totalReproducciones;
    });

    return resultado;
}

vector<Cancion*> GestorGrafo::recomendarParaUsuario(int idUsuario) {
    vector<Cancion*> recomendaciones;
    Usuario* usuarioTarget = buscarUsuarioPorId(idUsuario);
    if (usuarioTarget == nullptr) return recomendaciones;

    // 1) Recopilar canciones ya escuchadas por el usuario
    unordered_set<int> yaEscuchadas;
    Arista* aHist = usuarioTarget->cabezaHistorial;
    while (aHist) {
        yaEscuchadas.insert(aHist->cancion->id);
        aHist = aHist->sigEnHistorialUsuario;
    }

    // 2) Score de candidatos: map canción.id -> puntaje
    unordered_map<int, float> scoreMap;
    unordered_map<int, Cancion*> songById;

    aHist = usuarioTarget->cabezaHistorial;
    while (aHist) {
        Cancion* cancionBase = aHist->cancion;
        Arista* oyentes = cancionBase->cabezaOyentes;
        while (oyentes) {
            Usuario* vecino = oyentes->usuario;
            if (vecino && vecino->id != usuarioTarget->id) {
                // considerar solo vecinos con peso razonable (umbral opcional)
                Arista* hVec = vecino->cabezaHistorial;
                while (hVec) {
                    Cancion* candidata = hVec->cancion;
                    if (!candidata) { hVec = hVec->sigEnHistorialUsuario; continue; }
                    if (yaEscuchadas.count(candidata->id) == 0) {
                        // aumentar score por el peso de la arista del vecino
                        scoreMap[candidata->id] += oyentes->pesoFinal; // vecino->arista->pesoFinal
                        songById[candidata->id] = candidata;
                    }
                    hVec = hVec->sigEnHistorialUsuario;
                }
            }
            oyentes = oyentes->sigEnOyentesCancion;
        }
        aHist = aHist->sigEnHistorialUsuario;
    }

    // Transformar score map en vector ordenado
    vector<pair<int,float>> listaScores;
    for (auto &p : scoreMap) listaScores.push_back(p);
    sort(listaScores.begin(), listaScores.end(), [](const pair<int,float>& A, const pair<int,float>& B){
        return A.second > B.second;
    });

    for (auto &p : listaScores) {
        recomendaciones.push_back(songById[p.first]);
    }

    // Si no se generaron recomendaciones (nuevo usuario o aislado), fallback por genero
    if (recomendaciones.empty()) {
        // encontrar genero favorito del usuario (por pesoFinal en su historial)
        unordered_map<Genero, float> generoScore;
        aHist = usuarioTarget->cabezaHistorial;
        while (aHist) {
            generoScore[aHist->cancion->genero] += aHist->pesoFinal;
            aHist = aHist->sigEnHistorialUsuario;
        }
        Genero favorito = LLANERA;
        if (!generoScore.empty()) {
            float best = numeric_limits<float>::lowest();
            for (auto &g : generoScore) if (g.second > best) { best = g.second; favorito = g.first; }
            // sugerir top canciones del mismo genero
            vector<Cancion*> todas = obtenerTopCanciones();
            for (Cancion* c : todas) {
                if (c->genero == favorito && yaEscuchadas.count(c->id)==0) recomendaciones.push_back(c);
                if (recomendaciones.size() >= 10) break;
            }
        } else {
            // fallback final: top global
            vector<Cancion*> todas = obtenerTopCanciones();
            for (Cancion* c : todas) {
                if (yaEscuchadas.count(c->id)==0) recomendaciones.push_back(c);
                if (recomendaciones.size() >= 10) break;
            }
        }
    }

    return recomendaciones;
}

float GestorGrafo::calcularCompatibilidad(int idUsuarioA, int idUsuarioB) {
    Usuario* userA = buscarUsuarioPorId(idUsuarioA);
    Usuario* userB = buscarUsuarioPorId(idUsuarioB);
    if (!userA || !userB) return 0.0f;

    unordered_set<int> setA, setB;
    Arista* a = userA->cabezaHistorial;
    while (a) { setA.insert(a->cancion->id); a = a->sigEnHistorialUsuario; }
    a = userB->cabezaHistorial;
    while (a) { setB.insert(a->cancion->id); a = a->sigEnHistorialUsuario; }

    int inter = 0;
    for (int id : setA) if (setB.count(id)) inter++;
    int uni = (int)(setA.size() + setB.size() - inter);
    if (uni == 0) return 0.0f;
    return ( (float)inter / (float)uni ) * 100.0f;
}

// ==========================================
// GENERADOR DE DATOS
// ==========================================

void GestorGrafo::generarDatosAleatorios(int cantidadAristas) {
    srand((unsigned int)time(0));
    cout << "[GENERADOR] Verificando poblacion minima..." << endl;

    string nombres[] = {"Carlos", "Ana", "Luis", "Sofia", "Jorge", "Laura", "Andres", "Valentina", "Nestor", "Camila"};
    string paises[] = {"Colombia", "Mexico", "Peru", "Argentina", "Chile"};
    // Asegurar al menos 50 usuarios
    while(totalUsuarios < 50) { 
        string n = nombres[rand() % 10] + to_string(rand()%100);
        agregarUsuario(n, paises[rand() % 5]);
    }

    string artistas[] = {"Shakira", "Juanes", "J Balvin", "Niche", "Diomedes", "Queen", "Nirvana"};
    string titulos[] = {"Amor", "Fiesta", "Dolor", "Noche", "Dia", "Sueno", "Vida"};
    // Asegurar al menos 50 canciones
    while(totalCanciones < 50) { 
        string t = titulos[rand() % 7] + " " + to_string(rand() % 100);
        agregarCancion(t, artistas[rand() % 7], (Genero)(rand() % 7));
    }

    cout << "[GENERADOR] Insertando " << cantidadAristas << " interacciones aleatorias..." << endl;
    int insertadas = 0;
    int intentos = 0;
    int maxIntentos = max(100000, cantidadAristas * 10);

    while (insertadas < cantidadAristas && intentos < maxIntentos) {
        intentos++;
        int rUser = (rand() % totalUsuarios) + 1; 
        int rSong = (rand() % totalCanciones) + 1;
        
        Usuario* u = buscarUsuarioPorId(rUser);
        Cancion* c = buscarCancionPorId(rSong);
        
        if (u && c && !haEscuchado(u, c)) {
            registrarInteraccion(rUser, rSong, (rand()%5)+1, (rand()%2), "Generado Auto");
            insertadas++;
        }
    }

    cout << "[GENERADOR] Finalizado. Intentos: " << intentos << "  Insertadas: " << insertadas << "  Total aristas: " << totalInteracciones << endl;
}

void GestorGrafo::cargarDatosDesdeCSV(string archivo) {
    ifstream in(archivo);
    if (!in.is_open()) {
        cout << "[CSV] Error abriendo archivo: " << archivo << endl;
        return;
    }
    // Formato esperado: idUsuario, idCancion, calificacion, favorito(0/1), comentario
    string line;
    while (getline(in, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string token;
        int idU, idC, cal, fav;
        string comentario;
        if (!getline(ss, token, ',')) continue;
        idU = stoi(token);
        if (!getline(ss, token, ',')) continue;
        idC = stoi(token);
        if (!getline(ss, token, ',')) continue;
        cal = stoi(token);
        if (!getline(ss, token, ',')) continue;
        fav = stoi(token);
        if (!getline(ss, comentario, ',')) comentario = "";
        registrarInteraccion(idU, idC, cal, (fav!=0), comentario);
    }
    in.close();
}
