#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

#define DHTPIN 33
#define DHTTYPE 22
#define TRIG 18
#define ECHO 19

int ldr = 26; //PIN do ldr
int limite = 300; //Limite de até aonde a agua pode chega antes de transbordar

DHT dht(DHTPIN, DHTTYPE);

// Configurações - variáveis editáveis
const char* default_SSID = "Wokwi-GUEST"; // Nome da rede Wi-Fi
const char* default_PASSWORD = ""; // Senha da rede Wi-Fi
const char* default_BROKER_MQTT = "9.234.138.146"; // IP do Broker MQTT
const int default_BROKER_PORT = 1883; // Porta do Broker MQTT
const char* default_TOPICO_SUBSCRIBE = "/TEF/lamp007/cmd"; // Tópico MQTT de escuta
const char* default_TOPICO_PUBLISH_1 = "/TEF/lamp007/attrs"; // Tópico MQTT de envio de informações para Broker
const char* default_TOPICO_PUBLISH_2 = "/TEF/lamp007/attrs/l"; // Tópico de luminosidade
const char* default_TOPICO_PUBLISH_3 = "/TEF/lamp007/attrs/h"; // Tópico de umidade
const char* default_TOPICO_PUBLISH_4 = "/TEF/lamp007/attrs/t"; // Tópico de temperatura
const char* default_TOPICO_PUBLISH_5 = "/TEF/lamp007/attrs/d"; // Tópico de distância
const char* default_ID_MQTT = "fiware_001"; // ID MQTT
const int default_D4 = 2; // Pino do LED onboard
// Declaração da variável para o prefixo do tópico
const char* topicPrefix = "lamp007";

// Variáveis para configurações editáveis
char* SSID = const_cast<char*>(default_SSID);
char* PASSWORD = const_cast<char*>(default_PASSWORD);
char* BROKER_MQTT = const_cast<char*>(default_BROKER_MQTT);
int BROKER_PORT = default_BROKER_PORT;
char* TOPICO_SUBSCRIBE = const_cast<char*>(default_TOPICO_SUBSCRIBE);
char* TOPICO_PUBLISH_1 = const_cast<char*>(default_TOPICO_PUBLISH_1);
char* TOPICO_PUBLISH_2 = const_cast<char*>(default_TOPICO_PUBLISH_2);
char* TOPICO_PUBLISH_3 = const_cast<char*>(default_TOPICO_PUBLISH_3);
char* TOPICO_PUBLISH_4 = const_cast<char*>(default_TOPICO_PUBLISH_4);
char* TOPICO_PUBLISH_5 = const_cast<char*>(default_TOPICO_PUBLISH_5);
char* ID_MQTT = const_cast<char*>(default_ID_MQTT);
int D4 = default_D4;

WiFiClient espClient;
PubSubClient MQTT(espClient);
char EstadoSaida = '0';

void initSerial() {
    Serial.begin(115200);
}

void initWiFi() {
    delay(10);
    Serial.println("------Conexao WI-FI------");
    Serial.print("Conectando-se na rede: ");
    Serial.println(SSID);
    Serial.println("Aguarde");
    reconectWiFi();
}

void initMQTT() {
    MQTT.setServer(BROKER_MQTT, BROKER_PORT);
    MQTT.setCallback(mqtt_callback);
}

void setup() {
    InitOutput();
    initSerial();
    initWiFi();
    initMQTT();
    delay(5000);
    MQTT.publish(TOPICO_PUBLISH_1, "s|on");
    pinMode(TRIG, OUTPUT); //Lançador (Gatilho/trigger)
    pinMode(ECHO, INPUT);  //Recebedor (Echo)
    dht.begin(); //Iniciar o DHT
    pinMode(ldr, OUTPUT);
}

void loop() {
    VerificaConexoesWiFIEMQTT();
    EnviaEstadoOutputMQTT();
    TempHumi();
    Lumi();
    dist();
    Serial.println("");
    MQTT.loop();
}

