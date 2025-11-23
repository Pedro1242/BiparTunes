#ifndef ESTRUCTURAS_H
#define ESTRUCTURAS_H

#include <string>
#include <iostream>

using namespace std;

// --- 1. ENUMS Y ESTRUCTURAS AUXILIARES ---

// Géneros musicales para clasificar
enum Genero {
    LLANERA,
    ROCK,
    POP,
    SALSA,
    URBANO,
    ELECTRONICA,
    INDIE
};

// Estructura para la innovación (Vibe)
struct Vibe {
    float energia;       
    float felicidad;     
    float bailable;      
};

// Forward declarations
struct Usuario;
struct Cancion;

// --- 2. LA ARISTA (El corazón del Grafo Bipartito) ---

struct Arista {
    // Conexiones
    Usuario* usuario;   
    Cancion* cancion;   

    // Datos de la Interacción
    int calificacion;   
    string comentario;  
    bool esFavorito;
    
    // [NUEVO - ESTO FALTABA] Campo vital para que compile GestorGrafo.cpp
    int vecesEscuchada; 
    
    // Peso calculado
    Vibe vibeMatch;     
    float pesoFinal;    

    // --- MULTILISTA ---
    Arista* sigEnHistorialUsuario; 
    Arista* sigEnOyentesCancion;   

    // Constructor
    Arista() : usuario(nullptr), cancion(nullptr), 
               sigEnHistorialUsuario(nullptr), sigEnOyentesCancion(nullptr),
               pesoFinal(0.0), vecesEscuchada(0) {}
};

// --- 3. LOS NODOS ---

struct Cancion {
    int id;                 
    string nombre;
    string artista;
    Genero genero;
    
    Vibe vibePromedio;      

    Arista* cabezaOyentes;  
    
    // [NUEVO - ESTO FALTABA] Campo para el ranking de popularidad
    int totalReproducciones; 

    Cancion* siguiente; 
    
    Cancion(int _id, string _n, string _a, Genero _g) 
        : id(_id), nombre(_n), artista(_a), genero(_g), 
          cabezaOyentes(nullptr), totalReproducciones(0), siguiente(nullptr) {}
};

struct Usuario {
    int id;
    string nombre;
    string pais; 
    
    Arista* cabezaHistorial; 

    Usuario* siguiente; 

    Usuario(int _id, string _n, string _p) 
        : id(_id), nombre(_n), pais(_p), 
          cabezaHistorial(nullptr), siguiente(nullptr) {}
};

// --- UTILIDAD VISUAL ---
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