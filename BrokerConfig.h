/*
 * BrokerConfig.h
 *
 *  Created on: 21 déc. 2016
 *      Author: olamotte
 */

#ifndef IOT_BROKERCONFIG_H_
#define IOT_BROKERCONFIG_H_

#include "Config.h"

class BrokerConfig: public Config {
public:
	
	BrokerConfig(String clientUID = "");
	BrokerConfig(String clientUID = "", String user = "", String pwd = "");
	virtual ~BrokerConfig();

	void setMQTTServer(String _mqttServer);
	void setMQTTPort(String _mqttPort);
	void setMQTTUID(String _mqttUID);
	void setMQTTUser(String _mqttUser);
	void setMQTTPwd(String _mqttPwd);

	String getMQTTServer();
	String getMQTTPort();
	String getMQTTUID();
	String getMQTTUser();
	String getMQTTPwd();

	bool load();
	bool save();

private:

	const String MQTT_CONFIG_FILE = "/mqttConfig.txt";

	String forcedMqttUID;
	String forcedMqttUser;
	String forcedMqttPwd;

	String mqttServer;
	String mqttPort;
	String mqttUID;
	String mqttUser;
	String mqttPwd;	
};

#endif /* IOT_BROKERCONFIG_H_ */
