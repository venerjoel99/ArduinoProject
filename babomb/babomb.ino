//Breadboard configuration

//Digital pins for the wires
int red = 5;
int white = 6;
int blue = 7;

//Digital pins for LEDs
int led[] = {2,3,4};

//Analog pin for the potentiometer
int potentiometer = 0;
int lowMax = 300;
int highMin = 800;

//Digital pins for buttons and settings
int buttons = 6;
int firstButton = 8;
int buttonWait = 200;

//Potentiometer settings
int analogVal = -1;
int error = 10;
//bool touched = false;

//Speaker pin
int speaker = 0;
int freq = 80;

//Time limit
int seconds = 60;
int lightDelay = 500;

/**
 * Return an output based on button pressed
 */
int pressButton(){
  for (int i = firstButton; i < firstButton + buttons; i++){
    if (digitalRead(i)==HIGH){
      //Set a delay to acccount for button going crazy
      delay(buttonWait);
      return i - firstButton + 1;
    }
  }
  return 0;
}

/**
 * Check which wire is removed
 */
int removeWire(){
  int redVal = digitalRead(red);
  int whiteVal = digitalRead(white);
  int blueVal = digitalRead(blue);
  int sum = redVal + whiteVal + blueVal;
  if (sum < 2){
    return -1;
  }
  if (redVal==0) return 1;
  else if (whiteVal==0) return 2;
  else if (blueVal==0) return 3;
  else return 0;
}

/**
 * Check the state of the potentiometer
 */
int turnKnob(){
  int val = analogRead(potentiometer);
  if (val < lowMax) return 1;
  else if (val > highMin) return 2;
  else return 0;
}

void setup() {

  //Set up LEDs as output
  pinMode(led[0], OUTPUT);
  pinMode(led[1], OUTPUT);
  pinMode(led[2], OUTPUT);

  //Set up buttons as input
  for (int i = firstButton; i < firstButton + buttons; i++){
    pinMode(i, INPUT);
  }

  //Speaker test
  tone(speaker, freq);
  delay(500);
  noTone(speaker);
}

void loop() {
  // Set game states variables
  bool gameOver = false;
  bool won = false;
  bool progress[] = { false, false, false };
  
  // Set the prompts for player
  int wire = 3;
  int knob = 2;
  int codeProgress = 1;
  int runtime = 0;
  int iterations = 0;

  //Loop the game
  while (!gameOver){
    //Read the arduino for inputs
    int knobVal = turnKnob();
    int wireVal = removeWire();
    int buttonVal = pressButton();

    //Game is over if time runs out
    //Time limit as of now is 60 seconds
    delay(200);
    iterations++;
    if (iterations / 5 >= seconds){
      gameOver = true;
    }

    //Handle knob input
    progress[0] = knobVal==knob;
    if (knobVal!=0 && !progress[0]){
      gameOver = true;
    }

    //Handle wire input
    progress[1] = wireVal==wire;
    if (wireVal!=0 && !progress[1]){
      gameOver = true;
    }
    
    //Handle button input
    if (codeProgress <= buttons && buttonVal==codeProgress){
      codeProgress++;
    }
    else if (buttonVal != 0){
      gameOver = true;
    }
    if (!progress[2] && codeProgress > buttons){
      progress[2] = true;
    }
  
    //Turn on LEDs for progress  
    for (int i = 0; i < 3; i++){
      digitalWrite(led[i], progress[i] ? HIGH : LOW);
    }
    
    //Check for win
    won = progress[0] && progress[1] && progress[2];
    if (won){
      gameOver = won;
    }
  }

  //If user loses, KABOOM!
  if (!won){
    for (int i = 0; i < 4; i++){
      for (int i = freq; i < freq + 200; i++){
        tone(speaker, i);
        delay(1);
      }
      for (int i = freq + 200; i > freq; i--){
        tone(speaker, i);
        delay(1);
      }
    }
    noTone(speaker);
  }
  //If user wins, speaker beeps!
  else {
    tone(speaker, freq);
    delay(300);
    noTone(speaker);
    delay(300);
    tone(speaker, freq * 2);
    delay(300);
    noTone(speaker);
  }

  int knobVal = turnKnob();
  int wireVal = removeWire();
  int num = 0;
  bool pressed = false;

  //First turn off all progress lights
  //Indicate game is not reset by elevator pattern lights during while loops
  for (int i = 0; i < 3; i++){
    digitalWrite(led[i], LOW);
  }

  //Wait until potentiometer and wires are reset
  while(knobVal != 0 || wireVal != 0){
    knobVal = turnKnob();
    wireVal = removeWire();
    digitalWrite(led[num], LOW);
    num = (num + 1) % 3;
    digitalWrite(led[num], HIGH);
    delay(lightDelay);
  }

  //Then wait for the player to press the first button to restart
  while (!pressed){
    int val = pressButton();
    pressed = val==1;
    digitalWrite(led[num], LOW);
    num = (num + 1) % 3;
    digitalWrite(led[num], HIGH);
    delay(lightDelay - buttonWait);
  }
  
  //Code loops back to start; restarting the game again.
}
