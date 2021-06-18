// 16 Bant ledli spektrum analizörü
// neopiksel led şerit 8 * 16

// Mevcut sorun Yetersiz bellek nedeniyle 256'nın tümünü açmak zor
// Neo pikseli açan parçayı doğrudan kodlamak daha iyi olur
// Frekans bölünmesine tekrar dikkat edin.
// Değilse Mega kullanın

#include <Adafruit_NeoPixel.h>
#define LED_PIN 6

unsigned char Led_Sayisi=192;
unsigned char Bant_Sayisi=24;
unsigned char Led_Yuksekligi=8;

Adafruit_NeoPixel Serit = Adafruit_NeoPixel(Led_Sayisi, LED_PIN, NEO_GRB + NEO_KHZ800); //192

//#define LIN_OUT 1 
#define LOG_OUT 1//FHT doğrusal çıktı büyüklüğü
#define FHT_N 256 //spektrum genişliği x2
#include <FHT.h>  //FHT kütüphanesi
#define AUDIO_IN A0 //ses giriş pini

#define Serit_Dizilimi 0  //0:Paralel 1:ZigZag

unsigned char parlaklik = 255;      // 0 - 255

unsigned char cizgi_rengi = 0;     // 0: gökkuşağı
unsigned char cizgi_Yonu = 0 ;     // 1: yukarı 0: aşağı 

unsigned char Pik_Yonu = 0 ;       // 1: yukarı, 0: aşağı
unsigned char Pik_Rengi = 7  ;     // 7 beyaz
  
unsigned char Rasgele_Bayrak= 1;   // 1: açık  2: kapalı

int Cizgi_Rengi_Butonu = 7; //Cizgi Rengi secimi
int Cizgi_Rengi_Butonu_Durumu = 0, OncekiCizgi_Rengi_Butonu_Durumu = 0;  //buton durumları için
long Cizgi_Rengi_Butonu_Zamani; //buton basılma süresi için

int Pik_Rengi_Butonu = 8; //pik rengi secimi
int Pik_Rengi_Butonu_Durumu = 0, OncekiPik_Rengi_Butonu_Durumu = 0;
long Pik_Rengi_Butonu_Zamani;

int Desen_Yonu_Butonu = 9; //desen yönü sevimi
int Desen_Yonu_Butonu_Durumu = 0, OncekiDesen_Yonu_Butonu_Durumu = 0;
long Desen_Yonu_Butonu_Zamani;

int Pik_Yonu_Butonu = 10; //pik yönü sevimi
int Pik_Yonu_Butonu_Durumu = 0, OncekiPik_Yonu_Butonu_Durumu = 0;
long Pik_Yonu_Butonu_Zamani;

int Karsilama_Butonu = 11; //pik yönü sevimi
int Karsilama_Butonu_Durumu = 0, OncekiKarsilama_Butonu_Durumu = 0;
long Karsilama_Butonu_Zamani;

int parlaklikPin = A4; //parlaklık pini
int ParlaklikDegeri, OncekiParlaklikDegeri; //parlaklık durumları
int SensorSapmaParlakligi = 3;      // dalgalanan değerleri ortadan kaldırmak için
int LED_PARLAKLIGI = 5;  // parlaklık (0-255)

int Ses_Ayar_PinPin = A5;  //ses kazanc pini
int Ses_Ayar_Pin_Degeri = 110; // 0-255, başlangıç değeri
float Ses_Ayar_Degeri = 1;  //ses alt limit çarpanı
int maxSes_Ayar_Pin_Degeri = 1000;   //Ses ayar katsayısı  realde 500

signed int Frekans_Sec[26] = {  
  //0, 4, 8, 12, 16, 24, 32, 40, 48, 56, 64, 72, 90, 98, 106, 110, 127 //apo 16
  //0,5,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80,85,90,95,100,105,110,115,120,127 //apo 24
  0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,64,68,72,76,80,95,105,110,115,127
  //0,3,6,9,12,15,18,21,24,27,30,33,36,39,42,45,48,51,54,60,70,75,85,95,105,127
};
signed int Frekans_ofseti[24] = { 
   65,55,50,35,25,15,15,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10
};      
unsigned char Frekans_Bolumleri[24] = {  
   3,3,3,4,5,6,7,8,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6
};

unsigned char band[24] = { 0 };        //Ana Band seviyesi için
unsigned char display_band[24]= { 0 }; //Çubuk seviyesi için
unsigned char pik_band[24]={ 0 };      //pik seviyesi için

//--- pik düşme 2 -- için
boolean dusmeBayragi;   //Pik için düşme bayrağı
#define DUSME_DURMA 250 //max pik seviyesinde bekleme süresi
#define DUSME_BEKLE 20  //pik düşme hızı
unsigned long seviyeZamani[24]; //pik seviye zamanını aklında tutmak için
unsigned long dusmeZamanlayicisi; //şuan ki zamanı aklında tutmak için

float Ses_Kazanci = 0.9;  //gecerli olan 0.4

//------------------- Abdullah metin için -----------------------------------------
#include "fonts.h"

// **************** AYARLAR ****************
#define TEXT_ISTIKAMET 1  // 1 - yatay, 0 - dikey
#define CEVIR_DIKEY 0     // metni dikey olarak çevir (0/1)
#define CEVIR_YATAY 0     // metni yatay olarak çevir (0/1)

#define TEXT_Led_Yuksekligi 0     // metnin çalışma yüksekliği (matrisin altından)
#define KARAKTER_Bant_Sayisi 7  // yazı tipi harf genişliği )5x7:5 - 8x8:7
#define KARAKTER_Led_Yuksekligi 8 // yazı tipi harf yüksekliği
#define ARA_BOSLUK 1         // Harfler arası boşluk miktarı, piksel olarak

boolean tamMetinBayragi = false;
boolean yuklemeBayragi = true;

