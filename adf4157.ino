bool debug = false;

const double tRFout[]     =  { 5800000000.0, 796000000.0, 1296800000.0, 1296785000.0,  2592196250.0,  2592196150.0 };
const uint32_t tR0[]      =  {            0,           0,            0,            0,    0x3040CE08,    0x3040CE08 };
const uint32_t tR1[]      =  {            0,           0,            0,            0,     0xCAC0001,     0xC580001 };
const uint32_t tR2[]      =  {            0,           0,            0,            0,     0xD108002,     0xD108002 };
const uint32_t tR3[]      =  {            0,           0,            0,            0,          0x43,          0x43 };
const uint32_t tR4[]      =  {            0,           0,            0,            0,           0x4,           0x4 };
const uint  tREFin[]      =  {     10000000,    10000000,     10000000,     10000000,      10000000,      10000000 };
const uint tRefDouble[]   =  {            0,           0,            0,            0,             1,             1 };
const uint tRCounter[]    =  {            1,           1,            1,            1,             1,             1 };
const uint tRefDivBy2[]   =  {            0,           0,            0,            0,             0,             0 };
const uint tMuxout[]      =  {            0,           0,            0,            0,             6,             6 };
const uint tChargePump[]  =  {            0,           0,            0,            0,            13,            13 };
const bool tPD[]          =  {            0,           0,            0,            0,             0,             0 };

void adf4157(
  double RFout, 
  unsigned int REFin,
  unsigned int RefDouble,
  unsigned int RCounter,
  unsigned int RefDivBy2,
  unsigned int Muxout,
  bool PD,
  unsigned int ChargePump
  ) {

  // values hardset
  const unsigned int ClckDivVal = 0; // 0 - 4095
  const bool ClckDivMode = 0; // 0 (Off) or 1 (On)
  const unsigned int NegBleedCur = 0; // 0 (Off) or 3 (Switched R FastLock Enabled)
  const bool CounterRst = 0; // 0 (Disabled) or 1 (Enabled)
  const bool CP3St = 0; // 0 (Disabled) or 1 (Enabled)
  const bool SDRst = 0; // 0 (Enabled) or 1 (Disabled)
  const bool LDP = 0; // 0 (24 PFD Cycles) or 1 (40 PFD Cycles)
  const bool PDPol = 1; // 0 (Negative) or 1 (Positive)
  const bool CSR = 0; // 0 (Disabled) or 1 (Enabled)

  // values with automatic calculation
  bool Prescaler = 0;  if ( RFout > 3000000000 ) Prescaler = 1;
  
  double realRFout = 0.0;  // real output frequency 5802 MHz
  double N = 0.0; // integer division factor
  unsigned int INT = 0; // must be between 23 and 4095
  int FRAC = 0; // fractionality
  float fRES = 0.0; // fRES is a frequency resolution
  double fPFD = 0.0;
  int fMSB, fLSB;

  fPFD = REFin*((1.0 + RefDouble) / (RCounter * (1.0 + RefDivBy2)));
  fRES = fPFD/pow(2,25);
  N = RFout/fPFD;
  INT = (int)N;
  double fxSB = ((RFout/fPFD) - INT) * pow(2,12);
  fMSB = (int)fxSB;
  fLSB = floor(((fxSB - fMSB) * pow(2,13)));
  FRAC = fLSB + fMSB * pow(2,13);
  
  if (debug) {
    Serial.print("REFin: ");  Serial.print(REFin);  Serial.println(" Hz");
    Serial.print("fPFD: ");  Serial.print(bigPrint(fPFD));  Serial.println(" Hz");
    Serial.print("fRES: ");  Serial.print(fRES,10);  Serial.println(" Hz");
    Serial.print("N: ");  Serial.println(N,20);
    Serial.print("INT: ");  Serial.println(INT);
    Serial.print("fMSB: ");  Serial.println(fMSB);
    Serial.print("fLSB: ");  Serial.println(fLSB);
    Serial.print("FRAC: ");  Serial.println(FRAC);
  }

  uint32_t R0,R1,R2,R3,R4;
  R0 = (0x0<<31)|((Muxout)<<27)|((INT)<<15)|((fMSB)<<3)|(0<<0); // Done
  R1 = (0x0<<31)|(0x0<<30)|(0x0<<29)|(0x0<<28)|((fLSB)<<15)|(1<<0); // Done
  R2 = (0x0<<31)|((CSR)<<28)|((ChargePump)<<24)|((Prescaler)<<22)|((RefDivBy2)<<21)|((RefDouble)<<20)|((RCounter)<<15)|(2<<0); // Done
  R3 = (0x0<<31)|((SDRst)<<14)|((LDP)<<7)|((PDPol)<<6)|((PD)<<5)|((CP3St)<<4)|((CounterRst)<<3)|(3<<0); // Done
  R4 = (0x0<<31)|((NegBleedCur)<<23)|((ClckDivMode)<<19)|((ClckDivVal)<<7)|(4<<0); // Done

  Serial.print("Freq: ");  Serial.print(bigPrint(RFout));  Serial.print("Hz ");
  double realFreq = fPFD*(INT + (FRAC/pow(2,25)));
  Serial.print(bigPrint(realFreq)); Serial.print("Hz ");
  Serial.print(bigPrint(RFout-realFreq)); Serial.println("Hz ");
  Serial.print("Registers: "); Serial.print(R0, HEX); Serial.print(" ");
  Serial.print(R1, HEX); Serial.print(" "); Serial.print(R2, HEX); Serial.print(" ");
  Serial.print(R3, HEX); Serial.print(" "); Serial.println(R4, HEX);
}

String bigPrint(double n) {
  char buff[64];
  sprintf(buff, "%.*f", 5, n);
  return buff;
}

void setup() {
  Serial.begin(9600);
  delay(2000);
  Serial.println("===============================");

  for (int i=0; i<sizeof tRFout/sizeof tRFout[0]; i++) {
    adf4157(tRFout[i], tREFin[i], tRefDouble[i], tRCounter[i], tRefDivBy2[i], tMuxout[i], tPD[i], tChargePump[i]);
    Serial.print("Expected: "); Serial.print(tR0[i], HEX); Serial.print(" ");
    Serial.print(tR1[i], HEX); Serial.print(" "); Serial.print(tR2[i], HEX); Serial.print(" ");
    Serial.print(tR3[i], HEX); Serial.print(" "); Serial.println(tR4[i], HEX);
  }
}

void loop() {
}
