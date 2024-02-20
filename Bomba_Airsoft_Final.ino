#include <Keypad.h>
#include <LCD_I2C.h>

int Scount = 0;       // count seconds
int Mcount = 0;       // count minutes
int Hcount = 0;       // count hours
int DefuseTimer = 0;  // set timer to 0

int Hours = 00;
int Minutes = 00;
int Seconds = 00;
int keycount = 0;

int hourstenscode;
int hoursonescode;
int mintenscode;
int minonescode;
int sectenscode;
int seconescode;

long secMillis = 0;    // store last time for second add
long interval = 1000;  // interval for seconds

char password[4];       // number of characters in our password
int currentLength = 0;  //defines which number we are currently writing
int i = 0;
char entered[4];
int modo = 0;
int fim = 0;
bool timeout = false;

const byte ROWS = 4;
const byte COLS = 3;


const int buzzer = 13;       //buzzer
const int ledvermelho = 12;  //red led
const int ledamarelo = 11;   //yellow led
const int ledverde = 10;     //green led
const int botaoamarelo = A0;
const int botaovermelho = A1;

char hexaKeys[ROWS][COLS] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' }
};

byte rowPins[ROWS] = { 4, 9, 8, 6 };  // Conecte os pinos das linhas do teclado a estes pinos no Arduino
byte colPins[COLS] = { 5, 3, 7 };     // Conecte os pinos das colunas do teclado a estes pinos no Arduino

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

void (*resetFunc)(void) = 0;

LCD_I2C lcd(0x27, 16, 2);  // Default address of most PCF8574 modules, change according

void setup() {
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;

  // 1 Hz (16000000/((15624+1)*1024))
  OCR1A = 15624;
  // CTC
  TCCR1B |= (1 << WGM12);
  // Prescaler 1024
  TCCR1B |= (1 << CS12) | (1 << CS10);
  interrupts();

  pinMode(ledvermelho, OUTPUT);    // sets the digital pin as output
  pinMode(ledamarelo, OUTPUT);     // sets the digital pin as output
  pinMode(ledverde, OUTPUT);       // sets the digital pin as output
  pinMode(buzzer, OUTPUT);         // sets the digital pin as output
  digitalWrite(ledvermelho, LOW);  // sets the LED off
  digitalWrite(ledamarelo, LOW);   // sets the LED off
  digitalWrite(ledverde, LOW);     // sets the LED off
  pinMode(3, INPUT);               //Keypad
  pinMode(4, INPUT);               //Keypad
  pinMode(5, INPUT);               //Keypad
  pinMode(6, INPUT);               //Keypad
  pinMode(7, INPUT);               //Keypad
  pinMode(8, INPUT);               //Keypad
  pinMode(9, INPUT);               //Keypad
  pinMode(A0, INPUT_PULLUP);
  pinMode(A1, INPUT_PULLUP);
  lcd.begin();
  lcd.backlight();
  lcd.print("BOMBA AIRSOFT");
  lcd.setCursor(0, 1);
  lcd.print("MODO:");
}

void loop() {
  char customKey = customKeypad.getKey();
  lcd.setCursor(5, 1);
  switch (customKey) {
    case '1':
      lcd.print("BOMBA     ");
      modo = 1;  //bomba
      break;
    case '2':
      lcd.print("DOMINATION");
      modo = 2;  //domination
      break;
    case '*':
      if (modo == 1) {
        setupbomba();
        while (1) modobomba();
        break;
      } else if (modo == 2) {
        lcd.clear();
        domination();
        break;
      }
  }
}