#if (Serit_Dizilimi==0)
#define SEGMENTLER 1
#define MATRIX_TIPI 1      // matris tipi: 0 - zikzak, 1 - paralel
#define BAGLANTI_ACISI 0   // bağlantı açısı: 0 - sol alt, 1 - sol üst, 2 - sağ üst, 3 - sağ alt
#define KOSEGEN_YONU 1     // bandın köşeden yönü: 0 - sağ, 1 - yukarı, 2 - sol, 3 - aşağı

#else
#define SEGMENTLER 1
#define MATRIX_TIPI 0      // matris tipi: 0 - zikzak, 1 - paralel
#define BAGLANTI_ACISI 1   // bağlantı açısı: 0 - sol alt, 1 - sol üst, 2 - sağ üst, 3 - sağ alt
#define KOSEGEN_YONU 3     // bandın köşeden yönü: 0 - sağ, 1 - yukarı, 2 - sol, 3 - aşağı

#endif

//-------------------------------------------------------------------------


#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit)) // ADC Kayıt bitlerini ayarla (asm)
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))  
        
void setup() {
  
      //analogReference(EXTERNAL); //ref kullan
      //analogReference(INTERNAL);
  
      //TIMSK0 = 0; // alt titreşim için zamanlayıcıyı kapat0  //Pik düşme millis() için iptal edildi
      //ADCSRA = 0xe5; // adc'yi serbest çalışma moduna ayarlayın
      //ADMUX = 0x40; // adc0 kullan A0
      //ADMUX = 0x41; // adc1 kullan A1
      //DIDR0 = 0x01; // adc0 için dijital girişi kapat 

      //ADCSRA = 0b11100101;    // set ADC to free running mode and set pre-scaler to 32 (0xe5)
      //ADMUX =  0b00000000;    // use pin A0 and external voltage reference
       
        sbi(ADCSRA, ADPS2);
        cbi(ADCSRA, ADPS1);
        sbi(ADCSRA, ADPS0);

        pinMode(Cizgi_Rengi_Butonu, INPUT);  //cizgi renk butonu
        pinMode(Pik_Rengi_Butonu, INPUT);    //pik renk butonu
        pinMode(Desen_Yonu_Butonu, INPUT);   //Desen Yönü butonu
        pinMode(Pik_Yonu_Butonu, INPUT);     //pik Yönü butonu
        pinMode(Karsilama_Butonu, INPUT);    //Karsilama Ekranı
        
        Serit.setBrightness(parlaklik); 
        Serit.begin(); // Şerdi Başlat
        Serit.show();  // Tüm pikselleri 'kapalı' olarak başla
            
        //Karsilama_Ekrani();
        //Karsilama_Ekrani2(); 
}

