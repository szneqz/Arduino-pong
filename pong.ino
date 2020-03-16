//tworzone z pomoca https://learn.adafruit.com/adafruit-gfx-graphics-library?view=all
//Dziekuje tworcom za stworzenie bibliotek oraz poradnika

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);  //ustalenie pinow
                                                             // pin 7 - Serial clock out (SCLK)
                                                             // pin 6 - Serial data out (DIN)
                                                             // pin 5 - Data/Command select (D/C)
                                                             // pin 4 - LCD chip select (CS)
                                                             // pin 3 - LCD reset (RST)


int ga1;    //lewa paletka
int ga2;    //prawa paletka
int popga1;    //ruch poprzedni paletki (pomocny przy rozpoznaaniu rozpoczecia gry
int popga2;    //^^^^^
int pilkaa = 40;  //ustawienie pilki na srodku na osi x
int pilkab = 22;  //ustawienie pilki na srodku na osi y
int kier = 0; //0 - bezruch, 1 - w prawo, 2 - w lewo // kierunek pilki w poziomie
int kier2 = 0; //0 - leci prosto, 1 - w gore, 2 - w dol, 3 - mocniej w gore, 4 - mocniej w dol  // kierunek pilki w pionie
int punktyl = 0;  //punkty zdobyte przez gracza bedacego po lewej
int punktyp = 0;  //punkty zdobyte przez gracza bedacego po prawej
int wybor = 1; //zmienna w menu do wybieranai trybu pvp lub pvb
int wybrano = 0;  //zmienna, ktora przejmuje wartosc zmiennej wybor po zatwierdzeniu
int popkier;  //zmienna pomocnicza sprawdzajaca czy pilka zostala odbita by zapobiec odbijaniu pilki w nieskonczonosc
int antyafk;  //zmienna liczaca ilosc odbic prostopadlych pod rzad, takze dziala przeciwko odbijaniu pilki w nieskonczonosc
boolean tryb = 0;  //zmienna pomocnicza dzieki niej okreslane jest czy paletka podczas zapobieganiu odbijaniu pilki w nieskonczonosc ma miec okreslony kierunek (przy sciankach) czy ma go sobie wylosowac (na wolnym terenie)
int gd = 0;  //zmienna pomocnicza dzieki niej ruch paletki bota ma jeden okreslony kierunek, gdy zapobiega on odbijaniu pilki w nieskonczonosc 

void setup()
{
  Serial.begin(9600);
  while(!Serial);
  pinMode(9, OUTPUT);    //uruchamianie
  pinMode(2, OUTPUT);    //glosniczek
  pinMode(A7, INPUT);    //potencjometr nr 1
  pinMode(A8, INPUT);    //potencjometr nr 2
  pinMode(A1, INPUT);  //wejscia
  pinMode(A2, INPUT);  //klawiaturki
  pinMode(A3, INPUT);  //
  pinMode(A4, INPUT);  //
  digitalWrite(9, HIGH); //diody LED
  randomSeed(analogRead(A5));  //pin A5 odpowiedzialny jest za losowanie liczb
  display.begin();        //wlaczanie ekranu
  display.setContrast(58);  //ustawianie kontrastu (58)
  display.clearDisplay();  //czyszczenie obrazu
ga1 = analogRead(A7)/30.08 + 1;  //ustawienie paletki lewej wzgledem ustawienia potencjometru
ga2 = analogRead(A8)/30.08 + 1;  //ustawienie paletki prawej wzgledem ustawienia potencjometru
popga1 = ga1;  //ustalenie punktu odniesienia (zapobiega przedwczesnemu rozpoczeciu)
popga2 = ga2;  //^^^^
   
   while(wybrano==0)  //menu - jezeli nie ma wybranej i zatwierdzonej gry to ma sie powtarzac
   {
   display.setCursor(1, 1);
   display.setTextColor(BLACK, WHITE);
   display.setTextSize(1);
   display.print("Tryb gry:");
   display.setCursor(1, 10);
   display.print("PvP");
   display.setCursor(1, 19);
   display.print("PvB");
     if(((analogRead(A4) > 300 && analogRead(A4) < 550) || (analogRead(A3) > 300 && analogRead(A3) < 550)) && wybor == 1) //klawisz 2 w gore i klawisz 6 w dol (wybor miedzy PVP, a PVB)
     {wybor = 2;
     while((analogRead(A4) > 300 && analogRead(A4) < 550) || (analogRead(A3) > 300 && analogRead(A3) < 550));  //zapobieganie zbyt szybkiemu przelaczaniu
     delay(20);
     }
     if(((analogRead(A4) > 300 && analogRead(A4) < 550) || (analogRead(A3) > 300 && analogRead(A3) < 550)) && wybor == 2) //
     {
     wybor = 1;
     while((analogRead(A4) > 300 && analogRead(A4) < 550) || (analogRead(A3) > 300 && analogRead(A3) < 550)); //
     delay(20);
     }
   if(wybor == 1)  //podswietlanie trybu, na ktory wskazano
     {
     display.setTextColor(WHITE, BLACK);
     display.setCursor(1, 10);
     display.print("PvP");
     }
     if(wybor == 2)
     {
     display.setTextColor(WHITE, BLACK);
     display.setCursor(1, 19);
     display.print("PvB");
     }
     if(wybor == 1 && analogRead(A4) > 100 && analogRead(A4) < 300)  //zatwierdzenie wyboru
     wybrano = 1; 
     if(wybor == 2 && analogRead(A4) > 100 && analogRead(A4) < 300)
     wybrano = 2; 
     display.display();
     display.clearDisplay();
     
   }
   
}

