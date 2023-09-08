/*
  Just Another Morse Code Project by a guy who
  doesn't really understand C++ or Arduino architecture.

  Enjoy!
*/

// Setup defintions
static const int ledPin = 10;
static const int initialTimeUnit = 60;

// This code was originally built and run on an Arduino Uno.
// A 156 char limit might be conservative but I thought I'd
// rather be safer than sorrier?
//
// Change it! Caveat emptor!
static const int phraseSizeLimit = 156;

static const bool debugMode = true;
static const bool testMode = false;

struct morseCodes {
  private:
  /*
    I don't know a whole lot about morse code and the various timings and
    calculations. There's, like a standard word-per-minutes thing and
    there's something about Farnsworth Timing and whatnot.

    No, we're not doing Farnsworth Timing. Just because.

    Anyway, go learn about it: https://morsecode.world/international/timing.html
  */

  // Base units in miliseconds. Change timeUnit to alter all the timings.
  int timeUnit = initialTimeUnit;
  int dit = timeUnit; // dot
  int dah = timeUnit * 3; // dash
  int intra = timeUnit; // gap between dits and dahs within a character
  int inter = (timeUnit * 3); // gap between characters of a word
  int wordDelay = (timeUnit * 7); // gap between words

  /*
    One tragic thing about noodling on the Arduino Uno and C++ was to
    find that hashmaps/dictionaries/key-value data structure types
    were just not easily available to me.

    I ended up using a multidimensional array.

    Each row started with the a char type that is converted to its
    int representation (ie. 'A' = 65).

    Then it's the length of the morse code, as in: how many dits and dahs.

    After that it's the morse code dits and dahs for each letter.

    [
      int that corresponds to the char,
      int of the dit-dah morse code length,
      ints of the dits and dahs,
      ...
    ]
  */
  int codes[53][10] = {
    // Alphabet (26)
    {'A', 2, dit, dah}, // (Assuming timeUnit = 60) dit = 60, intra = 60, dah 180. So, flashing an "a" should total 300.
    {'B', 4, dah, dit, dit, dit},
    {'C', 4, dah, dit, dah, dit},
    {'D', 3, dah, dit, dit},
    {'E', 1, dit},
    {'F', 4, dit, dit, dah, dit},
    {'G', 3, dah, dah, dit},
    {'H', 4, dit, dit, dit, dit},
    {'I', 2, dit, dit},
    {'J', 4, dit, dah, dah, dah},
    {'K', 3, dah, dit, dah},
    {'L', 4, dit, dah, dit, dit},
    {'M', 2, dah, dah},
    {'N', 2, dah, dit},
    {'O', 3, dah, dah, dah},
    {'P', 4, dit, dah, dah, dit},
    {'Q', 4, dah, dah, dit, dah},
    {'R', 3, dit, dah, dit},
    {'S', 3, dit, dit, dit},
    {'T', 1, dah},
    {'U', 3, dit, dit, dah},
    {'V', 4, dit, dit, dit, dah},
    {'W', 3, dit, dah, dah},
    {'X', 4, dah, dit, dit, dah},
    {'Y', 4, dah, dit, dah, dah},
    {'Z', 4, dah, dah, dit, dit},

    // Numerals (10)
    {'0', 5, dah, dah, dah, dah, dah},
    {'1', 5, dit, dah, dah, dah, dah},
    {'2', 5, dit, dit, dah, dah, dah},
    {'3', 5, dit, dit, dit, dah, dah},
    {'4', 5, dit, dit, dit, dit, dah},
    {'5', 5, dit, dit, dit, dit, dit},
    {'6', 5, dah, dit, dit, dit, dit},
    {'7', 5, dah, dah, dit, dit, dit},
    {'8', 5, dah, dah, dah, dit, dit},
    {'9', 5, dah, dah, dah, dah, dit},

    // Space (1)
    {' ', 1, wordDelay},

    // Punctuation (15)
    {'&', 5, dit, dah, dit, dit, dit},
    {'\'', 6, dit, dah, dah, dah, dah, dit},
    {'@', 6, dit, dah, dah, dit, dah, dit},
    {')', 6, dah, dit, dah, dah, dit, dah},
    {'(', 5, dah, dit, dah, dah, dit},
    {':', 6, dah, dah, dah, dit, dit, dit},
    {',', 6, dah, dah, dit, dit, dah, dah},
    {'=', 5, dah, dit, dit, dit, dah},
    {'!', 6, dah, dit, dah, dit, dah, dah},
    {'.', 6, dit, dah, dit, dah, dit, dah},
    {'-', 6, dah, dit, dit, dit, dit, dah},
    /*
      {'%', 15, dah, dah, dah, dah, dah, dah, dit, dit, dah, dit, dah, dah, dah, dah, dah},

      Now this percent sign is interesting. I think this huge
      array overloads the Uno's limited RAM. I suppose multidimensional
      arrays are pretty large memory hogs.

      Because I'm specifying a short int (-32768 to 32767) I think
      the calculation is as follows:

      ex. 56 * 17 * 2 = 1904KBytes

      1904KBytes for 2KBytes of SRAM. That's pretty close and declaring this
      multidimensional array with this row borks everything. So we gotta
      treat this as a special case.
    */
    {'+', 5, dit, dah, dit, dah, dit},
    {'"', 6, dit, dah, dit, dit, dah, dit},
    {'?', 6, dit, dit, dah, dah, dit, dit},
    {'/', 5, dah, dit, dit, dah, dit},

    // Error (1)
    // {-1, 8, dit, dit, dit, dit, dit, dit, dit, dit}
  };