void loop() {
        
        while(1) { // titremeyi azalttığı için
        
                cli();  // UDRE kesintisi arduino'da bu şekilde yavaşlar
                for (int i = 0 ; i < FHT_N ; i++) {
                  int sesOrnegi = analogRead(AUDIO_IN); //analog değeri oku
                  fht_input[i] = sesOrnegi*Ses_Ayar_Degeri*6; // gerçek verileri dizine kaydet
                }
                fht_window();  // daha iyi frekans tepkisi için verileri pencerele
                fht_reorder(); // fht yapmadan önce verileri yeniden sırala
                fht_run();     // verileri fht içinde işlemek için
                fht_mag_log(); // fht çıktısını al (fht_log_out komutu için)
                //fht_mag_lin();
                sei();
          
        /*      
           for (int i = 0; i < FHT_N; i++) {
                while (!(ADCSRA & 0x10));   // ADC'nin mevcut dönüşümü tamamlamasını bekleyin, yani ADIF bit seti
                ADCSRA = 0b11110101 ;       // ADIF bitini temizle, böylece ADC bir sonraki işlemi yapabilir (0xf5)
                //ADLAR biti 0'dır, bu nedenle 10 bitlik ADC Veri yazmaçları sağa hizalanır
                byte m = ADCL;              // adc verilerini getir
                byte j = ADCH;
                int fth_deger = (j << 8) | m;   // int haline getir
                fth_deger -= 0x0200;            // signet bir int şeklinde
                fth_deger <<= 6;                // 16bit signet bir int şeklinde
                //fth_deger = fth_deger*2;   //0 ~ 7 yükseklik limiti. 0 ~ 15 için bu satırı iptal et.
                fht_input[i] = fth_deger;   // sıkıştırdıktan sonra fht girdi dizisine kopyala
                //fht_input[i+1] = 0;       // garip kutuları 0 olarak ayarla
              }
          // ++ FHT veri işlemine başla -+-+--+-+--+-+--+-+--+-+--+-+--+-+-
          fht_window();    // daha iyi frekans tepkisi için verileri pencerele
          fht_reorder();   // fht yapmadan önce verileri yeniden sırala
          fht_run();       // verileri fht içinde işle
          fht_mag_lin();   // fht çıktısını al (fft_lin_out)
          //fht_mag_log();   // fht çıktısını al (fht_log_out komutu için)
        */
                     
//----------------------------         Parlaklık ve Ses ayarları        -------------------------------- 
           
             //   SENSOR_DEGERLERINI_OKU(); //sensörleri oku    
                          
//----------------------------    Cizgi düşme Hızlarını Belirle   --------------------------------  
               volatile static unsigned char cizgi_gecikme_suresi=0;
                if(++cizgi_gecikme_suresi>=3) {    //3 Cizgi düşme geçikmesi için
                     cizgi_gecikme_suresi=0; 
                     for(int Sira=0; Sira<Bant_Sayisi; Sira++){ //16 bat sayısı kadar
                        if( display_band[Sira] > 0 ){ 
                            display_band[Sira] -=1;
                        }
                     }    
                } 
                
//----------------------------      Renk efektlerinde akış için     -------------------------------- 
                static unsigned char renk_akmasi=0;
                renk_akmasi++;

//----------------------------    Frekans Verilerini Şeride Göre Düzenlenmesi  --------------------------------
                for(int Band_Sirasi=0; Band_Sirasi<Bant_Sayisi; Band_Sirasi++)   //bant sayısı kadar dön
                {
                        int fht_deger=0;
                        long fht_toplam=0;
                        //Çizgi düzeltme kodu
                        unsigned char toplam_sayi = Frekans_Sec[Band_Sirasi+1] - Frekans_Sec[Band_Sirasi];
                        for(int z =toplam_sayi-1 ;  z > 0; z--) //  2,4,7
                        {
                                fht_toplam += fht_log_out[ Frekans_Sec[Band_Sirasi] + z ];
                        }
                        
                        // frekans belirle
                        fht_deger = (fht_toplam / toplam_sayi - Frekans_ofseti[Band_Sirasi]) * Ses_Kazanci;
                        // fht_deger = fht_log_out[ Frekans_Sec[Band_Sirasi] ] - Frekans_ofseti[Band_Sirasi];     

                        if( fht_deger < 0 ) fht_deger = 0; //al limit sınırlaması
                        
                       // Band yüksekliğini belirleme ve 0 ~ 8 yükseklik limitleme
                        band[Band_Sirasi] = fht_deger / Frekans_Bolumleri[Band_Sirasi];                       
                        if(Pik_Rengi == 8){  //pik rengi siyah ise yüksekliği 8 yap
                          if(band[Band_Sirasi] > (Led_Yuksekligi-1)) band[Band_Sirasi]=(Led_Yuksekligi);
                        }
                        else { //pik rengi siyah değil ise yüksekliği 7 yap
                          if(band[Band_Sirasi] > (Led_Yuksekligi-1)) band[Band_Sirasi]=(Led_Yuksekligi-1);
                        }
                        
                        // Cizgi uzunluklarını belirle
                        if( display_band[Band_Sirasi] < band[Band_Sirasi] )  display_band[Band_Sirasi]= band[Band_Sirasi];      
                        
                        // pik noktalarını belirle
                        if( pik_band[Band_Sirasi] < band[Band_Sirasi] | pik_band[Band_Sirasi] > (Led_Yuksekligi-1) ) { 
                          pik_band[Band_Sirasi] = band[Band_Sirasi];  // yukarı aşağı nokta güncelleme
                          seviyeZamani[Band_Sirasi] = millis(); //pik zamanını aklında tut
                        }
                        
//---------------------------------------------    pik düşme    --------------------------------
                        if (dusmeBayragi) {
                              if ((long)millis() - seviyeZamani[Band_Sirasi] > DUSME_DURMA) {    // maksimum, DUSME_DURMA'dan daha uzun bir süre yüksekliğinde kalırsa
                               // if (pik_band[Band_Sirasi] >= 0) pik_band[Band_Sirasi]--;                // nokta yüksekliğini 1 azalt - Dikkat! Minimum değer -1'dir!
                                if(Pik_Yonu){  
                                    if( pik_band[Band_Sirasi] > 0 )pik_band[Band_Sirasi] +=1;    // yukarı 
                                    }        
                                    else {  
                                      if( pik_band[Band_Sirasi] > 0 )pik_band[Band_Sirasi] -=1;  // aşağı
                                    }
                              }
                        } 

//----------------------------    Şerit Renklerinin Ayarlanması   --------------------------------

                for(int Band_Yuksekligi=0; Band_Yuksekligi < Led_Yuksekligi; Band_Yuksekligi++)
                {
                  //Renkleri ve desen yönünü Belirle
               /*   Desen_Yonu_Belirle_Buton();
                  Pik_Yonu_Belirle_Buton();
                  Cizgi_Renk_Belirle_Buton();
                  Pik_Renk_Belirle_Buton();
                  Karsilama_Buton();
              */    
                  int address =0;
                  // hat yönüne göre led adresini belirle
                  //if( (Band_Sirasi % 2) == (cizgi_Yonu)) 

                  if (Serit_Dizilimi==0){
                       if( cizgi_Yonu == 0) address = (Led_Yuksekligi*Band_Sirasi) + Band_Yuksekligi ;
                       else                 address = (Led_Yuksekligi*Band_Sirasi) + (Led_Yuksekligi-1)-Band_Yuksekligi; 
                      }
                      else {
                        if( (Band_Sirasi % 2) == (cizgi_Yonu)) address = (Led_Yuksekligi-1)-Band_Yuksekligi + (Led_Yuksekligi*Band_Sirasi);
                        else                                   address =  Band_Yuksekligi + (Led_Yuksekligi*Band_Sirasi); 
                  }
                  
                  // cizgi rengi
                  if(display_band[Band_Sirasi] > Band_Yuksekligi) 
                  {
                               //if(cizgi_rengi == 0)Serit.setPixelColor(address, Serit.ColorHSV(65536L / 10 * Band_Yuksekligi,240,250));  // çizgi rengi: Gökkuşağı
                               if(cizgi_rengi == 0)Serit.setPixelColor(address, Gokkusagi_apo( (Led_Yuksekligi*Band_Sirasi) + renk_akmasi  )   ) ;
                          else if(cizgi_rengi == 1)Serit.setPixelColor(address, Serit.ColorHSV(65536L / 10 * Band_Sirasi, 240, 250));
                          else if(cizgi_rengi == 2)Serit.setPixelColor(address, Serit.gamma32(Serit.ColorHSV(65536L / 10 * Band_Yuksekligi) ) );
                          else if(cizgi_rengi == 3)Serit.setPixelColor(address, Serit.gamma32(Serit.ColorHSV(65536L / 10 * Band_Sirasi) ) );
                          else if(cizgi_rengi == 4)Serit.setPixelColor(address, Gokkusagi((Led_Yuksekligi-1)-Band_Yuksekligi + (Led_Yuksekligi*Band_Sirasi) + renk_akmasi ) );
                          else if(cizgi_rengi == 5)Serit.setPixelColor(address, Gokkusagi( (Led_Yuksekligi*Band_Sirasi) + renk_akmasi  )   ) ;
                          else if(cizgi_rengi == 6)Serit.setPixelColor(address, 0xFF0000);
                          else if(cizgi_rengi == 7)Serit.setPixelColor(address, 0x0000FF);
                          else if(cizgi_rengi == 8)Serit.setPixelColor(address, 0x00FF00);
                          else ;
                  }
                  // pik rengi
                  else if(pik_band[Band_Sirasi] == Band_Yuksekligi) 
                  {
                          if(Pik_Rengi == 0)     Serit.setPixelColor(address, Gokkusagi((Led_Yuksekligi-1)-Band_Yuksekligi + (Led_Yuksekligi*Band_Sirasi) + renk_akmasi)  );  // pik rengi: Gökkuşağı
                          else if(Pik_Rengi == 1)Serit.setPixelColor(address, 0xFF0000);  // Kırmızı
                          else if(Pik_Rengi == 2)Serit.setPixelColor(address, 0xFFFF00);  // Sarı
                          else if(Pik_Rengi == 3)Serit.setPixelColor(address, 0x00FF00);  // Yeşil
                          else if(Pik_Rengi == 4)Serit.setPixelColor(address, 0x00FFFF);  // Turkuaz
                          else if(Pik_Rengi == 5)Serit.setPixelColor(address, 0x0000FF);  // Mavi
                          else if(Pik_Rengi == 6)Serit.setPixelColor(address, Gokkusagi((Led_Yuksekligi-1)-Band_Yuksekligi + (Led_Yuksekligi*Band_Sirasi)));  // Gökkuşağı
                          else if(Pik_Rengi == 7)Serit.setPixelColor(address, 0xFFFFFF);  // Beyaz
                          //else if(Pik_Rengi == 7)Serit.setPixelColor(address, Renk_HSV(30, 30, 30)) ;  // kısık Beyaz 
                          //else if(Pik_Rengi == 7)Serit.setPixelColor(address, Serit.ColorHSV(30,30,100)); // kısık Beyaz 
                          else if(Pik_Rengi == 8)Serit.setPixelColor(address, 0);  // Yiyah - Pik yok
                          else ;
                  }
                  else  Serit.setPixelColor(address, 0  ); //band yüksekliğinden fazla ise siyah yap (sönük kısımlar)
                }
        }
//----------------------------    Şerit Gösterimi   --------------------------------
       Serit.show();

//----------------------------    pik düşme Süresini belirlenmesi   --------------------------------
       dusmeBayragi = 0;                                    // düşme bayrağını sıfırla
       if (millis() - dusmeZamanlayicisi > DUSME_BEKLE) {   // düşme zamanıysa
          dusmeBayragi = 1;                                 // bayrağı 1yap
          dusmeZamanlayicisi = millis();                    // şuan ki zamanı aklında tut
       }      

//----------------------------    Renklerin ve Yönlerin Rasgele Belirlenmesi   ----------------------
                if( Rasgele_Bayrak )
                {
                    static unsigned int random_count=0;
                    random_count++;
                    if(random_count > 500) //değişim süresi
                    {
                      random_count=0;
                        cizgi_rengi  = random(9);
                        //cizgi_Yonu = random(2);
                        //Pik_Rengi  = random(9);
                        //Pik_Yonu   = random(2);
                    }
                }
                //Sabit renk testi için
                //cizgi_rengi = 6;
                //Pik_Rengi = 6;
                
//----------------------------------------------------------------------------------------------------               
    } //while sonu
}

