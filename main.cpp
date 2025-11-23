#include <iostream>
#include <limits> // Para limpiar el buffer de entrada
#include <cstdlib> // Para system("cls")
#include "GestorGrafo.h"
#include "estructuras.h"

using namespace std;

// Función para limpiar la consola (Estética)
void limpiarPantalla() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

// Función para pausar y que el usuario lea
void pausar() {
    cout << "\nPresione Enter para continuar...";
    // Se usa cin.ignore() y cin.get() para manejar el buffer de entrada después de leer un numero
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

// Módulo para interpretar la compatibilidad (10% de la nota)
void interpretarCompatibilidad(float comp) {
    cout << "\n=== INTERPRETACION TECNICA DE RESULTADOS ===" << endl;
    cout << "El valor de compatibilidad se calcula utilizando el INDICE DE JACCARD." << endl;
    cout << "Este es un metodo de Teoria de Conjuntos aplicado a Grafos Bipartitos." << endl;
    cout << "Formula: (Interseccion de gustos / Union de gustos) x 100." << endl;
    
    if (comp > 80.0) {
        cout << "Interpretacion: Los usuarios tienen una alta afinidad musical (> 80%). Comparten la mayoria de sus gustos y el Grafo Bipartito tiene un lazo muy fuerte entre sus vecinos. Esto predice una alta tasa de exito en las recomendaciones." << endl;
    } else if (comp > 40.0) {
        cout << "Interpretacion: Los usuarios tienen una compatibilidad media (40% - 80%). Comparten un conjunto significativo de canciones, lo que sugiere una buena base para la recomendacion colaborativa, pero con espacio para items nuevos." << endl;
    } else {
        cout << "Interpretacion: La compatibilidad es baja (< 40%). O no han interactuado con suficientes items o sus gustos estan en conjuntos separados del Grafo. La recomendacion debe enfocarse en la popularidad general (Grado de Vertice) o en la Vibe de la cancion." << endl;
    }
    cout << "========================================" << endl;
}


void mostrarMenu() {
    cout << "\n========================================" << endl;
    cout << "       BIPARTUNES - CONSOLA DE MANDO    " << endl;
    cout << "========================================" << endl;
    cout << " --- GESTION BASICA (ABM) ---" << endl;
    cout << " [1] Agregar Usuario Nuevo" << endl;
    cout << " [2] Agregar Cancion Nueva" << endl;
    cout << " [10] Modificar Cancion (Requisito PDF)" << endl;
    cout << " [11] Eliminar Cancion (Requisito PDF)" << endl;
    
    cout << "\n --- INTERACCION ---" << endl;
    cout << " [3] Registrar Interaccion (Usuario escucha Cancion)" << endl;
    
    cout << "\n --- CONSULTAS Y REPORTES ---" << endl;
    cout << " [4] Ver Top Canciones (Mas populares)" << endl;
    cout << " [8] VISUALIZAR GRAFO COMPLETO (Reporte Texto)" << endl;
    cout << " [9] BUSCAR CANCION (Por nombre)" << endl;
    
    cout << "\n --- INTELIGENCIA ARTIFICIAL ---" << endl;
    cout << " [5] RECOMENDAR Musica para un Usuario" << endl;
    cout << " [6] Calcular Compatibilidad (Jaccard)" << endl;
    cout << " [12] Interpretar Ultima Compatibilidad (Métricas)" << endl;
    
    cout << "\n --- UTILIDADES ---" << endl;
    cout << " [7] Generar Datos Masivos (600 Aristas)" << endl;
    cout << " [0] Salir" << endl;
    cout << "========================================" << endl;
    cout << " Opcion: ";
}

int main() {
    GestorGrafo app;
    int opcion = -1;
    float ultimaCompatibilidad = 0.0f; // Variable para la Opción 12
    
    // Carga inicial (para que no de error al probar opciones 4, 5, 6 al inicio)
    app.agregarUsuario("Juan del Llano", "Colombia"); 
    app.agregarCancion("Ay Mi Llanura", "Arnulfo Briceno", LLANERA); 
    
    do {
        limpiarPantalla();
        mostrarMenu();
        if (!(cin >> opcion)) { // Limpia el buffer si el input falla
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            opcion = -1;
        }

        switch (opcion) {
            case 1: {
                string n, p;
                cout << "\n--- NUEVO USUARIO ---" << endl;
                cout << "Nombre: "; cin.ignore(); getline(cin, n);
                cout << "Pais: "; getline(cin, p);
                app.agregarUsuario(n, p);
                cout << "Usuario agregado con exito." << endl;
                pausar();
                break;
            }
            case 2: {
                string n, a;
                int g;
                cout << "\n--- NUEVA CANCION ---" << endl;
                cout << "Titulo: "; cin.ignore(); getline(cin, n);
                cout << "Artista: "; getline(cin, a);
                cout << "Generos: 0=Llanera, 1=Rock, 2=Pop, 3=Salsa: "; cin >> g;
                app.agregarCancion(n, a, (Genero)g);
                cout << "Cancion agregada." << endl;
                pausar();
                break;
            }
            case 3: {
                int idU, idC, cal;
                bool fav;
                string com;
                cout << "\n--- REGISTRAR INTERACCION ---" << endl;
                cout << "ID Usuario: "; cin >> idU;
                cout << "ID Cancion: "; cin >> idC;
                cout << "Calificacion (1-5): "; cin >> cal;
                cout << "Es favorita? (1=Si, 0=No): "; cin >> fav;
                cout << "Comentario (una palabra): "; cin >> com;
                
                app.registrarInteraccion(idU, idC, cal, fav, com);
                pausar();
                break;
            }
            case 4: {
                cout << "\n--- TOP CANCIONES ---" << endl;
                vector<Cancion*> top = app.obtenerTopCanciones();
                if(top.empty()) cout << "No hay canciones aun." << endl;
                else {
                    for(int i=0; i<top.size() && i<10; i++) {
                        cout << " #" << (i+1) << " " << top[i]->nombre 
                             << " - " << top[i]->artista 
                             << " (" << top[i]->totalReproducciones << " repros)" << endl;
                    }
                }
                pausar();
                break;
            }
            case 5: {
                int idTarget;
                cout << "\n--- SISTEMA DE RECOMENDACION ---" << endl;
                cout << "Ingrese ID del Usuario a recomendar: "; cin >> idTarget;
                
                // La funcion de recomendacion ahora solo retorna el vector sin imprimir logs
                vector<Cancion*> recs = app.recomendarParaUsuario(idTarget); 
                
                cout << "\n--- RESULTADOS ---" << endl;
                if(recs.empty()) cout << "No hay suficiente informacion para recomendar." << endl;
                
                for(Cancion* c : recs) {
                    cout << " [RECOMENDADA] " << c->nombre 
                         << " (" << generoToString(c->genero) << ")" << endl;
                }
                pausar();
                break;
            }
            case 6: {
                int id1, id2;
                cout << "\n--- COMPATIBILIDAD (JACCARD) ---" << endl;
                cout << "ID Usuario A: "; cin >> id1;
                cout << "ID Usuario B: "; cin >> id2;
                ultimaCompatibilidad = app.calcularCompatibilidad(id1, id2);
                cout << "La compatibilidad musical es: " << ultimaCompatibilidad << "%" << endl;
                pausar();
                break;
            }
            case 7: {
                cout << "\n--- GENERADOR MASIVO ---" << endl;
                cout << "Generando 600 interacciones..." << endl;
                app.generarDatosAleatorios(600);
                pausar();
                break;
            }
            case 8: {
                cout << "\n--- VISUALIZADOR DE GRAFO ---" << endl;
                string reporte = app.obtenerGrafoComoTexto();
                cout << reporte << endl;
                pausar();
                break;
            }
            case 9: {
                string busqueda;
                cout << "\n--- BUSCADOR ---" << endl;
                cout << "Ingrese nombre de cancion: "; 
                cin.ignore(); 
                getline(cin, busqueda);
                
                vector<Cancion*> encontrados = app.buscarCanciones(busqueda);
                
                if(encontrados.empty()) cout << "No se encontraron resultados." << endl;
                else {
                    cout << "Resultados encontrados: " << encontrados.size() << endl;
                    for(Cancion* c : encontrados) {
                        cout << " [ID " << c->id << "] " << c->nombre 
                             << " - " << c->artista << " (" << generoToString(c->genero) << ")" << endl;
                    }
                }
                pausar();
                break;
            }
            case 10: {
                int id;
                string n, a;
                cout << "\n--- MODIFICAR CANCION ---" << endl;
                cout << "ID de cancion a editar: "; cin >> id;
                cout << "Nuevo Titulo: "; cin.ignore(); getline(cin, n);
                cout << "Nuevo Artista: "; getline(cin, a);
                
                if(app.modificarCancion(id, n, a)) cout << "Modificacion exitosa." << endl;
                else cout << "Error: ID no encontrado." << endl;
                pausar();
                break;
            }
            case 11: {
                int id;
                cout << "\n--- ELIMINAR CANCION ---" << endl;
                cout << "ID de cancion a borrar: "; cin >> id;
                
                if(app.eliminarCancion(id)) cout << "Cancion eliminada y conexiones borradas." << endl;
                else cout << "Error: ID no encontrado." << endl;
                pausar();
                break;
            }
            case 12: {
                if (ultimaCompatibilidad == 0.0f) {
                    cout << "\nPrimero debe calcular la compatibilidad (Opcion 6) para poder interpretarla." << endl;
                } else {
                    interpretarCompatibilidad(ultimaCompatibilidad);
                }
                pausar();
                break;
            }
            case 0:
                cout << "Saliendo... ¡Gracias por usar BiparTunes!" << endl;
                break;
            default:
                cout << "Opcion invalida." << endl;
                pausar();
        }

    } while (opcion != 0);

    return 0;
}