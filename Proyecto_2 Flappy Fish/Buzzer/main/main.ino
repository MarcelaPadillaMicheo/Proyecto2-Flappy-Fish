int speakerPin = 40; // Declarar el PIN digital D9 para el Piezo Speaker
int tiempo = 180;    // Tiempo de duración para el toque de notas

void setup() {
  pinMode(speakerPin, OUTPUT); // Configurar el puerto digital 9 como salida
}

int calcularFrecuencia(char nota, int octava) {
  char nombres[] = { 'C', 'D', 'd', 'E', 'F', 'G', 'g', 'A', 'B' };
  int tonos[] = { 1915, 1700, 1610, 1519, 1432, 1275, 1205, 1136, 1014 }; // Frecuencias de la octava 4 (C4 = 261 Hz)

  for (int i = 0; i < 9; i++) {
    if (nombres[i] == nota) {
      return tonos[i] * pow(2, octava - 4);
    }
  }
  return 0; // Devolver 0 si la nota no se encuentra
}

void tocarTono(int tono, int duracion) {
  for (long i = 0; i < duracion * 1000L; i += tono * 2) {
    digitalWrite(speakerPin, HIGH);
    delayMicroseconds(tono);
    digitalWrite(speakerPin, LOW);
    delayMicroseconds(tono);
  }
}

void decrementarOctava(int &octava) {
  if (octava > 0) {
    octava--;
  }
}

void tocarMelodia(String melodia, int duraciones[]) {
  for (int i = 0; i < melodia.length(); i++) {
    char nota = melodia.charAt(i);
    if (nota == ' ') {
      delay(duraciones[i] * tiempo);
    } else {
      int octava = 4; // Octava predeterminada
      if (melodia.charAt(i + 1) == '^') {
        // Si hay un '^', aumentar la octava en 1
        octava++;
        i++;
      }
      if (melodia.charAt(i - 1) == '-') {
        // Si hay un '-', decrementar la octava en 1
        decrementarOctava(octava);
        i++;
      }
      int frecuencia = calcularFrecuencia(nota, octava);
      if (frecuencia > 0) {
        tocarTono(frecuencia, duraciones[i] * tiempo);
      }
    }
    delay(tiempo / 2);
  }
}

void loop() {
 
  String melodia = "G,-C,G,A,B,E,E,A,G,F,G,C,C,D,D,E,F,F,G,A,B,-C,-D,G,-E,-D,-C,-D";
  int duraciones[] = {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2};
  
  tocarMelodia(melodia,duraciones);
}