  int error[10] = {-1, 8, dit, dit, dit, dit, dit, dit, dit, dit};

  public:
  // GETTERS START
  int getTimeUnit() {
    return timeUnit;
  }

  int getIntra() {
    return intra;
  }

  int getInter() {
    return inter;
  }
  // GETTERS END

  int* findGlyphArrPtr(char glyph) {
    if(!isAscii(glyph)) {
      return error;
    }

    int index = 0;
    int rows = sizeof codes / sizeof codes[0];

    // This is really an array bounds check to see if we're looking for a
    // character that isn't defined in the letterToMorse array (ie. Ø).
    // Otherwise we'd keep iterating into undefined behavior territory
    // and that's chaos.
    while( (index < rows) && (glyph != codes[index][0]) ) {
      index++;
    }

    bool charNotFound = glyph != codes[index][0];

    if(charNotFound) {
      return error;
    }

    return codes[index];
  }
};

struct morseBlinker {
  private:
  int ledPin;

  String phrase;
  int phraseLength;

  int intra;
  int inter;

  /*
    Prosigns are combinations of 2 or 3 letters
    sent together with no space inbetween.

    ie. no inter spaces

    In this program prosigns are indicated with
    angle brackets: < >

    https://morsecode.world/international/morse.html
  */
  bool isProsign = false;

  long totalDelay = 0;

  bool isTesting = false;
  bool isDebug = false;

  // Meat and potatoes START
  void blinkPhrase() {
    for(int i = 0; i < phraseLength; i++) {
      char glyph = phrase[i];

      if(glyph == '%') {
        blinkPercent();
      } else {
        blinkGlyph(glyph);
        printDebug(0);
      }

      if(shouldBlinkInter(i, glyph)) {
        blinkInter();
      }
    };
  }

  void blinkPercent() {
    blinkGlyph('0', '%', false, true);
    blinkGlyph('/', '\0', true, true);
    blinkGlyph('0', '\0', true);
    printDebug(0);
  }

  // All the crazy params after char glyph came about b/c of the nuttiness of working around '%'. Le sign.
  void blinkGlyph(char glyph, char overrideGlyph = '\0', bool skipPrintGlyph = false, bool forceIntra = false) {
    morseCodes morseCodes;

    int* glyphArr = morseCodes.findGlyphArrPtr(glyph);

    intra = morseCodes.getIntra();
    inter = morseCodes.getInter();

    int ditDahCount = glyphArr[1];

    if(!skipPrintGlyph) {
      printDebug(overrideGlyph ? overrideGlyph : glyph);
    }

    blinkGlyphDitDahs(glyphArr, 2, ditDahCount, forceIntra);
  };