void pileczka()
{
   display.fillRect(pilkaa, pilkab, 4, 4, BLACK); //rysowanie pilki
   if(wybrano == 1)
   {
   if(kier == 0 && ((ga1 - popga1 >= 6 || popga1 - ga1 >= 6) && (ga2 - popga2 >= 6 || popga2 - ga2 >= 6))) //jezeli pilka nie porusza sie oraz poruszono oba potencjomtery nalezy wylosowac jej kierunek
   {
     kier = random(1, 3);
   }
   }
   if(wybrano == 2)
   {
   if(kier == 0 && ((ga1 - popga1 >= 6 || popga1 - ga1 >= 6))) //jezeli pilka nie porusza sie oraz poruszono jeden potencjometr nalezy wylosowac jej kierunek
   {
     kier = random(1, 3);
   }
   }
   if(kier == 1)    //okreslenie ruchu poziomego zaleznie od kierunku
   {
     pilkaa = pilkaa + 2;
   }
      if(kier == 2)
   {
     pilkaa = pilkaa - 2;
   }
      if(kier2 == 1)  //okreslenie ruchu pionowego zaleznie od kierunku
   {
     pilkab = pilkab - 2;
   }
      if(kier2 == 2)
   {
     pilkab = pilkab + 2;
   }
         if(kier2 == 3)  
   {
     pilkab = pilkab - 3;
   }
      if(kier2 == 4)
   {
     pilkab = pilkab + 3;
   }
}

void sterowanie()
{

 ga1 = analogRead(A7)/30.08 + 1; //ustawienie paletki lewej wzgledem ustawienia potencjometru
 display.fillRect(3, ga1, 3, 12, BLACK);
 if(wybrano == 1) //jezeli pvp (czyli 1) to mozna sterowac
 ga2 = analogRead(A8)/30.08 + 1; //ustawienie paletki prawej wzgledem ustawienia potencjometru
 display.fillRect(78, ga2, 3, 12, BLACK);
}

void czyprzegralem()
{
  if(pilkaa <= -1) //jezeli pileczka przekroczy lewa linie
  {
    punktyp++;  //punkty dla zawodnika po prawej
    kier = 0;  //reset ustawien
    kier2 = 0; //
    pilkaa = 40; //
    pilkab = 22; //
    antyafk = 0;  //
    tone(2, 10, 100); //dzwiek zdobytego punktu
    delay(1500); //odczekanie
    sterowanie();  //zabezpieczenie przed przedwczesnym rozpoczeciem
    popga1 = ga1;  //ustalenie odniesienia paletek
    popga2 = ga2;  //^^^^
  }
  if(pilkaa >= 81)  //jezeli pileczka przekroczy prawa linie
  {
    punktyl++; //punkty dla zawodnika po lewej (reszta to samo)
    kier = 0;
    kier2 = 0;
    pilkaa = 40;
    pilkab = 22;
    antyafk = 0;
    tone(2, 10, 100);
    delay(1500);
    sterowanie();
    popga1 = ga1;  //ustalenie odniesienia paletek
    popga2 = ga2;  //^^^^
  }
}

