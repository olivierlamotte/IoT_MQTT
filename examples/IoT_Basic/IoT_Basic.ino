/*
 * Utilisation du protocol MQTT pour l'envoie et la réception de messages 
 * 
 * L'IoT envoie toutes les secondes sur le topic IOT/IOT_BASIC/OUT/ALIVE un entier qui croit continuellement
 * 
 * L'IoT écoute également plusieurs topics :
 *    > IOT/IOT_BASIC/IN : affiche simplement le message qui est envoyé sur la console série
 *    > IOT/IOT_BASIC/ADMIN : permet de changer le niveau de log, de redémarrer la carte ou de réinitialiser sa configuration réseau/broker
 *    > IOT/IOT_BASIC/BLINK ON | OFF : active ou non le clignotement de la led interne à la carte
 */


#include "IoT.h"

const String ESP_NAME = "IOT_BASIC";
unsigned long aliveCount = 0;

IoT* iot;

unsigned long aliveTime = 0;
unsigned int alivePause = 1000;

unsigned long blinkLastTime = 0;
unsigned int blinkDelay = 500;

bool firstLoop = true;
bool blinkLed = false;
bool blinkedLedState = false;

// Initialisation
void setup()
{
  // Initialisation de l'objet Serial (attention, adaptez la vitesse de communication si besoin)
  Serial.begin(115200);

  // Configuration de la Led interne à la carte
  pinMode(LED_BUILTIN, OUTPUT);
  
  // Création d'un nouvelle instance d'un IoT avec comme nom le contenu de la variable ESP_NAME
  iot = new IoT(ESP_NAME);

  // Définit le niveau de log (LOG_DEBUG | LOG_FINEST | LOG_FINE | LOG_INFO | LOG_WARNING | LOG_ERROR | LOG_OFF)
  iot->setLogLevel(IoT::LOG_FINE);

  // Définit la fonction callback qui sera appelée lorsqu'un message sur un topic observé arrivera
  iot->createPubSubClient(pubSubCallback);
}




/**
 * Fonction appelée lorsqu'un message est envoyé sur un topic observé
 * 
 * topic : le topic sur lequel est arrivé le nouveau message
 * payload : le message associé au topic
 * length : la tailler du message
 */
void pubSubCallback(char* topic, byte* payload, unsigned int length) {
	
  // Read the incomming message
  String mqttIncommingMsg = "";
  for (unsigned int i = 0; i < length; i++) {
    mqttIncommingMsg += (char)payload[i];
  }

  String searchedAdminTopic = "IOT/"+ESP_NAME+"/ADMIN";
  String searchedBlinkTopic = "IOT/"+ESP_NAME+"/BLINK";
  if (String(topic) == searchedAdminTopic) {
    Serial.print("#IOT> PubSub callback on admin topic: ");
    Serial.print(topic);
    Serial.print(" = ");  
    Serial.println(mqttIncommingMsg);

    if (mqttIncommingMsg == "LOG_DEBUG") {
      iot->setLogLevel(IoT::LOG_DEBUG);
      Serial.println("IoT log changed for DEBUG");
    } else if (mqttIncommingMsg == "LOG_FINEST") {
      iot->setLogLevel(IoT::LOG_FINEST);
      Serial.println("IoT log changed for FINEST");
    } else if (mqttIncommingMsg == "LOG_FINE") {
      iot->setLogLevel(IoT::LOG_FINE);
      Serial.println("IoT log changed for FINE");
    } else if (mqttIncommingMsg == "LOG_INFO") {
      iot->setLogLevel(IoT::LOG_INFO);
      Serial.println("IoT log changed for INFO");
    } else if (mqttIncommingMsg == "LOG_WARNING") {
      iot->setLogLevel(IoT::LOG_WARNING);
      Serial.println("IoT log changed for WARNING");
    } else if (mqttIncommingMsg == "LOG_ERROR") {
      iot->setLogLevel(IoT::LOG_ERROR);
      Serial.println("IoT log changed for ERROR");
    } else if (mqttIncommingMsg == "LOG_OFF") {
      iot->setLogLevel(IoT::LOG_OFF);
      Serial.println("IoT log changed for OFF");
    } else if (mqttIncommingMsg == "IOT_REBOOT") {
      iot->restart(false);
    } else if (mqttIncommingMsg == "IOT_RESET_BROKER") {
      iot->resetConfig(IoT::BROKER);
    } else if (mqttIncommingMsg == "IOT_RESET_NETWORK") {
      iot->resetConfig(IoT::NETWORK);
    } else {
      Serial.println("Admin msg unknown");
    }

  } else if (String(topic) == searchedBlinkTopic) {

    if (mqttIncommingMsg == "ON") {
      blinkLed = true;
    } else {
      blinkLed = false;
      digitalWrite(LED_BUILTIN, LOW);
    }
    
  } else {
    Serial.print("#IOT> PubSub callback on topic: ");
    Serial.print(topic);
    Serial.print(" = ");	
	  Serial.println(mqttIncommingMsg);
  }
}





