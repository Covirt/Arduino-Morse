/*
  Morse Code Transmission Through IR

  Translates and transmits a sentence from a string through a wire so that it can be received by another Arduino running the same code.
  OR
  Receives and prints the incoming morse code through an analog pin

  The circuit:
   * Button and Resistor to digital pin 8 (TOGPIN)
   * Buzzer / outgoing Arduino to digital pin 13 (OUPIN)
   * Incoming Arduino to digital pin 7 (INPIN)
   * LED to digital pin 3
   * LED to digital pin 4 (both to see if Arduino is sending or receiving)

  
  created 20 Jan 2022
  by Victor Szwarcberg
  
  Limits:
   - Only lowercase letters can be processed by this sketch. Otherwise, it will repeat the last recognised letter.
   - This sketch does not translate from Morse to English though it will show the individual morse letters when receiving.
   - If future version, there may be a 40-character limit on received messages due to how the sketch translates from Morse to English
   - While the sending speed can be modified with the unit variable, the receiving speed cannot be so easily, please stick to 1 unit = 40ms
   - For easier recognition by the audio decoder due to the time it takes for the buzzer to reach its final tone, some durations have been modified from the international standard
     
  Resources used:
   - https://scoutlife.org/wp-content/uploads/2007/02/morsecode-1.jpg?w=700
   - https://morsecode.world/international/timing.html
   - https://forum.arduino.cc/t/solved-rounding-to-the-nearest-5/236962/2
   - https://www.arduino.cc/reference/en/language/functions/advanced-io/pulsein/
  
  You can test the "translate" function with:
   - https://morsecode.world/international/decoder/audio-decoder-adaptive.html (set the frequency to that of the buzzer (2625Hz) and WPM to 20)
   
*/

int alphabet[] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
String morse[] = {".-","-...","-.-.","-..",".","..-.","--.","....","..",".---","-.-",".-..","--","-.","---",".--.","--.-",".-.","...","-","..-","...-",".--","-..-","-.--","--.."};
const int unit = 40; //(best result at 20wpm)
#define OUPIN 13

#define INPIN 7
int durationH; //HIGH time centiseconds - could be volatile int
int durationL; //LOW time centiseconds - could be volatile int
unsigned long timestamp1 = 0;
unsigned long timestamp2;
String outputString = " ";

#define TOGPIN 8
bool state;

int charNumber = 0;
String toSendToTranslator[40]; //this means that received messages cannot be longer than 40 characters long

char txt[]="hello world";

void setup() {
  Serial.begin(9600);
  pinMode(OUPIN,OUTPUT);
  pinMode(INPIN, INPUT);
  pinMode(TOGPIN,INPUT);
  if (digitalRead(TOGPIN) == HIGH){state = true;digitalWrite(4,HIGH);}
  else {state = false;digitalWrite(3,HIGH);}
}

void loop() {
  while (not state){translate();delay(9000);}
  while (state){measure();}
}


void morse2LED(char morseChar){
  if (morseChar=='-'){
    digitalWrite(OUPIN,HIGH);
    delay(5*unit); // dah duration
    digitalWrite(OUPIN,LOW);
  }
  else if (morseChar=='.'){
    digitalWrite(OUPIN,HIGH);
    delay(unit); // dit duration
    digitalWrite(OUPIN,LOW);
  }
}

String translateChar(char charToFind){
  for (int i=0; i<sizeof(alphabet); i++){
     if (charToFind == alphabet[i]){return morse[i];}
  }
}

void translate(){
  for (byte i=0;i<strlen(txt);i++){
    Serial.print(" ");
    if(txt[i]==' '){
      delay(7*unit); //inter-word space
      Serial.print('/');
      continue;
    }
    String morseChar = translateChar(txt[i]);
    for (byte i=0;i<=morseChar.length();i++){
      Serial.print(morseChar[i]);
      morse2LED(morseChar[i]);
    delay(2*unit); //inter-character space
    continue;
    }
  }
}

void measure(){
  durationH = pulseIn(INPIN,HIGH,10000000)/10000;
  unsigned long timestamp2 = micros()/10000;
  durationL = ((timestamp2 - timestamp1)-(durationH));
  timestamp1 = timestamp2;
  if (durationL>40){outputString += "/";}
  else if (durationL>15){outputString += " ";}
  if (durationH == 0){
    outputString += "|";
    Serial.println(outputString);
    outputString = " ";
    }
  else if (durationH<15){outputString += ".";}
  else {outputString += "-";} 
}