#ifndef GESTORGRAFO_H
#define GESTORGRAFO_H

#include "estructuras.h"
#include <vector> // Solo para retornar resultados a la interfaz, NO para el grafo
#include <string>

class GestorGrafo {
private:
    // Cabezas de las listas maestras
    Usuario* listaUsuarios;
    Cancion* listaCanciones;
    
    // Contadores para estadisticas rapidas
    int totalUsuarios;
    int totalCanciones;
    int totalInteracciones; // Aristas

    // Métodos privados auxiliares (Encapsulamiento)
    Usuario* buscarUsuarioPorId(int id);
    Cancion* buscarCancionPorId(int id);
    
    // Aquí ocurre la magia de conectar los 4 punteros
    void conectarNodos(Usuario* u, Cancion* c, Arista* nuevaArista);

public:
    GestorGrafo(); // Constructor
    ~GestorGrafo(); // Destructor (VITAL: Aquí liberamos toda la memoria manual)

    // --- ABM (Alta, Baja, Modificación) ---
    void agregarUsuario(string nombre, string pais);
    void agregarCancion(string nombre, string artista, Genero genero);
    
    // --- EL CORAZÓN DEL PROYECTO ---
    // Registra que alguien escuchó algo
    void registrarInteraccion(int idUsuario, int idCancion, int calificacion, 
                              bool favorito, string comentario);

    // --- ALGORITMOS DE RECOMENDACIÓN (Lo que pide el PDF) ---
    
    // 1. Top 10 canciones mas escuchadas (Usa el grado de vertice)
    // Retorna un vector simple solo para mostrar en pantalla
    vector<Cancion*> obtenerTopCanciones(); 
    
    // 2. Recomendación: "Porque escuchaste X, te recomendamos Y"
    // (Basado en vecinos cercanos)
    vector<Cancion*> recomendarParaUsuario(int idUsuario);

    // 3. FACTOR SORPRESA: Compatibilidad entre usuarios
    float calcularCompatibilidad(int idUsuarioA, int idUsuarioB);

    // --- UTILIDADES ---
    void cargarDatosDesdeCSV(string archivo); // Para los datos reales
    void generarDatosAleatorios(int cantidad); // Para llenar el resto
};

#endif // GESTORGRAFO_H