#include <adns2610.h>

class DiffDrive
{
  int _AA, _AB, _BB, _BA;
  
  public:
    DiffDrive(int AA, int AB, int BA, int BB)
    {
      _AA = AA;
      _BB = BB;
      _AB = AB;
      _BA = BA;
    }
    
    void init()
    {
      pinMode(_AA, OUTPUT);
      pinMode(_AB, OUTPUT);
      pinMode(_BA, OUTPUT);
      pinMode(_BB, OUTPUT);
    }
    
    void setSpeed(int a, int b)
    {
      byte motorA = (unsigned byte)a;
      byte motorB = (unsigned byte)b;
      analogWrite(_AA, 255 - (motorA << 1));
      digitalWrite(_AB, a >= 0 ? HIGH : LOW);
      analogWrite(_BA, (motorB << 1));
      digitalWrite(_BB, b >= 0 ? LOW : HIGH);
    }
};

class Motion
{
  ADNS2610 _left, _right;
  unsigned long _timestamp;
  
  float _x, _y;
  float _vx, _vy;
  
  public:
    Motion(int LSCK, int LDATA, int RSCK, int RDATA)
      : _left(LSCK, LDATA), _right(RSCK, RDATA)
    {
    }
    
    void init()
    {
      _left.init();
      _right.init();
    }
    
    void measure()
    {
    }
    
    void diagnostics()
    {
        byte Ldx, Ldy;
        byte Lsqual, Lmin, Lmax;
        byte Rdx, Rdy;
        byte Rsqual, Rmin, Rmax;
     
        Ldx = _left.dx();
        Rdx = _right.dx();
        Ldx = _left.dy();
        Rdx = _right.dy();
        Lsqual = _left.squal();
        Rsqual = _right.squal();
        Lmin = _left.minimum_pixel();
        Rmin = _right.minimum_pixel();
        Lmax = _left.maximum_pixel();
        Rmax = _right.maximum_pixel();
        
        Serial.print(Ldx, DEC); Serial.print(" ");
        Serial.print(Ldy, DEC); Serial.print(" ");
        Serial.print(Rdx, DEC); Serial.print(" ");
        Serial.print(Rdy, DEC); Serial.print(" ");
        Serial.print(Lsqual, DEC); Serial.print(" ");
        Serial.print(Rsqual, DEC); Serial.print(" ");
        Serial.print(Lmin, DEC); Serial.print(" ");
        Serial.print(Lmax, DEC); Serial.print(" ");
        Serial.print(Rmin, DEC); Serial.print(" ");
        Serial.print(Rmax, DEC); Serial.print(" ");
         Serial.println();
    }
};

DiffDrive drive(10, 12, 11, 13);
Motion motion(2,3, 4,5);


void setup() {
  // put your setup code here, to run once:

  Serial.begin(115600);
  drive.init();
  motion.init();
}

void loop() {
  for (int i = 0; i < 32; i++)
  {
    drive.setSpeed(i, i);
    delay(500);
    motion.diagnostics();
  }
  delay(1000);
  for (int i = 0; i < 32; i++)
  {
    drive.setSpeed(-i, -i);
    delay(500);
    motion.diagnostics();
  }
  delay(1000);
}
