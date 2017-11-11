/*
 
  Original Author: Karl THOMAS https://github.com/karlth
  Adds : David Souder
  Version : 10/10/2017 - V4
*/

#ifndef DUINOEDU_ESP8266_H
#define DUINOEDU_ESP8266_H

#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>

#define JUMP s+=
#define TAB0 s+="\r\n";s+=
#define TAB1 s+="\r\n\t";s+=
#define TAB2 s+="\r\n\t\t";s+=
#define TAB3 s+="\r\n\t\t\t";s+=
#define COTE "'"

#ifndef OUI
#define OUI true
#endif

#ifndef NON
#define NON false
#endif


class Duinoedu_Esp8266	
{
  public:
    Duinoedu_Esp8266(){;}
	String DUINOEDU_IP;
	String DUINOEDU_SSID;
	String DUINOEDU_PWD;


int Received( Adafruit_MQTT_Subscribe &Feed , Adafruit_MQTT_Subscribe *subs){
	 if (subs == &Feed) {
		int Value = atoi((char *)&Feed.lastread);
		 return Value;
    }
}
	
void mqttpublish(Adafruit_MQTT_Publish Feed, String Flux ,int Value, uint32_t interval=2000 ) {
  
  String FluxSecure = Flux;
  //FluxSecure.toLowerCase();
    
  static uint32_t lastTime = 0;
  uint32_t thisTime = millis();
  if( thisTime < (lastTime+interval) ) return;
  lastTime=thisTime;
  
  
  Serial.print("Sending Feed__ :");
  Serial.print(FluxSecure);
  Serial.print(" val :");
  Serial.print(Value);
  Serial.print("...");
  if (! Feed.publish(Value)) {
    Serial.println(F("Failed"));
  } 
  else {
    Serial.println(F("OK!"));
  }
}	
	
void mqqtconnect_Adafruit(Adafruit_MQTT_Client *_mqtt  ){
  int8_t ret;

  // Stop if already connected.
  if (_mqtt->connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = _mqtt->connect()) != 0) { // connect will return 0 for connected
    Serial.println(_mqtt->connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 10 seconds...");
    _mqtt->disconnect();
    delay(10000);  // wait 10 seconds
    retries--;
    if (retries == 0) {
      while (1);
    }
  }
	Serial.println("MQTT Connected!");
	if(! _mqtt->ping()) {
    _mqtt->disconnect();
  }
	
}	
	
void connect_AP(const char *ssid,const char *password ){
	Serial.begin(9600);
	delay(500);
	WiFi.mode(WIFI_AP); //Our ESP8266-12E is an AccessPoint;
	WiFi.softAP( ssid, password ); //The command will only work if the password length is 8 characters or more.
	Serial.print ( "Connected to access point : " ); 
	Serial.println ( ssid );  
	Serial.print ( "IP address: " ); 
	DUINOEDU_IP=WiFi.softAPIP().toString();
	DUINOEDU_SSID=ssid;
	DUINOEDU_PWD=password;
	Serial.println ( WiFi.softAPIP() );
}

void connect_AP_SetIP(const char *ssid,const char *password, IPAddress _local_ip, IPAddress _subnet, IPAddress  _gateway){
	Serial.begin(9600);
	delay(500);
	WiFi.mode(WIFI_AP); 									//Our ESP8266-12E is an AccessPoint;
	WiFi.softAP( ssid, password ); 							//The command will only work if the password length is 8 characters or more.
	WiFi.softAPConfig(_local_ip, _gateway, _subnet);
	Serial.println("---");
	Serial.print ( "Connected to access point : " ); 
	Serial.println ( ssid );  
	Serial.print ( "IP address: " ); 
	DUINOEDU_IP=WiFi.softAPIP().toString();
	DUINOEDU_SSID=ssid;
	DUINOEDU_PWD=password;
	Serial.println ( WiFi.softAPIP() );
}

void connect_STA(const char *ssid,const char *password ) {
	Serial.begin(9600);
	WiFi.mode(WIFI_STA);
	WiFi.begin ( ssid, password ); 
	while ( WiFi.status() != WL_CONNECTED ) {  
	delay ( 500 ); 
	Serial.print ( "." ); 
	}
	Serial.println ( "" ); 
	Serial.print ( "Connected to station : " ); 
	Serial.println ( ssid );  
	Serial.print ( "IP address: " ); 
	DUINOEDU_IP=WiFi.localIP().toString();
	DUINOEDU_SSID=ssid;
	DUINOEDU_PWD=password;
	Serial.println ( WiFi.localIP() );
	delay ( 500 );
}

void connect_STA_SetIP(const char *ssid,const char *password, IPAddress _local_ip, IPAddress _subnet, IPAddress  _gateway) {
	Serial.begin(9600);
	WiFi.mode(WIFI_STA);
	WiFi.begin ( ssid, password ); 
	WiFi.config(_local_ip, _gateway, _subnet);
	while ( WiFi.status() != WL_CONNECTED ) {  
	delay ( 500 ); 
	Serial.print ( "." ); 
	}
	Serial.println ( "" ); 
	Serial.print ( "Connected to station : " ); 
	Serial.println ( ssid );  
	Serial.print ( "IP address: " ); 
	DUINOEDU_IP=WiFi.localIP().toString();
	DUINOEDU_SSID=ssid;
	DUINOEDU_PWD=password;
	Serial.println ( WiFi.localIP() );
	delay ( 500 );
}

void sendHttpRequest( String _serverHost,String _name, String _data ) {
  String _send;
  HTTPClient http;
  _serverHost="http://"+_serverHost+"/";
  http.begin(_serverHost);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  _send = _name+ "="+  _data;
  Serial.println("- data stream: "+_send);
  http.POST(_send);
  http.writeToStream(&Serial);
  http.end();
}

void updateStringint(ESP8266WebServer *_server, String _etiquette, int &_varDeStockage){
  if ( _server->hasArg(_etiquette )){
    _varDeStockage= (_server->arg(_etiquette).toInt());
    //Serial.print(_etiquette);
    //Serial.print(":");
    //Serial.println(_varDeStockage);
  }
}

void updateStringbool(ESP8266WebServer *_server, String _etiquette, boolean &_varDeStockage){
  if ( _server->hasArg(_etiquette) ){
    if ((_server->arg(_etiquette))==("0")) { _varDeStockage = 0;}
    else { _varDeStockage = 1;}
    //Serial.print(_etiquette);   
    //Serial.print(":");
    //Serial.println(_varDeStockage);
  }
}

void updateStringdouble (ESP8266WebServer *_server, String _etiquette, float _varDeStockage){
	
	if ( _server->hasArg(_etiquette )){
    _varDeStockage=((_server->arg(_etiquette))).toFloat() ;
    //Serial.print(_etiquette);
    //Serial.print(":");
    //Serial.println(_varDeStockage);
	
	}
}

void updateString(ESP8266WebServer *_server, String _etiquette, String &_varDeStockage){
  if ( _server->hasArg(_etiquette )){
    _varDeStockage= (_server->arg(_etiquette));
    Serial.print(_etiquette);
    Serial.print(":");
    Serial.println(_varDeStockage);
  }
}

String gauge(int _Min, int _Max, String _Title){
	return "<p><script src='https://canvas-gauges.com/download/latest/radial/gauge.min.js'></script><h3>"+_Title+"</h3><canvas id='"+_Title+"'></canvas><script>    var gauge"+_Title+" = new RadialGauge({renderTo: '"+_Title+"', width: 200, height: 200, barWidth: 5, colorBarProgress: '#00FF00', minValue: "+String(_Min)+", maxValue: "+String(_Max)+", majorTicks: ['"+String(_Min)+"','"+String(_Max)+"'],animation: true,animationRule: 'linear',animateOnInit: true,animatedValue: true,listeners: { value: function(value) { this.update({ units: parseInt(value, 10) }); }}}).draw();</script></p>";	
}

String iframe(String _Url, int _Width, int _Height){
	return "<p><iframe style='border: 1px #FFFFFF none;' src='+_Url+' width='"+String(_Width)+"px' height='"+String(_Height)+"px' frameborder='1' marginwidth='0px' marginheight='0px' scrolling='yes'></iframe></p>";
}

String javaslider(int _Max=255) {

//==== SCRIPT 01 : script initial Karl
//return "<script type=\"text/javascript\">\r\nfunction sendDac(pin,value){\r\nserver = \"http://"+DUINOEDU_IP+"/?\"+pin+\"=\"+value;\r\nrequest=new XMLHttpRequest();\r\nrequest.open(\"GET\", server, true);\r\nrequest.send();\r\nValNum = \"value\"+pin;\r\ndocument.getElementById(ValNum).innerHTML=value;\r\n}\r\n</script> \r\n";

//==== SCRIPT 02 : script initial Karl
return "<script type=\"text/javascript\">\r\nfunction sendDac(pin,value){\r\nserver = \"http://"+DUINOEDU_IP+"/?\"+pin+\"=\"+value;\r\nrequest=new XMLHttpRequest();\r\nrequest.open(\"GET\", server, true);\r\nrequest.send(null);\r\nValNum = \"value\"+pin;\r\ndocument.getElementById(ValNum).innerHTML=value;\r\n}\r\n</script> \r\n";

//==== SCRIPT 03 : script David - en test 
/*
String s="";
TAB0"<script type=\"text/javascript\">";
TAB1	"var lastTime=0;";	
TAB1	"function sendDac(pin,value, interval){";
TAB2		"ValNum = \"value\" + pin;";
TAB2		"thisTime = Date.now();";
TAB2		"document.getElementById(ValNum).innerHTML=value;";					// Temps réel
TAB2		"if( thisTime > (lastTime+interval) ){";							// Modéré
TAB3			"server = \"http://"+DUINOEDU_IP+"/?\"+pin+\"=\"+value;";		
TAB3			"request = new XMLHttpRequest();";
TAB3			"request.open(\"GET\", server, true);";
TAB3			"request.send();";
TAB3			"lastTime = Date.now();";	
TAB2		"}";				
TAB1	"}";
TAB0"</script>";			
return s;
*/
}

String addPhoneStyle(){
	
String cssStyle;
cssStyle = "";
cssStyle += "\r\n<style> \r\n";

// Style général	
cssStyle += "\
*{\r\n\
	font-size:28px; \r\n\
	font-family:Arial; \r\n\
}\r\n";
	
cssStyle += "\
body{\r\n\
	padding-left:5%; \r\n\
	padding-right:5%; \r\n\
}\r\n";
			
cssStyle += "\
h2{\r\n\
		font-size:32px; \r\n\
}\r\n";

//==== SLIDER
cssStyle += "\
input[type=range] {\r\n\
	-webkit-appearance: none;\r\n\
	width: 100%;\
}\r\n";
			
cssStyle += "\
.slider::-webkit-slider-runnable-track {\r\n\
	width: 100%;\r\n\
	height: 50px;\r\n\
	cursor: pointer;\r\n\
	background-image:linear-gradient(#eeeeee, #eeeeee);\r\n\
	border-radius: 50px;\r\n\
}\r\n";
	
cssStyle += "\
.slider::-webkit-slider-thumb {\r\n\
	height: 50px; \r\n\
	width: 75px; \r\n\
	border-radius: 50px; \r\n\
	background-image:linear-gradient(#5588ee, #3355bb); \r\n\
	cursor: pointer; \r\n\
	-webkit-appearance: none; \r\n\
	margin-bottom: 0px; \r\n\
}\r\n";

//==== INTERRUPTEUR
cssStyle += "\
.inter::-webkit-slider-runnable-track {\r\n\
	width: 100%;\r\n\
	height: 50px;\r\n\
	cursor: pointer;\r\n\
	background-image:linear-gradient(#aaaaaa, #aaaaaa);\r\n\
	border-radius: 5px;\r\n\
}\r\n";
	
cssStyle += "\
.inter::-webkit-slider-thumb,.interGreen::-webkit-slider-thumb\r\n\
{\r\n\
	height: 50px;\r\n\
	width: 50%;\r\n\
	background-image:linear-gradient(#5588ee, #3355bb);\r\n\
	cursor: pointer;\r\n\
	-webkit-appearance: none;\r\n\
	margin-bottom: 0px;\r\n\
	border-radius: 5px;\r\n\
}\r\n";



//==== POUSSOIR
cssStyle += "\
.push{\
	height: 54px;\r\n\
	width: 100%;\r\n\
	background-image:linear-gradient(#5588ee, #3355bb);\r\n\
	-webkit-appearance: none;\r\n\
	margin-bottom: 0px;\r\n\
	border-radius: 5px;\r\n\
}\r\n";
	

// Style valeur dynamique (Ex:1023)			
cssStyle += "\
.value{\r\n\
	font-size:28px;\r\n\
	color:#3388ff;\r\n\
	Font-Weight:bold;\r\n\
}\r\n";

// Style nom de l'élément (Ex:BOUTON1)		
cssStyle += "\
.labelTab{\r\n\
	min-width:80px;\r\n\
	width:40%;\r\n\
}\r\n";

cssStyle += "\
.valueTab{\r\n\
	min-width:80px;\r\n\
	width:10%;\r\n\
	text-align:right;\r\n\
}\r\n";
	
cssStyle += "</style>\r\n";
	
return cssStyle; 

}

String addSpacer(uint8_t _Space){
String s="" ;
TAB0"<div"; 
JUMP	" style='";
JUMP		" float:left\;";
JUMP		" width:"+String(_Space)+"%\;";
JUMP	"'";
JUMP">";
TAB1	"&nbsp;";
TAB0"</div>";	
return s;
}

String startDiv(uint8_t _Width){
String s="" ;
	TAB0"<div"; 
	JUMP	" style='";
	JUMP		" float:left\;";
	JUMP		" width:"+String(_Width)+"%\;";
	JUMP	"'";
	JUMP">";
return s;	
}

String endDiv(){
return "</div>";	
}

String slider(int _Min, int _Max, String _Title, bool _DisplayNam=true, bool _DisplayValue=true, bool _DisplaySeparator=true,uint8_t _Width=100){

//==== Script initial Karl
// return "<tr>\r\n<td align=\'center\'>\r\n"+_Title+"\r\n <br>\r\n<input type=\'range\' style=\'width: 90px; height: 30px;\' id=\'"+_Title+"\' min=\'"+String(_Min)+"\' max=\'"+String(_Max)+"\' value=\'0\'step=\'1\' onchange=\'sendDac(\""+_Title+"\",this.value);\'/>\r\n<br>\r\n<span id=\'value"+_Title+"\'>0</span>\r\n</td>\r\n</tr>\r\n<hr>\r\n";

//==== Nouveau script
// CONFIG PAR DEFAUT
		int interval = 1000;	
	// Config attributs html par défaut
		String classCss 		=	"";
		String id				=	" id="+_Title;			
		String type				=	"";		
		String min				=	"";
		String max				=	"";
		String value			=	" value=0";
		String step				=	"";
	// Config gestionnaires événements par défaut
		// String sendDac = "'sendDac(\""+_Title+"\",this.value," +interval+");'"; 
		String sendDac 			= 	"'sendDac(\""+_Title+"\",this.value);'";
		String onChange 		=	" onchange="+sendDac;
		String onmousedown		=	"";
		String onmouseup		=	"";
		String ontouchstart		=	"";
		String ontouchend		=	"";
		String onmousemove  	=	"";
		String oninput			=	"";

// INPUT 
    // SLIDER/INTER/PUSH : classe
		if(_Max==0) classCss	=	" class=push";
		if(_Max==1)	classCss	=	" class=inter";
		if(_Max> 1) classCss	=	" class=slider";
			
	// SLIDER/INTER
		if(_Max>0) {
			type				=	" type=range"; 
			min					=	" min="+String(_Min);
			max					=	" max="+String(_Max);
			step				=	" step=1";
		}
	
	// SLIDER ONLY 
		if(_Max>1){
			//onmousemove=	" onmousemove="+sendDac;
			//oninput=		" oninput="+sendDac;
		}
	
	// PUSH ONLY
		if(_Max==0) {
			type				=	" type=button"; 
			onmousedown			=	" onmousedown='sendDac(\""+_Title+"\",1,200);'";
			ontouchstart		=   " ontouchstart='sendDac(\""+_Title+"\",1,200);'";
			onmouseup			=	" onmouseup='sendDac(\""+_Title+"\",0,200);'";
			ontouchend			=	" ontouchend='sendDac(\""+_Title+"\",0,200);'";		
		}


// Chaîne liée au slider
String s="" ;
/* Pour rappel
#define JUMP s+=
#define TAB0 s+="\r\n";s+=
#define TAB1 s+="\r\n\t";s+=
#define TAB2 s+="\r\n\t\t";s+=
#define TAB3 s+="\r\n\t\t\t";s+=
#define COTE "'"
*/

// DIV : GESTION DES COLONNES
/*
TAB0"<div"; 
JUMP	" style='";
JUMP		" float:left\;";
JUMP		" width:"+String(_Width)+"%\;";
JUMP	"'";
JUMP">";
*/

// TABLE : DEBUT
	TAB0"<table width=100%>";
	TAB1	"<tr>";
// Affichage : NOM ELEMENT
if(_DisplayNam){
	TAB2		"<td class=labelTab>"; 
	TAB3			_Title;
	TAB2		"</td>";
}
//  Affichage : OBJET:SLIDER/INTER/PUSH
	TAB2		"<td>";
	TAB3			"<input";
	JUMP				classCss;
	JUMP				type;
	JUMP				id; 
	JUMP				min;
	JUMP				max; 
	JUMP				value;
	JUMP				step;											
	JUMP				onChange; 
	JUMP				onmousedown;
	JUMP				ontouchstart;
	JUMP				onmouseup;	
	JUMP				ontouchend;	
	JUMP 				onmousemove;
	JUMP				oninput;	
	JUMP   			" />";
	TAB2		"</td>";

//  Affichage : VALEUR
if(_DisplayValue){
	TAB2 		"<td class=valueTab>";
	TAB3 			"<span class=value id=\'value"+_Title+"\'>0</span>";
	TAB2 		"</td>";
}
// TABLE : FIN
	TAB1 	"</tr>";
	TAB0"</table>";
	
// SEPARATEUR : HR
if(_DisplaySeparator){
	TAB0"<hr>";
	JUMP"\r\n";
}

// DIV : FIN
/*
TAB0"</div>";
*/

// NOUVEAU CONTENEUR CONTROL

/* Voir si c'est mieux avec du CSS
.grid {
  display: grid;
  grid-template-columns: 1fr 1fr 1fr;
  grid-template-rows: 500px;
}
https://developer.mozilla.org/fr/docs/Web/CSS/grid-template 
*/

/* CODE GENERE
<table width=100%>
	<tr>
		<td class=labelTab>
			LED_D3
		</td>
		<td>
			<input class=inter type=range id=LED_D3 min=0 max=1 value=0 step=1 onchange='sendDac("LED_D3",this.value);' />
		</td>
		<td class=valueTab>
			<span class=value id='valueLED_D3'>0</span>
		</td>
	</tr>
</table>
*/

return s;	

}
	
String javaScript_start(){
	String javaScript="<SCRIPT>\n";
	javaScript+="var xmlHttp=createXmlHttpObject();\n";
	javaScript+="function createXmlHttpObject(){\n";
	javaScript+=" if(window.XMLHttpRequest){\n";
	javaScript+="    xmlHttp=new XMLHttpRequest();\n";
	javaScript+=" }else{\n";
	javaScript+="    xmlHttp=new ActiveXObject('Microsoft.XMLHTTP');\n";
	javaScript+=" }\n";
	javaScript+=" return xmlHttp;\n";
	javaScript+="}\n";
	javaScript+="function process(){\n";
	javaScript+=" if(xmlHttp.readyState==0 || xmlHttp.readyState==4){\n";
	javaScript+="   xmlHttp.open('PUT','xml',true);\n";
	javaScript+="   xmlHttp.onreadystatechange=handleServerResponse;\n"; // no brackets?????
	javaScript+="   xmlHttp.send(null);\n";
	javaScript+=" }\n";
	javaScript+=" setTimeout('process()',500);\n";
	javaScript+="}\n";
	javaScript+="function handleServerResponse(){\n";
	javaScript+=" if(xmlHttp.readyState==4 && xmlHttp.status==200){\n";
	javaScript+="   xmlResponse=xmlHttp.responseXML;\n";	
	return javaScript;
}

String javaScript_end(){
	String javaScript=" }\n";
	javaScript+="}\n";
	javaScript+="</SCRIPT>\n";
	return javaScript;
}

void initExperimentShield(){
	// D4,D6,D8,D9,D12,D13 //D5=>LOW
	uint8_t pins[] ={5,9,10,11,12,13};
	for(uint8_t i=0;i<6;i++){
		uint8_t j=pins[i];
		pinMode(j, OUTPUT);
		digitalWrite(j, LOW);
	}
}


};

#endif