void punkty()  //wyswietlanie ilosci punktów
{
   display.setCursor(21, 5);
   display.setTextColor(BLACK, WHITE);
   display.setTextSize(1);
   display.print(punktyl);
   display.setCursor(58, 5);
   display.print(punktyp);
}

void czyodbito()
{
  if(pilkaa >= 4 && pilkaa <= 6 && pilkab >= (ga1 - 3) && pilkab <= (ga1 + 12)) //czy pilka dotknela paletki
    {  
  kier = 1;  //po dotknieciu odbija sie w prawo
  if((kier2 == 1 || kier2 == 0 || kier2 == 3) && pilkab >= (ga1 - 3) && pilkab <= (ga1 + 3))  //jezeli uderzyla w jej gorna czesc lecac w gore lub prosto to leci dalej w gore
  {
  kier2 = random(1, 11);  //10% szansy na superodbicie w gore
  if(kier2 <= 9)
  kier2 = 1;
  if(kier2 == 10)
  kier2 = 3;
  }
  if((kier2 == 1 || kier2 == 0 || kier2 == 2 || kier2 == 3 || kier2 == 4) && pilkab >= (ga1 + 4) && pilkab <= (ga1 + 6))  //jezeli uderzyla w jej srodek lecac w jakikolwiek sposob to leci prostpadle do paletki
  kier2 = 0;
  if((kier2 == 2 || kier2 == 0 || kier2 == 4) && pilkab >= (ga1 + 7) && pilkab <= (ga1 + 12))  //jezeli uderzyla w jej dolna czesc lecac w dol lub prosto to leci w dol
  {
  kier2 = random(1, 11);  //10% szansy na superodbicie w dol
  if(kier2 <= 9)
  kier2 = 2;
  if(kier2 == 10)
  kier2 = 4;
  }
  if((kier2 == 2 || kier2 == 4) && pilkab >= (ga1 - 3) && pilkab <= (ga1 + 3))  //jezeli uderzyla w jej gorna czesc lecac w dol to leci prostopadle do paletki
  kier2 = 0;
  if((kier2 == 1 || kier2 == 3) && pilkab >= (ga1 + 7) && pilkab <= (ga1 + 12))  //jezeli uderzyla w jej dolna czesc lecac w gore to leci prostopadle do paletki
  kier2 = 0;
  tone(2, 2000, 8); //odglos odbicia
    }
    if(pilkaa <= 77 && pilkaa >= 75 && pilkab >= (ga2 - 3) && pilkab <= (ga2 + 12))  //czy pilka dotknela paletki
    {
  kier = 2;  //po dotknieciu odbija sie w lewo
    if((kier2 == 1 || kier2 == 0 || kier2 == 3) && pilkab >= (ga2 - 3) && pilkab <= (ga2 + 3))  //to samo
  {
  kier2 = random(1, 11);  //
  if(kier2 <= 9)
  kier2 = 1;
  if(kier2 == 10)
  kier2 = 3;
  }
  if((kier2 == 1 || kier2 == 0 || kier2 == 2 || kier2 == 3 || kier2 == 4) && pilkab >= (ga2 + 4) && pilkab <= (ga2 + 6))  //
  kier2 = 0;
  if((kier2 == 2 || kier2 == 0 || kier2 == 4) && pilkab >= (ga2 + 7) && pilkab <= (ga2 + 12))  //
  {
  kier2 = random(1, 11);  //
  if(kier2 <= 9)
  kier2 = 2;
  if(kier2 == 10)
  kier2 = 4;
  }
  if((kier2 == 2 || kier2 == 4) && pilkab >= (ga2 - 3) && pilkab <= (ga2 + 3))  //
  kier2 = 0;
  if((kier2 == 1 || kier2 == 3) && pilkab >= (ga2 + 7) && pilkab <= (ga2 + 12))  //
  kier2 = 0;
  tone(2, 2000, 8); //
    }
    if(pilkab <= 1 && kier2 == 1)  //jezeli odbila sie od gornej sciany
    {
    kier2 = 2;  //leci w dol
    tone(2, 3000, 8);  //odglos odbicia
    }
    if(pilkab <= 1 && kier2 == 3)  //jezeli odbila sie od gornej sciany
    {
    kier2 = 4;  //leci mocno w dol
    tone(2, 3000, 8);  //odglos odbicia
    }
    if(pilkab >= 44 && kier2 == 2)  //jezeli odbila sie od dolnej sciany
    {
    kier2 = 1;  //leci w gore
    tone(2, 3000, 8);  //odglos odbicia
    }
    if(pilkab >= 44 && kier2 == 4)  //jezeli odbila sie od dolnej sciany
    {
    kier2 = 3;  //leci mocno w gore
    tone(2, 3000, 8);  //odglos odbicia
    }
}

