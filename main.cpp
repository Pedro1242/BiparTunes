#include <iostream>
#include "GestorGrafo.h"
#include "estructuras.h" 

using namespace std;

int main() {
    cout << "=== BIPARTUNES: PRUEBA DE EXONERACION ===" << endl;

    GestorGrafo app;

    // --- FASE 1: DATOS MANUALES (Para probar lógica fina) ---
    cout << "\n[PASO 1] Creando Escenario Controlado..." << endl;
    app.agregarUsuario("Juan del Llano", "Colombia");   // ID 1
    app.agregarUsuario("Maria Rockera", "Mexico");      // ID 2
    app.agregarUsuario("Pedro Salsero", "Puerto Rico"); // ID 3

    app.agregarCancion("Ay Mi Llanura", "Arnulfo Briceno", LLANERA); // ID 1
    app.agregarCancion("Bohemian Rhapsody", "Queen", ROCK);          // ID 2
    app.agregarCancion("El Cantante", "Hector Lavoe", SALSA);        // ID 3

    // Relaciones manuales
    app.registrarInteraccion(1, 1, 5, true, "A"); // Juan ama Llanera
    app.registrarInteraccion(1, 2, 4, false,"B"); // Juan escucha Queen
    app.registrarInteraccion(2, 2, 5, true, "C"); // Maria ama Queen
    app.registrarInteraccion(2, 1, 3, false,"D"); // Maria escucha Llanera
    app.registrarInteraccion(3, 3, 5, true, "E"); // Pedro solo Salsa

    // EL MATCH: Maria descubre Salsa -> Debería recomendársela a Juan
    cout << "\n[PASO 2] Evento Clave: Maria descubre Salsa..." << endl;
    app.registrarInteraccion(2, 3, 5, true, "F"); 

    // --- FASE 2: PRUEBA DE RECOMENDACIÓN ---
    cout << "\n[PASO 3] Buscando Recomendaciones para Juan..." << endl;
    vector<Cancion*> recs = app.recomendarParaUsuario(1);
    
    cout << "--- RESULTADO RECOMENDACION ---" << endl;
    if(recs.empty()){
        cout << "Sin recomendaciones." << endl;
    }
    for(Cancion* c : recs) {
        // Aquí usamos tu función nueva para ver el género bonito
        cout << " * RECOMENDADA: " << c->nombre << " (" << generoToString(c->genero) << ")" << endl;
    }

    // --- FASE 3: FACTOR SORPRESA (COMPATIBILIDAD) ---
    cout << "\n[PASO 4 - BONUS] Calculadora de Compatibilidad (Jaccard)" << endl;
    
    // Juan y Maria coinciden mucho
    float compJM = app.calcularCompatibilidad(1, 2);
    cout << "Compatibilidad Juan vs Maria: " << compJM << "% (Se espera > 50%)" << endl;
    
    // Juan y Pedro no coinciden nada
    float compJP = app.calcularCompatibilidad(1, 3);
    cout << "Compatibilidad Juan vs Pedro: " << compJP << "% (Se espera 0%)" << endl;

    // --- FASE 4: CARGA MASIVA (STRESS TEST) ---
    cout << "\n[PASO 5 - REQUISITO PDF] Generando 600 interacciones..." << endl;
    app.generarDatosAleatorios(600);
    
    cout << "\n[PASO 6] Top 5 Popularidad tras Carga Masiva:" << endl;
    vector<Cancion*> top = app.obtenerTopCanciones();
    for(int i=0; i<5 && i<top.size(); i++) {
        cout << " #" << (i+1) << " " << top[i]->nombre 
             << " (" << top[i]->totalReproducciones << " repros)" << endl;
    }

    cout << "\n=== PRUEBA COMPLETADA EXITOSAMENTE ===" << endl;
    return 0;
}