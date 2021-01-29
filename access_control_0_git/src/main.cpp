//librerias iniciales
#include <Arduino.h>
#include <PubSubClient.h> //(1)
#include <WiFi.h>         //(2)

//variables globales para conexion de internet
//negocio
const char* ssid      ="INFINITUM1C05_2.4";
const char* password  ="CuHpKyHT9n";

//casa
//const char* ssid      ="";
//const char* password  ="";

//celular
//const char* ssid      ="";
//const char* password  ="";

//*****************************
//***   CONFIGURACION MQTT  ***
//*****************************

const char* mqtt_server = "airpollutionqro.ga";  //dominio
const int   mqtt_port   = 1883;  //puerto TSP dentro del broker
const char* mqtt_user   = "web_client";  //por definir
const char* mqtt_pass   = "Franc1900";  //password (MYSQL)

WiFiClient espClient; //iniciamos un cliente wifi en SP32, (2)
PubSubClient client(espClient); //definimos por donde probienen los datos atraves del "espClient", (1)

long lastMsg = 0;
char msg[25];

//variables para mandar los datos
int temp1 =0;
int temp2 =1;
int volts =2;

//*****************************
//*** DECLARACION FUNCIONES ***
//*****************************
void setup_wifi();
void callback(char* topic, byte* payload, unsigned int length);
void reconnect();

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);   // inicializamos el pin BUILTIN_LED, como salida (pin 22)
  Serial.begin(115200);   // inicializamos la comunicacion serial
  randomSeed(micros());   //genera datos aleatorios
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port); //parametros para la conexion al servidor
  client.setCallback(callback); //se crea un "listener" el cual revisa si existen mesajes  o hay mensajes en el sistema
}

void loop(){
  //regresa un true, si no estamos conectados "!", activa un metodo
  //de reconexion para volver a conectar
  if (!client.connected()) {
    reconnect();
  }
  client.loop(); //metodo vigila que cuida la conexion

  unsigned long now = millis(); //segundos desde que inicio la SP32

  if (now - lastMsg > 500) {
    lastMsg = now; //reseteamos "lastMsg" para resetear la cuenta
    //sumamos un uno en cada vuelta
    temp1++;
    temp1++;
    volts++;

    String to_send = String(temp1) + "," + String(temp2) + ","+  String(volts); //"dato1,dato2,dato3" forma de impresion

    //Copia los caracteres de la cadena en el búfer proporcionado, "msg[25];"
    //myString: una variable de tipo String.
    //buf: el búfer en el que copiar los caracteres. Tipos de datos permitidos: matriz de char.
    //len: el tamaño del búfer. Tipos de datos permitidos: unsigned int.
    to_send.toCharArray(msg, 25); // myString.toCharArray(buf, len)

    Serial.print("Publicamos mensaje: ");
    Serial.println(msg);
    client.publish("values", msg);
  }
}

//*****************************
//***    CONEXION WIFI      ***
//*****************************

void setup_wifi() {

  delay(10);
  // Nos conectamosa nuestra red wifi
  Serial.println();
  Serial.print("Conectado a ");
  Serial.println(ssid);

  //WiFi.mode(WIFI_STA);

  //segenara la conexion a wifi
  WiFi.begin(ssid, password);

  //se genera un tiempo de espera para la conexion
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //randomSeed(micros());

  //Se imprime la canexion y se muestra la ip ("Conexion exitosa")
  Serial.println("");
  Serial.println("Conectado a red WIFI !!!!!");
  Serial.println("Direccion IP: ");
  Serial.println(WiFi.localIP());
}

//*****************************
//****  LISTENER CALLBACK  ****
//*****************************

void callback(char* topic, byte* payload, unsigned int length) {
  String incoming = ""; //almacena mensaje procesado,configuraciones necesarias
  Serial.print("Mesanje recibido desde [");
  Serial.print(topic);
  Serial.print("] ");
  //ciclo for para concatenar los datos obtenidos en el mensaje "payload", hasta lo que vale
  //la variable "length"
  for (int i = 0; i < length; i++) {
    incoming += ((char)payload[i]);
  }

  incoming.trim();//metodo para limpiar el mesaje,como quitar espacios etc.

  Serial.println("Mensaje -> " + incoming);//imprimimos la variable "incoming"

  // Si se recibe un "on" prende el led, intruccion de "incoming"
  if (incoming == "on") {
    digitalWrite(BUILTIN_LED, HIGH);   // Prende el led
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // se apaga el led
  }
}

//*****************************
//*******  RECONEXION   *******
//*****************************

void reconnect() {
  // bucle hasta que nos volvamos a conectar
  while (!client.connected()) {
    Serial.print("Intentando conexion MQTT...!!");
    // Creamos un cliente ID
    String clientId = "esp32_";
    clientId += String(random(0xffff), HEX); //genera un valor aleatorio,string aleatorio
    // Intentamos conectar,
    if (client.connect(clientId.c_str(),mqtt_user,mqtt_pass)) { //"c_str()" metodo que convierte ..
      Serial.println("Conectado!!");                            //..una variable en un "char array"
      // Una vez conectado, publique un anuncio....
      //client.publish("outTopic", "hello world");

      // ... and nos suscribimos a un topic
      client.subscribe("led1");
      client.subscribe("led1");
    } else {
      Serial.print("fallo conexion, con error -> ");
      Serial.print(client.state()); //se imprime la razon del error
      Serial.println("Intentamos de nuevo en 5 segundos");
      // Esperamos 5 segundos antes de volver a intentarlo
      delay(5000);
    }
  }
}