void bot()  //dzialania bota, gdy wybrano gre z nim
{
  if(ga2 + 5 == pilkab - 5 + random(0, 11) || (pilkaa < 40 || pilkaa > 40 && pilkaa < 41));  // jezeli pilka wpadnie na jego strone lub stoi ona w bezruchu ma on jechac w jej strone, ale ustawic sie w miare losowy sposob
  else
  {
  if(ga2 + 5 < pilkab - 5 + random(0, 11))  //
  ga2 = ga2 + 1;
  if(ga2 + 5 > pilkab - 5 + random(0, 11))  //
  ga2 = ga2 - 1;
  if(ga2 <= 0)  //ochrona przed przejechaniem za linie
  ga2 = 1;
  if(ga2 >= 36)
  ga2 = 35;
  pilkaprosto();  //jezeli pilka odbija sie ciagle prostopadle bot ma za zadanie to zmienic
  }
}

void pilkaprosto()
{

  if(kier2 == 0 && popkier != kier)  //liczenie odbic prostopadlych z rzedu
  antyafk++;
  if(kier2 != 0)  //reset liczenia, gdy nie bedzie ono prostopadle
  antyafk = 0;
  popkier = kier;  //spis poprzedniego kierunku by moc okreslic moment odbicia
  if(antyafk >= 3 && tryb == 0)  //jezeli pojawiaja sie trzy prostopadle odbicia z rzedu, a paletka jest przy sciankach 
  {
   if(ga2 < 10)
    ga2++; 
   else
   {
   if(ga2 > 25)
    ga2--;
    else
    {
      tryb = 1;  //przelaczenie trybu, gdy nie jest juz przy sciankach
    }
   }
  if(ga2 <= 0)  //ochrona przed przejechaniem za linie
  ga2 = 1;
  if(ga2 >= 36)
  ga2 = 35;
  }
    if(antyafk >= 3 && tryb == 1)  //jezeli pojawiaja sie trzy prostopadle odbicia z rzedu, a paletka jest na wolnym terenie
    {
      if(gd == 0 || antyafk > 5)
      gd = random(1, 3);  //losowanie kierunku
      if (gd == 1)  //dol
      ga2++;
      if(gd == 2)  //gora
      ga2--;
      if(kier2 != 0)  //gdy pilka zmienia kierunek wszystko jest resetowane
      {
      gd = 0;
      tryb = 0;
      } 
        if(ga2 <= 0)  //ochrona przed przejechaniem za linie
        ga2 = 1;
        if(ga2 >= 36)
        ga2 = 35;
      
    }

}

void loop()
{
display.drawRect(0, 0, 84, 48, BLACK);  //rysuje obramowke
display.drawLine(38, 0, 38, 48, BLACK);  //rysuje linie po lewej polowie
display.drawLine(45, 0, 45, 48, BLACK);  //rysuje linie po prawej polowie
czyprzegralem();
punkty();
if(wybrano==2)
bot();
sterowanie();
czyodbito();
pileczka();
display.display();  //wyswietla
delay(5);  //odczekuje 5/1000 sekundy
display.clearDisplay();  //czysci ekran
}




