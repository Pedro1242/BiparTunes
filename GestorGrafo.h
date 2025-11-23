#ifndef GESTORGRAFO_H
#define GESTORGRAFO_H

#include "estructuras.h"
#include <vector> 
#include <string>

using namespace std;

class GestorGrafo {
private:
    // --- LISTAS MAESTRAS ---
    Usuario* listaUsuarios;
    Cancion* listaCanciones;
    
    // --- ESTADÍSTICAS ---
    int totalUsuarios;
    int totalCanciones;
    int totalInteracciones; 

    // --- MÉTODOS PRIVADOS ---
    Usuario* buscarUsuarioPorId(int id);
    Cancion* buscarCancionPorId(int id);
    
    void conectarNodos(Usuario* u, Cancion* c, Arista* nuevaArista);
    bool haEscuchado(Usuario* u, Cancion* c);

    // [NUEVO] Ayuda a borrar limpiamente una arista de la lista de un usuario
    void desconectarAristaDeUsuario(Usuario* u, Arista* aBorrar);

public:
    GestorGrafo();  
    ~GestorGrafo(); 

    // --- ABM BÁSICO ---
    void agregarUsuario(string nombre, string pais);
    void agregarCancion(string nombre, string artista, Genero genero);
    
    // [NUEVO] Funciones para Editar/Eliminar (Requisito 1 PDF)
    bool modificarCancion(int id, string nuevoNombre, string nuevoArtista);
    bool eliminarCancion(int id); // La más difícil (borra punteros cruzados)

    // --- INTERACCIÓN ---
    void registrarInteraccion(int idUsuario, int idCancion, int calificacion, 
                              bool favorito, string comentario);

    // --- CONSULTAS (FRONTEND READY) ---
    // Estas funciones retornan datos, NO imprimen (ideal para GUI)
    
    // [NUEVO] Buscador por texto (Requisito 6 PDF)
    vector<Cancion*> buscarCanciones(string terminoBusqueda);

    // [NUEVO] Devuelve el grafo entero en un string para mostrarlo en pantalla
    string obtenerGrafoComoTexto();

    // Algoritmos ya existentes
    vector<Cancion*> obtenerTopCanciones(); 
    vector<Cancion*> recomendarParaUsuario(int idUsuario);
    float calcularCompatibilidad(int idUsuarioA, int idUsuarioB);

    // --- CARGA ---
    void generarDatosAleatorios(int cantidad); 
    void cargarDatosDesdeCSV(string archivo); 
};

#endif // GESTORGRAFO_H