  void blinkGlyphDitDahs(int* glyphArr, int startPosition, int ditDahCount,  bool forceIntra) {
    for(int i = startPosition; (i - startPosition) < ditDahCount; i++) {
      blinkDitDah(glyphArr[i]);
      blinkIntra(i, startPosition, ditDahCount, forceIntra);
    };
  }

  void blinkDitDah(int ditDahValue) {
    printDebug(ditDahValue);
    blinkOn(ditDahValue);
  }

  bool shouldBlinkInter(int index, char glyph) {
    if(isProsign) {
      return false;
    }

    bool isLastChar = index == phraseLength - 1;
    bool isNextCharNotSpace = !isSpace(phrase[index + 1]);
    bool isAlphaNumberOrPunct = isAlphaNumeric(glyph) || isPunct(glyph);

    return !isLastChar && isAlphaNumberOrPunct && isNextCharNotSpace;
  }

  void blinkIntra(int currentPosition, int startPosition, int ditDahCount, bool forceIntra) {
    if( ((currentPosition - startPosition) < (ditDahCount - 1)) || forceIntra) {
      printDebug(intra);
      blinkOff(intra);
    }
  };

  void blinkInter() {
    printDebug(inter);
    blinkOff(inter);
    printDebug(0);
  }

  void blinkOn(int delayLength) {
    totalDelay += delayLength;
    digitalWrite(ledPin, LOW);

    if(isTesting) { return; };

    digitalWrite(ledPin, HIGH);
    delay(delayLength);
    digitalWrite(ledPin, LOW);
  };

  void blinkOff(int delayLength) {
    totalDelay += delayLength;
    digitalWrite(ledPin, LOW);

    if(isTesting) { return; };

    delay(delayLength);
  }

  void printDebug(long value, String callingFunc = __builtin_FUNCTION()) {
    if(!isDebug) { return; }

    if(value == 0) {
      Serial.println();
    } else if(callingFunc == "blinkGlyph") {
      Serial.print(char(value));
      Serial.print(F(": "));
    } else if(callingFunc == "blinkDitDah") {
      Serial.print(value);
    } else if(callingFunc == "blinkInter") {
      Serial.print(F("|: ["));
      Serial.print(value);
      Serial.print(F(" - inter space]"));
    } else if(callingFunc == "blinkIntra") {
      Serial.print(F(" - ["));
      Serial.print(value);
      Serial.print(F("] - "));
    } else if(callingFunc == "blink") {
      Serial.println(F("==========="));
      Serial.print(F("Total time for phrase: "));
      Serial.print(value);
    }
  }
  // Meat and potatoes END

  void setIsProsign() {
    bool isProsignPhrase = (phraseLength <= 5) && (phrase[0] == '<') && (phrase[phraseLength - 1] == '>');

    if(isProsignPhrase) {
      isProsign = true;
      phrase.remove(phraseLength - 1, 1);
      phrase.remove(0, 1);

      phraseLength = phrase.length();
    }
  }

  public:
  // SETTERS START
  void setPhrase(String s) {
    s.toUpperCase();
    phrase = s;
    phraseLength = s.length();

    setIsProsign();
  }

  void setLedPin(int pin) {
    ledPin = pin;
    digitalWrite(ledPin, LOW);
  }

  void setIsTesting(bool testMode) { isTesting = testMode; }
  void setIsDebug(bool debugMode) { isDebug = debugMode; }
  // SETTERS END

  // GETTERS START
  String getPhrase() { return phrase; }
  bool getIsTesting() { return isTesting; }
  bool getIsDebug() { return isDebug; }
  // GETTERS END

  int blink() {
    totalDelay = 0;

    if(phrase.length() > phraseSizeLimit) {
      Serial.println(F(""));
      Serial.print(F("The entered phrase was too long. Phrase length: "));
      Serial.println(phrase.length());

      Serial.print("Phrase limit is: ");
      Serial.println(phraseSizeLimit);
      Serial.println(F(""));

      return 0;
    }

    blinkPhrase();

    printDebug(totalDelay);

    return totalDelay;
  }
};

