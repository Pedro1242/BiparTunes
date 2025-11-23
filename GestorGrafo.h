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

    // IDs monotónicos (no decrementan al borrar)
    int nextUsuarioId;
    int nextCancionId;

    // --- MÉTODOS PRIVADOS ---
    Usuario* buscarUsuarioPorId(int id);
    Cancion* buscarCancionPorId(int id);
    
    void conectarNodos(Usuario* u, Cancion* c, Arista* nuevaArista);
    bool haEscuchado(Usuario* u, Cancion* c);

    // buscar arista existente en el historial de un usuario
    Arista* buscarAristaEnUsuario(Usuario* u, Cancion* c);

    // Borrado y desconexión simétrica
    void desconectarAristaDeUsuario(Usuario* u, Arista* aBorrar);
    void desconectarAristaDeCancion(Cancion* c, Arista* aBorrar);
    void borrarArista(Arista* aBorrar); // quita de ambas listas y delete, ajusta contadores

public:
    GestorGrafo();  
    ~GestorGrafo(); 

    // --- ABM BÁSICO ---
    void agregarUsuario(string nombre, string pais);
    void agregarCancion(string nombre, string artista, Genero genero);
    
    // --- EDIT / DELETE ---
    bool modificarCancion(int id, string nuevoNombre, string nuevoArtista);
    bool eliminarCancion(int id); // borra punteros cruzados

    // --- INTERACCIÓN ---
    void registrarInteraccion(int idUsuario, int idCancion, int calificacion, 
                              bool favorito, string comentario);

    // --- CONSULTAS (FRONTEND READY) ---
    vector<Cancion*> buscarCanciones(string terminoBusqueda);
    string obtenerGrafoComoTexto();

    vector<Cancion*> obtenerTopCanciones(); 
    vector<Cancion*> recomendarParaUsuario(int idUsuario);
    float calcularCompatibilidad(int idUsuarioA, int idUsuarioB);

    // --- CARGA ---
    void generarDatosAleatorios(int cantidad); 
    void cargarDatosDesdeCSV(string archivo); 
};

#endif // GESTORGRAFO_H
