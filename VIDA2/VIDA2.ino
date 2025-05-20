#include <Servo.h>
// #include <DHT.h>
#include <SoftwareSerial.h>
SoftwareSerial BT(5,6); // RX, TX
// Luces internas
int led1 = 8;
int led2 = 9;
int led3 = 10;
int led4 = 11;
// Servos
Servo puertaServoGaraje;
int servoPuertaGaraje = 3;
Servo puertaServo;
int servoPuerta = 7;
// Servo garajeServo;
// int servoGaraje = A0;
// Modo seguridad
bool modoSeguridad = false;
int IRs = 12;
// Sensores
// Sensor ultrasónico
int trig = 2;
int echo = 4;
#define DISTANCIA_DETECCION_AUTO_GARAJE 8

// Buzzer pasivo
int buzPasivo = 13;

// // Humedad
// #define DHTPIN 2
// #define DHTTYPE DHT11
// DHT dht(DHTPIN, DHTTYPE);

void setup() {
  // Conexión
  Serial.begin(9600);
  BT.begin(38400);

  // Inicialización
  for(int i=8; i <= 11; i++){
    pinMode(i, OUTPUT); digitalWrite(i, LOW);
  }
  // Sensores
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);

  // Servos
  puertaServoGaraje.attach(servoPuertaGaraje);
  puertaServoGaraje.write(0);

  puertaServo.attach(servoPuerta);
  puertaServo.write(90);

}
// Funciones

void senalLuces(String msj);
void senalContra(String msj);
void senalSeguridad(String msj);
void senalEstado();

void loop() {

  if(BT.available() || Serial.available()){
    String msj;
    if(not BT.available()){
      msj = Serial.readStringUntil('\n');
    }else{
      msj = BT.readStringUntil('\n');
    }
    msj.trim();

    Serial.print("Mensaje recibido: "); Serial.println(msj);
    if (msj.indexOf("/") != -1) {
      if (msj.startsWith("Seguridad/")) {
        senalSeguridad(msj);
      } else {
        senalLuces(msj);
      }
    } else if (msj.indexOf(":") != -1) {
      senalContra(msj);
    } else if (msj == "Estado") {
      senalEstado();
    } else {
      BT.println("Formato no reconocido");
    }
    // senalSeguridad();
    // senalEstado();
  }
  // Sensores
  digitalWrite(trig, LOW);
  delayMicroseconds(10);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  // Abrir garaje
  unsigned long int pulso = pulseIn(echo, HIGH);
  float distancia = (pulso * 0.034) / 2.0; // Conversión a cm
  Serial.println(distancia);
  if(distancia > 0 && distancia <= DISTANCIA_DETECCION_AUTO_GARAJE){
    puertaServoGaraje.write(90);
    delay(3000);
    puertaServoGaraje.write(0);
  }
  // Modo seguridad
  if (modoSeguridad) {
    int estadoIR = digitalRead(IRs);
    if (estadoIR == LOW) {
      BT.println("Movimiento detectado");
      tone(buzPasivo, 2000);
      delay(200);
      noTone(buzPasivo);
      delay(200);
    } else {
      noTone(buzPasivo);
    }
  }

  delay(100);

}

void senalContra(String msj) {
  String contrasena = "1234";
  int posDosPuntos = msj.indexOf(':');
  String valor = msj.substring(posDosPuntos + 1);
  valor.trim();

  if (valor == contrasena) {
    BT.println("Bienvenido.");
    puertaServo.write(0);
    tone(buzPasivo, 1000);
    delay(2000);
    noTone(buzPasivo);
    puertaServo.write(90);
    // delay(2000);
    // puertaServo.write(0);
  } else {
    tone(buzPasivo, 3000);
    BT.println("Contraseña errónea.");
    delay(1000);
    noTone(buzPasivo);
  }
}


void senalLuces(String msj) {
  int posSlash = msj.indexOf('/');
  String dato = msj.substring(0, posSlash);
  String valor = msj.substring(posSlash + 1);
  dato.trim();
  valor.trim();

  if (dato == "Cuarto1") {
    digitalWrite(led1, (valor == "on" ? HIGH : LOW));
    BT.println("LED1");
  } else if (dato == "Cuarto2") {
    digitalWrite(led2, (valor == "on" ? HIGH : LOW));
    BT.println("LED2");
  } else if (dato == "Cuarto3") {
    digitalWrite(led3, (valor == "on" ? HIGH : LOW));
    BT.println("LED3");
  } else if (dato == "Cuarto4") {
    digitalWrite(led4, (valor == "on" ? HIGH : LOW));
    BT.println("LED4");
  } else {
    BT.println("Cuarto no válido");
  }
}
void senalSeguridad(String msj) {
  int posSlash = msj.indexOf('/');
  String comando = msj.substring(posSlash + 1);
  comando.trim();

  if (comando == "on") {
    modoSeguridad = true;
    BT.println("Seguridad activada.");
  } else if (comando == "off") {
    modoSeguridad = false;
    BT.println("Seguridad desactivada.");
    noTone(buzPasivo); // Apaga el buzzer por si estaba activo
  } else {
    BT.println("Comando de seguridad inválido.");
  }
}

void senalEstado() {

  // int temperatura = dht.readHumidity();
  // int humedad = dht.readTemperature();

  BT.println("--- ESTADO DE LA CASA ---");

  // Estado de luces
  BT.print("Cuarto1: "); BT.println(digitalRead(led1) == HIGH ? "Encendido" : "Apagado");
  BT.print("Cuarto2: "); BT.println(digitalRead(led2) == HIGH ? "Encendido" : "Apagado");
  BT.print("Cuarto3: "); BT.println(digitalRead(led3) == HIGH ? "Encendido" : "Apagado");
  BT.print("Cuarto4: "); BT.println(digitalRead(led3) == HIGH ? "Encendido" : "Apagado");

  // Estado del modo de seguridad
  BT.print("Modo Seguridad: "); BT.println(modoSeguridad ? "Activado" : "Desactivado");

  // Humedad
  // BT.print("Humedad: "); BT.print(humedad); BT.println(" %");
  // BT.print("Temperatura: "); BT.print(temperatura); BT.println(" °C");
  // Distancia del sensor ultrasónico
  digitalWrite(trig, LOW);
  delayMicroseconds(10);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  unsigned long int pulso = pulseIn(echo, HIGH);
  float distancia = (pulso * 0.034) / 2.0;

  BT.print("Distancia (garaje): ");
  if (distancia == 0 || distancia >= 400) {
    BT.println("No detectada");
  } else {
    BT.print(distancia);
    BT.println(" cm");
  }

  BT.println("-------------------------");
}