//--------------------    Renk Secimi 0 iken Cizgi ve Pik Rengi Gökkuşağı  ----------------------------
uint32_t Gokkusagi(byte konumu) {

        if(konumu < 85) {
                return Serit.Color(konumu * 3, 255 - konumu * 3, 0);   
        } 
        else if(konumu < 170) {
                konumu -= 85;
                return Serit.Color(255 - konumu * 3, 0, konumu * 3);
        } 
        else {
                konumu -= 170;
                return Serit.Color(0, konumu * 3, 255 - konumu * 3);
        }
}

uint32_t Gokkusagi_apo(byte konum) {
  
  static byte renk[3];
 
  if(konum < 85) {
   renk[0]=konum * 3;
   renk[1]=255 - konum * 3;
   renk[2]=0;

  } else if(konum < 170) {
   konum -= 85;
   renk[0]=255 - konum * 3;
   renk[1]=0;
   renk[2]=konum * 3;

  } else {
   konum -= 170;
   renk[0]=0;
   renk[1]=konum * 3;
   renk[2]=255 - konum * 3;
   
  }
  
  return Serit.Color(renk[0], renk[1], renk[2]);
  
}

//-----------------------------------------------------------------------------------
void Pik_Yonu_Belirle_Buton(){ //pik yönünü değiştiröek için

    Pik_Yonu_Butonu_Durumu = digitalRead(Pik_Yonu_Butonu);
    
    if (Pik_Yonu_Butonu_Durumu == HIGH && OncekiPik_Yonu_Butonu_Durumu == LOW)
      Pik_Yonu_Butonu_Zamani = millis();
      
    if (Pik_Yonu_Butonu_Durumu == HIGH && OncekiPik_Yonu_Butonu_Durumu == HIGH) {
      if ((millis() - Pik_Yonu_Butonu_Zamani) > 100) {

        if (Pik_Yonu == 1) Pik_Yonu = 0;
        else Pik_Yonu = 1;

        while (digitalRead(Pik_Yonu_Butonu) == HIGH) {}
        Pik_Yonu_Butonu_Durumu = LOW;
      }
    } 
    OncekiPik_Yonu_Butonu_Durumu = Pik_Yonu_Butonu_Durumu;
}

