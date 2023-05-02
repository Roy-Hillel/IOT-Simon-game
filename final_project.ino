/*  
 *  Multi-Player Simon memory game. Final project in IoT course semester 2022A.
 *  Instructable link: https://www.instructables.com/Multi-player-SIMON/
 *  Created by:
 *    Yoav Sternik #311170310
 *    Roy Hillel #204864805
 */

#include <Adafruit_CircuitPlayground.h>

#define BLYNK_PRINT SerialUSB
#define BLYNK_DEBUG SerialUSB
#define BLYNK_TEMPLATE_ID "TMPLC0Fm9VCe"
#define BLYNK_TEMPLATE_NAME "final pro"
#define BLYNK_AUTH_TOKEN "y2ZPLmNNXJWw5OR9mDdVYAU4fz0G8Y05"
#define EspSerial Serial1
#define ESP8266_BAUD 115200

#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>
#include <Adafruit_CircuitPlayground.h>

ESP8266 wifi(&EspSerial);

// blynk and wifi initiating
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Daniel";
char pass[] = "0587676557";

int game_round = 1;
int sequence[10];
static boolean first_round = true, game_on = false;
int start_time, end_time, score = 0;
int start_game_vp = 2;
int high_score_broken = 0;
int starter = 1;
int soundSensor = A0;
int sensorValue;

//parameters for scream recording
int s = 0;
float value = 0, maxVal = 0;

// IR controller codes for our remote control
uint32_t IR_value = 0;
#define IR_1 0x5B038877
#define IR_2 0x5B0348B7
#define IR_3 0x5B03C837
#define IR_4 0x5B0328D7
#define IR_5 0x5B03A857
#define IR_6 0x5B036897
#define IR_7 0x5B03E817
#define IR_8 0x5B0318E7
#define IR_9 0x5B039867


// Generates a new sequence for the current round
void generateSequence() {
  for (int i = 0; i < 10 ; i++) { // generate 10 random numbers to sequence array
    sequence[i] = 1 + random(9);
  }
}

// Displays the current sequence to the player
void displaySequence() {
  
  for (int i = 0; i < game_round; i++) { // iterate over sequence <game_round> times
    for (int l=0; l < sequence[i]; l++) { // for each stage turn on sequence[i] leds
      CircuitPlayground.setPixelColor(l, 0,0,255);
    }
    delay(700); // delay for the user to see the colors for each stage
    CircuitPlayground.clearPixels(); // turn off the stage's leds
    delay(500);
  }
}

// while playing a round this function returns win or lose
boolean playRound() {
  for (int i = 0; i < game_round; i++){
    int guess = input();
    for (int k=0; k < guess; k++) { // displays user guess
      CircuitPlayground.setPixelColor(k, 0,0,255);
    }
    delay(300);
    CircuitPlayground.clearPixels(); // clear user guess

    if (guess != sequence[i]) {
      return false;
    }
    
  }
  return true;
}

// lost round - start theme and lights accordingly
void lose() {
  for (int i=0; i < 10; i++) {
    CircuitPlayground.setPixelColor(i, 255,0,0);
  }
  CircuitPlayground.playTone(880, 500);
  CircuitPlayground.playTone(698, 500);
  CircuitPlayground.playTone(587, 500);
  CircuitPlayground.clearPixels();
}

// won round - start theme and lights accordingly
void winRound() {
  for (int i=0; i < 10; i++) {
    CircuitPlayground.setPixelColor(i, 0,255,0);
  }
  CircuitPlayground.playTone(587, 70);
  CircuitPlayground.playTone(698, 70);
  CircuitPlayground.playTone(880, 100);
  CircuitPlayground.clearPixels();
}

// after losing a round, or winning the game
void resetGame() {
  game_round = 1;
  generateSequence();
  startGameSong();
}

// after winning 10 rounds straight - winning theme and lights
void winGame() {
  for (int i=0; i < 10; i++) {
    CircuitPlayground.setPixelColor(i, 255,255,255);
  }
  
  CircuitPlayground.playTone(659, 50);
  CircuitPlayground.playTone(659, 50);
  CircuitPlayground.playTone(698, 50);
  CircuitPlayground.playTone(698, 50);
  CircuitPlayground.playTone(784, 50);
  CircuitPlayground.playTone(784, 50);
  CircuitPlayground.playTone(880, 50);
  CircuitPlayground.playTone(880, 50);
  CircuitPlayground.playTone(987, 50);
  CircuitPlayground.playTone(987, 50);
  CircuitPlayground.playTone(1046, 500);
  CircuitPlayground.clearPixels();
  delay(1000);
}