void domination() {
  int dominio = 0;
  uint equipeAPontos = 0;
  uint equipeBPontos = 0;
  uint tempo = 0;
  bool fim = false;
  char pontos[4];
  int limitePontos = 0;

  char key = customKeypad.getKey();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Limite pontos:");
  while (key != '*') {
    lcd.setCursor(currentLength, 1);
    lcd.cursor();
    key = customKeypad.getKey();
    key == NO_KEY;
    if (key != NO_KEY) {
      if ((key >= '0') && (key <= '9') && currentLength < 4) {
        lcd.print(key);
        pontos[currentLength] = key;
        currentLength++;
      }
    }
  }
  limitePontos = atoi(pontos);

  while (equipeAPontos < limitePontos && equipeBPontos < limitePontos) {
    if (dominio == 1) equipeAPontos++;
    else if (dominio == 2) equipeBPontos++;
    lcd.clear();
    lcd.noCursor();
    lcd.print("Equipa A: ");
    lcd.print(equipeAPontos);
    lcd.setCursor(0, 1);
    lcd.print("Equipa B: ");
    lcd.print(equipeBPontos);
    delay(900);
    int bota = digitalRead(botaoamarelo);
    int botv = digitalRead(botaovermelho);
    if (bota == LOW) {
      while (bota == LOW && tempo < 10 && dominio != 1) {
        bota = digitalRead(botaoamarelo);
        lcd.clear();
        lcd.print("A dominar eq. A");
        lcd.setCursor(0, 1);
        lcd.print(tempo);
        tempo++;
        delay(980);
        if (tempo >= 10) {
          dominio = 1;
          tone(13, 4000, 100);
          lcd.clear();
          lcd.print("Dominio eq. A!");
        }
      }
      tempo = 0;
    } else if (botv == LOW) {
      while (botv == LOW && tempo < 10 && dominio != 2) {
        botv = digitalRead(botaovermelho);
        lcd.clear();
        lcd.print("A dominar eq. B");
        lcd.setCursor(0, 1);
        lcd.print(tempo);
        tempo++;
        delay(980);
        if (tempo >= 10) {
          dominio = 2;
          tone(13, 4000, 100);
          lcd.clear();
          lcd.print("Dominio eq. B!");
        }
      }
      tempo = 0;
    }
  }

  if (equipeAPontos >= limitePontos) {
    lcd.clear();
    lcd.print("Equipa A!!");
    fim = true;
  } else {
    lcd.clear();
    lcd.print("Equipa B!!");
    fim = true;
  }
  int contad = 0;
  while (contad != 20) {
    contad += 1;
    digitalWrite(buzzer, HIGH);
    digitalWrite(ledvermelho, HIGH);  // sets the LED on
    delay(200);
    digitalWrite(ledvermelho, LOW);  // sets the LED off
    delay(200);
    digitalWrite(ledamarelo, HIGH);  // sets the LED on
    delay(200);
    digitalWrite(ledamarelo, LOW);  // sets the LED off
    delay(200);
    digitalWrite(ledverde, HIGH);  // sets the LED on
    delay(200);
    digitalWrite(ledverde, LOW);  // sets the LED off
    delay(200);
  }
  //        char customKey = customKeypad.getKey();
  //        if(customKey) {digitalWrite(buzzer, LOW);resetFunc();}
  resetFunc();  //call reset
}