//-----------------------------------------------------------------------------------
void Desen_Yonu_Belirle_Buton(){ //Desenin yönünü değiştiröek için

    Desen_Yonu_Butonu_Durumu = digitalRead(Desen_Yonu_Butonu);
    
    if (Desen_Yonu_Butonu_Durumu == HIGH && OncekiDesen_Yonu_Butonu_Durumu == LOW)
      Desen_Yonu_Butonu_Zamani = millis();
      
    if (Desen_Yonu_Butonu_Durumu == HIGH && OncekiDesen_Yonu_Butonu_Durumu == HIGH) {
      if ((millis() - Desen_Yonu_Butonu_Zamani) > 100) {

        if (cizgi_Yonu == 1) cizgi_Yonu = 0;
        else cizgi_Yonu = 1;

        while (digitalRead(Desen_Yonu_Butonu) == HIGH) {}
        Desen_Yonu_Butonu_Durumu = LOW;
      }
    } 
    OncekiDesen_Yonu_Butonu_Durumu = Desen_Yonu_Butonu_Durumu;
}

//-----------------------------------------------------------------------------------
void Cizgi_Renk_Belirle_Buton(){  //Cizgi Desenini değiştiröek için

    Cizgi_Rengi_Butonu_Durumu = digitalRead(Cizgi_Rengi_Butonu); //buton durumunu oku
    
    if (Cizgi_Rengi_Butonu_Durumu == HIGH && OncekiCizgi_Rengi_Butonu_Durumu == LOW) //buton durumu ve önceki durumu low ise 
      Cizgi_Rengi_Butonu_Zamani = millis();  //butonna basılma süresini aklında tut
      
    if (Cizgi_Rengi_Butonu_Durumu == HIGH && OncekiCizgi_Rengi_Butonu_Durumu == HIGH) { //buton durumu ve önceki durumu high ise
      if ((millis() - Cizgi_Rengi_Butonu_Zamani) > 100) {  //butonna basılma süresi limit süresini geçti ise işlem yap
        cizgi_rengi++;  //desen değerini artır
        if (cizgi_rengi > 8) cizgi_rengi =0; //max ise 0 la

        while (digitalRead(Cizgi_Rengi_Butonu) == HIGH) {} //buton bırakılana kadar bekle
        Cizgi_Rengi_Butonu_Durumu = LOW; //bırakıldıktan sonra bayrağı indir
      }
    } 
    OncekiCizgi_Rengi_Butonu_Durumu = Cizgi_Rengi_Butonu_Durumu; //buton durumunu güncelle
}

//-----------------------------------------------------------------------------------
void Pik_Renk_Belirle_Buton(){  //Pik Desenini değiştiröek için

    Pik_Rengi_Butonu_Durumu = digitalRead(Pik_Rengi_Butonu);
    
    if (Pik_Rengi_Butonu_Durumu == HIGH && OncekiPik_Rengi_Butonu_Durumu == LOW)
      Pik_Rengi_Butonu_Zamani = millis();
      
    if (Pik_Rengi_Butonu_Durumu == HIGH && OncekiPik_Rengi_Butonu_Durumu == HIGH) {
      if ((millis() - Pik_Rengi_Butonu_Zamani) > 100) {

         Pik_Rengi++;
         if (Pik_Rengi > 8) Pik_Rengi =0;

        while (digitalRead(Pik_Rengi_Butonu) == HIGH) {}
        Pik_Rengi_Butonu_Durumu = LOW;
      }
    }
    
    OncekiPik_Rengi_Butonu_Durumu = Pik_Rengi_Butonu_Durumu;
}

//-----------------------------------------------------------------------------------
void Karsilama_Buton(){  //Pik Desenini değiştiröek için

    Karsilama_Butonu_Durumu = digitalRead(Karsilama_Butonu);
    
    if (Karsilama_Butonu_Durumu == HIGH && OncekiKarsilama_Butonu_Durumu == LOW)
      Karsilama_Butonu_Zamani = millis();
      
    if (Karsilama_Butonu_Durumu == HIGH && OncekiKarsilama_Butonu_Durumu == HIGH) {
      if ((millis() - Karsilama_Butonu_Zamani) > 100) {
         Serit.clear();
         Karsilama_Ekrani();
         while (digitalRead(Karsilama_Butonu) == HIGH) {}
         Karsilama_Butonu_Durumu = LOW;
      }
    }
    
    OncekiKarsilama_Butonu_Durumu = Karsilama_Butonu_Durumu;
}

