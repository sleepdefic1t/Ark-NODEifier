# Ark-NODEifier
ARK-(NODE)ifier | Ark Ecosystem Delegate Node Notifier for ESP8266 / Arduino

POC using Ark-Cpp (https://github.com/ark-iot/ark-cpp)



# What is this?

  A simple template for making your own Ark Delegate Node notifier.  
  You can set it up and tweak this however you'd like;  
  but this example provides an entry point for alerts on missed blocks,  
  if your delegate rank changes, or is voted in or out of the top 51.



## Hardware & Software

  Testing and Development was done using the following:  
    - [Adafruit Feather ESP8266](https://www.adafruit.com/product/2821) (Development Board)  
    - [Ark-Cpp](https://github.com/Ark-IoT/Ark-Cpp) (Ark Ecosystem Arduino/C++ Wrapper)  
    - [Arduino IDE](https://www.arduino.cc/en/Main/Software)  
    - [ESP8266 Arduino Core](https://github.com/esp8266/Arduino)  
    - [VSCode](https://code.visualstudio.com/)  



## NODEifier.ino Configuration



Set your WiFi SSID and Password.
```cpp
/********************************************************************************/
/*  WiFi Configuration  */
const char* ssid = "yourWiFiSSID";
const char* password = "yourWiFiPassword";
/********************************************************************************/

Set your Delegate/Nodes Username and Publickey
/********************************************************************************/
/*  Ark Delegate Username & Publickey */
#define DELEGATE_USERNAME "yourDelegateUsername"
#define DELEGATE_PUBLICKEY "yourDelegatePublickey"
/********************************************************************************/
```


Below are example templates of paramters you might need to RollYourOwn notification service:
```cpp
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
```



## Interesting Parts



```cpp
enum AlertType { MissedBlocks, RankChanged, VotedOut, VotedIn, TEST };
```



```cpp
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
void getDelegateStatusFrom(ARK::API::Manager& arkManager) {

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
```



```cpp
/********************************************************************************
*
*   @method:  doAlert(ofType)
*   @parameter: Alert Type
*               enum { MissedBlocks, RankChanged, VotedOut, VotedIn, TEST }
*   @out:  sendNotificationWith(message)
*    
*       Handles and prepares the kind of alert to be sent
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
```



This is the entry point for making your own notification handling.
```cpp
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
```



## What now?

Try it out. Tweak it. Port it. Share it.. Whatever you'd like.
```cpp
  do
    hack();
  while (breath);
```
