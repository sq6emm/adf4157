const double REFin = 10000000.0; // Hz - 10 MHz
double RFout = 5802000000.0;  // output frequency 5802 MHz
// double RFout = 1296804600.0;  // output frequency 1296 MHz
// double RFout = 5900001000.0;  // output frequency 5802 MHz
double N = 0; // integer division factor
int FRAC = 0; // fractionality
int RefDouble = 0; // def: 0 - NO - RF REFin doubler bit
int RCounter = 1; // def: 1 - RF REFin reference division factor (1-32)
int RefDivBy2 = 0; // def: 0 - NO - reference divide by 2-bit (0 or 1)
float fRES = 0.0; // fRES is a frequency resolution
double fPFD = 0.0;
unsigned int fMSB, fLSB;

String bigPrint(double n) {
  char buff[64];
  sprintf(buff, "%.*f", 5, n);
  return buff;
}

void setup() {
  Serial.begin(9600);
  delay(3000);
  Serial.println("===============================");
  Serial.print("RFout: ");
  Serial.print(bigPrint(RFout));
  Serial.println(" Hz");
  Serial.print("REFin: ");
  Serial.print(REFin);
  Serial.println(" Hz");
  fPFD = REFin*((1.0 + RefDouble) / (RCounter * (1.0 + RefDivBy2)));
  Serial.print("fPFD: ");
  Serial.print(bigPrint(fPFD));
  Serial.println(" Hz");
  fRES = fPFD/pow(2,25);
  Serial.print("fRES: ");
  Serial.print(fRES,10);
  Serial.println(" Hz");
  N = RFout/fPFD;
  Serial.print("N: ");
  Serial.println(bigPrint(N));

  int fMSB = round(((RFout/fPFD) - int(N)) * pow(2,12));
  Serial.print("fMSB: ");
  Serial.println(fMSB);
  
  int fLSB = round(((((RFout/fPFD) - int(N)) * pow(2,12)) - fMSB) * pow(2,13));
  Serial.print("fLSB: ");
  Serial.println(fLSB);

  FRAC = fLSB + fMSB * pow(2,13);
  Serial.print("FRAC: ");
  Serial.println(FRAC);

  Serial.print("Real RFout: ");
  Serial.println(bigPrint(fPFD*(int(N) + (FRAC/pow(2,25)))));
}

void loop() {
}