void setupbomba() {
  lcd.setCursor(0, 0);
  lcd.print("Timer: HH:MM:SS");
  lcd.setCursor(0, 1);
  lcd.print("SET:   :  :");
  keycount = 5;

  while (keycount == 5) {
    char hourstens = customKeypad.getKey();
    lcd.setCursor(5, 1);
    lcd.blink();
    if (hourstens >= '0' && hourstens <= '9') {
      hourstenscode = hourstens - '0';
      lcd.print(hourstens);
      keycount++;
    }
  }

  while (keycount == 6) {
    char hoursones = customKeypad.getKey();
    lcd.setCursor(6, 1);
    lcd.blink();
    if (hoursones >= '0' && hoursones <= '9') {
      hoursonescode = hoursones - '0';
      lcd.print(hoursones);
      keycount++;
    }
  }

  while (keycount == 7) {
    char mintens = customKeypad.getKey();
    lcd.setCursor(8, 1);
    lcd.blink();
    if (mintens >= '0' && mintens <= '9') {
      mintenscode = mintens - '0';
      lcd.print(mintens);
      keycount++;
    }
  }

  while (keycount == 8) {
    char minones = customKeypad.getKey();
    lcd.setCursor(9, 1);
    lcd.blink();
    if (minones >= '0' && minones <= '9') {
      minonescode = minones - '0';
      lcd.print(minones);
      keycount++;
    }
  }

  while (keycount == 9) {
    char sectens = customKeypad.getKey();
    lcd.setCursor(11, 1);
    lcd.blink();
    if (sectens >= '0' && sectens <= '9') {
      sectenscode = sectens - '0';
      lcd.print(sectens);
      keycount = 10;
    }
  }

  while (keycount == 10) {
    char secones = customKeypad.getKey();
    lcd.setCursor(12, 1);
    lcd.blink();
    if (secones >= '0' && secones <= '9') {
      seconescode = secones - '0';
      lcd.print(secones);
      keycount = 11;
    }
  }

  if (keycount == 11)
    ;
  {
    Hcount = (hourstenscode * 10) + hoursonescode;
    Mcount = (mintenscode * 10) + minonescode;
    Scount = (sectenscode * 10) + seconescode;
    delay(100);
    lcd.noBlink();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Tempo Bomba:");
    if (Hcount >= 10) {
      lcd.setCursor(0, 1);
      lcd.print(Hcount);
    }
    if (Hcount < 10) {
      lcd.setCursor(0, 1);
      lcd.write("0");
      lcd.setCursor(1, 1);
      lcd.print(hoursonescode);
    }
    lcd.print(":");

    if (Mcount >= 10) {
      lcd.setCursor(3, 1);
      lcd.print(Mcount);
    }
    if (Mcount < 10) {
      lcd.setCursor(3, 1);
      lcd.write("0");
      lcd.setCursor(4, 1);
      lcd.print(minonescode);
    }
    lcd.print(":");

    if (Scount >= 10) {
      lcd.setCursor(6, 1);
      lcd.print(Scount);
    }

    if (Scount < 10) {
      lcd.setCursor(6, 1);
      lcd.write("0");
      lcd.setCursor(7, 1);
      lcd.print(seconescode);
    }
    delay(3000);
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("PIN Bomba: ");
  while (currentLength < 4) {
    lcd.setCursor(currentLength + 6, 1);
    lcd.cursor();
    char key = customKeypad.getKey();
    key == NO_KEY;
    if (key != NO_KEY) {
      if ((key != '*') && (key != '#')) {
        lcd.print(key);
        password[currentLength] = key;
        currentLength++;
      }
    }
  }

  if (currentLength == 4) {
    delay(500);
    lcd.noCursor();
    lcd.clear();
    lcd.home();
    lcd.print("Codigo: ");
    lcd.setCursor(6, 1);
    lcd.print(password[0]);
    lcd.print(password[1]);
    lcd.print(password[2]);
    lcd.print(password[3]);

    delay(3000);
    lcd.clear();
    currentLength = 0;
  }
}

void modobomba() {
  TIMSK1 |= (1 << OCIE1A);
  while (timeout != true) {
    char key2 = customKeypad.getKey();  // get the key
    if (key2 == '*') {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Code: ");

      while (currentLength < 4 && timeout != true) {
        char key2 = customKeypad.getKey();
        if (key2 != NO_KEY) {

          lcd.setCursor(currentLength + 7, 0);
          lcd.cursor();

          lcd.print(key2);
          entered[currentLength] = key2;
          currentLength++;
          delay(10);
          lcd.noCursor();
          lcd.setCursor(currentLength + 6, 0);
          lcd.print("*");
          lcd.setCursor(currentLength + 7, 0);
          lcd.cursor();
        }
      }

      if (currentLength == 4) {
        if (entered[0] == password[0] && entered[1] == password[1] && entered[2] == password[2] && entered[3] == password[3]) {
          TIMSK1 &= ~(1 << OCIE1A);
          lcd.noCursor();
          lcd.clear();
          lcd.setCursor(2, 0);
          lcd.print("Bomb Defused");
          tone(buzzer, 4000, 200);
          digitalWrite(ledverde, HIGH);
          delay(500);
          tone(buzzer, 4000, 200);
          digitalWrite(ledverde, HIGH);
          delay(500);
          tone(buzzer, 4000, 200);
          digitalWrite(ledverde, HIGH);
          currentLength = 0;
          delay(2500);
          lcd.setCursor(1, 1);
          lcd.print("Premir 1 tecla");
          while (1) {
            char customKey = customKeypad.getKey();
            if (customKey) {
              digitalWrite(buzzer, LOW);
              resetFunc();
            }
          }
        }
      } else {
        lcd.noCursor();
        lcd.clear();
        lcd.home();
        lcd.print("PIN ERRADO!");
      }
    }
    if (timeout != true) {
      // Exibe o tempo no LCD
      lcd.setCursor(0, 0);
      lcd.print("Tempo Restante: ");
      lcd.setCursor(0, 1);
      if (Hcount < 10) {
        lcd.print("0");
      }
      lcd.print(Hcount);
      lcd.print(":");

      lcd.setCursor(3, 1);
      if (Mcount < 10) {
        lcd.print("0");
      }
      lcd.print(Mcount);
      lcd.print(":");

      lcd.setCursor(6, 1);
      if (Scount < 10) {
        lcd.print("0");
      }
      lcd.print(Scount);
    }
  }
  char customKey = customKeypad.getKey();
  while (customKey != '*') {
    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("BOOOOOOM");
    lcd.setCursor(2, 1);
    lcd.print("! ! ! ! ! ! !");
    digitalWrite(buzzer, HIGH);
    // tone(buzzer, 5000,15000);
    digitalWrite(ledvermelho, HIGH);  // sets the LED on
    delay(200);
    digitalWrite(ledvermelho, LOW);  // sets the LED off
    delay(200);
    digitalWrite(ledamarelo, HIGH);  // sets the LED on
    delay(200);
    digitalWrite(ledamarelo, LOW);  // sets the LED off
    delay(200);
    digitalWrite(ledverde, HIGH);  // sets the LED on
    delay(200);
    digitalWrite(ledverde, LOW);  // sets the LED off
    delay(200);
    customKey = customKeypad.getKey();
    if (customKey) {
      digitalWrite(buzzer, LOW);
      resetFunc();
      TIMSK1 &= ~(1 << OCIE1A);
    }  //call reset
  }
}

ISR(TIMER1_COMPA_vect) {
  if (Scount > 0) {
    Scount--;  // Diminui os segundos se for maior que zero
  } else {
    Scount = 59;  // Reinicia os segundos para 59
    if (Mcount > 0) {
      Mcount--;  // Diminui os minutos se os segundos chegarem a zero
    } else {
      Mcount = 59;  // Reinicia os minutos para 59
      if (Hcount > 0) {
        Hcount--;  // Diminui as horas se os minutos chegarem a zero
      } else {
        timeout = true;
      }
    }
  }
}