void loop()
{

  // Gestion du clignotement de la Led interne
  if (blinkLed && millis() >= blinkLastTime + blinkDelay) {
    if (blinkedLedState) {
      digitalWrite(LED_BUILTIN, LOW);
    } else {
      digitalWrite(LED_BUILTIN, HIGH);
    }
    blinkedLedState = !blinkedLedState;
    blinkLastTime = millis();
  }
  
  // Connection de l'IoT (reconnection automatique en cas de perte de connexion)
	IoT::CONNECT_RESULT result = iot->autoConnect();

  // Analyse du résultat de la connection (Optionnel)
	switch (result) {
	case (IoT::AP_ERROR) :
		Serial.println("#INO> AccessPoint error");
	break;
	case (IoT::CONFIG_WS_STARTED) :
		Serial.println("#INO> Configuration site started");
	break;
	case(IoT::CONFIG_WS_UPDATED) :
		//Serial.println("#INO> Configuration site updated");
	break;
	case (IoT::CONFIG_WS_UPDATE_FAILED) :
		Serial.println("#INO> Configuration site update failed");
	break;
	case (IoT::NETWORK_CONNECTED) :
		Serial.println("#INO> Network connected");
	break;
	case (IoT::NETWORK_CONNECTION_FAILED) :
		Serial.println("#INO> Network connection failed");
	break;
	case (IoT::BROKER_CONNECTED) :
		Serial.println("#INO> Broker connected");
	break;
	case (IoT::BROKER_CONNECTION_FAILED) :
		Serial.println("#INO> Broker connectio failed");
	break;
	case (IoT::WS_AND_BROKER_UPDATED) :
		//Serial.println("#INO> Broker and Web site updated");
	break;
	case (IoT::WS_UPDATE_FAILED) :
		Serial.println("#INO> Web site update failed");
	break;
	case (IoT::BROKER_UPDATE_FAILED) :
		Serial.println("#INO> Broker update failed");
	break;
	case (IoT::CONFIRM_WS_STARTED) :
		Serial.println("#INO> Confirmation web site started");
	break;
	case (IoT::CUSTOM_WS_STARTED) :
		Serial.println("#INO> Custom web site updated");
	break;
	}

  // Si l'IoT est connecté au réseau et au broker MQTT
	if (iot->isNetworkConnected() && iot->isMQTTBrokerConnected()) {

    // Lors de la première loop seulement
		if (firstLoop) {
			firstLoop = false;

      // Création du topic des messages communs
			String topic = "IOT/"+ESP_NAME+"/IN";

      // Inscription à l'écoute du topic
      // Note : doit être fait une fois l'IoT connecté, mais une seule fois (d'où le firstLoop)
			if (iot->mqttSubscribe(topic)) {
				Serial.print("#IOT> Subscribe ");
				Serial.print(ESP_NAME);
				Serial.println(" OK");
			} else {
				Serial.print("#IOT> Subscribe ");
				Serial.print(ESP_NAME);
				Serial.println(" NOK");
			}

      // Topic des messages d'administration
      String topicAdmin = "IOT/"+ESP_NAME+"/ADMIN";
      if (iot->mqttSubscribe(topicAdmin)) {
        Serial.print("#IOT> Subscribe Admin ");
        Serial.print(ESP_NAME);
        Serial.println(" OK");
      } else {
        Serial.print("#IOT> Subscribe Admin ");
        Serial.print(ESP_NAME);
        Serial.println(" NOK");
      }

      // Topic des messages d'activation de la led interne
      String topicBlink = "IOT/"+ESP_NAME+"/BLINK";
      if (iot->mqttSubscribe(topicBlink)) {
        Serial.print("#IOT> Subscribe Blink ");
        Serial.print(ESP_NAME);
        Serial.println(" OK");
      } else {
        Serial.print("#IOT> Subscribe Blink ");
        Serial.print(ESP_NAME);
        Serial.println(" NOK");
      }
		}

   

    // Transmission de la valeur de aliveCount toute les secondes
		if (millis() >= aliveTime + alivePause) {
			aliveTime = millis();
      // Création du topic
			String topicAlive = "IOT/"+ESP_NAME+"/OUT/ALIVE";
      // Création du message
			String msg = String(aliveCount);
      // Evolution du compteur pour le prochain envoie
			aliveCount++;

      // Publication
			iot->mqttPublish(topicAlive, msg);
		}
	}
}