// -------------------------------- Sensor Okuma ------------------------------------
//-----------------------------------------------------------------------------------
void SENSOR_DEGERLERINI_OKU() {

  // parlaklık
  ParlaklikDegeri = analogRead(parlaklikPin);
  ParlaklikDegeri = map(ParlaklikDegeri, 0, 1023, 0, 255);
  if (abs(ParlaklikDegeri - OncekiParlaklikDegeri) > SensorSapmaParlakligi) {
      LED_PARLAKLIGI = ParlaklikDegeri;
      //LED_PARLAKLIGI =255; //proteus için
      Serit.setBrightness(LED_PARLAKLIGI);
      
      OncekiParlaklikDegeri = ParlaklikDegeri;
  }

  // Ses_Ayar_Pin
  Ses_Ayar_Pin_Degeri = analogRead(Ses_Ayar_PinPin);
  Ses_Ayar_Pin_Degeri = map(Ses_Ayar_Pin_Degeri, 0, 1023, 0, 255);
  Ses_Ayar_Degeri = ((float) Ses_Ayar_Pin_Degeri / 255 * (float) maxSes_Ayar_Pin_Degeri / 255);
  
}
//-----------------------------------------------------------------------------------
uint32_t Renk_HSV( uint16_t ton, uint8_t doygun, uint8_t parlak ) {

  ton = ton * 6;
  parlak = parlak * 2;
  
  uint8_t kirmizi, yesil, mavi;

  if (!doygun) {
    // Tek renkli, tüm bileşenler parlak
    kirmizi = yesil = mavi = parlak;
  } else {
    uint8_t altidaBiri = ton >> 8; //altidaBiri : dairenin altıda biri

    if (altidaBiri > 5)
      altidaBiri = 5;  // Ton altidaBirilarını tanımlı alanla sınırlayın

    yesil = parlak;    // Üst düzey

    // Gerçek hesaplamaları gerçekleştirin

    //   Aşağı seviye:
    //    -> (parlak * (255 - doygun) + hata kodu + 1) / 256
    
    uint16_t araSonuc1;        // Ara sonuç
    araSonuc1 = parlak * (uint8_t)(~doygun);
    araSonuc1 += 1;               // Hata düzeltme
    araSonuc1 += araSonuc1 >> 8;  // Hata düzeltme
    mavi = araSonuc1 >> 8;

    uint8_t ton_Kesir = ton & 0xff;  // altidaBiri içinde pozisyon
    uint32_t araSonuc2;      // Ara sonuç

    if (!(altidaBiri & 1)) {
      // kirmizi = ...eğim...
      // --> kirmizi = (parlak * ((255 << 8) - doygun * (256 - ton)) + hata_kodu1 + hata_Kodu2) / 65536
      araSonuc2 = parlak * (uint32_t)(0xff00 - (uint16_t)(doygun * (256 - ton_Kesir)));
      araSonuc2 += araSonuc2 >> 8;  // Hata düzeltme
      araSonuc2 += parlak;       // Hata düzeltme
      kirmizi = araSonuc2 >> 16;
    } 
    else {
      // kirmizi = ...aşağı eğim...
      // --> kirmizi = (parlak * ((255 << 8) - doygun * ton) + hata_kodu1 + hata_kodu2) / 65536
      araSonuc2 = parlak * (uint32_t)(0xff00 - (uint16_t)(doygun * ton_Kesir));
      araSonuc2 += araSonuc2 >> 8;  // Hata düzeltme
      araSonuc2 += parlak;       // Hata düzeltme
      kirmizi = araSonuc2 >> 16;
    }

    // RGB değerlerini altidaBirine göre değiştirin. Bu, ters sırada yapılır
    // aslına bağlı kalın çünkü takaslar atamalardan sonra yapılır.
    if (!(altidaBiri & 6)) {
      if (!(altidaBiri & 1)) {
        uint8_t temp = kirmizi;
        kirmizi = yesil;
        yesil = temp;
      }
    } 
    else {
      if (altidaBiri & 1) {
        uint8_t temp = kirmizi;
        kirmizi = yesil;
        yesil = temp;
      }
    }
    if (altidaBiri & 4) {
      uint8_t temp = yesil;
      yesil = mavi;
      mavi = temp;
    }
    if (altidaBiri & 2) {
      uint8_t temp = kirmizi;
      kirmizi = mavi;
      mavi = temp;
    }
  }
  return ((uint32_t)kirmizi << 16) | ((uint32_t)yesil << 8) | mavi;
}

//================================================================
//                        B O N U S
//================================================================
int Matrixden_Adres_Al(byte x, byte y) {
  int adres;
x = Bant_Sayisi - x - 1;

if (Serit_Dizilimi==0){ //Zig Zag

  adres = ((x + 1) * Led_Yuksekligi - Led_Yuksekligi + y);

}
else{  //Paralel
  if (x & 0x01) {
    //Tek sayı sütunlar geriye doğru artar  
    adres = ((x + 1) * Led_Yuksekligi - Led_Yuksekligi + y); 
  }
  else {
    //Cift sayı Sütunlar normal olarak artar
    adres = ((x + 1) * Led_Yuksekligi - y - 1);
  }
}

 return constrain(adres, 0, Led_Sayisi); 

}
//-------------------------------------------------------------------------------------------
// Özel karakterler çizin
void Karakteri_Ciz(byte deger, byte yeri, uint32_t rengi) {
  yeri = yeri-8;
  for (int x = 7; x >= 0; x--) {
    for (int y = 6; y >= 0; y--) {
      if ((Karakter_resmi[deger * 7 + 6 - y] >> x) & 0x01) {
        Serit.setPixelColor(Matrixden_Adres_Al(x + yeri, y + 1), rengi);
      }
    }
  }
}
//-------------------------------------------------------------------------------------------

void Karsilama_Ekrani(){
        //Serit.setBrightness(255); //proteus için
        
        Karakteri_Ciz(10, 23, Serit.Color(255,0,0)); //(karekter indeksi, başlama yeri, R,G,B)
        Karakteri_Ciz(11, 15, Serit.Color(0,255,0));
        Karakteri_Ciz(12, 7,  Serit.Color(0,0,255));
        Serit.show();
        delay(3000);
        Serit.clear();
        
        for (int a=0; a<3; a++){
                Karakteri_Ciz(14, 15, Serit.Color(0,255,0));
                Serit.show();
                delay(250);
                Serit.clear();
                Karakteri_Ciz(15, 15, Serit.Color(0,255,0));
                Serit.show();
                delay(250);
                Serit.clear();
                Karakteri_Ciz(14, 15, Serit.Color(0,255,0));
                Serit.show();
                delay(250);
                Serit.clear();
        }
        delay(1000);


        Karakteri_Ciz(13, 15, Gokkusagi(random(50,255))); //:)
        Serit.show();
        delay(1000);
        Serit.clear();

        Karakteri_Ciz(3, 15, Serit.Color(255,0,0));
        Serit.show();
        delay(500);
        Serit.clear();
        Karakteri_Ciz(2, 15, Serit.Color(0,255,0));
        Serit.show();
        delay(500);
        Serit.clear();
        Karakteri_Ciz(1, 15, Serit.Color(0,0,255));
        Serit.show();
        delay(500);
        Serit.clear();
        Karakteri_Ciz(0, 15, Serit.Color(64,0,255));
        Serit.show();
        delay(1000);
        Serit.clear();
}

