/********************************************************************************
 * 
 *  Ark-(NODE)ifier
 *  NODEifier.ino
 *  
 *  An Ark Ecosystem Delegate Node Notifier
 *    Send alerts if your Delegate node stops forging or changes rank.
 * 
 *  MIT License
 *  2018 sleepdeficit
 *  
 *  
 *  requires:
 *    ARK-Cpp
 *      https://github.com/Ark-IoT/Ark-Cpp
 *      
 *    ESP8266WiFi (ESP8266 Arduino Core)
 *      https://github.com/esp8266/Arduino
 *      ARDUINO Board Manager URL:
 *        http://arduino.esp8266.com/stable/package_esp8266com_index.json
 *  
 ********************************************************************************/

#include <ark.h>
#include <ESP8266WiFi.h>

/********************************************************************************/
/*  WiFi Configuration  */
const char* ssid = "yourWiFiSSID";
const char* password = "yourWiFiPassword";
/********************************************************************************/

/********************************************************************************/
/*  SMS GATEWAY EXAMPLES
      555-555-5555  AT&T  5555555555@txt.att.net
      555-555-5555  MetroPCS  5555555555@mymetropcs.com
      555-555-5555  Sprint  5555555555@messaging.sprintpcs.com
      555-555-5555  T-Mobile  5555555555@tmomail.net
      555-555-5555  Verizon 5555555555@vtext.com
*/
#define SMS "5555555555@myCarriersSMSGateway.com"

#define SUBJECT "Ark-(Node)ifier Demo"

#define SMTP_PORT 465
#define SMTP_SERVER "smtp.gmail.com"
#define SMTP_EMAIL: "yourGmailAddress"
#define SMTP_PASSWORD "yourGmailPassword"
/********************************************************************************/

/********************************************************************************/
/*  Ark Delegate Username & Publickey */
#define DELEGATE_USERNAME "yourDelegateUsername"
#define DELEGATE_PUBLICKEY "yourDelegatePublickey"
/********************************************************************************/

/********************************************************************************/
/*  Ark Network (Devnet || Mainnet) */
#define ARK_DEVNET ARK::Constants::Networks::Devnet::model
/********************************************************************************/


/********************************************************************************/
/********************************************************************************/
enum AlertType { MissedBlocks, RankChanged, VotedOut, VotedIn, TEST };

bool needsUpdate = true;
bool isTest = true;

int lastMissedBlocksCount;
int currentMissedBlocksCount;

int lastRank;
int currentRank;
/********************************************************************************/


/********************************************************************************/
/********************************************************************************
 *  @method:  setupWiFi()
 *  
 *      Sets WiFi to STA-mode, waits for connection.
 * 
*********************************************************************************/
void setupWiFi() {
  WiFi.mode(WIFI_STA); // Infrastructure/Station mode
  WiFi.begin(ssid, password); // You WiFi Address & Password
  while (WiFi.status() != WL_CONNECTED) //  Check if Connected
  { delay(500); Serial.println("waiting for connection..."); }  //  Delay if NOT connected
  Serial.print("local IP: "); Serial.println(WiFi.localIP()); // Print local IPAddress when connected
};
/********************************************************************************/




/********************************************************************************/
/********************************************************************************
*
*   @method:  sendNotificationWith(message)
*   @parameter: String message
*   @out:  doAlert(oftype)
*    
*       Entry for RYO notification handling (e.g. SMS, email, etc.).
*      
*********************************************************************************/
void sendNotificationWith(String message)
{
  Serial.println(message);
  // do something | SMS / EMAIL
}

/********************************************************************************
*
*   @method:  doAlert(ofType)
*   @parameter: Alert Type
*               enum { MissedBlocks, RankChanged, VotedOut, VotedIn, TEST }
*   @out:  sendNotificationWith(message)
*    
*       Handles and prepares the type of alert to be sent
*      
*********************************************************************************/
void doAlert(AlertType ofType) {

  String message;
  
  switch (ofType) {

    case MissedBlocks:
      message += DELEGATE_USERNAME;
      message += " is missing ";
      message += (currentMissedBlocksCount - lastMissedBlocksCount);
      message += (" blocks\n");
      break;
      
    case RankChanged:
      message += DELEGATE_USERNAME;
      message += " rank changed to ";
      message += currentRank;
      message += "\n";
      break;

    case VotedOut:
      message += DELEGATE_USERNAME;
      message += " no longer in top 51,\n currently ranked # ";
      message += currentRank;
      message += "\n";
      break;

    case VotedIn:
      message += DELEGATE_USERNAME;
      message += " is now in top 51,\n currently ranked # ";
      message += currentRank;
      message += "\n";
      break;

      
    case TEST:
      message += "Testing for Delegate: ";
      message += DELEGATE_USERNAME;
      message += "\n currently ranked # ";
      message += currentRank;
      message += "\n";
      break;
  }

  sendNotificationWith(message);
  needsUpdate = true;

}
/********************************************************************************/


/********************************************************************************/
/********************************************************************************
*
*   @method:  getDelegateStatusFrom(arkManager)
*   @parameter: ARK::API::Manager
*   @out:  doAlert(oftype)
*    
*       Uses arkManager instance to get delegate info from Ark Blockchain.
*      
*********************************************************************************/
void getDelegateStatusFrom(ARK::API::Manager arkManager) {

  ARK::Delegate delegate = arkManager.delegate(DELEGATE_USERNAME);
  
  Serial.print("Ark Delegate: ");
  Serial.print(DELEGATE_USERNAME);
  Serial.print("\nCurrent Rank: ");
  Serial.print(delegate.rate);
  Serial.print("\nCurrent productivity: ");
  Serial.print(delegate.productivity);
  Serial.print("%\n");


  if (needsUpdate) {
    needsUpdate = false;
    lastMissedBlocksCount = delegate.missedblocks;
    lastRank = delegate.rate;    
  }

  currentMissedBlocksCount = delegate.missedblocks;
  currentRank = delegate.rate;

  if (currentMissedBlocksCount < lastMissedBlocksCount)
    doAlert(MissedBlocks);

  if (currentRank != lastRank)
    doAlert(RankChanged);
    
  if (currentRank > 51 && lastRank <= 51)
    doAlert(VotedOut);

  if (currentRank <= 51 && lastRank > 51)
    doAlert(VotedIn);

  if (isTest)
    doAlert(TEST);
    
}
/********************************************************************************/


/********************************************************************************/
void setup()
{
  Serial.begin(115200);
  setupWiFi();
}
/********************************************************************************/
void loop()
{
  ARK::API::Manager arkManager(ARK_DEVNET); /*  connect Ark API Manager */

  getDelegateStatusFrom(arkManager);  /*  getDelegateStatus */
  
  delay(300000); /*  sleep for 5 minutes */
}
/********************************************************************************/