//when initiated and after reset
void startGameSong() {
    for (int i=0; i < 10; i++) {
    CircuitPlayground.setPixelColor(i, 200,70,70);
  }
  CircuitPlayground.playTone(523, 50);
  CircuitPlayground.playTone(587, 50);
  CircuitPlayground.playTone(659, 50);
  CircuitPlayground.playTone(698, 100);
  CircuitPlayground.playTone(784, 50);
  CircuitPlayground.playTone(880, 50);
  CircuitPlayground.playTone(987, 50);
  CircuitPlayground.playTone(1046, 100);
  CircuitPlayground.playTone(1174, 50);
  CircuitPlayground.playTone(1318, 50);
  CircuitPlayground.playTone(1396, 200);

  CircuitPlayground.clearPixels();

  delay(500);
}

// receieve IR code from controller using the CPX IR receiver sensor
// return the digit pressed
int input() {
  IR_value = 0;
  CircuitPlayground.irReceiver.enableIRIn();      //Restart receiver  

  while (true) {
    Blynk.run();
    if (CircuitPlayground.irReceiver.getResults()) {
      CircuitPlayground.irDecoder.decode(); 
      IR_value = CircuitPlayground.irDecoder.value;
      if (IR_value != 0 && IR_value != 0xFFFFFFFF) {
        switch (IR_value) {
          case IR_1: 
            return 1;
          case IR_2: 
            return 2;
          case IR_3: 
            return 3;
          case IR_4: 
            return 4;
          case IR_5: 
            return 5;
          case IR_6: 
            return 6;
          case IR_7: 
            return 7;
          case IR_8: 
            return 8;
          case IR_9: 
            return 9;
        }
      }
      CircuitPlayground.irReceiver.enableIRIn();      //Restart receiver  
    }
  }
}

/*
 * finish game function:
 * 1. receives level reached and time game started
 * 2. restored variables to reset cpx status - ready to listen to a new game
 * 3. sends the score to blynk via virtual pin V1
 */
void finishGame() {
  game_on = false;
  first_round = true;
  end_time = millis();
  score = (10-game_round)*1000 + ((end_time - start_time) / 1000) + 200000;
  Blynk.virtualWrite(V1, score);  
}

/*
 * Records loudest volume detected in the last 5 seconds.
 * writes the max volume in blynk virtual pin V3
 */
void recordScream() {
  for (int i=0; i < 10; i++) {
      CircuitPlayground.setPixelColor(i, 255,255,255);
  }
  while (s < 5) {
    value = CircuitPlayground.mic.soundPressureLevel(10);
    if (maxVal < value) {
      maxVal = value;
    }
    s++;
    delay(1000);
  }
  Blynk.virtualWrite(V3, maxVal);
  CircuitPlayground.clearPixels();
  maxVal = 0;
  s = 0;
  
}
void setup () {
    CircuitPlayground.begin();
    SerialUSB.begin(9600);
    EspSerial.begin(ESP8266_BAUD);
    Blynk.begin(auth, wifi, ssid, pass,"blynk.cloud", 80);
    CircuitPlayground.irReceiver.enableIRIn(); // start the IR receiver
}

void loop () {
  Blynk.run();
  if (CircuitPlayground.rightButton()){
    Blynk.virtualWrite(V2, starter);
    starter = (starter + 1) % 2;
  }
  if (start_game_vp < 2 || game_on) {
     if (first_round) { // reset game only for first iteration of loop
        start_game_vp = 2;
        game_on = true;
        resetGame();
        start_time = millis();
        first_round = false; 
     } 
     if (game_round == 10) { // won the game ! CONGRATULATIONS !!!
      winGame();
      finishGame();
    }
  
    displaySequence();
  
    if (!playRound()) { // LOST ROUND
      lose();
      game_round = game_round - 1;
      finishGame();
      
    }
    else { // WIN ROUND
      game_round = game_round + 1; // advance to next round
      winRound();
      generateSequence();
    }
    delay(1000);
   }
  
}

//Virtual pin V0 is the blynk "start_game"
BLYNK_WRITE(V0) {
  start_game_vp = param.asInt(); // assigning incoming value from pin V0 (startGame) to a variable
}

/*
 * Virtual pin V4 is "high score" - receives value only if high score was broken.
 * receives:
 *  1 - if player 1 broke the high score
 *  2 - if player 2 broke the high score
 */
BLYNK_WRITE(V4) {
  high_score_broken = param.asInt();
  if (high_score_broken == 2) {
    recordScream();
  }
}