//-----------------------------------------------------------------------------------------------------------
//                                           METIN İÇİN
//-----------------------------------------------------------------------------------------------------------

void Karsilama_Ekrani2(){
  
    for (int t=0; t<285; t++){
          Metin_Yaz("Abdullah YÖN - Spectrum Analyzer", 2);
    } 
        
}


//-----------------------------------------------------------------------------------------------------------
int ekran_dengeleme = Bant_Sayisi;

void Metin_Yaz(String text, uint32_t renk) {
  if (yuklemeBayragi) {
    ekran_dengeleme = Bant_Sayisi;   // sağ kenara geri sar
    yuklemeBayragi = false;    
    tamMetinBayragi = false;
  }
  
    Serit.clear();
    byte i = 0, j = 0;
    while (text[i] != '\0') {
      if ((byte)text[i] > 191) {    // ingilizce harfleriyle çalışıyoruz!
        i++;
      } else {
        HarfCiz(j, text[i], ekran_dengeleme + j * (KARAKTER_Bant_Sayisi + ARA_BOSLUK), renk);
        i++;
        j++;
      }
    }
    tamMetinBayragi = false;
    
    ekran_dengeleme--;
    if (ekran_dengeleme < -j * (KARAKTER_Bant_Sayisi + ARA_BOSLUK)) {
      ekran_dengeleme = Bant_Sayisi + 3;
      tamMetinBayragi = true;
    }

  Serit.show();
  //delay(25);  

}

void HarfCiz(uint8_t index, uint8_t harf, int16_t ekran_dengeleme, uint32_t renk) {
  int8_t baslangic_konumu = 0, bitis_konumu = KARAKTER_Bant_Sayisi;
  uint32_t harfrenk;
  if      (renk == 1) harfrenk = Serit.ColorHSV(65536L / 10 * ekran_dengeleme, 240, 250);
  else if (renk == 2) harfrenk = Serit.ColorHSV(65536L / 10 * index, 240, 250);
  else if (renk == 3) harfrenk = Serit.Color(255, 0, 0);
  else if (renk == 4) harfrenk = Serit.Color(0, 255, 0);
  else if (renk == 5) harfrenk = Serit.Color(0, 0, 255);
  else harfrenk = Serit.Color(255, 0, 0);

  if (ekran_dengeleme < -KARAKTER_Bant_Sayisi || ekran_dengeleme > Bant_Sayisi) return;
  if (ekran_dengeleme < 0) baslangic_konumu = -ekran_dengeleme;
  if (ekran_dengeleme > (Bant_Sayisi - KARAKTER_Bant_Sayisi)) bitis_konumu = Bant_Sayisi - ekran_dengeleme;

  for (byte i = baslangic_konumu; i < bitis_konumu; i++) {
    int buByte;
    if (CEVIR_DIKEY) buByte = Yazi_Fontu_Al((byte)harf, KARAKTER_Bant_Sayisi - 1 - i);
    else buByte = Yazi_Fontu_Al((byte)harf, i);

    for (byte j = 0; j < KARAKTER_Led_Yuksekligi; j++) {
      boolean buBit;
        
      if (CEVIR_YATAY) buBit = buByte & (1 << j);
      else buBit = buByte & (1 << (KARAKTER_Led_Yuksekligi - 1 - j));

      // sütun çizme (i - yatay konum, j - dikey)
      if (TEXT_ISTIKAMET) {
        if (buBit) Serit.setPixelColor(Piksel_Koordinatini_Al(ekran_dengeleme + i, TEXT_Led_Yuksekligi + j), harfrenk); 
        else PikselCiz_XY(ekran_dengeleme + i, TEXT_Led_Yuksekligi + j, 0x000000);
      } else {
        if (buBit) Serit.setPixelColor(Piksel_Koordinatini_Al(i, ekran_dengeleme + TEXT_Led_Yuksekligi + j), harfrenk); 
        else PikselCiz_XY(i, ekran_dengeleme + TEXT_Led_Yuksekligi + j, 0x000000);
      }

    }
  }
}

//-----------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------
// **************** MATRIX AYARI ****************
#if (BAGLANTI_ACISI == 0 && KOSEGEN_YONU == 0)
#define Bant_Sayisi Bant_Sayisi
#define BU_X x
#define BU_Y y

#elif (BAGLANTI_ACISI == 0 && KOSEGEN_YONU == 1)
#define Bant_Sayisi Led_Yuksekligi
#define BU_X y
#define BU_Y x

#elif (BAGLANTI_ACISI == 1 && KOSEGEN_YONU == 0)
#define Bant_Sayisi Bant_Sayisi
#define BU_X x
#define BU_Y (Led_Yuksekligi - y - 1)

#elif (BAGLANTI_ACISI == 1 && KOSEGEN_YONU == 3)
#define Bant_Sayisi Led_Yuksekligi
#define BU_X (Led_Yuksekligi - y - 1)
#define BU_Y x

#elif (BAGLANTI_ACISI == 2 && KOSEGEN_YONU == 2)
#define Bant_Sayisi Bant_Sayisi
#define BU_X (Bant_Sayisi - x - 1)
#define BU_Y (Led_Yuksekligi - y - 1)

#elif (BAGLANTI_ACISI == 2 && KOSEGEN_YONU == 3)
#define Bant_Sayisi Led_Yuksekligi
#define BU_X (Led_Yuksekligi - y - 1)
#define BU_Y (Bant_Sayisi - x - 1)

#elif (BAGLANTI_ACISI == 3 && KOSEGEN_YONU == 2)
#define Bant_Sayisi Bant_Sayisi
#define BU_X (Bant_Sayisi - x - 1)
#define BU_Y y