void spacer(int spaces = 5) {
  for(int i = 0; i < spaces; i++) {
    Serial.println(F(""));
  }
}

morseBlinker setupBlinker(String phrase = "test") {
  morseBlinker blinker;

  blinker.setLedPin(ledPin);
  blinker.setIsDebug(debugMode);
  blinker.setIsTesting(testMode);
  blinker.setPhrase(phrase);

  return blinker;
}

int runMorseBlinker() {
  Serial.println(F("Hi! What're we morse blinking?"));

  while (Serial.available() == 0) {}

  String phrase = Serial.readString();

  return setupBlinker(phrase).blink();
}

/*
  ==================================================================
  BEGIN TEST CODE
  ==================================================================

  With all the weird timings and not being able to read morse code at all
  and trusting myself and my abilities very little I thought that writing
  some tests would go a long way in helping keep my sanity limping along.
*/

struct assertions {
  bool equal(int a, int b, String desc) {
    bool eval = a == b;
    String result = eval ? F("PASS") : F("FAIL");
    String output = String(result + F(" --- ") + desc + F(": ") + a + F(" (actual)") + (eval ? F(" == ") : F(" != ")) + b + F(" (expected)"));

    Serial.println(output);

    return eval;
  }
};

void assertEqual(String testPhrase, int expected, String desc) {
  assertions assertions;
  morseBlinker subject = setupBlinker(testPhrase);

  if(debugMode) {
    Serial.println(F("---------------------------"));
    Serial.print(F("TESTING: "));
    Serial.println(testPhrase);
    spacer(1);

    int actual = subject.blink();
    spacer(2);

    assertions.equal(actual, expected, desc);

    Serial.println(F("---------------------------"));
    spacer(2);
  } else {
    int actual = subject.blink();

    assertions.equal(actual, expected, desc);
  }
}

void tests() {
  spacer(1);

  char testInput[phraseSizeLimit + 1];
  for(int i = 0; (phraseSizeLimit + 1) > i; i++) {
    testInput[i] = 'A';
  }

  assertEqual("a",          300, F("letter 'a' should be 300ms")); // 60 + 60 (intra delay) + 180 = 300ms
  assertEqual("aa a",      1500, F("phrase 'aa a' should be 1500ms")); // a + 180 (inter delay) + a + 420 (wordDelay) + a = 1500ms
  assertEqual(".",         1020, F("punctuation '.' should be 1020ms"));
  assertEqual(".. ! ., @", 7980, F("punctuation '.. ! ., @' should be 7980ms"));
  assertEqual("Paris ",    3000, F("word and trailing space 'Paris ' should be 3000ms"));
  assertEqual("&",          660, F("glyph '&' should be 660ms"));
  assertEqual("%",         3180, F("glyph '%' should be 3180ms"));
  assertEqual("",             0, F("An empty string should be 0ms"));
  assertEqual("#",          900, F("a single non Ascii character should return an error equaling 900ms"));
  assertEqual("<AA>",       600, F("'<AA>' prosign should not tally inter spaces 600ms"));
  assertEqual(testInput,      0, F("Should not run the blinker code if the input is over the phraseSizeLimit"));
}

void runTests() {
  Serial.println(F("Type a thing and press 'enter' to run tests"));

  while (Serial.available() == 0) {}

  Serial.read();

  tests();
}

/*
  ==================================================================
  END TEST CODE
  ==================================================================
*/


/*
  ==================================================================
  Meat and potatoes! If you want to make changes to the code and then
  verify your changes aren't mucking anything up then you'll have to
  uncomment testMorseCoder() in the look() method and uncomment all
  the test code!
  ==================================================================
*/

void setup() {
  pinMode(ledPin, OUTPUT);

  Serial.begin(115200);
  Serial.println(F("BEGIN (listening on 115200) ====================="));
}

void loop() {
  spacer();

  if(testMode) {
    runTests();
  } else {
    runMorseBlinker();
  }
}