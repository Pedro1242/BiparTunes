#ifndef ESTRUCTURAS_H
#define ESTRUCTURAS_H

#include <string>
#include <iostream>

using namespace std;

// --- 1. ENUMS Y ESTRUCTURAS AUXILIARES ---

// Géneros musicales para clasificar (vital para el filtrado)
enum Genero {
    LLANERA,
    ROCK,
    POP,
    SALSA,
    URBANO,
    ELECTRONICA,
    INDIE
};

// INNOVACIÓN: En lugar de un solo peso int, usamos un "Vector de Vibe"
// Esto nos permite calcular similitud no solo por rating, sino por "sentimiento".
struct Vibe {
    float energia;       // 0.0 (Chill) a 1.0 (Intenso)
    float felicidad;     // 0.0 (Triste) a 1.0 (Feliz)
    float bailable;      // 0.0 (Para escuchar) a 1.0 (Para bailar)
};

// Forward declarations (necesario porque Arista usa Punteros a Usuario y Cancion)
struct Usuario;
struct Cancion;

// --- 2. LA ARISTA (El corazón del Grafo Bipartito) ---

struct Arista {
    // Conexiones del Grafo Bipartito [cite: 20]
    Usuario* usuario;   // El nodo de origen (Set U)
    Cancion* cancion;   // El nodo de destino (Set V)

    // Datos de la Interacción (Pesos y Metadatos) 
    int calificacion;   // 1 a 5 estrellas
    string comentario;  // Reseña textual
    bool esFavorito;
    int vecesEscuchada; 
    
    // El peso calculado para el algoritmo (nuestra "salsa secreta")
    Vibe vibeMatch;     
    float pesoFinal;    // Resultado de la fórmula de relevancia

    // --- MAGIA DE PUNTEROS (MULTILISTA) ---
    // Esta arista vive en dos listas enlazadas al mismo tiempo:
    
    // 1. Siguiente canción en el historial de ESTE usuario
    Arista* sigEnHistorialUsuario; 
    
    // 2. Siguiente usuario que escuchó ESTA canción
    Arista* sigEnOyentesCancion;   

    // Constructor para inicializar punteros en null (Buena práctica)
    Arista() : usuario(nullptr), cancion(nullptr), 
               sigEnHistorialUsuario(nullptr), sigEnOyentesCancion(nullptr),
               pesoFinal(0.0) {}
};

// --- 3. LOS NODOS (Vértices) ---

struct Cancion {
    int id;                 // Identificador único
    string nombre;
    string artista;
    Genero genero;
    
    // Perfil base de la canción (su ADN musical)
    Vibe vibePromedio;      

    // Cabeza de la lista enlazada de quienes la han escuchado
    //  (Grado del vértice = tamaño de esta lista)
    Arista* cabezaOyentes;  
    int totalReproducciones; // Para métricas de popularidad [cite: 11]

    Cancion* siguiente; // Para la lista general de canciones del sistema
    
    Cancion(int _id, string _n, string _a, Genero _g) 
        : id(_id), nombre(_n), artista(_a), genero(_g), 
          cabezaOyentes(nullptr), totalReproducciones(0), siguiente(nullptr) {}
};

struct Usuario {
    int id;
    string nombre;
    string pais; // Ej: "Colombia - Meta"
    
    // Cabeza de la lista enlazada de su historial
    Arista* cabezaHistorial; 

    Usuario* siguiente; // Para la lista general de usuarios del sistema

    Usuario(int _id, string _n, string _p) 
        : id(_id), nombre(_n), pais(_p), 
          cabezaHistorial(nullptr), siguiente(nullptr) {}
};

inline string generoToString(Genero g) {
    switch(g) {
        case LLANERA: return "Llanera";
        case ROCK: return "Rock";
        case POP: return "Pop";
        case SALSA: return "Salsa";
        case URBANO: return "Urbano";
        case ELECTRONICA: return "Electronica";
        case INDIE: return "Indie";
        default: return "Desconocido";
    }
}

#endif // ESTRUCTURAS_H