#elif (BAGLANTI_ACISI == 3 && KOSEGEN_YONU == 1)
#define Bant_Sayisi Led_Yuksekligi
#define BU_X y
#define BU_Y (Bant_Sayisi - x - 1)

#else
#define Bant_Sayisi Bant_Sayisi
#define BU_X x
#define BU_Y y

#endif
// paneldeki bir pikselin koordinal bilgisini almak
uint16_t Piksel_Koordinatini_Al(int8_t x, int8_t y) {
  if ((BU_Y % 2 == 0) || MATRIX_TIPI) {                 // eğer paralel bir dize
    return (BU_Y * Bant_Sayisi + BU_X);
  } else {                                              // eğer zigzag dize
    return (BU_Y * Bant_Sayisi + Bant_Sayisi - BU_X - 1);
  }
}
//-----------------------------------------------------------------------------------------------------------
// koordinatlarla bir nokta çizme fonksiyonu X Y
void PikselCiz_XY(int8_t x, int8_t y, uint32_t renk) {
  if (x < 0 || x > Bant_Sayisi - 1 || y < 0 || y > Led_Yuksekligi - 1) return;
  int buPiksel = Piksel_Koordinatini_Al(x, y) * SEGMENTLER;
  for (byte i = 0; i < SEGMENTLER; i++) {
    Serit.setPixelColor(buPiksel + i, renk);
  }
}
// ------------- SERVİS FONKSİYONLARI --------------
// font5x7 dizisindeki karakter kodunun yorumlayıcısı (Arduino IDE 1.8. * ve üstü için)
/*
uint8_t Yazi_Fontu_Al(uint8_t font, uint8_t satir) {
  //font = font - '0' + 16;   // ASCII tablosundan karakter kodunun dizi numaralandırmasına göre sayıya çevrilmesi
  font = font - 32;
  
  if (font <= 90){ 
   // Serial.print(font);
   // Serial.print(" : ");
   // Serial.println(satir);
    return pgm_read_byte(&(font5x7[font][satir]));     // ingilizce harfler ve semboller için
  }

  else if (font >= 96 && font <= 111) {
    //Serial.print(font);
    //Serial.print(" / ");
    //Serial.println(satir);
    return pgm_read_byte(&(font5x7[font - 8][satir]));
  }

  else if (font >= 112 && font <= 159) {    
    //Serial.print(font);
    //Serial.print(" - ");
    //Serial.println(satir);
    return pgm_read_byte(&(font5x7[font - 60][satir])); // Türkçe harfler için
  } 
}
*/
void MetniDoldur(String text, uint32_t renk) {
  tamMetinBayragi = false;
  return;
}


// ASCII karakter kodu yorumlayıcısını font8x8 dizisindeki Türkçe İngilizce Karekterler için
uint8_t Yazi_Fontu_Al(uint8_t font, uint8_t satir) {

  //uint8_t fontOrg = font;
  //font = font - '0' + 16;   // ASCII tablosundan karakter kodunun dizi numaralandırmasına göre sayıya çevrilmesi
  font = font - 32;
  //Bilgi ekranı
  //Serial.print(satir);
  //Serial.print(" - ");
  //Serial.println(fontOrg);
  //delay(1000);

  //Büyük Tükçe Harfler
  if      (font == 103) return pgm_read_byte(&(font8x8[95][satir]));//Ç
  else if (font == 144) return pgm_read_byte(&(font8x8[96][satir]));//İ
  else if (font == 124) return pgm_read_byte(&(font8x8[97][satir]));//Ü
  else if (font == 118) return pgm_read_byte(&(font8x8[98][satir]));//Ö
  else if (font == 126) return pgm_read_byte(&(font8x8[99][satir]));//Ş

  //Küçük Türkçe Hafler
  else if (font == 145) return pgm_read_byte(&(font8x8[100][satir]));//ı
  else if (font == 156) return pgm_read_byte(&(font8x8[101][satir]));//ü
  else if (font == 150) return pgm_read_byte(&(font8x8[102][satir]));//ö
  else if (font == 127) return pgm_read_byte(&(font8x8[103][satir]));//ş
  else if (font == 135) return pgm_read_byte(&(font8x8[104][satir]));//ç
  
  else if (font < 126) return pgm_read_byte(&(font8x8[font][satir]));

}

// ASCII karakter kodu yorumlayıcısını font5x7 dizisindeki Türkçe İngilizce Karekterler için
uint8_t Yazi_Fontu_Al2(uint8_t font, uint8_t satir) {

  //uint8_t fontOrg = font;
  //font = font - '0' + 16;   // ASCII tablosundan karakter kodunun dizi numaralandırmasına göre sayıya çevrilmesi
  font = font - 32;
  //Bilgi ekranı
  //Serial.print(satir);
  //Serial.print(" - ");
  //Serial.println(fontOrg);
  //delay(1000);

  //Büyük Tükçe Harfler
  if      (font == 103) return pgm_read_byte(&(font5x7[95][satir]));//Ç
  else if (font == 144) return pgm_read_byte(&(font5x7[96][satir]));//İ
  else if (font == 124) return pgm_read_byte(&(font5x7[97][satir]));//Ü
  else if (font == 118) return pgm_read_byte(&(font5x7[98][satir]));//Ö
  else if (font == 126) return pgm_read_byte(&(font5x7[99][satir]));//Ş

  //Küçük Türkçe Hafler
  else if (font == 145) return pgm_read_byte(&(font5x7[100][satir]));//ı
  else if (font == 156) return pgm_read_byte(&(font5x7[101][satir]));//ü
  else if (font == 150) return pgm_read_byte(&(font5x7[102][satir]));//ö
  else if (font == 127) return pgm_read_byte(&(font5x7[103][satir]));//ş
  else if (font == 135) return pgm_read_byte(&(font5x7[104][satir]));//ç
 
  else if (font < 126) return pgm_read_byte(&(font5x7[font][satir]));// ingilizce harfler ve semboller için 

}