void reconectWiFi() {
    if (WiFi.status() == WL_CONNECTED)
        return;
    WiFi.begin(SSID, PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println();
    Serial.println("Conectado com sucesso na rede ");
    Serial.print(SSID);
    Serial.println("IP obtido: ");
    Serial.println(WiFi.localIP());

    // Garantir que o LED inicie desligado
    digitalWrite(D4, LOW);
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
    String msg;
    for (int i = 0; i < length; i++) {
        char c = (char)payload[i];
        msg += c;
    }
    Serial.print("- Mensagem recebida: ");
    Serial.println(msg);

    // Forma o padrão de tópico para comparação
    String onTopic = String(topicPrefix) + "@on|";
    String offTopic = String(topicPrefix) + "@off|";

    // Compara com o tópico recebido
    if (msg.equals(onTopic)) {
        digitalWrite(D4, HIGH);
        EstadoSaida = '1';
    }

    if (msg.equals(offTopic)) {
        digitalWrite(D4, LOW);
        EstadoSaida = '0';
    }
}

void VerificaConexoesWiFIEMQTT() {
    if (!MQTT.connected())
        reconnectMQTT();
    reconectWiFi();
}

void EnviaEstadoOutputMQTT() {
    if (EstadoSaida == '1') {
        MQTT.publish(TOPICO_PUBLISH_1, "s|on");
        Serial.println("- Led Ligado");
    }

    if (EstadoSaida == '0') {
        MQTT.publish(TOPICO_PUBLISH_1, "s|off");
        Serial.println("- Led Desligado");
    }
    Serial.println("- Estado do LED onboard enviado ao broker!");
    delay(5000);
}

void InitOutput() {
    pinMode(D4, OUTPUT);
    digitalWrite(D4, HIGH);
    boolean toggle = false;

    for (int i = 0; i <= 10; i++) {
        toggle = !toggle;
        digitalWrite(D4, toggle);
        delay(200);
    }
}

void reconnectMQTT() {
    while (!MQTT.connected()) {
        Serial.print("* Tentando se conectar ao Broker MQTT: ");
        Serial.println(BROKER_MQTT);
        if (MQTT.connect(ID_MQTT)) {
            Serial.println("Conectado com sucesso ao broker MQTT!");
            MQTT.subscribe(TOPICO_SUBSCRIBE);
        } else {
            Serial.println("Falha ao reconectar no broker.");
            Serial.println("Haverá nova tentativa de conexão em 2s");
            delay(2000);
        }
    }
}




void TempHumi(){
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  String temp = String(t);
  String humi = String(h);
  Serial.print("Valor temperatura: ");
  Serial.println(temp.c_str());
  MQTT.publish(TOPICO_PUBLISH_4, temp.c_str());
  Serial.print("Valor umidade: ");
  Serial.println(humi.c_str());
  MQTT.publish(TOPICO_PUBLISH_3, temp.c_str());
}

void Lumi(){
  //Peguei esse script dos exemplos do própio wokwi
  const float gama = 0.7;
  const float x = 50;
  int lumino = analogRead(ldr);
  float voltagem = lumino / 1024. * 5;
  float resistencia = 2000*voltagem/(1-voltagem/5);
  float lumi = pow(x*1e3*pow(10, gama)/resistencia,(1 / gama));

  String luminosi = String(lumi);
  Serial.print("Luminosidade: ");
  Serial.println(luminosi.c_str());
  MQTT.publish(TOPICO_PUBLISH_2, luminosi.c_str());

}

void dist(){
  digitalWrite(TRIG, HIGH);
  delay(10);
  digitalWrite(TRIG, LOW);

  int dis = (pulseIn(ECHO, HIGH))/58;
  String distanci = String(dis);
  Serial.print("Distância: ");
  Serial.println(distanci.c_str());
    MQTT.publish(TOPICO_PUBLISH_5, distanci.c_str());

  if(dis>=limite){
    Serial.println("###########~~~~ AGUA TRANSBORDANDO ~~~~###########");
  }
  else{
    Serial.println("Agua está okay por agora");
  }


}
