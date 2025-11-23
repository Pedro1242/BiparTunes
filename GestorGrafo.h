#ifndef GESTORGRAFO_H
#define GESTORGRAFO_H

#include "estructuras.h"
#include <vector> // Solo para retornar resultados a la interfaz, NO para la estructura del grafo
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
    int totalInteracciones; // Cantidad total de aristas en el sistema

    // --- MÉTODOS AUXILIARES (Encapsulamiento) ---
    // Buscadores internos para no exponer punteros en el main
    Usuario* buscarUsuarioPorId(int id);
    Cancion* buscarCancionPorId(int id);
    
    // Función quirúrgica: Conecta los 4 punteros de la Arista (Multilista)
    void conectarNodos(Usuario* u, Cancion* c, Arista* nuevaArista);

    // [NUEVO] Verifica si existe una arista entre Usuario y Canción
    // Vital para no recomendarle a alguien una canción que ya escuchó
    bool haEscuchado(Usuario* u, Cancion* c);

public:
    GestorGrafo();  // Constructor: Inicia todo en null/0
    ~GestorGrafo(); // Destructor: Limpieza manual de memoria (VITAL para la nota)

    // --- ABM (Alta, Baja, Modificación) ---
    // Inserción en cabeza (O(1)) para máxima eficiencia
    void agregarUsuario(string nombre, string pais);
    void agregarCancion(string nombre, string artista, Genero genero);
    
    // --- EL CORAZÓN DEL PROYECTO ---
    // Crea la conexión (Arista) con peso calculado
    void registrarInteraccion(int idUsuario, int idCancion, int calificacion, 
                              bool favorito, string comentario);

    // --- ALGORITMOS DE RECOMENDACIÓN (Requisitos del PDF) ---
    
    // 1. Top Popularidad: Ordena canciones según su Grado de Vértice
    vector<Cancion*> obtenerTopCanciones(); 
    
    // 2. Filtrado Colaborativo: "Tus vecinos escucharon esto..."
    // Usa la estructura de grafo para navegar entre usuarios y canciones
    vector<Cancion*> recomendarParaUsuario(int idUsuario);

    // 3. FACTOR SORPRESA: Compatibilidad (Jaccard / Intersección)
    float calcularCompatibilidad(int idUsuarioA, int idUsuarioB);

    // --- UTILIDADES DE CARGA DE DATOS ---
    void cargarDatosDesdeCSV(string archivo); // Para simular datos reales
    void generarDatosAleatorios(int cantidad); // Para rellenar hasta las 600 aristas
};

#endif // GESTORGRAFO_H