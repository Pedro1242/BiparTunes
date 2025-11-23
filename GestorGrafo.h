#ifndef GESTORGRAFO_H
#define GESTORGRAFO_H

#include "estructuras.h"
#include <vector> 
#include <string>

using namespace std;

class GestorGrafo {
private:
    // --- LISTAS MAESTRAS (Punteros a las cabezas) ---
    Usuario* listaUsuarios;
    Cancion* listaCanciones;
    
    // --- ESTADÍSTICAS RÁPIDAS ---
    int totalUsuarios;
    int totalCanciones;
    int totalInteracciones; // Aristas totales

    // --- MÉTODOS PRIVADOS (Encapsulamiento) ---
    Usuario* buscarUsuarioPorId(int id);
    Cancion* buscarCancionPorId(int id);
    
    // La función quirúrgica que conecta los punteros dobles
    void conectarNodos(Usuario* u, Cancion* c, Arista* nuevaArista);
    
    // Verifica si ya existe conexión para evitar duplicados
    bool haEscuchado(Usuario* u, Cancion* c);

public:
    GestorGrafo();  // Constructor
    ~GestorGrafo(); // Destructor (Limpieza de memoria manual)

    // --- ABM (Alta, Baja, Modificación) ---
    void agregarUsuario(string nombre, string pais);
    void agregarCancion(string nombre, string artista, Genero genero);
    
    // --- CREAR RELACIÓN (ARISTA) ---
    void registrarInteraccion(int idUsuario, int idCancion, int calificacion, 
                              bool favorito, string comentario);

    // --- ALGORITMOS (Requisitos del PDF) ---
    vector<Cancion*> obtenerTopCanciones(); 
    vector<Cancion*> recomendarParaUsuario(int idUsuario);

    // --- FACTOR SORPRESA ---
    float calcularCompatibilidad(int idUsuarioA, int idUsuarioB);

    // --- CARGA MASIVA ---
    void generarDatosAleatorios(int cantidad); 
    void cargarDatosDesdeCSV(string archivo); 
};

#endif // GESTORGRAFO_H