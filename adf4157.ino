const double REFin = 10000000.0; // Hz - 10 MHz
// double RFout = 5911000400.0;  // output frequency 5802 MHz
 double RFout = 826123456.2;  // output frequency 1296 MHz
// double RFout = 2401000000.0;  // output frequency 2400 MHz
double N = 0.0; // integer division factor
int INT = 0; // must be between 23 and 4095
int FRAC = 0; // fractionality
int RefDouble = 0; // def: 0 - NO - RF REFin doubler bit
int RCounter = 2; // def: 1 - RF REFin reference division factor (1-32)
int RefDivBy2 = 0; // def: 0 - NO - reference divide by 2-bit (0 or 1)
float fRES = 0.0; // fRES is a frequency resolution
double fPFD = 0.0;
int fMSB, fLSB;

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
  Serial.println(N,20);
  INT = floor(N);
  Serial.print("INT: ");
  Serial.println(INT);

  double fxSB = ((RFout/fPFD) - INT) * pow(2,12);

  fMSB = round(fxSB);
  Serial.print("fMSB: ");
  Serial.println(fMSB);
  
  fLSB = round(((fxSB - fMSB) * pow(2,13)));
  Serial.print("fLSB: ");
  Serial.println(fLSB);

  FRAC = fLSB + fMSB * pow(2,13);
  Serial.print("FRAC: ");
  Serial.println(FRAC);

  Serial.print("Real RFout: ");
  Serial.println(bigPrint(fPFD*(INT + (FRAC/pow(2,25)))));

  uint32_t R0,R1,R2,R3,R4;
  //             _____________________________ - MUXOUT to be made dynamic not static  
  R0 = (0x0<<31)|(0x0<<30)|(0x0<<29)|(0x0<<28)|((INT)<<15)|((fMSB)<<3)|(0x0<<2)|(0x0<<1)|(0x0<<0);

  R1 = (0x0<<31)|(0x0<<30)|(0x0<<29)|(0x0<<28)|((fLSB)<<15)|(0x0<<2)|(0x0<<1)|(0x1<<0);

  Serial.print("R0: ");
  Serial.println(R0, HEX);
  Serial.print("R1: ");
  Serial.println(R1, HEX);
  Serial.println(R1, BIN);
  Serial.println("00000010101000001000000000000001");
  // 1228CC8, 5811MHz
  Serial.println("R^ M^^   INT    ^^   FRAC   ^^C^");
}

void loop() {
}
