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
  //User has not moved
  //if (abs(val - analogVal) < error && !touched) return -1;
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
  
  // put your setup code here, to run once:
  Serial.begin(9600);
  //Serial.println("To initialize, turn potentiometer randomly the next three seconds");
  //delay(3000);
  //Serial.println("Stop now");
  //delay(2000);
  //Serial.println("Setting up");

  //If the random number is not seeded properly,
  //the same exact random numbers will be generated.
  analogVal = analogRead(potentiometer);
  randomSeed(analogVal);
}

//Hardware and software test loop
/*
void loop(){
  int knobTarget = 500;
  int knobVal = turnKnob();
  int wireVal = removeWire();
  int buttonVal = pressButton();
  
  if (true){
    Serial.println("Knob");
    Serial.println(knobVal);  
  }
  digitalWrite(led[0],knobVal==2 ? HIGH : LOW);
  digitalWrite(led[1], wireVal==3 ? HIGH : LOW);
  digitalWrite(led[2], buttonVal==6 ? HIGH : LOW);
  
  if (wireVal>-199){
    Serial.println("Wires");
    Serial.println(wireVal);
  }
  
  if (buttonVal!= 0){
    Serial.println("Buttons");
    Serial.println(buttonVal);
  }
}
*/

void loop() {
  // Set game states variables
  bool gameOver = false;
  bool won = false;
  bool progress[] = { false, false, false };
  
  // Set random prompts for player
  int wire = random(4);
  int knob = random(2) + 1;
  int code[3];
  int codeProgress = 0;
  for (int i = 0; i < 3; i++){
    code[i] = random(buttons) + 1;
  }

  //Set the potentiometer to untouched state
  //analogVal = analogRead(potentiometer);
  //touched = false;

  //Print the user prompts
  //TODO: Come up with better riddles and stuff and put them in a manual
  //Print the wire hint
  String signs = "";
  for (int i = 0; i < wire; i++){
    signs += '!';
  }
  Serial.println("New game! Get ready!" + signs);
  //Print the three-digit button code
  for (int i = 0; i < 3; i++){
    Serial.print(code[i]);
  }
  Serial.println("");
  //Print the knob turning hint
  switch(knob){
    case 1:
      Serial.println("Start!!!");
    case 2:
      Serial.println("Begin!!");
    default:
      Serial.println("Something's wrong with the code");
  }

  
  //Loop the game and print timer
  while (!gameOver){

    //Read the arduino for inputs
    int knobVal = turnKnob();
    int wireVal = removeWire();
    int buttonVal = pressButton();

    //Handle knob input
    if (!progress[0] && knobVal==knob){
      progress[0] = true;
    }
    else if (knobVal!=0){
      Serial.println("Incorrect knob setting");
      gameOver = true;
    }

    //Handle wire input
    if (wireVal<0){
      Serial.println("Too many wires removed");
      gameOver = true;
    }
    else if (!progress[1] && wireVal==wire){
      progress[1] = true;
    }
    else if (wireVal!=0){
      Serial.println("Wrong wire");
      gameOver = true;
    }

    //Handle button input
    if (codeProgress < 3 && buttonVal==code[codeProgress]){
      codeProgress++;
    }
    else if (buttonVal != 0){
      Serial.println("Wrong button");
      gameOver = true;
    }
    if (!progress[2] && codeProgress >=3){
      progress[2] = true;
    }

  
    //Turn on LEDs for progress  
    for (int i = 0; i < 3; i++){
      digitalWrite(led[i], progress[i] ? HIGH : LOW);
    }
    
    //Check for win
    won = progress[0] && progress[1] && progress[2];
    gameOver = won;
    
  }

  //If user loses, KABOOM!
  if (!won){
    //Serial.println("KABOOM!!!!");
    tone(speaker, 500);
    delay(1000);
    noTone(speaker);
  }

  //Tell user the result
  String result = won ? "won!" : "lost!";
  bool restart = false;
  Serial.println("You " + result);
  Serial.println("Set potentiometer to middle and press the first button to restart");

  //Wait for user to press restart button, which would be button # 1(pin 8)
  while (!restart){
    restart = digitalRead(firstButton)==HIGH;
  }
}
