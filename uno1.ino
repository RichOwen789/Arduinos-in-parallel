byte dataSize;
byte numOnes;
byte numWaves;
byte data[6];
byte bestWaves[10];


void receiveData(){
  //info
  byte info[3];
  boolean bytes_read = false;
  while(!bytes_read){
    if(Serial.available()){
      Serial.readBytes(info, 3);
      bytes_read = true;
    }
  }
  dataSize = info[0];
  numOnes = info[1];
  numWaves = info[2];

  //data
  byte data[dataSize];
  boolean data_read = false;
  while(!data_read){
    if(Serial.available()){
      Serial.readBytes(data, dataSize);
      data_read = true;
    }
  }
}


void setup() {
  Serial.begin(9600);
  receiveData();
  
}


boolean compareWaves(byte theseWaves[], byte thoseWaves[]) {
  //setup data
  byte theseData[dataSize];
  byte thoseData[dataSize];
  for (int i = 0; i < dataSize; i ++) {
    theseData[i] = 0;
    thoseData[i] = 0;
  }
  //crash the waves
  for (int i = 0; i < numWaves; i++) {
    if (theseWaves[i] > 0 && theseWaves[i] < dataSize+1) {
      int j = -1;
      do {
        j += theseWaves[i];
        theseData[j] ++;
      } while (j < dataSize);
    }
    if (thoseWaves[i] > 0 && thoseWaves[i] < dataSize+1) {
      int j = -1;
      do {
        j += thoseWaves[i];
        thoseData[j] ++;
      } while (j < dataSize);
    }
  }
  //revive 1's and 0's
  int theseOnes = 0;
  int thoseOnes = 0;
  byte maxValue = numWaves + 1; //waves cannot make data >= than this
  //these
  while (theseOnes < numOnes){
    int bigIndex = 0;
    for(int i = 0; i < dataSize; i++){
      if(theseData[i] >= theseData[bigIndex] && theseData[i] != maxValue){
        bigIndex = i;
      }
    }
    theseData[bigIndex] = maxValue; 
    theseOnes ++;
  }
  for(int i = 0; i < dataSize; i++){
    theseData[i] == maxValue ? theseData[i] = 1 : theseData[i] = 0;
  }
  //those
  while (thoseOnes < numOnes){
    byte bigIndex = 0;
    for(int i = 0; i < dataSize; i++){
      if(thoseData[i] >= thoseData[bigIndex] && thoseData[i] != maxValue){
        bigIndex = i;
      }
    }
    thoseData[bigIndex] = maxValue; 
    thoseOnes ++;
  }
  for(int i = 0; i < dataSize; i++){
    thoseData[i] == maxValue ? thoseData[i] = 1 : thoseData[i] = 0;
  }

  // return theseData are better than thoseData
  byte diff = 0;
  for(int i = 0; i < dataSize; i++) {
    if(thoseData[i] == data[i] && theseData[i] == data[i]) {
      diff += 0;
    }
    if(thoseData[i] == data[i] && theseData[i] != data[i]) {
      diff += -1;
    }
    if(thoseData[i] != data[i] && theseData[i] == data[i]) {
      diff += 1;
    }
  }
  return diff >= 0;
}


void findNewBest(){

  //change waves once
  byte changes = 0;
  byte theseWaves[numWaves];
  for(byte i = 0; i < numWaves; i ++){
    theseWaves[i] = bestWaves[i];
  }
  while(changes < 20){
    
    byte changeIndex = random(0, dataSize);
    byte changeLambda = random(0,2);
    changeLambda == 0 && theseWaves[changeIndex] > 0 ? theseWaves[changeIndex] -- : theseWaves[changeIndex] ++;
    boolean change = compareWaves(theseWaves, bestWaves);
    if(change){
      bestWaves[changeIndex] = theseWaves[changeIndex];
      changes ++;
    }
    for(int i = 0; i < numWaves; i ++){
      theseWaves[i] = bestWaves[i];
    }
  }
}


void loop() {
  findNewBest();
  if(Serial.available()){
    byte sig = Serial.read();
    if(sig == 9){
      Serial.write(bestWaves, numWaves);
    }
  }
}













